// QuoteListToolsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "QuoteListTools.h"
#include "QuoteListToolsDlg.h"
#include "DTFidIterator.h"
#include "QTFidIterator.h"
#include "LogFilterDlg.h"
#include "SelTextDlg.h"
#include "StatisticDlg.h"
#include "JRegistry.h"
#include "MsgQueueManager.h"
#include "SendMessageDialog.h"
#include "LogSchedule.h"
#include "SocketServerDlg.h" 

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{  
public:
	CAboutDlg();   
   
// Dialog Data 
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP() 
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQuoteListToolsDlg dialog

CQuoteListToolsDlg::CQuoteListToolsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CQuoteListToolsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CQuoteListToolsDlg)
	m_unPort = 6000;
	m_sPageNo = _T("");
	m_strFid = _T("");
	m_bLogWithTime = FALSE;
	m_bLogStatisticData = FALSE;
	m_unTCPPort = 0;
	m_bShowMsgDetail = FALSE;
	m_bShowMsgInfo = FALSE;
	m_strGroup = _T("");
	m_unSendPort = 0;
	m_strChannel = _T("");  
	m_bDisplayUpdateOnly = FALSE;
	m_bStartToRetrieveTCPData = FALSE;
	m_bPutDataToCache	= TRUE;
	m_bDCacheDisplay	= TRUE;
	m_bStatusDisplay	= TRUE; 
	m_bLogDCacheFile	= FALSE;
	m_bLogStatusFile	= FALSE;
	m_bDataCtrlReceivedData		= FALSE;
	m_bTCPCtrlReceivedData		= FALSE;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_iListChoice = -1;
	m_iCurFidNo = 0;
	m_lCurPageNo = 0;

	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
}

void CQuoteListToolsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQuoteListToolsDlg)
	DDX_Control(pDX, ID_INDICATOR_STATUS2, m_StatusOK2);
	DDX_Control(pDX, IDC_ANIMATE2, m_StatusAniCtrl2);
	DDX_Control(pDX, IDC_GROUPCOMBO2, m_ChannelCombo);
	DDX_Control(pDX, IDC_IPADDRESS2, m_ipLocalSend);
	DDX_Control(pDX, IDC_GROUPCOMBO, m_GroupCombo);
	DDX_Control(pDX, IDC_STATUSLISTFLAME, m_btnStatusListFrame);
	DDX_Control(pDX, IDC_MSG_DETAIL_CHECK, m_ShowMsgDetailButton);
	DDX_Control(pDX, IDC_IPADDRESS3, m_ipRetrieve);
	DDX_Control(pDX, ID_INDICATOR_STATUS, m_StatusOK);
	DDX_Control(pDX, IDC_ANIMATE1, m_StatusAniCtrl);
	DDX_Control(pDX, IDC_PAGENO, m_PageCtrl);
	DDX_Control(pDX, IDC_LIST2, m_Statuslist);
	DDX_Control(pDX, IDC_LIST1, m_DCachelist);
	DDX_Control(pDX, IDC_LIST3, m_QuoteList);
	DDX_Control(pDX, IDC_IPADDRESS1, m_ipLocal);
	DDX_Text(pDX, IDC_PORT, m_unPort);
	DDX_Text(pDX, IDC_PAGENO, m_sPageNo);
	DDX_Text(pDX, IDC_FID, m_strFid);
	DDX_Check(pDX, IDC_TIMECHECK, m_bLogWithTime);
	DDX_Check(pDX, IDC_STATISTIC_CHECK, m_bLogStatisticData);
	DDX_Text(pDX, IDC_PORT2, m_unTCPPort);
	DDX_Check(pDX, IDC_MSG_DETAIL_CHECK, m_bShowMsgDetail);
	DDX_Check(pDX, IDC_SHOW_INFO_CHECK, m_bShowMsgInfo);
	DDX_CBString(pDX, IDC_GROUPCOMBO, m_strGroup);
	DDX_Text(pDX, IDC_PORT3, m_unSendPort);
	DDX_CBString(pDX, IDC_GROUPCOMBO2, m_strChannel);
	DDX_Check(pDX, IDC_UPDATE_CHECK, m_bDisplayUpdateOnly);
	DDX_CBIndex(pDX, IDC_LISTCHOICE_COMBO, m_iListChoice);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CQuoteListToolsDlg, CDialog)
	//{{AFX_MSG_MAP(CQuoteListToolsDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_STOP, OnStop)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_PUTDATATOCACHE, OnPutdatatocache)
	ON_BN_CLICKED(IDC_DISPLAY2, OnStatusDisplay)
	ON_BN_CLICKED(IDC_DISPLAY1, OnDcacheDisplay)
	ON_BN_CLICKED(IDC_OUTPUTTOFILE, OnOutputtofile)
	ON_BN_CLICKED(IDC_OUTPUTTOFILE2, OnOutputtofile2)
	ON_BN_CLICKED(IDC_CLEAR1, OnDcacheClear)
	ON_BN_CLICKED(IDC_CLEAR2, OnStatusClear)
	ON_BN_CLICKED(IDC_SEARCH, OnSearch)
	ON_BN_CLICKED(IDC_FILTER, OnLogFilter)
	ON_BN_CLICKED(IDC_DROP, OnDropPage)
	ON_BN_CLICKED(IDC_LISTALLPAGES, OnListAllPages)
	ON_BN_CLICKED(IDC_LISTALLDROPPAGES, OnListAllDropPages)
	ON_BN_CLICKED(IDC_LIST_ALL_FIDS, OnListAllFids)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST2, OnDblclkStatusList)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkDcacheList)
	ON_BN_CLICKED(IDC_TEST_DROP, OnTestDrop)
	ON_BN_CLICKED(IDC_TEST, OnTest)
	ON_BN_CLICKED(IDC_LIST_ALL_DROP_FID, OnListAllDropFid)
	ON_BN_CLICKED(IDC_SAVE_DATA, OnSaveData)
	ON_BN_CLICKED(IDC_RETRIEVE_DATA, OnRetrieveData)
	ON_BN_CLICKED(IDC_SHOW_STAT, OnShowStat)
	ON_BN_CLICKED(IDC_TIMECHECK, OnTimecheck)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_STATISTIC_CHECK, OnStatisticCheck)
	ON_BN_CLICKED(IDC_RETRIEVE, OnRetrieve)
	ON_BN_CLICKED(IDC_MSG_DETAIL_CHECK, OnMsgDetailCheck)
	ON_BN_CLICKED(IDC_SHOW_INFO_CHECK, OnShowInfoCheck)
	ON_WM_SIZE()
	ON_BN_CLICKED(ID_LOAD_SETTING, OnLoadSetting)
	ON_BN_CLICKED(ID_SAVE_SETTING, OnSaveSetting)
	ON_BN_CLICKED(IDC_START2, OnSendStart)
	ON_BN_CLICKED(IDC_STOP2, OnSendStop)
	ON_BN_CLICKED(IDC_ADD2, OnAddChannel)
	ON_BN_CLICKED(IDC_REMOVE2, OnRemoveChannel)
	ON_BN_CLICKED(IDC_SEND, OnSendMessage)
	ON_BN_CLICKED(IDC_RETRIEVE_STOP, OnRetrieveStop)
	ON_BN_CLICKED(IDC_AUTO_2, OnAutoLog2)
	ON_BN_CLICKED(IDC_TCP_DCACHE, OnTcpDcache)
	ON_BN_CLICKED(IDC_SOCKET, OnSocketServer)
	ON_BN_CLICKED(ID_EXIT, OnExit)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_REMOVE_ALL, OnRemoveAll)
	ON_BN_CLICKED(IDC_SHOWQUOTELIST, OnShowquotelist)
	ON_CBN_SELCHANGE(IDC_LISTCHOICE_COMBO, OnSelchangeListchoiceCombo)
	ON_BN_CLICKED(IDC_REGISTER, OnRegister)
	//ON_MESSAGE(WM_STOP_TCPCTRL, CQuoteListToolsDlg::StopTCPCtrl)
	//ON_MESSAGE(WM_STOP_DATACTRL, CQuoteListToolsDlg::StopDataCtrl)
	//ON_MESSAGE(WM_RESUME_STATUS, CQuoteListToolsDlg::ResumeStatus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/*

*/
/////////////////////////////////////////////////////////////////////////////
// CQuoteListToolsDlg message handlers

BOOL CQuoteListToolsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	m_font.CreatePointFont(90, "Courier New");
	m_TotalSize = 0;
	m_TotalMessages = 0;
	m_UpdateTotalSize = 0;
	m_UpdateTotalMessages = 0;
	m_VSTotalSize = 0;
	m_VSTotalMessages = 0;
	m_bStart = FALSE;
	m_bShowQuoteList = FALSE;

	CButton* CheckBoxButton = (CButton*)GetDlgItem(IDC_PUTDATATOCACHE);
	CheckBoxButton->SetCheck(TRUE);
	DWORD nBufferLength=100;
	char PathName[100];       
	char FileName[100];       
	GetCurrentDirectory(nBufferLength,  PathName);
	sprintf(FileName, "%s\\Data\\", PathName);
	CString strOutput;
	if (SetCurrentDirectory(FileName))
	{
		strOutput.Format("The data files will be saved in %s", FileName);
		SetDlgItemText(IDC_FILE_MSG,strOutput);
		SetCurrentDirectory(PathName);
	}
	else
	{
		if (CreateDirectory(FileName, NULL))
		{
			strOutput.Format("The data files will be saved in %s", FileName);
			SetDlgItemText(IDC_FILE_MSG, strOutput);
		}
		else
		{
			strOutput.Format("The data files cannot be saved"); 
			SetDlgItemText(IDC_FILE_MSG, strOutput);
		} 
	}
	sprintf(FileName, "%s\\Data\\DCache", PathName); 
	m_TestDCache = new CDCache(TRUE, FileName, NULL, 7200, TRUE);

//	CWordArray cwaSaveData;
//	cwaSaveData.Add(1);
//	m_TestDCache  = new CDCache(TRUE, FileName, &cwaSaveData);

//	m_TestDCache->AddAFieldWatcher((FID_DEFINITION)1520, 0, 0 , 0, (CObject *)this, Send);
//	m_TestDCache->AddAAnyWatcher((CObject *)this, Send);
// 	m_TestDCache->AddAPageWatcher(7811, (CObject *)this, Send);

    JRegistry reg;
	m_port=6000;
	char ip_address[20];
	char port[20];
	memset( ip_address, 0x00, 20 );
	unsigned long len=20; 
	m_ip1=192; m_ip2=168; m_ip3=21; m_ip4=35; // DEBUG
	m_rcvip1=225; m_rcvip2=1; m_rcvip3=32; m_rcvip4=88; // DEBUG	
	m_tcpip1=192; m_tcpip2=168; m_tcpip3=20; m_tcpip4=74;	

	// Receive Init Start
	if (reg.key_exists("SOFTWARE\\AFE\\QuoteListTools")!=1)
		reg.add_key("SOFTWARE\\AFE\\QuoteListTools");
	//reg.read("SOFTWARE\\AFE\\QuoteListTools", "IP_Address", ip_address, &len );
	reg.read("SOFTWARE\\AFE\\QuoteListTools","PortNumber", &m_port );
	
	sprintf(port, "%d", m_port);
	SetDlgItemText(IDC_PORT, port);
	if (ip_address[0]!=0)
		sscanf(ip_address, "%d.%d.%d.%d", &m_ip1, &m_ip2, &m_ip3, &m_ip4);
	m_ipLocal.SetAddress(m_ip1, m_ip2, m_ip3, m_ip4);

	memset( ip_address, 0x00, 20 );
	len=20;
	//reg.read("SOFTWARE\\AFE\\QuoteListTools", "Group_IP_Address", ip_address, &len);
	if (ip_address[0]==0)
		sprintf(ip_address, "225.1.32.88"); //DEBUG
	m_GroupArray.Add(ip_address);
	UpdateList();
	m_GroupCombo.SetCurSel(m_GroupArray.GetSize()-1);
	// Receive Init End

	// Send Init Start
	memset( ip_address, 0x00, 20 );
	len=20;
	//reg.read("SOFTWARE\\AFE\\QuoteListTools", "IP_AddressSend", ip_address, &len );
	reg.read ("SOFTWARE\\AFE\\QuoteListTools","SendPortNumber", &m_port );
	
	sprintf(port, "%d", m_port);
	SetDlgItemText(IDC_PORT3, port);
	if (ip_address[0]!=0)
		sscanf(ip_address, "%d.%d.%d.%d", &m_ip1, &m_ip2, &m_ip3, &m_ip4);
	m_ipLocalSend.SetAddress(m_ip1, m_ip2, m_ip3, m_ip4);

	memset( ip_address, 0x00, 20 );
	len=20;
	//reg.read("SOFTWARE\\AFE\\QuoteListTools", "Channel_IP_Address", ip_address, &len);
	if (ip_address[0]==0)
		sprintf(ip_address, "225.1.1.14");
	m_ChannelArray.Add(ip_address);
	UpdateChannelList();
	m_ChannelCombo.SetCurSel(m_ChannelArray.GetSize()-1);

	// Send Init End


	m_tcpport=10014;
	len = 20;
	if (reg.key_exists("SOFTWARE\\AFE\\QuoteListTools")!=1)
		reg.add_key("SOFTWARE\\AFE\\QuoteListTools");
	//reg.read("SOFTWARE\\AFE\\QuoteListTools", "IP_TCPAddress", ip_address, &len );
	reg.read ("SOFTWARE\\AFE\\QuoteListTools","TCPPortNumber", &m_tcpport );
	
	sprintf(port, "%d", m_tcpport);
	SetDlgItemText(IDC_PORT2, port);
	if (ip_address[0]!=0)
		sscanf(ip_address, "%d.%d.%d.%d", &m_tcpip1, &m_tcpip2, &m_tcpip3, &m_tcpip4);
	m_ipRetrieve.SetAddress(m_tcpip1, m_tcpip2, m_tcpip3, m_tcpip4);


	
	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	DWORD dwStyle;	
	COLORREF  m_clrBack = GetSysColor(COLOR_3DFACE);

	m_Statuslist.ModifyStyle(LVS_NOSORTHEADER|LVS_SINGLESEL, LVS_SHOWSELALWAYS|LVS_SORTASCENDING);
	dwStyle = m_Statuslist.GetExtendedStyle();
	dwStyle |= (LVS_EX_ONECLICKACTIVATE&LVS_EX_TRACKSELECT|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP);
	m_Statuslist.SetExtendedStyle(dwStyle);
	m_Statuslist.SetCols(2, TRUE, LVCFMT_LEFT,1000);
	m_Statuslist.SetColWidth(1, 0);
	m_Statuslist.SetCodePage(950);
	m_Statuslist.SetBackColor(m_clrBack);
	m_Statuslist.SetFont(&m_font);
	
	m_DCachelist.ModifyStyle(LVS_NOSORTHEADER|LVS_SINGLESEL, LVS_SHOWSELALWAYS|LVS_SORTASCENDING);
	dwStyle = m_DCachelist.GetExtendedStyle();
	dwStyle |= (LVS_EX_ONECLICKACTIVATE&LVS_EX_TRACKSELECT|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP);
	m_DCachelist.SetExtendedStyle(dwStyle);
	m_DCachelist.SetCols(2, TRUE, LVCFMT_LEFT,1000);
	m_DCachelist.SetColWidth(1, 0);
	m_DCachelist.SetCodePage(950);
	m_DCachelist.SetBackColor(m_clrBack);
	m_DCachelist.SetFont(&m_font);

    // initialize the log file's name
    char     file_name[20];
	char	 path[130];
	strcpy( file_name, "Dcache.log" );
	::sprintf(path, "%s\\%s", PathName, file_name);
	SetDlgItemText(IDC_FILENAME, path);
	strcpy( file_name, "Status.log" );
	::sprintf(path, "%s\\%s", PathName, file_name);
	SetDlgItemText(IDC_FILENAME2, path);

   	m_iTimer = SetTimer( 1, 10000, NULL );
   	m_iTimer = SetTimer( 2, 10000, NULL );
   	m_iTimer = SetTimer( 3, 5000, NULL );
   	m_iTimer = SetTimer( 4, 2000, NULL );
   	m_iTimer = SetTimer( 5, 100, NULL );

	m_StatusAniCtrl.Open(IDR_RED_FLASHING);
	m_StatusAniCtrl.ShowWindow(SW_SHOW);
	m_StatusOK.ShowWindow(SW_HIDE);

	m_StatusAniCtrl2.Open(IDR_RED_FLASHING);
	m_StatusAniCtrl2.ShowWindow(SW_SHOW);
	m_StatusOK2.ShowWindow(SW_HIDE);
	m_MyQueue = new CMsgQueueManager();
	m_MyQueue->Init(this);
	
	m_ShowMsgDetailButton.SetCheck(TRUE);
	m_dlgLogFilter = new CLogFilterDlg;
	m_dlgLogFilter->SetMyDialog(this);
    m_dlgLogFilter->Create(CLogFilterDlg::IDD, this);
	m_dlgLogFilter->ShowWindow(SW_HIDE);

	m_dlgSendMessage = new CSendMessageDialog;
	m_dlgSendMessage->SetMyDialog(this);
    m_dlgSendMessage->Create(CSendMessageDialog::IDD, this);
	m_dlgSendMessage->ShowWindow(SW_HIDE);

	m_DCachelist.ShowWindow(SW_HIDE );
	m_Statuslist.ShowWindow(SW_SHOW );
	m_QuoteList.ShowWindow(SW_HIDE );
	CComboBox*	ChoiceCtrl = (CComboBox*)GetDlgItem(IDC_LISTCHOICE_COMBO);
	ChoiceCtrl->SetCurSel(2);
	UpdateData();
	return TRUE;  // return TRUE  unless you set the focus to a control
}
//---------------------------------------------------------------------------------
void CQuoteListToolsDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}
//---------------------------------------------------------------------------------
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CQuoteListToolsDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}
//---------------------------------------------------------------------------------
// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CQuoteListToolsDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

