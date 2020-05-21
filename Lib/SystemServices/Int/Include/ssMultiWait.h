/////////////////////////////////////////////////////////////////////////////
//          File:: ssMultiWait.h
// Create Author:: Terence Lai
// Create   Date:: 13 Aug 1998
//
//   Description:: ssMultiWait class declaration
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
#ifndef __ssMultiWait_H__
#define __ssMultiWait_H__

#include "ssCommon.h"

/////////////////////////////////////////////////////////////////////////////
//
// Class      : ssMultiWait
// Description: The ssMultiWait class encapsulates the WaitForMultipleObjects API
// Methods:
//   ssMultiWait()    - Constructor
//   ~ssMultiWait()   - Destructor
//   Add()            - Adds a synchronization object to the synchronization array.
//   Wait()           - Waits for one or all synchronization objects to become signaled with a specified timeout.
//   GetResult()      - Returns a Win32 API system error due to the last wait operation.
//   IsValid()        - Indicates whether the handle of the internal synchronization object is valid.
//   Clear()          - Clears ALL synchronzation objects from the synchronization array.
//   SetPrioritized() - Sets the synchronization array o prioritized or non prioritized.
//   Entries()         - Returns the number of configured synchronization objects in the synchronization array.
//   GetAbandonedObject() - This function returns the enumerated value associated with the last abandoned synchronization object.
//

class ssMultiWait
{
public:
	// -----------------------------------------------------------------------
	// Default Constructor
	ssMultiWait(
		const BOOL bPrioritized = FALSE,// Do not prioritize the synchrnoinzation objects by default
		const BOOL bWaitAll = FALSE);	// Wait on one synchronization objects by default

	// -----------------------------------------------------------------------
	// Destructor
	virtual ~ssMultiWait();

	// -----------------------------------------------------------------------
	// Adds a synchronization object to the internal synchronization array and 
	// associates an enumeration to the object.
	// - Returns TRUE if successful.
	BOOL Add(
		const HANDLE hHandle,	// HANDLE to be added to the synchronization array
		const DWORD dwMapping);	// Enumerated value to identify the specific synchronization object.
								// 0 and 0xFFFFFFFF are reserved.

	// -----------------------------------------------------------------------
	// Waits for one or all synchronization objects to become signaled with a specified timeout in milliseconds
	// This method will not effect the internal default timeout.
	// - If waiting for any ONE synchronization object:
	//   Returns the enumerated mapping associated with the synchronization object that was signaled.
	// - If waiting for all synchronization objects:
	//   Returns 0. 
	// - Upon Error 
	//   Returns WAIT_FAILED    - Indicates a Win32 API system error occured. To get the extended error information, call GetResult().
	DWORD Wait(DWORD dwTimeout = INFINITE);

	// -----------------------------------------------------------------------
	// Returns a Win32 API system error due to the last wait operation.(refer to winerror.h)
	// - ERROR_SUCCESS
	// - ERROR_INVALID_HANDLE
	// - ERROR_OUT_OF_STRUCTURES
	// - ERROR_ALREADY_ASSIGNED
	// - ERROR_OUTOFMEMORY
	// - WAIT_TIMEOUT
	// - WAIT_ABANDONED
	// - WAIT_IO_COMPLETION
	DWORD GetResult() const;

	// -----------------------------------------------------------------------
	// Indicates whether the handle of the internal synchronization object is valid.
	BOOL IsValid() const; 

	// -----------------------------------------------------------------------
	// Clears ALL synchronzation objects from the synchronization array.
	void Clear();

	// -----------------------------------------------------------------------
	// Sets the synchronization array to prioritized or non prioritized.
	void SetPrioritized(BOOL bPrioritized = TRUE);

	// -----------------------------------------------------------------------
	// Returns the number of configured synchronization objects.
	DWORD Entries() const;

	// -----------------------------------------------------------------------
	// This function returns the enumerated value associated of the last 
	// synchronization object which was abandoned.
	// This function should ONLY be called after a user receives a WAIT_ABANDONED
	DWORD GetAbandonedObject() const;

private:
	
	// -----------------------------------------------------------------------
	// Do not permit Copy Constructor.
	ssMultiWait(const ssMultiWait& that);

	// -----------------------------------------------------------------------
	// Do no permit Operator =. 
	const ssMultiWait& operator = (const ssMultiWait& that); 

	enum	{MAX_SYNC_OBJECT_HANDLES=64};
	HANDLE*	m_phHandleArrayPrimary;		// pointer to the start of primary section of handle array
	HANDLE*	m_phHandleArraySecondary;	// pointer to the start of secondary section of handle array
	HANDLE*	m_phCurrentHandle;			// pointer to current handle array, used for priorization
	DWORD	m_dwNumHandles;				// Number of handles in the array
	DWORD*	m_pdwMapArrayPrimary;		// pointer to the start of the primary section mapping array
	DWORD*	m_pdwMapArraySecondary;		// pointer to the start of the secondary section of mapping array
	DWORD*	m_pdwCurrentPosition;		// pointer current position in the handle array, used for priorization
	DWORD	m_dwAbandonedObject;			// abandoned object after wait operation 
	DWORD	m_dwLastError;				// error from last wait operation
	BOOL	m_bIsValid;					// flag for valid status
	BOOL	m_bWaitAll;					// wait for all synchronization objects to become signaled.
	BOOL	m_bPrioritized;				// Indicates whether the wait mode is operating in prioritized or non-prioritized	
};

#endif
