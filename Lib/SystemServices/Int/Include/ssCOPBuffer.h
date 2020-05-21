/////////////////////////////////////////////////////////////////////////////
//          File:: ssCOPBuffer.h
// Create   Date:: Dec 19 1995
// Create Author:: Joseph Payne
//
//   Description:: The file contains a definition for a buffer template class
//                 of counted object pointers.
//                 This class provides multi-thread protected access to the 
//                 buffer.
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000 by AFE Solutions Ltd.
// All rights reserved.
//
// No portion of this software in any form may be used or reproduced in 
// any manner without written consent from AFE Solutions Ltd.
//
#ifndef __COPBUFFER_H__
#define __COPBUFFER_H__

#include "afx.h"
#include "ssEvent.h"
#include "ssCriticalSection.h"
#include "ssCountedObjPtr.h"
#include <afxtempl.h>

const UINT DEFAULT_SS_COPBUFFER_UPPER_BOUND = 5;
const UINT DEFAULT_SS_COPBUFFER_INCREMENT = 5;
const UINT DEFAULT_SS_COPBUFFER_UPPER_LIMIT = 10;

//******************************************************************************
// Logic
// ~~~~~
//	* - Messages can be inserted & removed from COP Buffer
//	  - Messages are removed in the order in which they are inserted
//	  - Messages are inserted from the Head of the List
//
// Construction
// ~~~~~~~~~~~~
//	* When the buffer is constructed, the following are created
//	  - An event :	* This would be SET on the arrival of the first message so as
//					  to initiate components to remove messages from the buffer
//					* It is RESET when the buffer becomes empty; This prevents
//					  components from removing messages
//	  - A mutex  :	Protects data from being modified by more than 1 active 
//					components at any time
//	  - The INS pos, REM pos are made to point to the HEAD of the List
//	  - The TAIL pos of the List is also stored
//
// Insertion of Messages
// ~~~~~~~~~~~~~~~~~~~~~
//	* If the Buffer is FULL it is expanded
//		Expansion
//		---------
//		- Buffer can be expanded only till the Upper Limit
//		- Depending upon the INS pos, buffer will be expanded either in the 
//		  MIDDLE or at the TAIL
//		  (i.e) If the INS pos is at the TAIL 
//					the buffer is expanded at the TAIL
//				else
//					the buffer is expanded in the middle
//		- If the Buffer expansion is not successful, the Insert is STOPPED
//
//	* If there is EMPTY SPACE in the buffer
//		Modifying INS pos
//		-----------------
//		- Depending on whether it is the 1st time the msg is being inserted, 
//		  the INS position is modified
//		  (i.e) If it the 1st time the msg is inserted
//					the INS position remains at the HEAD of the List
//				else
//					The next position in the list is obtained
//					If the newly obtained position is null (i.e. TAIL has reached),
//					INS pos is made to point to the HEAD of the list (i.e.Circular Buffer)
//
//	* If INS pos not VALID due to some reason
//		the Insert is stopped
//	  else
// 		- The new msg is Inserted in the list at the position specified by the INS pos
// 		- The count (no. of elements in the buffer) is INCREMENTED
// 		- The event is SET when there is atleast 1 message Inserted into the buffer
//
// Removal of Messages
// ~~~~~~~~~~~~~~~~~~~
//	* If the Buffer is EMPTY (no msgs in the buffer)
//		an EMPTY COP is retured
//	  else
//		- The message is Removed
//		- An EMPTY COP is assigned at the position from where the msg is Removed
//		- The REM position is modified
//			Modifying REM pos
//			-----------------
//			: The REM pos is made to point to the next position in the List
//			: If the newly obtained position is null (i.e. TAIL has reached),
//			REM pos is made to point to HEAD of the list (i.e. Circular buffer)
//			- The count (no. of elements in the buffer) is decremented
//			- If the Buffer becomes EMPTY (i.e. count becomes 0), the event is Reset()
//******************************************************************************

//******************************************************************************
//******************************************************************************

// CLASS DECLARATIONS

//******************************************************************************
//******************************************************************************

