/////////////////////////////////////////////////////////////////////////////
//          File:: ssCriticalSection.h
// Create Author:: Terence Lai
// Create   Date:: 15 Aug 1998
//
//   Description:: ssCriticalSection class member function
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
#include "ssCriticalSection.h"

// -------------------------------------------------------------------------
// Default Constructor
ssCriticalSection::ssCriticalSection()
{
	::InitializeCriticalSection(&m_CriticalSection);
}


// -------------------------------------------------------------------------
// Destructor
ssCriticalSection::~ssCriticalSection()
{
	::DeleteCriticalSection(&m_CriticalSection);
}


//
// ********* ALL OTHER METHODS TO BE PLACED BELOW ***********
//

// -------------------------------------------------------------------------
// Enter the critical section
void ssCriticalSection::Enter()
{
	::EnterCriticalSection(&m_CriticalSection);
}


// -------------------------------------------------------------------------
// Try to Enter the critical section
BOOL ssCriticalSection::TryEnter()
{
#if (_WIN32_WINNT >= 0x0500)
	return ::TryEnterCriticalSection(&m_CriticalSection);
#else
	::EnterCriticalSection(&m_CriticalSection);
	return TRUE;
#endif
}

// -------------------------------------------------------------------------
// Leave the critical section
void ssCriticalSection::Leave()
{
	::LeaveCriticalSection(&m_CriticalSection);
}

int ssCriticalSection::LeaveAll()
{
    int nRc = 0;
    DWORD dwTID = GetCurrentThreadId();
    if ((DWORD)m_CriticalSection.OwningThread == dwTID)
    {
        while(m_CriticalSection.RecursionCount > 0)
        {
            Leave();
            nRc++;
        }
    }
    return nRc;
}
