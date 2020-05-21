// SendMessageDialog.cpp : implementation file
//

#include "stdafx.h"
#include "QuoteListTools.h"
#include "QuoteListToolsDlg.h"
#include "SendMessageDialog.h"
#include "ssCharacterSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSendMessageDialog dialog


CSendMessageDialog::CSendMessageDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSendMessageDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSendMessageDialog)
	m_iSendInterval = 0;
	m_iFidNo = 0;
	m_strFidValue = _T("");
	m_iRecType = 0;
	m_strPage = _T("");
	m_strMsgType = _T("");
	m_strPage2 = _T("");
	m_bRepeat = FALSE;
	//}}AFX_DATA_INIT
}


void CSendMessageDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendMessageDialog)
	DDX_Control(pDX, IDC_RECTYPE, m_ctrlRecType);
	DDX_Control(pDX, IDC_PAGE2, m_ctrlPage2);
	DDX_Text(pDX, IDC_SEND_INTERVAL, m_iSendInterval);
	DDX_Text(pDX, IDC_FIDNO, m_iFidNo);
	DDX_Text(pDX, IDC_FIDVALUE, m_strFidValue);
	DDX_Text(pDX, IDC_RECTYPE, m_iRecType);
	DDX_Text(pDX, IDC_PAGE, m_strPage);
	DDX_CBString(pDX, IDC_MSGTYPE, m_strMsgType);
	DDX_Text(pDX, IDC_PAGE2, m_strPage2);
	DDX_Check(pDX, IDC_REPEAT, m_bRepeat);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSendMessageDialog, CDialog)
	//{{AFX_MSG_MAP(CSendMessageDialog)
	ON_BN_CLICKED(IDC_SEND, OnSend)
	ON_BN_CLICKED(IDC_EXIT, OnExit)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_REPEAT, OnRepeat)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSendMessageDialog message handlers

