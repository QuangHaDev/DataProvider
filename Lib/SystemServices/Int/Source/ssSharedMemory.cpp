/////////////////////////////////////////////////////////////////////////////
//          File:: ssSharedMemory.cpp
// Create Author:: Calvin Tam
// Create   Date:: 10 Aug 1999
//
//   Description:: The ssSharedMemory class member function
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

#include "ssSharedMemory.h"

// ----------------------------------------------------------------------
// constructor
ssSharedMemory::ssSharedMemory () :
	m_hFileMapping (INVALID_HANDLE_VALUE),
	m_lpMemory (NULL)
{
}

// ----------------------------------------------------------------------
// destructor
ssSharedMemory::~ssSharedMemory ()
{
	Close();
}

//
// ********* ALL OTHER METHODS TO BE PLACED BELOW ***********
//

// ----------------------------------------------------------------------
// Create will have read-write access
// Return FALSE if the specific name exists and bCreateExist is FALSE
BOOL ssSharedMemory::Create (char* szMemoryName, DWORD dwMaximumSizeHigh,
						DWORD dwMaximumSizeLow, BOOL bCreateExist)
{
	ASSERT (m_hFileMapping == INVALID_HANDLE_VALUE);
	ASSERT (m_lpMemory == NULL);

	m_hFileMapping = CreateFileMapping (INVALID_HANDLE_VALUE,
		NULL, PAGE_READWRITE, dwMaximumSizeHigh, dwMaximumSizeLow,
		szMemoryName);

	if (!m_hFileMapping)
		return FALSE;

	if (!bCreateExist && GetLastError() == ERROR_ALREADY_EXISTS)
		return FALSE;

	m_lpMemory = MapViewOfFile (m_hFileMapping, FILE_MAP_WRITE,
		0, 0, 0);

	if (!m_lpMemory)
		return FALSE;

	return TRUE;
}

// Open will have read-only access
// Return FALSE if the specific name does not exist
BOOL ssSharedMemory::Open (char* szMemoryName)
{
	ASSERT (m_hFileMapping == INVALID_HANDLE_VALUE);
	ASSERT (m_lpMemory == NULL);

	m_hFileMapping = OpenFileMapping (FILE_MAP_READ,
		FALSE, szMemoryName);

	if (!m_hFileMapping)
	{
		m_hFileMapping = INVALID_HANDLE_VALUE;
		return FALSE;
	}

	m_lpMemory = MapViewOfFile (m_hFileMapping, FILE_MAP_READ,
		0, 0, 0);

	if (!m_lpMemory)
		return FALSE;

	return TRUE;
}


void ssSharedMemory::Close ()
{
	if (m_lpMemory)
		UnmapViewOfFile (m_lpMemory);

	if (m_hFileMapping != INVALID_HANDLE_VALUE)
		CloseHandle (m_hFileMapping);

	m_lpMemory = NULL;
	m_hFileMapping = INVALID_HANDLE_VALUE;
}
