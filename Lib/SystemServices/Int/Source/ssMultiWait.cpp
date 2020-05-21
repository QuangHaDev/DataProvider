/////////////////////////////////////////////////////////////////////////////
//          File:: ssMultiWait.h
// Create Author:: Terence Lai
// Create   Date:: 15 Aug 1998
//
//   Description:: ssMulitWait class member function
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
#include "ssMultiWait.h"

// --------------------------------------------------------------------------
// Default construction
ssMultiWait::ssMultiWait(
			const BOOL bPrioritized,
			const BOOL bWaitAll) :
	m_phHandleArrayPrimary(0),
	m_phHandleArraySecondary(0),
	m_phCurrentHandle(0),
	m_dwNumHandles(0),
	m_pdwMapArrayPrimary(0),
	m_pdwMapArraySecondary(0),
	m_pdwCurrentPosition(0),
	m_dwAbandonedObject(0),
	m_dwLastError(ERROR_SUCCESS),
	m_bIsValid(FALSE),
	m_bWaitAll(bWaitAll),
	m_bPrioritized(bPrioritized)
{
}


// --------------------------------------------------------------------------
// Destructor
ssMultiWait::~ssMultiWait()
{
	delete [] m_phHandleArrayPrimary;	m_phHandleArrayPrimary=0;
	delete [] m_pdwMapArrayPrimary;		m_pdwMapArrayPrimary=0;
}

//
// ********* ALL OTHER METHODS TO BE PLACED BELOW ***********
//

// --------------------------------------------------------------------------
// Adds the handle and associates the enumerated map.
// 
// Comments: The handle will be added to two locations in the 
//           syncrhonization array to provide non-priorization if utilized.
//
BOOL ssMultiWait::Add(const HANDLE hHandle, const DWORD dwMapping)
{
	DWORD   dwCount;
	HANDLE*	phHandle;
	HANDLE*	phNewHandleArrayPrimary;
	HANDLE*	phNewHandleArraySecondary;
	HANDLE*	phNewHandleArrayLast;
	DWORD*	pdwNewMapArrayPrimary;
	DWORD*	pdwNewMapArraySecondary;
	DWORD*	pdwNewMapArrayLast;

	// Check for maximum handles
	if(m_dwNumHandles >= MAX_SYNC_OBJECT_HANDLES)
	{
		m_dwLastError = ERROR_OUT_OF_STRUCTURES;
		return FALSE;											
	}

	// Check for redundant handles
	phHandle = m_phHandleArrayPrimary;
	while(phHandle != m_phHandleArraySecondary)
	{
		if(hHandle == (*phHandle))
		{
			m_dwLastError = ERROR_ALREADY_ASSIGNED;
			return FALSE;
		}
		phHandle++;
	}
	
	if((phNewHandleArrayPrimary = new HANDLE[2*(m_dwNumHandles+1)]) == 0)
	{
		m_dwLastError = ERROR_OUTOFMEMORY;
		return FALSE;
	}
	if((pdwNewMapArrayPrimary = new DWORD[2*(m_dwNumHandles+1)]) == 0)
	{
		delete [] phNewHandleArrayPrimary; phNewHandleArrayPrimary=0;
		m_dwLastError = ERROR_OUTOFMEMORY;
		return FALSE;
	}

	// Copy all handles and insert new handle
	if ((m_dwNumHandles) && (m_phHandleArrayPrimary))
	{
//		phNewHandleArraySecondary = (HANDLE*)memcpy(phNewHandleArrayPrimary, m_phHandleArrayPrimary, m_dwNumHandles*sizeof(HANDLE*));
				phNewHandleArraySecondary = (HANDLE*)memcpy(phNewHandleArrayPrimary, m_phHandleArrayPrimary, m_dwNumHandles*sizeof(HANDLE));
	}
	else
	{
		phNewHandleArraySecondary = phNewHandleArrayPrimary ;
	}

	dwCount = m_dwNumHandles;
	while(dwCount != 0)
	{
		phNewHandleArraySecondary++;
		dwCount--;
	}
	*phNewHandleArraySecondary = hHandle;	// Insert new handle
	phNewHandleArraySecondary++;
	if ((m_dwNumHandles) && (m_phHandleArrayPrimary))
	{
		//phNewHandleArrayLast = (HANDLE*)memcpy(phNewHandleArraySecondary, m_phHandleArrayPrimary, m_dwNumHandles*sizeof(HANDLE*));
		phNewHandleArrayLast = (HANDLE*)memcpy(phNewHandleArraySecondary, m_phHandleArrayPrimary, m_dwNumHandles*sizeof(HANDLE));
	}
	else
	{
		phNewHandleArrayLast = phNewHandleArraySecondary ;
	}

	dwCount = m_dwNumHandles;
	while(dwCount != 0)
	{
		phNewHandleArrayLast++;
		dwCount--;
	}
	*phNewHandleArrayLast = hHandle;		// Insert new handle

	// Copy all mappings and insert new mapping
	if ((m_dwNumHandles) && (m_pdwMapArrayPrimary))
	{
		//pdwNewMapArraySecondary = (DWORD*)memcpy(pdwNewMapArrayPrimary, m_pdwMapArrayPrimary, m_dwNumHandles*sizeof(DWORD*));
		pdwNewMapArraySecondary = (DWORD*)memcpy(pdwNewMapArrayPrimary, m_pdwMapArrayPrimary, m_dwNumHandles*sizeof(DWORD));
	}
	else
	{
		pdwNewMapArraySecondary = pdwNewMapArrayPrimary ;
	}

	dwCount = m_dwNumHandles;
	while(dwCount != 0)
	{
		pdwNewMapArraySecondary++;
		dwCount--;
	}
	*pdwNewMapArraySecondary = dwMapping;	// Insert new mapping
	pdwNewMapArraySecondary++;

	if ((m_dwNumHandles) && (m_pdwMapArrayPrimary))
	{
		//pdwNewMapArrayLast = (DWORD*)memcpy(pdwNewMapArraySecondary, m_pdwMapArrayPrimary, m_dwNumHandles*sizeof(DWORD*));
		pdwNewMapArrayLast = (DWORD*)memcpy(pdwNewMapArraySecondary, m_pdwMapArrayPrimary, m_dwNumHandles*sizeof(DWORD));
	}
	else
	{
		pdwNewMapArrayLast = pdwNewMapArraySecondary ;
	}
	dwCount = m_dwNumHandles;
	while(dwCount != 0)
	{
		pdwNewMapArrayLast++;
		dwCount--;	
	}
	*pdwNewMapArrayLast = dwMapping;		// Insert new mapping

	delete [] m_phHandleArrayPrimary;
	delete [] m_pdwMapArrayPrimary;
	m_phHandleArrayPrimary = phNewHandleArrayPrimary;
	m_phHandleArraySecondary = phNewHandleArraySecondary;
	m_pdwMapArrayPrimary = pdwNewMapArrayPrimary;
	m_pdwMapArraySecondary = pdwNewMapArraySecondary;
	m_dwNumHandles++;
	m_bIsValid = TRUE;
	m_pdwCurrentPosition = m_pdwMapArrayPrimary;
	m_phCurrentHandle = m_phHandleArrayPrimary;
	return TRUE;
}


