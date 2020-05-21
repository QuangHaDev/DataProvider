/////////////////////////////////////////////////////////////////////////////
//          File:: TraceParsedFidIterator.cpp
// Create Author:: Sammy Ha
// Create   Date:: 07 January 2001
//
//   Description:: The implementation file of the TraceParsedFidIterator.h
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

#include "ParsedFidIterator.h"
#include "TraceParsedFidIterator.h"
//#include "ApplManClient.h"

// -------------------------------------------------------------------------
// Constructor
/*ParsedFidIterator::ParsedFidIterator(const byte* pbyMsg)
{
	m_pMsgObj = NULL;
	m_pMsgObj = new MsgObject(pbyMsg);
}*/

// -------------------------------------------------------------------------
// Destructor
/*ParsedFidIterator::~ParsedFidIterator()
{
	if (m_pMsgObj)
	{
		delete[] m_pMsgObj;
		m_pMsgObj = NULL;
	}
}*/

// -------------------------------------------------------------------------
// Move first
/*void ParsedFidIterator::First()
{
	m_lCurParsedFid = 0;
	m_nFidType = m_pMsgObj->GetFidType(m_lCurParsedFid, m_nFid);
}

// -------------------------------------------------------------------------
// Move the counter by one
void ParsedFidIterator::Next()
{
	m_lCurParsedFid++;
	m_nFidType = m_pMsgObj->GetFidType(m_lCurParsedFid, m_nFid);
}
*/
/*
// -------------------------------------------------------------------------
// check whether the iterator reach the end
bool ParsedFidIterator::IsEnd() const
{
/*	short nFidsCount = m_pMsgObj->GetFidsCount();
	if (nFidsCount > 0)
		return m_lCurParsedFid >= nFidsCount;

	return FALSE;
*/
/*	if (m_nFidType == 0)
		return TRUE;
	return FALSE;
}
*/

/*
// -------------------------------------------------------------------------
// Process current fid
bool ParsedFidIterator::ProcessFid()
{
	bool retVal = false;

	switch (m_nFidType)
	{
		case NGN_CHAR:
			retVal = ProcessChar()	;
			break;
		case NGN_UNICODE:
			retVal = ProcessUnicode()	;
			break;
		case NGN_SHORT:
			retVal = ProcessShort()	;
			break;
		case NGN_USHORT:
			retVal = ProcessUShort()	;
			break;
		case NGN_LONG:
			retVal = ProcessLong()	;
			break;
		case NGN_ULONG:
			retVal = ProcessULong()	;
			break;
		case NGN_FLOAT:
			retVal = ProcessFloat()	;
			break;
		case NGN_UFLOAT:
			retVal = ProcessUFloat()	;
			break;
		case NGN_DOUBLE:
			retVal = ProcessDouble()	;
			break;
		case NGN_UDOUBLE:
			retVal = ProcessUDouble()	;
			break;
		case NGN_CHARARRAY:
			retVal = ProcessCharArray()	;
			break;
		case NGN_MCHARARRAY:
			retVal = ProcessMCharArray()	;
			break;
		case NGN_BDATEYEAR:
			retVal = ProcessBDateYear()	;
			break;
		case NGN_BDATEMONTH:
			retVal = ProcessBDateMonth()	;
			break;
		case NGN_BTIMEMIN:
			retVal = ProcessBTimeMin()	;
			break;
		case NGN_BTIMESEC:
			retVal = ProcessBTimeSec()	;
			break;
		case NGN_BTIMERANGES:
			retVal = ProcessBTimeRanges()	;
			break;
		case NGN_TRAN:
			retVal = ProcessTran()	;
			break;
		case NGN_TXN:
			retVal = ProcessTxn()	;
			break;
		case NGN_SHRORDLST:
			retVal = ProcessShrOrdLst()	;
			break;
		case NGN_PRCVOL:
			retVal = ProcessPrcVol()	;
			break;
		case NGN_BKRQUE:
			retVal = ProcessBkrQue()	;
			break;
		case NGN_LINK:
			retVal = ProcessLink()	;
			break;
		case NGN_WLINK:
			retVal = ProcessWlink()	;
			break;
		case NGN_YLABEL:
			retVal = ProcessYLabel()	;
			break;
		case NGN_XMTCLABEL:
			retVal = ProcessXMtcLabel()	;
			break;
		case NGN_XDTCLABEL:
			retVal = ProcessXDtcLabel()	;
			break;
		case NGN_MTCPOINTS:
			retVal = ProcessMtcPoints()	;
			break;
		case NGN_MTCVAL:
			retVal = ProcessMtcVal()	;
			break;
		case NGN_DTCPOINTS:
			retVal = ProcessDtcPoints()	;
			break;
		case NGN_HISVAL:
			retVal = ProcessHisVal()	;
			break;
		case NGN_FIELDLIST:
			retVal = ProcessFieldList()	;
			break;
		case NGN_FIELDLABEL:
			retVal = ProcessFieldLabel()	;
			break;
		case NGN_INFOMSG:
			retVal = ProcessInfoMsg()	;
			break;
		case NGN_HEADLINE:
			retVal = ProcessHeadLine()	;
			break;
		case NGN_HLINK:
			retVal = ProcessHLink()	;
			break;
		case NGN_MSHRORDLST:
			retVal = ProcessMShrOrdLst()	;
			break;
		default:
			break;
	}

	return retVal;
}
*/

