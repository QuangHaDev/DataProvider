/////////////////////////////////////////////////////////////////////////////
//          File:: StatisticPage.h
// Create Author:: Raymond Yung
// Create   Date:: 21 December 01
//
//   Description:: Class CStatisticPage Declaration
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
#ifndef __STATISTIC_PAGE_H__
#define __STATISTIC_PAGE_H__

#include "definition.h"
#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////
// The CStatisticPage class is the base class of all pages
/////////////////////////////////////////////////////////////////////////////

class CStatisticStruct : public CObject
{
	public:
		~CStatisticStruct () {}
		CStatisticStruct ()
		{
			iUpdateSize	=0;
			iVSSize		=0;
			iTotalSize	=0;
			iVSTimes	=0;
			iTotalTimes =0;
			iUpdateTimes=0;
			iMaxSize	=0;
		}
		int iVSTimes;
		int iTotalTimes;
		int iUpdateTimes;

		int iMaxSize;
		int iVSSize;
		int iTotalSize;
		int iUpdateSize;
};

typedef CMap <long,long,CStatisticStruct*,CStatisticStruct*> CStatisticFieldsMap;

#define DEFAULT_NO_OF_INIT_FIELDS	20

class CStatisticPage : public CObject
{
	DECLARE_DYNCREATE(CStatisticPage)

// Members
public :
	CStatisticStruct	StatisticData;
	CStatisticFieldsMap	m_mapFields;
	POSITION			m_CurFieldPos;
	CArray<int ,int>	m_TopTenFidTotalCountArray;
	CArray<int ,int>	m_TopTenFidUpdateCountArray;
	CArray<int ,int>	m_TopTenFidVSCountArray;
	CArray<int ,int>	m_TopTenFidUpdateSizeArray;
	CArray<int ,int>	m_TopTenFidVSSizeArray;
	CArray<int ,int>	m_TopTenFidTotalSizeArray;
	CArray<int ,int>	m_TopTenFidMaxSizeArray;

// Constructors & Destructors
public :
	// -------------------------------------------------------------------------
	// Default Constructor
	CStatisticPage();
	// -------------------------------------------------------------------------
	// Destructor
	virtual ~CStatisticPage();

// Implementation
public :
	long GetTotalNoOfFields();
//	SetAField(long lFid, int iSize);
	
	FID_DEFINITION GetNextField();
	void FirstField();

};
	
#endif //__STATISTIC_PAGE_H__