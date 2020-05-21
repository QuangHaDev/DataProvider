/////////////////////////////////////////////////////////////////////////////
//   File       :: Arbitrator.h
//   Author     :: John Lee
//   Date       :: 1 September 2000
//   Version    :: 0.10 (Beta)
// 
//
//   Description:: CArbitrator class definition.
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
#if !defined(AFX_ARBITRATOR_H__54820773_AA17_11D4_9634_0006290599A5__INCLUDED_)
#define AFX_ARBITRATOR_H__54820773_AA17_11D4_9634_0006290599A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MCastBuffer.h"
#include "udpcast.h"
#include "ssTimer.h"
#include "RecvCtrl.h"

#ifdef DATACTRL_EXPORTS
#define DATACTRL_API __declspec(dllexport)
#else
#define DATACTRL_API __declspec(dllimport)
#endif

typedef void (*REBUILDREQUESTCALLBACK) (const char *p, void *pData);
typedef void (*REBUILDREPLYCALLBACK) (void *pData);

class DATACTRL_API CArbitrator : public ssThread
{
public:
    CArbitrator(unsigned short usDP, byte byID);
    virtual ~CArbitrator();

    enum ARB_STATE {STATE_UNKNOWN, STATE_LISTEN, STATE_PRIMARY, STATE_RAISE_HAND, STATE_DISABLE};
    enum {EVENT_CHECKALIVE=1, EVENT_EXIT=2, EVENT_TIMER=3};

    virtual void Main();
    bool    Start(const char *pIP, unsigned short usPort);
    void    Stop();
    void    Process(COP_ITEM& item, const char *pIP);
    void    SetIdleLimit(ULONG ulLimit);
    void    SetPrimaryEnable(bool bEnable);
    void    Send(COP_ITEM& pItem);
    bool    IsPrimary() { return (m_arbState == STATE_PRIMARY); }
    int     GetStatus();
    int     GetIdleCount() { return m_iIdleCount; }

    inline unsigned short        GetDPID() { return m_usDP; }
    inline byte         GetDPIns() { return m_byID; }
    inline byte         GetPrimaryDPIns() { return m_byPrimaryID; }
    inline ARB_STATE    CheckState() { return m_arbState; }

    void    SetRecv(CRecvCtrl* recvctrl);
    static void RecvCallback(COP_ITEM& item, void *pData);
    void    ProcessRecvCallback(COP_ITEM& item);
    void    EnableChannelMonitor(bool bEnable);

    void    RebuildCycleRequest(string& strID);
    void    RebuildCycleReply(const char *pID);
    void    RegisterRequestCallback(REBUILDREQUESTCALLBACK req, void *pData);
    void    RegisterReplyCallback(REBUILDREPLYCALLBACK rep, void *pData);
    REBUILDREQUESTCALLBACK m_RequestFunc;
    REBUILDREPLYCALLBACK m_ReplyFunc;
    void *m_RequestData, *m_ReplyData;
    string m_strRebuildID;

private:
    bool    Validate(const byte* pData, int nLen);

    CUDPCast m_udp;
        
    unsigned short   m_usDP;
    byte    m_byID;
    byte    m_byPrimaryID;
    int     m_iIdleCount;
    int     m_iIdleLimit;
    int     m_iRaiseHandCount;
    ARB_STATE m_arbState;
    ssMultiWait m_MultiWait;
    BOOL    m_bExitThread;
    bool    m_bPrimaryEnable;

    void    RaiseHand();
    void    RaiseHandReply(byte byID);

    bool    m_bLogEnable;
    CRecvCtrl *m_RecvCtrl;
    int     m_nDataCheck;
    bool    m_bChannelMonitor;
};

#endif // !defined(AFX_ARBITRATOR_H__54820773_AA17_11D4_9634_0006290599A5__INCLUDED_)
