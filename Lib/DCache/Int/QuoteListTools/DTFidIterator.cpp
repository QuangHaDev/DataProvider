/////////////////////////////////////////////////////////////////////////////
//          File:: CDTFidIterator.cpp
// Create Author:: Raymond Yung
// Create   Date:: 30 Semptember 2000
//
//   Description:: The implementation file of the CDTFidIterator.h
//
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000 by AFE Solutions Ltd.
// All rights reserved.
//
// No portion of this software in any form may be used or reproduced in 
// any manner without written consent from AFE Solutions Ltd.
//
//
#include "DTFidIterator.h"
#include "QuoteListToolsDlg.h"
#include "NGUtil.h"

bool CDTFidIterator::ProcessFid()
{
	m_bytMsgType = m_pMsgObj->GetMsgType(); 	// Get Message Type
	m_lPageNo = m_pMsgObj->GetItemNo();			// Get Page No
	m_sFidNo = GetCurFidCode();					// Get Fid No
//	if (m_bCheckFid)
//	{
//		if (( m_sCompareFidNo == -1 ) || ( m_sCompareFidNo == m_sFidNo ))
//			return ParsedFidIterator::ProcessFid();	
//		else
//			return FALSE; 
//	}
//	else
		return ParsedFidIterator::ProcessFid();	   
}
// -------------------------------------------------------------------------
void CDTFidIterator::LogMsgToStatusList(CString strOutput, int iCodePage)
{
	CString strTemp;
	strTemp.Format("Msg Type: %d, ", m_bytMsgType);
	strOutput.Insert(0, strTemp);
	m_QuoteListToolsDlg->Log(strOutput, TRUE, iCodePage);
}

	
// -------------------------------------------------------------------------
void CDTFidIterator::LogMessage(CString strOutput)
{
	if (m_bCheckFid)
		LogMsgToStatusList(strOutput);
	else
		m_QuoteListToolsDlg->Log(strOutput);
}

// -------------------------------------------------------------------------
void CDTFidIterator::LogMessageWithBlankLine(CString strOutput, int iCodePage)
{
	if (m_bCheckFid)
	{
		LogMsgToStatusList(strOutput, iCodePage);
		m_QuoteListToolsDlg->Log(" ",TRUE);
	}
	else
	{
		m_QuoteListToolsDlg->Log(strOutput, FALSE, iCodePage);
		m_QuoteListToolsDlg->Log(" ");
	}
}


// -------------------------------------------------------------------------
// Process D1 - Char
bool CDTFidIterator::ProcessChar()
{
	char c = 0;
	if (!m_pMsgObj->GetFid(m_sFidNo, c))
		return FALSE;
	else
	{
		PrintChar(c);
		return true;
	}
}

// -------------------------------------------------------------------------
// Print D1 - Char
void CDTFidIterator::PrintChar(char& c)
{
	CString strOutput;
	strOutput.Format("Page: %d (0x%x), Fid: %d(Char), Data: %d", m_lPageNo, m_lPageNo, GetCurFidCode(), c);
	LogMessageWithBlankLine(strOutput);
}

// -------------------------------------------------------------------------
// Process D2 - Unicode
bool CDTFidIterator::ProcessUnicode()
{
	WCHAR c;
	if (!m_pMsgObj->GetFid(m_sFidNo, c))
		return FALSE;
	else
	{
		PrintUnicode(c);
		return true;
	}

}
// -------------------------------------------------------------------------
// Print D2 - Unicode
void CDTFidIterator::PrintUnicode(WCHAR& c)
{
	CString strOutput;
	strOutput.Format("Page: %d (0x%x), Fid: %d(Unicode), Data: %d", m_lPageNo, m_lPageNo, GetCurFidCode(), c);
	LogMessageWithBlankLine(strOutput);
}

// -------------------------------------------------------------------------
// Process D3 - short
bool CDTFidIterator::ProcessShort()
{
	short sh = 0;
	if (!m_pMsgObj->GetFid(m_sFidNo, sh))
		return FALSE;
	else
	{
		PrintShort(sh);
		return true;
	}
}

// -------------------------------------------------------------------------
// Print D3 - short
void CDTFidIterator::PrintShort(short& sh)
{
	CString strOutput;
	strOutput.Format("Page: %d (0x%x), Fid: %d(Short), Data: %d", m_lPageNo, m_lPageNo, GetCurFidCode(), sh);
	LogMessageWithBlankLine(strOutput);
}

// -------------------------------------------------------------------------
// Process D4 - unsigned short
bool CDTFidIterator::ProcessUShort()
{
	unsigned short ush = 0;
	if (!m_pMsgObj->GetFid(m_sFidNo, ush))
		return FALSE;
	else
	{
		PrintUShort(ush);
		return true;
	}
}

//// -------------------------------------------------------------------------
// Print D4 - unsigned short
void CDTFidIterator::PrintUShort(unsigned short& ush )
{
	CString strOutput;
	strOutput.Format("Page: %d (0x%x), Fid: %d(UShort), Data: %d", m_lPageNo, m_lPageNo, GetCurFidCode(), ush);
	LogMessageWithBlankLine(strOutput);
}

// -------------------------------------------------------------------------
// Process D5 - long
bool CDTFidIterator::ProcessLong()
{
	long l = 0;
	if (!m_pMsgObj->GetFid(m_sFidNo, l))
		return FALSE;
	else
	{
		PrintLong(l);
		return true;
	}
}

// -------------------------------------------------------------------------
// Print D5 - long
void CDTFidIterator::PrintLong(long& l)
{
	CString strOutput;
	strOutput.Format("Page: %d (0x%x), Fid: %d(Long), Data: %d(0x%x)", m_lPageNo, m_lPageNo, GetCurFidCode(), l, l);
	LogMessageWithBlankLine(strOutput);
}
// -------------------------------------------------------------------------
// Process D6 - unsigned long
bool CDTFidIterator::ProcessULong()
{
	unsigned long ul = 0;
	if (!m_pMsgObj->GetFid(m_sFidNo, ul))
		return FALSE;
	else
	{
		PrintULong(ul);
		return true;
	}
}

// -------------------------------------------------------------------------
// Print D6 - unsigned long
void CDTFidIterator::PrintULong(unsigned long& ul)
{
	CString strOutput;
	strOutput.Format("Page: %d (0x%x), Fid: %d(ULong), Data: %d(0x%x)", m_lPageNo, m_lPageNo, GetCurFidCode(), ul, ul);
	LogMessageWithBlankLine(strOutput);
}

// -------------------------------------------------------------------------
// Process D7 & D8 - float
bool CDTFidIterator::ProcessFloat()
{
	float f = 0;
	if (!m_pMsgObj->GetFid(m_sFidNo, f))
		return FALSE;
	else
	{
		PrintFloat(f);
		return true;
	}
}

// -------------------------------------------------------------------------
// Print D7 float
void CDTFidIterator::PrintFloat(float& f)
{
	CString strOutput;
	strOutput.Format("Page: %d (0x%x), Fid: %d(Float), Data: %f", m_lPageNo, m_lPageNo, GetCurFidCode(), f);
	LogMessageWithBlankLine(strOutput);
}

// -------------------------------------------------------------------------
// Process D8 - unsigned float
bool CDTFidIterator::ProcessUFloat()
{
	float f = 0;
	if (!m_pMsgObj->GetFid(m_sFidNo, f))
		return FALSE;
	else
	{
		PrintUFloat(f);
		return true;
	}
}
// -------------------------------------------------------------------------
// Print D8 - unsigned float
void CDTFidIterator::PrintUFloat(float& f)
{
	CString strOutput;
	strOutput.Format("Page: %d (0x%x), Fid: %d(UFloat), Data: %f", m_lPageNo, m_lPageNo, GetCurFidCode(), f);
	LogMessageWithBlankLine(strOutput);
}
// -------------------------------------------------------------------------
// Process D9 - double
bool CDTFidIterator::ProcessDouble()
{
	double d = 0;
	if (!m_pMsgObj->GetFid(m_sFidNo, d))
		return FALSE;
	else
	{
		PrintDouble(d);
		return true;
	}
}
// -------------------------------------------------------------------------
// Print D9 - double
void CDTFidIterator::PrintDouble(double& d )
{
	CString strOutput;
	strOutput.Format("Page: %d (0x%x), Fid: %d(Double), Data: %f", m_lPageNo, m_lPageNo, GetCurFidCode(), d);
	LogMessageWithBlankLine(strOutput);
}

