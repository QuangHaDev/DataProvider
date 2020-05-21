/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//          File:: ssCOPMultiActiveBuffer.h
// Create   Date:: 08/15/96
// Create Author:: Joe Payne 
//
//   Description:: Declaration of ssCOPMultiActiveBuffer class
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

#ifndef __COPMultiActiveBuffer_H__
#define __COPMultiActiveBuffer_H__

#include "ssCommon.h"
#include "ssCOPActiveBuffer.h"


/////////////////////////////////////////////////////////////////////////////
//
//	class:			ssCOPMultiActiveBuffer
//	Description:
//
//	Methods:
//
template <class T>
class ssCOPMultiActiveBuffer
{
public:
	// -------------------------------------------------------------------------
	// Default Constructor
	ssCOPMultiActiveBuffer();

	// -------------------------------------------------------------------------
	// Destructor
	~ssCOPMultiActiveBuffer();

	// -------------------------------------------------------------------------
	// Set up the active buffers
	void AddBuffer(UINT Mapping, ssCOPActiveBuffer<T>* Buffer, BOOL bSelfStart = FALSE);

	// -------------------------------------------------------------------------
	// Send COP to specified buffer
	BOOL Insert(UINT Mapping, ssCountedObjPtr<T>& cop);

	// -------------------------------------------------------------------------
	// Send COP to all buffers - broadcast
	BOOL Insert(ssCountedObjPtr<T>& cop);

	// -------------------------------------------------------------------------
	// Get the specified buffer
	ssCOPActiveBuffer<T>* GetBuffer(UINT Mapping);

	// -------------------------------------------------------------------------
	// Removes the specified buffer
	BOOL RemoveBuffer(UINT Mapping, UINT iTimeout = DEFAULT_SS_COP_ACTIVE_BUFFER_TERMINATE_TIMEOUT);

	// -------------------------------------------------------------------------
	// Removes the all buffers
	void RemoveAll(UINT iTimeout = DEFAULT_SS_COP_ACTIVE_BUFFER_TERMINATE_TIMEOUT);

	// -------------------------------------------------------------------------
	// Start all active cop buffers
	BOOL Start();

	// -------------------------------------------------------------------------
	// Check all active cop buffers are alive
	BOOL CheckAlive(UINT iTimeout);

	// -------------------------------------------------------------------------
	// Pause all active cop buffers the thread into wait state
	BOOL Pause();

	// -------------------------------------------------------------------------
	// Resume all active cop buffers paused threads from wait state
	BOOL Continue();

	// -------------------------------------------------------------------------
	// ITERATION
	// -------------------------------------------------------------------------

	// -------------------------------------------------------------------------
	// Get the position of the first buffer to remove
	POSITION GetStartPosition() const;

	// -------------------------------------------------------------------------
	// Get the next buffer specified by the position
	ssCOPActiveBuffer<T>* GetNextBuffer(POSITION& pos);

private:
	// -------------------------------------------------------------------------
	// Data
	CMap<UINT, UINT, ssCOPActiveBuffer<T>*, ssCOPActiveBuffer<T>*> Buffers_;  // map of buffers

	// -------------------------------------------------------------------------
	// prohibit copying
	ssCOPMultiActiveBuffer(const ssCOPMultiActiveBuffer &);

	// -------------------------------------------------------------------------
	// prohibit assignment
	ssCOPMultiActiveBuffer & operator=(const ssCOPMultiActiveBuffer &);

	// -------------------------------------------------------------------------
	// Terminate the passed buffer
	void TerminateBuffer(ssCOPActiveBuffer<T>* Buffer, UINT iTimeout);
};