// -------------------------------------------------------------------------
// Process D1 - Char
bool TraceParsedFidIterator::ProcessChar()
{
	char c = 0;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), c))
		return FALSE;

	TRACE("\n Char:		Fid %d:	  Data : %d", GetCurFidCode(), c);
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D2 - Unicode
bool TraceParsedFidIterator::ProcessUnicode()
{
	WCHAR c;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), c))
		return FALSE;

	TRACE("\n Unicode:	Fid %d:	  Data : %d", GetCurFidCode(), c);
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D3 - short
bool TraceParsedFidIterator::ProcessShort()
{
	short sh = 0;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), sh))
		return FALSE;

	TRACE("\n Short:		Fid %d:	  Data : %d", GetCurFidCode(), sh);
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D4 - unsigned short
bool TraceParsedFidIterator::ProcessUShort()
{
	unsigned short ush = 0;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), ush))
		return FALSE;

	TRACE("\n UShort:	Fid %d:	  Data : %d", GetCurFidCode(), ush);
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D5 - long
bool TraceParsedFidIterator::ProcessLong()
{
	long l = 0;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), l))
		return FALSE;

	TRACE("\n Long:		Fid %d:	  Data : %d", GetCurFidCode(), l);
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D6 - unsigned long
bool TraceParsedFidIterator::ProcessULong()
{
	unsigned long ul = 0;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), ul))
		return FALSE;

	TRACE("\n ULong:		Fid %d:	  Data : %d", GetCurFidCode(), ul);
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D7 & D8 - float
bool TraceParsedFidIterator::ProcessFloat()
{
	float f = 0;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), f))
		return FALSE;

	TRACE("\n Float:		Fid %d:	  Data : %f", GetCurFidCode(),  f);
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D8 - unsigned float
bool TraceParsedFidIterator::ProcessUFloat()
{
	float f = 0;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), f))
		return FALSE;
//	f = abs(f);

	TRACE("\n UFloat:		Fid %d:	  Data : %f", GetCurFidCode(),  f);
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D9 & D10 - double
bool TraceParsedFidIterator::ProcessDouble()
{
	double d = 0;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), d))
		return FALSE;

	TRACE("\n Double:	Fid %d:	  Data : %f", GetCurFidCode(),  d);
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D10 - unsigned double
bool TraceParsedFidIterator::ProcessUDouble()
{
	double d = 0;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), d))
		return FALSE;
