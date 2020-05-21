// StatisticDlg.cpp : implementation file
//

#include "stdafx.h"
#include "QuoteListTools.h"
#include "StatisticDlg.h"
#include "QuoteListToolsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStatisticDlg dialog


CStatisticDlg::CStatisticDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStatisticDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStatisticDlg)
	m_strPageNo = _T("");
	//}}AFX_DATA_INIT
}


void CStatisticDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStatisticDlg)
	DDX_Control(pDX, IDC_SELECTION_TEXT, m_SelectionTextCtrl);
	DDX_Control(pDX, IDC_TAB, m_TabCtrl);
	DDX_Control(pDX, IDC_LIST2, m_PageListCtrl);
	DDX_Control(pDX, IDC_PAGE_STATIC, m_PageStaticCtrl);
	DDX_Control(pDX, IDC_RETREIVE_BUTTON, m_RetrieveCtrl);
	DDX_Control(pDX, IDC_PAGE_EDIT, m_PageCtrl);
	DDX_Control(pDX, IDC_COMBO1, m_ChoiceCtrl);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
	DDX_Text(pDX, IDC_PAGE_EDIT, m_strPageNo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStatisticDlg, CDialog)
	//{{AFX_MSG_MAP(CStatisticDlg)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, OnSelchangeTab)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
	ON_BN_CLICKED(IDC_RETREIVE_BUTTON, OnRetreiveButton)
	ON_BN_CLICKED(IDC_RESET_BUTTON, OnResetAll)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_REFRESH_BUTTON, OnRefresh)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatisticDlg message handlers

BOOL CStatisticDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	LPTSTR lpszHeadings1[] = {
		_T("Counter"),
		_T("Values")		
	};

	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;
	lvc.fmt = LVCFMT_LEFT;
	for (int nColumn = 0; nColumn < 2; nColumn++) {
		lvc.iSubItem = nColumn;
		lvc.pszText = lpszHeadings1[nColumn];
		lvc.cx = ((nColumn == 0)||((nColumn >4)&&(nColumn<7))? 340 : 340);
		m_PageListCtrl.InsertColumn(nColumn, &lvc);
	}
	m_PageListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_SHOWSELALWAYS);
	m_PageListCtrl.SetItemState(0,2, LVIS_SELECTED);

	m_ChoiceCtrl.InsertString(0, "Top Ranking by Total Size Peak (All)");
	m_ChoiceCtrl.InsertString(1, "Top Ranking by Total Messages Peak (All)");
	m_ChoiceCtrl.InsertString(2, "Top Ranking by Total Size Peak (Update)");
	m_ChoiceCtrl.InsertString(3, "Top Ranking by Total Messages Peak (Update)");
	m_ChoiceCtrl.InsertString(4, "Top Ranking by Total Size Peak (VSync)");
	m_ChoiceCtrl.InsertString(5, "Top Ranking by Total Messages Peak (VSync)");

	m_ChoiceCtrl.InsertString(6, "Top Ranking by Total Size of Page Received (All)");
	m_ChoiceCtrl.InsertString(7, "Top Ranking by Total Times of Page Received (All)");
	m_ChoiceCtrl.InsertString(8, "Top Ranking by Total Size of Page Received (Update)");
	m_ChoiceCtrl.InsertString(9, "Top Ranking by Total Times of Page Received (Update)");
	m_ChoiceCtrl.InsertString(10, "Top Ranking by Total Size of Page Received (VSync)");
	m_ChoiceCtrl.InsertString(11, "Top Ranking by Total Times of Page Received (VSync)");
	m_ChoiceCtrl.InsertString(12, "Top Ranking by Max Sizes in one message Received (Page)");

	m_ChoiceCtrl.InsertString(13, "Top Ranking by Total Size of Fid Received (All)");
	m_ChoiceCtrl.InsertString(14, "Top Ranking by Total Times of Fid Received (All)");
	m_ChoiceCtrl.InsertString(15, "Top Ranking by Total Size of Fid Received (Update)");
	m_ChoiceCtrl.InsertString(16, "Top Ranking by Total Times of Fid Received (Update)");
	m_ChoiceCtrl.InsertString(17, "Top Ranking by Total Size of Fid Received (VSync)");
	m_ChoiceCtrl.InsertString(18, "Top Ranking by Total Times of Fid Received (VSync)");
	m_ChoiceCtrl.InsertString(19, "Top Ranking by Max Sizes in one message Received (Fid)");

	m_ChoiceCtrl.SetFocus();
	m_ChoiceCtrl.SetCurSel(0);
	OnSelchangeCombo1();

	TC_ITEM tcItem;
	tcItem.mask=TCIF_TEXT;
	tcItem.cchTextMax=0;
	tcItem.pszText=_T("General Information");
	m_TabCtrl.InsertItem(0, &tcItem);
	tcItem.pszText=_T("Page Information");
	m_TabCtrl.InsertItem(1, &tcItem);

	m_ChoiceCtrl.ShowWindow(SW_SHOW);
	m_ListCtrl.ShowWindow(SW_SHOW);

	m_PageCtrl.ShowWindow(SW_HIDE);
	m_PageListCtrl.ShowWindow(SW_HIDE);
	m_RetrieveCtrl.ShowWindow(SW_HIDE);
	m_PageStaticCtrl.ShowWindow(SW_HIDE);
  	m_iTimer = SetTimer( 1, 5000, NULL );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
