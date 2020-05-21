// QuoteListToolsDlg.h : header file
//

#if !defined(AFX_QuoteListToolsDLG_H__6D577567_D73F_11D5_9CA0_0006294E7FE4__INCLUDED_)
#define AFX_QuoteListToolsDLG_H__6D577567_D73F_11D5_9CA0_0006294E7FE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "afxcmn.h"
#include "Resource.h"
#include "DataCtrl.h"
#include "Page.h"
#include "ssCountedObjPtr.h"
#include "MsgObject.h"
#include "Dcache.h"
#include "JFile.h"
#include "StatisticPage.h"
#include "TCPCtrl.h"
#include "ABTPanelEx.h"

#define WM_STOP_DATACTRL (WM_USER+1)
#define WM_RESUME_STATUS (WM_USER+2)
#define WM_STOP_TCPCTRL  (WM_USER+3)
#define NO_OF_ITEM_IN_RANK	9

typedef CMap<long,long,CStatisticPage*,CStatisticPage*> CStatisticPageMap;

/////////////////////////////////////////////////////////////////////////////
// CQuoteListToolsDlg dialog
class CFilterStruct : public CObject
{
	public:
		~CFilterStruct () {}
		CFilterStruct ()
		{
			lPageNo = -1;
			FieldID = -1;
			bytMsgType = -1;
			sDPID	= -1; 
		}
		long lPageNo;
		int  FieldID;
		int bytMsgType;
		short sDPID;
};

class CPeakInfo : public CObject
{
	public:
		~CPeakInfo() {}
		CPeakInfo ()
		{
			iSize = 0;
			iTimes = 0;
			thatTime = CTime::GetCurrentTime();
		}
		int iSize;
		int iTimes;
		CTime thatTime;
};

class CLogFilterDlg;
class CStatisticStruct;
class CMsgQueueManager;
class CSendMessageDialog;
class CQuoteListToolsDlg : public CDialog
{

// Construction
public:
	CQuoteListToolsDlg(CWnd* pParent = NULL);	// standard constructor

	int				m_iTimer;
	int				m_TotalSize;
	int				m_TotalMessages;
	int				m_UpdateTotalSize;
	int				m_UpdateTotalMessages;
	int				m_VSTotalSize;
	int				m_VSTotalMessages;
	CFont			m_font;

	unsigned char	m_usIP[4];
	unsigned char	m_usSIP[4];
	BOOL			m_bPutDataToCache;
	BOOL			m_bDataCtrlReceivedData;
	BOOL			m_bTCPCtrlReceivedData;
	CString			m_strLogFrom;
	CString			m_strLogTo;	
	int				m_iFrom;
	int				m_iTo;
	JFile			m_DCacheFile;
	JFile			m_StatusFile;
	CString			m_strDCacheFileName;
	CString			m_strStatusFileName;
	BOOL			m_bLogDCacheFile;
	BOOL			m_bLogStatusFile;
	BOOL			m_bDCacheDisplay;
	BOOL			m_bStatusDisplay;
	BOOL			m_bStart;
	BOOL			m_bStartToRetrieveTCPData;
	BOOL			m_bShowQuoteList;

	CDCache*		m_TestDCache;
	CDataCtrl		m_DataCtrl;
	CTCPCtrl		m_TCPCtrl;
	CDataCtrl		m_SendDataCtrl;
	CPage*			m_page;
	CObArray		m_FilterArray;
	CStringArray	m_GroupArray;
	CStringArray	m_ChannelArray;
	int				m_ip1, m_ip2, m_ip3, m_ip4;
	int				m_rcvip1, m_rcvip2, m_rcvip3, m_rcvip4;
	int				m_tcpip1, m_tcpip2, m_tcpip3, m_tcpip4;
	unsigned long	m_port;
	unsigned long	m_tcpport;
	char			m_szFormattedSystemTime[200];
	CMsgQueueManager* m_MyQueue;
	ssCriticalSection	m_CriticalSection;
	CLogFilterDlg*	m_dlgLogFilter;
	CSendMessageDialog* m_dlgSendMessage;
	long m_lCurPageNo;
	int m_iCurFidNo;

