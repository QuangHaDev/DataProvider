/////////////////////////////////////////////////////////////////////////////
//   File       :: SendCtrl.cpp
//   Author     :: John Lee
//   Date       :: 1 September 2000
//   Version    :: 0.10 (Beta)
// 
//
//   Description:: Implementation of the CSendCtrl class.
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

#include "SendCtrl.h"
#include "Arbitrator.h"
#include "ApplManClient.h"


// class CSendDetail
CSendDetail::CSendDetail(const char *pIP, int iPort) :
    m_iPort(iPort), m_ucSeq(0)
{ 
    strcpy(m_szIP, pIP); 
}

unsigned char CSendDetail::GetSeq()
{
    if (m_ucSeq == 255)
        m_ucSeq = 0;
    else
        m_ucSeq++;
    return m_ucSeq;
}


// class CSendCtrl
// Thread Priority Changes by Terence - Start
CSendCtrl::CSendCtrl(CMCast *service, unsigned short usDPID, byte byDPIns, int iThreadPriority) :
    m_pService(service), m_bExit(FALSE), m_bCheckAlive(FALSE), 
    m_usDPID(usDPID), m_byDPIns(byDPIns)
// Thread Priority Changes by Terence - End
{
    m_pSendCallback = NULL;
    InitializeCriticalSection(&m_cs);
    m_hSend = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_hSendFinish = CreateEvent(NULL, FALSE, FALSE, NULL);

    m_buffer.AddCallback(CallbackFunc, this);
    m_buffer.Start();

// Thread Priority Changes by Terence - Start
	m_buffer.SetPriority(iThreadPriority);
// Thread Priority Changes by Terence - End

    m_pArb = NULL;
    Initialize();
    ssThread::Start();
// Thread Priority Changes by Terence - Start
	ssThread::SetPriority(iThreadPriority);
// Thread Priority Changes by Terence - End
}

CSendCtrl::~CSendCtrl()
{
    m_bExit = TRUE;
    ssThread::ExitThread();
    ssThread::WaitForExit();
    m_buffer.RemoveCallback(CallbackFunc, this);
    DeleteCriticalSection(&m_cs);
    m_buffer.Stop();
}

void CSendCtrl::SetSpeed(UINT uiBPS, UINT uiInterval)
{
    m_bSpeedControl = TRUE;
    m_uiBPS = uiBPS;
    m_uiInterval = uiInterval;
    m_uiSpeed = uiBPS * uiInterval / 1000;
}

void CSendCtrl::SetMsgPacking(bool bEnable)
{
    if (bEnable)
        SetSpeed(UINT_MAX, 100);
    else
        m_bSpeedControl = FALSE;
}

void CSendCtrl::AddChannel(const char *pIP, int iPort)
{
    PSEND_DETAIL_LIST pList;
    LOCK lock(&m_cs);
    for(pList=m_listSendDetail.begin(); 
        pList!=m_listSendDetail.end(); 
        pList++)
    {
        if ((strcmp(pIP, pList->GetIP()) == 0) &&
            (iPort == pList->GetPort()))
            return;
    }
    CSendDetail sd(pIP, iPort);
    m_listSendDetail.push_back(sd);
// The following line is not required if router operates properly.
//    m_pService->AddGroup(pIP, iPort);
}

void CSendCtrl::DelChannel(const char *pIP, int iPort)
{
    PSEND_DETAIL_LIST pList;
    LOCK lock(&m_cs);
    for(pList=m_listSendDetail.begin(); 
        pList!=m_listSendDetail.end(); 
        pList++)
    {
        if ((strcmp(pIP, pList->GetIP()) == 0) &&
            (iPort == pList->GetPort()))
        {
            m_listSendDetail.erase(pList);
            break;
        }
    }
}

void CSendCtrl::CallbackFunc(COP_ITEM& item, void *pData)
{
    if (pData != NULL)
        ((CSendCtrl*)pData)->ProcessCallback(item);
}

void CSendCtrl::Initialize()
{
    m_dwStart = GetTickCount();
    m_ulByteSent = 0;
    m_iBPS = 0;
    m_uiSent = 0;
    m_bSpeedControl = false;
}

