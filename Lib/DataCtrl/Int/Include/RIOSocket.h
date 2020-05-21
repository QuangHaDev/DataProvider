#pragma once
#ifndef RIOS_H_     // equivalently, #if !defined HEADER_H_
#define RIOS_H_

#ifndef _MSC_PLATFORM_TOOLSET_v120_xp
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Mswsock.h>
#pragma comment(lib, "ws2_32.lib")
#include <afxtempl.h>
#include "Mcast.h"
#include <deque>
#include <memory>

#define g_nthreads 1 // more than 1 thread will cause the msg seq out sync

using namespace std;

class RIOSocket : public CMCast
{
public:

	enum RIO_COMPLETION_KEY
	{
		CK_STOP = 0,
		CK_START = 1
	};

	enum RIO_OPERATION_TYPE
	{
		OP_NONE = 0,
		OP_RECV = 1,
		OP_SEND = 2
	};

	struct EXTENDED_RIO_BUF : public RIO_BUF
	{
		RIO_OPERATION_TYPE operation;
		DWORD addr_offset;
		DWORD addr_idx;
	};

	typedef struct DataPackage
	{
		char * buffer;
		SOCKADDR_IN addr;
		int len;
	} DataPack;

	typedef struct SendMsgPackage
	{
		char *pGrpAddr;
		int nPort;
		char *pMsg;
		int nMsgLen;
	} SendMsgPack;

	typedef deque<DataPack> DataPackList;

public:
	RIOSocket();
	~RIOSocket();
	
	virtual int Start(int nPort, const char * pIFAddr);
	virtual void Stop();
	virtual int SetTTL(int nTTL = 30);
	// Join the multicast group from which to receive datagrams.
	//virtual int AddGroup(const char *pGrpAddr, const char *pIFAddr = NULL);
	//// Leave the multicast group from which to receive datagrams.
	//virtual int DelGroup(const char *pGrpAddr, const char *pIFAddr = NULL);
	virtual int SelectRead(long lMsec, long Sec);
	virtual int Recv(char *pRecvBuf, int nBufSize, char *pRemote, int &nByteRecv);
	virtual int Send(const char *pGrpAddr, int nPort, const char *pMsg, int nMsgLen);

	int FlushRIOSend(EXTENDED_RIO_BUF * pBuf);

	bool IsReady(){ return m_bReady && m_Socket!=INVALID_SOCKET; };
	bool isRIO() { return bRIO; };
	CRITICAL_SECTION m_cs;
	CRITICAL_SECTION m_csRIO;
	//HANDLE hEvent;
	//CList<DataPack,DataPack&> m_buffer;
	deque<DataPackList*> m_buffer;
	deque<SendMsgPack> m_sendBuffer;
	deque<ULONG> m_sendRIOBUF;
	DataPackList * m_pDPL;
private:
	bool bRIO;
	//bool bSendInUse;
	char * AllocateBufferSpace(const DWORD bufSize, const DWORD bufCount, DWORD& totalBufferSize, DWORD& totalBufferCount);
	void PostRIORecvs(const DWORD recvBufferSize,const DWORD pendingRecvs);
	void CreateSendBuffer(const DWORD sendBufferSize, const DWORD pendingSends);
	static unsigned int __stdcall IOThread(void *pV);
	HANDLE CreateIOCP();
	bool BindtoIOCP(HANDLE h);

	char * RecvBuffer;
	char * AddrBuffer;
	char * SendBuffer;
	char * SendAddrBuffer;

	bool m_bReady;
	RIO_CQ m_completionQueue;
	RIO_RQ m_requestQueue;
	HANDLE m_hIOCP;
	RIO_BUFFERID m_RioRecvID, m_RioAddrID, m_RioSendID, m_RioAddrSendID;
	EXTENDED_RIO_BUF *pBufs;
	EXTENDED_RIO_BUF *pSBufs;
	RIO_BUF *pAddrBufs;
	RIO_BUF *pAddrSBufs;
	HANDLE m_threads[g_nthreads];
	CONDITION_VARIABLE m_cvAdded;

};

#endif
#endif