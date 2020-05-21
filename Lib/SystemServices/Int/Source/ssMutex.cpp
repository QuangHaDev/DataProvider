/////////////////////////////////////////////////////////////////////////////
//          File:: ssMutex.h
// Create Author:: Terence Lai
// Create   Date:: 13 Aug 1998
//
//   Description:: ssMutex class member function
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
#include "ssMutex.h"

// --------------------------------------------------------------------------
// Default construction
ssMutex::ssMutex(
		BOOL  bInitOwned,
		const LPSECURITY_ATTRIBUTES lpsa,
		const char* pszName) :
	m_hHandle(0),
	m_bInheritable(FALSE),
	m_dwLastResult(ERROR_SUCCESS)
{
	if(lpsa!=0)
	{
		m_bInheritable = lpsa->bInheritHandle;
	}

	m_hHandle = ::CreateMutex(lpsa, bInitOwned, pszName);
}


// --------------------------------------------------------------------------
// Copy constructor
ssMutex::ssMutex( const ssMutex& that)
{
	Duplicate(that);
}


// --------------------------------------------------------------------------
// Destructor
ssMutex::~ssMutex()
{
	if (m_hHandle!=0)
	{
		::CloseHandle(m_hHandle);
	}

	m_hHandle = 0;
}


// --------------------------------------------------------------------------
// operator =
const ssMutex& ssMutex::operator = (const ssMutex & that)
{
	HANDLE temp = m_hHandle;	// Save original Handle
	if(this == &that)
	{
		return *this;			// Self assignment
	}

	Duplicate(that);			// Clone 

	if (temp!=NULL)
	{
		::CloseHandle(temp);	// Close the old handle
	}
	return *this;
}

//
// ********* ALL OTHER METHODS TO BE PLACED BELOW ***********
//

// --------------------------------------------------------------------------
// Aquires the mutex with a specific timeout.
BOOL ssMutex::Acquire(DWORD dwTimeout)
{
	BOOL bRetValue=FALSE;
	switch(::WaitForSingleObjectEx(m_hHandle, dwTimeout, TRUE))
	{
		case WAIT_OBJECT_0:
			m_dwLastResult = ERROR_SUCCESS;
			bRetValue=TRUE;
			break;
		case WAIT_TIMEOUT:
			m_dwLastResult = WAIT_TIMEOUT;
			break;
		case WAIT_ABANDONED:
			m_dwLastResult = WAIT_ABANDONED;
			break;
		case WAIT_IO_COMPLETION:
			m_dwLastResult = WAIT_IO_COMPLETION;
			break;
		case WAIT_FAILED:
			m_dwLastResult = ::GetLastError();
			break;
	}
	return bRetValue;
}


// --------------------------------------------------------------------------
// Releases	the mutex
BOOL ssMutex::Release()
{
	return ::ReleaseMutex(m_hHandle);
}


// --------------------------------------------------------------------------
// Returns results caused by the last wait operation.
DWORD ssMutex::GetResult() const
{
	return m_dwLastResult;
}


// --------------------------------------------------------------------------
// Provides compatibility with the Win32 API synchronization wait functions.
ssMutex::operator HANDLE() const
{
	return m_hHandle;
}


// --------------------------------------------------------------------------
// Duplicates object
void ssMutex::Duplicate( const ssMutex &that)
{
	m_bInheritable = that.m_bInheritable;
	m_dwLastResult = that.m_dwLastResult;

	if (that.m_hHandle==0)
	{
		return;
	}

	HANDLE hProcess = GetCurrentProcess();
	if (::DuplicateHandle(
				hProcess, that,				// original SyncMutexMultiProc
				hProcess, &m_hHandle,		// new handle
				STANDARD_RIGHTS_REQUIRED,	// Ignored, since cloning
				m_bInheritable,				// Inheritable
				DUPLICATE_SAME_ACCESS)==0)	// clone access
	{
		m_hHandle = 0;						// If call failed, make sure our handle is NULLed.
		m_dwLastResult = ::GetLastError();
	}
}
