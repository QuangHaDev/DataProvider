#include "stdafx.h"
#include "RIOSocket.h"
//#include <sdkddkver.h>
#include <MSWSock.h>
#include <limits>
#include <algorithm>
#include "applmanclient.h"

#ifndef _MSC_PLATFORM_TOOLSET_v120_xp
RIO_EXTENSION_FUNCTION_TABLE rio;
GUID functionTableId = WSAID_MULTIPLE_RIO;
static const DWORD RIO_PENDING_RECVS = 256;
static const DWORD RIO_PENDING_SENDS = 16;
static const DWORD RIO_RECVS_BUFFER_SIZE = 32768;
static const DWORD RIO_SEND_BUFFER_SIZE = 32768;
static const DWORD RIO_MAX_RESULTS = 128;
static unsigned long sULPkt = 0;
static unsigned long sULPktOut = 0;

RIOSocket::RIOSocket() :m_bReady(false), m_completionQueue(0), m_requestQueue(0), m_hIOCP(INVALID_HANDLE_VALUE), bRIO(true)
{
	m_pDPL = nullptr;
	m_Socket = INVALID_SOCKET;
	bool bOk = true;
	WSADATA wsadata;
	if (0 != WSAStartup(0x202, &wsadata))
	{
		TRACE("WSAStartup failed at RIOSocket Constructor");
		bOk = false; // WSAStartup failed.
	}
	m_bReady = bOk;
	//hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	InitializeCriticalSection(&m_cs);
	InitializeCriticalSection(&m_csRIO);
	InitializeConditionVariable(&m_cvAdded);
}

RIOSocket::~RIOSocket()
{
	Stop();
	WSACleanup();
	//CloseHandle(hEvent);
	DeleteCriticalSection(&m_cs);
	DeleteCriticalSection(&m_csRIO);
}

int RIOSocket::Start(int nPort, const char * pIFAddr)
{
	if (!m_bReady)
		return MC_ERROR;

	EnterCriticalSection(&m_csRIO);

	if (m_Socket != INVALID_SOCKET)
	{
		closesocket(m_Socket);
	}

	m_Socket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_REGISTERED_IO);
	//m_Socket = INVALID_SOCKET;
	if (m_Socket == INVALID_SOCKET)
	{
		bRIO = false;
		CApplManClient::Log(LOG_NORMAL, "[CMast] RIO is not in use");

		m_Socket = WSASocket(AF_INET, SOCK_DGRAM, 0, NULL, 0, WSA_FLAG_MULTIPOINT_C_LEAF
			| WSA_FLAG_MULTIPOINT_D_LEAF
			| WSA_FLAG_OVERLAPPED);

		if (m_Socket == INVALID_SOCKET){

			const DWORD gle = ::GetLastError();
			CApplManClient::Log(LOG_NORMAL, "[RIO] WSASocket Error : %d", gle);
//			TRACE("WSASocket Error: %d", gle);
			LeaveCriticalSection(&m_csRIO);
			return MC_ERROR;
		}
	}

	/// port binding
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(nPort);

	if (pIFAddr == NULL)
		addr.sin_addr.s_addr = INADDR_ANY;
	else
		addr.sin_addr.s_addr = inet_addr(pIFAddr);

	if (true) // force allow rebind
	{
		int x = 1;
		setsockopt(m_Socket, SOL_SOCKET, SO_REUSEADDR, (char *)&x, sizeof(x));
	}

	if (SOCKET_ERROR == ::bind(m_Socket, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)))
	{
		const DWORD gle = ::GetLastError();
		CApplManClient::Log(LOG_NORMAL, "[RIO] WSASocket Error bind: %d", gle);
		//m_bReady = false;
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
		LeaveCriticalSection(&m_csRIO);
		return MC_ERROR;
	}

	SetTTL(); // default to 30 node

