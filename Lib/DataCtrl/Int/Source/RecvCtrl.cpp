/////////////////////////////////////////////////////////////////////////////
//   File       :: RecvCtrl.cpp
//   Author     :: John Lee
//   Date       :: 1 September 2000
//   Version    :: 1.00
// 
//
//   Description:: Implementation of the CRecvCtrl class.
//                            
//
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000 by AFE Solutions Ltd.
// All rights reserved.
//
// No portion of this software in any form may be used or reproduced in 
// any manner without written consent from AFE Solutions Ltd.
//
//
#define _CRT_SECURE_NO_WARNINGS			1

#include "RecvCtrl.h"
#include "ApplManClient.h"
#include <process.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <memory>
using namespace std;

CRecvDetail::CRecvDetail(unsigned short usDPID, byte byDPIns, bool bSeqLogEnable, int nRecoveryWindowSize) :
m_recovery(bSeqLogEnable, nRecoveryWindowSize),
m_bRecoveryEnable(true)
{
    m_usDPID = usDPID;
    m_byDPIns = byDPIns;
    m_bySeq = 0;
    m_ulDiff = 0;
}

bool CRecvDetail::CheckDP(unsigned short usDPID, byte byDPIns)
{
    if ((m_usDPID == usDPID) &&
        (m_byDPIns == byDPIns))
        return true;
    else
        return false;
}

bool CRecvDetail::CheckSeq(byte bySeq)
{
    bool bRc = true;
    if (m_bySeq != 0)
    {
        if (m_bySeq == 255)
        {
            if (bySeq != 0)
            {
                m_ulDiff += bySeq;
                bRc = false;
            }
        }
        else if (m_bySeq+1 != bySeq)
        {
            if (bySeq > m_bySeq)
                m_ulDiff += (bySeq - m_bySeq);
            else
                m_ulDiff += (255 - m_bySeq + bySeq);
            bRc = false;
        }
    }
    m_bySeq = bySeq;
    return bRc;
}

// class CRecvCtrl

// Thread Priority Changes by Terence - Start
CRecvCtrl::CRecvCtrl(CMCast *service, bool bLogEnable, bool bSeqLogEnable, bool bBufferEnable, int iThreadPriority, int nRecoveryWindowSize) :
// Thread Priority Changes by Terence - End
    m_pService(service), 
    m_bExitThread(FALSE), 
    m_bCheckAlive(FALSE), 
    m_bLogEnable(bLogEnable),
    m_bSeqLogEnable(bSeqLogEnable),
    m_bBufferEnable(bBufferEnable),
    m_iThreadPriority(iThreadPriority),
    m_nRecoveryWindowSize(nRecoveryWindowSize)
// Thread Priority Changes by Terence - End
//    m_pCallbackData(NULL),
//    m_callback(NULL)
{
//Terence 20020314
    HKEY hKey;
    DWORD dwLogEnable = 0;
    m_bPacketLogEnable = false;
    DWORD dwSize = sizeof(dwLogEnable);
    if ( RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                 "SOFTWARE\\AFE\\DataCtrl",
                 0,
                 KEY_QUERY_VALUE,
                 &hKey) == ERROR_SUCCESS )
    {
        if ( RegQueryValueEx( hKey, 
                              "PacketLogEnable", 
                              NULL, 
                              NULL, 
                              (LPBYTE) &dwLogEnable, 
                              &dwSize) == ERROR_SUCCESS )
        {
            if(dwLogEnable == 1) 
                m_bPacketLogEnable = true;
            else
                m_bPacketLogEnable = false;
        }
        RegCloseKey(hKey);
    }
//Terence 20020314
    Reset();
    Start();
}

CRecvCtrl::~CRecvCtrl()
{
    Stop();
}