//---------------------------------------------------------------------------------

void CQuoteListToolsDlg::OnStart() 
{
    UpdateData();

    m_ipLocal.GetAddress(m_usIP[0], m_usIP[1], m_usIP[2], m_usIP[3]);

    CString s, csMsg;
    s.Format("%d.%d.%d.%d", m_usIP[0], m_usIP[1], m_usIP[2], m_usIP[3]);
    if (m_DataCtrl.Start(m_unPort, s)  == CMCast::MC_SUCCESS)
	{
		JRegistry reg;
		char ip_address[20];
		sprintf(ip_address, "%s", s.GetBuffer(0));
		reg.write_value("SOFTWARE\\AFE\\QuoteListTools", "IP_Address", REG_SZ, (const unsigned char *)&ip_address, strlen(ip_address));
		reg.write("SOFTWARE\\AFE\\QuoteListTools","PortNumber", m_unPort);

        csMsg.Format("DATACTRL started! - IP:%s  Port:%d", s.GetBuffer(0), m_unPort);
	    m_DataCtrl.RegisterCallback(DataCtrlPutDataToDCache, this);
	}
    else
        csMsg = "DATACTRL start error!";

	m_bStart = TRUE;
	Log(csMsg, TRUE);	
}

//---------------------------------------------------------------------------------

void CQuoteListToolsDlg::Log(CString strText, BOOL bStatus, int iCodePage)
{
	CABTPanelEx*	clbList = (bStatus)?&m_Statuslist:&m_DCachelist;
	BOOL		bDisplay = (bStatus)?m_bStatusDisplay:m_bDCacheDisplay;
	BOOL		bLogToFile = (bStatus)?m_bLogStatusFile:m_bLogDCacheFile;
	strText.TrimLeft(' ');
	if ((strText!="") && m_bLogWithTime)
		strText.Insert(0, GetFormattedSystemTime());

	if (bDisplay)
		SetListBoxData(strText, clbList, iCodePage);

	if (bLogToFile)
		LogDataToFile(strText, bStatus);
}
//---------------------------------------------------------------------------------

void CQuoteListToolsDlg::SetListBoxData(CString strText, CABTPanelEx* clbList, int iCodePage)
{
	clbList->SetCodePage(iCodePage);
	clbList->InsertItem(0, "");
	clbList->SetRedraw();
	clbList->SetItemTextEx(0, 0, strText);
	clbList->SetRedraw();
}

//---------------------------------------------------------------------------------
void CQuoteListToolsDlg::LogDataToFile(CString strText, BOOL bStatus)
{
	char log_msg[10000];
	sprintf( log_msg, "%s\n", strText.GetBuffer(0) );
	int len = strlen(log_msg);

	if (bStatus)
	    m_StatusFile.Write(log_msg, len);
	else
		m_DCacheFile.Write(log_msg, len);
}
//---------------------------------------------------------------------------------

void CQuoteListToolsDlg::OnStop() 
{
	Log("Stop DATACTRL!!", TRUE);	
	m_bStatusDisplay = FALSE; 
	PostMessage(WM_STOP_DATACTRL);
	PostMessage(WM_RESUME_STATUS);
}
//---------------------------------------------------------------------------------

void CQuoteListToolsDlg::ResumeStatus(UINT wParam, LONG lParam) 
{
 	CString strCurrent;
	GetDlgItemText(IDC_DISPLAY2, strCurrent);
	m_bStatusDisplay  = (strCurrent != "Display")?TRUE:FALSE;

}

//---------------------------------------------------------------------------------
void CQuoteListToolsDlg::StopDataCtrl(UINT wParam, LONG lParam)
{
	CString str; 
    unsigned char a, b, c, d;
    CString csLocal;

    UpdateData();
    m_ipLocal.GetAddress(a,b,c,d);
    csLocal.Format("%d.%d.%d.%d", a, b, c, d);

	for (int i = 0; i< m_GroupCombo.GetCount();i++)
	{
		m_GroupCombo.GetLBText(i, str);
		m_DataCtrl.DelGroup(str.GetBuffer(0), csLocal);
	}
	m_DataCtrl.UnregisterCallback(DataCtrlPutDataToDCache, this);
	m_DataCtrl.Stop();
	m_bStart = FALSE;
}
//---------------------------------------------------------------------------------

void CQuoteListToolsDlg::OnAdd() 
{
    unsigned char a, b, c, d;
    CString csLocal, csMsg;

    UpdateData();
    m_ipLocal.GetAddress(a,b,c,d);
    csLocal.Format("%d.%d.%d.%d", a, b, c, d);

    if (m_DataCtrl.AddGroup(m_strGroup, csLocal) == CDataCtrl::DC_ERROR)
        csMsg.Format("Add group %s failed! - IP:%s", m_strGroup, csLocal);
    else 
	{
		JRegistry reg;
		char ip_address[20];
		sprintf(ip_address, "%s", m_strGroup.GetBuffer(0));
		reg.write_value("SOFTWARE\\AFE\\QuoteListTools", "Group_IP_Address", REG_SZ, (const unsigned char *)&ip_address, strlen(ip_address));
        csMsg.Format("Add group %s - IP:%s", m_strGroup, csLocal);
		BOOL bFound = FALSE;
		int iTotalGroup = m_GroupArray.GetSize();
		if (iTotalGroup>0)
		{
			while (iTotalGroup>0)
			{
				iTotalGroup--;
				if (m_GroupArray.GetAt(iTotalGroup)==m_strGroup)
				{
					bFound = TRUE;
					break;
				}
			}
		}
		if (!bFound)
			m_GroupArray.Add(m_strGroup);
		UpdateList();
		m_GroupCombo.SetCurSel(m_GroupArray.GetSize()-1);
		OnTimer(1);
	}
    Log(csMsg, TRUE);	
}

//----------------------------------------------------------------------------
void CQuoteListToolsDlg::UpdateList() 
{
	m_GroupCombo.ResetContent();
	for (int i = 0; i < m_GroupArray.GetSize(); i++)
		m_GroupCombo.InsertString(i, m_GroupArray.GetAt(i));
}

//---------------------------------------------------------------------------------
void CQuoteListToolsDlg::DataCtrlPutDataToDCache(COP_ITEM& copMsg, void* instance)
{
	CQuoteListToolsDlg* Myself = (CQuoteListToolsDlg*)instance;
	Myself->m_bDataCtrlReceivedData = TRUE;
	Myself->PutDataToDCache(copMsg);
	
}
//---------------------------------------------------------------------------------
void CQuoteListToolsDlg::TCPCtrlPutDataToDCache(COP_ITEM& copMsg, void* instance)
{
	CQuoteListToolsDlg* Myself = (CQuoteListToolsDlg*)instance;
	Myself->m_bTCPCtrlReceivedData = TRUE;
	Myself->PutDataToDCache(copMsg);
}
//---------------------------------------------------------------------------------

void CQuoteListToolsDlg::PutDataToDCache(COP_ITEM& copMsg)
{
	if (m_bLogStatisticData)
		m_MyQueue->Add(copMsg);
	BOOL bShowLog = FALSE;
	CString strTemp, strOutput, strOutput1;

/*	if (m_bTestDrop)
	{
		CString strOutput;
		COP_ITEM pMsgObj;
		MsgObject* MsgObj = new MsgObject();
		MsgObj->SetMsgType(MGT_DROP);	
		MsgObj->SetItemNo(copMsg->GetItemNo());
		strOutput.Format("Sent Drop Page - %d to Channel 225.1.1.14 port 6000", copMsg->GetItemNo());
		pMsgObj = new MsgObject(MsgObj->GetMessage());
		m_DataCtrl.Send(pMsgObj);
		Log(strOutput, TRUE);
		Log("  ", TRUE);
	}
*/
	if (m_bPutDataToCache)
	{
		m_TestDCache->PutDataIntoDCache(copMsg);
	}

	if ( m_FilterArray.GetSize()>0)
	{
		int iNoOfFid = 0;
		for (int i = 0; i < m_FilterArray.GetSize(); i++)
		{

			strOutput.Format("ItemNo: %d, MsgLen: %d, Fid: ", copMsg->GetItemNo(), copMsg->GetMsgLen());

			CFilterStruct* curFilter = (CFilterStruct*) m_FilterArray.GetAt(i);
			if (((curFilter->lPageNo == copMsg->GetItemNo()) || (curFilter->lPageNo == -1)) && (copMsg->GetItemNo()!=10001000))
			{
				if ((curFilter->sDPID==copMsg->GetDPID())||(curFilter->sDPID==-1))
				{
					if ((curFilter->bytMsgType==copMsg->GetMsgType())||(curFilter->bytMsgType==-1))
					{
						CDTFidIterator PI(copMsg->GetMessage());
						PI.SetMyDialog(this);
						PI.SetInit(m_bShowQuoteList, TRUE, curFilter->FieldID);
						PI.First();
						while (!PI.IsEnd()) 
						{
							BOOL bUpdated = TRUE;
							iNoOfFid ++;
							if (m_bDisplayUpdateOnly)
								bUpdated = m_TestDCache->IsFidUpdated((FID_DEFINITION)PI.GetCurFidCode());

							if (bUpdated)
							{
								if ((curFilter->FieldID==PI.GetCurFidCode())||(curFilter->FieldID==-1))
								{
									if (m_bShowMsgDetail)
										PI.ProcessFid();
									bShowLog = TRUE;
								}
								strTemp.Format("%d, ", PI.GetCurFidCode());
								strOutput += strTemp;
							}
							PI.Next();
						}
					}
				}
			}
		}
		if ((m_bShowMsgInfo) && (bShowLog))
		{
			strOutput1.Format("MsgType: %d, DPID: %d, DPInsID: %d, SEQ NO: %d, FidCount: %d", copMsg->GetMsgType(), 
								copMsg->GetDPID(), copMsg->GetDPInsID(), copMsg->GetSeqNo(), iNoOfFid);
			Log(strOutput, TRUE);
			Log(strOutput1, TRUE);	
			Log("", TRUE);
		}
	}
}


//---------------------------------------------------------------------------------


void CQuoteListToolsDlg::OnRemove() 
{
    unsigned char a, b, c, d;
    CString csLocal, csMsg;

    UpdateData();
    m_ipLocal.GetAddress(a,b,c,d);
    csLocal.Format("%d.%d.%d.%d", a, b, c, d);

    if (m_DataCtrl.DelGroup(m_strGroup, csLocal) == CDataCtrl::DC_ERROR)
        csMsg.Format("Remove group %s failed! - IP:%s", m_strGroup, csLocal);
    else 
        csMsg.Format("Remove group %s - IP:%s", m_strGroup, csLocal);

	int iTotalGroup = m_GroupArray.GetSize();
	if (iTotalGroup>0)
	{
		while (iTotalGroup>0)
		{
			iTotalGroup--;
			if (m_GroupArray.GetAt(iTotalGroup)==m_strGroup)
			{
				m_GroupArray.RemoveAt(iTotalGroup);
				break;
			}
		}
	}
	UpdateList(); 
	if (m_GroupArray.GetSize()>0)
		m_GroupCombo.SetCurSel(m_GroupArray.GetSize()-1);
    Log(csMsg, TRUE);	
}

//---------------------------------------------------------------------------------

void CQuoteListToolsDlg::OnPutdatatocache() 
{
	CButton* CheckBoxButton = (CButton*)GetDlgItem(IDC_PUTDATATOCACHE);
	ASSERT (CheckBoxButton!= NULL);
			
	if (CheckBoxButton->GetCheck())
 		m_bPutDataToCache = TRUE;
	else
 		m_bPutDataToCache = FALSE;
}

//---------------------------------------------------------------------------------

void CQuoteListToolsDlg::OnCancel() 
{
}

//---------------------------------------------------------------------------------
void CQuoteListToolsDlg::OnStatusDisplay() 
{
	CString strDontDisplay = "Don't Display";
	CString strDisplay = "Display";
	CString strCurrent;
	GetDlgItemText(IDC_DISPLAY2, strCurrent);
	if (strCurrent == strDontDisplay)
	{
		SetDlgItemText(IDC_DISPLAY2, strDisplay);
		m_bStatusDisplay  = FALSE; 
	}
	else
	{
		SetDlgItemText(IDC_DISPLAY2, strDontDisplay);
		m_bStatusDisplay  = TRUE; 
	}
}
//---------------------------------------------------------------------------------

void CQuoteListToolsDlg::OnDcacheDisplay() 
{
	CString strDontDisplay = "Don't Display";
	CString strDisplay = "Display";
	CString strCurrent;
	GetDlgItemText(IDC_DISPLAY1, strCurrent);
	if (strCurrent == strDontDisplay)
	{
		SetDlgItemText(IDC_DISPLAY1, strDisplay);
		m_bDCacheDisplay  = FALSE; 
	}
	else
	{
		SetDlgItemText(IDC_DISPLAY1, strDontDisplay);
		m_bDCacheDisplay  = TRUE; 
	}
}
//---------------------------------------------------------------------------------

void CQuoteListToolsDlg::OnOutputtofile() 
{
	CButton* CheckBoxButton = (CButton*)GetDlgItem(IDC_OUTPUTTOFILE);
	ASSERT (CheckBoxButton!= NULL);
			
	if (CheckBoxButton->GetCheck())
	{
		m_bLogDCacheFile = TRUE;
		char file_name[100];
		GetDlgItemText(IDC_FILENAME, file_name, 100);
		m_DCacheFile.Open(file_name, JFile::W_Plus);
    }
	else
	{
		m_DCacheFile.Close();
		m_bLogDCacheFile = FALSE;
	}
}
//---------------------------------------------------------------------------------

void CQuoteListToolsDlg::OnOutputtofile2() 
{
	CButton* CheckBoxButton = (CButton*)GetDlgItem(IDC_OUTPUTTOFILE2);
	ASSERT (CheckBoxButton!= NULL);
			
	if (CheckBoxButton->GetCheck())
	{
		CButton* CheckBoxButton2 = (CButton*)GetDlgItem(IDC_AUTO_2);
		if (m_bLogStatusFile)
			m_StatusFile.Close();

		if (CheckBoxButton2->GetCheck())
		{
			DWORD nBufferLength=100;
			char PathName[100];       
			char file_name[100];
			SYSTEMTIME sysTime;

			CTime Today = CTime::GetCurrentTime();
			int iNow = Today.GetHour()*60+Today.GetMinute();
			if (((iNow<m_iTo) && (iNow>m_iFrom)))
				m_bLogStatusFile = TRUE;
			GetSystemTime(&sysTime);
			CString strToday;
			GetCurrentDirectory(nBufferLength,  PathName);
			strToday.Format(_T("%s\\Data\\%04d_%02d_%02d.log"), PathName, sysTime.wYear, sysTime.wMonth, sysTime.wDay);
			sprintf((char*) &file_name, "%s", strToday.GetBuffer(0));
			m_StatusFile.Open(file_name, JFile::A_Plus);
		}
		else
		{
			m_bLogStatusFile = TRUE;
			char file_name[100];
			GetDlgItemText(IDC_FILENAME2, file_name, 100);
			m_StatusFile.Open(file_name, JFile::W_Plus);
		}
    }
	else
 	{
		if (m_bLogStatusFile)
			m_StatusFile.Close();
		m_bLogStatusFile = FALSE;
	}
}

