/////////////////////////////////////////////////////////////////////////////
//   File       :: DataCtrl.h
//   Author     :: John Lee
//   Date       :: 1 September 2000
//   Version    :: 1.00
// 
//
//   Description:: CDataCtrl class definition.
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
#ifndef __DATACTRL_H__
#define __DATACTRL_H__

#ifdef DATACTRL_EXPORTS
#define DATACTRL_API __declspec(dllexport)
#else
#define DATACTRL_API __declspec(dllimport)
#endif

#include "MCast.h"
#include "SendCtrl.h"
#include "RecvCtrl.h"
#include "Arbitrator.h"
#ifndef _MSC_PLATFORM_TOOLSET_v120_xp
#include "RIOSocket.h"
#endif
// This class is exported from the DataCtrl.dll
class DATACTRL_API CDataCtrl {
public:
    CDataCtrl(void);
    virtual ~CDataCtrl();

    int Start(int nPort, const char *pIFAddr=NULL);
    void Stop();
    // CLIENT
    void RegisterCallback(PCALLBACKFUNC p, void *that);
    void UnregisterCallback(PCALLBACKFUNC p, void *that);
    void RegisterCallback(PSEQNOCALLBACK p, void *that);
    void UnregisterCallback(PSEQNOCALLBACK p, void *that);
    void RegisterCallback(PSENDCALLBACK pSendCallback, void *that);

    void Register_OOB_Callback(PCALLBACKFUNC p, void *that);
    void Unregister_OOB_Callback(PCALLBACKFUNC p, void *that);

    int AddGroup(const char *pGrpAddr, const char *pIFAddr=NULL);
    int DelGroup(const char *pGrpAddr, const char *pIFAddr=NULL);
    int GetCliCount(); // Get the counter for the internal receiving queue
    int GetCliMaxCount(); // Get the peak counter for the internal receiving queue
    int GetCliBPS();
    void CliReset();
    unsigned long GetTotalRecv();
    unsigned long GetCliPacketTotal();
    unsigned long GetCliMessageTotal();
    unsigned short GetCliMessagePerPacket();
    unsigned long GetCliMessagePerSec();
    unsigned long GetCliPacketPerSec();
	bool GetOutSeqTotal(string & outtext);
//    unsigned long GetRecoverTotal(int i);
	BOOL SetCliMaxLimit(UINT unLimit);
	BOOL SetCliMaxBound(UINT unBound);
	BOOL SetCliIncrement(UINT unIncre);

    void RequestRebuild(REBUILDREPLYCALLBACK cb, void *pData);
    void CancelRebuild();

    int GetRecvID();

    // SERVER
    void AddChannel(const char *pIP, int nPort);
    void DelChannel(const char *pIP, int nPort);
    void AddMonitorChannel(const char *pIP, int nPort);
    void DelMonitorChannel(const char *pIP, int nPort);

    void SetSvrTTL(int iTTL);
    int Send(COP_ITEM& item);
    // Do not use SetSvrSpeed if speed control is not wanted.
    int SetSvrSpeed(UINT uiBPS, UINT uiInterval);
    int GetSvrBPS();
    int GetSvrCount(); // Get the counter for the internal sending queue
    void SvrReset();
    int Send(const char *p, int iLen); // for testing
    // App
	void SetSendCtrlDPIP(unsigned short usID, byte byIns);
    void SetSvrDPID(unsigned short usID, byte byIns);
    void SetSvrMsgPacking(bool bEnbale);
    int GetSvrStatus();
    int GetIdleCount(); // for testing
    void SetSvrPrimaryEnable(bool bEnable);
    void SetLogEnable(bool bLogEnable);
    bool SetUDPPort(unsigned short usPort);

    // Arbitrator
    byte GetPrimaryIns();

    enum {DC_SUCCESS, DC_ERROR, DC_ERR_NOT_PRIMARY};
    enum {DC_STATUS_UNKNOWN, DC_STATUS_LISTEN, DC_STATUS_PRIMARY, DC_STATUS_RAISE_HAND, DC_STATUS_DISABLE};

private:
 
#ifdef _MSC_PLATFORM_TOOLSET_v120_xp
    CMCast m_service;
#else
	RIOSocket m_service;
#endif

//	CMCast m_service;

    CSendCtrl *m_sendCtrl;
    CRecvCtrl *m_recvCtrl;
    CArbitrator *m_arbitrator;
    char m_szIP[16];
    bool m_bLogEnable;
    bool m_bSeqLogEnable;
    bool m_bReceiveBufferEnable;
// Thread Priority Changes by Terence - Start
    int  m_iThreadPriority;
// Thread Priority Changes by Terence - End
    unsigned short m_usUDPPort;
    string m_strRebuildID;
    int m_nRecoveryWindowSize;
};

#endif // __DATACTRL_H__