template <class T> class ssCOPBuffer
{
//MEMBER FUNCTIONS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
public: 
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// constructor & destructor
	ssCOPBuffer(	UINT upper_bound = DEFAULT_SS_COPBUFFER_UPPER_BOUND, 
					UINT increment   = DEFAULT_SS_COPBUFFER_INCREMENT,
					UINT upper_limit = DEFAULT_SS_COPBUFFER_UPPER_LIMIT);
	virtual ~ssCOPBuffer();

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Insert & Remove Operations
	BOOL Insert(const ssCountedObjPtr<T>& cop);
	const ssCountedObjPtr<T>& Remove();
	const ssCountedObjPtr<T>& Peek();

	BOOL Insert(T* p_element);
	BOOL Remove(ssCountedObjPtr<T>& cop);
	BOOL Peek(ssCountedObjPtr<T>& cop);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Retrieving components wait upon this Synchronous Event
	const ssEvent& GetEvent() const;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Modify Boundaries
	BOOL SetUpperBound(UINT upper_bound);
	BOOL SetIncrement(UINT increment);
	BOOL SetUpperLimit(UINT upper_limit);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Obtain statistics
	UINT GetCount();
	UINT GetMaxCount();
	UINT GetUpperBound();
	UINT GetIncrement();
	UINT GetUpperLimit();

protected:
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Expand the buffer by the no. specified by the increment; 
	BOOL ExpandBuffer(UINT increment, BOOL upper_limit_check);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Modify Insert & Remove positions
	BOOL IncrementInsertPosition();
	void IncrementRemovePosition();
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Check Status
	BOOL IfBufferEmpty();	
	BOOL IfBufferFull();

private:
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// make copy constructor and assignment private	- not available
	ssCOPBuffer(const ssCOPBuffer<T>&);
	ssCOPBuffer& operator=(const ssCOPBuffer<T>&);

//DATA MEMBERS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
private:
	// Components removing messages from the buffer, wait on this event
	ssEvent m_event;
	// Mutex for the queues
	ssCriticalSection m_mutex;
	// The COP Buffer 
	CList<ssCountedObjPtr<T>, const ssCountedObjPtr<T>&> m_cops; // linked list of cops
	// An empty COP
	ssCountedObjPtr<T> m_empty_cop;
	// Store the COP that is removed from the buffer 
	ssCountedObjPtr<T> m_removal_cop;

	UINT m_upper_bound;					// Store the upper bound
	UINT m_increment;					// Store the increment
	UINT m_upper_limit;					// Store the upper limit
	UINT m_elements_in_buffer;			// Store the no. of elements in the buffer
	UINT m_peak_elements_in_buffer;		// Store the peak no. of elements in the buffer
	POSITION m_ipos, m_rpos, m_tailPos;	// Store the Insert, Remove & Tail positions in the list	
	BOOL m_bFirst;						// Indicate whether it is the first time the element is being added
}; 

//******************************************************************************
//******************************************************************************

// inlineS - ssCOPBuffer

//******************************************************************************
//******************************************************************************

template <class T>
inline ssCOPBuffer<T>::ssCOPBuffer(UINT upper_bound, UINT increment, UINT upper_limit) 
												:m_event(TRUE),
												 m_upper_bound(upper_bound),
												 m_increment(increment),
												 m_upper_limit(upper_limit),
												 m_elements_in_buffer(0),
												 m_peak_elements_in_buffer(0)
{
	// Check to make sure the upper limit is greater than or equal to the
	// upper bound, if not, set the upper limit to the upper bound
	if(m_upper_bound > m_upper_limit)
		m_upper_limit = m_upper_bound;
	
	if(m_upper_bound == 0) m_upper_bound 	= DEFAULT_SS_COPBUFFER_UPPER_BOUND;
	if(m_upper_limit == 0) m_upper_limit	= DEFAULT_SS_COPBUFFER_UPPER_LIMIT;
	// Create a List of empty COPs; Size = m_upper_bound 
	for( UINT i=0;i<m_upper_bound;i++ )
	{
		m_cops.AddTail(m_empty_cop);
	}
	// Store the Head 
	m_ipos = m_cops.GetHeadPosition();
	m_rpos = m_cops.GetHeadPosition();
	// Store the Tail
	m_tailPos = m_cops.GetTailPosition();
	m_bFirst = FALSE;
}