void CRecvCtrl::Main()
{
    char szAddr[16];
    int nByte, nLen;
    int nRet;
    char *p = new char[RECV_BUFFER_SIZE];
    ULONG ulByte = 0;
    unsigned short usMPP = 0;
    unsigned long ulMT = 0, ulPT = 0;

    SYSTEMTIME systime;
    GetLocalTime(&systime);
    if (m_bLogEnable)
    {
        ofstream outFile("datactrl.log", ios::out|ios::app|ios::binary);
        outFile << "<RecvCtrl Log - START - " << systime.wYear << "\\" << systime.wMonth << "\\" << systime.wDay 
                << " " << systime.wHour << ":" << systime.wMinute << ":" << systime.wSecond << ">" ;
        outFile.put(0x0D);
        outFile << endl;
        outFile.close();
    }

    m_ulPacketPerSec = 0;
    m_ulMessagePerSec = 0;
    ssMultiWait MultiWait;
    MultiWait.Add(ssThread::m_CheckAliveEvent, EVENT_CHECKALIVE);
    MultiWait.Add(ssThread::m_ExitEvent, EVENT_EXIT);
//    MultiWait.Add((HANDLE)m_timer, EVENT_TIMER);

// Thread Priority Changes by Terence - Start
    SetPriority(m_iThreadPriority);
/*testtest
    char szText[256];
    sprintf(szText, "Receive Thread ID:%x Priority:%d", GetID(), GetPriority());
    MessageBox(NULL, szText, "Thread Priority", MB_OK);
*/
// Thread Priority Changes by Terence - End
	short nCurDPID = 0;
	byte byCurDPIns = 0;
	bool bMsgDecoded = false;

    while (!m_bExitThread)
    {
        switch(MultiWait.Wait(0))
        {
        case EVENT_CHECKALIVE:
            m_bCheckAlive = FALSE;
            ssThread::AckAlive();
            break;
        case EVENT_EXIT:
            m_bExitThread = TRUE;
            break;
        default:
            // check m_service == null?
//            do
//            {
                nRet = m_pService->SelectRead(500);
				switch (nRet)
				{
				case CMCast::MC_SUCCESS: // timeout
					continue;
				case CMCast::MC_ERROR:
					m_bExitThread = TRUE;
					break;
				case CMCast::MC_READ:
					memset(szAddr, 0, sizeof(szAddr));
					if (m_pService->Recv(p, RECV_BUFFER_SIZE, szAddr, nByte) == CMCast::MC_SUCCESS)
					{
						//continue;
						if (nByte > 0)
						{
							m_iBitCount += (nByte * 8 + 336);
							m_ulByteRecv += nByte;
							m_ulPacketTotal++;
							nLen = nByte;
							bMsgDecoded = false;
							while (nLen > 0 && !m_bExitThread)
							{
								if (Validate((const byte *)(p + nByte - nLen), nLen))
								{
									COP_ITEM pItem = new BUFFER_ITEM((const unsigned char *)(p + (nByte - nLen)));
									nCurDPID = pItem->GetDPID();
									byCurDPIns = pItem->GetDPInsID();
									bMsgDecoded = true;
									if (nLen == nByte)
									{
										if (!RecoveryProcess(pItem, (const byte *)p, nLen))
											break;
									}
									nLen -= pItem()->GetMsgLen();
									// Terence Lai Test Length Start
									int nTestLen = pItem()->GetMsgLen();
									// ** 10Feb2011 **
									MsgTypeValidate(pItem, szAddr);
									// ** 10Feb2011 **
									ProcessCOP(pItem);
									if ((nTestLen != pItem()->GetMsgLen()) && (m_bLogEnable))
									{
										GetLocalTime(&systime);
										ofstream outFile2("datactrl.log", ios::out | ios::app | ios::binary);
										outFile2 << "<Invalid Length " << systime.wYear << "\\" << systime.wMonth << "\\" << systime.wDay
											<< " " << systime.wHour << ":" << systime.wMinute << ":" << systime.wSecond << "-"
											<< nByte << ":" << nLen << "-" << nTestLen << ":" << pItem()->GetMsgLen() << ">";
										outFile2.put(0x0D); outFile2 << endl;
										outFile2.write(p, nByte);
										outFile2.put(0x0D); outFile2 << endl;
										outFile2.put(0x0D); outFile2 << endl;
										outFile2.close();
									}
									// Terence Lai Test Length End
									m_ulMessageTotal++;
									usMPP++;
								}
								else
								{
									if (m_bLogEnable)
									{
										GetLocalTime(&systime);
										ofstream outFile2("datactrl.log", ios::out | ios::app | ios::binary);
										outFile2 << "<RecvCtrl Log " << systime.wYear << "\\" << systime.wMonth << "\\" << systime.wDay
											<< " " << systime.wHour << ":" << systime.wMinute << ":" << systime.wSecond << "-"
											<< nByte << ":" << nLen << ">";
										outFile2.put(0x0D);	outFile2 << endl;
										outFile2.write(p, nByte);
										outFile2.put(0x0D); outFile2 << endl;
										outFile2.put(0x0D);	outFile2 << endl;
										outFile2.close();
									}
									nLen = 0;
								}
							}
							if (usMPP > 0)
								m_usMessagePerPacket = usMPP;
						}


					}


					PRECV_DETAIL_LIST pDetail;
					//for (pDetail = m_RecvDetailList.begin(); pDetail != m_RecvDetailList.end(); pDetail++)
					if (bMsgDecoded && ((pDetail = m_RecvDetailList.find(DPidx(nCurDPID,byCurDPIns))) != m_RecvDetailList.end()))
					{
						while (pDetail->second->m_recovery.GetData((byte *)p, nByte))
						{
							nLen = nByte;
							while (nLen > 0)
							{
								if (Validate((const byte *)(p + nByte - nLen), nLen))
								{
									COP_ITEM pItem = new BUFFER_ITEM((const unsigned char *)(p + (nByte - nLen)));
									nLen -= pItem()->GetMsgLen();
									// Terence Lai Test Length Start
									int nTestLen = pItem()->GetMsgLen();
									// ** 10Feb2011 **
									MsgTypeValidate(pItem, szAddr);
									// ** 10Feb2011 **
									ProcessCOP(pItem);
									if ((nTestLen != pItem()->GetMsgLen()) && (m_bLogEnable))
									{
										GetLocalTime(&systime);
										ofstream outFile2("datactrl.log", ios::out | ios::app | ios::binary);
										outFile2 << "<Invalid Length from window" << systime.wYear << "\\" << systime.wMonth << "\\" << systime.wDay
											<< " " << systime.wHour << ":" << systime.wMinute << ":" << systime.wSecond << "-"
											<< nByte << ":" << nLen << "-" << nTestLen << ":" << pItem()->GetMsgLen() << ">";
										outFile2.put(0x0D); outFile2 << endl;
										outFile2.write(p, nByte);
										outFile2.put(0x0D); outFile2 << endl;
										outFile2.put(0x0D); outFile2 << endl;
										outFile2.close();
									}
									// Terence Lai Test Length End
									m_ulMessageTotal++;
									usMPP++;
								}
								else
								{
									if (m_bLogEnable)
									{
										GetLocalTime(&systime);
										ofstream outFile2("datactrl.log", ios::out | ios::app | ios::binary);
										outFile2 << "<RecvCtrl Log " << systime.wYear << "\\" << systime.wMonth << "\\" << systime.wDay
											<< " " << systime.wHour << ":" << systime.wMinute << ":" << systime.wSecond << "-"
											<< nByte << ":" << nLen << ">";
										outFile2.put(0x0D); outFile2 << endl;
										outFile2.write(p, nByte);
										outFile2.put(0x0D); outFile2 << endl;
										outFile2.put(0x0D); outFile2 << endl;
										outFile2.close();
									}
									nLen = 0;
								}
							}
							if (usMPP > 0)
								m_usMessagePerPacket = usMPP;
						}
					}
				}
               /* if (m_bCheckAlive || m_bExitThread)
                    break;*/
//            } while (nRet == CMCast::MC_READ);
            ProcessDone();
        }
    }
	delete p;
	p = nullptr;
}

