/////////////////////////////////////////////////////////////////////////////
//          File:: Callback.cpp
// Create Author:: Raymond Yung
// Create   Date:: 1 September 00
//
//   Description:: Callback Class member function
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
#include "DCache.h"
#include "Callback.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////
// Class CCallback Implementation
//////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CCallback, CObject)

// -------------------------------------------------------------------------
// Default Constructor

CCallback::CCallback(CDCache * pDCache)
{
	m_pDCache = pDCache;
	m_bSigleCallback = false;
}

// -------------------------------------------------------------------------
// Destructor
CCallback::~CCallback()
{
	CALLBACK_FID_MAP* mapFids;
	CObList* listCallBackElement;

	m_CriticalSection.Enter();
	
	while (!m_mapCallback.empty())
	{
		mPage_Iter = m_mapCallback.begin();
		mapFids = (*mPage_Iter).second;

		while (mapFids != nullptr && !(*mapFids).empty())
		{
			mFid_Iter = (*mapFids).begin();
			listCallBackElement = (*mFid_Iter).second;
			if (listCallBackElement)
			{
				while (!listCallBackElement->IsEmpty())
				{
					delete listCallBackElement->RemoveHead();
				}
				(*mapFids).erase(mFid_Iter);
				delete listCallBackElement;
			}
		}
		m_mapCallback.erase(mPage_Iter);			
		delete mapFids;
	}
	m_CriticalSection.Leave();
}