//---------------------------------------------------------------------------------

void CStatisticDlg::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int index=m_TabCtrl.GetCurSel();
	if (index==0)
	{
		m_ChoiceCtrl.ShowWindow(SW_SHOW);
		m_ListCtrl.ShowWindow(SW_SHOW);
		m_SelectionTextCtrl.ShowWindow(SW_SHOW);

		m_PageCtrl.ShowWindow(SW_HIDE);
		m_PageListCtrl.ShowWindow(SW_HIDE);
		m_RetrieveCtrl.ShowWindow(SW_HIDE);
		m_PageStaticCtrl.ShowWindow(SW_HIDE);
	}
	else
	{
		m_ChoiceCtrl.ShowWindow(SW_HIDE);
		m_ListCtrl.ShowWindow(SW_HIDE);
		m_SelectionTextCtrl.ShowWindow(SW_HIDE);

		m_PageCtrl.ShowWindow(SW_SHOW);
		m_PageListCtrl.ShowWindow(SW_SHOW);
		m_RetrieveCtrl.ShowWindow(SW_SHOW);
		m_PageStaticCtrl.ShowWindow(SW_SHOW);
	}
	
	*pResult = 0;
}
//---------------------------------------------------------------------------------

void CStatisticDlg::OnSelchangeCombo1() 
{
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;
	lvc.fmt = LVCFMT_LEFT;

	int nColumnCount = m_ListCtrl.GetHeaderCtrl()->GetItemCount();
	// Delete all of the columns.
	for (int i=0;i < nColumnCount;i++)
	   m_ListCtrl.DeleteColumn(0);
	int iCurSel = m_ChoiceCtrl.GetCurSel();
	CArray<int, int>*	tempArray = NULL;
	CObArray*			tempArray2 = NULL;

	switch (iCurSel)
	{
		case 0: //Top Ranking by Total Size Peak (All)
		case 1: //Top Ranking by Total Messages Peak (All)
		case 2: //Top Ranking by Total Size Peak (Update)
		case 3: //Top Ranking by Total Messages Peak (Update)
		case 4: //Top Ranking by Total Size Peak (VSync)
		case 5: //Top Ranking by Total Messages Peak (VSync)
			{
				if (iCurSel == 0)
					tempArray2 = &m_QuoteListToolsDlg->m_TopTenPeakTotalSizeArray;
				else if (iCurSel == 1)
					tempArray2 = &m_QuoteListToolsDlg->m_TopTenPeakTotalMessagesArray;
				else if (iCurSel == 2)
					tempArray2 = &m_QuoteListToolsDlg->m_TopTenPeakUpdateTotalSizeArray;
				else if (iCurSel == 3)
					tempArray2 = &m_QuoteListToolsDlg->m_TopTenPeakUpdateTotalMessagesArray;
				else if (iCurSel == 4)
					tempArray2 = &m_QuoteListToolsDlg->m_TopTenPeakVSTotalSizeArray;
				else 
					tempArray2 = &m_QuoteListToolsDlg->m_TopTenPeakVSTotalMessagesArray;


				LPTSTR lpszHeadings[] = {
					_T("Rank"),
					_T("Messages/Sec"),
					_T("Bytes/Sec"),
					_T("Time"),
				};
				m_ListCtrl.DeleteAllItems();
				for (int nColumn = 0; nColumn < 4; nColumn++) {
					lvc.iSubItem = nColumn;
					lvc.pszText = lpszHeadings[nColumn];
					lvc.cx = ((nColumn == 0)? 40 : 160);
					m_ListCtrl.InsertColumn(nColumn, &lvc);
				}
 
				CString strTemp;
				for (int iIndex=0; iIndex<=tempArray2->GetUpperBound();iIndex++)
				{
					CPeakInfo * tempPeak = (CPeakInfo*) tempArray2->GetAt(iIndex);
					InsertTopRankingItem(iIndex, iIndex+1);
					strTemp.Format("%d", tempPeak->iTimes);
					InsertTopRankingItem(iIndex, tempPeak->iTimes, 1, TRUE, strTemp);
					InsertTopRankingItem(iIndex, tempPeak->iSize, 2);
					strTemp = tempPeak->thatTime.Format("%H:%M:%S, %B %d, %Y");
					InsertTopRankingItem(iIndex, 0, 3, TRUE, strTemp);
				}
			}
			break;

		case 6:	//	Top Ranking by Total Size of Page Received
		case 8:	//	Top Ranking by Total Size of Page Received (Update)
		case 10://  Top Ranking by Total Size of Page Received (VSync)
		case 12://  Top Ranking by Max Sizes in one message Received 
			{
				if (iCurSel == 6)
					tempArray = &m_QuoteListToolsDlg->m_TopTenPageTotalSizeArray;
				else if (iCurSel == 8)
					tempArray = &m_QuoteListToolsDlg->m_TopTenPageUpdateSizeArray;
				else if (iCurSel == 10)
					tempArray = &m_QuoteListToolsDlg->m_TopTenPageVSSizeArray;
				else
					tempArray = &m_QuoteListToolsDlg->m_TopTenPageMaxSizeArray;

				LPTSTR lpszHeadings[] = {
					_T("Rank"),
					_T("Page"),
					_T("Total Size"),
					_T("Update Size"),
					_T("VSync Size"),
					_T("Max Size"),				
				};
				m_ListCtrl.DeleteAllItems();
				for (int nColumn = 0; nColumn < 6; nColumn++) {
					lvc.iSubItem = nColumn;
					lvc.pszText = lpszHeadings[nColumn];
					lvc.cx = ((nColumn == 0)? 40 : 130);
					m_ListCtrl.InsertColumn(nColumn, &lvc);
				}

				for (int iIndex=0; iIndex<=tempArray->GetUpperBound();iIndex++)
				{
					int iPage = tempArray->GetAt(iIndex);
					CStatisticPage* pStatisticPage = NULL; 
					if (m_QuoteListToolsDlg->m_mapPages.Lookup(iPage, (CStatisticPage*)pStatisticPage ))
					{
						InsertTopRankingItem(iIndex, iIndex+1);
						InsertTopRankingItem(iIndex, iPage, 1);
						InsertTopRankingItem(iIndex, pStatisticPage->StatisticData.iTotalSize, 2);
						InsertTopRankingItem(iIndex, pStatisticPage->StatisticData.iUpdateSize, 3);
						InsertTopRankingItem(iIndex, pStatisticPage->StatisticData.iVSSize, 4);
						InsertTopRankingItem(iIndex, pStatisticPage->StatisticData.iMaxSize, 5);
					}
				}
			}
			break;
		case 7:	// Top Ranking by Total Times of Page Received
		case 9: // Top Ranking by Total Times of Page Received (Update)
		case 11:// Top Ranking by Total Times of Page Received (VSync)
			{
				if (iCurSel == 7)
					tempArray = &m_QuoteListToolsDlg->m_TopTenPageTotalCountArray;
				else if (iCurSel == 9)
					tempArray = &m_QuoteListToolsDlg->m_TopTenPageUpdateCountArray;
				else if (iCurSel == 11)
					tempArray = &m_QuoteListToolsDlg->m_TopTenPageVSCountArray;

				LPTSTR lpszHeadings[] = {
					_T("Rank"),
					_T("Page"),
					_T("Total No of Count"),
					_T("No of Update Count"),
					_T("No of VSync Count"),
				};

				m_ListCtrl.DeleteAllItems();
				for (int nColumn = 0; nColumn < 5; nColumn++) {
					lvc.iSubItem = nColumn;
					lvc.pszText = lpszHeadings[nColumn];
					lvc.cx = ((nColumn == 0)? 40 : 160);
					m_ListCtrl.InsertColumn(nColumn, &lvc);
				}

				for (int iIndex=0; iIndex<=tempArray->GetUpperBound();iIndex++)
				{
					int iPage = tempArray->GetAt(iIndex);
					CStatisticPage* pStatisticPage = NULL; 
					if (m_QuoteListToolsDlg->m_mapPages.Lookup(iPage, (CStatisticPage*)pStatisticPage ))
					{
						InsertTopRankingItem(iIndex, iIndex+1);
						InsertTopRankingItem(iIndex, iPage, 1);
						InsertTopRankingItem(iIndex, pStatisticPage->StatisticData.iTotalTimes, 2);
						InsertTopRankingItem(iIndex, pStatisticPage->StatisticData.iUpdateTimes, 3);
						InsertTopRankingItem(iIndex, pStatisticPage->StatisticData.iVSTimes, 4);
					}
				}
			}
			break;
		case 13: // Top Ranking by Total Size of Fid Received
		case 15: // Top Ranking by Total Size of Fid Received (Update)
		case 17: // Top Ranking by Total Size of Fid Received (VSync)
		case 19: // Top Ranking by Max Sizes in one message Received (Fid)
			{
				if (iCurSel == 13)
					tempArray2 = &m_QuoteListToolsDlg->m_TopTenFidTotalSizeArray;
				else if (iCurSel == 15)
					tempArray2 = &m_QuoteListToolsDlg->m_TopTenFidUpdateSizeArray;
				else if (iCurSel == 17)
					tempArray2 = &m_QuoteListToolsDlg->m_TopTenFidVSSizeArray;
				else
					tempArray2 = &m_QuoteListToolsDlg->m_TopTenFidMaxSizeArray;

				LPTSTR lpszHeadings[] = {
					_T("Rank"),
					_T("Page"),
					_T("Fid"),
					_T("Total Size"),
					_T("Update Size"),
					_T("VSync Size"),
					_T("Max Size"),				
				};
				m_ListCtrl.DeleteAllItems();
				for (int nColumn = 0; nColumn < 7; nColumn++) {
					lvc.iSubItem = nColumn;
					lvc.pszText = lpszHeadings[nColumn];
					lvc.cx = (((nColumn == 0)||(nColumn == 2))? 40 : 130);
					m_ListCtrl.InsertColumn(nColumn, &lvc);
				}

				for (int iIndex=0; iIndex<=tempArray2->GetUpperBound();iIndex++)
				{
					CFilterStruct* tempFilter = (CFilterStruct*) tempArray2->GetAt(iIndex);
					CStatisticPage* pStatisticPage = NULL; 
					if (m_QuoteListToolsDlg->m_mapPages.Lookup(tempFilter->lPageNo, (CStatisticPage*)pStatisticPage ))
					{
						CStatisticStruct* StatisticObject = NULL;
						if (pStatisticPage->m_mapFields.Lookup(tempFilter->FieldID, (CStatisticStruct*&)StatisticObject ))
						{
							InsertTopRankingItem(iIndex, iIndex+1);
							InsertTopRankingItem(iIndex, tempFilter->lPageNo, 1);
							InsertTopRankingItem(iIndex, tempFilter->FieldID, 2);
							InsertTopRankingItem(iIndex, StatisticObject->iTotalSize, 3);
							InsertTopRankingItem(iIndex, StatisticObject->iUpdateSize, 4);
							InsertTopRankingItem(iIndex, StatisticObject->iVSSize, 5);
							InsertTopRankingItem(iIndex, StatisticObject->iMaxSize, 6);
						}
					}
				}
			}
			break;
		case 14:	// Top Ranking by Total Times of Fid Received
		case 16:	// Top Ranking by Total Times of Fid Received (Update)
		case 18:	// Top Ranking by Total Times of Fid Received (VSync)
			{
				if (iCurSel == 14)
					tempArray2 = &m_QuoteListToolsDlg->m_TopTenFidTotalCountArray;
				else if (iCurSel == 16)
					tempArray2 = &m_QuoteListToolsDlg->m_TopTenFidUpdateCountArray;
				else if (iCurSel == 18)
					tempArray2 = &m_QuoteListToolsDlg->m_TopTenFidVSCountArray;

				LPTSTR lpszHeadings[] = {
					_T("Rank"),
					_T("Page"),
					_T("Fid"),
					_T("Total No of Count"),
					_T("No of Update Count"),
					_T("No of VSync Count"),
				};

				m_ListCtrl.DeleteAllItems();
				for (int nColumn = 0; nColumn < 6; nColumn++) {
					lvc.iSubItem = nColumn;
					lvc.pszText = lpszHeadings[nColumn];
					lvc.cx = (((nColumn == 0)||(nColumn == 2))? 40 : 130);
					m_ListCtrl.InsertColumn(nColumn, &lvc);
				}

				for (int iIndex=0; iIndex<=tempArray2->GetUpperBound();iIndex++)
				{
					CFilterStruct* tempFilter = (CFilterStruct*) tempArray2->GetAt(iIndex);
					CStatisticPage* pStatisticPage = NULL; 
					if (m_QuoteListToolsDlg->m_mapPages.Lookup(tempFilter->lPageNo, (CStatisticPage*)pStatisticPage ))
					{
						CStatisticStruct* StatisticObject = NULL;
						if (pStatisticPage->m_mapFields.Lookup(tempFilter->FieldID, (CStatisticStruct*&)StatisticObject ))
						{
							InsertTopRankingItem(iIndex, iIndex+1);
							InsertTopRankingItem(iIndex, tempFilter->lPageNo, 1);
							InsertTopRankingItem(iIndex, tempFilter->FieldID, 2);
							InsertTopRankingItem(iIndex, StatisticObject->iTotalTimes, 3);
							InsertTopRankingItem(iIndex, StatisticObject->iUpdateTimes, 4);
							InsertTopRankingItem(iIndex, StatisticObject->iVSTimes, 5);
						}
					}
				}
			}
			break;



		default:
			break;
	}
	CString strOutput;
	m_ChoiceCtrl.GetLBText(iCurSel, strOutput);
	SetDlgItemText(IDC_SELECTION_TEXT, strOutput);
	
	m_ListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_SHOWSELALWAYS);
	m_ListCtrl.SetItemState(0,2, LVIS_SELECTED);
}
//---------------------------------------------------------------------------------

