/////////////////////////////////////////////////////////////////////////////
//          File:: ApplManClient.h
// Create Author:: Nick Fung
// Create   Date:: 09 Nov 2000
//
//   Description:: ApplManClient class declaration
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
// $Date: 10/08/15 3:04p $ 
// $Author: Sulfred_lee $ 

#if !defined(AFX_APPLMANCLIENT_H__20BED4F7_795E_11D2_BDC9_00C04F881F8D__INCLUDED_)
#define AFX_APPLMANCLIENT_H__20BED4F7_795E_11D2_BDC9_00C04F881F8D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "MsgLevel.h"
#include "ssCriticalSection.h"

/////////////////////////////////////////////////////////////////////////////
//
// Class      : CApplManClient
// Description: The CApplManClient class provide list of static member 
//  			functions for user to monitor their process. 
//
// Methods:
//    CApplManClient()	- Constructs a CApplManClient object.
//    ~CApplManClient()	- Destructs a CApplManClient object.
//    Initialize()		- Initizalize the class.
//    Uninitialize()	- Suspends exection of the thread.
//	  SetTraceOff()		- Turn off all trace message.
//	  SetTraceOn()		- Turn on all trace message.
//	  IsTraceOn()		- get the status of trace message.
//	  Log()				- print out 5 level of log message.
//	  Trace()			- print out 10 level of trace message
//
//////////////////////////////////////////////////////////////////////

class CApplManClient
{
public:
	CApplManClient();
	virtual ~CApplManClient();
	static void SetTraceOff();
	static void SetTraceOn();
	static BOOL Initialize(DWORD dwAppId, BOOL bIsIsapiDllClient=FALSE);
	static void Uninitialize();
	static BOOL Log(int iLevel, LPCTSTR szFormat, ...);
	static BOOL Trace (int iLevel, LPCTSTR szFormat, ...);
	static BOOL Alert(LPCTSTR szFormat, ...);
	static BOOL IsTraceOn();
	static void AckAlive();
	static void RebuildCompleted();
	static void SetDataCtrlStatus(long lDCStatus);

	static int WaitForEvents(DWORD dwTimeout = INFINITE);

private:

	static void RebuildStarted();

	static void FormatLog(int iLevel, size_t sizeBuffer, char* pszString);
	static BOOL SendLog(int iLevel, char* pszString);

	static void FormatTrace(int iLevel, size_t sizeBuffer, char* pszString);
	static BOOL SendTrace(int iLevel, char* pszString);

	static BOOL SendAlert(char* pszString);

	static const char* GetFormattedSystemTime()
	{
		SYSTEMTIME SysTime;
		GetSystemTime(&SysTime);
		sprintf_s(m_szFormattedSystemTime, "%04u/%02u/%02u %02u:%02u:%02u", 
			SysTime.wYear, SysTime.wMonth, SysTime.wDay, 
			SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
		return(m_szFormattedSystemTime);
	}
	
	static BOOL CheckEvents();
	static BOOL InitRpcClient();
	static BOOL UninitRpcClient();

	static char	m_szFormattedSystemTime[TEMP_BUFFER];

	static HANDLE m_hWaitEvent[NUMBER_WAITEVENT];
	static HANDLE m_hReplyEvent[NUMBER_REPLYEVENT];
	static DWORD m_dwAppId;
	static BOOL m_bInstantiated;
	static BOOL m_bInitialized;
	static BOOL m_bIsapidllClient;
	static BOOL m_bTraceOn;
	static BOOL m_bExit;
	static unsigned char* m_pszStringBinding;
	static ssCriticalSection m_CS;
};

static CApplManClient manClient;

#endif // !defined(AFX_APPLMANCLIENT_H__20BED4F7_795E_11D2_BDC9_00C04F881F8D__INCLUDED_)