	const char* GetFormattedSystemTime()
	{
		SYSTEMTIME SysTime;
		GetSystemTime(&SysTime);
		sprintf(m_szFormattedSystemTime, "%04u/%02u/%02u %02u:%02u:%02u.%03u  ", 
			SysTime.wYear, SysTime.wMonth, SysTime.wDay, 
			SysTime.wHour, SysTime.wMinute, SysTime.wSecond, SysTime.wMilliseconds);
		return(m_szFormattedSystemTime);
	}
	void Log(CString strText, BOOL bStatus = FALSE, int iCodePage = 950);
	void SetListBoxData(CString strText, CABTPanelEx* clbList, int iCodePage);
	void LogDataToFile(CString strText, BOOL bStatus = FALSE);
	void IteratedLog(byte* bytData);
	void IteratedQuoteList(byte* bytData);
	void IteratedQuoteList(long lPage, short sFid,QuoteList& qlist);
	static void IteratedUpdateQuoteList(long lPageNo, FID_DEFINITION enumFieldId, long lIndex, COP_ITEM copMsg, CObject* pCallbackObject);
	void ResetStatisticContent();
	void ResetFilterArray(CObArray* tempFilterArray);
	void ResetPeakArray(CObArray* tempPeakArray);
	void UpdateList();
	void UpdateChannelList();
	void InitQuoteListCol(int iRow, int iCol);
	void InsertQuoteCell(int iRow, int iCol, CString strData, int iCodePage);

	BOOL LogOnePageData(long lPageNo, int iFidNo);
	void StopDataCtrl(UINT wParam, LONG lParam);
	void StopTCPCtrl(UINT wParam, LONG lParam);
	void ResumeStatus(UINT wParam, LONG lParam);
	void UpdateStatus();
	void CreateMsgObjcet(COP_ITEM& MsgParser );
	void ProcessStatisticData(CStatisticPage* pStatisticPage, int iSize, int iRecordType, int iPage, int enumFieldId =-1);
	void PutDataToDCache(COP_ITEM& copMsg);
	static void DataCtrlPutDataToDCache(COP_ITEM& copMsg, void* instance);
	static void TCPCtrlPutDataToDCache(COP_ITEM& copMsg, void* instance);
	static void MyCallBack(long lPageNo, FID_DEFINITION enumFIeldID, long lIndex, COP_ITEM copMsg, CObject *pCallbackObject) ;
	static void Send(long lPageNo, FID_DEFINITION enumFieldId, long lIndex, COP_ITEM copMsg, CObject* pCallbackObject);
	int IsInArray(CArray<int ,int>* CountArray, int iPage);
	int IsInArray(CObArray* CountArray, int iPage, int iFid);
	void CompareArray(CArray<int ,int>* CountArray, int iPage, int iValue, int iEnum);
	void CompareArray(CObArray* CountArray, int iPage, int iValue, int iEnum, int iFid);
	void ComparePeakArray(CObArray* CountArray, int iSize, int iTimes, int iValue, int iEnum);
	BOOL ComparePeakArrayValue(CPeakInfo* tempPeak, int iValue, int iEnum);

	BOOL CompareArrayValue(int iPage, int iValue, int iEnum);
	BOOL CompareArrayValue(CFilterStruct* tempFilter, int iValue, int iEnum);
	BOOL CheckDirectory(CString strBasePath, CString& subDir);
	enum 
	{
		TOP_TEN_PAGE_TOTAL_COUNT,
		TOP_TEN_PAGE_UPDATE_COUNT,
		TOP_TEN_PAGE_VS_COUNT,
		TOP_TEN_PAGE_UPDATE_SIZE,
		TOP_TEN_PAGE_VS_SIZE,
		TOP_TEN_PAGE_TOTAL_SIZE,
		TOP_TEN_PAGE_MAX_SIZE,

		TOP_TEN_FID_TOTAL_COUNT,
		TOP_TEN_FID_UPDATE_COUNT,
		TOP_TEN_FID_VS_COUNT,
		TOP_TEN_FID_UPDATE_SIZE,
		TOP_TEN_FID_VS_SIZE,
		TOP_TEN_FID_TOTAL_SIZE,
		TOP_TEN_FID_MAX_SIZE,

		TOP_TEN_PEAK_TOTAL_SIZE,
		TOP_TEN_PEAK_TOTAL_MESSAGES,
		TOP_TEN_PEAK_UPDATE_TOTAL_SIZE,
		TOP_TEN_PEAK_UPDATE_TOTAL_MESSAGES,
		TOP_TEN_PEAK_VS_TOTAL_SIZE,
		TOP_TEN_PEAK_VS_TOTAL_MESSAGES,
	};

	CArray<int ,int>	m_TopTenPageMaxSizeArray;
	CArray<int ,int>	m_TopTenPageTotalCountArray;
	CArray<int ,int>	m_TopTenPageUpdateCountArray;
	CArray<int ,int>	m_TopTenPageVSCountArray;
	CArray<int ,int>	m_TopTenPageUpdateSizeArray;
	CArray<int ,int>	m_TopTenPageVSSizeArray;
	CArray<int ,int>	m_TopTenPageTotalSizeArray;