// -------------------------------------------------------------------------
// Process D10 - unsigned double
bool CDTFidIterator::ProcessUDouble()
{
	double d = 0;
	if (!m_pMsgObj->GetFid(m_sFidNo, d))
		return FALSE;
	else
	{
		PrintUDouble(d);
		return true;
	}
}
// -------------------------------------------------------------------------
// Print D10 - unsigned double
void CDTFidIterator::PrintUDouble(double& d)
{
	CString strOutput;
	strOutput.Format("Page: %d (0x%x), Fid: %d(UDouble), Data: %f", m_lPageNo, m_lPageNo, GetCurFidCode(), d);
	LogMessageWithBlankLine(strOutput);
}
// -------------------------------------------------------------------------
// Process D11 - CharArray
bool CDTFidIterator::ProcessCharArray()
{
	CharArray caOld;
	if (!m_pMsgObj->GetFid(m_sFidNo, caOld))
		return false;
	else
	{
		if ((m_sFidNo == 423) || (m_sFidNo == 424))
			PrintGBCharArray(caOld);
		else
			PrintCharArray(caOld);
		return true;
	}
}
// -------------------------------------------------------------------------
// Print D11 - CharArray
void CDTFidIterator::PrintCharArray(CharArray& caOld)
{
	CharArray ca;
	CNGUtil::ConvertUTF8ToBIG5(caOld, ca);
	CString strOutput;
	strOutput.Format("Page: %d (0x%x), Fid: %d(CharArray), Data: Len: %d, Sq: %d, Pos: %d, String: %s", 
					m_lPageNo, m_lPageNo, GetCurFidCode(), ca.GetLen(), ca.GetSq(), ca.GetPos(), ca.GetString());
	LogMessageWithBlankLine(strOutput);
}

// -------------------------------------------------------------------------
// Print D11 - CharArray
void CDTFidIterator::PrintGBCharArray(CharArray& caOld)
{
	CharArray ca;
	CNGUtil::ConvertUTF8ToGB(caOld, ca);
	CString strOutput;
	strOutput.Format("Page: %d (0x%x), Fid: %d(CharArray(GB)), Data: Len: %d, Sq: %d, Pos: %d, String: %s", 
					m_lPageNo, m_lPageNo, GetCurFidCode(), ca.GetLen(), ca.GetSq(), ca.GetPos(), ca.GetString());
	LogMessageWithBlankLine(strOutput, 936);
}
// -------------------------------------------------------------------------
// Process D12 - MCharArray
bool CDTFidIterator::ProcessMCharArray()
{
	MCharArray mcaOld;
	if (!m_pMsgObj->GetFid(m_sFidNo, mcaOld))
		return FALSE;
	else
	{
		PrintMCharArray(mcaOld);
		return true;
	}
}
// -------------------------------------------------------------------------
// Print D12 - MCharArray
void CDTFidIterator::PrintMCharArray(MCharArray& mcaOld)
{
	MCharArray mca2;
	CNGUtil::ConvertUTF8ToBIG5(mcaOld, mca2);
	CString strOutput;
	for (int i=0; i<mca2.GetNe(); i++)
	{
		strOutput.Format("(CharArray)Data: Ln: %d,Len: %d, Pos: %d, Sq: %d, String: %s", 
						 mca2.GetLn(i), mca2.GetCA(i)->GetLen(), mca2.GetCA(i)->GetPos(), mca2.GetCA(i)->GetSq(),
						 mca2.GetCA(i)->GetString());
		LogMessage(strOutput);
	}
	strOutput.Format("Page: %d (0x%x), Fid: %d(MCharArray), Data: Total Line: %d, NE: %d", 
					m_lPageNo, m_lPageNo, GetCurFidCode(), mca2.GetTL(), mca2.GetNe());
	LogMessageWithBlankLine(strOutput);
}
// -------------------------------------------------------------------------
// Process D13 - BDateYear
bool CDTFidIterator::ProcessBDateYear()
{
	BDateYear dy;
	if (!m_pMsgObj->GetFid(m_sFidNo, dy))
		return FALSE;
	else
	{
		PrintBDateYear(dy);
		return true;
	}
}
// -------------------------------------------------------------------------
// Print D13 - BDateYear
void CDTFidIterator::PrintBDateYear(BDateYear& dy)
{
	CString strOutput;
	strOutput.Format("Page: %d (0x%x), Fid: %d(BDateYear), Data: Day: %d, Month: %d, Year: %d", 
					 m_lPageNo, m_lPageNo, GetCurFidCode(), dy.GetDay(), dy.GetMonth(), dy.GetYear());
	LogMessageWithBlankLine(strOutput);
}
// -------------------------------------------------------------------------
// Process D14 - BDateMonth
bool CDTFidIterator::ProcessBDateMonth()
{
	BDateMonth dm;
	if (!m_pMsgObj->GetFid(m_sFidNo, dm))
		return FALSE;
	else
	{
		PrintBDateMonth(dm);
		return true;
	}
}

// -------------------------------------------------------------------------
// Print D14 - BDateMonth
void CDTFidIterator::PrintBDateMonth(BDateMonth& dm)
{
	CString strOutput;
	strOutput.Format("Page: %d (0x%x), Fid: %d(BDateMonth), Data: Day: %d, Month: %d", m_lPageNo, m_lPageNo, GetCurFidCode(), dm.GetDay(), dm.GetMonth());
	LogMessageWithBlankLine(strOutput);
}

// -------------------------------------------------------------------------
// Process D15 - BTimeMin
bool CDTFidIterator::ProcessBTimeMin()
{
	BTimeMin tm;
	if (!m_pMsgObj->GetFid(m_sFidNo, tm))
		return FALSE;
	else
	{
		PrintBTimeMin(tm);
		return true;
	}
}
// -------------------------------------------------------------------------
// Print D15 - BTimeMin
void CDTFidIterator::PrintBTimeMin(BTimeMin& tm)
{
	CString strOutput;
	strOutput.Format("Page: %d (0x%x), Fid: %d(BTimeMin), Data: Hour: %d, Minute: %d", 
					 m_lPageNo, m_lPageNo, GetCurFidCode(), tm.GetHH(), tm.GetMIN());
	LogMessageWithBlankLine(strOutput);
}
// -------------------------------------------------------------------------
// Process D16 - BTimeSec
bool CDTFidIterator::ProcessBTimeSec()
{
	BTimeSec ts;
	if (!m_pMsgObj->GetFid(m_sFidNo, ts))
		return FALSE;
	else
	{
		PrintBTimeSec(ts);
		return true;
	}
}
// -------------------------------------------------------------------------
// Print D16 - BTimeSec
void CDTFidIterator::PrintBTimeSec(BTimeSec& ts)
{
	CString strOutput;
	strOutput.Format("Page: %d (0x%x), Fid: %d(BTimeSec), Data: Hour: %d, Minute: %d,  Second: %d", 
					 m_lPageNo, m_lPageNo, GetCurFidCode(), ts.GetHH(), ts.GetMIN(), ts.GetSS());
	LogMessageWithBlankLine(strOutput);
}

// -------------------------------------------------------------------------
// Process D17 - BTimeRanges
bool CDTFidIterator::ProcessBTimeRanges()
{
	BTimeRanges tr;
	if (!m_pMsgObj->GetFid(m_sFidNo, tr))
		return FALSE;
	else
	{
		PrintBTimeRanges(tr);
		return true;
	}
}
// -------------------------------------------------------------------------
// Print D17 - BTimeRanges
void CDTFidIterator::PrintBTimeRanges(BTimeRanges& tr)
{
	CString strOutput;

	BTimeMin s_tm, e_tm;
	for (int i=0; i<tr.GetNe(); i++)
	{
		if (tr.GetRange(i, s_tm, e_tm))
		{
			strOutput.Format("(BTimeSec), Data: Index: %d, Start Time: %d:%d, End Time: %d:%d", 
							i, s_tm.GetHH(), s_tm.GetMIN(), e_tm.GetHH(), e_tm.GetMIN());
			LogMessage(strOutput);
		}
	}
	strOutput.Format("Page: %d (0x%x), Fid: %d(BTimeRanges), Data: NE : %d", 
					 m_lPageNo, m_lPageNo, GetCurFidCode(), tr.GetNe());
	LogMessageWithBlankLine(strOutput);
}
// -------------------------------------------------------------------------
// Process D18 - Tran
bool CDTFidIterator::ProcessTran()
{
	Tran tran;
	if (!m_pMsgObj->GetFid(m_sFidNo, tran))
		return FALSE;
	else
	{
		PrintTran(tran);
		return true;
	}
}