void CStatisticDlg::OnRetreiveButton() 
{
	UpdateData(TRUE);
	long lPageNo = 0;
	if (m_strPageNo.GetLength() > 1)
	{
		if (m_strPageNo.Left(2) == "0x")
			sscanf(m_strPageNo, "%x", &lPageNo);
		else
			sscanf(m_strPageNo, "%d", &lPageNo);

	}
	else
		lPageNo = atoi(m_strPageNo);
	CStatisticPage* pStatisticPage = NULL; 
	int nItem = 0;
	if (m_QuoteListToolsDlg->m_mapPages.Lookup(lPageNo, (CStatisticPage*)pStatisticPage ))
	{
		m_PageListCtrl.DeleteAllItems();
		InsertPageItem(nItem++, "Updates Times", pStatisticPage->StatisticData.iUpdateTimes);
		InsertPageItem(nItem++, "Updates Size", pStatisticPage->StatisticData.iUpdateSize);
		InsertPageItem(nItem++, "VSync Times", pStatisticPage->StatisticData.iVSTimes);
		InsertPageItem(nItem++, "VSync Size", pStatisticPage->StatisticData.iVSSize);
		InsertPageItem(nItem++, "Max Size", pStatisticPage->StatisticData.iMaxSize);
	}
	else
	{
		m_PageListCtrl.DeleteAllItems();
		LV_ITEM lvi;
		lvi.mask = LVIF_TEXT ;
		lvi.iItem = 0;
		lvi.iSubItem = 0;
		lvi.pszText = "No data in this Page";
		m_PageListCtrl.InsertItem(&lvi);
	}
	m_PageCtrl.SetSel( MAKELONG(0,-1) ) ;
	m_PageCtrl.SetFocus();

}
//---------------------------------------------------------------------------------
void CStatisticDlg::InsertPageItem(int iRow, CString strCounterName, int iValue) 
{
	char szBuffer[100];
	LV_ITEM lvi;
	lvi.mask = LVIF_TEXT ;
	lvi.iItem = iRow;
	lvi.iSubItem = 0;
	lvi.pszText = strCounterName.GetBuffer(0);
	m_PageListCtrl.InsertItem(&lvi);

	sprintf(szBuffer, "%d", iValue);
	lvi.mask = LVIF_TEXT;
	lvi.iSubItem = 1;
	lvi.pszText = szBuffer;
	m_PageListCtrl.SetItem(&lvi);

}