//---------------------------------------------------------------------------------
void CQuoteListToolsDlg::OnAutoLog2() 
{
	CButton* CheckBoxButton = (CButton*)GetDlgItem(IDC_AUTO_2);
	CEdit * pEditBox = (CEdit *)this->GetDlgItem(IDC_FILENAME2);
	
	if (CheckBoxButton->GetCheck())
	{
		CLogSchedule dlg;
		dlg.m_strFrom = m_strLogFrom;
		dlg.m_strTo = m_strLogTo;
		if (dlg.DoModal())
		{
			m_strLogFrom = dlg.m_strFrom;
			m_strLogTo = dlg.m_strTo;
			m_iFrom = dlg.m_iFrom;
			m_iTo = dlg.m_iTo;
		}
		pEditBox->EnableWindow(FALSE);

	}
	else
		pEditBox->EnableWindow(TRUE);

	OnOutputtofile2();
}

//---------------------------------------------------------------------------------

void CQuoteListToolsDlg::OnDcacheClear() 
{
    m_DCachelist.DeleteAllItems();
}
//---------------------------------------------------------------------------------

void CQuoteListToolsDlg::OnStatusClear() 
{
    m_Statuslist.DeleteAllItems();
}
//---------------------------------------------------------------------------------
BOOL CQuoteListToolsDlg::LogOnePageData(long lPageNo, int iFidNo) 
{
	COP_ITEM copMsg;
	CDWordArray cdwaFIDList;
	fidDatabase fDB;
	BOOL bLog = FALSE;

	if (m_TestDCache->GetFIDList(lPageNo, cdwaFIDList))
	{
		for (int i=0; i<cdwaFIDList.GetSize();i++)
		{
			if ((iFidNo==cdwaFIDList.GetAt(i)) || (iFidNo==-1))
			{
				bLog = TRUE;
				switch (fDB.GetFidType(cdwaFIDList.GetAt(i)))
				{
					case NGN_QUOTELIST:
						{
							long lTemp = -1;
//							if (m_TestDCache->GetAField(lPageNo, copMsg, (FID_DEFINITION)cdwaFIDList.GetAt(i), (long&) lTemp))
//								IteratedQuoteList((unsigned char *)copMsg->GetMessage());
							QuoteList qlist;
						//	qlist.SetTL(0);
							if (m_TestDCache->GetAField(lPageNo, qlist, (FID_DEFINITION)cdwaFIDList.GetAt(i),-1))
								IteratedQuoteList(lPageNo,(FID_DEFINITION)cdwaFIDList.GetAt(i), qlist);
						}
						break;
					case NGN_YHEADLINE:
					case NGN_HEADLINE:
						{
							CDWordArray cdwaHeadlineList;
							if (m_TestDCache->GetHeadlineList(lPageNo, cdwaHeadlineList, (FID_DEFINITION)cdwaFIDList.GetAt(i)))
							{
								for (int iLine=0; iLine< cdwaHeadlineList.GetSize(); iLine++)
								{
									long lTemp = cdwaHeadlineList.GetAt(iLine);
									if (m_TestDCache->GetAField(lPageNo, copMsg, (FID_DEFINITION)cdwaFIDList.GetAt(i), lTemp))
										IteratedLog((unsigned char *)copMsg->GetMessage());
								}
							}
						}
						break;
					default:
						{
							long lTemp = -1;
							if (m_TestDCache->GetAField(lPageNo, copMsg, (FID_DEFINITION)cdwaFIDList.GetAt(i), (long&) lTemp))
								IteratedLog((unsigned char *)copMsg->GetMessage());
						}
						break;
				}
			}
		}
	}
	return bLog;
}


//---------------------------------------------------------------------------------
void CQuoteListToolsDlg::OnSearch() 
{
	UpdateData(TRUE);
	CString strOutput;
	int iFidNo = (m_strFid!="")?atoi(m_strFid):-1;
	long lPageNo = 0;
	if (m_sPageNo.GetLength() > 1)
	{
		if ((m_sPageNo.Left(2) == "0x")||(m_sPageNo.Left(2) == "0X"))
			sscanf(m_sPageNo, "%x", &lPageNo);
		else
			sscanf(m_sPageNo, "%d", &lPageNo);
	}
	else
		lPageNo = atoi(m_sPageNo);

	if (m_sPageNo.GetLength()>0)
	{
		if (!LogOnePageData(lPageNo, iFidNo))
		{
			if (iFidNo==-1)
				strOutput.Format("No data in Page - %s", m_sPageNo);
			else
				strOutput.Format("No Fid %s data in Page - %s", m_strFid, m_sPageNo);
		}
	}
	else
	{
		if (m_strFid!="")
		{
			BOOL bLog = FALSE;	
			CDWordArray	cdwaPageList;
			if (m_TestDCache->GetPageList(cdwaPageList))
			{
				do 
				{
					if (cdwaPageList.GetSize()>0)
					{
						long lCurPageNo = cdwaPageList.GetAt(0);
						cdwaPageList.RemoveAt(0);
						if (LogOnePageData(lCurPageNo, iFidNo) && !bLog)
							bLog = TRUE;
					}
				} while (cdwaPageList.GetSize()>0);
				if (!bLog)
					strOutput.Format("No Fid %s data in DCache", m_strFid);
			}
			else
			{
				strOutput.Format("No data in DCache!");
			}
		}
		else
			AfxMessageBox("Please enter Page No / Fid No");
	}
	Log(strOutput);
	Log("  ");
	m_PageCtrl.SetSel( MAKELONG(0,-1) ) ;
	m_PageCtrl.SetFocus();
}
//---------------------------------------------------------------------------------

void CQuoteListToolsDlg::IteratedLog(byte* bytData) 
{
	CDTFidIterator PI(bytData);
	PI.SetMyDialog(this);
	PI.SetInit(m_bShowQuoteList);
	PI.First();
	while (!PI.IsEnd())
	{
		PI.ProcessFid();
		PI.Next();
	}
}
//---------------------------------------------------------------------------------

void CQuoteListToolsDlg::IteratedQuoteList(byte* bytData) 
{
	CQTFidIterator PI(bytData);
	PI.SetMyDialog(this);
	PI.SetInit(m_bShowQuoteList);
	PI.First();
	while (!PI.IsEnd())
	{
		PI.ProcessFid();
		PI.Next();
	}
}

void CQuoteListToolsDlg::IteratedQuoteList(long lPage, short sFid,QuoteList& qlist) {
	CQTFidIterator PI(NULL);
	PI.SetMyDialog(this);
	PI.SetQuoteListPageFid(lPage,sFid);
	PI.SetInit(m_bShowQuoteList);
	QuoteList qlFix;
	qlFix.Init(qlist.GetData());
	PI.PrintQuoteList(qlFix);
}

//---------------------------------------------------------------------------------
void CQuoteListToolsDlg::IteratedUpdateQuoteList(long lPageNo, FID_DEFINITION enumFieldId, long lIndex, COP_ITEM copMsg, CObject* pCallbackObject)
{
	CQuoteListToolsDlg* pMyself = (CQuoteListToolsDlg*)pCallbackObject;    
	CQTFidIterator PI((unsigned char *)copMsg->GetMessage());
	PI.SetMyDialog(pMyself);
	PI.SetInit(pMyself->m_bShowQuoteList, TRUE);
	PI.First();
	while (!PI.IsEnd())
	{
		if (PI.GetCurFidCode()==enumFieldId)
			PI.ProcessFid();
		PI.Next();
	}
/*	long lIndex1 = lIndex;
	long lIndex2 = lIndex;
	long lIndex3 = lIndex;
	COP_ITEM copMsg1, copMsg2, copMsg3;
	pMyself->m_TestDCache->GetAField(lPageNo, copMsg1, enumFieldId, lIndex1);
	pMyself->m_TestDCache->GetAField(lPageNo, copMsg2, enumFieldId, lIndex2);
	pMyself->m_TestDCache->GetAField(lPageNo, copMsg3, enumFieldId, lIndex3);
*/

//	pMyself->IteratedLog((unsigned char *)copMsg->GetMessage());
//	pMyself->Log("Receive Callback");
}


//---------------------------------------------------------------------------------
void CQuoteListToolsDlg::InitQuoteListCol(int iRow, int iCol)
{
	m_QuoteList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_SHOWSELALWAYS );
	m_QuoteList.SetRows(0);
	m_QuoteList.SetCols(0);

	DWORD dwStyle = m_QuoteList.GetExtendedStyle();
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;
	lvc.fmt = LVCFMT_LEFT;
	m_QuoteList.SetFixedRows(TRUE);

	m_QuoteList.DeleteAllItems();

	for (int nColumn = 0; nColumn < iCol+1; nColumn++) {
		lvc.iSubItem = nColumn;
		CString strColumn;
		strColumn.Format("Col %d",nColumn-1);
		lvc.pszText = strColumn.GetBuffer(0);
		lvc.cx = ((nColumn == 0)? 50 : 160);
		m_QuoteList.InsertColumn(nColumn, &lvc);		
	}

	for (int i = 0; i< iRow;i++)
	{
		LV_ITEM lvi;
		lvi.mask = LVIF_TEXT ;
		lvi.iItem = i;
		lvi.iSubItem = 0;
		CString strRow;
		strRow.Format("Row-%d",i);
		lvi.pszText = strRow.GetBuffer(0);
		m_QuoteList.InsertItem(&lvi);
	}
	dwStyle |= LVS_EX_FULLROWSELECT + LVS_EX_HEADERDRAGDROP + LVS_EX_TRACKSELECT;
	m_QuoteList.SetItemState(0,2, LVIS_SELECTED);
	m_QuoteList.SetCodePage(950);
	m_QuoteList.SetFont(&m_font);
	m_QuoteList.RedrawWindow();
}

//---------------------------------------------------------------------------------
void CQuoteListToolsDlg::InsertQuoteCell(int iRow, int iCol, CString strData, int iCodePage)
{
	m_QuoteList.SetCodePage(iCodePage);
	m_QuoteList.SetRedraw();
	m_QuoteList.SetItemTextEx(iRow, iCol+1, strData);
	m_QuoteList.SetRedraw();
}
//---------------------------------------------------------------------------------
void CQuoteListToolsDlg::OnLogFilter() 
{
	if (!m_dlgLogFilter->IsWindowVisible())
		m_dlgLogFilter->ShowWindow(SW_RESTORE);
	else
		m_dlgLogFilter->SetActiveWindow();
}	
//---------------------------------------------------------------------------------
void CQuoteListToolsDlg::OnDropPage() 
{
	UpdateData(TRUE);
	long lPageNo = 0;
	if (m_sPageNo.GetLength() > 1)
	{
		if (m_sPageNo.Left(2) == "0x")
			sscanf(m_sPageNo, "%x", &lPageNo);
		else
			sscanf(m_sPageNo, "%d", &lPageNo);
	}
	else
		lPageNo = atoi(m_sPageNo);
	CString strTemp;
	strTemp.Format("Are you sure you want to drop Page %s",m_sPageNo);

	if (::AfxMessageBox(strTemp, MB_YESNO |MB_DEFBUTTON2) == IDYES)
	{

		COP_ITEM copMsg;
		if (m_TestDCache->DropAPage(lPageNo, copMsg))
		{
			CString strOutput;
			strOutput.Format("Drop the Page - %s by U!", m_sPageNo);
			Log(strOutput);
		}
		else
		{
			CString strOutput;
			strOutput.Format("Can't drop the page coz No data in Page - %s", m_sPageNo);
			Log(strOutput);
		}
		Log("  ");
	}

}

