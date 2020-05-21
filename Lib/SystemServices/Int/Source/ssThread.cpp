/////////////////////////////////////////////////////////////////////////////
//          File:: ssThread.h
// Create Author:: Terence Lai
// Create   Date:: 15 Aug 1998
//
//   Description:: ssThread class member function
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
#include <process.h>	// for _beginThrdThreadex and _endThrdThreadex
#include "ssThread.h"

ssCriticalSection ssThread::m_csThrdPtr;

// --------------------------------------------------------------------------
// Default construction
ssThread::ssThread(
			const char* pszThreadName,
			LPSECURITY_ATTRIBUTES pSecurityAttrs,
			UINT iStackSize) :
	m_CheckAliveEvent(),	// Auto Reset
	m_AckAliveEvent(),		// AutoReset
	m_ExitEvent(TRUE),		// Manual Reset
	m_bStarted(FALSE),
	m_bExit(FALSE),
	m_hHandle(0),
	m_iID(0),
	m_pSecurityAttrs(pSecurityAttrs),
	m_iStackSize(iStackSize),
	m_dwLastResult(ERROR_SUCCESS)
{
	strncpy_s(m_szThreadName, pszThreadName, SS_MAX_THREAD_NAME);
	m_szThreadName[SS_MAX_THREAD_NAME] = 0;
}


// --------------------------------------------------------------------------
// Destructor
ssThread::~ssThread()
{
	if (!m_bExit)
	{
		ExitThread();
	}

	if(m_hHandle!=0)
	{
		WaitForExit(5000);  // timeout for 5 seconds
		::CloseHandle(m_hHandle);
	}
}

//
// ********* ALL OTHER METHODS TO BE PLACED BELOW ***********
//

// --------------------------------------------------------------------------
// Start the thread executing
HANDLE ssThread::Start()
{
	if(m_bStarted)
	{
		return m_hHandle;
	}
	// Use the argument list to pass the 'this' pointer to new ThrdThread. 
	m_hHandle = (HANDLE)_beginthreadex(	m_pSecurityAttrs,
										m_iStackSize,
										&ThreadEntryPoint,
										(LPVOID)this,
										0,				// start immediately
										&m_iID);	// thread ID
	return(m_hHandle);
}


// --------------------------------------------------------------------------
// Suspends the thread
DWORD ssThread::Suspend() const
{
	return(::SuspendThread(m_hHandle));
}


// --------------------------------------------------------------------------
// Resumes the thread
DWORD ssThread::Resume() const
{
	return(::ResumeThread(m_hHandle));
}


// --------------------------------------------------------------------------
// Sets the thread's priority
BOOL ssThread::SetPriority(const int iPriority) const
{
	return(::SetThreadPriority(m_hHandle, iPriority));
}


// --------------------------------------------------------------------------
// Gets the thread's priority
int ssThread::GetPriority() const
{
	return(::GetThreadPriority(m_hHandle));
}


// --------------------------------------------------------------------------
// Gets the thread's ThreadID
UINT ssThread::GetID() const
{
	return(m_iID);
}


// --------------------------------------------------------------------------
// Gets the thread's Name
const char* ssThread::GetName() const
{
	return(m_szThreadName);
}


// --------------------------------------------------------------------------
// Gets the thread's ThreadID
void ssThread::SetName(char* pszThreadName)
{
	strncpy_s(m_szThreadName, pszThreadName, SS_MAX_THREAD_NAME);
	m_szThreadName[SS_MAX_THREAD_NAME] = 0;
}


// --------------------------------------------------------------------------
// Determines whether the thread has started
BOOL ssThread::HasStarted() const
{
	return(m_bStarted);
}


// --------------------------------------------------------------------------
// Waits for the thread to start executing. 
BOOL ssThread::IsAlive(DWORD dwTimeout)
{
	// trigger check alive event
	m_CheckAliveEvent.Set();

	// wait for child class to acknowledge alive event
	if (!m_AckAliveEvent.Wait(dwTimeout))
	{
		m_dwLastResult = m_AckAliveEvent.GetResult();
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}


// --------------------------------------------------------------------------
// Trigger thread to exit
void ssThread::ExitThread()
{
	m_bExit = TRUE;
	m_ExitEvent.Set();
}


// --------------------------------------------------------------------------
// Trigger thread to exit
BOOL ssThread::IsExit()
{
	return m_bExit;
}


// --------------------------------------------------------------------------
// Waits for the thread to exit.
BOOL ssThread::WaitForExit(DWORD dwTimeout)
{
	BOOL bRetValue = FALSE;
	switch(::WaitForSingleObject(m_hHandle, dwTimeout))
	{
		case WAIT_OBJECT_0:
			m_dwLastResult = WAIT_OBJECT_0;
			bRetValue=TRUE;
			break;
		case WAIT_TIMEOUT:
			m_dwLastResult = WAIT_TIMEOUT;
			break;
		case WAIT_ABANDONED:
			m_dwLastResult = WAIT_ABANDONED;
			break;
		case WAIT_FAILED:
			m_dwLastResult=::GetLastError();
			break;
	}
	return bRetValue;
}


// --------------------------------------------------------------------------
// Returns a system error or one of the specified results caused by the last wait operation.
DWORD ssThread::GetResult() const
{
	return m_dwLastResult;
}


// --------------------------------------------------------------------------
// Provides compatibility with the Win32 API synchronization wait functions.
ssThread::operator HANDLE() const
{
	return m_hHandle;
}


// ----------------------------------------------------------------------
// For concrete implementation to acknowledge alive
void ssThread::AckAlive()
{
	m_AckAliveEvent.Set();
}

// --------------------------------------------------------------------------
// This static function is the actual starting execution point.
UINT APIENTRY ssThread::ThreadEntryPoint(LPVOID pParam)
{
	m_csThrdPtr.Enter();	//Protect pParam. It is the "this pointer"
	ssThread* pThread = (ssThread*) pParam;
	pThread->ExecuteUserThread();
	return(0);
}


// --------------------------------------------------------------------------
// Executes the user-defined Thread.
void ssThread::ExecuteUserThread()
{
	m_csThrdPtr.Leave();		// Release pParam. It is the "this pointer"
	m_bStarted = TRUE;	// Indicate the thread is executing

	Main();					//Execute the main routine
}