void CSendMessageDialog::OnSend() 
{
	UpdateData();
	byte bytMsgType = 0;
	BOOL bPageNoExist = FALSE;
	BOOL bPageNo2Exist = FALSE;

	if (m_strMsgType == "MGT_LOGONREQUEST") 
		bytMsgType = 	1; 		
	else if (m_strMsgType == "MGT_LOGONRESPONSE") 
		bytMsgType = 	2; 		
	else if (m_strMsgType == "MGT_ADDWATCH") 
		bytMsgType = 	3; 	
	else if (m_strMsgType == "MGT_DELETEWATCH") 
		bytMsgType = 	4; 		
	else if (m_strMsgType == "MGT_SNAPSHOT") 
		bytMsgType = 	5; 		
	else if (m_strMsgType == "MGT_DATAPERMISSION") 
		bytMsgType = 	6; 		
	else if (m_strMsgType == "MGT_FORCEUPDATE") 
		bytMsgType = 	8; 		
	else if (m_strMsgType == "MGT_UPDATE") 
		bytMsgType = 	9; 		
	else if (m_strMsgType == "MGT_VERIFYSYNC") 
		bytMsgType = 	10;		
	else if (m_strMsgType == "MGT_CLOSINGRUN") 
		bytMsgType = 	11;		
	else if (m_strMsgType == "MGT_DROP") 
		bytMsgType = 	12;		
	else if (m_strMsgType == "MGT_INTRADAYREBUILD") 
		bytMsgType = 	13;		
	else if (m_strMsgType == "MGT_INTRADAYCORRECTION") 
		bytMsgType = 	14;		
	else if (m_strMsgType == "MGT_SERVERSTATUS") 
		bytMsgType = 	15;		
	else if (m_strMsgType == "MGT_ALIVE") 
		bytMsgType = 	17;		
	else if (m_strMsgType == "MGT_RAISEHANDREQUEST") 
		bytMsgType = 	18;		
	else if (m_strMsgType == "MGT_RAISEHANDREPLY") 
		bytMsgType = 	19;		
	else if (m_strMsgType == "MGT_SYSTEMMESSAGE") 
		bytMsgType = 	20;		
	else
	{
		AfxMessageBox("Please select the Msg Type first and then send again");
		return;
	}
	if (m_strMsgType != "MGT_DROP") 
	{
		if ((m_strPage.GetLength()==0) && (m_iRecType==-1))
		{
			AfxMessageBox("Please enter the Page No or RecType and then send again");
			return;
		}
		fidDatabase fDB;

		if (m_strFidValue.GetLength()==0)
		{
			AfxMessageBox("Please enter fid value and then send again.");
			return;
		}
		switch (fDB.GetFidType(m_iFidNo))
		{
			case NGN_CHAR:
			case NGN_SHORT:
			case NGN_UNICODE:
			case NGN_USHORT:
			case NGN_LONG:
			case NGN_ULONG:
			case NGN_FLOAT:
			case NGN_UFLOAT:
			case NGN_DOUBLE:
			case NGN_UDOUBLE:
			case NGN_FIELDLIST:
			case NGN_KMULTIPLE:
			case NGN_10KMULTIPLE:
			case NGN_CHARARRAY:
			case NGN_LINK:
			case NGN_WLINK:
				break;
			case NGN_MCHARARRAY:
			case NGN_BDATEYEAR:
			case NGN_BDATEMONTH:
			case NGN_BTIMEMIN:
			case NGN_BTIMESEC:
			case NGN_BTIMERANGES:
			case NGN_TRAN:
			case NGN_SHRORDLST:
			case NGN_PRCVOL:
			case NGN_BKRQUE:
			case NGN_YLABEL:
			case NGN_XMTCLABEL:
			case NGN_XDTCLABEL:
			case NGN_MTCPOINTS:
			case NGN_MTCVAL:
			case NGN_DTCPOINTS:
			case NGN_HISVAL:
			case NGN_FIELDLABEL:
			case NGN_INFOMSG:
			case NGN_HLINK:
			case NGN_SPRTBL:
			case NGN_MTCTICKS:
			case NGN_MTCV:
			case NGN_OBKRQUE:
			case NGN_HEADLINE:
			case NGN_YHEADLINE:
			case NGN_QUOTELIST:
			case NGN_BQUEVOSN:
			case NGN_MTCPTS:
			case NGN_MTCF:
			case NGN_PRCVOLWSEQ:
				AfxMessageBox("This fid type cannot support to send in this version.");
				return;
			default:
				AfxMessageBox("Please enter the valid Fid No.");
				return;
				break;
		}
	}

	if (m_iSendInterval<0)
	{
		AfxMessageBox("Please enter send Interval > 0 and then send again");
		return;
	}
	long lPageNo = 0 ;
	long lPageNo2 = 0 ;	
	if (m_strPage.GetLength() > 1)
	{
		if ((m_strPage.Left(2) == "0x")||(m_strPage.Left(2) == "0X"))
			sscanf(m_strPage, "%x", &lPageNo);
		else
			sscanf(m_strPage, "%d", &lPageNo);
	}
	else
		lPageNo = atoi(m_strPage);

	if (m_strPage2.GetLength() > 1)
	{
		if ((m_strPage2.Left(2) == "0x")||(m_strPage2.Left(2) == "0X"))
			sscanf(m_strPage2, "%x", &lPageNo2);
		else
			sscanf(m_strPage2, "%d", &lPageNo2);
	}
	else
		lPageNo2 = atoi(m_strPage2);
	CString strQuestion, strTemp;
	strQuestion.Format("Are you sure you want to send the message?\n\n");
	if (m_strPage.GetLength()>0)
	{
		bPageNoExist = TRUE;
		strTemp.Format("Page: %s ", m_strPage.GetBuffer(0));
		strQuestion+=strTemp;
		if (m_strPage2.GetLength()>0)
		{
			strTemp.Format("- %s ", m_strPage2.GetBuffer(0));
			strQuestion+=strTemp;
			bPageNo2Exist = TRUE;
		}
		strQuestion+="\n";
	}
	
	if (m_iRecType != -1)
	{
		strTemp.Format("Rec Type: %d\n", m_iRecType);
		strQuestion+=strTemp;
	}

	strTemp.Format("Fid No: %d, MsgType: %d, Value: %s to Channel %s - Port:%d with interval: %d", 
					m_iFidNo, bytMsgType, m_strFidValue, m_QuoteListToolsDlg->m_strChannel, m_QuoteListToolsDlg->m_unSendPort, m_iSendInterval);
	strQuestion+=strTemp;

	int result = AfxMessageBox( strQuestion, MB_ICONQUESTION + MB_YESNO + MB_DEFBUTTON2, NULL);
	if (result == IDYES)
	{
		int iFromPage = lPageNo;
		int iToPage = lPageNo2;
		if (lPageNo>lPageNo2)
		{
			iFromPage	= lPageNo2;
			iToPage		= lPageNo;
		}

		if (m_iRecType == -1)
		{	
			if (!bPageNo2Exist)
			{
				AfxMessageBox( "Repeat to send one message");
				SendOneMessage(lPageNo, bytMsgType);
                m_unTimer = SetTimer( 1, m_iSendInterval, 0 );
			}
			else
				for (int iPageCount = iFromPage; iPageCount <= iToPage; iPageCount ++)
				{
					SendOneMessage(iPageCount, bytMsgType);
					Sleep(m_iSendInterval);
				}

		}
		else
		{
			if ((bPageNoExist) && (!bPageNo2Exist))
				 iFromPage = iToPage;

			CDWordArray	cdwaPageList;
			if (m_QuoteListToolsDlg->m_TestDCache->GetPageList(cdwaPageList))
			{
				do 
				{
					if (cdwaPageList.GetSize()>0)
					{
						long lCurPageNo = cdwaPageList.GetAt(0);
						cdwaPageList.RemoveAt(0);
						unsigned char ucRecType;
						long lIndex = 0 ;
						if ((!bPageNoExist) || ((bPageNoExist) && (iToPage>=lCurPageNo) && (iFromPage<=lCurPageNo)))
							if (m_QuoteListToolsDlg->m_TestDCache->GetAField(lCurPageNo, ucRecType, FID_REC_TYPE, lIndex))
								if (ucRecType == m_iRecType)
								{
									SendOneMessage(lCurPageNo, bytMsgType) ;
									Sleep(m_iSendInterval);
								}
					}
				} while (cdwaPageList.GetSize()>0);
			}
		}
	}
}
// -------------------------------------------------------------------------

