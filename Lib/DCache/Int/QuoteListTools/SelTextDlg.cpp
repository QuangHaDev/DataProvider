// SelTextDlg.cpp : implementation file
//

#include "stdafx.h"
#include "QuoteListTools.h"
#include "SelTextDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelTextDlg dialog


CSelTextDlg::CSelTextDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelTextDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelTextDlg)
	m_strOrgText = _T("");
	m_strTextInNo = _T("");
	//}}AFX_DATA_INIT
}


void CSelTextDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelTextDlg)
	DDX_Control(pDX, IDC_ORIG_TEXT, m_orig_text_box);
	DDX_Text(pDX, IDC_ORIG_TEXT, m_strOrgText);
	DDX_Text(pDX, IDC_NO_IN_TEXT, m_strTextInNo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelTextDlg, CDialog)
	//{{AFX_MSG_MAP(CSelTextDlg)
	ON_BN_CLICKED(IDC_TRANSLATE, OnTranslate)
	ON_BN_CLICKED(IDC_BUTTON1, OnGB)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelTextDlg message handlers

void CSelTextDlg::OnTranslate() 
{
	int iStart, iEnd;
	CString strTempNo;
	m_strTextInNo = "";
	m_orig_text_box.GetSel(iStart, iEnd);
	for (int i=iStart; i<iEnd; i++)
	{
		strTempNo.Format("%d ", m_strOrgText.GetAt(i));
		m_strTextInNo += strTempNo;
	}
	SetDlgItemText(IDC_NO_IN_TEXT, m_strTextInNo);
}

void CSelTextDlg::OnGB() 
{
	CString strTemp;	
	GetDlgItemText(IDC_BUTTON1, strTemp);
	int iGBCode = 0;
	if (strTemp=="GB Code")
	{
		iGBCode = 1;
		this->SetDlgItemText(IDC_BUTTON1, "BIG5");
	}
	else
	{
		this->SetDlgItemText(IDC_BUTTON1, "GB Code");
	}
	m_orig_text_box.SetFont(m_pFont[iGBCode]);
}

BOOL CSelTextDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	for (int i = 0; i<2; i++)
	{
		m_pFont[i]= new CFont();
		if (m_pFont)
		{
			CString szFont = "";
			int nheight = 14;
			DWORD nWeight = FW_NORMAL;
			unsigned char CharSet = DEFAULT_CHARSET;
			switch (i)
			{
				case 0:
				break;

				case 1:				
				nheight = 14;
				szFont =  "MingLiu";
				CharSet = GB2312_CHARSET;
				nWeight= FW_NORMAL;
				break;
			}
			m_pFont[i]->CreateFont(
				nheight,            // nHeight
			   0,                         // nWidth
			   0,                         // nEscapement
			   0,                         // nOrientation
			   nWeight,                 // nWeight
			   FALSE,                     // bItalic
			   FALSE,                     // bUnderline
			   0,                         // cStrikeOut
			   CharSet,					  // nCharSet
			   OUT_DEFAULT_PRECIS,        // nOutPrecision
			   CLIP_DEFAULT_PRECIS,       // nClipPrecision
			   DEFAULT_QUALITY,           // nQuality
			   DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
			   szFont);//"Book Antiqua");
		}
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CSelTextDlg::DestroyWindow() 
{
	for (int i = 0; i<2; i++)
		delete m_pFont[i];
	return CDialog::DestroyWindow();
}