//	bSendInUse = false;
	GUID functionTableId = WSAID_MULTIPLE_RIO;
	DWORD dwBytes = 0;

	if (bRIO) {
		if (NULL != WSAIoctl(m_Socket, SIO_GET_MULTIPLE_EXTENSION_FUNCTION_POINTER, &functionTableId, sizeof(GUID),
			(void**)&rio, sizeof(rio), &dwBytes, NULL, NULL))
		{
			bRIO = false;
			CApplManClient::Log(LOG_NORMAL, "[CMast] RIO is not in use");
		}
	}
	if (bRIO) {
		CApplManClient::Log(LOG_NORMAL, "[CMast] Using RIO");
		// create IOCP and bind to RIO
		m_hIOCP = CreateIOCP();
		if (!BindtoIOCP(m_hIOCP)){
			LeaveCriticalSection(&m_csRIO);
			return MC_ERROR;
		}

		CreateSendBuffer(RIO_SEND_BUFFER_SIZE, RIO_PENDING_SENDS);
		// allocated RIO recvs buffer and add pending recvs.
		PostRIORecvs(RIO_RECVS_BUFFER_SIZE, RIO_PENDING_RECVS);
		//create IOCP thread
		for (int i = 0; i < g_nthreads; i++)
		{
			unsigned int notUsed;
			const uintptr_t result = ::_beginthreadex(0, 0, IOThread, (void*)this, 0, &notUsed);
			if (result == 0)
			{
				//m_bReady = false;
				LeaveCriticalSection(&m_csRIO);
				return MC_ERROR;
			}
			m_threads[i] = reinterpret_cast<HANDLE>(result);
		}
		// notify io completion.
		rio.RIONotify(m_completionQueue);
	}

	LeaveCriticalSection(&m_csRIO);
	return MC_SUCCESS;
}


void RIOSocket::Stop()
{
	if (m_Socket == INVALID_SOCKET)
		return;

	if (bRIO) {
		EnterCriticalSection(&m_csRIO);
		for (int i = 0; i < g_nthreads; i++)
			if (0 == ::PostQueuedCompletionStatus(m_hIOCP, 0, CK_STOP, 0)){}

		for (int i = 0; i < g_nthreads; i++)
			if (WAIT_OBJECT_0 != ::WaitForSingleObject(m_threads[i], INFINITE))	{}

		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
		rio.RIOCloseCompletionQueue(m_completionQueue);
		rio.RIODeregisterBuffer(m_RioRecvID); //deregister RIORegisterBuffer returned id;

		VirtualFree(RecvBuffer, 0, MEM_RELEASE);
		VirtualFree(AddrBuffer, 0, MEM_RELEASE);
		VirtualFree(SendBuffer, 0, MEM_RELEASE);
		delete[] pBufs;
		delete[] pAddrBufs;
		delete[] pSBufs;
		LeaveCriticalSection(&m_csRIO);
	}
	else
		CMCast::Stop();
	//return true;
}

HANDLE RIOSocket::CreateIOCP()
{
	return CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
}

bool RIOSocket::BindtoIOCP(HANDLE h)
{
	OVERLAPPED overlapped;
	RIO_NOTIFICATION_COMPLETION type;
	type.Type = RIO_IOCP_COMPLETION;
	type.Iocp.IocpHandle = h;
	type.Iocp.CompletionKey = (void*)CK_START;
	type.Iocp.Overlapped = &overlapped;

	m_completionQueue = rio.RIOCreateCompletionQueue(RIO_PENDING_RECVS + RIO_PENDING_SENDS, &type);
	if (m_completionQueue == RIO_INVALID_CQ)
	{
		const DWORD gle = ::GetLastError();
		CApplManClient::Log(LOG_NORMAL, "[RIO] WSASocket Error RIO_INVALID_CQ : %d", gle);
		return false;
		//m_bReady = false; 
	}

	m_requestQueue = rio.RIOCreateRequestQueue(m_Socket, RIO_PENDING_RECVS, 1, RIO_PENDING_SENDS, 1, m_completionQueue, m_completionQueue, NULL);
	if (m_requestQueue == RIO_INVALID_RQ)
	{
		const DWORD gle = ::GetLastError();
		CApplManClient::Log(LOG_NORMAL, "[RIO] WSASocket Error RIO_INVALID_RQ : %d", gle);
		return false;
		//m_bReady = false;
	}


	return true;
}

template <typename TV, typename TM>
inline TV RoundDown(TV Value, TM Multiple)
{
	return((Value / Multiple) * Multiple);
}

template <typename TV, typename TM>
inline TV RoundUp(TV Value, TM Multiple)
{
	return(RoundDown(Value, Multiple) + (((Value % Multiple) > 0) ? Multiple : 0));
}

