/////////////////////////////////////////////////////////////////////////////
//          File:: Page.cpp
// Create Author:: Raymond Yung
// Create   Date:: 1 September 00
//
//   Description:: CStatisticPage Class member function
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

#include "stdafx.h"
#include "StatisticPage.h"

/*#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/
IMPLEMENT_DYNCREATE(CStatisticPage, CObject)
// -------------------------------------------------------------------------
// Default Constructor
// -------------------------------------------------------------------------

CStatisticPage::CStatisticPage()
{
	m_mapFields.InitHashTable( DEFAULT_NO_OF_INIT_FIELDS );
}

// -------------------------------------------------------------------------
// Destructor
// -------------------------------------------------------------------------
CStatisticPage::~CStatisticPage()
{

   // Remove the elements with even key values.
	long				nKey;
	CStatisticStruct*	objTemp;
	POSITION			pos = m_mapFields.GetStartPosition();

	while (pos != NULL)
	{
		m_mapFields.GetNextAssoc( pos, nKey, objTemp );
		delete objTemp;
	}
	m_mapFields.RemoveAll();
}
// -------------------------------------------------------------------------
FID_DEFINITION CStatisticPage::GetNextField()
{
	long				nKey = FID_END;
	CStatisticStruct*	objTemp;
	if (m_CurFieldPos != NULL)
		m_mapFields.GetNextAssoc( m_CurFieldPos, nKey, objTemp );
	return (FID_DEFINITION) nKey;
}
// -------------------------------------------------------------------------
void CStatisticPage::FirstField()
{
	m_CurFieldPos = m_mapFields.GetStartPosition();
}

