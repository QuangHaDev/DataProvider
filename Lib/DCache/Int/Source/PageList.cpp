/////////////////////////////////////////////////////////////////////////////
//          File:: PageList.cpp
// Create Author:: Raymond Yung
// Create   Date:: 1 September 00
//
//   Description:: DCache Class member function
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
#include <stdafx.h>
#include "PageList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CPageList, CObject)
// -------------------------------------------------------------------------
// Default Constructor
// -------------------------------------------------------------------------

CPageList::CPageList()
{
	m_pacPageList = NULL;
}
// -------------------------------------------------------------------------
CPageList::CPageList(unsigned long lNoOfPage)
{
	int nCharSize = (lNoOfPage+7)/8;
	m_pacPageList = new char[nCharSize];
	m_lNoOfPages = lNoOfPage;
	memset(m_pacPageList, 0, nCharSize);
}

// -------------------------------------------------------------------------
// Destructor
CPageList::~CPageList()
{
	delete m_pacPageList;
}
// -------------------------------------------------------------------------

void CPageList::SetPages(CString *pstrPages)
{
	if (pstrPages == NULL) return;
	char szBuffer[1024];
	unsigned long lPageNo1;
	unsigned long lPageNo2;
	char *pacWorkingPtr;

	strcpy(szBuffer, (LPCSTR)*pstrPages);
	pacWorkingPtr = strtok(szBuffer, " ");
	while (pacWorkingPtr != NULL)
	{
			if (sscanf(pacWorkingPtr, "%d-%d", &lPageNo1, &lPageNo2) != 2)
			{
				sscanf(pacWorkingPtr, "%d", &lPageNo1);
				lPageNo2 = lPageNo1;
			}
			if (lPageNo1 > lPageNo2)
			{
				unsigned long lTempPageNo = lPageNo1;
				lPageNo1 = lPageNo2;
				lPageNo2 = lTempPageNo;
			}

			for (unsigned long i=lPageNo1; i<=lPageNo2; i++)
			{
				Add(i);
			}
		pacWorkingPtr = strtok(NULL, " ");
	}
}
// -------------------------------------------------------------------------

void CPageList::Add(unsigned long lPageNo)
{
	if (m_lNoOfPages <= lPageNo) return;

	unsigned long lIndex = (lPageNo >> 3);	// Means lPageNo / 8
	int  nBit = (lPageNo & 0x7);	// Means lPageNo % 8;

	char *pcChar = &m_pacPageList[lIndex];
	char cChar = (0x80 >> nBit);
	*pcChar |= cChar;
}
// -------------------------------------------------------------------------

void CPageList::Remove(unsigned long lPageNo)
{
	if (m_lNoOfPages <= lPageNo) return;

	unsigned long lIndex = (lPageNo >> 3);	// Means lPageNo / 8
	int  nBit = (lPageNo & 0x7);	// Means lPageNo % 8;

	char *pcChar = &m_pacPageList[lIndex];
	char cChar = !(0x80 >> nBit);
	*pcChar &= cChar;
}
// -------------------------------------------------------------------------

BOOL CPageList::IsIn(unsigned long lPageNo)
{
	if (m_lNoOfPages <= lPageNo) return(FALSE);

	unsigned long lIndex = (lPageNo >> 3);	// Means lPageNo / 8
	int  nBit = (lPageNo & 0x7);	// Means lPageNo % 8;

	char cChar = m_pacPageList[lIndex];
	char cTemp = (0x80 >> nBit);
	return(cChar & cTemp);
}
