// LogFilterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "QuoteListTools.h"
#include "LogFilterDlg.h"
#include "QuoteListToolsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLogFilterDlg dialog


CLogFilterDlg::CLogFilterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLogFilterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLogFilterDlg)
	m_strMsgType = _T("");
	m_strPageNo = _T("");
	m_strFidNo = _T("");
	m_DPID = _T("");
	//}}AFX_DATA_INIT
}


void CLogFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLogFilterDlg)
	DDX_Control(pDX, IDC_PAGENO, m_ctrlPageNo);
	DDX_Control(pDX, IDC_COMBO2, m_FilterList);
	DDX_Text(pDX, IDC_MSGTYPE, m_strMsgType);
	DDX_Text(pDX, IDC_PAGENO, m_strPageNo);
	DDX_Text(pDX, IDC_FIDNO, m_strFidNo);
	DDX_Text(pDX, IDC_DPID, m_DPID);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLogFilterDlg, CDialog)
	//{{AFX_MSG_MAP(CLogFilterDlg)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_CBN_SELCHANGE(IDC_COMBO2, OnSelchangeCombo2)
	ON_BN_CLICKED(ID_CLOSE, OnClose)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogFilterDlg message handlers

void CLogFilterDlg::OnAdd() 
{
    UpdateData(); 
	CFilterStruct* tempFilter = new CFilterStruct;
	if (m_strPageNo=="")
		tempFilter->lPageNo = -1;
	else
	{
		if (m_strPageNo.GetLength() > 1)
		{
			if ((m_strPageNo.Left(2) == "0x")||(m_strPageNo.Left(2) == "0X"))
				sscanf(m_strPageNo, "%x", &tempFilter->lPageNo );
			else
				sscanf(m_strPageNo, "%d", &tempFilter->lPageNo );

		}
		else
			tempFilter->lPageNo = atoi(m_strPageNo);
	}

	tempFilter->FieldID = (m_strFidNo=="")?-1:atoi(m_strFidNo);
	tempFilter->bytMsgType = (m_strMsgType=="")?-1:atoi(m_strMsgType);
	tempFilter->sDPID = (m_DPID=="")?-1:atoi(m_DPID);
	m_QuoteListToolsDlg->m_FilterArray.Add(tempFilter);
	UpdateList();
	m_FilterList.SetCurSel(m_QuoteListToolsDlg->m_FilterArray.GetSize()-1);
	OnSelchangeCombo2();
	
	m_ctrlPageNo.SetSel( MAKELONG(0,-1) ) ;
	m_ctrlPageNo.SetFocus();
}
//----------------------------------------------------------------------------
void CLogFilterDlg::OnRemove() 
{
	if (m_QuoteListToolsDlg->m_FilterArray.GetSize()>0)
	{
		CFilterStruct* curFilter = (CFilterStruct*) m_QuoteListToolsDlg->m_FilterArray.GetAt(m_FilterList.GetCurSel());
		delete curFilter ;
		m_QuoteListToolsDlg->m_FilterArray.RemoveAt(m_FilterList.GetCurSel());
		UpdateList();
		m_FilterList.SetCurSel(0);
		OnSelchangeCombo2();
		m_ctrlPageNo.SetSel( MAKELONG(0,-1) ) ;
		m_ctrlPageNo.SetFocus();
	}
}
//----------------------------------------------------------------------------
void CLogFilterDlg::Init() 
{
	UpdateList();
	m_FilterList.SetCurSel(0);
	OnSelchangeCombo2();
}

//----------------------------------------------------------------------------
BOOL CLogFilterDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	Init();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//----------------------------------------------------------------------------
void CLogFilterDlg::UpdateList() 
{
	m_FilterList.ResetContent();
	for (int i = 0; i < m_QuoteListToolsDlg->m_FilterArray.GetSize(); i++)
	{
		CString strFilter;
		CFilterStruct* curFilter = (CFilterStruct*) m_QuoteListToolsDlg->m_FilterArray.GetAt(i);
		
		strFilter.Format("Page: %d, Fid: %d, MsgType: %d, DPID: %d", curFilter->lPageNo, curFilter->FieldID, 
						curFilter->bytMsgType, curFilter->sDPID);
		m_FilterList.InsertString(i, strFilter);
	}
}
//----------------------------------------------------------------------------
void CLogFilterDlg::OnSelchangeCombo2() 
{
	if (m_QuoteListToolsDlg->m_FilterArray.GetSize()>0)
	{
		CFilterStruct* curFilter = (CFilterStruct*) m_QuoteListToolsDlg->m_FilterArray.GetAt(m_FilterList.GetCurSel());
		m_strPageNo.Format("%d", curFilter->lPageNo);
		m_strFidNo.Format("%d", curFilter->FieldID );
		m_strMsgType.Format("%d", curFilter->bytMsgType);
		m_DPID.Format("%d", curFilter->sDPID);
		SetDlgItemText(IDC_PAGENO, m_strPageNo);
		SetDlgItemText(IDC_FIDNO, m_strFidNo);
		SetDlgItemText(IDC_MSGTYPE, m_strMsgType);
		SetDlgItemText(IDC_DPID, m_DPID);
		UpdateData();
	}
}

//----------------------------------------------------------------------------

void CLogFilterDlg::OnClose() 
{
	this->ShowWindow(SW_HIDE);
}
