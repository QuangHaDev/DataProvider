/////////////////////////////////////////////////////////////////////////////
//   File       :: Arbitrator.h
//   Author     :: John Lee
//   Date       :: 1 September 2000
//   Version    :: 1.0
// 
//
//   Description:: Implementation of the CArbitrator class.
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

#include "Arbitrator.h"
#include "ApplManClient.h"
#include <fstream>
#include <iomanip>
using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CArbitrator::CArbitrator(unsigned short usDP, byte byID) :
    m_usDP(usDP), m_byID(byID)
{
    m_iIdleCount = 0;
    m_iIdleLimit = 3; // sec
    m_byPrimaryID = 255;
    m_iRaiseHandCount = 0;
    m_arbState = STATE_LISTEN;
    m_bPrimaryEnable = true;
    m_RequestFunc = NULL;
    m_ReplyFunc = NULL;
    m_bLogEnable = 0;
    m_RecvCtrl = NULL;
    m_nDataCheck = 0;
    m_bChannelMonitor = false;
}

CArbitrator::~CArbitrator()
{
    Stop();
}

void CArbitrator::RaiseHand()
{
    m_arbState = STATE_RAISE_HAND;
    COP_ITEM pItem;
    pItem = new BUFFER_ITEM;
    pItem()->SetMsgType(MGT_RAISEHANDREQUEST);
    Send(pItem);
}

void CArbitrator::RaiseHandReply(byte byID)
{
    COP_ITEM pItem;
    pItem = new BUFFER_ITEM;
    pItem()->AddFid(1, &byID);
    pItem()->SetMsgType(MGT_RAISEHANDREPLY);
    Send(pItem);
}   

void CArbitrator::Send(COP_ITEM& pItem)
{
    pItem()->SetDPID(GetDPID());
    pItem()->SetDPInsID(GetDPIns());
    pItem()->GetMessage();
    // seqno ?
    m_udp.Send((LPCSTR)pItem()->GetMessage(), pItem()->GetMsgLen());
}

void CArbitrator::Process(COP_ITEM& item, const char *pIP)
{
    byte byMsgType = item()->GetMsgType();

    short sFidType;
    short sFid;
// 20121112
//    long lCurFid = 0;
	short sCurFid = 0;

    if (byMsgType == MGT_LOGONREQUEST)
    {
        if (m_RequestFunc != NULL)
        {
            CharArray ca;
            COP_ITEM copItem = new MsgObject(item()->GetMessage());
            sFidType = copItem()->GetFidType(sCurFid, sFid);
            copItem()->GetFid(sFid, (datatype&)ca);
            (*m_RequestFunc)((const char *)ca.GetString(), m_RequestData);
        }
    } else if (byMsgType == MGT_LOGONRESPONSE)
    {
        if (m_ReplyFunc != NULL)
        {
            CharArray ca;
            COP_ITEM copItem = new MsgObject(item()->GetMessage());
            sFidType = copItem()->GetFidType(sCurFid, sFid);
            copItem()->GetFid(sFid, (datatype&)ca);
            if (strcmp(m_strRebuildID.c_str(), (const char *)ca.GetString()) == 0)
                (*m_ReplyFunc)(m_ReplyData);
        }
    }
    // Only check for the same DP
    if (item()->GetDPID() != GetDPID())
        return;
    byte byDPInsID = item()->GetDPInsID();
    if (m_bChannelMonitor && m_nDataCheck > 6)
    {
        char szState[64];
        switch(m_arbState)
        {
        case STATE_LISTEN:
            strcpy_s(szState, sizeof(szState), "STATE_LISTEN"); break;
        case STATE_PRIMARY:
            strcpy_s(szState, sizeof(szState), "STATE_PRIMARY"); break;
        case STATE_RAISE_HAND:
            strcpy_s(szState, sizeof(szState), "STATE_RAISE_HAND"); break;
        default:
            strcpy_s(szState, sizeof(szState), "UNKNOWN"); break;
        }
        CApplManClient::Log(LOG_ERROR, "ARB %s(%s) Type(%d) %d(%d) Pri(%d)  check:%d\n", szState, pIP, (int)byMsgType,
            item->GetDPID(), byDPInsID, (int)m_byPrimaryID, m_nDataCheck);
        if (byDPInsID == m_byPrimaryID)
            return;
    }
    m_iIdleCount = 0;

//    TRACE("ARB (%s) TYPE(%d) Primary:%d(%d)(%s)  check:%d\n", szState, (int)byMsgType,
//        item->GetDPID(), byDPInsID, pIP, m_nDataCheck);
    if (m_arbState == STATE_LISTEN)
    {
        // DP is sending data -> set DP to primary
        switch(byMsgType)
        {
        case MGT_RAISEHANDREPLY:
            m_byPrimaryID = byDPInsID;
            if (m_bChannelMonitor && m_nDataCheck <= 6)
                m_nDataCheck++;
            break;
//        case MGT_RAISEHANDREQUEST:
//            m_byPrimaryID = byDPInsID;
//            RaiseHandReply(byDPInsID);
//            break;
        default:
            break;
        }
    }
    else
    {
        switch(byMsgType)
        {
        case MGT_RAISEHANDREPLY:
            BYTE i;
            item()->GetFid(1, (char&)i);
            if (GetDPIns() == i)
                m_arbState = STATE_PRIMARY;
            else
                m_arbState = STATE_LISTEN;
            break;
        case MGT_RAISEHANDREQUEST:
            if (byDPInsID == GetDPIns())
                RaiseHandReply(byDPInsID);
            break;
        default:
            m_arbState = STATE_LISTEN;
        }
    }
}