//---------------------------------------------------------------------------------
void CQuoteListToolsDlg::OnListAllPages() 
{
	CDWordArray cdwaPageList;
	CString		strOutput;
	if (m_TestDCache->GetPageList(cdwaPageList))
	{
		Log(strOutput);
		int iCount = 0;
		while (cdwaPageList.GetSize()>0)
		{
			CString strTemp;
			long lPageNo = cdwaPageList.GetAt(0);
			strTemp.Format("%d (0x%x),   ", lPageNo, lPageNo);
			strOutput +=strTemp;
			cdwaPageList.RemoveAt(0);
			if (iCount == 4)
			{
				iCount = 0;
				Log(strOutput);
				strOutput = "";
			}
			else
				iCount++;

		}
		if (iCount!=0)
			Log(strOutput);
		Log("Pages in Dcache are ");
	}
	else
		Log("No Pages in DCache");
	Log("  ");	
}
//---------------------------------------------------------------------------------
void CQuoteListToolsDlg::OnListAllDropPages() 
{
	CDWordArray cdwaPageList;
	CString		strOutput;
	if (m_TestDCache->GetDropPageList(cdwaPageList))
	{
		Log(strOutput);
		int iCount = 0;
		while (cdwaPageList.GetSize()>0)
		{
			CString strTemp;
			long lPageNo = cdwaPageList.GetAt(0);
			strTemp.Format("%d (0x%x),   ", lPageNo, lPageNo);
			strOutput +=strTemp;
			cdwaPageList.RemoveAt(0);
			if (iCount == 4)
			{
				iCount = 0;
				Log(strOutput);
				strOutput = "";
			}
			else
				iCount++;

		}
		if (iCount!=0)
			Log(strOutput);
		Log("Drop Pages in Dcache are ");
	}
	else
		Log("No Pages in DCache");
	Log("  ");	
}
//---------------------------------------------------------------------------------
void CQuoteListToolsDlg::OnListAllFids() 
{
	CDWordArray cdwaPageList;
	CString		strOutput;

	UpdateData(TRUE);
	long lPageNo = 0;
	if (m_sPageNo.GetLength() > 1)
	{
		if (m_sPageNo.Left(2) == "0x")
			sscanf(m_sPageNo, "%x", &lPageNo);
		else
			sscanf(m_sPageNo, "%d", &lPageNo);

	}
	else
		lPageNo = atoi(m_sPageNo);

	if (m_TestDCache->GetFIDList(lPageNo, cdwaPageList))
	{
		Log(strOutput);
		int iCount = 0;
		while (cdwaPageList.GetSize()>0)
		{
			CString strTemp;
			long lFidNo = cdwaPageList.GetAt(0);
			strTemp.Format("%d,   ", lFidNo);
			strOutput +=strTemp;
			cdwaPageList.RemoveAt(0);
			if (iCount == 4)
			{
				iCount = 0;
				Log(strOutput);
				strOutput = "";
			}
			else
				iCount++;

		}
		if (iCount!=0)
			Log(strOutput);
		strOutput.Format("Fids in Page %s are", m_sPageNo);
		Log(strOutput);
	}
	else
		Log("No Pages in DCache");
	Log("  ");	
}
//---------------------------------------------------------------------------------
void CQuoteListToolsDlg::OnSaveData() 
{
	static char BASED_CODE szFilter[] = _T("dat Files (*.dat)|*.dat|All Files (*.*)|*.*||");
	CFileDialog dlg( FALSE, _T("Data"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	dlg.DoModal();
	CString strFileName = dlg.GetPathName();
	if (strFileName != "")
		m_TestDCache->LogDataToFile(strFileName);
}
//---------------------------------------------------------------------------------

void CQuoteListToolsDlg::OnRetrieveData() 
{
	static char BASED_CODE szFilter[] = _T("dat Files (*.dat)|*.dat|All Files (*.*)|*.*||");
	CFileDialog dlg( TRUE, _T("Data"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	dlg.DoModal();
	CString strFileName = dlg.GetPathName();
	BOOL bReadSuccessFlag = FALSE;
	if (strFileName != "")
	{
		FILE *data_fp = NULL;

		if ((data_fp=fopen(strFileName.GetBuffer(0), "rb")) != NULL)
		{
			char strLastMessage[] = "DCACHE_COMPLETED";
			char strTestMessage[30] = "";
			fseek( data_fp , -16, SEEK_END);
			fread( &strTestMessage, 1, 16, data_fp);
			strTestMessage[17] = 0x00;
			if (strcmp(strTestMessage, strLastMessage)==0)
				bReadSuccessFlag = m_TestDCache->ReadData(data_fp);
			fclose(data_fp);
		}
	}
	if (!bReadSuccessFlag)
		AfxMessageBox("Retrieved Data unsucessfully!!");
	else
		AfxMessageBox("Retrieved Data sucessfully!!");
}

//---------------------------------------------------------------------------------
void CQuoteListToolsDlg::OnDblclkStatusList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CString strTempTxt;
	CString strTempNo;
	strTempTxt = m_Statuslist.GetItemText(m_Statuslist.GetSelectionMark(), 0);
	CSelTextDlg dlgSelText;
	dlgSelText.m_strOrgText = strTempTxt;
	dlgSelText.DoModal();
	*pResult = 0;
}
//---------------------------------------------------------------------------------
void CQuoteListToolsDlg::OnDblclkDcacheList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CString strTempTxt;
	CString strTempNo;
	strTempTxt = m_DCachelist.GetItemText(m_DCachelist.GetSelectionMark(), 0);
	CSelTextDlg dlgSelText;
	dlgSelText.m_strOrgText = strTempTxt;
	dlgSelText.DoModal();
	*pResult = 0;
}
//---------------------------------------------------------------------------------
void CQuoteListToolsDlg::OnListAllDropFid() 
{
	UpdateData(TRUE);
	long lPageNo = 0;
	if (m_sPageNo.GetLength() > 1)
	{
		if (m_sPageNo.Left(2) == "0x")
			sscanf(m_sPageNo, "%x", &lPageNo);
		else
			sscanf(m_sPageNo, "%d", &lPageNo);

	}
	else
		lPageNo = atoi(m_sPageNo);
	
	CString strOutput;
	CDWordArray cdwaDropFidList;
	if (m_TestDCache->GetDropFidList(lPageNo, cdwaDropFidList))
	{
		Log(strOutput);
		int iCount = 0;
		while (cdwaDropFidList.GetSize()>0)
		{
			CString strTemp;
			long lFidNo = cdwaDropFidList.GetAt(0);
			strTemp.Format("%d,   ", lFidNo);
			strOutput +=strTemp;
			cdwaDropFidList.RemoveAt(0);
			if (iCount == 4)
			{
				iCount = 0;
				Log(strOutput);
				strOutput = "";
			}
			else
				iCount++;

		}
		if (iCount!=0)
			Log(strOutput);
		strOutput.Format("Drop Fids in Page %s are", m_sPageNo);
		Log(strOutput);
	}
	else
		Log("No Pages in DCache");
	Log("  ");	


}
//---------------------------------------------------------------------------------
void CQuoteListToolsDlg::OnTestDrop() 
{
	UpdateData(TRUE);
	long lPageNo = 0;
	if (m_sPageNo.GetLength() > 1)
	{
		if (m_sPageNo.Left(2) == "0x")
			sscanf(m_sPageNo, "%x", &lPageNo);
		else
			sscanf(m_sPageNo, "%d", &lPageNo);
	}
	else
		lPageNo = atoi(m_sPageNo);

	CString strOutput;
	COP_ITEM pMsgObj;
	MsgObject* MsgObj = new MsgObject();
	MsgObj->SetMsgType(MGT_DROP);	
	MsgObj->SetItemNo(lPageNo);
	if (m_strFid!="")
	{
		FieldList fldlst;
		fldlst.AddField(atoi(m_strFid));
		MsgObj->AddFid(FID_REV_LST5, &fldlst);
		strOutput.Format("Sent Drop Page - %s, Fid - %s to DCache", m_sPageNo, m_strFid);
	}
	else
		strOutput.Format("Sent Drop Page - %s to DCache", m_sPageNo);
	pMsgObj = new MsgObject(MsgObj->GetMessage());
	delete MsgObj;
	m_TestDCache->PutDataIntoDCache(pMsgObj);

	Log(strOutput);	
	Log("  ");	
}
//---------------------------------------------------------------------------------

void CQuoteListToolsDlg::OnTest() 
{
	COP_ITEM pMsgObj;
	CreateMsgObjcet(pMsgObj);
	m_TestDCache->PutDataIntoDCache(pMsgObj);
}

//---------------------------------------------------------------------------------

void CQuoteListToolsDlg::CreateMsgObjcet(COP_ITEM& m_pMsgObj )
{

	// construct message
	// Add fid, Get Fid, Iterator parsing testing

	MsgObject* MsgObj = new MsgObject();
	// HisVal - 38

	unsigned char ch = 12;
	WCHAR uni;
	uni = (WCHAR) "中";
	short sh = -3650;
	unsigned short ush = 65530;
	long l = -1000002;
	unsigned long ul = 1002253;
	float f = 1.192092896e-02F;
	double d = -2.365441236;
	long lKMValue	= 123456789;
	long l10KMValue = 234567891;

	// basic types 1-10
	
	MsgObj->AddFid(35, &ch);		// 	char	32
	MsgObj->AddFid(101, (unsigned short*)&uni);		// 	unicode "中文輸入法"
	MsgObj->AddFid(121, &sh);		// 	Short	-3650
	MsgObj->AddFid(155, &ush);		// 	uShort	65530
	MsgObj->AddFid(201, &l);		// 	long	-1000002
	MsgObj->AddFid(251, &ul);		// 	ulong	1002253
	MsgObj->AddFid(281, &f); 		// 	float	1.192....
	MsgObj->AddFid(313, &f); 		// 	Ufloat	1.192....
	MsgObj->AddFid(390, &d);		// 	double	-2.365441...
	MsgObj->AddFid(380, &d);		// 	Udouble	2.365441...
	

	// CharArray 11
	CharArray CA;
	char cde[] = "Try Another CA2";
	byte bytPos=255;
	CA.SetSq(12);
	CA.SetPos(bytPos);
	int abc = strlen(cde);
	CA.SetLen(abc);
	CA.SetString((byte*)cde);
	// MCharArray 12
	CharArray ca1, ca2, ca3;
	char* c1 = "Testing for Multiple Char Array - Line 1";
	char* c2 = "Testing for Multiple Char Array - Line 2";
	char* c3 = "Testing for Multiple Char Array - Line 3";

	ca1.SetSq(1);
	ca1.SetPos(255);
	ca1.SetLen(strlen(c1));
	ca1.SetString((byte*)c1);

	ca2.SetSq(2);
	ca2.SetPos(255);
	ca2.SetLen(strlen(c2));
	ca2.SetString((byte*)c2);
	
	ca3.SetSq(3);
	ca3.SetPos(255);
	ca3.SetLen(strlen(c3));
	ca3.SetString((byte*)c3);

	MCharArray mca1;
	mca1.SetTL(10);
	mca1.AddLine(1, &ca1);
	mca1.AddLine(3, &ca2); 
	mca1.AddLine(9, &ca3);
	// Time - BDateYear13, BDataMonth14, BTimeMin15, BTimeSec16, BTimeRanges17
	BDateYear dy(2000, 12, 20);
	BDateMonth m(1, 2);
	BTimeMin tm(17, 40);
	BTimeSec s(17, 40, 30);
	BTimeMin s_tm1(9, 30);
	BTimeMin e_tm1(10, 30);
	BTimeMin s_tm2(11, 30);
	BTimeMin e_tm2(12, 30);
	BTimeMin s_tm3(13, 30);
	BTimeMin e_tm3(14, 30);
	BTimeMin s_tm4(15, 30);
	BTimeMin e_tm4(16, 30);
	BTimeMin s_tm5(17, 30);
	BTimeMin e_tm5(18, 30);
	BTimeMin s_tm6(19, 30);
	BTimeMin e_tm6(20, 30);
	BTimeRanges tr;
	tr.AddRange(s_tm1, e_tm1);
	tr.AddRange(s_tm2, e_tm2);
	tr.AddRange(s_tm3, e_tm3);
	tr.AddRange(s_tm4, e_tm4);
	tr.AddRange(s_tm5, e_tm5);
	tr.AddRange(s_tm6, e_tm6);
	// Tran - 18
	Tran tran;
	BTimeMin T(10, 20);
	tran.AddTranElement(101, T, 80, 13, (byte)'b');
	tran.AddTranElement(102, T, 60, 14, (byte)'a');
	tran.AddTranElement(103, T, 90, 15, (byte)'Y');
	// Txn - 19
	Txn txn;
	txn.SetTL(10);
	txn.SetSq(1);
	T.SetHH(22);
	txn.AddTranElement(T, 544, 1000, (byte)'a');
	txn.AddTranElement(T, 135, 2500, (byte)'b');
	txn.AddTranElement(T, 854, 1025, (byte)'*');
	// ShrOrdLst - 20
	ShrOrdLst sol;
	sol.SetSh(7890000);
	sol.SetOr(16383);
	// PrcVol - 21
	byte prcTL=10, prcPos=1; 
	PrcVol prc;
	prc.SetTL(prcTL); 
	prc.SetPos(prcPos);
	prc.AddPrcVol(30254, 2039.1);
	prc.AddPrcVol(25156, 1569.1);
	prc.AddPrcVol(15869, 2566.1);
	// BkrQue - 22
	BkrQue bkrque;
	bkrque.AddBroker(0, 9900, 0);
	bkrque.AddBroker(0, 9999, 0);
	bkrque.AddBroker(0, 1211, 0);
	bkrque.AddBroker(0, 11, 0);
	bkrque.AddBroker(0, 0, 0);
	bkrque.AddBroker(1, 0, 0);
	bkrque.AddBroker(2, 1234, 100);
	bkrque.AddBroker(3, 5678, 1257);
	// Link - 23
	Link link;
	link.SetTL(10);
	link.SetPos(1);
	link.AddLink(9999);
	link.AddLink(10251);
	link.AddLink(9582);
	// WLink - 24
	WLink wlink;
	wlink.SetTL(19);
	wlink.SetPos(2);
	wlink.AddWLink(2940, 382, (float)201.34, 3924);
	wlink.AddWLink(2941, 383, (float)202.34, 3925);
	wlink.AddWLink(2942, 384, (float)203.34, 3926);
	// YLabel - 25
	YLabel ylabel;
	ylabel.SetHigh((float)20123.2);
	ylabel.SetMid((float)2582.24);
	ylabel.SetLow((float)25369.2);
	// XMtcLabel - 26
	XMtcLabel xmtcLbl;
	xmtcLbl.AddTime(1, BTimeMin(10, 12));
	xmtcLbl.AddTime(2, BTimeMin(11, 12));
	xmtcLbl.AddTime(3, BTimeMin(12, 12));
	xmtcLbl.AddTime(4, BTimeMin(13, 12));
	xmtcLbl.AddTime(5, BTimeMin(14, 12));
	// XDtcLabel - 27
	XDtcLabel xdtcLbl;
	xdtcLbl.AddDate(1, BDateMonth(11, 1));
	xdtcLbl.AddDate(2, BDateMonth(11, 2));
	xdtcLbl.AddDate(3, BDateMonth(11, 3));
	xdtcLbl.AddDate(4, BDateMonth(11, 4));
	xdtcLbl.AddDate(5, BDateMonth(11, 5));
	// MTCPTS - 28
	MtcPoints mtcPts1;
	mtcPts1.SetTL(10);
	mtcPts1.SetPos(1);
	mtcPts1.AddPoint(10);
	mtcPts1.AddPoint(11);
	mtcPts1.AddPoint(12);
	mtcPts1.AddPoint(13);
	mtcPts1.AddPoint(14);
	MtcPoints mtcPts(mtcPts1.GetData());
	// MtcVal - 29
	MtcVal mtcval;
	mtcval.SetTL(12);
	mtcval.SetPos(1);
	mtcval.AddVal((float)293.2);
	mtcval.AddVal((float)295.2);
	mtcval.AddVal((float)215.2);
	mtcval.AddVal((float)218.2);
	mtcval.AddVal((float)198.2);
	// DtcPoints - 30
	DtcPoints dtcPts;
	dtcPts.AddDtcElement(10, 15, 28);
	dtcPts.AddDtcElement(91, 12, 36);
	dtcPts.AddDtcElement(28, 64, 21);
	dtcPts.AddDtcElement(19, 78, 36);
	dtcPts.AddDtcElement(37, 56, 59);
	// HisVal - 31
	HisVal hisval;
	hisval.SetTL(12);
	hisval.SetDt(BDateYear(2000, 11, 18));
	hisval.AddVal((float)20.23, (float)20.53, (float)20.53, 40000);
	hisval.AddVal((float)11.95, (float)11.28, (float)11.0, 283654);
	hisval.AddVal((float)21.65, (float)19.46, (float)20.15, 96555);
	hisval.AddVal((float)90.12, (float)60.10, (float)60.25, 156798);
	hisval.AddVal((float)86.89, (float)92.46, (float)98.5, 655621);
	// FieldList - 32
	FieldList fldlst;
	fldlst.AddField(19);
	fldlst.AddField(35);
	fldlst.AddField(23);
	fldlst.AddField(569);
	fldlst.AddField(366);
	// FieldLabel - 33
	FieldLabel fldlbl;
	fldlbl.AddLabel("High");
	fldlbl.AddLabel("Low");
	fldlbl.AddLabel("Weight");
	fldlbl.AddLabel("Long");
	fldlbl.AddLabel("Chan Tai Man");
	// InfoMsg - 34
 	InfoMsg infomsg;
	infomsg.SetTL(12);
	infomsg.AddInfo("", 3999, 2912);
	infomsg.AddInfo("Testing100", 3999, 2912);
	infomsg.AddInfo("Testing101", 4568, 2569);
	infomsg.AddInfo("Testing102", 2253, 5533);
	infomsg.AddInfo("Testing103", 8822, 3322);
	infomsg.AddInfo("Testing104", 6954, 2563);
	InfoMsg infomsg1(infomsg.GetData());
	// HeadLine - 35
	short nLn = 10;
	BDateMonth dt(11, 3);
	BTimeSec tm1(16, 46, 30);
	unsigned long pulPg[10] = {23653,26898,95466,6689,98666,455456,10000001,25563,55999,775556699};
	HeadLine headline(nLn, dt, tm1, 1, "ZHE", 3495, "THIS IS PNAC", "SHK/HK TW SHK", 10, pulPg, "This is a testing for news headlines.");
	// HLink - 36
 	HLink hlink1;
	hlink1.SetTL(254);
	hlink1.AddHLink(7899, 10);
	hlink1.AddHLink(7654, 11);
	hlink1.AddHLink(9999, 12);
	hlink1.AddHLink(2235, 13);
	hlink1.AddHLink(3668, 14);
	HLink hlink(hlink1.GetData());
	// MShrOrdLst - 37
	MShrOrdLst msol;
	ShrOrdLst sol1;
	sol1.SetSh(3100000);
	sol1.SetOr(6789);
	msol.AddShrOrdLst(&sol1);
	sol1.SetSh(123);
	sol1.SetOr(4569);
	msol.AddShrOrdLst(&sol1);
	sol1.SetSh(3256630);
	sol1.SetOr(2102);
	msol.AddShrOrdLst(&sol1);
	// SprTbl - 38
	SprTbl sprtbl;
	unsigned short usTempPos = 0;
	unsigned short usTempTL  = 12;
	sprtbl.SetTL(usTempTL);
	sprtbl.SetPos(usTempPos);
	sprtbl.AddSprTbl((float)122.23, (float)20.53, (float)20.53);
	sprtbl.AddSprTbl((float)122.95, (float)11.28, (float)11.0);
	sprtbl.AddSprTbl((float)21.65, (float)122.46, (float)20.15);
	sprtbl.AddSprTbl((float)90.12, (float)122.10, (float)60.25);
	sprtbl.AddSprTbl((float)86.89, (float)92.46, (float)122.5);
	// QuoteList - 39
	QuoteList quoteList;
	float fNominal = 83.40f;
	double dTurnover = 2000000000;
	quoteList.AddQuote (0, 304, sizeof(fNominal), &fNominal);
	quoteList.AddQuote (0, 373, sizeof(dTurnover), &dTurnover);
	quoteList.SetTL (5);
	// MTCTicks - 40
	MTCTicks mtcTicks;
	mtcTicks.SetTL(240);
	mtcTicks.SetPos (0);
	int nTotalTick = 10;
	for (int q=0; q<nTotalTick; q++)
	{
		if (q <= 8)
			mtcTicks.AddPoint (q+1, q+1, q, q);
		else
			mtcTicks.AddPoint (397-q, 397-q, 397-q-1, 397-q);
	}

	// MTCV - 41
	MTCV mtcv;
	mtcv.SetTL(240);
	mtcv.SetPos (0);
	int nTT = 10;
	for (int p=0; p<nTT; p++)
	{
		if (p <= 8)
			mtcv.AddPoint (p+1, p, p, p, p+1);
		else
			mtcv.AddPoint (397-p,397-p, 397-p-1, 397-p, 397-p-1);
	}

	BDateYear yr (2001, 11, 5);
	BTimeSec tmHL (13, 31, 02);
	unsigned long ulPg[] = { 1, 5, 13, 941 };
	// YHeadLine - 42
	YHeadLine yheadline (210, yr, tmHL, 1, "EN", 0xFE203839, "p837FS", "RTRS HK", 4, ulPg, "This is testing headline");

	// OBkrQue - 43
	OBkrQue obkrque;
	obkrque.AddBroker(0, 9900, 0, 1);
	obkrque.AddBroker(0, 9999, 0, 2);
	obkrque.AddBroker(0, 1211, 0, 3);
	obkrque.AddBroker(0, 11, 0, 4);
	obkrque.AddBroker(0, 0, 0, 5);
	obkrque.AddBroker(1, 0, 0, 6);
	obkrque.AddBroker(2, 1234, 100, 7);
	obkrque.AddBroker(3, 5678, 1257, 8);

	// BQueVOSN - 46
	BQueVOSN bquevosn;
	bquevosn.AddBroker(0, 9900, 0, 1);
	bquevosn.AddBroker(0, 9999, 0, 2);
	bquevosn.AddBroker(0, 1211, 0, 3);
	bquevosn.AddBroker(0, 11, 0, 4);
	bquevosn.AddBroker(0, 0, 0, 5);
	bquevosn.AddBroker(1, 0, 0, 6);
	bquevosn.AddBroker(2, 1234, 100, 7);
	bquevosn.AddBroker(3, 5678, 1257, 8);

	// MTCPTS - 47
	MTCPTS mtcPts2;
	mtcPts2.SetTL(270);
	mtcPts2.SetPos(260);
	mtcPts2.AddPoint(10);
	mtcPts2.AddPoint(11);
	mtcPts2.AddPoint(12);
	mtcPts2.AddPoint(13);
	mtcPts2.AddPoint(14);
	MTCPTS mtcPts3(mtcPts2.GetData());

	// MTCPTS - 48
	MTCF mtcf;
	mtcf.SetTL(270);
	mtcf.SetPos(0);
	for (int iPoint = 0; iPoint <270; iPoint++)
	mtcf.AddPoint(1.0,2.0,3.0,4.0,1000000.0);
/*	mtcf.AddPoint(2.0,3.0,4.0,5.0,2000000.0);
	mtcf.AddPoint(3.0,4.0,5.0,6.0,3000000.0);
	mtcf.AddPoint(4.0,5.0,6.0,7.0,4000000.0);
	mtcf.AddPoint(5.0,6.0,7.0,8.0,5000000.0);
*/	MTCF mtcf3(mtcf.GetData());

	// PrcVolwSeq - 49
	long lSeq = 1234;
	PrcVolwSeq prc1;
	prc1.SetTL(prcTL); 
	prc1.SetPos(prcPos);
	prc1.SetSq(lSeq);
	prc1.AddPrcVol(30254, 2039.1);
	prc1.AddPrcVol(25156, 1569.1);
	prc1.AddPrcVol(15869, 2566.1);

	
	// Add Fid
	MsgObj->AddFid(401, &CA);						// CharArray 11
	MsgObj->AddFid(451, &mca1);						// MCharArray 12
	MsgObj->AddFid(1109, &dy);						// Time - BDateYear13
									// ---
	MsgObj->AddFid(1211, &tm);						// Time - BTimeMin15
	MsgObj->AddFid(1201, &s);						// Time - BTimeSec16
	MsgObj->AddFid(1405, &tr);						// Time - BTimeRanges17	
	MsgObj->AddFid(1301, &tran);					// Tran - 18
	MsgObj->AddFid(1304, &txn);		// ---			// Txn - 19
	MsgObj->AddFid(1371, &sol);						// ShrOrdLst - 20
	MsgObj->AddFid(1303, &prc);						// PrcVol - 21
	MsgObj->AddFid(1361, &bkrque);					// BkrQue - 22
	MsgObj->AddFid(1384, &link);					// Link - 23
	MsgObj->AddFid(1401, &wlink);					// WLink - 24
	MsgObj->AddFid(1403, &ylabel);					// YLabel - 25
	MsgObj->AddFid(1408, &xmtcLbl);					// XMtcLabel - 26
	MsgObj->AddFid(1409, &xdtcLbl);					// XDtcLabel - 27
	MsgObj->AddFid(1404, &mtcPts);					// MTCPTS - 28
	MsgObj->AddFid(1305, &mtcval);					// MtcVal - 29
	MsgObj->AddFid(1407, &dtcPts);					// DtcPoints - 30
	MsgObj->AddFid(1410, &hisval);	// ---			// HisVal - 31
	MsgObj->AddFid(1355, &fldlst);					// FieldList - 32
	MsgObj->AddFid(1351, &fldlbl);					// FieldLabel - 33
	MsgObj->AddFid(1412, &infomsg1);				// InfoMsg - 34
	MsgObj->AddFid(1392, &headline);				// HeadLine - 35
	MsgObj->AddFid(1391, &hlink1);					// HLink - 36
									// ---	
	MsgObj->AddFid(1450, &sprtbl);					// SprTbl - 38
	MsgObj->AddFid(1460, &quoteList);				// QuoteList - 39
	MsgObj->AddFid(1500, &mtcTicks);				// MTCTicks - 40
	MsgObj->AddFid(1520, &mtcv);					// MTCV - 41
	MsgObj->AddFid(1393, &yheadline);				// YHeadLine - 42
	MsgObj->AddFid(1541, &obkrque);					// OBkrQue - 43
	MsgObj->AddFid(1545, &bquevosn);				// BQueVOSN - 46
	MsgObj->AddFid(1600, &lKMValue );				// KMultiple - 44
	MsgObj->AddFid(1700, &l10KMValue );				// 10KMultiple - 45
	MsgObj->AddFid(1800, &mtcPts3);					// MTCPTS - 47
	MsgObj->AddFid(1525, &mtcf);					// MTCF - 48
	MsgObj->AddFid(1830, &prc1);					// PrcVolwSeq 49

	MsgObj->SetDPID(71);
	MsgObj->SetDPInsID(2);
	MsgObj->SetSeqNo(102);
	MsgObj->SetMsgType(16);
	MsgObj->SetItemClass(127);
	MsgObj->SetDelay(TRUE);
	MsgObj->SetItemNo(199999); 
	m_pMsgObj = new MsgObject(MsgObj->GetMessage()); 
	delete MsgObj;
} 

//---------------------------------------------------------------------------------
void CQuoteListToolsDlg::Send(long lPageNo, FID_DEFINITION enumFieldId, long lIndex, COP_ITEM copMsg, CObject* pCallbackObject)
{
	CQuoteListToolsDlg* pMyself = (CQuoteListToolsDlg*)pCallbackObject;    
/*	long lIndex1 = lIndex;
	long lIndex2 = lIndex;
	long lIndex3 = lIndex;
	COP_ITEM copMsg1, copMsg2, copMsg3;
	pMyself->m_TestDCache->GetAField(lPageNo, copMsg1, enumFieldId, lIndex1);
	pMyself->m_TestDCache->GetAField(lPageNo, copMsg2, enumFieldId, lIndex2);
	pMyself->m_TestDCache->GetAField(lPageNo, copMsg3, enumFieldId, lIndex3);
*/

//	pMyself->IteratedLog((unsigned char *)copMsg->GetMessage());
//	pMyself->Log("Receive Callback");
}
//---------------------------------------------------------------------------------

void CQuoteListToolsDlg::OnShowStat() 
{
	CStatisticDlg dlg;
	dlg.SetMyDialog(this);
	dlg.DoModal();
}
//---------------------------------------------------------------------------------

void CQuoteListToolsDlg::OnTimecheck() 
{
	UpdateData();
}
//---------------------------------------------------------------------------------
int CQuoteListToolsDlg::IsInArray(CArray<int ,int>* CountArray, int iPage)
{
	int iReturnValue = -1;
	int iIndex = -1;
	int iArrayUpperBound = 0;
	if ((iArrayUpperBound=CountArray->GetUpperBound())>-1)
	{
		while (iArrayUpperBound>=++iIndex)
		{
			if (iPage==CountArray->GetAt(iIndex))
			{
				iReturnValue = iIndex;
				break;
			}
		}
	}
	return iReturnValue;
}

//---------------------------------------------------------------------------------
int CQuoteListToolsDlg::IsInArray(CObArray* CountArray, int iPage, int iFid)
{
	int iReturnValue = -1;
	int iIndex = -1;
	int iArrayUpperBound = 0;
	if ((iArrayUpperBound=CountArray->GetUpperBound())>-1)
	{
		while (iArrayUpperBound>=++iIndex)
		{
			CFilterStruct* curFilter = (CFilterStruct*) CountArray->GetAt(iIndex);
			if ((iFid==curFilter->FieldID)&&(iPage==curFilter->lPageNo))
			{
				iReturnValue = iIndex;
				break;
			}
		}
	}
	return iReturnValue;
}
//---------------------------------------------------------------------------------
BOOL CQuoteListToolsDlg::CompareArrayValue(int iPage, int iValue, int iEnum)
{
	BOOL bReturnValue = FALSE;
	CStatisticPage* pStatisticPage = NULL; 
	if (m_mapPages.Lookup(iPage, (CStatisticPage*)pStatisticPage ))
	{
		switch (iEnum)
		{
			case TOP_TEN_PAGE_TOTAL_COUNT:
				if (pStatisticPage->StatisticData.iTotalTimes<iValue)
					bReturnValue = TRUE;
				break;
			case TOP_TEN_PAGE_UPDATE_COUNT:
				if (pStatisticPage->StatisticData.iUpdateTimes<iValue)
					bReturnValue = TRUE;
				break;
			case TOP_TEN_PAGE_VS_COUNT:
				if (pStatisticPage->StatisticData.iVSTimes<iValue)
					bReturnValue = TRUE;
				break;
			case TOP_TEN_PAGE_UPDATE_SIZE:
				if (pStatisticPage->StatisticData.iUpdateSize<iValue)
					bReturnValue = TRUE;
				break;
			case TOP_TEN_PAGE_VS_SIZE:
				if (pStatisticPage->StatisticData.iVSSize<iValue)
					bReturnValue = TRUE;
				break;
			case TOP_TEN_PAGE_TOTAL_SIZE:
				if (pStatisticPage->StatisticData.iTotalSize<iValue)
					bReturnValue = TRUE;
				break;
			case TOP_TEN_PAGE_MAX_SIZE:
				if (pStatisticPage->StatisticData.iMaxSize<iValue)
					bReturnValue = TRUE;
				break;
			default:
				break;
		}
	}
	return bReturnValue;
}
//---------------------------------------------------------------------------------
BOOL CQuoteListToolsDlg::CompareArrayValue(CFilterStruct* tempFilter, int iValue, int iEnum)
{
	BOOL bReturnValue = FALSE;
	CStatisticPage* pStatisticPage = NULL; 
	if (m_mapPages.Lookup(tempFilter->lPageNo, (CStatisticPage*)pStatisticPage ))
	{
		CStatisticStruct* StatisticObject = NULL;
		if (pStatisticPage->m_mapFields.Lookup(tempFilter->FieldID, (CStatisticStruct*&)StatisticObject ))
		{
			switch (iEnum)
			{
				case TOP_TEN_FID_TOTAL_COUNT:
					if (StatisticObject->iTotalTimes<iValue)
						bReturnValue = TRUE;
					break;
				case TOP_TEN_FID_UPDATE_COUNT:
					if (StatisticObject->iUpdateTimes<iValue)
						bReturnValue = TRUE;
					break;
				case TOP_TEN_FID_VS_COUNT:
					if (StatisticObject->iVSTimes<iValue)
						bReturnValue = TRUE;
					break;
				case TOP_TEN_FID_UPDATE_SIZE:
					if (StatisticObject->iUpdateSize<iValue)
						bReturnValue = TRUE;
					break;
				case TOP_TEN_FID_VS_SIZE:
					if (StatisticObject->iVSSize<iValue)
						bReturnValue = TRUE;
					break;
				case TOP_TEN_FID_TOTAL_SIZE:
					if (StatisticObject->iTotalSize<iValue)
						bReturnValue = TRUE;
					break;
				case TOP_TEN_FID_MAX_SIZE:
					if (StatisticObject->iMaxSize<iValue)
						bReturnValue = TRUE;
					break;
				default:
					break;
			}
		}
	}
	return bReturnValue;
}
//---------------------------------------------------------------------------------
void CQuoteListToolsDlg::CompareArray(CArray<int ,int>* CountArray, int iPage, int iValue, int iEnum)
{
	int iIndex = -1;
	int iArrayUpperBound = 0;
	int iRemoveIndex = -1;
	BOOL bFound = FALSE;

	if ((iRemoveIndex=IsInArray(CountArray, iPage))!=-1)
		CountArray->RemoveAt(iRemoveIndex);

	if ((iArrayUpperBound=CountArray->GetUpperBound())>-1)
	{
		if ((CompareArrayValue(CountArray->GetAt(iArrayUpperBound), iValue, iEnum)))
		{
			while (iArrayUpperBound>=++iIndex)
			{
				if ((bFound=CompareArrayValue(CountArray->GetAt(iIndex), iValue, iEnum)))
				{
					CountArray->InsertAt(iIndex, iPage);
					break;
				}
			}
		}
		if ((iArrayUpperBound<NO_OF_ITEM_IN_RANK)&&(!bFound))
			CountArray->SetAtGrow(iArrayUpperBound+1, iPage);
	}
	else
	{
		CountArray->Add(iPage);
	}
	if (CountArray->GetUpperBound()>NO_OF_ITEM_IN_RANK)
		CountArray->RemoveAt(NO_OF_ITEM_IN_RANK+1);
}

//---------------------------------------------------------------------------------
void CQuoteListToolsDlg::CompareArray(CObArray* CountArray, int iPage, int iValue, int iEnum, int iFid)
{
	int iIndex = -1;
	int iArrayUpperBound = 0;
	int iRemoveIndex = -1;
	BOOL bFound = FALSE;

	if ((iRemoveIndex=IsInArray(CountArray, iPage, iFid))!=-1)
	{
		CFilterStruct* curFilter = (CFilterStruct*) CountArray->GetAt(iRemoveIndex);
		CountArray->RemoveAt(iRemoveIndex);
		delete curFilter;
	}

	if ((iArrayUpperBound=CountArray->GetUpperBound())>-1)
	{
		if ((CompareArrayValue((CFilterStruct*)CountArray->GetAt(iArrayUpperBound), iValue, iEnum)))
		{
			while (iArrayUpperBound>=++iIndex)
			{
				if ((bFound=CompareArrayValue((CFilterStruct*)CountArray->GetAt(iIndex), iValue, iEnum)))
				{
					CFilterStruct* tempFilter = new CFilterStruct; 
					tempFilter->lPageNo = iPage;
					tempFilter->FieldID = iFid;
					CountArray->InsertAt(iIndex, tempFilter );
					break;
				}
			}
		}
		if ((iArrayUpperBound<NO_OF_ITEM_IN_RANK)&&(!bFound))
		{
			CFilterStruct* tempFilter = new CFilterStruct; 
			tempFilter->lPageNo = iPage;
			tempFilter->FieldID = iFid;
			CountArray->SetAtGrow(iArrayUpperBound+1, tempFilter);
		}
	}
	else
	{
		CFilterStruct* tempFilter = new CFilterStruct; 
		tempFilter->lPageNo = iPage;
		tempFilter->FieldID = iFid;
		CountArray->Add(tempFilter);
	}
	if (CountArray->GetUpperBound()>NO_OF_ITEM_IN_RANK)
	{
		CFilterStruct* curFilter = (CFilterStruct*) CountArray->GetAt(NO_OF_ITEM_IN_RANK+1);
		CountArray->RemoveAt(NO_OF_ITEM_IN_RANK+1);
		delete curFilter;
	}
}


//---------------------------------------------------------------------------------
BOOL CQuoteListToolsDlg::ComparePeakArrayValue(CPeakInfo* tempPeak, int iValue, int iEnum)
{
	BOOL bReturnValue = FALSE;
	switch (iEnum)
	{
		case TOP_TEN_PEAK_TOTAL_SIZE:
		case TOP_TEN_PEAK_UPDATE_TOTAL_SIZE:
		case TOP_TEN_PEAK_VS_TOTAL_SIZE:
			if (tempPeak->iSize<iValue)
				bReturnValue = TRUE;
			break;
		case TOP_TEN_PEAK_TOTAL_MESSAGES:
		case TOP_TEN_PEAK_UPDATE_TOTAL_MESSAGES:
		case TOP_TEN_PEAK_VS_TOTAL_MESSAGES:
			if (tempPeak->iTimes<iValue)
				bReturnValue = TRUE;
			break;
		default:
			break;
	}
	return bReturnValue;
}

//---------------------------------------------------------------------------------
void CQuoteListToolsDlg::ComparePeakArray(CObArray* CountArray, int iSize, int iTimes, int iValue, int iEnum)
{
	int iIndex = -1;
	int iArrayUpperBound = 0;
	int iRemoveIndex = -1;
	BOOL bFound = FALSE;

	if ((iArrayUpperBound=CountArray->GetUpperBound())>-1)
	{
		if ((ComparePeakArrayValue((CPeakInfo*)CountArray->GetAt(iArrayUpperBound), iValue, iEnum)))
		{
			while (iArrayUpperBound>=++iIndex)
			{
				if ((bFound=ComparePeakArrayValue((CPeakInfo*)CountArray->GetAt(iIndex), iValue, iEnum)))
				{
					CPeakInfo* tempPeak = new CPeakInfo; 
					tempPeak->iSize = iSize;
					tempPeak->iTimes = iTimes;
					CountArray->InsertAt(iIndex, tempPeak );
					break;
				}
			}
		}
		if ((iArrayUpperBound<9)&&(!bFound))
		{
			CPeakInfo* tempPeak = new CPeakInfo; 
			tempPeak->iSize = iSize;
			tempPeak->iTimes = iTimes;
			CountArray->SetAtGrow(iArrayUpperBound+1, tempPeak);
		}
	}
	else
	{
		CPeakInfo* tempPeak = new CPeakInfo; 
		tempPeak->iSize = iSize;
		tempPeak->iTimes = iTimes;
		CountArray->Add(tempPeak);
	}
	if (CountArray->GetUpperBound()>9)
	{
		CPeakInfo* curPeak = (CPeakInfo*) CountArray->GetAt(10);
		CountArray->RemoveAt(10);
		delete curPeak;
	}
}

//---------------------------------------------------------------------------------
void CQuoteListToolsDlg::ProcessStatisticData(CStatisticPage* pStatisticPage, int iSize, int iRecordType, int iPage, int enumFieldId)
{
	m_CriticalSection.Enter();
	if (enumFieldId!=-1)
	{
		m_TotalSize+=iSize;
		m_TotalMessages++;
		CStatisticStruct* StatisticObject = NULL;
		
		if (!pStatisticPage->m_mapFields.Lookup(enumFieldId, (CStatisticStruct*&)StatisticObject ))
		{
			StatisticObject = new CStatisticStruct;
			pStatisticPage->m_mapFields[enumFieldId] = (CStatisticStruct*) StatisticObject ;
		}

		if (StatisticObject->iMaxSize<iSize)
		{
			StatisticObject->iMaxSize = iSize;
			CompareArray(&m_TopTenFidMaxSizeArray, iPage, StatisticObject->iMaxSize, TOP_TEN_FID_MAX_SIZE, enumFieldId);
		}

		StatisticObject->iTotalSize+=iSize;
		StatisticObject->iTotalTimes++;
		CompareArray(&m_TopTenFidTotalCountArray, iPage, StatisticObject->iTotalTimes, TOP_TEN_FID_TOTAL_COUNT, enumFieldId);
		CompareArray(&m_TopTenFidTotalSizeArray, iPage, StatisticObject->iTotalSize, TOP_TEN_FID_TOTAL_SIZE, enumFieldId);
		switch (iRecordType)
		{
			case MGT_UPDATE :
			case MGT_FORCEUPDATE:
				StatisticObject->iUpdateTimes++;
				StatisticObject->iUpdateSize+=iSize;
				CompareArray(&m_TopTenFidUpdateCountArray, iPage, StatisticObject->iUpdateTimes, TOP_TEN_FID_UPDATE_COUNT, enumFieldId);
				CompareArray(&m_TopTenFidUpdateSizeArray, iPage, StatisticObject->iUpdateSize, TOP_TEN_FID_UPDATE_SIZE, enumFieldId);
				m_UpdateTotalSize+=iSize;
				m_UpdateTotalMessages++;
				break;
			case MGT_VERIFYSYNC:
				StatisticObject->iVSTimes++;
				StatisticObject->iVSSize+=iSize;
				CompareArray(&m_TopTenFidVSCountArray, iPage, StatisticObject->iVSTimes, TOP_TEN_FID_VS_COUNT, enumFieldId);
				CompareArray(&m_TopTenFidVSSizeArray, iPage, StatisticObject->iVSSize, TOP_TEN_FID_VS_SIZE, enumFieldId);
				m_VSTotalSize+=iSize;
				m_VSTotalMessages++;
				break;
			case MGT_LOGONREQUEST:		
			case MGT_LOGONRESPONSE:		
			case MGT_ADDWATCH:		
			case MGT_DELETEWATCH:
			case MGT_SNAPSHOT:		
			case MGT_DATAPERMISSION:
			case MGT_CLOSINGRUN:		
			case MGT_DROP: 		
			case MGT_INTRADAYREBUILD:
			case MGT_INTRADAYCORRECTION:
			case MGT_ALIVE:
			case MGT_RAISEHANDREQUEST:
			case MGT_RAISEHANDREPLY:	
			case MGT_SYSTEMMESSAGE:	
				break;
			default:
				break;
		}
	}	
	else 
	{
		pStatisticPage->StatisticData.iTotalSize+=iSize;
		pStatisticPage->StatisticData.iTotalTimes++;

		if (pStatisticPage->StatisticData.iMaxSize<iSize)
		{
			pStatisticPage->StatisticData.iMaxSize = iSize;
			CompareArray(&m_TopTenPageMaxSizeArray, iPage, pStatisticPage->StatisticData.iMaxSize, TOP_TEN_PAGE_MAX_SIZE);
		}

		CompareArray(&m_TopTenPageTotalCountArray, iPage, pStatisticPage->StatisticData.iTotalTimes, TOP_TEN_PAGE_TOTAL_COUNT);
		CompareArray(&m_TopTenPageTotalSizeArray, iPage, pStatisticPage->StatisticData.iTotalSize, TOP_TEN_PAGE_TOTAL_SIZE);

		switch (iRecordType)
		{
			case MGT_UPDATE :
			case MGT_FORCEUPDATE:
				pStatisticPage->StatisticData.iUpdateTimes++;
				pStatisticPage->StatisticData.iUpdateSize+=iSize;
				CompareArray(&m_TopTenPageUpdateCountArray, iPage, pStatisticPage->StatisticData.iUpdateTimes, TOP_TEN_PAGE_UPDATE_COUNT);
				CompareArray(&m_TopTenPageUpdateSizeArray, iPage, pStatisticPage->StatisticData.iUpdateSize, TOP_TEN_PAGE_UPDATE_SIZE);
				break;
			case MGT_VERIFYSYNC:
				pStatisticPage->StatisticData.iVSTimes++;
				pStatisticPage->StatisticData.iVSSize+=iSize;
				CompareArray(&m_TopTenPageVSCountArray, iPage, pStatisticPage->StatisticData.iVSTimes, TOP_TEN_PAGE_VS_COUNT);
				CompareArray(&m_TopTenPageVSSizeArray, iPage, pStatisticPage->StatisticData.iVSSize, TOP_TEN_PAGE_VS_SIZE);
				break;
			case MGT_LOGONREQUEST:		
			case MGT_LOGONRESPONSE:		
			case MGT_ADDWATCH:		
			case MGT_DELETEWATCH:
			case MGT_SNAPSHOT:		
			case MGT_DATAPERMISSION:
			case MGT_CLOSINGRUN:		
			case MGT_DROP: 		
			case MGT_INTRADAYREBUILD:
			case MGT_INTRADAYCORRECTION:
			case MGT_ALIVE:
			case MGT_RAISEHANDREQUEST:
			case MGT_RAISEHANDREPLY:	
			case MGT_SYSTEMMESSAGE:	
				break;
			default:
				break;
		}
	}
	m_CriticalSection.Leave();
}

//---------------------------------------------------------------------------------

void CQuoteListToolsDlg::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == 1)
	{
		UpdateStatus();
		CString strOutput;
		strOutput.Format("No of Faults: %d", m_TestDCache->GetFaultCounter());
		SetDlgItemText(IDC_FAULTS, strOutput); 
	}
	else if (nIDEvent == 2)
	{
		ComparePeakArray(&m_TopTenPeakTotalSizeArray, m_TotalSize/10, m_TotalMessages/10, m_TotalSize/10, TOP_TEN_PEAK_TOTAL_SIZE);
		ComparePeakArray(&m_TopTenPeakTotalMessagesArray, m_TotalSize/10, m_TotalMessages/10, m_TotalMessages/10, TOP_TEN_PEAK_TOTAL_MESSAGES);
		ComparePeakArray(&m_TopTenPeakUpdateTotalSizeArray, m_UpdateTotalSize/10, m_UpdateTotalMessages/10, m_UpdateTotalSize/10, TOP_TEN_PEAK_UPDATE_TOTAL_SIZE);
		ComparePeakArray(&m_TopTenPeakUpdateTotalMessagesArray, m_UpdateTotalSize/10, m_UpdateTotalMessages/10, m_UpdateTotalMessages/10, TOP_TEN_PEAK_UPDATE_TOTAL_MESSAGES);
		ComparePeakArray(&m_TopTenPeakVSTotalSizeArray, m_VSTotalSize/10, m_VSTotalMessages/10, m_VSTotalSize/10,  TOP_TEN_PEAK_VS_TOTAL_SIZE);
		ComparePeakArray(&m_TopTenPeakVSTotalMessagesArray, m_VSTotalSize/10, m_VSTotalMessages/10, m_VSTotalMessages/10, TOP_TEN_PEAK_VS_TOTAL_MESSAGES);
		m_TotalSize = 0;
		m_TotalMessages = 0;
		m_UpdateTotalSize = 0;
		m_UpdateTotalMessages = 0;
		m_VSTotalSize = 0;
		m_VSTotalMessages = 0;
	}
	else if (nIDEvent == 3)
	{
		CTime Today = CTime::GetCurrentTime();
		int iNow = Today.GetHour()*60+Today.GetMinute();
		if (m_bLogStatusFile)
		{
			CButton* CheckBoxButton2 = (CButton*)GetDlgItem(IDC_AUTO_2);
			if (CheckBoxButton2->GetCheck())
			{
				char PathName[100];       
				char file_name[100];
				SYSTEMTIME sysTime;
				DWORD nBufferLength=100;

				GetSystemTime(&sysTime);
				CString strToday;
				GetCurrentDirectory(nBufferLength,  PathName);
				sprintf((char*) &PathName, "%s\\Data\\", PathName);
				strToday.Format(_T("%04d_%02d_%02d.log"), sysTime.wYear, sysTime.wMonth, sysTime.wDay);
				sprintf((char*) &file_name, "%s%s", PathName, strToday.GetBuffer(0));
				if (!m_StatusFile.FindFile(PathName, strToday.GetBuffer(0)))
				{
					m_bLogStatusFile = FALSE;
					m_StatusFile.Close();
					m_StatusFile.Open(file_name, JFile::A_Plus);
					m_bLogStatusFile = TRUE;
				}
				if (!((iNow<m_iTo) && (iNow>m_iFrom)))
					m_bLogStatusFile = FALSE;
			}
		} 
		else
		{
			CButton* CheckBoxButton2 = (CButton*)GetDlgItem(IDC_AUTO_2);
			if (CheckBoxButton2->GetCheck())
			{
				CButton* CheckBoxButton3 = (CButton*)GetDlgItem(IDC_OUTPUTTOFILE2);
				if (CheckBoxButton3->GetCheck())
				{
					if ((iNow<m_iTo) && (iNow>m_iFrom))
						m_bLogStatusFile = TRUE;
				}
			}
		}
	}
	else if (nIDEvent == 4)
	{
		if (m_bStartToRetrieveTCPData) 
		{
			if (!m_TestDCache->CheckTCPConnectionAlive())
			{
				CButton* CheckBoxButton1 = (CButton*)GetDlgItem(IDC_RETRIEVE);
				CheckBoxButton1->EnableWindow(TRUE);
				m_bStartToRetrieveTCPData = FALSE;					
			}
			else
				m_bTCPCtrlReceivedData = TRUE;
		}
	}
 
	CDialog::OnTimer(nIDEvent);
}