// --------------------------------------------------------------------------
// Waits for one or all synchronization objects to become signaled with a specified timeout.
DWORD ssMultiWait::Wait(DWORD dwTimeout)
{
	DWORD	dwResult;
	DWORD*	pdwCurrentPosition;

	if(m_bIsValid == FALSE)
	{
		m_dwLastError = ERROR_INVALID_HANDLE;
		return WAIT_FAILED;
	}
	dwResult = ::WaitForMultipleObjectsEx(m_dwNumHandles , m_phCurrentHandle, m_bWaitAll, dwTimeout, TRUE); 

	pdwCurrentPosition = m_pdwCurrentPosition;	// Maintain a local copy
	if(m_bPrioritized == FALSE)
	{
		m_phCurrentHandle++;
		m_pdwCurrentPosition++;
		if(m_phCurrentHandle == m_phHandleArraySecondary)
		{
			m_phCurrentHandle = m_phHandleArrayPrimary;	// Reset the pointer to begining of handle array
			m_pdwCurrentPosition = m_pdwMapArrayPrimary;	// Reset the pointer to beginning of map array
		}
	}

	if((dwResult >= WAIT_OBJECT_0) && (dwResult < (WAIT_OBJECT_0+m_dwNumHandles)))
	{
		m_dwLastError = ERROR_SUCCESS;
		return(pdwCurrentPosition[(dwResult-WAIT_OBJECT_0)]);
	}

	if((dwResult >= WAIT_ABANDONED_0) && (dwResult < (WAIT_ABANDONED_0+m_dwNumHandles)))
	{
		m_dwAbandonedObject = (pdwCurrentPosition[(dwResult-WAIT_ABANDONED_0)]);
		m_dwLastError = WAIT_ABANDONED;
		return WAIT_FAILED;
	}

	switch (dwResult)
	{
		case WAIT_TIMEOUT:
			m_dwLastError = WAIT_TIMEOUT;
			break;
		case WAIT_IO_COMPLETION:
			m_dwLastError = WAIT_IO_COMPLETION;
			break;
		case WAIT_FAILED:
		default:
			m_dwLastError = ::GetLastError();
			break;
	}
	return WAIT_FAILED;
}


// --------------------------------------------------------------------------
// Returns a Win32 API system error due to the last wait operation.
DWORD ssMultiWait::GetResult() const
{
	return m_dwLastError;
}


// --------------------------------------------------------------------------
// Indicates whether the handle of the internal synchronization object is valid.
BOOL ssMultiWait::IsValid() const
{
	return(m_bIsValid);
}


// --------------------------------------------------------------------------
// Clears ALL synchronzation objects from the synchronization array.
void ssMultiWait::Clear()
{
	m_bIsValid = FALSE;
	m_dwNumHandles = 0;
	m_phCurrentHandle = 0;
	delete [] m_phHandleArrayPrimary;	m_phHandleArrayPrimary = 0;
	delete [] m_pdwMapArrayPrimary;		m_pdwMapArrayPrimary = 0;
	m_phHandleArrayPrimary = m_phHandleArraySecondary;
	m_pdwMapArrayPrimary = m_pdwMapArraySecondary;
}


// --------------------------------------------------------------------------
// Sets the synchronization object to prioritized or non prioritized.
void ssMultiWait::SetPrioritized(BOOL bPrioritized)
{
	m_bPrioritized = bPrioritized;
	m_pdwCurrentPosition = m_pdwMapArrayPrimary;
	m_phCurrentHandle = m_phHandleArrayPrimary;
}


// --------------------------------------------------------------------------
// Returns the number of configured synchronization objects.
DWORD ssMultiWait::Entries() const
{
	return m_dwNumHandles;
}


// --------------------------------------------------------------------------
// Returns the enumerated value associated with the last synchronization object which was abandoned.
DWORD ssMultiWait::GetAbandonedObject() const
{
	return(m_dwAbandonedObject);
}
