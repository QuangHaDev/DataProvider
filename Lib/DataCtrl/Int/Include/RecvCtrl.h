/////////////////////////////////////////////////////////////////////////////
//   File       :: RecvCtrl.h
//   Author     :: John Lee
//   Date       :: 1 September 2000
//   Version    :: 1.00
// 
//
//   Description:: CRecvCtrl class definition.
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
#ifndef __RECVCTRL_H__
#define __RECVCTRL_H__

#include "MCast.h"
#include "RIOSocket.h"
#include "MCastBuffer.h"
#include "ssTimer.h"
#include <vector>
#include <map>
#include <unordered_map>
#include <list>
#include "recovery.h"
#include <memory>

#ifdef DATACTRL_EXPORTS
#define DATACTRL_API __declspec(dllexport)
#else
#define DATACTRL_API __declspec(dllimport)
#endif

// class CRecvDetail
typedef void (*PSEQNOCALLBACK)(unsigned short usDPID, byte byDPIns, ULONG ulDiff, void *that);


class CSeqNoCallback
{
public:
    CSeqNoCallback(PSEQNOCALLBACK pCallback, void *pData) : m_pCallback(pCallback), m_pData(pData) {};
    virtual ~CSeqNoCallback() {};

    void *m_pData;
    PSEQNOCALLBACK m_pCallback;
};

//list to vector to improve memory locality and performance. as there is not random insertion required.
typedef std::vector<CSeqNoCallback> SEQ_NO_CALLBACK_LIST;
typedef SEQ_NO_CALLBACK_LIST::iterator PSEQ_NO_CALLBACK_LIST;


class CRecvDetail
{
public:
    CRecvDetail(unsigned short usDPID, byte byDPIns, bool bSeqLogEnable, int nRecoveryWindowSize);
    bool CheckDP(unsigned short usDPID, byte byDPIns);
    bool CheckSeq(byte bySeq);
    inline ULONG GetDiff() { return m_ulDiff; }
    CRecovery m_recovery;
    bool m_bRecoveryEnable;

private:
    unsigned short m_usDPID;
    byte m_byDPIns;
    byte m_bySeq;

    ULONG m_ulDiff;
};

typedef std::pair<short, byte> DPIDkey;

//typedef std::map<DPIDkey, shared_ptr<CRecvDetail>> RECV_DETAIL_LIST;
inline int DPidx(short i, byte j) { return (((int)i) << 16 )| j; }
inline short PDidx2DPID(int i) { return i >> 16; }
inline byte DPidx2DPIns(int i){ return (byte)i; }
typedef std::unordered_map<int, shared_ptr<CRecvDetail>> RECV_DETAIL_LIST;
typedef RECV_DETAIL_LIST::iterator PRECV_DETAIL_LIST;


// class CRecvCtrl
class DATACTRL_API CRecvCtrl : public ssThread, ssCriticalSection
{
public:
// Thread Priority Changes by Terence - Start
    CRecvCtrl(CMCast *service, bool bLogEnable, bool bSeqLogEnable, bool bBufferEnable, int iThreadPriority, int nRecoveryWindowSize);
// Thread Priority Changes by Terence - End
    virtual ~CRecvCtrl();

    void Start();
    void Stop();
    void Reset();
    void RegisterCallback(PCALLBACKFUNC p, void *that);
    void RegisterCallback(PSEQNOCALLBACK p, void *that);
    void UnregisterCallback(PCALLBACKFUNC p, void *that);
    void UnregisterCallback(PSEQNOCALLBACK pCallback, void *that);

    void RegisterOOBCallback(PCALLBACKFUNC p, void *pData);
    void UnregisterOOBCallback(PCALLBACKFUNC p, void *pData);

    // Statistics
    inline int GetCount() { return m_buffer.GetCount(); }
    inline int GetMaxCount() { return m_buffer.GetMaxCount(); }
    inline int GetBPS() { return m_iBPS; }
    inline unsigned long GetByteRecv() { return m_ulByteRecv; }
    inline unsigned long GetPacketTotal() { return m_ulPacketTotal; }
    inline unsigned long GetMessageTotal() { return m_ulMessageTotal; }
    inline unsigned short GetMessagePerPacket() { return m_usMessagePerPacket; }
    inline unsigned long GetMessagePerSec() { return m_ulMessagePerSec; }
    inline unsigned long GetPacketPerSec() { return m_ulPacketPerSec; }

    BOOL SetIncrement(UINT unIncre);
	BOOL SetMaxBound(UINT unBound);
	BOOL SetMaxLimit(UINT unLimit);


	bool GetOutSeqTotal(string & outputtext);
	//unsigned long GetOutSeqTotal(short nDPID, byte bDPIns);
    //unsigned long GetRecoverTotal(int i);

    static UINT APIENTRY ThreadFunc(void* lpParam);
    void ProcessThreadFunc();

    enum {EVENT_DATA, EVENT_CHECKALIVE, EVENT_EXIT, EVENT_TIMER};

    bool m_bLogEnable;
    bool m_bSeqLogEnable;
    bool m_bBufferEnable;
    bool m_bPacketLogEnable;	// Terence 20020314
// Thread Priority Changes by Terence - Start
    int  m_iThreadPriority;
// Thread Priority Changes by Terence - End
    int m_nRecoveryWindowSize;

private:
    virtual void Main();
    virtual void ProcessCOP(COP_ITEM& copItem);
    virtual void ProcessDone();

    bool RecoveryProcess(COP_ITEM& copItem, const byte* pData, int nLen);
    BOOL IsAlive(DWORD dwTimeout = INFINITE);
    bool Validate(const byte* pData, int nLen);
    bool MsgTypeValidate(COP_ITEM& copItem, char *pAddr);
    
    // Statistics
    int             m_iBPS;
    unsigned long   m_ulByteRecv;
    unsigned long   m_ulPacketTotal, m_ulMessageTotal;
    unsigned long   m_ulPacketPerSec, m_ulMessagePerSec;
    unsigned short  m_usMessagePerPacket;

    CMCast*         m_pService;
    BOOL            m_bExitThread;
    BOOL            m_bCheckAlive;
    CMCastBuffer    m_buffer;
    // Callback while wrong seq no
    RECV_DETAIL_LIST        m_RecvDetailList;
    SEQ_NO_CALLBACK_LIST    m_SeqCallbackList;

    CALLBACKLIST    m_CallbackList;
    CALLBACKLIST    m_OOBCallbackList;
    HANDLE          m_hThread;
    UINT            m_nTID;
    int             m_iBitCount;

};

#endif // __RECVCTRL_H__