//---------------------------------------------------------------------------------
void CQuoteListToolsDlg::UpdateStatus() 
{
	if (!m_bDataCtrlReceivedData)
	{
		m_StatusAniCtrl.ShowWindow(SW_SHOW);
		m_StatusOK.ShowWindow(SW_HIDE);
	}
	else
	{
		m_StatusAniCtrl.ShowWindow(SW_HIDE);
		m_StatusOK.ShowWindow(SW_SHOW);
		m_bDataCtrlReceivedData = FALSE;
	}
	if (!m_bTCPCtrlReceivedData )
	{
		m_StatusAniCtrl2.ShowWindow(SW_SHOW);
		m_StatusOK2.ShowWindow(SW_HIDE);
	}
	else
	{
		m_StatusAniCtrl2.ShowWindow(SW_HIDE);
		m_StatusOK2.ShowWindow(SW_SHOW);
		m_bTCPCtrlReceivedData = FALSE;
	}
}
//---------------------------------------------------------------------------------
void CQuoteListToolsDlg::ResetStatisticContent() 
{
	// Reset all statistic Pages
	{	
		long	nKey;
		CStatisticPage*	objTemp;
		POSITION pos = m_mapPages.GetStartPosition();

		while (pos != NULL)
		{
			m_mapPages.GetNextAssoc( pos, nKey, objTemp );
			CStatisticPage* RemovedPage = (CStatisticPage*) objTemp;
			m_mapPages.RemoveKey(nKey);
			delete RemovedPage;
		}		
		m_mapPages.RemoveAll();
	}

	ResetFilterArray(&m_TopTenFidTotalCountArray);
	ResetFilterArray(&m_TopTenFidUpdateCountArray);
	ResetFilterArray(&m_TopTenFidVSCountArray);
	ResetFilterArray(&m_TopTenFidUpdateSizeArray);
	ResetFilterArray(&m_TopTenFidVSSizeArray);
	ResetFilterArray(&m_TopTenFidTotalSizeArray);
	ResetFilterArray(&m_TopTenFidMaxSizeArray);

	ResetPeakArray(&m_TopTenPeakTotalSizeArray);
	ResetPeakArray(&m_TopTenPeakTotalMessagesArray);
	ResetPeakArray(&m_TopTenPeakUpdateTotalSizeArray);
	ResetPeakArray(&m_TopTenPeakUpdateTotalMessagesArray);
	ResetPeakArray(&m_TopTenPeakVSTotalSizeArray);
	ResetPeakArray(&m_TopTenPeakVSTotalMessagesArray);

	m_TopTenPageMaxSizeArray.RemoveAll();
	m_TopTenPageTotalCountArray.RemoveAll();
	m_TopTenPageUpdateCountArray.RemoveAll();
	m_TopTenPageVSCountArray.RemoveAll();
	m_TopTenPageUpdateSizeArray.RemoveAll();
	m_TopTenPageVSSizeArray.RemoveAll();
	m_TopTenPageTotalSizeArray.RemoveAll();
	m_TotalSize = 0;
	m_TotalMessages = 0;
	m_UpdateTotalSize = 0;
	m_UpdateTotalMessages = 0;
	m_VSTotalSize = 0;
	m_VSTotalMessages = 0;
}
//---------------------------------------------------------------------------------
void CQuoteListToolsDlg::ResetFilterArray(CObArray* tempFilterArray) 
{
	while (tempFilterArray->GetSize()>0)
	{
		CFilterStruct* curFilter = (CFilterStruct*) tempFilterArray->GetAt(0);
		delete curFilter ;
		tempFilterArray->RemoveAt(0);

	}
}
//---------------------------------------------------------------------------------