bool CRecvCtrl::MsgTypeValidate(COP_ITEM& copItem, char *pAddr)
{
    byte byMsgType = copItem()->GetMsgType();
    if (byMsgType < 1 || byMsgType > 20)
    {
        CApplManClient::Log(LOG_ERROR, "[RecvCtrl] MsgTypeValidate error! Msg:%d  IP:%s", byMsgType, pAddr);
        return false;
    }
    return true;
}

bool CRecvCtrl::Validate(const byte* pData, int nLen)
{
    short nMsgLen;
    if (*pData & 0x01) // 2 bytes
        nMsgLen = *(unsigned short*) pData >> 1;
    else // 1 byte
        nMsgLen = (unsigned short) (*pData >> 1);
    if ((nMsgLen <= 0) || (nMsgLen > nLen))
    {
        CApplManClient::Trace(TRACE_LEVEL0, "CRecvCtrl::Validate failed!");
        return false;
    }
    return true;
}

bool CRecvCtrl::RecoveryProcess(COP_ITEM& copItem, const byte* pData, int nLen)
{
    bool bRc = true;
    PRECV_DETAIL_LIST pDetail;
    if (copItem->GetItemNo() == 0XFE000000)
    {
        short sCurFid = 0;
        short sFidType;
        short sFid;
        sFidType = copItem()->GetFidType(sCurFid, sFid);
        int iRc = CRecovery::SEQ_IN;
        if (sFid == 263)
        {
            ULONG ul;
            copItem()->GetFid(sFid, ul);
            // ****
// Terence 20020314			
            if (m_bPacketLogEnable)
            {
                SYSTEMTIME systime;
                GetLocalTime(&systime);
                char szLogFile[64];
                sprintf(szLogFile, "packet_%02d%02d_%02d.log", systime.wMonth, systime.wDay, systime.wHour);
                ofstream outFile3(szLogFile, ios::out|ios::app);
                outFile3 << systime.wHour << ":" << systime.wMinute << ":" << systime.wSecond;
                outFile3 << " " << ul << " " << nLen << endl;
                outFile3.close();
            }
// Terence 20020314			

			if ((pDetail = m_RecvDetailList.find(DPidx(copItem()->GetDPID(), copItem()->GetDPInsID()))) != m_RecvDetailList.end())
			{
				if (pDetail->second->m_bRecoveryEnable)
                {
					iRc = pDetail->second->m_recovery.CheckSeq(ul, nLen);
                    if (iRc == CRecovery::SEQ_IN)
						pDetail->second->m_recovery.TTLCheck();
                }
               
            } else {
				shared_ptr<CRecvDetail> recvDetail(new CRecvDetail(copItem()->GetDPID(), copItem()->GetDPInsID(), m_bSeqLogEnable, m_nRecoveryWindowSize));
				m_RecvDetailList[DPidx(copItem()->GetDPID(), copItem()->GetDPInsID())] = recvDetail;
				iRc = recvDetail->m_recovery.CheckSeq(ul, nLen);
				pDetail = m_RecvDetailList.find(DPidx(copItem()->GetDPID(), copItem()->GetDPInsID()));
            }
            // ****
            switch(iRc)
            {
            case CRecovery::SEQ_OUT:
				pDetail->second->m_recovery.AddData(ul, pData, nLen);
            case CRecovery::SEQ_EQUAL:
                bRc = false;
                break;
            }
        }
    }
    else
    {
		if ((pDetail = m_RecvDetailList.find(DPidx(copItem()->GetDPID(), copItem()->GetDPInsID()))) != m_RecvDetailList.end())
        {
			if (pDetail->second->m_bRecoveryEnable)
            {
				pDetail->second->m_bRecoveryEnable = false;
                if (m_bLogEnable)
                {
                    SYSTEMTIME systime;
                    GetLocalTime(&systime);
                    ofstream outFile2("datactrl.log", ios::out|ios::app|ios::binary);
                    outFile2 << "<RecvCtrl Log " << systime.wYear << "\\" << systime.wMonth << "\\" << systime.wDay 
                                << " " << systime.wHour << ":" << systime.wMinute << ":" << systime.wSecond 
                                << "> DP:"  << copItem()->GetDPID() << " Ins:" << copItem()->GetDPInsID() 
                                << "  Recovery disable!" ;
                    outFile2.put(0x0D); outFile2 << endl;
                    outFile2.write((const char*)pData, nLen);
                    outFile2.put(0x0D); outFile2 << endl;
                    outFile2.put(0x0D); outFile2 << endl;
                    outFile2.close();
                }
            }
            
        }

    }
    return bRc;
}