void CArbitrator::SetIdleLimit(ULONG ulLimit)
{
    m_iIdleLimit = ulLimit;
}

bool CArbitrator::Start(const char *pIP, unsigned short usPort)
{
    m_udp.Start(pIP, usPort);
    m_bExitThread = FALSE;
    ssThread::Start();
    ssThread::SetPriority(1);
    return true;
}

void CArbitrator::Stop()
{
    if (m_RecvCtrl != NULL)
    {
        m_RecvCtrl->UnregisterCallback(RecvCallback, this);
        delete m_RecvCtrl;
    }
    m_bExitThread = TRUE;
    ssThread::ExitThread();
    ssThread::WaitForExit();
}

void CArbitrator::Main()
{
    int nRet, nLen;
    char szBuffer[1024], szRemote[32];
    ssTimer TimerEvent;
    SYSTEMTIME systime;

    TimerEvent.Set(500);
    m_MultiWait.Add(TimerEvent, EVENT_TIMER);
    m_MultiWait.Add(ssThread::m_CheckAliveEvent, EVENT_CHECKALIVE);
    m_MultiWait.Add(ssThread::m_ExitEvent, EVENT_EXIT);

    while (!m_bExitThread)
    {
        switch(m_MultiWait.Wait(0))
        {
        case EVENT_CHECKALIVE:
            ssThread::AckAlive();
            break;
        case EVENT_EXIT:
            m_bExitThread = TRUE;
            break;
        case EVENT_TIMER:
            if (!m_bPrimaryEnable)
                break;
            if (m_arbState != STATE_PRIMARY)
            {
                if (++m_iIdleCount > m_iIdleLimit)
                    RaiseHand();
            } else if (m_arbState == STATE_PRIMARY)
                RaiseHandReply(GetDPIns());
            break;
        default:
            {
            nRet = m_udp.SelectRead(30);
            switch(nRet)
            {
            case CUDPCast::UDP_SUCCESS:
                break;
            case CUDPCast::UDP_ERROR:
                break;
            case CUDPCast::UDP_READ:
                if (m_udp.Recv(szBuffer, sizeof(szBuffer), szRemote, nLen) == CUDPCast::UDP_SUCCESS)
                {
                    if (Validate((const byte *)szBuffer, nLen))
                    {
                        COP_ITEM pItem = new BUFFER_ITEM((const unsigned char *)szBuffer);
                        Process(pItem, szRemote);
                    }
                    else
                    {
                        if (m_bLogEnable)
                        {
                            GetLocalTime(&systime);
                            ofstream outFile2("datactrl.log", ios::out|ios::app|ios::binary);
                            outFile2 << "<CArbitrator Log " << systime.wYear << "\\" << systime.wMonth << "\\" << systime.wDay 
                                    << " " << systime.wHour << ":" << systime.wMinute << ":" << systime.wSecond << ">" ;
                            outFile2.put(0x0D); outFile2 << endl;
                            outFile2.write(szBuffer, nLen);
                            outFile2.put(0x0D); outFile2 << endl;
                            outFile2.put(0x0D); outFile2 << endl;
                            outFile2.close();
                        }
                        nLen = 0;
                    }
                }
                break;
            }
            }
        }
    }
}