// -------------------------------------------------------------------------
BOOL CCallback::AddItem(long lPageNo, FID_DEFINITION enumFieldID, 
		BYTE bytRecordType, BYTE bytMarketCode, BYTE bytSubMarket, 
		CObject *pCallbackObject,
		void (*fpCallbackFunction)(long lPage, FID_DEFINITION enumFieldID,
								long lIndex, COP_ITEM copMsg, CObject *pCallbackObject))
{
	CCallbackElement* pNew;

	m_CriticalSection.Enter();
	pNew = new CCallbackElement;
	pNew->m_lPageNo = lPageNo;
	pNew->m_enumFieldID = enumFieldID;
	pNew->m_bytMarketCode = bytMarketCode;
	pNew->m_bytRecordType = bytRecordType;
	pNew->m_bytSubMarket  = bytSubMarket;
	pNew->m_pCallbackObject = pCallbackObject;
	pNew->m_fpCallbackFunction = fpCallbackFunction;
	
	CALLBACK_FID_MAP* mapFids;
	if (m_mapCallback.find(lPageNo) == m_mapCallback.end())
	{
		mapFids = new CALLBACK_FID_MAP;
		m_mapCallback[lPageNo] = mapFids;
	}
	else
		mapFids = m_mapCallback[lPageNo];

	CObList* listCallBackElement;
	if (mapFids->find(enumFieldID) == mapFids->end())
	{
		listCallBackElement = new CObList;
		(*mapFids)[enumFieldID] = (CObList*) listCallBackElement;
	}
	else
		listCallBackElement = (*mapFids)[enumFieldID];

	listCallBackElement->AddTail((CObject*)pNew);		

	m_CriticalSection.Leave();
	return(TRUE);

}
// -------------------------------------------------------------------------
BOOL CCallback::RemoveItem(long lPageNo, FID_DEFINITION enumFieldID, 		
						   BYTE bytRecordType, BYTE bytMarketCode, BYTE bytSubMarket, 
						   CObject *pCallbackObject)
{
	BOOL bReturnValue = FALSE;
	POSITION pos;
	POSITION posPrev;
	CCallbackElement *pCallbackElement;

	m_CriticalSection.Enter();

	CALLBACK_FID_MAP* mapFids;
	if (m_mapCallback.find(lPageNo) != m_mapCallback.end())
	{
		mapFids = m_mapCallback[lPageNo];

		CObList* listCallBackElement;
		if (mapFids->find(enumFieldID) != mapFids->end())
		{
			listCallBackElement = (*mapFids)[enumFieldID];
			if((pos=listCallBackElement->GetHeadPosition()) != NULL)
			{
				do {
					posPrev = pos;
					pCallbackElement = (CCallbackElement*)listCallBackElement->GetNext(pos);
					if (pCallbackElement->m_bytMarketCode == bytMarketCode)
						if (pCallbackElement->m_bytRecordType == bytRecordType)
							if (pCallbackElement->m_bytSubMarket == bytSubMarket)
								if (pCallbackElement->m_pCallbackObject == pCallbackObject)
								{
									listCallBackElement->RemoveAt(posPrev);
									delete pCallbackElement;
									bReturnValue = TRUE;
									break;
								}
				} while(pos != NULL);
				if (listCallBackElement->IsEmpty())
				{
					delete listCallBackElement;
					mapFids->erase(enumFieldID);
					if (mapFids->empty())
					{
						m_mapCallback.erase(lPageNo);
						delete mapFids;
					}
				}
			}
		}
	}
	m_CriticalSection.Leave();
	return bReturnValue;

}
// -------------------------------------------------------------------------
void CCallback::DoCallbackFunctions(long lPageNo, FID_DEFINITION enumFieldID, long lIndex, COP_ITEM copMsg, 
									BYTE bytRecordType, BYTE bytMarketCode, BYTE bytSubMarket)
{
	m_CriticalSection.Enter();

	if (m_bSigleCallback) {
		try{
			m_fpCallbackFunction(lPageNo, enumFieldID,lIndex, copMsg, m_pCallbackObject);
		} catch (...){
			char szFaultMsg[MAX_LEN];
			sprintf(szFaultMsg, "(DCACHE) Callback exception catched.Page: %u", lPageNo);
			m_CriticalSection.Leave();
			if (m_pDCache)
				m_pDCache->LogFault((char*)&szFaultMsg);
			return;
		}
	}
	else {

		CObList*			listCallBackElement = NULL;
		CCallbackElement*	pCallbackElement = NULL;
		POSITION			pos;

		try { // prevent crash 
			if ((listCallBackElement = LookupCallbackElementList(lPageNo, enumFieldID, bytRecordType, bytMarketCode, bytSubMarket)) != NULL)
			{
				if ((pos = listCallBackElement->GetHeadPosition()) != NULL)
				{
					do {
						pCallbackElement = (CCallbackElement*)listCallBackElement->GetNext(pos);
						if (pCallbackElement->m_bytMarketCode == bytMarketCode || pCallbackElement->m_bytMarketCode == 0)
							if (pCallbackElement->m_bytRecordType == bytRecordType || pCallbackElement->m_bytRecordType == 0)
								if (pCallbackElement->m_bytSubMarket == bytSubMarket || pCallbackElement->m_bytSubMarket == 0)
								{
									pCallbackElement->m_fpCallbackFunction(lPageNo, enumFieldID,
										lIndex, copMsg, pCallbackElement->m_pCallbackObject);
								}
					} while (pos != NULL);
				}
			}
			if ((listCallBackElement = LookupCallbackElementList(-1, enumFieldID, bytRecordType, bytMarketCode, bytSubMarket)) != NULL)
			{
				if ((pos = listCallBackElement->GetHeadPosition()) != NULL)
				{
					do {
						pCallbackElement = (CCallbackElement*)listCallBackElement->GetNext(pos);
						if (pCallbackElement->m_bytMarketCode == bytMarketCode || pCallbackElement->m_bytMarketCode == 0)
							if (pCallbackElement->m_bytRecordType == bytRecordType || pCallbackElement->m_bytRecordType == 0)
								if (pCallbackElement->m_bytSubMarket == bytSubMarket || pCallbackElement->m_bytSubMarket == 0)
								{
									pCallbackElement->m_fpCallbackFunction(lPageNo, enumFieldID,
										lIndex, copMsg, pCallbackElement->m_pCallbackObject);
								}
					} while (pos != NULL);
				}
			}

			if ((listCallBackElement = LookupCallbackElementList(lPageNo, FID_END, bytRecordType, bytMarketCode, bytSubMarket)) != NULL)
			{
				if ((pos = listCallBackElement->GetHeadPosition()) != NULL)
				{
					do {
						pCallbackElement = (CCallbackElement*)listCallBackElement->GetNext(pos);
						if (pCallbackElement->m_bytMarketCode == bytMarketCode || pCallbackElement->m_bytMarketCode == 0)
							if (pCallbackElement->m_bytRecordType == bytRecordType || pCallbackElement->m_bytRecordType == 0)
								if (pCallbackElement->m_bytSubMarket == bytSubMarket || pCallbackElement->m_bytSubMarket == 0)
								{
									pCallbackElement->m_fpCallbackFunction(lPageNo, enumFieldID,
										lIndex, copMsg, pCallbackElement->m_pCallbackObject);
								}
					} while (pos != NULL);
				}
			}

			if ((listCallBackElement = LookupCallbackElementList(-1, FID_END, bytRecordType, bytMarketCode, bytSubMarket)) != NULL)
			{
				if ((pos = listCallBackElement->GetHeadPosition()) != NULL)
				{
					do {
						pCallbackElement = (CCallbackElement*)listCallBackElement->GetNext(pos);
						if (pCallbackElement->m_bytMarketCode == bytMarketCode || pCallbackElement->m_bytMarketCode == 0)
							if (pCallbackElement->m_bytRecordType == bytRecordType || pCallbackElement->m_bytRecordType == 0)
								if (pCallbackElement->m_bytSubMarket == bytSubMarket || pCallbackElement->m_bytSubMarket == 0)
								{
									pCallbackElement->m_fpCallbackFunction(lPageNo, enumFieldID,
										lIndex, copMsg, pCallbackElement->m_pCallbackObject);
								}
					} while (pos != NULL);
				}
			}
		}
		catch (...) {
			char szFaultMsg[MAX_LEN];
			sprintf(szFaultMsg, "(DCACHE) Callback exception catched.Page: %u", lPageNo);
			m_CriticalSection.Leave();
			if (m_pDCache)
				m_pDCache->LogFault((char*)&szFaultMsg);
			return;
		}
	}
	m_CriticalSection.Leave();

}
// -------------------------------------------------------------------------
CObList* CCallback::LookupCallbackElementList(long lPageNo, FID_DEFINITION enumFieldID, BYTE bytRecordType, BYTE bytMarketCode, BYTE bytSubMarket)
{
	CObList*			listCallBackElement = NULL;
	CALLBACK_FID_MAP*	mapFids;

	if (m_mapCallback.find(lPageNo) != m_mapCallback.end())
	{
		mapFids = m_mapCallback[lPageNo];
		if (mapFids->find(enumFieldID) != mapFids->end())
		{
			listCallBackElement = (*mapFids)[enumFieldID];
		}
	}
	return listCallBackElement;
}

//////////////////////////////////////////////////////////////////////////////
// Class CCallbackElement Implementation
//////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CCallbackElement, CObject)

// -------------------------------------------------------------------------
// Default Constructor
CCallbackElement::CCallbackElement()
{
	m_lPageNo = -1L;
	m_enumFieldID = FID_END;
	m_pCallbackObject = NULL;
	m_fpCallbackFunction = NULL;
	m_bytRecordType = 0;
	m_bytMarketCode = 0;
	m_bytSubMarket = 0;
}

// -------------------------------------------------------------------------
// Copy Constructor
CCallbackElement& CCallbackElement::operator=(const CCallbackElement& Source)
{
	m_lPageNo = Source.m_lPageNo;
	m_enumFieldID = Source.m_enumFieldID;
	m_bytRecordType = Source.m_bytRecordType;
	m_bytMarketCode = Source.m_bytMarketCode;
	m_bytSubMarket  = Source.m_bytSubMarket;
	m_pCallbackObject = Source.m_pCallbackObject;
	m_fpCallbackFunction = Source.m_fpCallbackFunction;
	return(*this);
}
