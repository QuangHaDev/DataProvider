// QuoteListTools.h : main header file for the QuoteListTools application
//

#if !defined(AFX_QuoteListTools_H__6D577565_D73F_11D5_9CA0_0006294E7FE4__INCLUDED_)
#define AFX_QuoteListTools_H__6D577565_D73F_11D5_9CA0_0006294E7FE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CQuoteListToolsApp:
// See QuoteListTools.cpp for the implementation of this class
//

class CQuoteListToolsApp : public CWinApp
{
public:
	CQuoteListToolsApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQuoteListToolsApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CQuoteListToolsApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QuoteListTools_H__6D577565_D73F_11D5_9CA0_0006294E7FE4__INCLUDED_)