// -------------------------------------------------------------------------
// Print D18 - Tran
void CDTFidIterator::PrintTran(Tran& tran)
{
	CString strOutput;

	unsigned long sq = 0;
	BTimeMin tm;
	double vl = 0;
	float nm = 0;
	char fg = 0;

	for (int i=0; i<tran.GetNe(); i++)
	{
		if (tran.GetTranElement(i, sq, tm, vl, nm, (byte&)fg))
		{
			strOutput.Format("Data: Index: %d, SqNo: %d, Time: %d:%d, Vol: %f,Price: %f, Flag: %c", 
			i, sq, tm.GetHH(), tm.GetMIN(), vl, nm, fg);
				LogMessage(strOutput);
		}
	}
	strOutput.Format("Page: %d (0x%x), Fid: %d(Tran), Data: NE : %d", 
					 m_lPageNo, m_lPageNo, GetCurFidCode(), tran.GetNe());
	LogMessageWithBlankLine(strOutput);
}
// -------------------------------------------------------------------------
// Process D19 - Txn
bool CDTFidIterator::ProcessTxn()
{
	Txn txn;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), txn))
		return FALSE;
	else
	{
		PrintTxn(txn);
		return true;
	}
}
// -------------------------------------------------------------------------
// Print D19 - Txn
void CDTFidIterator::PrintTxn(Txn& txn)
{
	CString strOutput;
	BTimeMin tm;
	double vl = 0;
	float nm = 0;
	char fg = 0;

	for (int i=0; i<txn.GetNe(); i++)
	{
		if (txn.GetTranElement(i, tm, vl, nm, (byte&)fg))
		{
			strOutput.Format("Data: Index: %d, Time: %d:%d, Vol: %f, Price: %f, Flag: %c", 
			i, tm.GetHH(), tm.GetMIN(), vl, nm, fg);
			LogMessage(strOutput);
		}
	}
	strOutput.Format("Page: %d (0x%x), Fid: %d(Txn), Data: Total: %d, NE: %d, Sq: %d", 
					 m_lPageNo, m_lPageNo, GetCurFidCode(), txn.GetTL(), txn.GetNe(), txn.GetSq());
	LogMessageWithBlankLine(strOutput);
}

// -------------------------------------------------------------------------
// Process D20 - ShrOrdLst
bool CDTFidIterator::ProcessShrOrdLst()
{
	ShrOrdLst sol;
	if (!m_pMsgObj->GetFid(m_sFidNo, sol))
		return FALSE;
	else
	{
		PrintShrOrdLst(sol);
		return true;
	}
}
// -------------------------------------------------------------------------
// Print D20 - ShrOrdLst
void CDTFidIterator::PrintShrOrdLst(ShrOrdLst& sol)
{
	CString strOutput;
	strOutput.Format("Page: %d (0x%x), Fid: %d(ShrOrdLst), Data: Shares: %s, Orders: %d", 
					 m_lPageNo, m_lPageNo, GetCurFidCode(), sol.GetShStr(), sol.GetOr());
	LogMessageWithBlankLine(strOutput);
}
// -------------------------------------------------------------------------
// Process D21 - PRCVOL 
bool CDTFidIterator::ProcessPrcVol()
{
	PrcVol prc;
	if (!m_pMsgObj->GetFid(m_sFidNo, prc))
		return FALSE;
	else
	{
		PrintPrcVol(prc);
		return true;
	}
}
// -------------------------------------------------------------------------
// Print D21 - PRCVOL 
void CDTFidIterator::PrintPrcVol(PrcVol& prc)
{
	CString strOutput;

	float fPr = 0;
	double dVl = 0;

	for (int i=0; i<prc.GetNe(); i++)
	{
		if (prc.GetPrcVol(i, fPr, dVl))
		{
			strOutput.Format("Data: Index: %d, Price: %f, Volumn: %f", i, fPr, dVl);
				LogMessage(strOutput);
		}
	}
	strOutput.Format("Page: %d (0x%x), Fid: %d(PRCVOL), Data: NE: %d Pos: %d, TL: %d", 
					 m_lPageNo, m_lPageNo, GetCurFidCode(), prc.GetNe(), prc.GetPos(), prc.GetTL());
	LogMessageWithBlankLine(strOutput);
}

// -------------------------------------------------------------------------
// Process D22 - BkrQue
bool CDTFidIterator::ProcessBkrQue()
{
	BkrQue bkrque;
	if (!m_pMsgObj->GetFid(m_sFidNo, bkrque)) 
		return FALSE;
	else
	{
		PrintBkrQue(bkrque);
		return true;
	}
}

// -------------------------------------------------------------------------
// Print D22 - BkrQue
void CDTFidIterator::PrintBkrQue(BkrQue& bkrque)
{
	CString strOutput;

	byte byA = 0;
	unsigned short unB = 0;
	unsigned short unC = 0;

	for (int i=0; i<bkrque.GetNe(); i++)
	{
		if (bkrque.GetBroker(i, byA, unB, unC))
		{
			strOutput.Format("Data: Index: %d, Type: %d, Broker No: %d, Spare: %d",i, byA, unB, unC);
			LogMessage(strOutput);
		}
	}
	strOutput.Format("Page: %d (0x%x), Fid: %d(BkrQue), Data: NE: %d", 
					 m_lPageNo, m_lPageNo, GetCurFidCode(), bkrque.GetNe());
	LogMessageWithBlankLine(strOutput);
}
// -------------------------------------------------------------------------
// Process D23 - Link
bool CDTFidIterator::ProcessLink()
{
	Link link;
	if (!m_pMsgObj->GetFid(m_sFidNo, link))
		return FALSE;
	else
	{
		PrintLink(link);
		return true;
	}
}
// -------------------------------------------------------------------------
// Print D23 - Link
void CDTFidIterator::PrintLink(Link& link)
{
	CString strOutput;
	unsigned long ulLink;

	for (int i=0; i<link.GetNe(); i++)
	{
		if (link.GetLink(i, ulLink))
		{
			strOutput.Format("Data: Index: %d, Link : %d",i, ulLink);
			LogMessage(strOutput);
		}
	}
	strOutput.Format("Page: %d (0x%x), Fid: %d(Link), Data: Total: %d, NE: %d, Sq: %d", 
					 m_lPageNo, m_lPageNo, GetCurFidCode(), link.GetTL(), link.GetNe(), link.GetPos());
	LogMessageWithBlankLine(strOutput);
}
// -------------------------------------------------------------------------
// Process D24 - Wlink
bool CDTFidIterator::ProcessWlink()
{
	WLink wlink;
	if (!m_pMsgObj->GetFid(m_sFidNo, wlink))
		return FALSE;
	else
	{
		PrintWlink(wlink);
		return true;
	}
}
// -------------------------------------------------------------------------
// Print D24 - Wlink
void CDTFidIterator::PrintWlink(WLink& wlink)
{
	CString strOutput;
	unsigned long ulLink;
	unsigned short unCur;
	float fWt;
	unsigned short unCu;

	for (int i=0; i<wlink.GetNe(); i++)
	{
		if (wlink.GetWLink(i, ulLink, unCur, fWt, unCu))
		{
			strOutput.Format("Data: Index: %d, PgNo. : %d	Cur: %d	Wt: %f	Cu: %d",i, ulLink, unCur, fWt, unCu);
			LogMessage(strOutput);
		}
	}
	strOutput.Format("Page: %d (0x%x), Fid: %d(WLink), Data: Total: %d, NE: %d, Sq: %d", 
					 m_lPageNo, m_lPageNo, GetCurFidCode(), wlink.GetTL(), wlink.GetNe(), wlink.GetPos());
	LogMessageWithBlankLine(strOutput);
}
// -------------------------------------------------------------------------
// Process D25 - YLabel
bool CDTFidIterator::ProcessYLabel()
{
	YLabel ylabel;
	if (!m_pMsgObj->GetFid(m_sFidNo, ylabel))
		return FALSE;
	else	
	{
		PrintYLabel(ylabel);
		return true;
	}
}
// -------------------------------------------------------------------------
// Print D25 - YLabel
void CDTFidIterator::PrintYLabel(YLabel& ylabel)
{
	CString strOutput;
	strOutput.Format("Page: %d (0x%x), Fid: %d(YLabel), Data: High: %f, Mid: %f, Low: %f", 
					 m_lPageNo, m_lPageNo, GetCurFidCode(), ylabel.GetHigh(), ylabel.GetMid(), ylabel.GetLow());
	LogMessageWithBlankLine(strOutput);
}

