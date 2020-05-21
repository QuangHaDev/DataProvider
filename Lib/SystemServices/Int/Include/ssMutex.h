/////////////////////////////////////////////////////////////////////////////
//          File:: ssMutex.h
// Create Author:: Terence Lai
// Create   Date:: 13 Aug 1998
//
//   Description:: ssMutex class declaration
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
#ifndef __ssMutex_H__
#define __ssMutex_H__

#include "ssCommon.h"

/////////////////////////////////////////////////////////////////////////////
//
// Class      : ssMutex
// Description: The ssMutex class encapsulates a Win32 Event object.
// Methods:
//   Default Constructor, destructor, copy constructor, overloaded assignment operator
//   Optional constructor	- Provides full functionality of the mutex object.
//   Acquire()				- Blocks until the mutex is acquired with a specified timeout.
//   Release()				- Releases the mutex.
//   GetResult()			- Returns the result of the last failed wait operation.
//   operator HANDLE()		- Exposes the handle of the syncrhonization object.
//

class ssMutex
{
public:
	// ----------------------------------------------------------------------
	// Default Constructor
	ssMutex(
		BOOL  bInitOwned = FALSE,			// Indicates whether initially owned
		const LPSECURITY_ATTRIBUTES lpsa = 0,	// Pointer to security attributes,
		const char* szName = "");				// ssMutex name.

	// ----------------------------------------------------------------------
	// Copy Constructor.
	ssMutex(const ssMutex& that);

	// ----------------------------------------------------------------------
	// Destructor
	virtual ~ssMutex();

	// ----------------------------------------------------------------------
	// Operator =.
	const ssMutex& operator = (const ssMutex& that);

	// ----------------------------------------------------------------------
	// Releases the mutex
	// - Returns a flag indicating whether the mutex was released.
	BOOL Release();

	// ----------------------------------------------------------------------
	// Aquires the mutex with a specific timeout in m-sec (default INIFINITE)
	// - Returns a flag indicating whether the SyncMutexMultiProc was aquired.
	BOOL Acquire(DWORD dwTimeout = INFINITE);

	// ----------------------------------------------------------------------
	// Returns a system error or one of the specified results caused by the last wait operation.
	// ERROR_SUCCESS    - successful
	// WAIT_TIMEOUT     - last wait operation has timed-out.
	// WAIT_ABANDONED   - last wait operation was abandoned.
	// WAIT_IO_COMPLETION - One or more I/O completion routines are queued for execution
	DWORD GetResult() const;

	// ----------------------------------------------------------------------
	// Provides compatibility with the Win32 API synchronization wait functions.
	// - Returns the handle.
	operator HANDLE() const;

private:
	// ----------------------------------------------------------------------
	// Duplicates 
	void Duplicate( const ssMutex& that);

	HANDLE	m_hHandle;			// handle of ssMutex
	BOOL	m_bInheritable;		// indicates whether inheritable	
	DWORD	m_dwLastResult;		// error from last wait operation
};

#endif
