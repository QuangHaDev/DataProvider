/////////////////////////////////////////////////////////////////////////////
//   File       :: SendCtrl.h
//   Author     :: John Lee
//   Date       :: 1 September 2000
//   Version    :: 0.10 (Beta)
// 
//
//   Description:: CSendCtrl class definition.
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
#ifndef __SENDCTRL_H__
#define __SENDCTRL_H__

#pragma warning (disable:4786)
#include <list>
#include "MCast.h"
#include "MCastBuffer.h"
#include "Message.h"
#include "Arbitrator.h"

struct SEND_RESULT
{
    // 0: Success
    // 1: Error
    // 2: Not primary
    USHORT usReturnCode;
    int nErrorCode;
    UINT unByteSend;
};

typedef void (*PSENDCALLBACK)(SEND_RESULT sendResult, void* pData);

// class CSendDetail
class CSendDetail
{
public:
    CSendDetail(const char *pIP, int iPort);

    inline const char *GetIP() { return m_szIP; }
    inline int GetPort() { return m_iPort; }
    unsigned char GetSeq();
    CMCastMessage m_message;

private:
    char m_szIP[16];
    int m_iPort;
    unsigned char m_ucSeq;
};
typedef std::list<CSendDetail> SEND_DETAIL_LIST;
typedef SEND_DETAIL_LIST::iterator PSEND_DETAIL_LIST;


// class LOCK
class LOCK
{
public:
    LOCK(LPCRITICAL_SECTION pcs) : m_pcs(pcs) 
    { ::EnterCriticalSection(pcs); }
    ~LOCK() 
    { ::LeaveCriticalSection(m_pcs); }
private:
    LPCRITICAL_SECTION m_pcs;
};


class CArbitrator;

// class CSendCtrl
class CSendCtrl : public ssThread
{
public:
    // Thread Priority Changes by Terence - Start
    CSendCtrl(CMCast *service, unsigned short usDPID, byte byDPIns, int iThreadPriority);
    // Thread Priority Changes by Terence - End
    virtual ~CSendCtrl();

    enum {EVENT_CHECKALIVE=1, EVENT_EXIT=2, EVENT_SEND=3};
    void AddChannel(const char *pIP, int iPort);
    void DelChannel(const char *pIP, int iPort);
    void ProcessCallback(COP_ITEM& item);
    int SendCOP(COP_ITEM& item);
    int Send(const char *p, int iLen);
    void RegisterCallback(PSENDCALLBACK pSendCallback, void* pData);

    static void CallbackFunc(COP_ITEM& item, void *pData);

    void SetSpeed(UINT uiBPS, UINT uiInterval);
    void SetMsgPacking(bool bEnable = true);
    void SetArbitrator(CArbitrator *pArb);
    void Reset();

    unsigned long GetByteSent();
    inline int GetBPS() { return m_iBPS; }
    inline int GetCount() { return m_buffer.GetCount(); }

    // App
    void SetDPID(unsigned short sDPID);
    void SetDPIDIns(byte byDPIns);
    inline unsigned short GetDPID() { return m_usDPID; }
    inline byte GetDPIns() { return m_byDPIns; }

    enum {SEND_SUCCESS, SEND_ERROR, NOT_PRIMARY};

private:
    CMCast *m_pService;
    unsigned short m_usDPID;
    byte m_byDPIns;
    SEND_DETAIL_LIST m_listSendDetail;
    CRITICAL_SECTION m_cs;

    CMCastBuffer m_buffer;

    UINT m_uiBPS;
    UINT m_uiInterval;
    UINT m_uiSpeed;
    UINT m_uiSent;
    DWORD m_dwStart;

    BOOL m_bExit;
    BOOL m_bCheckAlive;
    bool m_bSpeedControl;
    ssMultiWait m_MultiWait;
    HANDLE m_hSend, m_hSendFinish;

    CArbitrator *m_pArb;
    PSENDCALLBACK m_pSendCallback;
    void* m_pSendCallbackData;

    // Statistics
    ULONG m_ulByteSent;
    int m_iBPS;

    void Initialize();
    virtual void Main();
};

#endif // __SENDCTRL_H__