//	d = abs(d);

	TRACE("\n UDouble:	Fid %d:	  Data : %f", GetCurFidCode(),  d);
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D11 - CharArray
bool TraceParsedFidIterator::ProcessCharArray()
{
	CharArray ca;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), ca))
		return FALSE;

	TRACE("\n-----------------------------------------------------------------------------");
	TRACE("\n CharArray:	Fid %d:	  Data : Len : %d	Sq : %d		Pos : %d	String : %s \n\n", 
		GetCurFidCode(),
		ca.GetLen(),
		ca.GetSq(),
		ca.GetPos(),
		ca.GetString());
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D12 - MCharArray
bool TraceParsedFidIterator::ProcessMCharArray()
{
	MCharArray mca2;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), mca2))
		return FALSE;

	TRACE("\n-----------------------------------------------------------------------------");
	TRACE("\nFid: %d	MCharArray Total Line : %d	NE : %d\n", GetCurFidCode(), mca2.GetTL(), mca2.GetNe());
	for (int i=0; i<mca2.GetNe(); i++)
	{
		TRACE("\n%d CharArray Length : %d	CharArray Pos : %d	CharArray Sq : %d\n", mca2.GetLn(i), mca2.GetCA(i)->GetLen(), mca2.GetCA(i)->GetPos(), mca2.GetCA(i)->GetSq());
		TRACE("CharArray String : %s\n", mca2.GetCA(i)->GetString());
		TRACE("CharArray Byte String : %s\n\n", (char*) mca2.GetCA(i)->GetData());
	}
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D14 - BDateMonth
bool TraceParsedFidIterator::ProcessBDateMonth()
{
	BDateMonth dm;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), dm))
		return FALSE;

	TRACE("\n-----------------------------------------------------------------------------");
	TRACE("\nFid: %d	BDateMonth Day : %d	BdateMonth Month : %d\n", GetCurFidCode(), dm.GetDay(), dm.GetMonth());
	TRACE("BDateMonth Byte String : %.2s\n\n", dm.GetData());
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D13 - BDateYear
bool TraceParsedFidIterator::ProcessBDateYear()
{
	BDateYear dy;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), dy))
		return FALSE;

	TRACE("\n-----------------------------------------------------------------------------");
	TRACE("\nFid: %d	BDateYear Day : %d	BDateYear Month : %d	BDateYear Year : %d\n", GetCurFidCode(), dy.GetDay(), dy.GetMonth(), dy.GetYear());
	TRACE("BDateYear Byte String : %.4s\n\n", dy.GetData());
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D15 - BTimeMin
bool TraceParsedFidIterator::ProcessBTimeMin()
{
	BTimeMin tm;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), tm))
		return FALSE;

	TRACE("\n-----------------------------------------------------------------------------");
	TRACE("\nFid: %d	BTimeMin Hour : %d	BTimeMin Minute : %d\n", GetCurFidCode(), tm.GetHH(), tm.GetMIN());
	TRACE("BTimeMin Byte String : %.2s\n\n", tm.GetData());
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D16 - BTimeSec
bool TraceParsedFidIterator::ProcessBTimeSec()
{
	BTimeSec ts;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), ts))
		return FALSE;

	TRACE("\n-----------------------------------------------------------------------------");
	TRACE("\nFid: %d	BTimeSec Hour : %d	BTimeSec Minute : %d	BTimeSec Second : %d\n", GetCurFidCode(), ts.GetHH(), ts.GetMIN(), ts.GetSS());
	TRACE("BTimeSec Byte String : %.3s\n\n", ts.GetData());
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D17 - BTimeRanges
bool TraceParsedFidIterator::ProcessBTimeRanges()
{
	BTimeRanges tr;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), tr))
		return FALSE;

	TRACE("\n-----------------------------------------------------------------------------");
	TRACE("\nFid: %d	BTimeRanges NE : %d\n", GetCurFidCode(), tr.GetNe());
	BTimeMin s_tm, e_tm;
	for (int i=0; i<tr.GetNe(); i++)
	{
		if (tr.GetRange(i, s_tm, e_tm))
			TRACE("Start Time : %d:%d		End Time : %d:%d\n", s_tm.GetHH(), s_tm.GetMIN(), e_tm.GetHH(), e_tm.GetMIN());
	}
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D18 - Tran
bool TraceParsedFidIterator::ProcessTran()
{
	Tran tran;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), tran))
		return FALSE;

	TRACE("\n-----------------------------------------------------------------------------");
	TRACE("\nFid: %d	Tran NE : %d\n", GetCurFidCode(), tran.GetNe());
	unsigned long sq = 0;
	BTimeMin tm;
	double vl = 0;
	float nm = 0;
	char fg = 0;

	for (int i=0; i<tran.GetNe(); i++)
	{
		if (tran.GetTranElement(i, sq, tm, vl, nm, (byte&)fg))
			TRACE("Sequence No : %d	Time : %d:%d	Vol: %f	Price: %f	Flag: %c\n", sq, tm.GetHH(), tm.GetMIN(), vl, nm, fg);
	}
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D19 - Txn
bool TraceParsedFidIterator::ProcessTxn()
{
	Txn txn;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), txn))
		return FALSE;

	TRACE("\n-----------------------------------------------------------------------------");
	TRACE("\nFid: %d	Txn Total: %d	Txn NE : %d	Txn Sq: %d\n", GetCurFidCode(), txn.GetTL(), txn.GetNe(), txn.GetSq());
	BTimeMin tm;
	double vl = 0;
	float nm = 0;
	char fg = 0;

	for (int i=0; i<txn.GetNe(); i++)
	{
		if (txn.GetTranElement(i, tm, vl, nm, (byte&)fg))
			TRACE("Time : %d:%d	Vol: %f	Price: %f	Flag: %c\n", tm.GetHH(), tm.GetMIN(), vl, nm, fg);
	}
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D20 - ShrOrdLst
bool TraceParsedFidIterator::ProcessShrOrdLst()
{
	ShrOrdLst sol;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), sol))
		return FALSE;

	TRACE("\n-----------------------------------------------------------------------------");
	TRACE("\nFid: %d	 Shares: %s	 Orders: %d\n", GetCurFidCode(), sol.GetShStr(), sol.GetOr());
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D21 - PrcVol
bool TraceParsedFidIterator::ProcessPrcVol()
{
	PrcVol prc;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), prc))
		return FALSE;

	TRACE("\n-----------------------------------------------------------------------------");
	TRACE("\nFid: %d	PrcAskNuBid NE : %d\n", GetCurFidCode(), prc.GetNe());
	float fPr = 0;
	double dVl = 0;

	for (int i=0; i<prc.GetNe(); i++)
	{
		if (prc.GetPrcVol(i, fPr, dVl))
			TRACE("Price: %f	Volumn: %f\n", fPr, dVl);
	}
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D22 - BkrQue
bool TraceParsedFidIterator::ProcessBkrQue()
{
	BkrQue bkrque;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), bkrque))
		return FALSE;

	TRACE("\n-----------------------------------------------------------------------------");
	TRACE("\nFid: %d	BkrQue NE : %d\n", GetCurFidCode(), bkrque.GetNe());
	byte byA = 0;
	unsigned short unB = 0;
	unsigned short unC = 0;

	for (int i=0; i<bkrque.GetNe(); i++)
	{
		if (bkrque.GetBroker(i, byA, unB, unC))
			TRACE("Type: %d	Broker Number: %d	Spare : %d\n",  byA, unB, unC);
	}
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D23 - Link
bool TraceParsedFidIterator::ProcessLink()
{
	Link link;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), link))
		return FALSE;

	TRACE("\n-----------------------------------------------------------------------------");
	TRACE("\nFid: %d	Link Total: %d	Link NE : %d	Link Sq: %d\n", GetCurFidCode(), link.GetTL(), link.GetNe(), link.GetPos());
	unsigned long ulLink;

	for (int i=0; i<link.GetNe(); i++)
	{
		if (link.GetLink(i, ulLink))
			TRACE("Link : %d\n", ulLink);
	}
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D24 - Wlink
bool TraceParsedFidIterator::ProcessWlink()
{
	WLink wlink;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), wlink))
		return FALSE;

	TRACE("\n-----------------------------------------------------------------------------");
	TRACE("\nFid: %d	WLink Total: %d	WLink NE : %d	WLink Sq: %d\n", GetCurFidCode(), wlink.GetTL(), wlink.GetNe(), wlink.GetPos());
	unsigned long ulLink;
	unsigned short unCur;
	float fWt;
	unsigned short unCu;

	for (int i=0; i<wlink.GetNe(); i++)
	{
		if (wlink.GetWLink(i, ulLink, unCur, fWt, unCu))
			TRACE("PgNo. : %d	Cur: %d	Wt: %f	Cu: %d\n", ulLink, unCur, fWt, unCu);
	}
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D25 - YLabel
bool TraceParsedFidIterator::ProcessYLabel()
{
	YLabel ylabel;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), ylabel))
		return FALSE;

	TRACE("\n-----------------------------------------------------------------------------");
	TRACE("\nFid: %d	YLabel High: %f	YLabel Mid : %f	YLabel Low: %f\n", GetCurFidCode(), ylabel.GetHigh(), ylabel.GetMid(), ylabel.GetLow());
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D26 - XMtcLabel
bool TraceParsedFidIterator::ProcessXMtcLabel()
{
	XMtcLabel xmtcLbl;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), xmtcLbl))
		return FALSE;

	TRACE("\n-----------------------------------------------------------------------------");
	TRACE("\nFid: %d	XMtcLabel NE : %d\n", GetCurFidCode(), xmtcLbl.GetNe());
	unsigned short nPos = 0;
	BTimeMin tm;

	for (int i=0; i<xmtcLbl.GetNe(); i++)
	{
		if (xmtcLbl.GetTime(i, nPos, tm))
			TRACE("Position : %d	Time: %d:%d\n", nPos, tm.GetHH(), tm.GetMIN());
	}
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D27 - XDtcLabel
bool TraceParsedFidIterator::ProcessXDtcLabel()
{
	XDtcLabel xdtcLbl;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), xdtcLbl))
		return FALSE;

	TRACE("\n-----------------------------------------------------------------------------");
	TRACE("\nFid: %d	XDtcLabel NE : %d\n", GetCurFidCode(), xdtcLbl.GetNe());
	unsigned short nPos = 0;
	BDateMonth dt;

	for (int i=0; i<xdtcLbl.GetNe(); i++)
	{
		if (xdtcLbl.GetDate(i, nPos, dt))
			TRACE("Position : %d	Date: %d-%d\n", nPos, dt.GetMonth(), dt.GetDay());
	}
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D28 - MtcPoints
bool TraceParsedFidIterator::ProcessMtcPoints()
{
	MtcPoints mtcPts;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), mtcPts))
		return FALSE;

	TRACE("\n-----------------------------------------------------------------------------");
	TRACE("\nFid: %d	MtcPoints Total: %d	MtcPoints NE : %d	MtcPoints Pos : %d\n", GetCurFidCode(), mtcPts.GetTL(), mtcPts.GetNe(), mtcPts.GetPos());
	byte byPts = 0;

	for (int i=0; i<mtcPts.GetNe(); i++)
	{
		if (mtcPts.GetPoint(i, byPts))
			TRACE("Points : %d	\n", byPts);
	}
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D29 - MtcVal
bool TraceParsedFidIterator::ProcessMtcVal()
{
	MtcVal mtcval;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), mtcval))
		return FALSE;

	TRACE("\n-----------------------------------------------------------------------------");
	TRACE("\nFid: %d	MtcVal Total: %d	MtcVal NE : %d	MtcVal Pos : %d\n", GetCurFidCode(), mtcval.GetTL(), mtcval.GetNe(), mtcval.GetPos());
	float fVal = 0;

	for (int i=0; i<mtcval.GetNe(); i++)
	{
		if (mtcval.GetVal(i, fVal))
			TRACE("Actual Value : %f	\n", fVal);
	}
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D30 - DtcPoints
bool TraceParsedFidIterator::ProcessDtcPoints()
{
	DtcPoints dtcPts;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), dtcPts))
		return FALSE;

	TRACE("\n-----------------------------------------------------------------------------");
	TRACE("\nFid: %d	DtcPoints NE : %d\n", GetCurFidCode(), dtcPts.GetNe());
	byte byHt = 0;
	byte byLw = 0;
	byte byCls = 0;

	for (int i=0; i<dtcPts.GetNe(); i++)
	{
		if (dtcPts.GetDtcElement(i, byHt, byLw, byCls))
			TRACE("High : %d	Low : %d	Close : %d\n", byHt, byLw, byCls);
	}
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D31 - HisVal
bool TraceParsedFidIterator::ProcessHisVal()
{
	HisVal hisval;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), hisval))
		return FALSE;
	BDateYear Dt;
	hisval.GetDt(Dt);

	TRACE("\n-----------------------------------------------------------------------------");
	TRACE("\nFid: %d	HisVal Total : %d	HisVal NE : %d	HisVal Date: %d-%d-%d\n", GetCurFidCode(), hisval.GetTL(), hisval.GetNe(), Dt.GetYear(), Dt.GetMonth(), Dt.GetDay());
	float fHt = 0;
	float fLw = 0;
	float fCls = 0;
	unsigned long ulVol = 0;

	for (int i=0; i<hisval.GetNe(); i++)
	{
		if (hisval.GetVal(i, fHt, fLw, fCls, ulVol))
			TRACE("High : %f	Low : %f	Close : %f	Vol : %d\n", fHt, fLw, fCls, ulVol);
	}
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D32 - FieldList
bool TraceParsedFidIterator::ProcessFieldList()
{
	FieldList fldLst;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), fldLst))
		return FALSE;

	TRACE("\n-----------------------------------------------------------------------------");
	TRACE("\nFid: %d	FieldList  NE : %d\n", GetCurFidCode(), fldLst.GetNe());
	unsigned short nFd;

	for (int i=0; i<fldLst.GetNe(); i++)
	{
		if (fldLst.GetField(i, nFd))
			TRACE("Field: %d\n", nFd);
	}
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D33 - FieldLabel
bool TraceParsedFidIterator::ProcessFieldLabel()
{
	FieldLabel fldLbl;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), fldLbl))
		return FALSE;

	TRACE("\n-----------------------------------------------------------------------------");
	TRACE("\nFid: %d	FieldLabel  NE : %d\n", GetCurFidCode(), fldLbl.GetNe());
	char c[256] = "";

	for (int i=0; i<fldLbl.GetNe(); i++)
	{
		if (fldLbl.GetLabel(i, c, 255))
			TRACE("Field: %s\n", c);
	}
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D34 - InfoMsg
bool TraceParsedFidIterator::ProcessInfoMsg()
{
	InfoMsg infomsg;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), infomsg))
		return FALSE;

	TRACE("\n-----------------------------------------------------------------------------");
	TRACE("\nFid: %d	InfoMsg Total : %d	InfoMsg NE: %d\n", GetCurFidCode(), infomsg.GetTL(), infomsg.GetNe());
	char c[256] = "";
	unsigned long ulPg = 0;
	unsigned short unFd = 0;

	for (int i=0; i<infomsg.GetNe(); i++)
	{
//		if (infomsg.GetLabel(i, c, 255) && infomsg.GetPg(i, ulPg) && infomsg.GetFd(i, unFd))
		if (infomsg.GetInfo(i, c, 255, ulPg, unFd))
			TRACE("Field: %s	Link: %d	Field Code: %d\n", c, ulPg, unFd);
	}
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D35 - HeadLine
bool TraceParsedFidIterator::ProcessHeadLine()
{
	HeadLine headline;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), headline))
		return FALSE;

	unsigned long nLn = 0;
	BDateMonth dt;
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

	headline.GetHeadLine(nLn, dt, tm, byType, pcLg, ulPg, pcPNAC, pcTopicCode, byNe, pulLong, pcString);
	TRACE("\n-----------------------------------------------------------------------------");
	TRACE("\nFid: %d	HeadLine Line Number: %d	Date : %d-%d	Time: %d:%d:%d\n", GetCurFidCode(), nLn, dt.GetMonth(), dt.GetDay(), tm.GetHH(), tm.GetMIN(), tm.GetSS());
	TRACE("Type: %d	 Lang. ID: %s	PageNo.: %d	PNAC: %s\n", byType, pcLg, ulPg, pcPNAC);
	TRACE("TopicCode: %s\n", pcTopicCode);
	for (int i =0; i<byNe; i++)
	{
		TRACE("Page No. %d: %d\n", i, pulLong[i]);
	}
	TRACE("String: %s\n", pcString);
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D36 - HLink
bool TraceParsedFidIterator::ProcessHLink()
{
	HLink hlink;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), hlink))
		return FALSE;

	TRACE("\n-----------------------------------------------------------------------------");
	TRACE("\nFid: %d	HLink Total : %d	HLink NE: %d	HLink Pos: %d\n", GetCurFidCode(), hlink.GetTL(), hlink.GetNe(), hlink.GetPos());
	unsigned long ulPg = 0;
	unsigned long unLn = 0;

	for (int i=0; i<hlink.GetNe(); i++)
	{
		if (hlink.GetHLink(i, ulPg, unLn))
			TRACE("Pg. No.: %d	Line No.: %d\n", ulPg, unLn);
	}
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D37 - MShrOrdLst
bool TraceParsedFidIterator::ProcessMShrOrdLst()
{
	MShrOrdLst mshr;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), mshr))
		return FALSE;

	TRACE("\n-----------------------------------------------------------------------------");
	TRACE("\nFid: %d	MShrOrdLst NE : %d\n", GetCurFidCode(), mshr.GetNe());
	ShrOrdLst shr;

	for (int i=0; i<mshr.GetNe(); i++)
	{
		if (mshr.GetShrOrdLst(i, shr))
			TRACE("\nFid: %d	 Shares: %s	 Orders: %d", GetCurFidCode(), shr.GetShStr(), shr.GetOr());
	}
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D38 - SprTbl
bool TraceParsedFidIterator::ProcessSprTbl()
{
	SprTbl sprtbl;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), sprtbl))
		return FALSE;

	TRACE("\n-----------------------------------------------------------------------------");
	TRACE("\nFid: %d	SprTbl Total : %d	SprTbl NE : %d\n", GetCurFidCode(), sprtbl.GetTL(), sprtbl.GetNe());
	float fPF = 0;
	float fPT = 0;
	float fSP = 0;

	for (int i=0; i<sprtbl.GetNe(); i++)
	{
		if (sprtbl.GetSprTbl(i, fPF, fPT, fSP))
			TRACE("PriceFrom : %f	PriceTo : %f	SprVal : %f\n", fPF, fPT, fSP);
	}
	return TRUE;
}

