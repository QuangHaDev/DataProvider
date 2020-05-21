/////////////////////////////////////////////////////////////////////////////
//          File:: ssCriticalSection.h
// Create Author:: Terence Lai
// Create   Date:: 15 Aug 1998
//
//   Description:: ssCriticalSection class declaration
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
#ifndef __ssCriticalSection_H__
#define __ssCriticalSection_H__

#include "ssCommon.h"

/////////////////////////////////////////////////////////////////////////////
//
// Class      : ssCriticalSection
// Description: The ssCriticalSection encapsulates the CRITICAL_SECTION into a class,
//              which automatically perform initialization, but copy construction 
//              and assignment are not allowed.
//
// Methods:
//   Default Constructor, destructor, 
//   Enter()      - Enter the critical section
//   TryEnter()   - Try to Enter the critical section
//   Leave()      - Leave the critical section
//                  Cautions :
//                  If a thread calls Leave() when it does not have ownership
//                  of the specified critical section object, an error occurs
//                  that may cause another thread using Enter to wait indefinitely.

class ssCriticalSection
{
public:
	// ----------------------------------------------------------------------
	// Default Constructor
	ssCriticalSection();

	// ----------------------------------------------------------------------
	// Destructor
	virtual ~ssCriticalSection();

	// ----------------------------------------------------------------------
	// Enter the critical section
	void Enter();

	// ----------------------------------------------------------------------
	// Try to Enter the critical section
	BOOL TryEnter();

	// ----------------------------------------------------------------------
	// Leave the critical section
	void Leave();

	// ----------------------------------------------------------------------
	// Leave the critical section no matter how many Enter() is called.
    // return number of Leave() called
    // eg. use this function for catch() function (try() ... catch() ...)
    int LeaveAll();

private:
	// ----------------------------------------------------------------------
	// Copy Constructor not permitted
	ssCriticalSection(const ssCriticalSection& that);

	// ----------------------------------------------------------------------
	// Operator = not permitted
	const ssCriticalSection& operator=(const ssCriticalSection& that);

	CRITICAL_SECTION m_CriticalSection;
};

#endif
