// LogSchedule.cpp : implementation file
//

#include "stdafx.h"
#include "QuoteListTools.h"
#include "LogSchedule.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLogSchedule dialog


CLogSchedule::CLogSchedule(CWnd* pParent /*=NULL*/)
	: CDialog(CLogSchedule::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLogSchedule)
	m_strTo = _T("");
	m_strFrom = _T("");
	//}}AFX_DATA_INIT
}


void CLogSchedule::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLogSchedule)
	DDX_Text(pDX, IDC_TO, m_strTo);
	DDX_Text(pDX, IDC_FROM, m_strFrom);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLogSchedule, CDialog)
	//{{AFX_MSG_MAP(CLogSchedule)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogSchedule message handlers

BOOL CLogSchedule::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if (m_strTo=="")
		m_strTo == "09:00"
	if (m_strFrom=="")
		m_strFrom == "16:00"

	SetDlgItemText(IDC_FROM, m_strTo);
	SetDlgItemText(IDC_TO, m_strFrom);	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