//---------------------------------------------------------------------------------
void CStatisticDlg::InsertTopRankingItem(int iRow, int iValue, int iCol, BOOL bString, CString strTemp) 
{
	char szBuffer[100];
	LV_ITEM lvi;
	if (!bString)
	{
		if (iCol!=1)
			sprintf(szBuffer, "%d", iValue);
		else
			sprintf(szBuffer, "%d (0x%x)", iValue, iValue);
	}
	else
		sprintf(szBuffer, "%s", strTemp.GetBuffer(0));
	if (iCol == 0)
	{
		lvi.mask = LVIF_TEXT ;
		lvi.iItem = iRow;
		lvi.iSubItem = iCol;
		lvi.pszText = szBuffer;
		m_ListCtrl.InsertItem(&lvi);
	}
	else
	{
		lvi.iItem = iRow;
		lvi.mask = LVIF_TEXT;
		lvi.iSubItem = iCol;
		lvi.pszText = szBuffer;
		m_ListCtrl.SetItem(&lvi);
	}

}

//---------------------------------------------------------------------------------

void CStatisticDlg::OnResetAll() 
{
	if (AfxMessageBox("Reset Statistic Data need to stop DataCtrl. Would you like to proceed?", MB_OKCANCEL) == IDOK)
	{
		m_QuoteListToolsDlg->m_DataCtrl.Stop();
		m_QuoteListToolsDlg->ResetStatisticContent();		
	}
}
//---------------------------------------------------------------------------------

void CStatisticDlg::OnTimer(UINT nIDEvent) 
{
	int index=m_TabCtrl.GetCurSel();
	if (index==0)
		OnSelchangeCombo1();
	CDialog::OnTimer(nIDEvent);
}
//---------------------------------------------------------------------------------

void CStatisticDlg::OnRefresh() 
{
	OnSelchangeCombo1();
}