//static unsigned long ctn2 = 0;
void CRecvCtrl::ProcessCOP(COP_ITEM& copItem)
{
	if (m_SeqCallbackList.size() > 0)
	{
		PRECV_DETAIL_LIST pDetail;
		if ((pDetail = m_RecvDetailList.find(DPidx(copItem()->GetDPID(), copItem()->GetDPInsID()))) != m_RecvDetailList.end())
		{
			if (!pDetail->second->CheckSeq(copItem()->GetSeqNo()))
			{
				PSEQ_NO_CALLBACK_LIST pList;
				for (pList = m_SeqCallbackList.begin(); pList != m_SeqCallbackList.end(); pList++)
				{
					(*pList->m_pCallback)(copItem()->GetDPID(), copItem()->GetDPInsID(), pDetail->second->GetDiff(), pList->m_pData);
				}
			}
		}
		else {
			shared_ptr<CRecvDetail> recvDetail(new CRecvDetail(copItem()->GetDPID(), copItem()->GetDPInsID(), m_bSeqLogEnable, m_nRecoveryWindowSize));
			recvDetail->CheckSeq(copItem()->GetSeqNo());
			m_RecvDetailList[DPidx(copItem()->GetDPID(), copItem()->GetDPInsID())] = recvDetail;
		}
	}

    if (m_bBufferEnable)
    {
        // 20102010 - Add OOB for system message
        bool bOOB = false;
        if (copItem->GetItemNo() == 0xFE000000)
        {
            Enter();
            PCALLBACKLIST p;
            for (p=m_OOBCallbackList.begin(); p!=m_OOBCallbackList.end(); p++)
            {
                (*p->m_pCallback)(copItem, p->m_Data);
                bOOB = true;
            }
            Leave();
        }
        // 20102010 - Add OOB for system message
        if (!bOOB)
        {
            m_buffer().Insert(copItem);
        }
    }
    else
    {
        Enter();
        PCALLBACKLIST p;
        for (p=m_CallbackList.begin(); p!=m_CallbackList.end(); p++)
            (*p->m_pCallback)(copItem, p->m_Data);
        Leave();
    }
}