// -------------------------------------------------------------------------
// Process Type D39 - QuoteList
bool TraceParsedFidIterator::ProcessQuoteList()
{
	QuoteList quoteList;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), quoteList))
		return FALSE;

	TRACE("\n-----------------------------------------------------------------------------");
	TRACE("\nFid: %d	QL TL : %d	QL NE : %d\n", GetCurFidCode(), quoteList.GetTL(), quoteList.GetNE());

	byte yLn, yLen;
	short sFid;
	void* pFV;;
	for (int i=0; i<quoteList.GetNE(); i++)
	{
		if (quoteList.GetQuote (i, yLn, sFid, yLen, pFV))
		{
			TRACE ("%d. Ln: %d, Fd: %d, Len: %d\n", i, yLn, sFid, yLen);
		}
	}
	return TRUE;
}


// -------------------------------------------------------------------------
// Process Type D40 - MTCTicks
bool TraceParsedFidIterator::ProcessMTCTicks()
{
	MTCTicks mtcTicks;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), mtcTicks))
		return FALSE;

	TRACE("\n-----------------------------------------------------------------------------");
	TRACE ("\nTL: %u, NE: %u, Pos: %u\n", mtcTicks.GetTL(), mtcTicks.GetNe(), mtcTicks.GetPos());
	TRACE ("n.\tHigh\tLow\tClose\n");
	short Open, High, Low, Close;
	for (int i=0; i<mtcTicks.GetNe(); i++)
	{
		mtcTicks.GetPoint (i, Open, High, Low, Close);
		TRACE ("%u.\t%d\t%d\t%d\t%d\n", i, Open, High, Low, Close);
	}

	return TRUE;
}


