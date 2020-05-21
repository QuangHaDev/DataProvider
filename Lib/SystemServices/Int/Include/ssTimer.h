/////////////////////////////////////////////////////////////////////////////
//          File:: ssTimer.h
// Create Author:: Terence Lai
// Create   Date:: 15 Aug 1998
//
//   Description:: ssTimer class declaration
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
#ifndef __ssTimer_H__
#define __ssTimer_H__

#include "ssCommon.h"

/////////////////////////////////////////////////////////////////////////////
//
// Class      : ssTimer
// Description: The ssTimer class encapsulates a Win32 Waitable Timer object.
// Methods:
//   Default Constructor, destructor
//   Optional constructor	- Provides full functionality of the Timer object.
//   Set()					- Sets the timer with optional completion routine
//   Reset()				- Resets the timer
//   Wait()					- Blocks until the event becomes signaled with specified timeout. 
//   GetResult()			- Returns the result of the last failed wait operation.
//   operator HANDLE()		- Exposes the handle of the syncrhonization object.
//

class ssTimer
{
public:
	// ------------------------------------------------------------------------
	// Optional Constructor. Instantiates an Timer object.
	ssTimer(
		DWORD  dwPeriod = 1000,					// periodic timer interval in m-sec
												// If lPeriod is zero, the timer is signaled once 
		PTIMERAPCROUTINE pCompletionRoutine = 0,// pointer completion routine
		LPVOID pArgToCompletionRoutine = 0,		// pointer to structure that pass to completion routine
		BOOL   bResume = FALSE,					// Resume from suspend mode
		BOOL   bManualReset = FALSE,			// Manual or Automatic reset Event
		const  LPSECURITY_ATTRIBUTES lpsa=0,	// Pointer to security attributes,
		const  char* szName="");				// Event name.

	ssTimer(
		const LARGE_INTEGER * pDueTime,			// when timer will become signaled
		DWORD  dwPeriod = 1000,					// periodic timer interval in m-sec
												// If lPeriod is zero, the timer is signaled once 
		PTIMERAPCROUTINE pCompletionRoutine = 0,// pointer completion routine
		LPVOID pArgToCompletionRoutine = 0,		// pointer to structure that pass to completion routine
		BOOL   bResume = FALSE,					// Resume from suspend mode
		BOOL   bManualReset = FALSE,			// Manual or Automatic reset Event
		const  LPSECURITY_ATTRIBUTES lpsa=0,	// Pointer to security attributes,
		const  char* szName="");				// Event name.
	// ------------------------------------------------------------------------
	// Destructor
	virtual ~ssTimer();

	// ------------------------------------------------------------------------
	// Sets the Timer singnal interval
	// - Returns a flag indicating whether the Timer was set. 
	BOOL Set(
		DWORD  dwPeriod,							// periodic timer interval in m-sec
													// If lPeriod is zero, the timer is signaled once 
		PTIMERAPCROUTINE pCompletionRoutine = 0,	// pointer completion routine
		LPVOID pArgToCompletionRoutine = 0,			// pointer to structure that pass to completion routine
		BOOL bResume = FALSE);						// Resume from suspend mode

	BOOL Set(
		const LARGE_INTEGER * pDueTime,				// when timer will become signaled
		DWORD  dwPeriod,							// periodic timer interval in m-sec
													// If lPeriod is zero, the timer is signaled once 
		PTIMERAPCROUTINE pCompletionRoutine = 0,	// pointer completion routine
		LPVOID pArgToCompletionRoutine = 0,			// pointer to structure that pass to completion routine
		BOOL bResume = FALSE);						// Resume from suspend mode


	// ------------------------------------------------------------------------
	// Re-sets the timer
	// - Returns a flag indicating whether the Timer was re-set. 
	BOOL Reset() const;

	// ------------------------------------------------------------------------
	// Waits for the Timer to become signaled 
	// - Returns a flag indicating whether the Event was signaled.
	BOOL Wait();

	// ------------------------------------------------------------------------
	// Returns a system error caused by the last wait operation.
	// WAIT_TIMEOUT     - last wait operation has timed-out.
	// WAIT_ABANDONED   - last wait operation was abandoned.
	DWORD GetResult() const;

	// ------------------------------------------------------------------------
	// Provides compatibility with the Win32 API synchronization wait functions.
	// - Returns the handle.
	operator HANDLE() const; 

private:
	// ------------------------------------------------------------------------
	// Copy Constructor not permitted
	ssTimer(const ssTimer& that);

	// ------------------------------------------------------------------------
	// Operator = not permitted
	const ssTimer& operator = (const ssTimer& that);

	HANDLE	m_hHandle;
	DWORD	m_dwLastResult;
};

#endif