void CRecvCtrl::ProcessDone()
{
}

void CRecvCtrl::Start()
{
    m_buffer.Start();

// Thread Priority Changes by Terence - Start
    m_buffer.SetPriority(m_iThreadPriority);
/*testtest
    char szText[256]; 
    sprintf(szText, "Receive Buffer Thread ID:%x Priority:%d", m_buffer.GetID(), m_buffer.GetPriority());
    MessageBox(NULL, szText, "Thread Priority", MB_OK);
*/
// Thread Priority Changes by Terence - End

    m_bExitThread = FALSE;
    ssThread::Start();
    m_hThread = (HANDLE)_beginthreadex(NULL,1,ThreadFunc,this,1,&m_nTID);
    SetThreadPriority(m_hThread, m_iThreadPriority);
}

void CRecvCtrl::Stop()
{
    m_bExitThread = TRUE;
    m_buffer.Stop();
    ssThread::ExitThread();
    ssThread::WaitForExit();
    WaitForSingleObject(m_hThread, INFINITE);
}

BOOL CRecvCtrl::IsAlive(DWORD dwTimeout)
{
    m_bCheckAlive = TRUE;
    return(ssThread::IsAlive(dwTimeout));
}

void CRecvCtrl::Reset()
{
    m_iBPS = 0;
    m_ulByteRecv = 0;
    m_ulPacketTotal = 0;
    m_ulMessageTotal = 0;
    m_usMessagePerPacket = 0;
    m_ulMessagePerSec = 0;
    m_ulPacketPerSec = 0;
    m_iBitCount = 0;
}

// Delegation
void CRecvCtrl::RegisterCallback(PCALLBACKFUNC p, void *pData)
{
    if (m_bBufferEnable)
        m_buffer.AddCallback(p, pData);
    else
    {
        Enter();
        CCallbackData data(p, pData);
        m_CallbackList.push_back(data);
        Leave();
    }
}

void CRecvCtrl::UnregisterCallback(PCALLBACKFUNC p, void *pData)
{
    if (m_bBufferEnable)
        m_buffer.RemoveCallback(p, pData);
    else
    {
        Enter();
        PCALLBACKLIST ptr;
        if (p == NULL)
            return;
        for (ptr=m_CallbackList.begin(); ptr!=m_CallbackList.end(); ptr++)
        {
            if (ptr->m_pCallback == p)
            {
                m_CallbackList.erase(ptr);
                break;
            }
        }
        Leave();
    }
}

void CRecvCtrl::RegisterCallback(PSEQNOCALLBACK p, void *that)
{
    CSeqNoCallback data(p, that);
    m_SeqCallbackList.push_back(data);
}

void CRecvCtrl::UnregisterCallback(PSEQNOCALLBACK pCallback, void *that)
{
    PSEQ_NO_CALLBACK_LIST p;
    if (pCallback == NULL) 
        return;
    for (p=m_SeqCallbackList.begin(); p!=m_SeqCallbackList.end(); p++)
    {
        if (p->m_pCallback == pCallback)
        {
            m_SeqCallbackList.erase(p);
            break;
        }
    }
}