// -------------------------------------------------------------------------
// Process D26 - XMtcLabel
bool CDTFidIterator::ProcessXMtcLabel()
{
	XMtcLabel xmtcLbl;
	if (!m_pMsgObj->GetFid(m_sFidNo, xmtcLbl))
		return FALSE;
	else
	{
		PrintXMtcLabel(xmtcLbl);
		return true;
	}
}
// -------------------------------------------------------------------------
// Print D26 - XMtcLabel
void CDTFidIterator::PrintXMtcLabel(XMtcLabel& xmtcLbl)
{
	CString strOutput;
	unsigned short nPos = 0;
	BTimeMin tm;

	for (int i=0; i<xmtcLbl.GetNe(); i++)
	{
		if (xmtcLbl.GetTime(i, nPos, tm))
		{
			strOutput.Format("Data: Index: %d, Pos: %d, Time: %d:%d",i, nPos, tm.GetHH(), tm.GetMIN());
			LogMessage(strOutput);
		}
	}
	strOutput.Format("Page: %d (0x%x), Fid: %d(XMtcLabel), Data: NE : %d", 
					 m_lPageNo, m_lPageNo, GetCurFidCode(), xmtcLbl.GetNe());
	LogMessageWithBlankLine(strOutput);
}
// -------------------------------------------------------------------------
// Process D27 - XDtcLabel
bool CDTFidIterator::ProcessXDtcLabel()
{
	XDtcLabel xdtcLbl;
	if (!m_pMsgObj->GetFid(m_sFidNo, xdtcLbl))
		return FALSE;
	else
	{
		PrintXDtcLabel(xdtcLbl);
		return true;
	}
}


// -------------------------------------------------------------------------
// Print D27 - XDtcLabel
void CDTFidIterator::PrintXDtcLabel(XDtcLabel& xdtcLbl)
{
	CString strOutput;
	unsigned short nPos = 0;
	BDateMonth dt;

	for (int i=0; i<xdtcLbl.GetNe(); i++)
	{
		if (xdtcLbl.GetDate(i, nPos, dt))
		{
			strOutput.Format("Data: Index: %d, Pos: %d, Date: %d-%d",i, nPos, dt.GetMonth(), dt.GetDay());
			LogMessage(strOutput);
		}
	}
	strOutput.Format("Page: %d (0x%x), Fid: %d(XDtcLabel), Data: NE : %d", 
						 m_lPageNo, m_lPageNo, GetCurFidCode(), xdtcLbl.GetNe());
	LogMessageWithBlankLine(strOutput);
}

// -------------------------------------------------------------------------
// Process D28 - MtcPoints
bool CDTFidIterator::ProcessMtcPoints()
{
	MtcPoints mtcPts;
	if (!m_pMsgObj->GetFid(m_sFidNo, mtcPts))
		return FALSE;
	else
	{
		PrintMtcPoints(mtcPts);
		return true;
	}
}
// -------------------------------------------------------------------------
// Print D28 - MtcPoints
void CDTFidIterator::PrintMtcPoints(MtcPoints& mtcPts)
{
	CString strOutput;
	byte byPts = 0;
	for (int i=0; i<mtcPts.GetNe(); i++)
	{
		if (mtcPts.GetPoint(i, byPts))
		{
			strOutput.Format("Data: Index: %d, Points: %d",i, byPts);
			LogMessage(strOutput);
		}
	}
	strOutput.Format("Page: %d (0x%x), Fid: %d(MtcPoints), Data: Total: %d, NE: %d, Pos: %d", 
					 m_lPageNo, m_lPageNo, GetCurFidCode(), mtcPts.GetTL(), mtcPts.GetNe(), mtcPts.GetPos());
	LogMessageWithBlankLine(strOutput);
}


// -------------------------------------------------------------------------
// Process D29 - MtcVal
bool CDTFidIterator::ProcessMtcVal()
{
	MtcVal mtcval;
	if (!m_pMsgObj->GetFid(m_sFidNo, mtcval))
		return FALSE;
	else
	{
		PrintMtcVal(mtcval);
		return true;
	}
}
// -------------------------------------------------------------------------
// Print D29 - MtcVal
void CDTFidIterator::PrintMtcVal(MtcVal& mtcval)
{
	CString strOutput;
	float fVal = 0;
	for (int i=0; i<mtcval.GetNe(); i++)
	{
		if (mtcval.GetVal(i, fVal))			
		{
			strOutput.Format("Data: Index: %d, Actual Value: %f",i, fVal);
			LogMessage(strOutput);
		}
	}
	strOutput.Format("Page: %d (0x%x), Fid: %d(MtcVal), Data: Total: %d, NE: %d, Pos: %d", 
						 m_lPageNo, m_lPageNo, GetCurFidCode(), mtcval.GetTL(), mtcval.GetNe(), mtcval.GetPos());
	LogMessageWithBlankLine(strOutput);
}
// -------------------------------------------------------------------------
// Process D30 - DtcPoints
bool CDTFidIterator::ProcessDtcPoints()
{
	DtcPoints dtcPts;
	if (!m_pMsgObj->GetFid(m_sFidNo, dtcPts))
		return FALSE;
	else
	{	
		PrintDtcPoints(dtcPts);
		return true;
	}
}
// -------------------------------------------------------------------------
// Print D30 - DtcPoints
void CDTFidIterator::PrintDtcPoints(DtcPoints& dtcPts)
{
	CString strOutput;
	byte byHt = 0;
	byte byLw = 0;
	byte byCls = 0;

	for (int i=0; i<dtcPts.GetNe(); i++)
	{
		if (dtcPts.GetDtcElement(i, byHt, byLw, byCls))
		{
			strOutput.Format("Data: Index: %d, High: %d, Low: %d, Close: %d",i, byHt, byLw, byCls);
			LogMessage(strOutput);
		}
	}
	strOutput.Format("Page: %d (0x%x), Fid: %d(DtcPoints), Data: NE : %d", 
					 m_lPageNo, m_lPageNo, GetCurFidCode(), dtcPts.GetNe());
	LogMessageWithBlankLine(strOutput);
}
// -------------------------------------------------------------------------
// Process D31 - HisVal
bool CDTFidIterator::ProcessHisVal()
{
	HisVal hisval;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), hisval))
		return FALSE;
	else
	{
		PrintHisVal(hisval);
		return true;
	}
}
// -------------------------------------------------------------------------
// Print D31 - HisVal
void CDTFidIterator::PrintHisVal(HisVal& hisval)
{

	BDateYear Dt;
	hisval.GetDt(Dt);

	CString strOutput;

	float fHt = 0;
	float fLw = 0;
	float fCls = 0;
	unsigned long ulVol = 0;

	for (int i=0; i<hisval.GetNe(); i++)
	{
		if (hisval.GetVal(i, fHt, fLw, fCls, ulVol))
		{
			strOutput.Format("Data: Index: %d, High: %f, Low: %f, Close: %f, Vol: %d",i , fHt, fLw, fCls, ulVol);
			LogMessage(strOutput);
		}
	}
	strOutput.Format("Page: %d (0x%x), Fid: %d(HisVal), Data: Total : %d	HisVal NE : %d	HisVal Date: %d-%d-%d", 
					 m_lPageNo, m_lPageNo, GetCurFidCode(), hisval.GetTL(), hisval.GetNe(), Dt.GetYear(), Dt.GetMonth(), Dt.GetDay());
	LogMessageWithBlankLine(strOutput);

}

// -------------------------------------------------------------------------
// Process D32 - FieldList
bool CDTFidIterator::ProcessFieldList()
{
	FieldList fldLst;
	if (!m_pMsgObj->GetFid(m_sFidNo, fldLst))
		return FALSE;
	else
	{
		PrintFieldList(fldLst);
		return true;
	}
}

// -------------------------------------------------------------------------
// Print D32 - FieldList
void CDTFidIterator::PrintFieldList(FieldList& fldLst)
{
	CString strOutput;

	unsigned short nFd;
	for (int i=0; i<fldLst.GetNe(); i++)
	{
		if (fldLst.GetField(i, nFd))
		{
			strOutput.Format("Data: Index: %d, Field: %d",i ,nFd);
			LogMessage(strOutput);
		}
	}
	strOutput.Format("Page: %d (0x%x), Fid: %d(FieldList), Data: NE : %d", 
					 m_lPageNo, m_lPageNo, GetCurFidCode(), fldLst.GetNe());
	LogMessageWithBlankLine(strOutput);

}