void CSendMessageDialog::SendOneMessage(long lPageNo, byte bytMsgType) 
{
	CString strOutput;
	COP_ITEM pMsgObj;
	MsgObject* MsgObj = new MsgObject();

	fidDatabase fDB;
	switch (fDB.GetFidType(m_iFidNo))
	{
		case NGN_CHAR:
			{
				unsigned char ch;
				ch = atoi(m_strFidValue.GetBuffer(0));
				MsgObj->AddFid(m_iFidNo, &ch);
			}
			break;
		case NGN_SHORT:
			{
				short sh;
				sh = atoi(m_strFidValue.GetBuffer(0));
				MsgObj->AddFid(m_iFidNo, &sh);
			}
			break;
		case NGN_UNICODE:
		case NGN_USHORT:
			{
				unsigned short ush;
				ush = atoi(m_strFidValue.GetBuffer(0));
				MsgObj->AddFid(m_iFidNo, &ush);
			}
			break;
		case NGN_LONG:
		case NGN_KMULTIPLE:
		case NGN_10KMULTIPLE:
			{
				long l;
				l = atol(m_strFidValue.GetBuffer(0));
				MsgObj->AddFid(m_iFidNo, &l);
			}
			break;
		case NGN_ULONG:
			{
				unsigned long ul;
				ul = atol(m_strFidValue.GetBuffer(0));
				MsgObj->AddFid(m_iFidNo, &ul);
			}
			break;
		case NGN_FLOAT:
		case NGN_UFLOAT:
			{
				float f;
				f = atof(m_strFidValue.GetBuffer(0));
				MsgObj->AddFid(m_iFidNo, &f);
			}
			break;
		case NGN_DOUBLE:
		case NGN_UDOUBLE:
			{
				double d;
				d = atof(m_strFidValue.GetBuffer(0));
				MsgObj->AddFid(m_iFidNo, &d);
			}
			break;
		case NGN_LINK:
		case NGN_WLINK:
			{
				unsigned long usl = atol(m_strFidValue.GetBuffer(0));
				Link link;
				link.AddLink(usl);
				link.SetTL(1);
				link.SetPos(0);
				MsgObj->AddFid(m_iFidNo, &link);
			}
			break;
		case NGN_FIELDLIST:
			{
				unsigned short ush = atoi(m_strFidValue.GetBuffer(0));
				FieldList fldlst;
				fldlst.AddField(ush);
				MsgObj->AddFid(m_iFidNo, &fldlst);
			}
			break;
		case NGN_CHARARRAY:
			{
				CharArray CA;
				char big5[256];
				char utf8[256];
				sprintf(big5, "%s", m_strFidValue.GetBuffer(0));
				ssCharacterSet::Big5ToUTF8(big5, utf8, (strlen(big5)*2));
				CA.SetSq(1);
				CA.SetPos(255);
				CA.SetLen(strlen(big5)*2+1);
				CA.SetString((byte*)utf8);
				MsgObj->AddFid(m_iFidNo, &CA);
			}
			break;
		default:
			break;
	}
	

	MsgObj->SetMsgType(bytMsgType);	
	MsgObj->SetItemNo(lPageNo);
	strOutput.Format("Sent Page: %d, Fid: %d, MsgType: %d, Value: %s,  to Channel %s - Port:%d", 
					  lPageNo, m_iFidNo, bytMsgType, m_strFidValue, m_QuoteListToolsDlg->m_strChannel, m_QuoteListToolsDlg->m_unSendPort);

	pMsgObj = new MsgObject(MsgObj->GetMessage());
	delete MsgObj;
	m_QuoteListToolsDlg->m_SendDataCtrl.Send(pMsgObj);
	m_QuoteListToolsDlg->Log(strOutput, TRUE);
	m_QuoteListToolsDlg->Log("  ", TRUE);


}
	

