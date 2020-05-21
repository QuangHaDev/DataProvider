/////////////////////////////////////////////////////////////////////////////
//          File:: Callback.cpp
// Create Author:: Raymond Yung
// Create   Date:: 1 September 00
//
//   Description:: CMsgQueueManager Class member function
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
#include "Dcache.h"
#include "QuoteListToolsDlg.h"
#include "MsgQueueManager.h"
#include "STFidIterator.h"

#define PACKET_QUEUE_SIZE	2000

// -------------------------------------------------------------------------
// Default Constructor
CMsgQueueManager::CMsgQueueManager()
{
}
// -------------------------------------------------------------------------
// Destructor
CMsgQueueManager::~CMsgQueueManager()
{
}

// -------------------------------------------------------------------------

void CMsgQueueManager::Main(COP_ITEM& copMsg)
{
	{
		CStatisticPage* pStatisticPage = NULL; 
		if (!m_pMyself->m_mapPages.Lookup(copMsg->GetItemNo(), (CStatisticPage*)pStatisticPage ))
		{
			if ((pStatisticPage =new CStatisticPage) == NULL)
				m_pMyself->Log("Cannot new CStatusticPage", TRUE);
			else
				m_pMyself->m_mapPages[copMsg->GetItemNo()] = pStatisticPage ;
		}
		m_pMyself->ProcessStatisticData(pStatisticPage, copMsg->GetMsgLen(), copMsg->GetMsgType(), copMsg->GetItemNo());
		CSTFidIterator* pPI = new CSTFidIterator(copMsg->GetMessage());
		pPI->First();
		while (!pPI->IsEnd())
		{
			pPI->ProcessFid();
			m_pMyself->ProcessStatisticData(pStatisticPage, pPI->GetCurSize(), copMsg->GetMsgType(), copMsg->GetItemNo(), pPI->GetCurFidCode());
			pPI->Next();
		}

		if (pPI)
		{
			delete pPI;
			pPI = NULL;
		}
	}
}

// -------------------------------------------------------------------------

void CMsgQueueManager::Init(CQuoteListToolsDlg *pParent)
{
	m_pMyself		= (CQuoteListToolsDlg*)pParent;
}
// -------------------------------------------------------------------------

int	CMsgQueueManager::GetCount()
{
	return m_queue.GetCount();
}