bool CArbitrator::Validate(const byte* pData, int nLen)
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

int CArbitrator::GetStatus()
{
    return m_arbState;
}

void CArbitrator::SetPrimaryEnable(bool bEnable)
{
    if (!bEnable)
    {
        m_arbState = STATE_DISABLE;
        m_bPrimaryEnable = false;
    }
    else
    {
        if (!m_bPrimaryEnable)
        {
            m_arbState = STATE_LISTEN;
            m_bPrimaryEnable = true;
        }
    }
}

void CArbitrator::SetRecv(CRecvCtrl* recvctrl)
{
    m_RecvCtrl = recvctrl;
    if (m_RecvCtrl != NULL)
    {
        m_RecvCtrl->RegisterCallback(RecvCallback, this);
    }
}

void CArbitrator::RecvCallback(COP_ITEM& item, void *pData)
{
    ((CArbitrator*)pData)->ProcessRecvCallback(item);
}

void CArbitrator::ProcessRecvCallback(COP_ITEM& item)
{
//    TRACE("ARB  item:%d  dp:%d  ins:%d\n", item->GetItemNo(), item->GetDPID(), item->GetDPInsID());
    if (GetDPID() != item->GetDPID())
        return;
    if (GetDPIns() != item->GetDPInsID())
        m_arbState = STATE_LISTEN;
    m_nDataCheck = 0;
}

void CArbitrator::EnableChannelMonitor(bool bEnable)
{
    m_nDataCheck = 0;
    m_bChannelMonitor = bEnable;
}

// Front End
void CArbitrator::RebuildCycleRequest(string& strID)
{
    COP_ITEM pItem;
    char szID[128];
    m_strRebuildID = strID;
    memset(szID, '\0', sizeof(szID));
    strcpy(szID, m_strRebuildID.c_str());
    CharArray *pca = new CharArray(szID, 0, 0);
    pItem = new BUFFER_ITEM;
    pItem()->AddFid(FID_GEN_TXT1, pca);
    pItem()->SetMsgType(MGT_LOGONREQUEST);
    Send(pItem);
}   

// RSS
void CArbitrator::RebuildCycleReply(const char *pID)
{
    ASSERT(strlen(pID));
    COP_ITEM pItem;
    char szID[128];
    strcpy(szID, pID);
    CharArray *pca = new CharArray(szID, 0, 0);
    pItem = new BUFFER_ITEM;
    pItem()->AddFid(FID_GEN_TXT1, pca);
    pItem()->SetMsgType(MGT_LOGONRESPONSE);
    Send(pItem);
}   

void CArbitrator::RegisterRequestCallback(REBUILDREQUESTCALLBACK req, void *pData)
{
    m_RequestFunc = req;
    m_RequestData = pData;
}

void CArbitrator::RegisterReplyCallback(REBUILDREPLYCALLBACK rep, void *pData)
{
    m_ReplyFunc = rep;
    m_ReplyData = pData;
}