//
//	public inline functions
//
	// -------------------------------------------------------------------------
	// Default Constructor
	template <class T>
	inline ssCOPMultiActiveBuffer<T>::ssCOPMultiActiveBuffer()
	{
	}

	// -------------------------------------------------------------------------
	// Destructor
	template <class T>
	inline ssCOPMultiActiveBuffer<T>::~ssCOPMultiActiveBuffer()
	{
		// loop thru and clean up each buffer
		POSITION pos = Buffers_.GetStartPosition();

		while (pos)
		{
			UINT mapping;
			ssCOPActiveBuffer<T>* buffer;

			Buffers_.GetNextAssoc(pos, mapping, buffer);
			Buffers_.RemoveKey(mapping);
			TerminateBuffer(buffer, DEFAULT_SS_COP_ACTIVE_BUFFER_TERMINATE_TIMEOUT);
		}
	}

	// -------------------------------------------------------------------------
	// Set the list of buffers
	template <class T>
	inline void ssCOPMultiActiveBuffer<T>::AddBuffer(UINT Mapping,
		ssCOPActiveBuffer<T>* Buffer, BOOL bSelfStart)
	{
		// add buffer to the map
		Buffers_.SetAt(Mapping, Buffer);

		// Self Start, if already started, this does nothing.
		if (bSelfStart)
			Buffer->Start();
	}

	// -------------------------------------------------------------------------
	// Get the specified buffer
	template <class T>
	inline ssCOPActiveBuffer<T>* ssCOPMultiActiveBuffer<T>::GetBuffer(UINT Mapping)
	{
		// find the buffer assigned to this address
		ssCOPActiveBuffer<T>* buffer;

		if (Buffers_.Lookup(Mapping, buffer))
			return buffer;
		else
			return NULL;
	}

	// -------------------------------------------------------------------------
	// Removes the specified buffer
	template <class T>
	inline BOOL ssCOPMultiActiveBuffer<T>::RemoveBuffer(UINT Mapping, UINT iTimeout)
	{
		BOOL removed = FALSE;

		// find the buffer assigned to this address
		ssCOPActiveBuffer<T>* buffer;

		if (Buffers_.Lookup(Mapping, buffer))
		{
			Buffers_.RemoveKey(Mapping);
			// Terminate this buffer
			TerminateBuffer(buffer, iTimeout);
			removed = TRUE;
		}
		return removed;
	}

	// -------------------------------------------------------------------------
	// Removes all buffers
	template <class T>
	inline void ssCOPMultiActiveBuffer<T>::RemoveAll(UINT iTimeout)
	{
		POSITION pos = Buffers_.GetStartPosition();
		while (pos)
		{
			UINT mapping;
			ssCOPActiveBuffer<T>* buffer;

			Buffers_.GetNextAssoc(pos, mapping, buffer);
			Buffers_.RemoveKey(mapping);
			TerminateBuffer(buffer, iTimeout);
		}
	}

	// -------------------------------------------------------------------------
	// Start all active cop buffers
	template <class T>
	inline BOOL ssCOPMultiActiveBuffer<T>::Start()
	{
		BOOL ok = TRUE;

		POSITION pos = Buffers_.GetStartPosition();
		while (pos)
		{
			UINT mapping;
			ssCOPActiveBuffer<T>* buffer;

			Buffers_.GetNextAssoc(pos, mapping, buffer);
			if (buffer->Start() == 0)
				ok = FALSE;
		}

		return ok;	// TRUE  - all started
					// FALSE - At least one buffer cannot start
	}

	// -------------------------------------------------------------------------
	// Check all active cop buffers are alive
	template <class T>
	inline BOOL ssCOPMultiActiveBuffer<T>::CheckAlive(UINT iTimeout = INFINITE)
	{
		BOOL ok = TRUE;

		POSITION pos = Buffers_.GetStartPosition();
		while (pos)
		{
			UINT mapping;
			ssCOPActiveBuffer<T>* buffer;

			Buffers_.GetNextAssoc(pos, mapping, buffer);
			if (!buffer->IsAlive(iTimeout))
				ok = FALSE;
		}

		return ok;	// TRUE  - all alive
					// FALSE - At least one buffer is dead
	}

	// -------------------------------------------------------------------------
	// Pause all active cop buffers
	template <class T>
	inline BOOL ssCOPMultiActiveBuffer<T>::Pause()
	{
		BOOL ok = TRUE;

		POSITION pos = Buffers_.GetStartPosition();
		while (pos)
		{
			UINT mapping;
			ssCOPActiveBuffer<T>* buffer;

			Buffers_.GetNextAssoc(pos, mapping, buffer);
			if (!buffer->Pause())
				ok = FALSE;
		}

		return ok;	// TRUE  - all paused
					// FALSE - At least one buffer cannot pause
	}

	// -------------------------------------------------------------------------
	// Continue all paused active cop buffers
	template <class T>
	inline BOOL ssCOPMultiActiveBuffer<T>::Continue()
	{
		BOOL ok = TRUE;

		POSITION pos = Buffers_.GetStartPosition();
		while (pos)
		{
			UINT mapping;
			ssCOPActiveBuffer<T>* buffer;

			Buffers_.GetNextAssoc(pos, mapping, buffer);
			if (!buffer->Continue())
				ok = FALSE;
		}

		return ok;	// TRUE  - all continued
					// FALSE - At least one buffer cannot continue
	}

	// -------------------------------------------------------------------------
	// Send COP to specified buffer
	template <class T>
	inline BOOL ssCOPMultiActiveBuffer<T>::Insert(UINT Mapping, ssCountedObjPtr<T>& cop)
	{
		BOOL ok = FALSE;

		// find the buffer assigned to this address
		ssCOPActiveBuffer<T>* buffer;

		if (Buffers_.Lookup(Mapping, buffer))
			// put the object in the buffer
			ok = ((*buffer)().Insert(cop));

		return ok;
	}

	// -------------------------------------------------------------------------
	// Send COP to all buffers - broadcast
	template <class T>
	inline BOOL ssCOPMultiActiveBuffer<T>::Insert(ssCountedObjPtr<T>& cop)
	{
		BOOL ok = TRUE;

		POSITION pos = Buffers_.GetStartPosition();

		while (pos)
		{
			UINT mapping;
			ssCOPActiveBuffer<T>* buffer;

			Buffers_.GetNextAssoc(pos, mapping, buffer);
			// put the object in the buffer
			if (!((*buffer)().Insert(cop)))
				ok = FALSE;
		}

		return ok;	// TRUE  - Successful for all addresses
					// FALSE - At least one buffer failed
	}

//
//	private inline functions
//

	// -------------------------------------------------------------------------
	// Terminate the passed buffer
	template <class T>
	inline void ssCOPMultiActiveBuffer<T>::TerminateBuffer(ssCOPActiveBuffer<T>* Buffer, UINT iTimeout)
	{
		// terminate the buffer and wait for it to finish
		Buffer->ExitThread();
		Buffer->WaitForExit(iTimeout);

		// now delete it
		delete Buffer;
	}
	
	// -------------------------------------------------------------------------
	// Get the position of the first buffer to remove
	template <class T>
	inline POSITION ssCOPMultiActiveBuffer<T>::GetStartPosition() const
	{
		return Buffers_.GetStartPosition();
	}

	// -------------------------------------------------------------------------
	// Get the next buffer specified by the position
	template <class T>
	inline ssCOPActiveBuffer<T>* ssCOPMultiActiveBuffer<T>::GetNextBuffer(POSITION& pos)
	{
		// find the buffer assigned to this address
		UINT mapping;
		ssCOPActiveBuffer<T>*	buffer;

		Buffers_.GetNextAssoc(pos, mapping, buffer);

		return buffer;
	}


#endif	// __COPMultiActiveBuffer_H__