void CQuoteListToolsDlg::ResetPeakArray(CObArray* tempPeakArray) 
{
	while (tempPeakArray->GetSize()>0)
	{
		CPeakInfo* curPeak = (CPeakInfo*) tempPeakArray->GetAt(0);
		delete curPeak ;
		tempPeakArray->RemoveAt(0);

	}
}
//---------------------------------------------------------------------------------

void CQuoteListToolsDlg::OnStatisticCheck() 
{
	UpdateData(TRUE);	
}
//---------------------------------------------------------------------------------

void CQuoteListToolsDlg::OnRetrieve() 
{
    UpdateData();

    m_ipRetrieve.GetAddress(m_usIP[0], m_usIP[1], m_usIP[2], m_usIP[3]);

    CString s, csMsg;
    s.Format("%d.%d.%d.%d", m_usIP[0], m_usIP[1], m_usIP[2], m_usIP[3]);

	string m_strHost = s.GetBuffer(0);

	CButton* CheckBoxButton = (CButton*)GetDlgItem(IDC_TCP_DCACHE);
	if (!CheckBoxButton->GetCheck())
	{
		if (!m_TCPCtrl.Start())
		{
			JRegistry reg;
			char ip_address[20];
			sprintf(ip_address, "%s", s.GetBuffer(0));
			reg.write_value("SOFTWARE\\AFE\\QuoteListTools", "IP_TCPAddress", REG_SZ, (const unsigned char *)&ip_address, strlen(ip_address));
			reg.write("SOFTWARE\\AFE\\QuoteListTools","TCPPortNumber", m_unTCPPort);
			
			m_TCPCtrl.RegisterCallback(TCPCtrlPutDataToDCache, this);
			m_TCPCtrl.AddGroup(s, m_unTCPPort);
			csMsg.Format("TCP Retrieval started! - IP:%s  Port:%d", s.GetBuffer(0), m_unTCPPort);
		}
	}
	else
	{
		JRegistry reg;
		char ip_address[20];
		sprintf(ip_address, "%s", s.GetBuffer(0));
		reg.write_value("SOFTWARE\\AFE\\QuoteListTools", "IP_TCPAddress", REG_SZ, (const unsigned char *)&ip_address, strlen(ip_address));
		reg.write("SOFTWARE\\AFE\\QuoteListTools","TCPPortNumber", m_unTCPPort);
		
		csMsg.Format("TCP Retrieval from DCache started! - IP:%s  Port:%d", s.GetBuffer(0), m_unTCPPort);
		m_TestDCache->RetrieveDataByTCP(s, m_unTCPPort);
		CButton* CheckBoxButton1 = (CButton*)GetDlgItem(IDC_RETRIEVE);
		CheckBoxButton1->EnableWindow(FALSE);
		m_bStartToRetrieveTCPData = TRUE;
		Sleep(1000);
		if (m_TestDCache->CheckTCPConnectionAlive())
			m_bTCPCtrlReceivedData = TRUE;
	}
	OnTimer(1);
	Log(csMsg, TRUE);	
}
//----------------------------------------------------------------------------