// allocate buff to fit the granularity of the system
char *RIOSocket::AllocateBufferSpace(const DWORD bufSize, const DWORD bufCount, DWORD& totalBufferSize, DWORD& totalBufferCount)
{
	SYSTEM_INFO systemInfo;

	::GetSystemInfo(&systemInfo);

	const unsigned __int64 granularity = systemInfo.dwAllocationGranularity;

	const unsigned __int64 desiredSize = bufSize * bufCount;

	unsigned __int64 actualSize = RoundUp(desiredSize, granularity);

	if (actualSize > (std::numeric_limits<DWORD>::max)())
	{
		actualSize = ((std::numeric_limits<DWORD>::max)() / granularity) * granularity;
	}

	totalBufferCount = std::min<DWORD>(bufCount, static_cast<DWORD>(actualSize / bufSize));

	totalBufferSize = static_cast<DWORD>(actualSize);

	char *pBuffer = reinterpret_cast<char *>(VirtualAllocEx(GetCurrentProcess(), 0, totalBufferSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));

	if (pBuffer == 0)
	{
		const DWORD gle = ::GetLastError();
		TRACE("VirtualAllocEx Error: %d", gle);
	}

	return pBuffer;
}

inline void RIOSocket::CreateSendBuffer(const DWORD sendBufferSize, const DWORD pendingSends)
{
	DWORD totalBufferCount = 0;
	DWORD totalBufferSize = 0;
	SendAddrBuffer = AllocateBufferSpace(
		sizeof(SOCKADDR_INET), RIO_PENDING_SENDS, totalBufferSize, totalBufferCount);
	m_RioAddrSendID = rio.RIORegisterBuffer(SendAddrBuffer, static_cast<DWORD>(totalBufferSize));
	if (m_RioAddrSendID == RIO_INVALID_BUFFERID)
	{
		TRACE("RIO Invalid ADDR BufferID");
	}


	SendBuffer = AllocateBufferSpace(RIO_SEND_BUFFER_SIZE, RIO_PENDING_SENDS, totalBufferSize, totalBufferCount);
	m_RioSendID = rio.RIORegisterBuffer(SendBuffer, static_cast<DWORD>(totalBufferSize));

	if (m_RioSendID == RIO_INVALID_BUFFERID)
	{
		TRACE("RIO Invalid Send BufferID");
	}

	//DWORD offset = 0;

	pSBufs = new EXTENDED_RIO_BUF[totalBufferCount];
	pAddrSBufs = new RIO_BUF[totalBufferCount];

	DWORD SOffset = 0;
	DWORD AOffset = 0;

	for (DWORD i = 0; i < totalBufferCount; ++i)
	{
		/// RIO operation
		EXTENDED_RIO_BUF* pBuffer = pSBufs +i;

		pBuffer->operation = OP_SEND;
		pBuffer->BufferId = m_RioSendID;
		pBuffer->Offset = SOffset;
		pBuffer->Length = RIO_SEND_BUFFER_SIZE;
		pBuffer->addr_idx = i;
		pBuffer->addr_offset = AOffset;
		SOffset += RIO_SEND_BUFFER_SIZE;

		RIO_BUF* pAddr = pAddrSBufs + i;
		pAddr->BufferId = m_RioAddrSendID;
		pAddr->Offset = AOffset;
		pAddr->Length = sizeof(SOCKADDR_INET);
		AOffset += sizeof(SOCKADDR_INET);
		//offset += RIO_SEND_BUFFER_SIZE;
		m_sendRIOBUF.push_back(i);
	}
}