// -------------------------------------------------------------------------
// Process Type D41 - MTCV
bool TraceParsedFidIterator::ProcessMTCV()
{
	MTCV mtcv;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), mtcv))
		return FALSE;

	TRACE("\n-----------------------------------------------------------------------------");
	TRACE ("\nTL: %u, NE: %u, Pos: %u\n", mtcv.GetTL(), mtcv.GetNe(), mtcv.GetPos());
	TRACE ("n.\tHigh\tLow\tClose\tVol\n");
	short Open, High, Low, Close;
	double Vol;
	for (int i=0; i<mtcv.GetNe(); i++)
	{
		mtcv.GetPoint (i, Open, High, Low, Close, Vol);
		TRACE ("%u.\t%d\t%d\t%d\t%d\t%.3f\n", i, Open, High, Low, Close, Vol);
	}

	return TRUE;
}


// -------------------------------------------------------------------------
// Process D42 - YHeadLine
bool TraceParsedFidIterator::ProcessYHeadLine()
{
	YHeadLine yheadline;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), yheadline))
		return FALSE;

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
	TRACE("\n-----------------------------------------------------------------------------");
	TRACE("\nFid: %d	HeadLine Line Number: %d	Date : %d-%d-%d	Time: %d:%d:%d\n", GetCurFidCode(), nLn, yr.GetYear(), yr.GetMonth(), yr.GetDay(), tm.GetHH(), tm.GetMIN(), tm.GetSS());
	TRACE("Type: %d	 Lang. ID: %s	PageNo.: %X	PNAC: %s\n", byType, pcLg, ulPg, pcPNAC);
	TRACE("TopicCode: %s\n", pcTopicCode);
	for (int i =0; i<byNe; i++)
	{
		TRACE("Page No. %d: %d\n", i, pulLong[i]);
	}
	TRACE("String: %s\n", pcString);
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D43 - OBkrQue
bool TraceParsedFidIterator::ProcessOBkrQue()
{
	OBkrQue obkrque;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), obkrque))
		return FALSE;

	TRACE("\n-----------------------------------------------------------------------------");
	TRACE("\nFid: %d	OBkrQue NE : %d\n", GetCurFidCode(), obkrque.GetNe());
	byte byA = 0;
	unsigned short unB = 0;
	unsigned short unC = 0;
	unsigned long  ulOSN = 0;
	for (int i=0; i<obkrque.GetNe(); i++)
	{
		if (obkrque.GetBroker(i, byA, unB, unC, ulOSN))
			TRACE("Type: %d	Broker Number: %d	Spare : %d, OSN : %d\n",  byA, unB, unC, ulOSN);
	}
	return TRUE;
}