void CQuoteListToolsDlg::OnRetrieveStop() 
{
	m_bStatusDisplay = FALSE;
	PostMessage(WM_STOP_TCPCTRL);
	PostMessage(WM_RESUME_STATUS);
}
//----------------------------------------------------------------------------

void CQuoteListToolsDlg::StopTCPCtrl(UINT wParam, LONG lParam)
{
	m_TCPCtrl.UnRegisterCallback(TCPCtrlPutDataToDCache, this);
	m_TCPCtrl.Stop();
}

//---------------------------------------------------------------------------------

void CQuoteListToolsDlg::OnMsgDetailCheck() 
{
	UpdateData(TRUE);	
}
//---------------------------------------------------------------------------------

void CQuoteListToolsDlg::OnShowInfoCheck() 
{
	UpdateData(TRUE);	
}
//---------------------------------------------------------------------------------

void CQuoteListToolsDlg::OnSize(UINT nType, int cx, int cy) 
{
	if (cx > 605)
	{
		CRect rect, rect2;
		CDialog::OnSize(nType, cx, cy);
		if (m_DCachelist.GetSafeHwnd())
		{
			m_DCachelist.SetWindowPos(NULL, 0, 0, cx-30, cy-200, SWP_NOZORDER|SWP_NOMOVE);
			m_Statuslist.SetWindowPos(NULL, 0, 0, cx-30, cy-200, SWP_NOZORDER|SWP_NOMOVE);
			m_QuoteList.SetWindowPos(NULL, 0, 0, cx-30, cy-200, SWP_NOZORDER|SWP_NOMOVE);
			CButton* GroupBoxButton = (CButton*)GetDlgItem(IDC_STATICGROUP_BOX);
			GroupBoxButton->SetWindowPos(NULL, 0, 0, cx-12, cy-150, SWP_NOZORDER|SWP_NOMOVE);
		}
	}
}

//---------------------------------------------------------------------------------
/*
void CQuoteListToolsDlg::OnTestdrop() 
{
	CString strOutput;
	UpdateData(TRUE);
	if (m_bTestDrop)
	{
		m_DataCtrl.AddChannel("225.1.1.14", 6000);
		m_DataCtrl.SetSvrDPID(103, 1);
		strOutput.Format("Add Channel 225.1.1.14 port 6000 & set DPID = 103 & INSID = 1");
	}
	else
	{
		m_DataCtrl.DelChannel("225.1.1.14", 6000);
		strOutput.Format("Remove Channel 225.1.1.14 port 6000");
	}
	Log(strOutput, TRUE);
	Log("  ", TRUE);

}
*/
//---------------------------------------------------------------------------------