// -------------------------------------------------------------------------



void CSendMessageDialog::OnExit() 
{
	this->ShowWindow(SW_HIDE);
}
// -------------------------------------------------------------------------

BOOL CSendMessageDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetDlgItemText(IDC_FIDNO, "-1");
	SetDlgItemText(IDC_RECTYPE, "-1");	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSendMessageDialog::OnTimer(UINT nIDEvent) 
{
	if (m_bRepeat)
	{
		byte bytMsgType = 0;

		if (m_strMsgType == "MGT_LOGONREQUEST") 
			bytMsgType = 	1; 		
		else if (m_strMsgType == "MGT_LOGONRESPONSE") 
			bytMsgType = 	2; 		
		else if (m_strMsgType == "MGT_ADDWATCH") 
			bytMsgType = 	3; 	
		else if (m_strMsgType == "MGT_DELETEWATCH") 
			bytMsgType = 	4; 		
		else if (m_strMsgType == "MGT_SNAPSHOT") 
			bytMsgType = 	5; 		
		else if (m_strMsgType == "MGT_DATAPERMISSION") 
			bytMsgType = 	6; 		
		else if (m_strMsgType == "MGT_FORCEUPDATE") 
			bytMsgType = 	8; 		
		else if (m_strMsgType == "MGT_UPDATE") 
			bytMsgType = 	9; 		
		else if (m_strMsgType == "MGT_VERIFYSYNC") 
			bytMsgType = 	10;		
		else if (m_strMsgType == "MGT_CLOSINGRUN") 
			bytMsgType = 	11;		
		else if (m_strMsgType == "MGT_DROP") 
			bytMsgType = 	12;		
		else if (m_strMsgType == "MGT_INTRADAYREBUILD") 
			bytMsgType = 	13;		
		else if (m_strMsgType == "MGT_INTRADAYCORRECTION") 
			bytMsgType = 	14;		
		else if (m_strMsgType == "MGT_SERVERSTATUS") 
			bytMsgType = 	15;		
		else if (m_strMsgType == "MGT_ALIVE") 
			bytMsgType = 	17;		
		else if (m_strMsgType == "MGT_RAISEHANDREQUEST") 
			bytMsgType = 	18;		
		else if (m_strMsgType == "MGT_RAISEHANDREPLY") 
			bytMsgType = 	19;		
		else if (m_strMsgType == "MGT_SYSTEMMESSAGE") 
			bytMsgType = 	20;		
		else
		{
			AfxMessageBox("Please select the Msg Type first and then send again");
			return;
		}
	
		long lPageNo = 0 ;
		if (m_strPage.GetLength() > 1)
		{
			if ((m_strPage.Left(2) == "0x")||(m_strPage.Left(2) == "0X"))
				sscanf(m_strPage, "%x", &lPageNo);
			else
				sscanf(m_strPage, "%d", &lPageNo);
		}
		else
			lPageNo = atoi(m_strPage);

		if (m_iRecType == -1)
		{	
			SendOneMessage(lPageNo, bytMsgType);
		}
	}
	
	CDialog::OnTimer(nIDEvent);
}
// -------------------------------------------------------------------------
void CSendMessageDialog::OnRepeat() 
{
	if (!m_bRepeat)
	{
		KillTimer( m_unTimer );
		m_ctrlRecType.EnableWindow(FALSE);
		m_ctrlPage2.EnableWindow(FALSE);
	}
	else
	{
		m_ctrlRecType.EnableWindow(FALSE);
		m_ctrlPage2.EnableWindow(FALSE);
	}

}