// -------------------------------------------------------------------------
// Process Type D44 - KMultiple
bool TraceParsedFidIterator::ProcessKMultiple()
{
	long l = 0;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), l))
		return FALSE;

	TRACE("\n KMultiple:	Fid %d:	  Data : %f", GetCurFidCode(),  l/1000.0);
	return TRUE;
}

// -------------------------------------------------------------------------
// Process Type D45 - 10KMultiple
bool TraceParsedFidIterator::Process10KMultiple()
{
	long l = 0;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), l))
		return FALSE;

	TRACE("\n 10KMultiple:	Fid %d:	  Data : %f", GetCurFidCode(),  l/10000.0);
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D46 - BQueVOSN
bool TraceParsedFidIterator::ProcessBQueVOSN()
{
	BQueVOSN obkrque;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), obkrque))
		return FALSE;

	TRACE("\n-----------------------------------------------------------------------------");
	TRACE("\nFid: %d	OBkrQue NE : %d\n", GetCurFidCode(), obkrque.GetNe());
	byte byA = 0;
	unsigned short unB = 0;
	unsigned short unC = 0;
	unsigned long  ulOSN = 0;
	for (int i=0; i<obkrque.GetNe(); i++)
	{
		if (obkrque.GetBroker(i, byA, unB, unC, ulOSN))
			TRACE("Type: %d	Broker Number: %d	Spare : %d, OSN : %d\n",  byA, unB, unC, ulOSN);
	}
	return TRUE;
}