//******************************************************************************
template <class T>                                        
inline ssCOPBuffer<T>::~ssCOPBuffer()
{
	// CList will clean up automatically on destruction
}

//******************************************************************************
template <class T>
inline BOOL ssCOPBuffer<T>::Insert(const ssCountedObjPtr<T>& cop)
{
	m_mutex.Enter();
	{
		// If buffer is full, but expansion not possible
		if( IfBufferFull() && (!ExpandBuffer(m_increment,TRUE)))	
		{
			m_mutex.Leave();
			return FALSE;
		}
		// Modify the Insertion Position
		if(IncrementInsertPosition() == FALSE)
		{
			m_mutex.Leave();
			return FALSE;
		}
		// Insert the element at the Inertion position
		m_cops.SetAt(m_ipos,cop);
		// Now, Increment the count 
		m_elements_in_buffer++;
		if (m_elements_in_buffer > m_peak_elements_in_buffer)
		{
			m_peak_elements_in_buffer = m_elements_in_buffer;
		}
		// Signal the event, on the arrival of the 1st msg
		if (m_elements_in_buffer == 1)
		{
			m_event.Set();
		}
	}
	m_mutex.Leave();
	return TRUE;
}

//******************************************************************************
template <class T>
inline const ssCountedObjPtr<T>& ssCOPBuffer<T>::Remove()
{
	BOOL Empty = TRUE;	
	m_mutex.Enter();

	// Msgs can be removed only if present
	if (!IfBufferEmpty()) 
	{	// Buffer not empty
		Empty = FALSE;

		// If removal pos invalid, return
		if (m_rpos == NULL)
		{
			m_mutex.Leave();
			return m_empty_cop;
		}
		// Get a reference to the element at removal pos
		m_removal_cop = m_cops.GetAt(m_rpos);
		// Initialize the removed posiiton with an empty cop
		m_cops.SetAt(m_rpos,m_empty_cop);
		// Modify removal pos
		IncrementRemovePosition();
		// Decrement the count 
		m_elements_in_buffer--;

		// Unsignal, If there are no msgs,
		if(IfBufferEmpty())
		{
			m_event.Reset();
		}
	}
	m_mutex.Leave();
	
	// If buffer empty, return empty cop else return removed cop
	if (Empty == TRUE)
	{
		return m_empty_cop;
	}
	else
		return m_removal_cop;
}

//******************************************************************************
template <class T>
inline BOOL ssCOPBuffer<T>::Insert(T* p_element)
{
	m_mutex.Enter();
	{
		// If buffer is full, but expansion not possible
		if( IfBufferFull() && (!ExpandBuffer(m_increment,TRUE)))
		{
			m_mutex.Leave();
			return FALSE;
		}
		// Modify the Insertion Position
		if(IncrementInsertPosition() == FALSE)
		{
			m_mutex.Leave();
			return FALSE;
		}
		// Set the element at that position with the new pointer
		// Use a temporary cop to pass it into the list
		m_cops.SetAt(m_ipos,ssCountedObjPtr<T>(p_element));

		// Now, Increment the count
		m_elements_in_buffer++;
		if (m_elements_in_buffer > m_peak_elements_in_buffer)
		{
			m_peak_elements_in_buffer = m_elements_in_buffer;
		}

		// Signal the event, on the arrival of the 1st msg
		if (m_elements_in_buffer == 1)
		{
			m_event.Set();
		}
	}
	m_mutex.Leave();
	return TRUE;
}

