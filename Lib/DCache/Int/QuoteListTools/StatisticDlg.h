#if !defined(AFX_STATISTICDLG_H__DE15B201_E892_11D5_9CAC_0006294E7FE4__INCLUDED_)
#define AFX_STATISTICDLG_H__DE15B201_E892_11D5_9CAC_0006294E7FE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StatisticDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStatisticDlg dialog
class CQuoteListToolsDlg;
class CStatisticDlg : public CDialog
{
// Construction
public:
	CStatisticDlg(CWnd* pParent = NULL);   // standard constructor
	CQuoteListToolsDlg*	m_QuoteListToolsDlg;
	void	SetMyDialog(CQuoteListToolsDlg* tmpQuoteListToolsDlg) {m_QuoteListToolsDlg = tmpQuoteListToolsDlg;}
	void	InsertPageItem(int iRow, CString strCounterName, int iValue);
	void	InsertTopRankingItem(int iRow, int iValue, int iCol = 0, BOOL bString = FALSE, CString strTemp = "");
	int		m_iTimer;
// Dialog Data
	//{{AFX_DATA(CStatisticDlg)
	enum { IDD = IDD_STAT_DIALOG };
	CStatic	m_SelectionTextCtrl;
	CTabCtrl	m_TabCtrl;
	CListCtrl	m_PageListCtrl;
	CStatic	m_PageStaticCtrl;
	CButton	m_RetrieveCtrl;
	CEdit	m_PageCtrl;
	CComboBox	m_ChoiceCtrl;
	CListCtrl	m_ListCtrl;
	CString	m_strPageNo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatisticDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CStatisticDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeCombo1();
	afx_msg void OnRetreiveButton();
	afx_msg void OnResetAll();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnRefresh();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATISTICDLG_H__DE15B201_E892_11D5_9CAC_0006294E7FE4__INCLUDED_)