// -------------------------------------------------------------------------
// Process D47 - MTCPTS
bool TraceParsedFidIterator::ProcessMTCPTS()
{
	MTCPTS mtcPts;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), mtcPts))
		return FALSE;

	TRACE("\n-----------------------------------------------------------------------------");
	TRACE("\nFid: %d	MTCPTS Total: %d	MTCPTS NE : %d	MTCPTS Pos : %d\n", GetCurFidCode(), mtcPts.GetTL(), mtcPts.GetNe(), mtcPts.GetPos());
	byte byPts = 0;

	for (int i=0; i<mtcPts.GetNe(); i++)
	{
		if (mtcPts.GetPoint(i, byPts))
			TRACE("Points : %d	\n", byPts);
	}
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D48 - MTCF
bool TraceParsedFidIterator::ProcessMTCF()
{
	MTCF mtcf;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), mtcf))
		return FALSE;

	TRACE("\n-----------------------------------------------------------------------------");
	TRACE("\nFid: %d	MTCF Total: %d	MTCF NE : %d	MTCF Pos : %d\n", GetCurFidCode(), mtcf.GetTL(), mtcf.GetNe(), mtcf.GetPos());

	TRACE ("n.\tHigh\tLow\tClose\tVol\n");
	float Open, High, Low, Close;
	double Vol;		
	for (int i=0; i<mtcf.GetNe(); i++)
	{
		if (mtcf.GetPoint(i, Open, High, Low, Close, Vol))
			TRACE ("%u.\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\n", i, Open, High, Low, Close, Vol);			
	}
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D50 - PrcVolwSeq
bool TraceParsedFidIterator::ProcessPrcVolwSeq()
{
	PrcVolwSeq prc;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), prc))
		return FALSE;

	TRACE("\n-----------------------------------------------------------------------------");
	TRACE("\nFid: %d	PrcAskNuBid NE : %d, SeqNo : %d\n", GetCurFidCode(), prc.GetNe(), prc.GetSq());
	float fPr = 0;
	double dVl = 0;

	for (int i=0; i<prc.GetNe(); i++)
	{
		if (prc.GetPrcVol(i, fPr, dVl))
			TRACE("Price: %f	Volumn: %f\n", fPr, dVl);
	}
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D51 - Tran
bool TraceParsedFidIterator::ProcessTranD()
{
	TranD tranD;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), tranD))
		return FALSE;

	TRACE("\n-----------------------------------------------------------------------------");
	TRACE("\nFid: %d	Tran NE : %d\n", GetCurFidCode(), tranD.GetNe());
	unsigned long sq = 0;
	BTimeSec tm;
	double vl = 0;
	float nm = 0;
	char fg = 0;

	for (int i=0; i<tranD.GetNe(); i++)
	{
		if (tranD.GetTranElement(i, sq, tm, vl, nm, (byte&)fg))
			TRACE("Sequence No : %d	Time : %d:%d:%d	Vol: %f	Price: %f	Flag: %c\n", sq, tm.GetHH(), tm.GetMIN(), tm.GetSS(), vl, nm, fg);
	}
	return TRUE;
}

// -------------------------------------------------------------------------
// Process D52 - TxnD
bool TraceParsedFidIterator::ProcessTxnD()
{
	TxnD txnD;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), txnD))
		return FALSE;

	TRACE("\n-----------------------------------------------------------------------------");
	TRACE("\nFid: %d	Txn Total: %d	Txn NE : %d	Txn Sq: %d\n", GetCurFidCode(), txnD.GetTL(), txnD.GetNe(), txnD.GetSq());
	BTimeSec tm;
	double vl = 0;
	float nm = 0;
	char fg = 0;

	for (int i=0; i<txnD.GetNe(); i++)
	{
		if (txnD.GetTranElement(i, tm, vl, nm, (byte&)fg))
			TRACE("Time : %d:%d:%d	Vol: %f	Price: %f	Flag: %c\n", tm.GetHH(), tm.GetMIN(), tm.GetSS(), vl, nm, fg);
	}
	return TRUE;
}