inline void RIOSocket::PostRIORecvs(const DWORD recvBufferSize,	const DWORD pendingRecvs)
{
	DWORD totalBuffersAllocated = 0;

	//while (totalBuffersAllocated < pendingRecvs)
	//{
		DWORD bufferSize = 0;

		DWORD receiveBuffersAllocated = 0;
		DWORD receiveAddrBuffersAllocated = 0;

		AddrBuffer = AllocateBufferSpace(
			sizeof(SOCKADDR_INET), pendingRecvs, bufferSize, receiveAddrBuffersAllocated);

		m_RioAddrID = rio.RIORegisterBuffer(AddrBuffer, static_cast<DWORD>(bufferSize));
		if (m_RioAddrID == RIO_INVALID_BUFFERID)
		{
			TRACE("RIO Invalid ADDR BufferID");
		}
		RecvBuffer = AllocateBufferSpace(
			recvBufferSize,
			pendingRecvs,
			bufferSize,
			receiveBuffersAllocated); 

		totalBuffersAllocated += receiveBuffersAllocated;

		m_RioRecvID = rio.RIORegisterBuffer(RecvBuffer, static_cast<DWORD>(bufferSize));

		if (m_RioRecvID == RIO_INVALID_BUFFERID)
		{
			TRACE("RIO Invalid RECV BufferID");
		}

		if (receiveAddrBuffersAllocated != receiveAddrBuffersAllocated){
			TRACE("RIO buffer allocated size not match");
		}

		DWORD offset = 0 , offsetA = 0;

		const DWORD recvFlags = 0;

		pBufs = new EXTENDED_RIO_BUF[receiveBuffersAllocated];
		pAddrBufs = new RIO_BUF[receiveAddrBuffersAllocated];

		for (DWORD i = 0; i < receiveBuffersAllocated; ++i)
		{
			// now split into buffer slices and post our recvs

			EXTENDED_RIO_BUF *pBuffer = pBufs + i;
			RIO_BUF * pAddr = pAddrBufs + i;

			pBuffer->operation = OP_RECV;
			pBuffer->addr_offset = offsetA;
			pBuffer->addr_idx = i;
			pBuffer->BufferId = m_RioRecvID;
			pBuffer->Offset = offset;
			pBuffer->Length = recvBufferSize;

			pAddr->BufferId = m_RioAddrID;
			pAddr->Offset = offsetA;
			pAddr->Length = sizeof(SOCKADDR_INET);

			offset += recvBufferSize;
			offsetA += sizeof(SOCKADDR_INET);

			//if (!rio.RIOReceive(m_requestQueue, pBuffer, 1, recvFlags, pBuffer))
			if( !rio.RIOReceiveEx(m_requestQueue,pBuffer,1,NULL,pAddr,NULL,0,0,pBuffer))
			{
				const DWORD gle = ::GetLastError();
				TRACE("WSASocket Error: %d", gle);
				m_bReady = false;
			}
		}

		
	//}
}


unsigned int __stdcall RIOSocket::IOThread(void *pV)
{
	RIOSocket * rioS = reinterpret_cast<RIOSocket*>(pV);
	DWORD numberOfBytes = 0;

	ULONG_PTR completionKey = 0;
	OVERLAPPED* pOverlapped = 0;

	RIORESULT results[RIO_MAX_RESULTS];

	while (true) {

		if (!::GetQueuedCompletionStatus(rioS->m_hIOCP, &numberOfBytes, &completionKey, &pOverlapped, INFINITE))
		{
			const DWORD gle = ::GetLastError();
			return false;
		}

		if (completionKey == CK_STOP) // stop call from the main thread
			return false;

		//memset(results, 0, sizeof(results));
		//EnterCriticalSection(&rioS->m_csRIO);
		ULONG numResults = rio.RIODequeueCompletion(rioS->m_completionQueue, results, RIO_MAX_RESULTS);
		if (0 == numResults || RIO_CORRUPT_CQ == numResults)
		{
		//	LeaveCriticalSection(&rioS->m_csRIO);
			return false;
		}
		INT notifyResult = rio.RIONotify(rioS->m_completionQueue);
		if (notifyResult != ERROR_SUCCESS)
		{
		//	LeaveCriticalSection(&rioS->m_csRIO);
			return false;
		}
		//LeaveCriticalSection(&rioS->m_csRIO);

		DataPackList *  dpList = new DataPackList;
		bool bAdded = false;
		for (DWORD i = 0; i < numResults; ++i)
		{
			EXTENDED_RIO_BUF *pBuffer = reinterpret_cast<EXTENDED_RIO_BUF*>(results[i].RequestContext);

			switch (pBuffer->operation)
			{
			case OP_RECV:
			
				/*if (results[i].BytesTransferred != RIO_RECVS_BUFFER_SIZE)
					break;*/
				if (results[i].BytesTransferred > 0) {
					DataPackage data;
					data.buffer = new char[results[i].BytesTransferred];
					data.len = results[i].BytesTransferred;
					memcpy_s(data.buffer, results[i].BytesTransferred, rioS->RecvBuffer + pBuffer->Offset, results[i].BytesTransferred);

					memcpy_s(&(data.addr), sizeof(SOCKADDR_IN),&(( SOCKADDR_INET*)(rioS->AddrBuffer + pBuffer->addr_offset))->Ipv4, sizeof(SOCKADDR_IN));
					dpList->push_back(data);
					bAdded = true;
					//EnterCriticalSection(&rioS->m_cs);
					//rioS->m_buffer.AddTail(data);
					//LeaveCriticalSection(&rioS->m_cs);
					sULPkt++;
					DWORD recvFlags = 0;
					RIO_BUF * pAddr = rioS->pAddrBufs + pBuffer->addr_idx;
					//if (!rio.RIOReceive(rioS->m_requestQueue, pBuffer, 1, recvFlags, pBuffer))
					EnterCriticalSection(&rioS->m_csRIO);
					if (!rio.RIOReceiveEx(rioS->m_requestQueue, pBuffer, 1, NULL, pAddr, NULL, 0, 0, pBuffer))
					{
						const DWORD gle = ::GetLastError();
						TRACE("RIOReceive Error: %d", gle);
						LeaveCriticalSection(&rioS->m_csRIO);
//						LeaveCriticalSection(&rioS->m_cs);
						return false;
					}	
					LeaveCriticalSection(&rioS->m_csRIO);
				}
				break;
			case OP_SEND:
 				rioS->FlushRIOSend(pBuffer);
				break;
			default: //ignore other operation
				break;			
			}
			
		}		

		if (bAdded)
		{
			EnterCriticalSection(&rioS->m_cs);
			rioS->m_buffer.push_back(dpList);
			LeaveCriticalSection(&rioS->m_cs);
			WakeConditionVariable(&rioS->m_cvAdded);
			//SetEvent(rioS->hEvent);
		}
	}

}