void CSendCtrl::ProcessCallback(COP_ITEM& item)
{
    DWORD dwEnd;
    PSEND_DETAIL_LIST pList;
    LOCK lock(&m_cs);
    bool bSuccess = false;
	int nRc = CMCast::MC_ERROR;
    SEND_RESULT sendResult;

    item()->SetDPID(GetDPID());
    item()->SetDPInsID(GetDPIns());
    item()->GetMessage();

    // no speed control
    if (!m_bSpeedControl)
    {
        for(pList=m_listSendDetail.begin(); 
            pList!=m_listSendDetail.end(); 
            pList++)
        {
            item()->SetSeqNo(pList->GetSeq());
            nRc = m_pService->Send(pList->GetIP(), pList->GetPort(), 
                    (LPCSTR)item()->GetMessage(), item()->GetMsgLen());
        }
        m_ulByteSent += item()->GetMsgLen();
        if (nRc == CMCast::MC_ERROR)
            CApplManClient::Trace(TRACE_LEVEL0, "SendCtrl(0) error!  item:%d  len:%d", item()->GetItemNo(), item()->GetMsgLen());
        if (m_pSendCallback != NULL)
        {
            if (nRc == CMCast::MC_ERROR)
            {
                sendResult.usReturnCode = 1;
                sendResult.nErrorCode = m_pService->GetLastError();
                sendResult.unByteSend = 0;
            }
            else
            {
                sendResult.usReturnCode = 0;
                sendResult.nErrorCode = 0;
                sendResult.unByteSend = item()->GetMsgLen();
            }
            (*m_pSendCallback)(sendResult, m_pSendCallbackData);
        }
        return;
    }
    // speed control
    while (!bSuccess && !m_bExit)
    {
        dwEnd = GetTickCount();
        if ((dwEnd - m_dwStart) >= m_uiInterval)
        {
            SetEvent(m_hSend);
            WaitForSingleObject(m_hSendFinish, INFINITE);
            m_uiSent = 0;
            m_dwStart = dwEnd;
        }

        if (m_uiSent < m_uiSpeed)
        {
            bSuccess = true;
            for(pList=m_listSendDetail.begin(); 
                pList!=m_listSendDetail.end(); 
                pList++)
            {
                item()->SetSeqNo(pList->GetSeq());
                if (pList->m_message.AddMessage(item()->GetMsgLen(), (LPCSTR)item()->GetMessage()) == 1)
                {
                    SetEvent(m_hSend);
                    WaitForSingleObject(m_hSendFinish, INFINITE);
                    pList->m_message.AddMessage(item()->GetMsgLen(), (LPCSTR)item()->GetMessage());
                }
            }
            if (bSuccess)
            {
                m_uiSent += item()->GetMsgLen();
                m_ulByteSent += item()->GetMsgLen();
                break;
            }
            Sleep(0);
        }
        else
            Sleep(5);
    }
}

int CSendCtrl::SendCOP(COP_ITEM& item)
{

    if (m_pArb != NULL)
    {
        if (!m_pArb->IsPrimary())
            return NOT_PRIMARY;
    }

    if (m_buffer().Insert(item))
        return SEND_SUCCESS;
    else
        return SEND_ERROR;
}

int CSendCtrl::Send(const char *p, int iLen)
{
    PSEND_DETAIL_LIST pList;
    LOCK lock(&m_cs);
    for(pList=m_listSendDetail.begin(); 
        pList!=m_listSendDetail.end(); 
        pList++)
    {
        m_pService->Send(pList->GetIP(), pList->GetPort(), 
            p, iLen);
    }
//    m_ulByteSent += iLen;
    return SEND_SUCCESS;
}

ULONG CSendCtrl::GetByteSent()
{
    return m_ulByteSent;
}

void CSendCtrl::Main()
{
    ULONG ulByte = 0;
    PSEND_DETAIL_LIST pList;
    SEND_RESULT sendResult;
    int nRc;

    m_MultiWait.Add(m_hSend, EVENT_SEND);
    m_MultiWait.Add(ssThread::m_CheckAliveEvent, EVENT_CHECKALIVE);
    m_MultiWait.Add(ssThread::m_ExitEvent, EVENT_EXIT);
    
    while (!m_bExit)
    {
        switch(m_MultiWait.Wait(1000))
        {
        case EVENT_CHECKALIVE:
            m_bCheckAlive = FALSE;
            ssThread::AckAlive();
            break;
        case EVENT_EXIT:
            m_bExit = TRUE;
            break;
        case EVENT_SEND:
            for(pList=m_listSendDetail.begin(); 
                pList!=m_listSendDetail.end(); 
                pList++)
            {
                if (pList->m_message.GetLength() <= 0)
                    continue;
                nRc = m_pService->Send(pList->GetIP(), pList->GetPort(), 
                    pList->m_message.GetData(), pList->m_message.GetLength());

                if (m_pSendCallback != NULL)
                {
                    if (nRc == CMCast::MC_ERROR)
                    {
                        sendResult.usReturnCode = 1;
                        sendResult.nErrorCode = m_pService->GetLastError();
                        sendResult.unByteSend = 0;
                    }
                    else
                    {
                        sendResult.usReturnCode = 0;
                        sendResult.nErrorCode = 0;
                        sendResult.unByteSend = pList->m_message.GetLength();
                    }
                    (*m_pSendCallback)(sendResult, m_pSendCallbackData);
                }
                pList->m_message.Reset();
            }
            SetEvent(m_hSendFinish);
            break;
        default:
//            if (m_bCheckDelay)
//                CheckDelay();
            if (ulByte < m_ulByteSent)
                m_iBPS = m_ulByteSent - ulByte;
            ulByte = m_ulByteSent;
            if (m_buffer.GetCount() == 0)
                SetEvent(m_hSend);
        }
    }
}

void CSendCtrl::SetDPID(unsigned short usDPID)
{
    m_usDPID = usDPID;
}

void CSendCtrl::SetDPIDIns(byte byDPIns)
{
    m_byDPIns = byDPIns;
}

void CSendCtrl::Reset()
{
    m_iBPS = 0;
    m_ulByteSent = 0;
}

void CSendCtrl::SetArbitrator(CArbitrator *pArb)
{
    m_pArb = pArb;
}

void CSendCtrl::RegisterCallback(PSENDCALLBACK pSendCallback, void* pData)
{
    m_pSendCallback = pSendCallback;
    m_pSendCallbackData = pData;
}
