// SocketServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "QuoteListTools.h"
#include "SocketServerDlg.h"
#include "QuoteListToolsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSocketServerDlg dialog


CSocketServerDlg::CSocketServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSocketServerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSocketServerDlg)
	m_iPort = 0;
	//}}AFX_DATA_INIT
}


void CSocketServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSocketServerDlg)
	DDX_Text(pDX, IDC_PORT, m_iPort);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSocketServerDlg, CDialog)
	//{{AFX_MSG_MAP(CSocketServerDlg)
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_STOP, OnStop)
	ON_BN_CLICKED(IDC_EXIT, OnExit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSocketServerDlg message handlers
//----------------------------------------------------------------------------

void CSocketServerDlg::OnStart() 
{
	if (UpdateData())
	{
		m_QuoteListToolsDlg->m_TestDCache->StartSocketServer(m_iPort);
		SetDialog();
	}
}
//----------------------------------------------------------------------------

void CSocketServerDlg::OnStop() 
{
	m_QuoteListToolsDlg->m_TestDCache->StopSocketServer();
	SetDialog();
}
//----------------------------------------------------------------------------

void CSocketServerDlg::OnExit() 
{
	CDialog::OnOK();
}
//----------------------------------------------------------------------------

BOOL CSocketServerDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	int		iServerPort;
	CString strTemp;
	HKEY hKey;
	DWORD dwEnableTCPServer = 0;
	DWORD dwSize = sizeof(dwEnableTCPServer);
	if ( RegOpenKeyEx(HKEY_LOCAL_MACHINE,
				 "SOFTWARE\\AFE\\DCache",
				 0,
				 KEY_QUERY_VALUE,
				 &hKey) == ERROR_SUCCESS )
	{
		if ( RegQueryValueEx( hKey, 
							  "ServerPort", 
							  NULL, 
							  NULL, 
							  (LPBYTE) &dwEnableTCPServer, 
							  &dwSize) == ERROR_SUCCESS )
        {
			iServerPort = dwEnableTCPServer ;
        }

		RegCloseKey(hKey);
	}	
	

	strTemp.Format("%d",iServerPort);
	SetDlgItemText(IDC_PORT, strTemp);
	SetDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//----------------------------------------------------------------------------

void CSocketServerDlg::SetDialog() 
{
	int iIDCNo;
	CString strStatus;
	if (m_QuoteListToolsDlg->m_TestDCache->GetSocketServerStatus())
	{
		strStatus = "ENABLED";
		CButton* Button = (CButton*)GetDlgItem(IDC_START);
		Button->EnableWindow(FALSE);
		Button = (CButton*)GetDlgItem(IDC_STOP);
		Button->EnableWindow(TRUE);
	}
	else
	{
		strStatus = "DISABLED";
		iIDCNo = IDC_STOP;
		CButton* Button = (CButton*)GetDlgItem(IDC_STOP);
		Button->EnableWindow(FALSE);
		Button = (CButton*)GetDlgItem(IDC_START);
		Button->EnableWindow(TRUE);
	}
	SetDlgItemText(IDC_SERVER_STATUS, strStatus);
}