int RIOSocket::SetTTL(int nTTL)
{
	if (m_bReady && m_Socket!=INVALID_SOCKET && setsockopt(m_Socket, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&nTTL, sizeof(nTTL)) == SOCKET_ERROR)
	{
		return MC_SUCCESS;
	}
	return MC_ERROR;
}
//
//int RIOSocket::AddGroup(const char *pGrpAddr, const char *pIFAddr)
//{
//	if (!IsReady()) return false;
//
//	struct ip_mreq mreq;
//	mreq.imr_multiaddr.s_addr = inet_addr(pGrpAddr);
//	if (pIFAddr == NULL)
//		mreq.imr_interface.s_addr = INADDR_ANY;
//	else
//		mreq.imr_interface.s_addr = inet_addr(pIFAddr);
//
//	if (setsockopt(m_Socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq, sizeof(mreq)) == -1)
//	{
//		int m_nStatus = WSAGetLastError();
//		TRACE("Add Group Failed: %d\n", m_nStatus);
//		return MC_ERROR;
//	}
//	return MC_SUCCESS;
//}
//
//int RIOSocket::DelGroup(const char *pGrpAddr, const char *pIFAddr)
//{
//	if (!IsReady())	return false;
//
//	struct ip_mreq mreq;
//	mreq.imr_multiaddr.s_addr = inet_addr(pGrpAddr);
//	if (pIFAddr == NULL)
//		mreq.imr_interface.s_addr = INADDR_ANY;
//	else
//		mreq.imr_interface.s_addr = inet_addr(pIFAddr);
//	if (setsockopt(m_Socket, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *)&mreq, sizeof(mreq)) == -1)
//	{
//		int m_nStatus = WSAGetLastError();
//		TRACE("Delete Group Failed: %d\n", m_nStatus);
//		return MC_ERROR;
//	}
//
//	return MC_SUCCESS;
//}

int RIOSocket::SelectRead(long lMsec, long Sec)
{
	if (!bRIO) {
		return CMCast::SelectRead(lMsec, Sec);
	}
	else {
		BOOL rtn = false;
		EnterCriticalSection(&m_cs);

		
		if ((m_pDPL && !m_pDPL->empty()) || !m_buffer.empty()) {
			LeaveCriticalSection(&m_cs);
			return MC_READ;
		}

		rtn  = SleepConditionVariableCS(&m_cvAdded, &m_cs, Sec * 1000 + lMsec);
		LeaveCriticalSection(&m_cs);



		//if (WAIT_TIMEOUT == WaitForSingleObject(hEvent, Sec * 1000 + lMsec))
		if ( !rtn)
			return MC_SUCCESS;// MC_SUCCESS;
		else
			return MC_READ;// MC_READ;
	}
	return MC_SUCCESS;
}