// -------------------------------------------------------------------------
// Process D33 - FieldLabel
bool CDTFidIterator::ProcessFieldLabel()
{
	FieldLabel fldLblOld, fldLbl;
	if (!m_pMsgObj->GetFid(m_sFidNo, fldLblOld))
		return FALSE;
	else
	{
		PrintFieldLabel(fldLblOld);
		return true;
	}
}

// -------------------------------------------------------------------------
// Print D33 - FieldLabel
void CDTFidIterator::PrintFieldLabel(FieldLabel& fldLblOld)
{
	FieldLabel fldLbl;
	CNGUtil::ConvertUTF8ToBIG5(fldLblOld, fldLbl);
	CString strOutput;

	char c[256] = "";
	for (int i=0; i<fldLbl.GetNe(); i++)
	{
		if (fldLbl.GetLabel(i, c, 255))	
		{
			strOutput.Format("Data: Index: %d, Field: %s", i, c);
			LogMessage(strOutput);
		}
	}
	strOutput.Format("Page: %d (0x%x), Fid: %d(FieldLabel), Data: NE : %d", 
					 m_lPageNo, m_lPageNo, GetCurFidCode(), fldLbl.GetNe());
	LogMessageWithBlankLine(strOutput);
}

// -------------------------------------------------------------------------
// Process D34 - InfoMsg
bool CDTFidIterator::ProcessInfoMsg()
{
	InfoMsg infomsgOld, infomsg;
	if (!m_pMsgObj->GetFid(m_sFidNo, infomsgOld))
		return FALSE;
	else
	{
		PrintInfoMsg(infomsgOld);
		return true;
	}
}

// -------------------------------------------------------------------------
// Print D34 - InfoMsg
void CDTFidIterator::PrintInfoMsg(InfoMsg& infomsgOld)
{
	InfoMsg infomsg;
	CNGUtil::ConvertUTF8ToBIG5(infomsgOld, infomsg);
	CString strOutput;

	char c[256] = "";
	unsigned long ulPg = 0;
	unsigned short unFd = 0;

	for (int i=0; i<infomsg.GetNe(); i++)
	{
		if (infomsg.GetInfo(i, c, 255, ulPg, unFd))
		{
			strOutput.Format("Data: Index: %d, Field: %s, Link: %d, Field Code: %d", i, c, ulPg, unFd);
			LogMessage(strOutput);
		}
	}
	strOutput.Format("Page: %d (0x%x), Fid: %d(InfoMsg), Data: Total: %d, NE: %d", 
					 m_lPageNo, m_lPageNo, GetCurFidCode(), infomsg.GetTL(), infomsg.GetNe());
	LogMessageWithBlankLine(strOutput);
}
// -------------------------------------------------------------------------
// Process D35 - HeadLine
bool CDTFidIterator::ProcessHeadLine()
{
	HeadLine headline;
	if (!m_pMsgObj->GetFid(m_sFidNo, headline))
		return FALSE;
	else
	{
		PrintHeadLine(headline);
		return true;
	}
}
// -------------------------------------------------------------------------
// Print D35 - HeadLine
void CDTFidIterator::PrintHeadLine(HeadLine& headline)
{
	unsigned long nLn = 0;
	BDateMonth dt;
	BTimeSec tm;
	byte byType;
	char pcLg[4] = "";
	unsigned long ulPg;
	char pcPNAC[256] = "";
	char pcTopicCode[1024] = "";
	char pcString[257] = "";
	byte byNe = 0;
	unsigned long pulLong[1024];
	memset(pulLong, 0, 1024*4);

	headline.GetHeadLine(nLn, dt, tm, byType, pcLg, ulPg, pcPNAC, pcTopicCode, byNe, pulLong, pcString);

	CString strOutput;
	strOutput.Format("TopicCode: %s", pcTopicCode);
	LogMessage(strOutput);

	for (int i =0; i<byNe; i++)
	{
		strOutput.Format("Data: Page No. %d: %d", i, pulLong[i]);
		LogMessage(strOutput);
	}
	strOutput.Format("Data: String: %s", pcString);
	LogMessage(strOutput);
	strOutput.Format("Type: %d, Lang. ID: %s, PageNo.: %d, PNAC: %s", byType, pcLg, ulPg, pcPNAC);
	LogMessage(strOutput);
	strOutput.Format("Page: %d (0x%x), Fid: %d(HeadLine), Data: Line Number: %d, Date: %d-%d, Time: %d:%d:%d", 
					 m_lPageNo, m_lPageNo, GetCurFidCode(), nLn, dt.GetMonth(), dt.GetDay(), tm.GetHH(), tm.GetMIN(), tm.GetSS());
	LogMessageWithBlankLine(strOutput);
}

// -------------------------------------------------------------------------
// Process D36 - HLink
bool CDTFidIterator::ProcessHLink()
{
	HLink hlink;
	if (!m_pMsgObj->GetFid(m_sFidNo, hlink))
		return FALSE;
	else
	{
		PrintHLink(hlink);
		return true;
	}
}

// -------------------------------------------------------------------------
// Print D36 - HLink
void CDTFidIterator::PrintHLink(HLink& hlink)
{
	CString strOutput;
	unsigned long ulPg = 0;
	unsigned long unLn = 0;

	for (int i=0; i<hlink.GetNe(); i++)
	{
		if (hlink.GetHLink(i, ulPg, unLn))
		{
			strOutput.Format("Data: Index: %d, Page No: %d, Line No.: %d", i, ulPg, unLn);
		LogMessage(strOutput);

		}
	}
	strOutput.Format("Page: %d (0x%x), Fid: %d(HLink), Data: Total : %d, NE: %d, Pos: %d", 
					 m_lPageNo, m_lPageNo, GetCurFidCode(), hlink.GetTL(), hlink.GetNe(), hlink.GetPos());
	LogMessageWithBlankLine(strOutput);
}
// -------------------------------------------------------------------------
// Process D37 - MShrOrdLst
bool CDTFidIterator::ProcessMShrOrdLst()
{
	MShrOrdLst mshr;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), mshr))
		return FALSE;
	else
	{
		PrintMShrOrdLst(mshr);
		return true;
	}
}
// -------------------------------------------------------------------------
// Print D37 - MShrOrdLst
void CDTFidIterator::PrintMShrOrdLst(MShrOrdLst& mshr)
{
	CString strOutput;
	ShrOrdLst shr;

	for (int i=0; i<mshr.GetNe(); i++)
	{
		if (mshr.GetShrOrdLst(i, shr))
		{
			strOutput.Format("Data: Index: %d, Shares: %s, Orders: %d", i, shr.GetShStr(), shr.GetOr());
			LogMessage(strOutput);
		}
	}
	strOutput.Format("Page: %d (0x%x), Fid: %d(MShrOrdLst), Data: NE : %d", 
					 m_lPageNo, m_lPageNo, GetCurFidCode(), mshr.GetNe());
	LogMessageWithBlankLine(strOutput);
}
// -------------------------------------------------------------------------
// Process D38 - SprTbl
bool CDTFidIterator::ProcessSprTbl()
{

	SprTbl sprtbl;
	if (!m_pMsgObj->GetFid(m_sFidNo, sprtbl))
		return FALSE;
	else
	{
		PrintSprTbl(sprtbl);
		return true;
	}
}
// -------------------------------------------------------------------------
// Print D38 - SprTbl
void CDTFidIterator::PrintSprTbl(SprTbl& sprtbl)
{
	CString strOutput;

	float fPF = 0;
	float fPT = 0;
	float fSP = 0;

	for (int i=0; i<sprtbl.GetNe(); i++)
	{
		if (sprtbl.GetSprTbl(i, fPF, fPT, fSP))
		{
			strOutput.Format("Data: Index: %d, PriceFrom: %f, PriceTo: %f, SprVal: %f", i, fPF, fPT, fSP);
			LogMessage(strOutput);
		}
	}
	strOutput.Format("Page: %d (0x%x), Fid: %d(SprTbl), Pos: %d, Total: %d, NE: %d", 
					 m_lPageNo, m_lPageNo, GetCurFidCode(), sprtbl.GetPos(), sprtbl.GetTL(), sprtbl.GetNe());
	LogMessageWithBlankLine(strOutput);
}
// -------------------------------------------------------------------------
// Process Type D39 - QuoteList
bool CDTFidIterator::ProcessQuoteList()
{
	QuoteList quoteList;
	if (!m_pMsgObj->GetFid(m_sFidNo, quoteList))
		return FALSE;
	else
	{
		PrintQuoteList(quoteList);
		return true;
	}
}

