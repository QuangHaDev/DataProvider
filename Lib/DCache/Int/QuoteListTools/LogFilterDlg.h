#if !defined(AFX_LOGFILTERDLG_H__044BB1E6_D80C_11D5_9CA0_0006294E7FE4__INCLUDED_)
#define AFX_LOGFILTERDLG_H__044BB1E6_D80C_11D5_9CA0_0006294E7FE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LogFilterDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLogFilterDlg dialog
class CQuoteListToolsDlg;
class CLogFilterDlg : public CDialog
{
// Construction
public:
	CQuoteListToolsDlg*	m_QuoteListToolsDlg;
	CLogFilterDlg(CWnd* pParent = NULL);   // standard constructor
	void	SetMyDialog(CQuoteListToolsDlg* tmpQuoteListToolsDlg) {m_QuoteListToolsDlg = tmpQuoteListToolsDlg;}
	void	UpdateList(); 
	void	Init();

// Dialog Data
	//{{AFX_DATA(CLogFilterDlg)
	enum { IDD = IDD_FILTERDIALOG };
	CEdit	m_ctrlPageNo;
	CComboBox	m_FilterList;
	CString	m_strMsgType;
	CString	m_strPageNo;
	CString	m_strFidNo;
	CString	m_DPID;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogFilterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLogFilterDlg)
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCombo2();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGFILTERDLG_H__044BB1E6_D80C_11D5_9CA0_0006294E7FE4__INCLUDED_)
