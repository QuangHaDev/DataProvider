/////////////////////////////////////////////////////////////////////////////
//   File       :: DataCtrl.cpp
//   Author     :: John Lee
//   Date       :: 1 September 2000
//   Version    :: 1.00
// 
//
//   Description:: Implementation of the CDataCtrl class.
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

//#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include "stdafx.h"
#include "DataCtrl.h"
#include "comdef.h"
#include <sstream>

// This is the constructor of a class that has been exported.
// see DataCtrl.h for the class definition
CDataCtrl::CDataCtrl()
{ 
    m_recvCtrl = NULL; 
    m_sendCtrl = NULL;
    m_arbitrator = NULL;
    m_usUDPPort = 0;
    m_bLogEnable = false;
    m_bSeqLogEnable = false;
    m_bReceiveBufferEnable = true;
    m_nRecoveryWindowSize = 11;


// Thread Priority Changes by Terence - Start
    m_iThreadPriority = THREAD_PRIORITY_ABOVE_NORMAL;
    int iThreadPriority = 0;
    DWORD dwSizeInt = sizeof(iThreadPriority);
// Thread Priority Changes by Terence - End
    int nRWS = 11;

    HKEY hKey;
    DWORD dwLogEnable = 0;
    DWORD dwSize = sizeof(dwLogEnable);
    char szVer[] = "2.0.2.30";
    if ( RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\AFE\\DataCtrl", 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS )
    {
        if ( RegQueryValueEx( hKey, "LogEnable", NULL, NULL, (LPBYTE) &dwLogEnable, &dwSize) == ERROR_SUCCESS )
			m_bLogEnable = (dwLogEnable == 1);
		dwSize = sizeof(dwLogEnable);
        if ( RegQueryValueEx( hKey, "SeqLogEnable", NULL, NULL, (LPBYTE) &dwLogEnable, &dwSize) == ERROR_SUCCESS )
            m_bSeqLogEnable = (dwLogEnable == 1) ;
		dwSize = sizeof(dwLogEnable);
        if ( RegQueryValueEx( hKey, "ReceiveBufferEnable", NULL, NULL, (LPBYTE) &dwLogEnable, &dwSize) == ERROR_SUCCESS )
            m_bReceiveBufferEnable = (dwLogEnable == 1);
// Thread Priority Changes by Terence - Start
        if ( RegQueryValueEx( hKey, "ThreadPriority", NULL, NULL, (LPBYTE) &iThreadPriority, &dwSizeInt) == ERROR_SUCCESS )
        {
            m_iThreadPriority = iThreadPriority;
        }
        if ((m_iThreadPriority != THREAD_PRIORITY_NORMAL) &&
            (m_iThreadPriority != THREAD_PRIORITY_ABOVE_NORMAL) &&
            (m_iThreadPriority != THREAD_PRIORITY_HIGHEST))
        {
            char szText[256];
            sprintf(szText, "Thread priority (%d) is not the recommended\nsetting, it may affect system performance.\nWould you like to reset it?", m_iThreadPriority);
            if (MessageBox(NULL, szText, "Thread Priority", MB_YESNO|MB_ICONWARNING|MB_DEFBUTTON1|MB_SYSTEMMODAL) == IDYES)
            {
                m_iThreadPriority = THREAD_PRIORITY_ABOVE_NORMAL;
            }
        }
// Thread Priority Changes by Terence - End
		dwSizeInt = sizeof(nRWS);

        if ( RegQueryValueEx( hKey, "RecoveryWindowSize", NULL, NULL, (LPBYTE) &nRWS, &dwSizeInt) == ERROR_SUCCESS )
        {
            m_nRecoveryWindowSize = nRWS;
        }

        RegCloseKey(hKey);
    }
    if ( RegCreateKey( HKEY_LOCAL_MACHINE, "SOFTWARE\\AFE\\DataCtrl", &hKey) == ERROR_SUCCESS )
    {
		dwSize = sizeof(dwLogEnable);
		dwSizeInt = sizeof(m_iThreadPriority);
        RegSetValueEx(hKey, "Version", 0, REG_SZ, (LPBYTE) szVer, strlen(szVer));
        dwLogEnable = (m_bLogEnable)?1:0;
        RegSetValueEx(hKey, "LogEnable", 0, REG_DWORD, (LPBYTE) &dwLogEnable, dwSize);
        dwLogEnable = (m_bSeqLogEnable)?1:0;
        RegSetValueEx(hKey, "SeqLogEnable", 0, REG_DWORD, (LPBYTE) &dwLogEnable, dwSize);
        dwLogEnable = (m_bReceiveBufferEnable)?1:0;
        RegSetValueEx(hKey, "ReceiveBufferEnable", 0, REG_DWORD, (LPBYTE) &dwLogEnable, dwSize);
// Thread Priority Changes by Terence - Start
        RegSetValueEx(hKey, "ThreadPriority", 0, REG_DWORD, (LPBYTE) &m_iThreadPriority, dwSizeInt);
// Thread Priority Changes by Terence - End
        dwSize = sizeof(m_nRecoveryWindowSize);
        RegSetValueEx(hKey, "RecoveryWindowSize", 0, REG_DWORD, (LPBYTE) &m_nRecoveryWindowSize, dwSize);
        RegCloseKey(hKey);
    }
}

CDataCtrl::~CDataCtrl()
{
    if (m_recvCtrl != NULL)
        delete m_recvCtrl;
    if (m_sendCtrl != NULL)
        delete m_sendCtrl;
    if (m_arbitrator != NULL)
        delete m_arbitrator;
}

bool CDataCtrl::SetUDPPort(unsigned short usPort)
{
    if (m_sendCtrl == NULL)
    {
        m_usUDPPort = usPort;
        return true;
    }
    return false;
}

int CDataCtrl::Start(int nPort, const char *pIFAddr)
{
    GUID m_guid;
    HRESULT hr = CoCreateGuid(&m_guid);
    if (FAILED (hr)) 
    {
        TRACE("CoCreateGuid failed!\n");
        return false;
    }

    char szTemp[128];
    sprintf(szTemp, "%x%x%x%x%x%x%x%x%x%x%x",
        m_guid.Data1, m_guid.Data2, m_guid.Data3, m_guid.Data4[0], m_guid.Data4[1], m_guid.Data4[2],
        m_guid.Data4[3], m_guid.Data4[4], m_guid.Data4[5], m_guid.Data4[6], m_guid.Data4[7]);
    m_strRebuildID = szTemp;
    TRACE("%s\n", m_strRebuildID.c_str());

    strcpy(m_szIP, pIFAddr);
    return m_service.Start(nPort, pIFAddr); 
}

void CDataCtrl::Stop()
{
    if (m_recvCtrl != NULL)
    {
        delete m_recvCtrl;
        m_recvCtrl = NULL;
    }
    if (m_sendCtrl != NULL)
    {
        delete m_sendCtrl;
        m_sendCtrl = NULL;
    }
    if (m_arbitrator != NULL)
    {
		m_arbitrator->SetRecv(NULL);
        delete m_arbitrator;
        m_arbitrator = NULL;
    }
    m_service.Stop();
}

// Client
void CDataCtrl::SetLogEnable(bool bLogEnable)
{
    m_bLogEnable = bLogEnable;
    if (m_recvCtrl != NULL)
        m_recvCtrl->m_bLogEnable = bLogEnable;
}

int CDataCtrl::GetRecvID()
{
    if (m_recvCtrl == NULL)
        return 0;
    return m_recvCtrl->GetID();
}

void CDataCtrl::RegisterCallback(PCALLBACKFUNC p, void *that)
{
    if (m_recvCtrl == NULL)
        // Thread Priority Changes by Terence - Start
        m_recvCtrl = new CRecvCtrl((CMCast*)&m_service, m_bLogEnable, m_bSeqLogEnable, m_bReceiveBufferEnable, m_iThreadPriority, m_nRecoveryWindowSize);
        // Thread Priority Changes by Terence - End
    m_recvCtrl->RegisterCallback(p, that);
}

void CDataCtrl::RegisterCallback(PSEQNOCALLBACK p, void *that)
{
    if (m_recvCtrl == NULL)
        // Thread Priority Changes by Terence - Start
		m_recvCtrl = new CRecvCtrl((CMCast*)&m_service, m_bLogEnable, m_bSeqLogEnable, m_bReceiveBufferEnable, m_iThreadPriority, m_nRecoveryWindowSize);
        // Thread Priority Changes by Terence - End
    m_recvCtrl->RegisterCallback(p, that);
}

void CDataCtrl::Register_OOB_Callback(PCALLBACKFUNC p, void *that)
{
    if (m_recvCtrl == NULL)
        // Thread Priority Changes by Terence - Start
		m_recvCtrl = new CRecvCtrl((CMCast*)&m_service, m_bLogEnable, m_bSeqLogEnable, m_bReceiveBufferEnable, m_iThreadPriority, m_nRecoveryWindowSize);
        // Thread Priority Changes by Terence - End
    m_recvCtrl->RegisterOOBCallback(p, that);
}

void CDataCtrl::Unregister_OOB_Callback(PCALLBACKFUNC p, void *that)
{
    if (m_recvCtrl == NULL)
        return;
    m_recvCtrl->UnregisterOOBCallback(p, that);
}

void CDataCtrl::UnregisterCallback(PSEQNOCALLBACK p, void *that)
{
    if (m_recvCtrl == NULL)
        return;
    m_recvCtrl->UnregisterCallback(p, that);
}

void CDataCtrl::UnregisterCallback(PCALLBACKFUNC p, void *that)
{
    if (m_recvCtrl == NULL)
        return;
    m_recvCtrl->UnregisterCallback(p, that);
    // Remove m_recvCtrl if callback list == 0?
}

int CDataCtrl::AddGroup(const char *pGrpAddr, const char *pIFAddr)
{
    if (m_recvCtrl == NULL)
        // Thread Priority Changes by Terence - Start
		m_recvCtrl = new CRecvCtrl((CMCast*)&m_service, m_bLogEnable, m_bSeqLogEnable, m_bReceiveBufferEnable, m_iThreadPriority, m_nRecoveryWindowSize);
        // Thread Priority Changes by Terence - End
    return (m_service.AddGroup(pGrpAddr, pIFAddr) == CMCast::MC_SUCCESS)? 
        DC_SUCCESS : DC_ERROR;
}

int CDataCtrl::DelGroup(const char *pGrpAddr, const char *pIFAddr)
{
    if (m_recvCtrl == NULL)
        return DC_ERROR;
    return (m_service.DelGroup(pGrpAddr, pIFAddr) == CMCast::MC_SUCCESS)?
        DC_SUCCESS : DC_ERROR;
}

int CDataCtrl::GetCliCount()
{
    if (m_recvCtrl == NULL)
        return DC_ERROR;
    return m_recvCtrl->GetCount();
}

int CDataCtrl::GetCliMaxCount()
{
    if (m_recvCtrl == NULL)
        return DC_ERROR;
    return m_recvCtrl->GetMaxCount();
}

BOOL CDataCtrl::SetCliMaxLimit(UINT unLimit)
{
	if (m_recvCtrl == NULL)
		return FALSE;
	return m_recvCtrl->SetMaxLimit(unLimit);
}

BOOL CDataCtrl::SetCliIncrement(UINT unIncre)
{
	if (m_recvCtrl == NULL)
		return FALSE;
	return m_recvCtrl->SetIncrement(unIncre);
}

BOOL CDataCtrl::SetCliMaxBound(UINT unBound)
{
	if (m_recvCtrl == NULL)
		return FALSE;
	return m_recvCtrl->SetMaxBound(unBound);
}

int CDataCtrl::GetCliBPS()
{
    if (m_recvCtrl == NULL)
        return 0;
    return m_recvCtrl->GetBPS();
}

void CDataCtrl::CliReset()
{
    if (m_recvCtrl == NULL)
        return;
    m_recvCtrl->Reset();
}

unsigned long CDataCtrl::GetTotalRecv()
{
    if (m_recvCtrl == NULL)
        return 0;
    return m_recvCtrl->GetByteRecv();
}

unsigned long CDataCtrl::GetCliPacketTotal()
{
    if (m_recvCtrl == NULL)
        return NULL;
    return m_recvCtrl->GetPacketTotal(); 
}

unsigned long CDataCtrl::GetCliMessageTotal()
{
    if (m_recvCtrl == NULL)
        return NULL;
    return m_recvCtrl->GetMessageTotal(); 
}

unsigned short CDataCtrl::GetCliMessagePerPacket()
{
    if (m_recvCtrl == NULL)
        return NULL;
    return m_recvCtrl->GetMessagePerPacket(); 
}

unsigned long CDataCtrl::GetCliMessagePerSec()
{
    if (m_recvCtrl == NULL)
        return NULL;
    return m_recvCtrl->GetMessagePerSec(); 
}

unsigned long CDataCtrl::GetCliPacketPerSec()
{
    if (m_recvCtrl == NULL)
        return NULL;
    return m_recvCtrl->GetPacketPerSec(); 
}

bool CDataCtrl::GetOutSeqTotal(string & outtext)
{
    if (m_recvCtrl == NULL)
        return 0;
	return m_recvCtrl->GetOutSeqTotal(outtext);
}

/*
unsigned long CDataCtrl::GetRecoverTotal(int i)
{
    if (m_recvCtrl == NULL)
        return 0; 
    return m_recvCtrl->GetRecoverTotal(i);
}*/

// Server
void CDataCtrl::AddChannel(const char *pIP, int nPort)
{
    if (m_sendCtrl == NULL)
        // Thread Priority Changes by Terence - Start
		m_sendCtrl = new CSendCtrl((CMCast*)&m_service, 0, 0, m_iThreadPriority);
        // Thread Priority Changes by Terence - End
    m_sendCtrl->AddChannel(pIP, nPort);
}

void CDataCtrl::AddMonitorChannel(const char *pIP, int nPort)
{
    if (m_arbitrator == NULL)
        return;
    if (m_service.AddGroup(pIP, m_szIP) == CMCast::MC_SUCCESS)
    {
        TRACE("AddMonitorChannel ok\n");
        m_arbitrator->EnableChannelMonitor(true);
    }
}

void CDataCtrl::DelChannel(const char *pIP, int nPort)
{
    if (m_sendCtrl == NULL)
        return;
    m_sendCtrl->DelChannel(pIP, nPort);
}

void CDataCtrl::DelMonitorChannel(const char *pIP, int nPort)
{
    if (m_arbitrator == NULL)
        return;
    if (m_service.DelGroup(pIP, m_szIP) == CMCast::MC_SUCCESS)
    {
        TRACE("DelMonitorChannel ok\n");
        m_arbitrator->EnableChannelMonitor(false);
    }
}

int CDataCtrl::Send(COP_ITEM& item)
{
    int nRc = DC_ERROR;
    if (m_sendCtrl == NULL)
        return nRc;
    switch (m_sendCtrl->SendCOP(item))
    {
    case CSendCtrl::SEND_SUCCESS:
        nRc = DC_SUCCESS; break;
    case CSendCtrl::SEND_ERROR:
        nRc = DC_ERROR; break;
    case CSendCtrl::NOT_PRIMARY:
        nRc = DC_ERR_NOT_PRIMARY; break;
    }
    return nRc;
}

int CDataCtrl::Send(const char *p, int iLen)
{
    if (m_sendCtrl == NULL)
        return DC_ERROR;
    return (m_sendCtrl->Send(p, iLen) == CSendCtrl::SEND_SUCCESS)?
        DC_SUCCESS : DC_ERROR;
}

int CDataCtrl::SetSvrSpeed(UINT uiBPS, UINT uiInterval)
{
    if (m_sendCtrl == NULL)
        return DC_ERROR;
    m_sendCtrl->SetSpeed(uiBPS, uiInterval);
    return DC_SUCCESS;
}

int CDataCtrl::GetSvrBPS()
{
    if (m_sendCtrl == NULL)
        return 0;
    return m_sendCtrl->GetBPS();
}

int CDataCtrl::GetSvrCount()
{
    if (m_sendCtrl == NULL)
        return 0;
    return m_sendCtrl->GetCount();
}

void CDataCtrl::SetSvrTTL(int iTTL)
{
    if (m_sendCtrl == NULL)
        return;
    m_service.SetTTL(iTTL);
}

void CDataCtrl::SvrReset()
{
    if (m_sendCtrl == NULL)
        return;
    m_sendCtrl->Reset();
}

void CDataCtrl::RegisterCallback(PSENDCALLBACK pSendCallback, void *that)
{
    if (m_sendCtrl == NULL)
        return;
    m_sendCtrl->RegisterCallback(pSendCallback, that);
}

void CDataCtrl::SetSendCtrlDPIP(unsigned short usID, byte byIns) //No m_arbitabor

{
  if (m_sendCtrl == NULL)
        // Thread Priority Changes by Terence - Start
		m_sendCtrl = new CSendCtrl((CMCast*)&m_service, usID, byIns, m_iThreadPriority);
        // Thread Priority Changes by Terence - End
    else
    {
        m_sendCtrl->SetDPID(usID);
        m_sendCtrl->SetDPIDIns(byIns);
    }

}

// Application
void CDataCtrl::SetSvrDPID(unsigned short usID, byte byIns)
{
    SetSendCtrlDPIP(usID, byIns);
    if (m_arbitrator == NULL)
    {
        m_arbitrator = new CArbitrator(usID, byIns);
		m_arbitrator->SetRecv(new CRecvCtrl((CMCast*)&m_service, false, false, false, 1, 0));
        m_arbitrator->Start(m_szIP, m_usUDPPort);
        // Thread Priority Changes by Terence - Start
        m_arbitrator->SetPriority(m_iThreadPriority);
        // Thread Priority Changes by Terence - End
        m_sendCtrl->SetArbitrator(m_arbitrator);
    }
}

void CDataCtrl::SetSvrMsgPacking(bool bEnable)
{
    if (m_sendCtrl == NULL)
        return;
    m_sendCtrl->SetMsgPacking(bEnable);
}

// Arbitrator
byte CDataCtrl::GetPrimaryIns()
{
    if (m_arbitrator == NULL)
        return 255;
    return m_arbitrator->GetPrimaryDPIns();
}

int CDataCtrl::GetSvrStatus()
{
    if (m_arbitrator == NULL)
        return DC_STATUS_UNKNOWN;
    switch(m_arbitrator->GetStatus())
    {
    case CArbitrator::STATE_LISTEN:
        return DC_STATUS_LISTEN;
    case CArbitrator::STATE_PRIMARY:
        return DC_STATUS_PRIMARY;
    case CArbitrator::STATE_RAISE_HAND:
        return DC_STATUS_RAISE_HAND;
    case CArbitrator::STATE_DISABLE:
        return DC_STATUS_DISABLE;
    default:
        return DC_STATUS_UNKNOWN;
    }
}

int CDataCtrl::GetIdleCount()
{
    if (m_arbitrator == NULL)
        return 0;
    return m_arbitrator->GetIdleCount();
}

void CDataCtrl::SetSvrPrimaryEnable(bool bEnable)
{
    if (m_arbitrator == NULL)
        return;
    m_arbitrator->SetPrimaryEnable(bEnable);
}

void CDataCtrl::RequestRebuild(REBUILDREPLYCALLBACK cb, void *pData)
{
    ASSERT(m_szIP != NULL);
    if (cb == NULL)
        return;
    if (m_arbitrator == NULL)
    {
        m_arbitrator = new CArbitrator(0, 0);
        m_arbitrator->SetPrimaryEnable(false);
        m_arbitrator->Start(m_szIP, m_usUDPPort);
    }
    m_arbitrator->RegisterReplyCallback(cb, pData);
    m_arbitrator->RebuildCycleRequest(m_strRebuildID);
}

void CDataCtrl::CancelRebuild()
{
    if (m_arbitrator != NULL)
    {
        m_arbitrator->RegisterReplyCallback(NULL, NULL);
        delete m_arbitrator;
        m_arbitrator = NULL;
    }
}


// *********************************************************************************
static AFX_EXTENSION_MODULE DataCtrlDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("DATACTRL.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(DataCtrlDLL, hInstance))
			return 0;

		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		new CDynLinkLibrary(DataCtrlDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("DATACTRL.DLL Terminating!\n");
		// Terminate the library before destructors are called
		AfxTermExtensionModule(DataCtrlDLL);
	}
	return 1;   // ok
}
