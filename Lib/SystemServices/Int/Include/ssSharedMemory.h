/////////////////////////////////////////////////////////////////////////////
//          File:: ssSharedMemory.h
// Create Author:: Calvin Tam
// Create   Date:: 10 Aug 1999
//
//   Description:: The ssSharedMemory class declaration
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

#ifndef __ssSharedMemory_h_
#define __ssSharedMemory_h_

#include "ssCommon.h"

class ssSharedMemory
{
public:
	// ----------------------------------------------------------------------
	// constructor
	ssSharedMemory ();

	// ----------------------------------------------------------------------
	// destructor
	virtual ~ssSharedMemory ();

	// Create will have read-write access
	// Return FALSE if the specific name exists and bCreateExist is FALSE
	BOOL Create (char* szMemoryName, DWORD dwMaximumSizeHigh,
		DWORD dwMaximumSizeLow, BOOL bCreateExist = FALSE);

	// Open will have read-only access
	// Return FALSE if the specific name does not exist
	BOOL Open (char* szMemoryName);

	void Close ();

	LPVOID GetBuffer ()
	{
		return m_lpMemory;
	}

protected:
	HANDLE		m_hFileMapping;
	LPVOID		m_lpMemory;
};

#endif
