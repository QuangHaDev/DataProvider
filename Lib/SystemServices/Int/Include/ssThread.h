/////////////////////////////////////////////////////////////////////////////
//          File:: ssThread.h
// Create Author:: Terence Lai
// Create   Date:: 15 Aug 1998
//
//   Description:: ssThread class declaration
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
#ifndef __ssThread_H__
#define __ssThread_H__

#include "ssCommon.h"
#include "ssCriticalSection.h"
#include "ssEvent.h"

/////////////////////////////////////////////////////////////////////////////
//
// Class      : ssThread
// Description: The ssThread abstract class encapsulates a WinNT Thread object.
//              Designing of a concrete thread requires over-riding the Main()
//              function and implementing a thread procedure.
//
// Methods:
//    ssThread()            - Constructs a thread object.
//    ~ssThread()           - Destructs a thread object.  Care must be taken to assure the thread has terminated.
//    Start()               - Start execution of the thread.
//    Suspend()             - Suspends exection of the thread.
//    Resume()              - Resumes exection of the thread.
//    SetThreadPriority()   - Sets the thread priority.
//    GetThreadPriority()   - Obtains the thread priority.
//    GetID()               - Gets the thread ID.
//    GetName()             - Gets the thread Name
//    SetName()             - Sets the thread Name
//    IsAlive()             - Determines whether the thread is alive
//    ExitThread()          - Trigger thread to exit
//    WaitForExit()         - Waits for the thread to exit with timeout.
//    GetResult()           - Returns the result of the last failed wait operation.
//    operator HANDLE()     - Exposes the handle of the syncrhonization object.
//    AckAlive()            - Acknowledge alive by child class
//

class ssThread
{
public:
	// ----------------------------------------------------------------------
	// Default Constructor
	ssThread(
		const char* pszThreadName = "",
		LPSECURITY_ATTRIBUTES pSecurityAttrs = 0,	// Security attributes
		UINT iStackSize = 0);						// StackSize

	// ----------------------------------------------------------------------
	// Destructor, Care must be taken to assure the thread has terminated.
	virtual ~ssThread();

	// ----------------------------------------------------------------------
	// Starts execution of the thread
	HANDLE Start();

	// ----------------------------------------------------------------------
	// Suspends the thread
	// - Returns the threads previous suspend count.
	DWORD Suspend() const;

	// ----------------------------------------------------------------------
	// Resumes the thread
	// - Returns the threads previous suspend count.
	DWORD Resume() const;

	// ----------------------------------------------------------------------
	// Sets the thread priority.
	BOOL SetPriority(const int iPriority) const;

	// ----------------------------------------------------------------------
	// Gets the thread priority.
	int GetPriority() const;

	// ----------------------------------------------------------------------
	// Returns thread ID.
	UINT GetID () const;

	// ----------------------------------------------------------------------
	// Returns thread name.
	const char* GetName () const;

	// ----------------------------------------------------------------------
	// Sets thread name.
	void SetName (char* pszThreadName);

	// ----------------------------------------------------------------------
	// Indicates whether the thread has started execution.
	BOOL HasStarted() const;

	// ----------------------------------------------------------------------
	// Waits for the concrete implementation to AckAlive with a specific timeout in m-sec
	// - Returns a flag indicating whether the thread has started.
	virtual BOOL IsAlive(DWORD dwTimeout = INFINITE); 

	// ----------------------------------------------------------------------
	// Trigger thread to exit
	virtual void ExitThread();

	// ----------------------------------------------------------------------
	// Returns a flag to indicate exit thread has been triggered
	BOOL IsExit();

	// ----------------------------------------------------------------------
	// Waits for the thread to exit with a specific timeout in m-sec
	// - Returns a flag indicating whether the thread has terminated.
	BOOL WaitForExit(DWORD dwTimeout = INFINITE); 

	// ----------------------------------------------------------------------
	// Returns a system error caused by the last wait operation.
	// WAIT_TIMEOUT     - Indicates the last wait operation has timed-out.
	// WAIT_ABANDONED   - Indicates the synchronization object of the last wait operation was abandoned.
	DWORD GetResult() const;

	// ----------------------------------------------------------------------
	// Provides compatibility with the Win32 API synchronization wait functions.
	// - Returns the handle.
	operator HANDLE() const;


protected:
	// ----------------------------------------------------------------------
	// For concrete implementation to acknowledge alive
	// The main function should monitor the CheckAliveEvent, and call the
	// AckAlive() if signaled
	void AckAlive();
	ssEvent m_CheckAliveEvent;

	// ----------------------------------------------------------------------
	// Event to wait up the main thread to exit
	ssEvent m_ExitEvent;

	// ----------------------------------------------------------------------
	// A deferred main implementation to be defined in the concrete thread class.
	// The Main loop should wait on 
	// - CheckAliveEvent for alive acknowledge
	// - ExitEvent to exit the main loop
	virtual void Main() = 0;

private:
	// ----------------------------------------------------------------------
	// Do not permit Copy Constructor. 
	ssThread(const ssThread& that);

	// ----------------------------------------------------------------------
	// Do not permit Operator =.
	const ssThread& operator = (const ssThread& that);

	// ----------------------------------------------------------------------
	// This static function is the actual starting execution point.
	static UINT APIENTRY ThreadEntryPoint(LPVOID lpParam);

	// ----------------------------------------------------------------------
	// Executes the user-defined Thread.
	void ExecuteUserThread();
	
	static ssCriticalSection	m_csThrdPtr;		// Mutal execlusion utilizes to protect thread pointer
	ssEvent						m_AckAliveEvent;	// Acknowkedge alive event
	BOOL						m_bStarted;			// Flag to indicate if thread has started execution
	BOOL						m_bExit;			// Flag to indicate thread should exit
	HANDLE						m_hHandle;			// thread Handle
	UINT						m_iID;				// thread ID 
	LPSECURITY_ATTRIBUTES		m_pSecurityAttrs;	// Security attributes
	UINT						m_iStackSize;		// Stack Size
	DWORD						m_dwLastResult;		// error from last wait operation
	char						m_szThreadName[SS_MAX_THREAD_NAME+1];	// Thread Name
};


#endif