//static int prevCount = 0;
int RIOSocket::Recv(char *pRecvBuf, int nBufSize, char *pRemote, int &nByteRecv)
{//pRemote is not touched
	if (!bRIO)
		return CMCast::Recv(pRecvBuf, nBufSize, pRemote, nByteRecv);

	DataPackage data;
	bool bHasData = false;

	EnterCriticalSection(&m_cs);
	if (m_pDPL == nullptr) {
		if (!m_buffer.empty()){
			m_pDPL = m_buffer.front();
			m_buffer.pop_front();
		}
	}
	if ( m_pDPL ) {
		if (!m_pDPL->empty()) {
			data = m_pDPL->front();

			if (nBufSize >= data.len){
				bHasData = true;
				m_pDPL->pop_front();
				sULPktOut++;
			}
			/*else {
				m_pDPL->AddHead(data);
			}*/
		}  else {
			delete m_pDPL;
			m_pDPL = nullptr;
		}

	}
	LeaveCriticalSection(&m_cs);
			

	if (!bHasData)
		return MC_ERROR;

	nByteRecv = data.len;
	if (nBufSize >= nByteRecv) {
		memcpy(pRecvBuf, data.buffer, nByteRecv);
		if (pRemote != NULL)
			strcpy(pRemote, inet_ntoa(data.addr.sin_addr));

		delete [] data.buffer;
	}
	
	return MC_SUCCESS;
}


int RIOSocket::Send(const char *pGrpAddr, int nPort, const char *pMsg, int nMsgLen)
{
	if (!bRIO)
		return CMCast::Send(pGrpAddr, nPort, pMsg, nMsgLen);

	EnterCriticalSection(&m_cs);

	if (m_sendRIOBUF.empty()) {
		SendMsgPack smp;
		smp.pGrpAddr = (char*) pGrpAddr;
		smp.nPort = nPort;
		smp.pMsg = (char*)pMsg;
		smp.nMsgLen = nMsgLen;
		m_sendBuffer.push_back(smp);
	}
	else {

		ULONG i = m_sendRIOBUF.front();
		m_sendRIOBUF.pop_front();
		EXTENDED_RIO_BUF* sendBuf = &(pSBufs[i]);
		int j = sendBuf->addr_idx;
		RIO_BUF* pAddr = &(pAddrSBufs[j]);

		SOCKADDR_INET* pA = (SOCKADDR_INET*)(SendAddrBuffer+sendBuf->addr_offset);

		pA->Ipv4.sin_addr.S_un.S_addr = inet_addr(pGrpAddr);
		pA->Ipv4.sin_port = htons(nPort);
		pA->Ipv4.sin_family = AF_INET;
		pA->si_family = AF_INET;

		memcpy_s(SendBuffer+sendBuf->Offset, RIO_SEND_BUFFER_SIZE, pMsg, nMsgLen);
		sendBuf->Length = nMsgLen;
		if (!rio.RIOSendEx(m_requestQueue, sendBuf, 1, NULL, pAddr, NULL, NULL, 0, sendBuf))
		{
			const DWORD gle = ::GetLastError();
			TRACE("Error:%d", gle);
		}
//		bSendInUse = true;
	}
	LeaveCriticalSection(&m_cs);
	
	return MC_SUCCESS;	
}

int RIOSocket::FlushRIOSend(EXTENDED_RIO_BUF* pBuf)
{
	EnterCriticalSection(&m_cs);
	if (!m_sendBuffer.empty())
	{
		SendMsgPack smp = m_sendBuffer.front();
		m_sendBuffer.pop_front();
		EXTENDED_RIO_BUF* sendBuf = pBuf;
		int j = sendBuf->addr_idx;

		RIO_BUF* pAddr = &(pAddrSBufs[j]);


		SOCKADDR_INET* pA = (SOCKADDR_INET*)(SendAddrBuffer + sendBuf->addr_offset);

		pA->Ipv4.sin_addr.S_un.S_addr = inet_addr(smp.pGrpAddr);
		pA->Ipv4.sin_port = htons(smp.nPort);
		pA->Ipv4.sin_family = AF_INET;
		pA->si_family = AF_INET;
		memcpy_s(SendBuffer + sendBuf->Offset, RIO_SEND_BUFFER_SIZE, smp.pMsg, smp.nMsgLen);
		sendBuf->Length = smp.nMsgLen;
		rio.RIOSendEx(m_requestQueue, sendBuf, 1, NULL, pAddr, NULL, NULL, 0, sendBuf);
	}
	else
	{
		ULONG i = pBuf->addr_idx;
		m_sendRIOBUF.push_front(i);

	}
	LeaveCriticalSection(&m_cs);
	return 0;
}

#endif