// -------------------------------------------------------------------------
// Print Type D39 - QuoteList
void CDTFidIterator::PrintQuoteList(QuoteList& quoteList)
{
	CString strOutput;
	byte yLn, yLen;
	short sFid;
	void* pFV;;
	int iTotalCol = 12;

	int col[1024]={ULong,UFloat,UFloat,UFloat,UFloat,UFloat,Other,Other,Other,Other,Other,Other,Other,Other};

	switch (m_sFidNo)
	{
		case 1460: // Top Ranking
			{
				col[6]  = UDouble;
				col[7]  = UDouble;
				col[8]  = ECharrArray;
				col[9]  = CCharArray;
				col[10] = UChar;
				col[11] = GBCharArray;
			}
			break;
		case 1463: // World Indices
			{
				col[6]  = UFloat;
				col[7]  = DateYear;
				col[8]  = ECharrArray;
				col[9]  = CCharArray;
				col[10] = UChar;
				col[11] = GBCharArray;
			}
			break;
		case 1464: // Warrent Summary
			{
				col[6]  = UFloat;
				col[7]  = UFloat;
				col[8]  = DateYear;
				col[9]  = UDouble;
				col[10] = ECharrArray;
				col[11] = CCharArray;
				col[12] = UChar;
				col[13] = GBCharArray;
				iTotalCol = 14;
			}
			break;
		case 1465: // Index Constituent
			{
				col[6]  = ECharrArray;
				col[7]  = CCharArray;
				col[8]  = UChar;
				col[9]  = GBCharArray;
				iTotalCol = 10;
			}
			break;
		case 1466: //Futures
			{				
				col[1]=CCharArray;
				col[2]=ULong;
				col[3]=CCharArray;
				col[4]=ULong;
				col[5]=CCharArray;
				col[6]=CCharArray;
				col[7]=CCharArray;
				col[8]=CCharArray;
				col[9]=CCharArray;
				col[10]=UDouble;
				//last
				col[11]=UChar;
				col[12]=CCharArray;
				col[13]=UDouble;
				//last1
				col[14]=UChar;
				col[15]=CCharArray;
				col[16]=UDouble;
				//last2
				col[17]=UChar;
				col[18]=CCharArray;
				col[19]=UDouble;
				//last3
				col[20]=UChar;
				col[21]=CCharArray;
				col[22]=UDouble;
				//last4
				col[23]=UChar;
				col[24]=CCharArray;
				col[25]=UDouble;

				col[26]=CCharArray;
				col[27]=CCharArray;
				col[28]=UDouble;
				col[29]=UDouble;
				col[30]=ULong;
				col[31]=ULong;
				col[32]=DateYear;
				col[33]=CCharArray;
				col[34]=ECharrArray;
				iTotalCol = 35;
			}
			break;
		case 1467: //Options
			{		
				col[1]=ULong;
				col[2]=UDouble;
				col[3]=UDouble;
				col[4]=UFloat;
				col[5]=UFloat;
				//Call Bid
				col[6]=UFloat;
				col[7]=UFloat;
				col[8]=ULong;
				col[9]=ULong;
				col[10]=UFloat;
				col[11]=UFloat;
				//open
				col[12]=UFloat;
				col[13]=UFloat;
				//high
				col[14]=UFloat;
				col[15]=UFloat;
				//low
				col[16]=UFloat;
				col[17]=UFloat;
				//Prv Close
				col[18]=UFloat;				
				col[19]=UFloat;
				//O.I.
				col[20]=ULong;
				col[21]=ULong;			
				//
				col[22]=ULong;
				col[23]=ULong;
				//last1
				col[24]=UFloat;
				col[25]=UFloat;
				col[26]=UDouble;
				col[27]=UDouble;
				//last2
				col[28]=UFloat;
				col[29]=UFloat;
				col[30]=UDouble;
				col[31]=UDouble;
				//last3
				col[32]=UFloat;
				col[33]=UFloat;
				col[34]=UDouble;
				col[35]=UDouble;
				//last4
				col[36]=UFloat;
				col[37]=UFloat;
				col[38]=UDouble;
				col[39]=UDouble;
				//last5
				col[40]=UFloat;
				col[41]=UFloat;
				col[42]=UDouble;
				col[43]=UDouble;
				//
				iTotalCol = 44;
				//
				col[4]=col[5]=col[6]=col[7]=col[12]=col[13]=col[14]=col[15]=col[16]=col[17]=col[18]=col[19]=CCharArray;
				col[24]=col[25]=CCharArray;
				col[28]=col[29]=CCharArray;
				col[32]=col[33]=CCharArray;
				col[36]=col[37]=CCharArray;
				col[40]=col[41]=CCharArray;
			}
			break;		
		case 1468: 
		case 1469: 
		case 1470: 
		case 1471: 
		case 1472: 
		case 1473: 
		case 1474: 
		case 1475: 
		case 1476: 
		case 1477: 
			{		
				for (int i=0; i<10; i++) 
					col[i]=ECharrArray;
				col[0]  = ECharrArray;
				col[1]  = CCharArray;
				col[2]  = GBCharArray;
				iTotalCol = 10;
			}
			break;
		default:
			{
				for (int count=0; count<6; count++)
					col[count] = Other;
			}
			break;
	}
	if (m_bShowQuoteList)
	{
		for (int iRow=0; iRow<quoteList.GetTL(); iRow++)
		{
			for (int iCol=0; iCol<iTotalCol; iCol++)
			{
				if (quoteList.GetQuote(iRow, iCol, yLen, pFV))
				{
					strOutput.Format("Data: Index: Ln: %d, Col: %d, Len: %d", iRow, iCol, yLen);
					LogMessage(strOutput);
					PrintQuote(col[iCol], yLen, pFV);
				}
			}
		}
	}
	else
	{
		for (int i=0; i<quoteList.GetNE(); i++)
		{
			if (quoteList.GetQuote(i, yLn, sFid, yLen, pFV))
			{
				strOutput.Format("Data: Index: %d, Ln: %d, Fd: %d, Len: %d", i, yLn, sFid, yLen);
				LogMessage(strOutput);
				PrintQuote(sFid, yLen, pFV);
			}
		}
	}

	strOutput.Format("Page: %d (0x%x), Fid: %d(QuoteList), Total: %d, NE: %d", 
					 m_lPageNo, m_lPageNo, GetCurFidCode(), quoteList.GetTL(), quoteList.GetNE());
	LogMessageWithBlankLine(strOutput);

}
// -------------------------------------------------------------------------
void CDTFidIterator::PrintQuote(int iFidType, unsigned char& yLen, void*& pFV)
{
	switch(iFidType)
	{
	case DateYear:
		{
			BDateYear dy((unsigned char*)pFV);	
			PrintBDateYear(dy)	;
		}
		break;
	case ULong:
		{
			PrintULong(*(unsigned long*)pFV)	;
		}
		break;
	case UFloat:
		{
			PrintFloat(*(float*)pFV)	;
		}
		break;
	case CCharArray:
	case ECharrArray:
		{
			CharArray CA((unsigned char*)pFV);
			PrintCharArray(CA)	;
		}
		break;
	case GBCharArray:
		{
			CharArray CA((unsigned char*)pFV);
			PrintGBCharArray(CA)	;
		}
		break;
	case UChar:
		{
			PrintChar(*(char*)pFV)	;
		}
	   break;                
	case UDouble:
		{
			PrintUDouble(*(double*)pFV);
		}
		break;
	}
}

