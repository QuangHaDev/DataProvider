#if !defined(AFX_SELTEXTDLG_H__678DEF23_DD68_11D5_9CA6_0006294E7FE4__INCLUDED_)
#define AFX_SELTEXTDLG_H__678DEF23_DD68_11D5_9CA6_0006294E7FE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelTextDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelTextDlg dialog

class CSelTextDlg : public CDialog
{
// Construction
public:
	CSelTextDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelTextDlg)
	enum { IDD = IDD_TEXT };
	CEdit	m_orig_text_box;
	CString	m_strOrgText;
	CString	m_strTextInNo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelTextDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CFont*m_pFont[2] ;
	// Generated message map functions
	//{{AFX_MSG(CSelTextDlg)
	afx_msg void OnTranslate();
	afx_msg void OnGB();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELTEXTDLG_H__678DEF23_DD68_11D5_9CA6_0006294E7FE4__INCLUDED_)
