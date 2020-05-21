/////////////////////////////////////////////////////////////////////////////
//          File:: ssEvent.h
// Create Author:: Terence Lai
// Create   Date:: 13 Aug 1998
//
//   Description:: ssEvent class declaration
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
#ifndef __ssEvent_H__
#define __ssEvent_H__

#include "ssCommon.h"

/////////////////////////////////////////////////////////////////////////////
//
// Class      : ssEvent
// Description: The ssEvent class encapsulates a Win32 Event object.
// Methods:
//   Default Constructor, destructor, copy constructor, overloaded assignment operator
//   Optional constructor	- Provides full functionality of the event object.
//   Set()					- Set the event.
//   Reset()				- Resets the event
//   Wait()					- Blocks until the event becomes signaled with specified timeout. 
//   GetResult()			- Returns the result of the last failed wait operation.
//   operator HANDLE()		- Exposes the handle of the syncrhonization object.
//

class ssEvent
{
public:
	// ------------------------------------------------------------------------
	// Instantiates an event object.
	// - Default creates internal HANDLE.
	// - Uses automatic reset. 
	// - Initial state is non-signaled.
	// - No Security attributes
	// - No name.
	ssEvent();

	// ------------------------------------------------------------------------
	// Optional Constructor. Instantiates an event object.
	ssEvent(
		BOOL  bManualReset,					// Manual or Automatic reset Event
		BOOL  bInitialState=FALSE,			// Event is initially signaled
		const LPSECURITY_ATTRIBUTES lpsa=0,	// Pointer to security attributes,
		const char* szName="");				// Event name.

	// ------------------------------------------------------------------------
	// Copy Constructor
	ssEvent(const ssEvent& that);

	// ------------------------------------------------------------------------
	// Destructor
	virtual ~ssEvent();

	// ------------------------------------------------------------------------
	// Operator =
	const ssEvent& operator = (const ssEvent& that);

	// ------------------------------------------------------------------------
	// Sets the Event
	// - Returns a flag indicating whether the Event was set. 
	BOOL Set() const;

	// ------------------------------------------------------------------------
	// Re-sets the Event
	// - Returns a flag indicating whether the Event was re-set. 
	BOOL Reset() const;

	// ------------------------------------------------------------------------
	// Waits for the Event to become signaled with a specific timeout in m-sec (default INFINITE)
	// - Returns a flag indicating whether the Event was signaled.
	BOOL Wait(DWORD dwTimeout = INFINITE);

	// ------------------------------------------------------------------------
	// Returns a system error caused by the last wait operation.
	// ERROR_SUCCESS    - successful
	// WAIT_TIMEOUT     - last wait operation has timed-out.
	// WAIT_ABANDONED   - last wait operation was abandoned.
	// WAIT_IO_COMPLETION - One or more I/O completion routines are queued for execution
	DWORD GetResult() const;

	// ------------------------------------------------------------------------
	// Provides compatibility with the Win32 API synchronization wait functions.
	// - Returns the handle.
	operator HANDLE() const; 

private:
	// ------------------------------------------------------------------------
	// Duplicates the object
	void Duplicate( const ssEvent &that);

	BOOL	m_bInheritable;
	HANDLE	m_hHandle;
	DWORD	m_dwLastResult;
};

#endif
