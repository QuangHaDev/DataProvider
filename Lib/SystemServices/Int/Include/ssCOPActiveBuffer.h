/////////////////////////////////////////////////////////////////////////////
//          File:: ssCOPActiveBuffer.h
// Create   Date:: Aug 15 1996
// Create Author:: Joseph Payne
//
//   Description:: Active COP Buffer Class Declaration
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000 by AFE Solutions Ltd.
// All rights reserved.
//
// No portion of this software in any form may be used or reproduced in 
// any manner without written consent from AFE Solutions Ltd.
//
#ifndef	__COPActiveBuffer_H__
#define	__COPActiveBuffer_H__

#include "ssThread.h"
#include "ssEvent.h"
#include "ssMultiWait.h"
#include "ssCOPBuffer.h"

/***************************************************************************************/
/***************************************************************************************/ 

//	CLASS DECLARATIONS

/***************************************************************************************/
/***************************************************************************************/    

/////////////////////////////////////////////////////////////////////////////
//
//	class:			ssCOPActiveBuffer
//	Description:	Abstract active buffer class. Override this class and
//					implement ProcessCOP() to handle COP's once they are
//					placed into the buffer.
//
//	Methods:
//
#define DEFAULT_SS_COP_ACTIVE_BUFFER_TERMINATE_TIMEOUT 10000 // 10 seconds
template <class T> class ssCOPActiveBuffer : public ssThread
{
public:
	// -------------------------------------------------------------------------
	// Default Constructor
	ssCOPActiveBuffer(UINT upper_bound = DEFAULT_SS_COPBUFFER_UPPER_BOUND, 
						UINT increment   = DEFAULT_SS_COPBUFFER_INCREMENT,
						UINT upper_limit = DEFAULT_SS_COPBUFFER_UPPER_LIMIT);

	// -------------------------------------------------------------------------
	// Check the thread is alive
	virtual BOOL IsAlive(DWORD dwTimeout);

	// -------------------------------------------------------------------------
	// Pause the thread into wait state
	BOOL Pause();

	// -------------------------------------------------------------------------
	// Resume the paused thread from wait state
	BOOL Continue();

	// -------------------------------------------------------------------------
	// Terminate the thread
	virtual void ExitThread();

	// -------------------------------------------------------------------------
	// Access to the buffer
	ssCOPBuffer<T>& operator()();

	// -------------------------------------------------------------------------
	// Destructor
	virtual	~ssCOPActiveBuffer() {}

protected:
	// -------------------------------------------------------------------------
	// COP Buffer
	ssCOPBuffer<T> m_Buffer;

	// -------------------------------------------------------------------------
	// Terminate event
	//ssEvent m_EventTerminate;
	BOOL m_bTerminateThread;

	// -------------------------------------------------------------------------
	// Check Alive event
	//ssEvent m_CheckAlive;
	//ssEvent m_Alive;
	BOOL m_bCheckAlive;

	// -------------------------------------------------------------------------
	// Pause Event
	ssEvent m_Continue;
	BOOL m_bPause;

	// -------------------------------------------------------------------------
	// Multi-Wait
	ssMultiWait m_MultiWait;

	// -------------------------------------------------------------------------
	// Override of abstract Main()
	virtual void Main();

	// -------------------------------------------------------------------------
	// Abstract method to override which provides the stategy of WHAT to do 
	// with the COP once one is retrieved from the buffer.
	virtual void ProcessCOP(ssCountedObjPtr<T>& cop) = 0;

	// -------------------------------------------------------------------------
	// Abstract method to override which provides the stategy of WHAT to do 
	// if the COP buffer becomes empty, or active thread going to terminate
	virtual void ProcessDone() {}

	// ----------------------------------------------------------------------
	// Override to handle the error		
	virtual void OnError(DWORD dwError)
	{
/*		switch (dwError)
		{
			case ERROR_SUCCESS:
			case ERROR_INVALID_HANDLE:
			case ERROR_OUT_OF_STRUCTURES:
			case ERROR_ALREADY_ASSIGNED:
			case ERROR_OUTOFMEMORY:
			case WAIT_TIMEOUT:
			case WAIT_ABANDONED:
			case WAIT_IO_COMPLETION:
				do something;
				break;
			default:
				do something;
				break;
		}
*/		
#ifdef _DEBUG
		MessageBox(NULL, "MulitWaitFailed", "ActiveCOPBuffer", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
#endif
		m_bTerminateThread = TRUE;
	}

	// ----------------------------------------------------------------------
	// User defined events - Return FALSE if defined EventMap has been used by this class
	BOOL DefineUserEvent(HANDLE hHandle, DWORD dwEventMap);

	// ----------------------------------------------------------------------
	// Override to execute user defined event handling
	virtual void OnUserDefinedEvent(DWORD dwEventMap)	{}


	enum {

		COP_IN_BUFFER =						1,
		CHECK_ALIVE =						2,
		TERMINATE_SELF =					3,
		LAST_ENUM_IN_SS_COPACTIVEBUFFER =	4,
		// following enum reserved for future expansion
		Reserved05 =						5,
		Reserved06 =						6,
		Reserved07 =						7,
		Reserved08 =						8,
		Reserved09 =						9,
		Reserved10 =						10,
	};

private:
	// -------------------------------------------------------------------------
	// Copy Constructor - not allowed to use
	ssCOPActiveBuffer(const ssCOPActiveBuffer& that);

	// -------------------------------------------------------------------------
	// Operator = - not allowed to use
	const	ssCOPActiveBuffer&	operator = (const ssCOPActiveBuffer& that);

};

//
//	public inline functions:
//
	// -------------------------------------------------------------------------
	// Default Constructor
	template <class T>
	inline ssCOPActiveBuffer<T>::ssCOPActiveBuffer(UINT upper_bound, 
		UINT increment, UINT upper_limit) : 
		ssThread(), m_Buffer(upper_bound,increment,upper_limit),
		m_bTerminateThread(FALSE), m_bCheckAlive(FALSE), m_bPause(FALSE)
	{
	}

	// -------------------------------------------------------------------------
	// Check internal thread is alive 
	template <class T>
	inline BOOL ssCOPActiveBuffer<T>::IsAlive(DWORD dwTimeout = INFINITE)
	{
		m_bCheckAlive = TRUE;	// break the ProcessCOP loop
		//m_CheckAlive.Set();		// signal the check alive event
		m_Continue.Set();		// signal the thread if it is paused
		return(ssThread::IsAlive(dwTimeout));
		//return(m_Alive.Wait(iTimeout));
	}

	// -------------------------------------------------------------------------
	// Pause internal thread to wait state
	template <class T>
	inline BOOL ssCOPActiveBuffer<T>::Pause()
	{
		m_Continue.Reset();		// in case already set by CheckAlive()
		m_bPause = TRUE;		// break the ProcessCOP loop
		return(TRUE);
	}

	// -------------------------------------------------------------------------
	// Resume internal thread 
	template <class T>
	inline BOOL ssCOPActiveBuffer<T>::Continue()
	{
		m_bPause = FALSE;			// break the ProcessCOP loop
		return(m_Continue.Set());	// Resume the thread
	}

	// -------------------------------------------------------------------------
	// Terminate the thread
	template <class T>
	inline void ssCOPActiveBuffer<T>::ExitThread()
	{
		m_bTerminateThread = TRUE;	// break the ProcessCOP loop if required
		ssThread::ExitThread();		// signal the terminate event
		m_Continue.Set();			// signal paused thread if required, EventTerminate must be set befor this.
		//return(TRUE);
	}

	// -------------------------------------------------------------------------
	// Access to the buffer
	template <class T>
	inline ssCOPBuffer<T>& ssCOPActiveBuffer<T>::operator()()
	{
		return m_Buffer;
	}

//
//	protected inline functions:
//

	// -------------------------------------------------------------------------
	// Main loop of thread
	template <class T>
	inline void ssCOPActiveBuffer<T>::Main()
	{
		ssCountedObjPtr<T> cop;	  // COP used to do retrievals from the buffer
		ssCountedObjPtr<T> empty_cop;	  // COP used to do retrievals from the buffer

		m_MultiWait.Add(m_Buffer.GetEvent(), COP_IN_BUFFER);
		m_MultiWait.Add(ssThread::m_CheckAliveEvent, CHECK_ALIVE);
		m_MultiWait.Add(ssThread::m_ExitEvent, TERMINATE_SELF);

		while(!m_bTerminateThread)
		{
			DWORD dwEventMap = m_MultiWait.Wait();
			switch(dwEventMap)
			{
				case COP_IN_BUFFER :
				{
					if ((!m_bPause) && (!m_bTerminateThread))
					{
						while(m_Buffer.Remove(cop))
						{
							ProcessCOP(cop);

							if (m_bCheckAlive || m_bPause || m_bTerminateThread)
								break;		// break the loop
						}

						// release the p_body in the copy
						cop = empty_cop;

						if ((!m_bCheckAlive) && (!m_bPause))
							ProcessDone();
					}
					if ((m_bPause) && (!m_bCheckAlive) && (!m_bTerminateThread))
						m_Continue.Wait();	// pause the thread

					break;
				}
				case CHECK_ALIVE :
				{
					m_bCheckAlive = FALSE;
					//m_Alive.Set();			// Alive Signal
					ssThread::AckAlive();
					break;
				}
					
				case TERMINATE_SELF :
				{
					m_bTerminateThread = TRUE;
					break;
				}
				case WAIT_FAILED :	// Problem in MultiWait, use on OnError() virtual function to handle the error
				{
					//TL
					OnError(m_MultiWait.GetResult());
					break;
				}
				default :  // User Defined Events
				{
					OnUserDefinedEvent(dwEventMap);
					break;
				}
			}
		}
	}



	template <class T>
	inline BOOL ssCOPActiveBuffer<T>::DefineUserEvent(HANDLE hHandle, DWORD dwEventMap)
	{
		switch (dwEventMap)
		{
			case COP_IN_BUFFER 	:		
			case CHECK_ALIVE	:		
			case TERMINATE_SELF	:
			case LAST_ENUM_IN_SS_COPACTIVEBUFFER :
			case Reserved05 	:
			case Reserved06 	:
			case Reserved07 	:
			case Reserved08 	:
			case Reserved09 	:
			case Reserved10 	:
			case WAIT_FAILED  	:	// TL
				return FALSE;
			default :
				return m_MultiWait.Add(hHandle, dwEventMap);
		}
	}
#endif	// __COPActiveBuffer_H__






