void CQuoteListToolsDlg::OnLoadSetting() 
{

	static char BASED_CODE szFilter[] = _T("Settings Files (*.set)|*.set|All Files (*.*)|*.*||");
	CFileDialog dlg( TRUE, _T("Data"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	dlg.DoModal();
	CString strFileName = dlg.GetPathName();
	BOOL bReadSuccessFlag = FALSE;
	if (strFileName != "")
	{
		char strInput[1000];

		CButton* CheckBoxButton;
		m_bDCacheDisplay = GetPrivateProfileInt("GENERAL", "DCacheDisplay", 0, strFileName);
		if (!m_bDCacheDisplay)
			SetDlgItemText(IDC_DISPLAY1, "Display");
		else
			SetDlgItemText(IDC_DISPLAY1, "Don't Display");


		m_bStatusDisplay = GetPrivateProfileInt("GENERAL", "StatusDisplay", 0, strFileName);
		if (!m_bStatusDisplay)
			SetDlgItemText(IDC_DISPLAY2, "Display");
		else
			SetDlgItemText(IDC_DISPLAY2, "Don't Display");

		m_bLogWithTime = GetPrivateProfileInt("GENERAL", "LogWithTime", 0, strFileName);
		CheckBoxButton = (CButton*)GetDlgItem(IDC_TIMECHECK);
		CheckBoxButton->SetCheck(m_bLogWithTime);

		m_bLogStatisticData = GetPrivateProfileInt("GENERAL", "LogStatisticData", 0, strFileName);
		CheckBoxButton = (CButton*)GetDlgItem(IDC_STATISTIC_CHECK);
		CheckBoxButton->SetCheck(m_bLogStatisticData);

		m_bShowMsgDetail = GetPrivateProfileInt("GENERAL", "ShowMsgDetail", 0, strFileName);
		CheckBoxButton = (CButton*)GetDlgItem(IDC_MSG_DETAIL_CHECK);
		CheckBoxButton->SetCheck(m_bShowMsgDetail);

		m_bShowMsgInfo = GetPrivateProfileInt("GENERAL", "ShowMsgInfo", 0, strFileName);
		CheckBoxButton = (CButton*)GetDlgItem(IDC_SHOW_INFO_CHECK);
		CheckBoxButton->SetCheck(m_bShowMsgInfo);

		m_bPutDataToCache = GetPrivateProfileInt("GENERAL", "PutDataToCache", 0, strFileName);
		CheckBoxButton = (CButton*)GetDlgItem(IDC_PUTDATATOCACHE);
		CheckBoxButton->SetCheck(m_bPutDataToCache);


		GetPrivateProfileString("GENERAL", "StatusFileName", "", strInput, sizeof(strInput), strFileName);
		SetDlgItemText(IDC_FILENAME2, strInput);

		GetPrivateProfileString("GENERAL", "DCacheFileName", "", strInput, sizeof(strInput), strFileName);
		SetDlgItemText(IDC_FILENAME, strInput);		

		m_bLogDCacheFile = GetPrivateProfileInt("GENERAL", "LogDCacheFile", 0, strFileName);
		CheckBoxButton = (CButton*)GetDlgItem(IDC_OUTPUTTOFILE);
		if (CheckBoxButton->GetCheck()==m_bLogDCacheFile)
		{
			if (CheckBoxButton->GetCheck()&&m_bLogDCacheFile)
			{
				CheckBoxButton->SetCheck(FALSE);
				OnOutputtofile();
				CheckBoxButton->SetCheck(TRUE);
				OnOutputtofile();
			}
		}
		else
		{
			CheckBoxButton->SetCheck(m_bLogDCacheFile);
			OnOutputtofile();
		}


		GetPrivateProfileString("GENERAL", "LogFrom", "", strInput, sizeof(strInput), strFileName);
		m_strLogFrom = strInput;
		GetPrivateProfileString("GENERAL", "LogTo", "", strInput, sizeof(strInput), strFileName);
		m_strLogTo = strInput;
		int iHour, iMin;
		sscanf(m_strLogFrom, "%d:%d", &iHour, &iMin);
		m_iFrom = iHour*60+iMin-1;
		sscanf(m_strLogTo, "%d:%d", &iHour, &iMin);
		m_iTo = iHour*60+iMin+1;

		CheckBoxButton = (CButton*)GetDlgItem(IDC_AUTO_2);
		BOOL bAutoLog = GetPrivateProfileInt("GENERAL", "AutoLock", 0, strFileName);
		CheckBoxButton->SetCheck(bAutoLog);
		if (CheckBoxButton->GetCheck())
		{
			CEdit * pEditBox = (CEdit *)this->GetDlgItem(IDC_FILENAME2);
			pEditBox->EnableWindow(FALSE);
		}

		CheckBoxButton = (CButton*)GetDlgItem(IDC_OUTPUTTOFILE2);
		m_bLogStatusFile = GetPrivateProfileInt("GENERAL", "LogStatusFile", 0, strFileName);
		if (CheckBoxButton->GetCheck()==m_bLogStatusFile)
		{
			if (CheckBoxButton->GetCheck()&&m_bLogStatusFile)
			{
				CheckBoxButton->SetCheck(FALSE);
				OnOutputtofile2();
				CheckBoxButton->SetCheck(TRUE);
				OnOutputtofile2();
			}
		}
		else
		{
			CheckBoxButton->SetCheck(m_bLogStatusFile);
			m_bLogStatusFile = FALSE;
			OnOutputtofile2();
		}

		int NoOfFilter = GetPrivateProfileInt("GENERAL", "NoOfFilter", 0, strFileName);
		ResetFilterArray(&m_FilterArray);
		if ( NoOfFilter>0)
		{
			for (int i = 0; i < NoOfFilter; i++)
			{
				char tempString[1000];

				CFilterStruct* tempFilter = new CFilterStruct;
				sprintf(tempString, "Filter%dPageNo", i);
				tempFilter->lPageNo = GetPrivateProfileInt("GENERAL", tempString, -1, strFileName);
				sprintf(tempString, "Filter%dFieldID", i);
				tempFilter->FieldID = GetPrivateProfileInt("GENERAL", tempString, -1, strFileName);
				sprintf(tempString, "Filter%dMsgType", i);
				tempFilter->bytMsgType = GetPrivateProfileInt("GENERAL", tempString, -1, strFileName);
				sprintf(tempString, "Filter%dDPID", i);
				tempFilter->sDPID = GetPrivateProfileInt("GENERAL", tempString, -1, strFileName);
				m_FilterArray.Add(tempFilter);
			}
		}
		m_dlgLogFilter->Init();
		
		if (m_bStart)
			OnStop();
		m_bStart = GetPrivateProfileInt("GENERAL", "Start", FALSE, strFileName);
		m_unPort = GetPrivateProfileInt("GENERAL", "port", 6000, strFileName);
		sprintf(strInput, "%d", m_unPort);
		SetDlgItemText(IDC_PORT, strInput);
		GetPrivateProfileString("GENERAL", "ip_address", "", strInput, sizeof(strInput), strFileName);
		if (strInput[0]!=0)
			sscanf(strInput, "%d.%d.%d.%d", &m_ip1, &m_ip2, &m_ip3, &m_ip4);
		m_ipLocal.SetAddress(m_ip1, m_ip2, m_ip3, m_ip4);

		m_unTCPPort = GetPrivateProfileInt("GENERAL", "TCP_Port", 10014, strFileName);
		sprintf(strInput, "%d", m_unTCPPort);
		SetDlgItemText(IDC_PORT2, strInput);
		GetPrivateProfileString("GENERAL", "Retrieve_ip_address", "", strInput, sizeof(strInput), strFileName);
		if (strInput[0]!=0)
			sscanf(strInput, "%d.%d.%d.%d", &m_ip1, &m_ip2, &m_ip3, &m_ip4);
		m_ipRetrieve.SetAddress(m_ip1, m_ip2, m_ip3, m_ip4);

		UpdateData();
		if (m_bStart)
			OnStart();
		m_GroupArray.RemoveAll();
		m_GroupCombo.ResetContent();
		int NoOfGroups = GetPrivateProfileInt("GENERAL", "NoOfGroups", 0, strFileName);
		for (int i=0; i<NoOfGroups;i++)
		{
			char tempString[1000];
			sprintf(tempString, "group%d", i);
			GetPrivateProfileString("GENERAL", tempString, "", strInput, sizeof(strInput), strFileName);
			m_GroupArray.Add(strInput);
			if (m_bStart)
			{
				unsigned char a, b, c, d;
				CString csLocal, csMsg;

				m_ipLocal.GetAddress(a,b,c,d);
				csLocal.Format("%d.%d.%d.%d", a, b, c, d);

				if (m_DataCtrl.AddGroup(strInput, csLocal) == CDataCtrl::DC_ERROR)
					csMsg.Format("Add group %s failed! - IP:%s", strInput, csLocal);
				else
			        csMsg.Format("Add group %s - IP:%s", strInput, csLocal);
				Log(csMsg, TRUE);	
			}
		}
		UpdateList();
		m_GroupCombo.SetCurSel(m_GroupArray.GetSize()-1);
		bReadSuccessFlag = TRUE;
	}
	if (!bReadSuccessFlag)
		AfxMessageBox("Retrieved Setting unsucessfully!!");
	else
		AfxMessageBox("Retrieved Setting sucessfully!!");

}
//---------------------------------------------------------------------------------

void CQuoteListToolsDlg::OnSaveSetting() 
{
    UpdateData(); 

	static char BASED_CODE szFilter[] = _T("Settings Files (*.set)|*.set|All Files (*.*)|*.*||");
	CFileDialog dlg( FALSE, _T("Data"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	dlg.DoModal();
	CString strFileName = dlg.GetPathName();
	if (strFileName != "")
	{
		JFile			SettingFile;
		if (SettingFile.Open(strFileName, JFile::W_Plus))
		{

			char strOutput[1000];
			sprintf(strOutput, "[GENERAL]\n");
			SettingFile.Write(strOutput, strlen(strOutput));
			
			m_ipLocal.GetAddress(m_usIP[0], m_usIP[1], m_usIP[2], m_usIP[3]);
			sprintf(strOutput, "ip_address = %d.%d.%d.%d\n", m_usIP[0], m_usIP[1], m_usIP[2], m_usIP[3]);
			SettingFile.Write(strOutput, strlen(strOutput));

			sprintf(strOutput, "port = %d\n", m_unPort);
			SettingFile.Write(strOutput, strlen(strOutput));

			m_ipRetrieve.GetAddress(m_usIP[0], m_usIP[1], m_usIP[2], m_usIP[3]);
			sprintf(strOutput, "Retrieve_ip_address = %d.%d.%d.%d\n", m_usIP[0], m_usIP[1], m_usIP[2], m_usIP[3]);
			SettingFile.Write(strOutput, strlen(strOutput));

			sprintf(strOutput, "TCP_Port = %d\n", m_unTCPPort);
			SettingFile.Write(strOutput, strlen(strOutput));


			sprintf(strOutput, "NoOfGroups = %d\n", m_GroupArray.GetSize());
			SettingFile.Write(strOutput, strlen(strOutput));
			for (int i=0; i<m_GroupArray.GetSize();i++)
			{
				//sprintf(strOutput, "group%d = %s\n", i, m_GroupArray.GetAt(i).GetBuffer(0));
				SettingFile.Write(strOutput, strlen(strOutput));
			}

			sprintf(strOutput, "DCacheDisplay = %d\n", m_bDCacheDisplay);
			SettingFile.Write(strOutput, strlen(strOutput));

			sprintf(strOutput, "StatusDisplay = %d\n", m_bStatusDisplay);
			SettingFile.Write(strOutput, strlen(strOutput));

			CButton* CheckBoxButton2 = (CButton*)GetDlgItem(IDC_OUTPUTTOFILE2);		
			sprintf(strOutput, "LogStatusFile = %d\n", CheckBoxButton2->GetCheck());
			SettingFile.Write(strOutput, strlen(strOutput));

			char file_name[100];
			GetDlgItemText(IDC_FILENAME2, file_name, 100);
			sprintf(strOutput, "StatusFileName = %s\n", file_name);
			SettingFile.Write(strOutput, strlen(strOutput));

			sprintf(strOutput, "LogDCacheFile = %d\n", m_bLogDCacheFile);
			SettingFile.Write(strOutput, strlen(strOutput));

			GetDlgItemText(IDC_FILENAME, file_name, 100);
			sprintf(strOutput, "DCacheFileName = %s\n", file_name);
			SettingFile.Write(strOutput, strlen(strOutput));

			sprintf(strOutput, "NoOfFilter = %d\n", m_FilterArray.GetSize());
			SettingFile.Write(strOutput, strlen(strOutput));
			if ( m_FilterArray.GetSize()>0)
			{
				for (int i = 0; i < m_FilterArray.GetSize(); i++)
				{
					CFilterStruct* curFilter = (CFilterStruct*) m_FilterArray.GetAt(i);
					
					sprintf(strOutput, "Filter%dPageNo = %d\n", i, curFilter->lPageNo);
					SettingFile.Write(strOutput, strlen(strOutput));
					sprintf(strOutput, "Filter%dFieldID = %d\n", i, curFilter->FieldID);
					SettingFile.Write(strOutput, strlen(strOutput));
					sprintf(strOutput, "Filter%dMsgType = %d\n", i, curFilter->bytMsgType);
					SettingFile.Write(strOutput, strlen(strOutput));
					sprintf(strOutput, "Filter%dDPID = %d\n", i, curFilter->sDPID);
					SettingFile.Write(strOutput, strlen(strOutput));
				}
			}

			sprintf(strOutput, "LogWithTime = %d\n", m_bLogWithTime);
			SettingFile.Write(strOutput, strlen(strOutput));

			sprintf(strOutput, "LogStatisticData = %d\n", m_bLogStatisticData);
			SettingFile.Write(strOutput, strlen(strOutput));

			sprintf(strOutput, "ShowMsgDetail = %d\n", m_bShowMsgDetail);
			SettingFile.Write(strOutput, strlen(strOutput));

			sprintf(strOutput, "ShowMsgInfo = %d\n", m_bShowMsgInfo);
			SettingFile.Write(strOutput, strlen(strOutput));

			sprintf(strOutput, "PutDataToCache = %d\n", m_bPutDataToCache);
			SettingFile.Write(strOutput, strlen(strOutput));

			sprintf(strOutput, "Start = %d\n", m_bStart );
			SettingFile.Write(strOutput, strlen(strOutput));

			CButton* CheckBoxButton = (CButton*)GetDlgItem(IDC_AUTO_2);
			sprintf(strOutput, "AutoLock = %d\n", CheckBoxButton->GetCheck());
			SettingFile.Write(strOutput, strlen(strOutput));
			

			sprintf(strOutput, "LogFrom = %s\n", m_strLogFrom.GetBuffer(0));
			SettingFile.Write(strOutput, strlen(strOutput));
			
			sprintf(strOutput, "LogTo = %s\n", m_strLogTo.GetBuffer(0));
			SettingFile.Write(strOutput, strlen(strOutput));

			SettingFile.Close();
		}
		else
		{
			CString strError = "Cannot open file - ";
			strError += strFileName;
			AfxMessageBox(strError);
		}
	}	
}
//---------------------------------------------------------------------------------

void CQuoteListToolsDlg::OnSendStart() 
{
    UpdateData();

    m_ipLocalSend.GetAddress(m_usSIP[0], m_usSIP[1], m_usSIP[2], m_usSIP[3]);

    CString s, csMsg;
    s.Format("%d.%d.%d.%d", m_usSIP[0], m_usSIP[1], m_usSIP[2], m_usSIP[3]);
    if (m_SendDataCtrl.Start(m_unSendPort, s)  == CMCast::MC_SUCCESS)
	{
		JRegistry reg;
		char ip_address[20];
		sprintf(ip_address, "%s", s.GetBuffer(0));
		reg.write_value("SOFTWARE\\AFE\\QuoteListTools", "IP_AddressSend", REG_SZ, (const unsigned char *)&ip_address, strlen(ip_address));
		reg.write("SOFTWARE\\AFE\\QuoteListTools","SendPortNumber", m_unPort);
        csMsg.Format("SEND DATACTRL started! - IP:%s  Port:%d", s.GetBuffer(0), m_unPort);
	}
    else
        csMsg = "SEND DATACTRL start error!";
	m_bStart = TRUE;
	Log(csMsg, TRUE);	

}
//---------------------------------------------------------------------------------

void CQuoteListToolsDlg::OnSendStop() 
{
	m_SendDataCtrl.Stop();
	Log("SEND DATACTRL Stopped!!", TRUE);	
}
//---------------------------------------------------------------------------------

void CQuoteListToolsDlg::OnAddChannel() 
{
    CString csMsg;
    UpdateData();

	m_SendDataCtrl.AddChannel(m_strChannel, m_unSendPort);
	JRegistry reg;
	char ip_address[20];
	sprintf(ip_address, "%s", m_strChannel.GetBuffer(0));
	reg.write_value("SOFTWARE\\AFE\\QuoteListTools", "Channel_IP_Address", REG_SZ, (const unsigned char *)&ip_address, strlen(ip_address));
    csMsg.Format("Add Channel %s - Port:%d", m_strChannel, m_unSendPort);
	BOOL bFound = FALSE;
	int iTotalChannel = m_ChannelArray.GetSize();
	if (iTotalChannel>0)
	{
		while (iTotalChannel>0)
		{
			iTotalChannel--;
			if (m_ChannelArray.GetAt(iTotalChannel)==m_strChannel)
			{
				bFound = TRUE;
				break;
			}
		}
	}
	if (!bFound)
		m_ChannelArray.Add(m_strChannel);
	UpdateChannelList();
	m_ChannelCombo.SetCurSel(m_ChannelArray.GetSize()-1);

    Log(csMsg, TRUE);	
	
}


//----------------------------------------------------------------------------
void CQuoteListToolsDlg::UpdateChannelList() 
{
	m_ChannelCombo.ResetContent();
	for (int i = 0; i < m_ChannelArray.GetSize(); i++)
		m_ChannelCombo.InsertString(i, m_ChannelArray.GetAt(i));
}
//----------------------------------------------------------------------------

void CQuoteListToolsDlg::OnRemoveChannel() 
{
	CString strOutput;
	UpdateData(TRUE);
	m_DataCtrl.DelChannel(m_strChannel, m_unSendPort);
	strOutput.Format("Remove Channel %s - Port:%d", m_strChannel, m_unSendPort);

	int iTotalChannel = m_ChannelArray.GetSize();
	if (iTotalChannel>0)
	{
		while (iTotalChannel>0)
		{
			iTotalChannel--;
			if (m_ChannelArray.GetAt(iTotalChannel)==m_strChannel)
			{
				m_ChannelArray.RemoveAt(iTotalChannel);
				break;
			}
		}
	}
	UpdateChannelList(); 
	if (m_ChannelArray.GetSize()>0)
		m_ChannelCombo.SetCurSel(m_ChannelArray.GetSize()-1);
	Log(strOutput, TRUE);
	Log("  ", TRUE);

}
//----------------------------------------------------------------------------

void CQuoteListToolsDlg::OnSendMessage() 
{
	UpdateData();
	if (!m_dlgSendMessage->IsWindowVisible())
		m_dlgSendMessage->ShowWindow(SW_RESTORE);
	else
		m_dlgSendMessage->SetActiveWindow();
}


//----------------------------------------------------------------------------

BOOL CQuoteListToolsDlg::CheckDirectory(CString strBasePath, CString& subDir)
{
	SYSTEMTIME sysTime;

	GetSystemTime(&sysTime);
	subDir.Format(_T("%04d_%02d_%02d"), sysTime.wYear, sysTime.wMonth, sysTime.wDay);
	CString path;
	path.Format(_T("%s\\%s"), strBasePath, subDir);
	if (!CreateDirectory(path, NULL))
	{
		DWORD err=GetLastError();
		if (err!=ERROR_ALREADY_EXISTS)
			return FALSE;
	}
	return TRUE;
}


//----------------------------------------------------------------------------

void CQuoteListToolsDlg::OnTcpDcache() 
{
	CButton* CheckBoxButton = (CButton*)GetDlgItem(IDC_TCP_DCACHE);
	CButton* CheckBoxButton1 = (CButton*)GetDlgItem(IDC_RETRIEVE_STOP);
	if (!CheckBoxButton->GetCheck())
		CheckBoxButton1->EnableWindow(TRUE);
	else
		CheckBoxButton1->EnableWindow(FALSE);
}
//----------------------------------------------------------------------------

void CQuoteListToolsDlg::OnSocketServer() 
{	
	CSocketServerDlg dlgSocketServer;
	dlgSocketServer.SetMyDialog(this);
		dlgSocketServer.DoModal();
}
//----------------------------------------------------------------------------

void CQuoteListToolsDlg::OnExit() 
{
	ResetFilterArray(&m_FilterArray);
	this->PostMessage(WM_SYSCOMMAND, SC_CLOSE);
}

//----------------------------------------------------------------------------


void CQuoteListToolsDlg::OnClose() 
{
	m_bStatusDisplay = FALSE;
	StopDataCtrl(0,0);
	StopTCPCtrl(0,0);
	m_dlgLogFilter->DestroyWindow();
	delete m_dlgLogFilter;
	m_dlgSendMessage->DestroyWindow();
	delete m_dlgSendMessage;
	while (m_MyQueue->GetCount() > 0)
		Sleep(1);   
	delete m_MyQueue;  
	m_TestDCache->ForceWriteToDisk(); 
	delete m_TestDCache;
	ResetStatisticContent();
	CDialog::OnCancel();
}

//----------------------------------------------------------------------------

void CQuoteListToolsDlg::OnRemoveAll() 
{
	CString strTemp = "Are you sure you want to drop all pages";
	if (::AfxMessageBox(strTemp, MB_YESNO |MB_DEFBUTTON2) == IDYES)
	{
		CDWordArray	cdwaPageList;
		if (m_TestDCache->GetPageList(cdwaPageList))
		{
			do 
			{
				if (cdwaPageList.GetSize()>0)
				{
					long lCurPageNo = cdwaPageList.GetAt(0);
					cdwaPageList.RemoveAt(0);
					m_TestDCache->DropAPage(lCurPageNo);
				}
			} while (cdwaPageList.GetSize()>0);
		}
		CString strOutput = "Drop All Pages - by U!";
		Log(strOutput);
	}
}
//----------------------------------------------------------------------------


void CQuoteListToolsDlg::OnShowquotelist() 
{
	CString strButtonName;
	GetDlgItemText(IDC_SHOWQUOTELIST, strButtonName);
	m_bShowQuoteList = (strButtonName == "ShowQuoteList")?TRUE:FALSE;
	if (strButtonName != "ShowQuoteList")
		SetDlgItemText(IDC_SHOWQUOTELIST, "ShowQuoteList");
	else
		SetDlgItemText(IDC_SHOWQUOTELIST, "ShowNormal");

}
//----------------------------------------------------------------------------


void CQuoteListToolsDlg::OnSelchangeListchoiceCombo() 
{
	UpdateData();
	switch (m_iListChoice)
	{
	case 0:
		m_DCachelist.ShowWindow(SW_SHOW );
		m_Statuslist.ShowWindow(SW_HIDE );
		m_QuoteList.ShowWindow(SW_HIDE );
		break;
	case 2:
		m_DCachelist.ShowWindow(SW_HIDE );
		m_Statuslist.ShowWindow(SW_SHOW );
		m_QuoteList.ShowWindow(SW_HIDE );
		break;
	case 1:
		m_DCachelist.ShowWindow(SW_HIDE );
		m_Statuslist.ShowWindow(SW_HIDE );
		m_QuoteList.ShowWindow(SW_SHOW );
		break;
	default:
		break;
	}	
}
//---------------------------------------------------------------------------------
void CQuoteListToolsDlg::OnRegister() 
{
	CString strOutput;
	UpdateData(TRUE);
	int iFidNo = (m_strFid!="")?atoi(m_strFid):-1;
	long lPageNo = 0;
	if (m_sPageNo.GetLength() > 1)
	{
		if ((m_sPageNo.Left(2) == "0x")||(m_sPageNo.Left(2) == "0X"))
			sscanf(m_sPageNo, "%x", &lPageNo);
		else
			sscanf(m_sPageNo, "%d", &lPageNo);
	}
	else
		lPageNo = atoi(m_sPageNo);
	
	CString strButtonName;
	GetDlgItemText(IDC_REGISTER, strButtonName);
	if (strButtonName != "Reg")
	{
		if ((m_iCurFidNo != -1) && (m_lCurPageNo != -1))
		{
			SetDlgItemText(IDC_REGISTER, "Reg");
			m_TestDCache->RemoveAPageAndFieldWatcher(m_lCurPageNo, (FID_DEFINITION)m_iCurFidNo, this);
			strOutput.Format("Unregister successfully, %d (%x), Fid: %d", m_lCurPageNo,m_lCurPageNo,m_iCurFidNo);
			AfxMessageBox(strOutput);
		}
		else
		{
			strOutput.Format("Unregister unsuccessfully, %d (%x), Fid: %d", m_lCurPageNo,m_lCurPageNo,m_iCurFidNo);
			AfxMessageBox(strOutput);
		}

	}
	else
	{
		if ((lPageNo != 0) && (iFidNo != -1))
		{
			m_iCurFidNo = iFidNo;
			m_lCurPageNo = lPageNo;
			m_TestDCache->AddAPageAndFieldWatcher(m_lCurPageNo,(FID_DEFINITION)m_iCurFidNo, (CObject *)this, IteratedUpdateQuoteList);
			SetDlgItemText(IDC_REGISTER, "UnReg");
			strOutput.Format("Register successfully, %d (%x), Fid: %d", m_lCurPageNo,m_lCurPageNo,m_iCurFidNo);
			AfxMessageBox(strOutput);
		}
		else
		{
			strOutput.Format("Please enter PageNo and Fid No and the register");
			AfxMessageBox(strOutput);
		}
	}
}
