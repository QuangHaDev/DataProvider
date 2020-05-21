/////////////////////////////////////////////////////////////////////////////
//          File:: ssTimer.h
// Create Author:: Terence Lai
// Create   Date:: 15 Aug 1998
//
//   Description:: ssTimer class member function
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
#include "ssTimer.h"

// --------------------------------------------------------------------------
// Default construction
ssTimer::ssTimer(
			DWORD	dwPeriod,
			PTIMERAPCROUTINE pCompletionRoutine,
			LPVOID	pArgToCompletionRoutine,
			BOOL	bResume,
			BOOL	bManualReset,
			const LPSECURITY_ATTRIBUTES lpsa,
			const char* pszName) :
	m_hHandle(0),
	m_dwLastResult(ERROR_SUCCESS)
{
	if ((m_hHandle = ::CreateWaitableTimer(lpsa, bManualReset, pszName)) != 0)
	{
		FILETIME ftSysTime;
		LARGE_INTEGER liSysTime;

		::GetSystemTimeAsFileTime(&ftSysTime);
		liSysTime.LowPart = ftSysTime.dwLowDateTime + (dwPeriod * 10000);
		liSysTime.HighPart = ftSysTime.dwHighDateTime;

		if (!::SetWaitableTimer(m_hHandle, &liSysTime, dwPeriod, pCompletionRoutine, pArgToCompletionRoutine, bResume))
		{
			::CloseHandle(m_hHandle);
			m_hHandle = 0;
			m_dwLastResult = ::GetLastError();
		}
	}
	else
	{
		m_dwLastResult = ::GetLastError();
	}
}

ssTimer::ssTimer(
			const LARGE_INTEGER* pDueTime,
			DWORD	dwPeriod,
			PTIMERAPCROUTINE pCompletionRoutine,
			LPVOID	pArgToCompletionRoutine,
			BOOL	bResume,
			BOOL	bManualReset,
			const LPSECURITY_ATTRIBUTES lpsa,
			const char* pszName) :
	m_hHandle(0),
	m_dwLastResult(ERROR_SUCCESS)
{
	if ((m_hHandle = ::CreateWaitableTimer(lpsa, bManualReset, pszName)) != 0)
	{
		if (!::SetWaitableTimer(m_hHandle, pDueTime, dwPeriod, pCompletionRoutine, pArgToCompletionRoutine, bResume))
		{
			::CloseHandle(m_hHandle);
			m_hHandle = 0;
			m_dwLastResult = ::GetLastError();
		}
	}
	else
	{
		m_dwLastResult = ::GetLastError();
	}
}
// --------------------------------------------------------------------------
// Destructor
ssTimer::~ssTimer()
{
	if (m_hHandle != 0)
	{
		::CloseHandle(m_hHandle) ;
	}
	m_hHandle = 0 ;
}

//
// ********* ALL OTHER METHODS TO BE PLACED BELOW ***********
//

// --------------------------------------------------------------------------
// signal event
BOOL ssTimer::Set(
			DWORD dwPeriod,
			PTIMERAPCROUTINE pCompletionRoutine,
			LPVOID pArgToCompletionRoutine,
			BOOL bResume)
{
	if (m_hHandle != 0)
	{
		FILETIME ftSysTime;
		LARGE_INTEGER liSysTime;

		::GetSystemTimeAsFileTime(&ftSysTime);
		liSysTime.LowPart = ftSysTime.dwLowDateTime + (dwPeriod * 10000);
		liSysTime.HighPart = ftSysTime.dwHighDateTime;

		if (!::SetWaitableTimer(m_hHandle, &liSysTime, dwPeriod, pCompletionRoutine, pArgToCompletionRoutine, bResume))
		{
			m_dwLastResult = ::GetLastError();
			return FALSE;
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL ssTimer::Set(
			const LARGE_INTEGER* pDueTime,
			DWORD dwPeriod,
			PTIMERAPCROUTINE pCompletionRoutine,
			LPVOID pArgToCompletionRoutine,
			BOOL bResume)
{
	if (m_hHandle != 0)
	{
		if (!::SetWaitableTimer(m_hHandle, pDueTime, dwPeriod, pCompletionRoutine, pArgToCompletionRoutine, bResume))
		{
			m_dwLastResult = ::GetLastError();
			return FALSE;
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

// --------------------------------------------------------------------------
// reset event signal
BOOL ssTimer::Reset() const
{
	return ::CancelWaitableTimer(m_hHandle);
}


// --------------------------------------------------------------------------
// Waits for the SyncEvent to become signaled with a specific timeout. 
BOOL ssTimer::Wait()
{
	BOOL bRetValue=FALSE;
	switch(::WaitForSingleObjectEx(m_hHandle, INFINITE, TRUE))
	{
		case WAIT_OBJECT_0:
			m_dwLastResult = ERROR_SUCCESS;
			bRetValue=TRUE;
			break;
		case WAIT_TIMEOUT:
			m_dwLastResult= WAIT_TIMEOUT;
			break;
		case WAIT_ABANDONED:
			m_dwLastResult= WAIT_ABANDONED;
			break;
		case WAIT_IO_COMPLETION:
			m_dwLastResult = WAIT_IO_COMPLETION;
			break;
		case WAIT_FAILED:
			m_dwLastResult=::GetLastError();
			break;
	}
	return bRetValue;
}


// --------------------------------------------------------------------------
// Returns a system error caused by the last wait operation.
DWORD ssTimer::GetResult() const
{
	return m_dwLastResult;
}


// --------------------------------------------------------------------------
// Provides compatibility with the Win32 API synchronization wait functions.
ssTimer::operator HANDLE() const
{
	return m_hHandle;
}


