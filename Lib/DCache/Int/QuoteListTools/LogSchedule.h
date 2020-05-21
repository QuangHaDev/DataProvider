#if !defined(AFX_LOGSCHDULE_H__003F82B1_56A6_11D6_9CD0_0006294E7FE4__INCLUDED_)
#define AFX_LOGSCHDULE_H__003F82B1_56A6_11D6_9CD0_0006294E7FE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LogSchdule.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLogSchedule dialog

class CLogSchedule : public CDialog
{
// Construction
public:
	CLogSchedule(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLogSchedule)
	enum { IDD = IDD_LOG_SCHEDULE_DIALOG };
	CString	m_strTo;
	CString	m_strFrom;
	int m_iTo;
	int m_iFrom;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogSchedule)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLogSchedule)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGSCHDULE_H__003F82B1_56A6_11D6_9CD0_0006294E7FE4__INCLUDED_)