// -------------------------------------------------------------------------
void CDTFidIterator::PrintQuote(short& sFid, unsigned char& yLen, void*& pFV)
{
	fidDatabase fDB;

	switch (fDB.GetFidType(sFid))
	{
		case NGN_CHAR:
			PrintChar(*(char*)pFV)	;
			break;
		case NGN_UNICODE:
			PrintUnicode(*(WCHAR*)pFV)	;
			break;
		case NGN_SHORT:
			PrintShort(*(short*)pFV)	;
			break;
		case NGN_USHORT:
			PrintUShort(*(unsigned short*)pFV)	;
			break;
		case NGN_LONG:
			PrintLong(*(long*)pFV)	;
			break;
		case NGN_ULONG:
			PrintULong(*(unsigned long*)pFV)	;
			break;
		case NGN_FLOAT:
			PrintFloat(*(float*)pFV)	;
			break;
		case NGN_UFLOAT:
			PrintUFloat(*(float*)pFV)	;
			break;
		case NGN_DOUBLE:
			PrintDouble(*(double*)pFV)	;
			break;
		case NGN_UDOUBLE:
			PrintUDouble(*(double*)pFV);
			break;
		case NGN_CHARARRAY:
			{
				CharArray CA((unsigned char*)pFV);
				PrintCharArray(CA)	;
			}
			break;
		case NGN_MCHARARRAY:
			PrintMCharArray(*(MCharArray*)pFV)	;
			break;
		case NGN_BDATEYEAR:
			{
				BDateYear dy((unsigned char*)pFV);	
				PrintBDateYear(dy)	;
			}
			break;
		case NGN_BDATEMONTH:
			PrintBDateMonth(*(BDateMonth*)pFV)	;
			break;
		case NGN_BTIMEMIN:
			PrintBTimeMin(*(BTimeMin*)pFV)	;
			break;
		case NGN_BTIMESEC:
			PrintBTimeSec(*(BTimeSec*)pFV)	;
			break;
		case NGN_BTIMERANGES:
			PrintBTimeRanges(*(BTimeRanges*)pFV)	;
			break;
		case NGN_TRAN:
			PrintTran(*(Tran*)pFV)	;
			break;
		case NGN_TXN:
			PrintTxn(*(Txn*)pFV)	;
			break;
		case NGN_SHRORDLST:
			PrintShrOrdLst(*(ShrOrdLst*)pFV)	;
			break;
		case NGN_PRCVOL:
			PrintPrcVol(*(PrcVol*)pFV)	;
			break;
		case NGN_BKRQUE:
			PrintBkrQue(*(BkrQue*)pFV)	;
			break;
		case NGN_LINK:
			PrintLink(*(Link*)pFV)	;
			break;
		case NGN_WLINK:
			PrintWlink(*(WLink*)pFV)	;
			break;
		case NGN_YLABEL:
			PrintYLabel(*(YLabel*)pFV)	;
			break;
		case NGN_XMTCLABEL:
			PrintXMtcLabel(*(XMtcLabel*)pFV)	;
			break;
		case NGN_XDTCLABEL:
			PrintXDtcLabel(*(XDtcLabel*)pFV)	;
			break;
		case NGN_MTCPOINTS:
			PrintMtcPoints(*(MtcPoints*)pFV)	;
			break;
		case NGN_MTCVAL:
			PrintMtcVal(*(MtcVal*)pFV)	;
			break;
		case NGN_DTCPOINTS:
			PrintDtcPoints(*(DtcPoints*)pFV)	;
			break;
		case NGN_HISVAL:
			PrintHisVal(*(HisVal*)pFV)	;
			break;
		case NGN_FIELDLIST:
			PrintFieldList(*(FieldList*)pFV)	;
			break;
		case NGN_FIELDLABEL:
			PrintFieldLabel(*(FieldLabel*)pFV)	;
			break;
		case NGN_INFOMSG:
			PrintInfoMsg(*(InfoMsg*)pFV)	;
			break;
		case NGN_HEADLINE:
			PrintHeadLine(*(HeadLine*)pFV)	;
			break;
		case NGN_HLINK:
			PrintHLink(*(HLink*)pFV)	;
			break;
		case NGN_MSHRORDLST:
			PrintMShrOrdLst(*(MShrOrdLst*)pFV)	;
			break;
		case NGN_SPRTBL:
			PrintSprTbl(*(SprTbl*)pFV)	;
			break;
		case NGN_QUOTELIST:
			PrintQuoteList(*(QuoteList*)pFV)	;
			break;
		case NGN_MTCTICKS:
			PrintMTCTicks(*(MTCTicks*)pFV)	;
			break;
		case NGN_MTCV:
			PrintMTCV(*(MTCV*)pFV)	;
			break;
		case NGN_YHEADLINE:
			PrintYHeadLine(*(YHeadLine*)pFV)	;
			break;
		case NGN_OBKRQUE:
			PrintOBkrQue(*(OBkrQue*)pFV)	;
			break;
		case NGN_KMULTIPLE:
			PrintKMultiple(*(long*)pFV)	;
			break;
		case NGN_10KMULTIPLE:
			Print10KMultiple(*(long*)pFV)	;
			break;
		case NGN_BQUEVOSN:
			PrintBQueVOSN(*(BQueVOSN*)pFV)	;
			break;
		case NGN_MTCPTS:
			PrintMTCPTS(*(MTCPTS*)pFV)	;
			break;
		case NGN_PRCVOLWSEQ:
			PrintPrcVolwSeq(*(PrcVolwSeq*)pFV)	;
			break;
		default:
			break;
	}
}
// -------------------------------------------------------------------------
// Process Type D40 - MTCTicks
bool CDTFidIterator::ProcessMTCTicks()
{
	MTCTicks mtcTicks;
	if (!m_pMsgObj->GetFid(m_sFidNo, mtcTicks))
		return FALSE;
	else
	{
		PrintMTCTicks(mtcTicks);
		return true;
	}
}

// -------------------------------------------------------------------------
// Print Type D40 - MTCTicks
void CDTFidIterator::PrintMTCTicks(MTCTicks& mtcTicks)
{
	CString strOutput;

	short sOpen, sHigh, sLow, sClose;
	for (int i=0; i<mtcTicks.GetNe(); i++)
	{
		if (mtcTicks.GetPoint(i, sOpen, sHigh, sLow, sClose))
		{
			strOutput.Format("Data: Index: %d, Open: %d, High: %d, Low: %d, Close: %d", i, sOpen, sHigh, sLow, sClose);
			LogMessage(strOutput);
		}
	}
	strOutput.Format("Page: %d (0x%x), Fid: %d(MTCTicks), TL: %u, NE: %u, Pos: %u", 
					 m_lPageNo, m_lPageNo, GetCurFidCode(), mtcTicks.GetTL(), mtcTicks.GetNe(), mtcTicks.GetPos());

	LogMessageWithBlankLine(strOutput);
}

// -------------------------------------------------------------------------
// Process Type D41 - MTCV
bool CDTFidIterator::ProcessMTCV()
{
	MTCV mtcv;
	if (!m_pMsgObj->GetFid(m_sFidNo, mtcv))
		return FALSE;
	else
	{
		PrintMTCV(mtcv);
		return true;
	}
}
// -------------------------------------------------------------------------
// Print Type D41 - MTCV
void CDTFidIterator::PrintMTCV(MTCV& mtcv)
{
	CString strOutput;

	short sOpen, sHigh, sLow, sClose;
	double dVol;
	for (int i=0; i<mtcv.GetNe(); i++)
	{
		if (mtcv.GetPoint (i, sOpen, sHigh, sLow, sClose, dVol))
		{
			strOutput.Format("Data: Index: %d, Open: %d, High: %d, Low: %d, Close: %d, Vol: %g", 
							  i, sOpen, sHigh, sLow, sClose, dVol);
			LogMessage(strOutput);
		}
	}
	strOutput.Format("Page: %d (0x%x), Fid: %d(MTCV), TL: %u, NE: %u, Pos: %u", 
					 m_lPageNo, m_lPageNo, GetCurFidCode(), mtcv.GetTL(), mtcv.GetNe(), mtcv.GetPos());
	LogMessageWithBlankLine(strOutput);
}

// -------------------------------------------------------------------------
// Process Type D42 - YHeadLine
bool CDTFidIterator::ProcessYHeadLine()
{
	YHeadLine yheadline;
	if (!m_pMsgObj->GetFid(m_sFidNo, yheadline))
		return FALSE;
	else
	{
		PrintYHeadLine(yheadline);
		return true;
	}
}

// -------------------------------------------------------------------------
// Print Type D42 - YHeadLine
void CDTFidIterator::PrintYHeadLine(YHeadLine& yheadline)
{
	unsigned long nLn = 0;
	BDateYear yr;
	BTimeSec tm;
	byte byType;
	char pcLg[4] = "";
	unsigned long ulPg;
	char pcPNAC[20] = "";
	char pcTopicCode[1024] = "";
	char pcString[257] = "";
	byte byNe = 0;
	unsigned long pulLong[1024];
	memset(pulLong, 0, 1024*4);

	yheadline.GetHeadLine(nLn, yr, tm, byType, pcLg, ulPg, pcPNAC, pcTopicCode, byNe, pulLong, pcString);
	CString strOutput;

	strOutput.Format("TopicCode: %s", pcTopicCode);
	LogMessage(strOutput);
	for (int i =0; i<byNe; i++)
	{
		strOutput.Format("Data: Page No. %d: %d", i, pulLong[i]);
		LogMessage(strOutput);
	}
	strOutput.Format("Data: String: %s", pcString);
	LogMessage(strOutput);
	strOutput.Format("Type: %d, Lang. ID: %s, PageNo.: %d, PNAC: %s", byType, pcLg, ulPg, pcPNAC);
	LogMessage(strOutput);
	strOutput.Format("Page: %d (0x%x), Fid: %d(YHeadLine), Data: Line No: %d, Date: %d-%d-%d, Time: %d:%d:%d", 
					 m_lPageNo, m_lPageNo, GetCurFidCode(), nLn, yr.GetYear(), yr.GetMonth(), yr.GetDay(), tm.GetHH(), tm.GetMIN(), tm.GetSS());
	LogMessageWithBlankLine(strOutput);
}

