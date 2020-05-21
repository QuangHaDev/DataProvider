#if !defined(AFX_SOCKETSERVERDLG_H__5DFD1823_9896_11D6_9D02_0006294E7FE4__INCLUDED_)
#define AFX_SOCKETSERVERDLG_H__5DFD1823_9896_11D6_9D02_0006294E7FE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SocketServerDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSocketServerDlg dialog
class CQuoteListToolsDlg;
class CSocketServerDlg : public CDialog
{
// Construction
public:
	CSocketServerDlg(CWnd* pParent = NULL);   // standard constructor
	CQuoteListToolsDlg*	m_QuoteListToolsDlg;
	void	SetMyDialog(CQuoteListToolsDlg* tmpQuoteListToolsDlg) {m_QuoteListToolsDlg = tmpQuoteListToolsDlg;}
	void	SetDialog();

// Dialog Data
	//{{AFX_DATA(CSocketServerDlg)
	enum { IDD = IDD_SOCKET_SERVER_DIALOG };
	int		m_iPort;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSocketServerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSocketServerDlg)
	afx_msg void OnStart();
	afx_msg void OnStop();
	afx_msg void OnExit();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOCKETSERVERDLG_H__5DFD1823_9896_11D6_9D02_0006294E7FE4__INCLUDED_)
