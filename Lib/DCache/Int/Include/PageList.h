/////////////////////////////////////////////////////////////////////////////
//          File:: PageList.h
// Create Author:: Raymond Yung
// Create   Date:: 1 September 00
//
//   Description:: PageList Class Declaration
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


#ifndef __PAGELIST_H__
#define __PAGELIST_H__
/////////////////////////////////////////////////////////////////////////////
//
// Class      : 
// Description:
//

class CPageList : public CObject
{
	DECLARE_DYNCREATE(CPageList)

public :
	// -------------------------------------------------------------------------
	// Default Constructor
	CPageList();
	CPageList(unsigned long lNoOfPage);
	// -------------------------------------------------------------------------
	// Destructor
	virtual ~CPageList();

// Operations
	void SetPages(CString* pstrPages);
	void Add(unsigned long lPageNo);
	void Remove(unsigned long lPageNo);
	BOOL IsIn(unsigned long lPageNo);

private :
// Attributes21

	char *m_pacPageList;
	unsigned long m_lNoOfPages;
};

#endif  // __PAGELIST_H__