// -------------------------------------------------------------------------
// Process D43 - OBkrQue
bool CDTFidIterator::ProcessOBkrQue()
{
	OBkrQue obkrque;
	if (!m_pMsgObj->GetFid(m_sFidNo, obkrque)) 
		return FALSE;
	else
	{
		PrintOBkrQue(obkrque);
		return true;
	}
}

// -------------------------------------------------------------------------
// Print D43 - OBkrQue
void CDTFidIterator::PrintOBkrQue(OBkrQue& obkrque)
{
	CString strOutput;
	byte byA = 0;
	unsigned short unB = 0;
	unsigned short unC = 0;
	unsigned long  ulOSN = 0;

	for (int i=0; i<obkrque.GetNe(); i++)
	{
		if (obkrque.GetBroker(i, byA, unB, unC, ulOSN))
		{
			strOutput.Format("Data: Index: %d, Type: %d, Broker No: %d, Spare: %d, OSN: %d",i, byA, unB, unC, ulOSN);
			LogMessage(strOutput);
		}
	}
	strOutput.Format("Page: %d (0x%x), Fid: %d(OBkrQue), Data: NE: %d", 
					 m_lPageNo, m_lPageNo, GetCurFidCode(), obkrque.GetNe());
	LogMessageWithBlankLine(strOutput);
}
// -------------------------------------------------------------------------
// Process Type D44 - KMultiple
bool CDTFidIterator::ProcessKMultiple()
{
	long l = 0;
	if (!m_pMsgObj->GetFid(m_sFidNo, l))
		return FALSE;
	else
	{
		PrintKMultiple(l);
		return true;
	}
}

// -------------------------------------------------------------------------
// Print Type D44 - KMultiple
void CDTFidIterator::PrintKMultiple(long& l)
{
	CString strOutput;
	strOutput.Format("Page: %d (0x%x), Fid: %d(KMultiple), Data: %f", m_lPageNo, m_lPageNo, GetCurFidCode(), l/1000.0);
	LogMessageWithBlankLine(strOutput);
}
// -------------------------------------------------------------------------
// Process Type D45 - 10KMultiple
bool CDTFidIterator::Process10KMultiple()
{
	long l = 0;
	if (!m_pMsgObj->GetFid(m_sFidNo, l))
		return FALSE;
	else
	{
		Print10KMultiple(l);
		return true;
	}
}
// -------------------------------------------------------------------------
// Print Type D45 - 10KMultiple
void CDTFidIterator::Print10KMultiple(long& l)
{

	CString strOutput;
	strOutput.Format("Page: %d (0x%x), Fid: %d(KMultiple), Data: %f", m_lPageNo, m_lPageNo, GetCurFidCode(), l/10000.0);
	LogMessageWithBlankLine(strOutput);
}


// -------------------------------------------------------------------------
// Process D46 - BQueVOSN
bool CDTFidIterator::ProcessBQueVOSN()
{
	BQueVOSN bquevosn;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), bquevosn))
		return FALSE;
	else
	{
		PrintBQueVOSN(bquevosn); 
		return true;
	}
}

// -------------------------------------------------------------------------
// Process D46 - BQueVOSN
void CDTFidIterator::PrintBQueVOSN(BQueVOSN& bquevosn)
{
	CString strOutput;
	byte byA = 0;
	unsigned short unB = 0;
	unsigned short unC = 0;
	unsigned long  ulOSN = 0; 

	for (int i=0; i<bquevosn.GetNe(); i++)
	{
		if (bquevosn.GetBroker(i, byA, unB, unC, ulOSN))
		{
			strOutput.Format("Data: Index: %d, Type: %d, Broker No: %d, Spare: %d, OSN: %d",i, byA, unB, unC, ulOSN);
			LogMessage(strOutput);
		}
	}
	strOutput.Format("Page: %d (0x%x), Fid: %d(BQueVOSN), Data: NE: %d", 
					 m_lPageNo, m_lPageNo, GetCurFidCode(), bquevosn.GetNe());
	LogMessageWithBlankLine(strOutput);
}



// -------------------------------------------------------------------------
// Process D47 - MTCPTS
bool CDTFidIterator::ProcessMTCPTS()
{
	MTCPTS mtcPts;
	if (!m_pMsgObj->GetFid(m_sFidNo, mtcPts))
		return FALSE;
	else
	{
		PrintMTCPTS(mtcPts);
		return true;
	}
}
// -------------------------------------------------------------------------
// Print D47 - MTCPTS
void CDTFidIterator::PrintMTCPTS(MTCPTS& mtcPts)
{
	CString strOutput;
	byte byPts = 0;
	for (int i=0; i<mtcPts.GetNe(); i++)
	{
		if (mtcPts.GetPoint(i, byPts))
		{
			strOutput.Format("Data: Index: %d, Points: %d",i, byPts);
			LogMessage(strOutput);
		}
	}
	strOutput.Format("Page: %d (0x%x), Fid: %d(MTCPTS), Data: Total: %d, NE: %d, Pos: %d", 
					 m_lPageNo, m_lPageNo, GetCurFidCode(), mtcPts.GetTL(), mtcPts.GetNe(), mtcPts.GetPos());
	LogMessageWithBlankLine(strOutput);
}

// -------------------------------------------------------------------------
// Process D48 - MTCF
bool CDTFidIterator::ProcessMTCF()
{
	MTCF mtcf;
	if (!m_pMsgObj->GetFid(m_sFidNo, mtcf))
		return FALSE;
	else
	{
		PrintMTCF(mtcf);
		return true;
	}
}
// -------------------------------------------------------------------------
// Print D48 - MTCF
void CDTFidIterator::PrintMTCF(MTCF& mtcf)
{
	CString strOutput;
	float Open, High, Low, Close;
	double Vol;		
	for (int i=0; i<mtcf.GetNe(); i++)
	{
		if (mtcf.GetPoint(i, Open, High, Low, Close, Vol))
		{
			strOutput.Format("Data: Index: %d, Open: %.3f, High: %.3f, Low: %.3f, Close: %.3f, Vol: %.3f",i, Open, High, Low, Close, Vol);
			LogMessage(strOutput);
		}
	}
	strOutput.Format("Page: %d (0x%x), Fid: %d(MTCF), Data: Total: %d, NE: %d, Pos: %d", 
					 m_lPageNo, m_lPageNo, GetCurFidCode(), mtcf.GetTL(), mtcf.GetNe(), mtcf.GetPos());
	LogMessageWithBlankLine(strOutput);
}

// -------------------------------------------------------------------------
// Process D49 - PrcVolwSeq 
bool CDTFidIterator::ProcessPrcVolwSeq()
{
	PrcVolwSeq prc;
	if (!m_pMsgObj->GetFid(m_sFidNo, prc))
		return FALSE;
	else
	{
		PrintPrcVolwSeq(prc);
		return true;
	}
}
// -------------------------------------------------------------------------
// Print D49 - PrcVolwSeq 
void CDTFidIterator::PrintPrcVolwSeq(PrcVolwSeq& prc)
{
	CString strOutput;

	float fPr = 0;
	double dVl = 0;

	for (int i=0; i<prc.GetNe(); i++)
	{
		if (prc.GetPrcVol(i, fPr, dVl))
		{
			strOutput.Format("Data: Index: %d, Price: %f, Volumn: %f", i, fPr, dVl);
				LogMessage(strOutput);
		}
	}
	strOutput.Format("Page: %d (0x%x), Fid: %d(PrcVolwSeq), Data: NE: %d Pos: %d, TL: %d, Seq: %d", 
					 m_lPageNo, m_lPageNo, GetCurFidCode(), prc.GetNe(), prc.GetPos(), prc.GetTL(), prc.GetSq());
	LogMessageWithBlankLine(strOutput);
}
