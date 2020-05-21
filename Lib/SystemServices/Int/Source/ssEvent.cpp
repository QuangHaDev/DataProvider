/////////////////////////////////////////////////////////////////////////////
//          File:: ssEvent.h
// Create Author:: Terence Lai
// Create   Date:: 13 Aug 1998
//
//   Description:: ssEvent class member function
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
#include "ssEvent.h"

// --------------------------------------------------------------------------
// Default construction
ssEvent::ssEvent() :
	m_hHandle(0),
	m_bInheritable(FALSE),
	m_dwLastResult(ERROR_SUCCESS)
{
	// Auto-reset, initial state is not signal
	m_hHandle = ::CreateEvent(0,FALSE,FALSE,"");
}

// --------------------------------------------------------------------------
// Optional construction
ssEvent::ssEvent(
			BOOL  bManualReset,
			BOOL  bInitialState,
			const LPSECURITY_ATTRIBUTES lpsa,
			const char* pszName) :
	m_hHandle(0),
	m_bInheritable(FALSE),
	m_dwLastResult(ERROR_SUCCESS)
{
	if(lpsa!=0)
	{
		m_bInheritable=lpsa->bInheritHandle;
	}

	m_hHandle = ::CreateEvent(lpsa, bManualReset, bInitialState, pszName);
}


// --------------------------------------------------------------------------
// Copy constructor
ssEvent::ssEvent( const ssEvent& that)
{
	Duplicate(that);
}


// --------------------------------------------------------------------------
// Destructor
ssEvent::~ssEvent()
{
	if (m_hHandle != 0)
	{
		::CloseHandle(m_hHandle) ;
	}
	m_hHandle = 0 ;
}


// --------------------------------------------------------------------------
// operator =
const ssEvent& ssEvent::operator = (const ssEvent & that)
{
	HANDLE temp = m_hHandle;	// Save original Handle
	if(this == &that)
	{
		return *this;			// Self assignment
	}

	Duplicate(that) ;			// Clone 

	if (temp != NULL)
	{
		::CloseHandle(temp);	// Close the old handle
	}
	return *this;
}

//
// ********* ALL OTHER METHODS TO BE PLACED BELOW ***********
//

// --------------------------------------------------------------------------
// signal event
BOOL ssEvent::Set() const
{
	return ::SetEvent(m_hHandle);
}


// --------------------------------------------------------------------------
// reset event signal
BOOL ssEvent::Reset() const
{
	return ::ResetEvent(m_hHandle);
}


// --------------------------------------------------------------------------
// Waits for the SyncEvent to become signaled with a specific timeout. 
BOOL ssEvent::Wait(DWORD dwTimeout)
{
	BOOL bRetValue=FALSE;
	switch(::WaitForSingleObjectEx(m_hHandle, dwTimeout, TRUE))
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
DWORD ssEvent::GetResult() const
{
	return m_dwLastResult;
}


// --------------------------------------------------------------------------
// Provides compatibility with the Win32 API synchronization wait functions.
ssEvent::operator HANDLE() const
{
	return m_hHandle;
}


// --------------------------------------------------------------------------
// Duplicates object
void ssEvent::Duplicate( const ssEvent &that)
{
	m_bInheritable = that.m_bInheritable;
	m_dwLastResult = that.m_dwLastResult;

	if(that.m_hHandle==0)
	{
		return;
	}

	HANDLE hProcess = GetCurrentProcess();
	if (::DuplicateHandle(
				hProcess, that,				// original Event
				hProcess,&m_hHandle,		// new handle
				STANDARD_RIGHTS_REQUIRED,	// Ignored, since cloning
				m_bInheritable,				// Inheritable
				DUPLICATE_SAME_ACCESS)==0)	// clone access
	{
		m_hHandle = 0;						// If call failed, make sure our handle is NULLed.
		m_dwLastResult = ::GetLastError();
	}
}