//******************************************************************************
template <class T>
inline BOOL ssCOPBuffer<T>::Remove(ssCountedObjPtr<T>& cop)
{
	BOOL Empty = TRUE;
	m_mutex.Enter();
	{
		// Msgs can be removed only if present
		if (!IfBufferEmpty())
		{	// Buffer not empty
			Empty = FALSE;

			// if removal pos invalid, return
			if (m_rpos == NULL) 
			{
				m_mutex.Leave();
				return FALSE;
			}
			// Get a reference to the element at that position
			cop = m_cops.GetAt(m_rpos);

			// Initialize the removed posiiton with an empty cop
			m_cops.SetAt(m_rpos,m_empty_cop);

			// Modify removal pos
			IncrementRemovePosition();

			// Decrement the number of elements	first
			m_elements_in_buffer--;

			// Unsignal, if there are no msgs
			if(IfBufferEmpty())
			{
				m_event.Reset();
			}
		}
	} // end Mutex
	m_mutex.Leave();
	if(Empty)
		return FALSE;
	else
		return !cop.isNull();
}
//******************************************************************************
template <class T>
inline const ssCountedObjPtr<T>& ssCOPBuffer<T>::Peek()
{
	BOOL Empty = TRUE;	
	m_mutex.Enter();

	// Msgs can be retrieved only if present
	if (!IfBufferEmpty()) 
	{	// Buffer not empty
		Empty = FALSE;

		// If removal pos invalid, return
		if (m_rpos == NULL)
		{
			m_mutex.Leave();
			return m_empty_cop;
		}
		// Get a reference to the element at removal pos
		m_removal_cop = m_cops.GetAt(m_rpos);
	}
	m_mutex.Leave();
	
	// If buffer empty, return empty cop else return removed cop
	if (Empty == TRUE)
	{
		return m_empty_cop;
	}
	else
		return m_removal_cop;
}


//******************************************************************************
template <class T>
inline BOOL ssCOPBuffer<T>::Peek(ssCountedObjPtr<T>& cop)
{
	BOOL Empty = TRUE;
	m_mutex.Enter();
	{
		// Msgs can be removed only if present
		if (!IfBufferEmpty())
		{	// Buffer not empty
			Empty = FALSE;

			// if removal pos invalid, return
			if (m_rpos == NULL) 
			{
				m_mutex.Leave();
				return FALSE;
			}
			// Get a reference to the element at that position
			cop = m_cops.GetAt(m_rpos);
		}
	} // end Mutex
	m_mutex.Leave();
	if(Empty)
		return FALSE;
	else
		return !cop.isNull();
}
//******************************************************************************
template <class T>
inline BOOL ssCOPBuffer<T>::ExpandBuffer(UINT increment, BOOL upper_limit_check)
{
	// if the increment value is set to zero,
	// expanding buffer has been disabled -- return FALSE
	if( increment == 0 )
		return FALSE;

	// if we need to do the upper limit check
	if(upper_limit_check)
		// Check to see if adding the extra space to
		// the buffer will exceed the upper limit
		if( (m_upper_bound + increment) > m_upper_limit )
			return FALSE;

	if(m_ipos == m_tailPos)	// Add empty cops to the tail of the list
	{
		// Store the Tail position
		for(UINT i = 1; i <= increment; i++)
		{
			m_tailPos = m_cops.AddTail(m_empty_cop);
			m_upper_bound++;
		}
		return TRUE;
	}
	else					//  Add in the middle of the list
	{
		if(!m_ipos)	return FALSE;
		
		POSITION tmpPos = m_ipos;
		for(UINT i = 1; i <= increment; i++)
		{
			tmpPos = m_cops.InsertAfter(tmpPos, m_empty_cop);
			m_upper_bound++;
		}
		return TRUE;
	}
}

//******************************************************************************
template <class T>
inline UINT ssCOPBuffer<T>::GetCount()
{
	UINT elements_in_buffer;
	m_mutex.Enter();
	elements_in_buffer = m_elements_in_buffer;
	m_mutex.Leave();
	return elements_in_buffer;
}

//******************************************************************************
template <class T>
inline UINT ssCOPBuffer<T>::GetMaxCount()
{
	UINT peak_elements_in_buffer;
	m_mutex.Enter();
	peak_elements_in_buffer = m_peak_elements_in_buffer;
	m_mutex.Leave();
	return peak_elements_in_buffer;
}

//******************************************************************************
template <class T>
inline UINT ssCOPBuffer<T>::GetUpperBound()
{
	UINT upper_bound;
	m_mutex.Enter();
	upper_bound = m_upper_bound;
	m_mutex.Leave();
	return upper_bound;
}