	CObArray			m_TopTenFidTotalCountArray;
	CObArray			m_TopTenFidUpdateCountArray;
	CObArray			m_TopTenFidVSCountArray;
	CObArray			m_TopTenFidUpdateSizeArray;
	CObArray			m_TopTenFidVSSizeArray;
	CObArray			m_TopTenFidTotalSizeArray;
	CObArray			m_TopTenFidMaxSizeArray;

	CObArray			m_TopTenPeakTotalSizeArray;
	CObArray			m_TopTenPeakTotalMessagesArray;
	CObArray			m_TopTenPeakUpdateTotalSizeArray;
	CObArray			m_TopTenPeakUpdateTotalMessagesArray;
	CObArray			m_TopTenPeakVSTotalSizeArray;
	CObArray			m_TopTenPeakVSTotalMessagesArray;

	CStatisticPageMap m_mapPages;

// Dialog Data
	//{{AFX_DATA(CQuoteListToolsDlg)
	enum { IDD = IDD_QuoteListTools_DIALOG };
	CStatic	m_StatusOK2;
	CAnimateCtrl	m_StatusAniCtrl2;
	CComboBox	m_ChannelCombo;
	CIPAddressCtrl	m_ipLocalSend;
	CComboBox	m_GroupCombo;
	CButton	m_btnStatusListFrame;
	CButton	m_ShowMsgDetailButton;
	CIPAddressCtrl	m_ipRetrieve;
	CStatic	m_StatusOK;
	CAnimateCtrl	m_StatusAniCtrl;
	CEdit		m_PageCtrl;
	CABTPanelEx	m_Statuslist;
	CABTPanelEx	m_DCachelist;
	CABTPanelEx	m_QuoteList;
	CIPAddressCtrl	m_ipLocal;
	UINT	m_unPort;
	CString	m_sPageNo;
	CString	m_strFid;
	BOOL	m_bLogWithTime;
	BOOL	m_bLogStatisticData;
	UINT	m_unTCPPort;
	BOOL	m_bShowMsgDetail;
	BOOL	m_bShowMsgInfo;
	CString	m_strGroup;
	UINT	m_unSendPort;
	CString	m_strChannel;
	BOOL	m_bDisplayUpdateOnly;
	int		m_iListChoice;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQuoteListToolsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CQuoteListToolsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnStart();
	afx_msg void OnStop();
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	afx_msg void OnPutdatatocache();
	afx_msg void OnStatusDisplay();
	afx_msg void OnDcacheDisplay();
	afx_msg void OnOutputtofile();
	afx_msg void OnOutputtofile2();
	afx_msg void OnDcacheClear();
	afx_msg void OnStatusClear();
	afx_msg void OnSearch();
	afx_msg void OnLogFilter();
	afx_msg void OnDropPage();
	afx_msg void OnListAllPages();
	afx_msg void OnListAllDropPages();
	afx_msg void OnListAllFids();
	afx_msg void OnDblclkStatusList(NMHDR* pNMHDR, LRESULT* pResult);	
	afx_msg void OnDblclkDcacheList(NMHDR* pNMHDR, LRESULT* pResult);	
	afx_msg void OnTestDrop();
	afx_msg void OnTest();
	afx_msg void OnListAllDropFid();
	afx_msg void OnSaveData();
	afx_msg void OnRetrieveData();
	afx_msg void OnShowStat();
	afx_msg void OnTimecheck();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnStatisticCheck();
	afx_msg void OnRetrieve();
	afx_msg void OnMsgDetailCheck();
	afx_msg void OnShowInfoCheck();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLoadSetting();
	afx_msg void OnSaveSetting();
	afx_msg void OnSendStart();
	afx_msg void OnSendStop();
	afx_msg void OnAddChannel();
	afx_msg void OnRemoveChannel();
	afx_msg void OnSendMessage();
	afx_msg void OnRetrieveStop();
	afx_msg void OnAutoLog2();
	afx_msg void OnTcpDcache();
	afx_msg void OnSocketServer();
	afx_msg void OnExit();
	virtual void OnCancel();
	afx_msg void OnClose();
	afx_msg void OnRemoveAll();
	afx_msg void OnShowquotelist();
	afx_msg void OnExportData();
	afx_msg void OnSelchangeListchoiceCombo();
	afx_msg void OnRegister();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QuoteListToolsDLG_H__6D577567_D73F_11D5_9CA0_0006294E7FE4__INCLUDED_)