bool CRecvCtrl::GetOutSeqTotal(string & outputtext)
{
	//PRECV_DETAIL_LIST pDetail;

	//string outputtext;
	std::ostringstream os;
	bool bPL = false;
	PRECV_DETAIL_LIST pDetail = m_RecvDetailList.begin();

	while (pDetail != m_RecvDetailList.end())
	{
		if (pDetail->second->m_recovery.m_ulOutSeq > pDetail->second->m_recovery.m_ulOutSeqLast) {
			pDetail->second->m_recovery.m_ulOutSeqLast = pDetail->second->m_recovery.m_ulOutSeq;
			os << PDidx2DPID(pDetail->first) << "(" << DPidx2DPIns(pDetail->first) << ") : " << pDetail->second->m_recovery.m_ulOutSeq << endl;
		}
		pDetail++;
	}

	if (bPL)
		outputtext = os.str();
	return bPL;
   
}
/*

unsigned long CRecvCtrl::GetRecoverTotal(int i)
{ 
    if (m_RecvDetailList.size() >  i)
        return m_RecvDetailList[i]->m_recovery.m_ulRecover;
    else
        return 0;
}*/


UINT CRecvCtrl::ThreadFunc(void* lpParam)
{
    ((CRecvCtrl*)lpParam)->ProcessThreadFunc();
    return 0;
}

void CRecvCtrl::ProcessThreadFunc()
{
    ULONG ulByte = 0;
    unsigned long ulMT = 0, ulPT = 0;
    int iBitPerSec = 0;
    int iBitCount = 0;

    ssTimer Timer;
    ssMultiWait MultiWait;
    MultiWait.Add(ssThread::m_ExitEvent, EVENT_EXIT);
    MultiWait.Add(Timer, EVENT_TIMER);
    while (!m_bExitThread)
    {
        switch(MultiWait.Wait())
        {
        case EVENT_EXIT:
            m_bExitThread = TRUE;
            break;
        case EVENT_TIMER:
            if (ulByte < m_ulByteRecv)
            {
                m_iBPS = m_ulByteRecv - ulByte;
            }
            ulByte = m_ulByteRecv;
            if (ulMT < m_ulMessageTotal)
                m_ulMessagePerSec = m_ulMessageTotal - ulMT;
            ulMT = m_ulMessageTotal;
            if (ulPT < m_ulPacketTotal)
                m_ulPacketPerSec = m_ulPacketTotal - ulPT;
            ulPT = m_ulPacketTotal;

// Terence 20020507
            iBitPerSec = iBitCount;
            iBitCount = 0;
            if (m_bPacketLogEnable)
            {
                SYSTEMTIME systime;
                GetLocalTime(&systime);
                char szLogFile[64];
                sprintf(szLogFile, "BPS_%02d%02d.log", systime.wMonth, systime.wDay);
                ofstream outFile3(szLogFile, ios::out|ios::app);
                outFile3 << systime.wHour << ":" << systime.wMinute << ":" << systime.wSecond;
                outFile3 << " " << m_iBitCount << " " << m_ulPacketPerSec << " " << m_iBPS << endl;
                outFile3.close();
            }
            m_iBitCount = 0;
// Terence 20020507
            break;
        }
    }
}

BOOL CRecvCtrl::SetMaxLimit(UINT unLimit)
{
	return m_buffer.SetUpperLimit(unLimit);
}

BOOL CRecvCtrl::SetIncrement(UINT unIncre)
{
	return m_buffer.SetIncrement(unIncre);
}

BOOL CRecvCtrl::SetMaxBound(UINT unBound)
{
	return m_buffer.SetUpperBound(unBound);
}

//20Oct2010 - Add OOB for system message
void CRecvCtrl::RegisterOOBCallback(PCALLBACKFUNC p, void *pData)
{
    Enter();
    CCallbackData data(p, pData);
    m_OOBCallbackList.push_back(data);
    Leave();
}

void CRecvCtrl::UnregisterOOBCallback(PCALLBACKFUNC p, void *pData)
{
    Enter();
    PCALLBACKLIST ptr;
    if (p == NULL)
        return;
    for (ptr=m_OOBCallbackList.begin(); ptr!=m_OOBCallbackList.end(); ptr++)
    {
        if (ptr->m_pCallback == p)
        {
            m_OOBCallbackList.erase(ptr);
            break;
        }
    }
    Leave();
}
// 20102010 - Add OOB for system message