//******************************************************************************
template <class T>
inline UINT ssCOPBuffer<T>::GetIncrement()
{
	UINT increment;
	m_mutex.Enter();
	increment = m_increment;
	m_mutex.Leave();
	return increment;
}

//******************************************************************************
template <class T>
inline UINT ssCOPBuffer<T>::GetUpperLimit()
{
	UINT upper_limit;
	m_mutex.Enter();
	upper_limit = m_upper_limit;
	m_mutex.Leave();
	return upper_limit;
}

//******************************************************************************
template <class T>
inline BOOL ssCOPBuffer<T>::SetUpperBound(UINT upper_bound)
{
	// At this point in time, only allow upper bound to go up
	// or stay the same.
	m_mutex.Enter();
	if (m_upper_bound > upper_bound)
	{
		m_mutex.Leave();
		return FALSE;
	}

	// Ok, just doesn't change anything
	if(m_upper_bound == upper_bound)
	{
		m_mutex.Leave();
		return TRUE;
	}

	// See if we can expand the buffer to the new upper bound
	if(!ExpandBuffer(upper_bound - m_upper_bound, FALSE))
	{
		m_mutex.Leave();
		return FALSE;
	}

	// Check to make sure the upper limit is greater than or equal to the
	// upper bound, if not, set the upper limit to the upper bound
	if(m_upper_bound > m_upper_limit)
		m_upper_limit = m_upper_bound;

	m_mutex.Leave();
	return TRUE;   
}

//******************************************************************************
template <class T>
inline BOOL ssCOPBuffer<T>::SetIncrement(UINT increment) 
{
	// An increment of zero will stop the buffer from expanding
	m_mutex.Enter();
	m_increment = increment;
	m_mutex.Leave();
	return TRUE;
}

//******************************************************************************
template <class T>
inline BOOL ssCOPBuffer<T>::SetUpperLimit(UINT upper_limit) 
{
	// At this point in time, only allow upper limit to go up
	// or stay the same.
	m_mutex.Enter();
	if (m_upper_limit > upper_limit)
	{
		m_mutex.Leave();
		return FALSE;
	}
	m_upper_limit = upper_limit;
	m_mutex.Leave();
	return TRUE;  
}

//******************************************************************************
template <class T>
inline const ssEvent&ssCOPBuffer<T>::GetEvent() const
{
	return m_event;
} 

//******************************************************************************
template <class T>
inline BOOL ssCOPBuffer<T>::IfBufferEmpty()
{
	BOOL Empty = FALSE;
	m_mutex.Enter();
	if (m_elements_in_buffer == 0)
		Empty = TRUE;
	m_mutex.Leave();
	return Empty;
}

//******************************************************************************
template <class T>
inline BOOL ssCOPBuffer<T>::IfBufferFull()
{
	BOOL Full = FALSE;
	m_mutex.Enter();
	if (m_elements_in_buffer == m_upper_bound )
		Full = TRUE;
	m_mutex.Leave();
	return Full;
}

//******************************************************************************
template <class T>
inline BOOL ssCOPBuffer<T>::IncrementInsertPosition()
{
	// If it is not the 1st and Insert pos is valid
	if((m_bFirst == TRUE) && (m_ipos))
	{
		// Obtain the next valid Insert position
		m_cops.GetNext(m_ipos);
		if(!m_ipos)
		{
			// If Insert position is not valid, Make it point to Head
			m_ipos = m_cops.GetHeadPosition();
			if(!m_ipos) return FALSE;
		}
	}
	// If it is the 1st time, add at the current position(Head pos) & change the status
	// Happens only once	
	else
		m_bFirst = TRUE;

	return TRUE;
}

//******************************************************************************
template <class T>
inline void ssCOPBuffer<T>::IncrementRemovePosition()
{
	//  Change removal pos to point to the next position
	m_cops.GetNext(m_rpos);
	// If end of list is reached, make it point to head
	if (m_rpos == NULL) m_rpos = m_cops.GetHeadPosition();
}

#endif	// __COPBUFFER_H__


