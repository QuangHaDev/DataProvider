/////////////////////////////////////////////////////////////////////////////
//          File:: Callback.h
// Create Author:: Raymond Yung
// Create   Date:: 1 September 00
//
//   Description:: CALLBACK Class Declaration
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

#ifndef __CALLBACK_H__
#define __CALLBACK_H__

#include "ssCountedObjPtr.h"
#include "MsgObject.h"
#include "Definition.h"
#include <map>
using namespace std;

/////////////////////////////////////////////////////////////////////////////
//
// Class      : 
// Description:
//


//////////////////////////////////////////////////////////////////////////////
// Class CCallbackElement
//////////////////////////////////////////////////////////////////////////////
class CCallbackElement : public CObject
{
	DECLARE_DYNCREATE(CCallbackElement)

public :
	// -------------------------------------------------------------------------
	// Default Constructor
	CCallbackElement();

	// -------------------------------------------------------------------------
	// Copy Constructor
	CCallbackElement& operator=(const CCallbackElement& pSource);

public :
	long			m_lPageNo;
	FID_DEFINITION	m_enumFieldID;
	BYTE			m_bytRecordType;
	BYTE			m_bytMarketCode;
	BYTE			m_bytSubMarket;
	CObject*	m_pCallbackObject;
	void (*m_fpCallbackFunction)(long lPageNo, FID_DEFINITION enumFIeldID, long lIndex,
		COP_ITEM copMsg, CObject *pCallbackObject);
};

//////////////////////////////////////////////////////////////////////////////
// Class CCallback
//////////////////////////////////////////////////////////////////////////////
class CCallback : public CObject
{
	DECLARE_DYNCREATE(CCallback)

//public :
//	CObList m_listCallback;

public :
	// -------------------------------------------------------------------------
	// Default Constructor
	CCallback(CDCache * pDCache = NULL);
	// -------------------------------------------------------------------------
	// Destructor
	virtual ~CCallback();

// Implementation
public :
	BOOL AddItem(long lPageNo, FID_DEFINITION enumFieldID, BYTE bytRecordType, 
	BYTE bytMarketCode, BYTE bytSubMarket, CObject *pCallbackObject,
	void (*fpCallbackFunction)(long lPageNo, FID_DEFINITION enumFieldID, long lIndex,
								   COP_ITEM copMsg, CObject *pCallbackObject));
	BOOL RemoveItem(long lPageNo, FID_DEFINITION enumFieldID, BYTE bytRecordType, 
					BYTE bytMarketCode, BYTE bytSubMarket, CObject *pCallbackObject);
	void DoCallbackFunctions(long lPageNo, FID_DEFINITION enumFieldID, long lIndex, COP_ITEM copMsg, 
							 BYTE bytRecordType, BYTE bytMarketCode, BYTE bytSubMarket);
	CObList* LookupCallbackElementList(long lPageNo, FID_DEFINITION enumFieldID, BYTE bytRecordType, BYTE bytMarketCode, BYTE bytSubMarket);
	int ReleaseCriticalSection() { return m_CriticalSection.LeaveAll(); }

	void ForceSingleCallback(CObject *pCallbackObject,
		void(*fpCallbackFunction)(long lPageNo, FID_DEFINITION enumFieldID, long lIndex,
		COP_ITEM copMsg, CObject *pCallbackObject))
	{
		m_bSigleCallback = true;
		m_fpCallbackFunction = fpCallbackFunction;
		m_pCallbackObject = pCallbackObject;
	}
private:
	ssCriticalSection	m_CriticalSection;

	typedef map<long, CObList*> CALLBACK_FID_MAP;
	typedef map<long, CALLBACK_FID_MAP*> CALLBACK_PAGE_MAP;

	map <long, CObList*>:: iterator mFid_Iter;
	map <long, CObList*>:: const_iterator mFid_cIter;

	map <long, CALLBACK_FID_MAP*>:: iterator mPage_Iter;
	map <long, CALLBACK_FID_MAP*>:: const_iterator mPage_cIter;

	CALLBACK_PAGE_MAP	m_mapCallback;
	CDCache * m_pDCache;


	bool m_bSigleCallback;
	void(*m_fpCallbackFunction)(long lPageNo, FID_DEFINITION enumFIeldID, long lIndex, COP_ITEM copMsg, CObject *pCallbackObject);
	CObject*	m_pCallbackObject;

};

#endif  // __CALLBACK_H__ 
