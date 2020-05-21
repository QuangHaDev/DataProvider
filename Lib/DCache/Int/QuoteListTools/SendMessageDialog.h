#if !defined(AFX_SENDMESSAGEDIALOG_H__84FD7003_31A5_11D6_9CC3_0006294E7FE4__INCLUDED_)
#define AFX_SENDMESSAGEDIALOG_H__84FD7003_31A5_11D6_9CC3_0006294E7FE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendMessageDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSendMessageDialog dialog
class CQuoteListToolsDlg;
class CSendMessageDialog : public CDialog
{
// Construction
public:
	CSendMessageDialog(CWnd* pParent = NULL);   // standard constructor
	CQuoteListToolsDlg*	m_QuoteListToolsDlg;
	void	SetMyDialog(CQuoteListToolsDlg* tmpQuoteListToolsDlg) {m_QuoteListToolsDlg = tmpQuoteListToolsDlg;}
	void SendOneMessage(long lPageNo, byte bytMsgType);

// Dialog Data
	//{{AFX_DATA(CSendMessageDialog)
	enum { IDD = IDD_SEND_MSG_DIALOG };
	CEdit	m_ctrlRecType;
	CEdit	m_ctrlPage2;
	int		m_iSendInterval;
	int		m_iFidNo;
	CString	m_strFidValue;
	int		m_iRecType;
	CString	m_strPage;
	CString	m_strMsgType;
	CString	m_strPage2;
	BOOL	m_bRepeat;
	UINT    m_unTimer;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendMessageDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSendMessageDialog)
	afx_msg void OnSend();
	afx_msg void OnExit();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnRepeat();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDMESSAGEDIALOG_H__84FD7003_31A5_11D6_9CC3_0006294E7FE4__INCLUDED_)
