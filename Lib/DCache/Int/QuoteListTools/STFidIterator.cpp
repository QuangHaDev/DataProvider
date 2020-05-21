/////////////////////////////////////////////////////////////////////////////
//          File:: CSTFidIterator.cpp
// Create Author:: Raymond Yung
// Create   Date:: 30 Semptember 2000
//
//   Description:: The implementation file of the CSTFidIterator.h
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
#include "STFidIterator.h"

bool CSTFidIterator::ProcessFid()
{
	m_bytMsgType = m_pMsgObj->GetMsgType(); 	// Get Message Type
	m_lPageNo = m_pMsgObj->GetItemNo();			// Get Page No
	m_sFidNo = GetCurFidCode();					// Get Fid No
	return ParsedFidIterator::ProcessFid();	
}
// -------------------------------------------------------------------------
// Process D1 - Char
bool CSTFidIterator::ProcessChar()
{
	char c = 0;
	if (!m_pMsgObj->GetFid(m_sFidNo, c))
		return FALSE;
	else
	{
		m_iSize = sizeof(c);
		return true;
	}
}

// -------------------------------------------------------------------------
// Process D2 - Unicode
bool CSTFidIterator::ProcessUnicode()
{
	WCHAR c;
	if (!m_pMsgObj->GetFid(m_sFidNo, c))
		return FALSE;
	else
	{
		m_iSize = sizeof(c);
		return true;
	}

}

// -------------------------------------------------------------------------
// Process D3 - short
bool CSTFidIterator::ProcessShort()
{
	short sh = 0;
	if (!m_pMsgObj->GetFid(m_sFidNo, sh))
		return FALSE;
	else
	{
		m_iSize = sizeof(sh);
		return true;
	}
}

// -------------------------------------------------------------------------
// Process D4 - unsigned short
bool CSTFidIterator::ProcessUShort()
{
	unsigned short ush = 0;
	if (!m_pMsgObj->GetFid(m_sFidNo, ush))
		return FALSE;
	else
	{
		m_iSize = sizeof(ush);
		return true;
	}
}

// -------------------------------------------------------------------------
// Process D5 - long
bool CSTFidIterator::ProcessLong()
{
	long l = 0;
	if (!m_pMsgObj->GetFid(m_sFidNo, l))
		return FALSE;
	else
	{
		m_iSize = sizeof(l);
		return true;
	}
}

// -------------------------------------------------------------------------
// Process D6 - unsigned long
bool CSTFidIterator::ProcessULong()
{
	unsigned long ul = 0;
	if (!m_pMsgObj->GetFid(m_sFidNo, ul))
		return FALSE;
	else
	{
		m_iSize = sizeof(ul);
		return true;
	}
}

// -------------------------------------------------------------------------
// Process D7 & D8 - float
bool CSTFidIterator::ProcessFloat()
{
	float f = 0;
	if (!m_pMsgObj->GetFid(m_sFidNo, f))
		return FALSE;
	else
	{
		m_iSize = sizeof(f);
		return true;
	}
}

// -------------------------------------------------------------------------
// Process D8 - unsigned float
bool CSTFidIterator::ProcessUFloat()
{
	float f = 0;
	if (!m_pMsgObj->GetFid(m_sFidNo, f))
		return FALSE;
	else
	{
		m_iSize = sizeof(f);
		return true;
	}
}

// -------------------------------------------------------------------------
// Process D9 & D10 - double
bool CSTFidIterator::ProcessDouble()
{
	double d = 0;
	if (!m_pMsgObj->GetFid(m_sFidNo, d))
		return FALSE;
	else
	{
		m_iSize = sizeof(d);
		return true;
	}
}

// -------------------------------------------------------------------------
// Process D10 - unsigned double
bool CSTFidIterator::ProcessUDouble()
{
	double d = 0;
	if (!m_pMsgObj->GetFid(m_sFidNo, d))
		return FALSE;
	else
	{
		m_iSize = sizeof(d);
		return true;
	}
}

// -------------------------------------------------------------------------
// Process D11 - CharArray
bool CSTFidIterator::ProcessCharArray()
{
	CharArray ca;
	if (!m_pMsgObj->GetFid(m_sFidNo, ca))
		return false;
	else
	{
		m_iSize = ca.GetDataLen();
		return true;
	}

}

// -------------------------------------------------------------------------
// Process D12 - MCharArray
bool CSTFidIterator::ProcessMCharArray()
{
	MCharArray mca2;
	if (!m_pMsgObj->GetFid(m_sFidNo, mca2))
		return FALSE;
	else
	{
		m_iSize = mca2.GetDataLen();
		return true;
	}
}

// -------------------------------------------------------------------------
// Process D14 - BDateMonth
bool CSTFidIterator::ProcessBDateMonth()
{
	BDateMonth dm;
	if (!m_pMsgObj->GetFid(m_sFidNo, dm))
		return FALSE;
	else
	{
		m_iSize = dm.GetDataLen();
		return true;
	}
}

// -------------------------------------------------------------------------
// Process D13 - BDateYear
bool CSTFidIterator::ProcessBDateYear()
{
	BDateYear dy;
	if (!m_pMsgObj->GetFid(m_sFidNo, dy))
		return FALSE;
	else
	{
		m_iSize = dy.GetDataLen();
		return true;
	}
}

// -------------------------------------------------------------------------
// Process D15 - BTimeMin
bool CSTFidIterator::ProcessBTimeMin()
{
	BTimeMin tm;
	if (!m_pMsgObj->GetFid(m_sFidNo, tm))
		return FALSE;
	else
	{
		m_iSize = tm.GetDataLen();
		return true;
	}
}

// -------------------------------------------------------------------------
// Process D16 - BTimeSec
bool CSTFidIterator::ProcessBTimeSec()
{
	BTimeSec ts;
	if (!m_pMsgObj->GetFid(m_sFidNo, ts))
		return FALSE;
	else
	{
		m_iSize = ts.GetDataLen();
		return true;
	}
}

// -------------------------------------------------------------------------
// Process D17 - BTimeRanges
bool CSTFidIterator::ProcessBTimeRanges()
{
	BTimeRanges tr;
	if (!m_pMsgObj->GetFid(m_sFidNo, tr))
		return FALSE;
	else
	{
		m_iSize = tr.GetDataLen();
		return true;
	}
}

// -------------------------------------------------------------------------
// Process D18 - Tran
bool CSTFidIterator::ProcessTran()
{
	Tran tran;
	if (!m_pMsgObj->GetFid(m_sFidNo, tran))
		return FALSE;
	else
	{
		m_iSize = tran.GetDataLen();
		return true;
	}
}


// -------------------------------------------------------------------------
// Process D19 - Txn
bool CSTFidIterator::ProcessTxn()
{
	Txn txn;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), txn))
		return FALSE;
	else
	{
		m_iSize = txn.GetDataLen();
		return true;
	}
}

// -------------------------------------------------------------------------
// Process D20 - ShrOrdLst
bool CSTFidIterator::ProcessShrOrdLst()
{
	ShrOrdLst sol;
	if (!m_pMsgObj->GetFid(m_sFidNo, sol))
		return FALSE;
	else
	{
		m_iSize = sol.GetDataLen();
		return true;
	}
}

// -------------------------------------------------------------------------
// Process D21 - PRCVOL 
bool CSTFidIterator::ProcessPrcVol()
{
	PrcVol prc;
	if (!m_pMsgObj->GetFid(m_sFidNo, prc))
		return FALSE;
	else
	{
		m_iSize = prc.GetDataLen();
		return true;
	}
}

// -------------------------------------------------------------------------
// Process D22 - BkrQue
bool CSTFidIterator::ProcessBkrQue()
{
	BkrQue bkrque;
	if (!m_pMsgObj->GetFid(m_sFidNo, bkrque)) 
		return FALSE;
	else
	{
		m_iSize = bkrque.GetDataLen();
		return true;
	}
}

// -------------------------------------------------------------------------
// Process D23 - Link
bool CSTFidIterator::ProcessLink()
{
	Link link;
	if (!m_pMsgObj->GetFid(m_sFidNo, link))
		return FALSE;
	else
	{
		m_iSize = link.GetDataLen();
		return true;
	}
}

// -------------------------------------------------------------------------
// Process D24 - Wlink
bool CSTFidIterator::ProcessWlink()
{
	WLink wlink;
	if (!m_pMsgObj->GetFid(m_sFidNo, wlink))
		return FALSE;
	else
	{
		m_iSize = wlink.GetDataLen();
		return true;
	}
}

// -------------------------------------------------------------------------
// Process D25 - YLabel
bool CSTFidIterator::ProcessYLabel()
{
	YLabel ylabel;
	if (!m_pMsgObj->GetFid(m_sFidNo, ylabel))
		return FALSE;
	else	
	{
		m_iSize = ylabel.GetDataLen();
		return true;
	}
}

// -------------------------------------------------------------------------
// Process D26 - XMtcLabel
bool CSTFidIterator::ProcessXMtcLabel()
{
	XMtcLabel xmtcLbl;
	if (!m_pMsgObj->GetFid(m_sFidNo, xmtcLbl))
		return FALSE;
	else
	{
		m_iSize = xmtcLbl.GetDataLen();
		return true;
	}
}

// -------------------------------------------------------------------------
// Process D27 - XDtcLabel
bool CSTFidIterator::ProcessXDtcLabel()
{
	XDtcLabel xdtcLbl;
	if (!m_pMsgObj->GetFid(m_sFidNo, xdtcLbl))
		return FALSE;
	else
	{
		m_iSize = xdtcLbl.GetDataLen();
		return true;
	}
}

// -------------------------------------------------------------------------
// Process D28 - MtcPoints
bool CSTFidIterator::ProcessMtcPoints()
{
	MtcPoints mtcPts;
	if (!m_pMsgObj->GetFid(m_sFidNo, mtcPts))
		return FALSE;
	else
	{
		m_iSize = mtcPts.GetDataLen();
		return true;
	}
}

// -------------------------------------------------------------------------
// Process D29 - MtcVal
bool CSTFidIterator::ProcessMtcVal()
{
	MtcVal mtcval;
	if (!m_pMsgObj->GetFid(m_sFidNo, mtcval))
		return FALSE;
	else
	{
		m_iSize = mtcval.GetDataLen();
		return true;
	}
}

// -------------------------------------------------------------------------
// Process D30 - DtcPoints
bool CSTFidIterator::ProcessDtcPoints()
{
	DtcPoints dtcPts;
	if (!m_pMsgObj->GetFid(m_sFidNo, dtcPts))
		return FALSE;
	else
	{
		m_iSize = dtcPts.GetDataLen();
		return true;
	}
}

// -------------------------------------------------------------------------
// Process D31 - HisVal
bool CSTFidIterator::ProcessHisVal()
{
	HisVal hisval;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), hisval))
		return FALSE;
	else
	{
		m_iSize = hisval.GetDataLen();
		return true;
	}
}

// -------------------------------------------------------------------------
// Process D32 - FieldList
bool CSTFidIterator::ProcessFieldList()
{
	FieldList fldLst;
	if (!m_pMsgObj->GetFid(m_sFidNo, fldLst))
		return FALSE;
	else
	{
		m_iSize = fldLst.GetDataLen();
		return true;
	}
}

// -------------------------------------------------------------------------
// Process D33 - FieldLabel
bool CSTFidIterator::ProcessFieldLabel()
{
	FieldLabel fldLbl;
	if (!m_pMsgObj->GetFid(m_sFidNo, fldLbl))
		return FALSE;
	else
	{
		m_iSize = fldLbl.GetDataLen();
		return true;
	}
}

// -------------------------------------------------------------------------
// Process D34 - InfoMsg
bool CSTFidIterator::ProcessInfoMsg()
{
	InfoMsg infomsg;
	if (!m_pMsgObj->GetFid(m_sFidNo, infomsg))
		return FALSE;
	else
	{
		m_iSize = infomsg.GetDataLen();
		return true;
	}
}

// -------------------------------------------------------------------------
// Process D35 - HeadLine
bool CSTFidIterator::ProcessHeadLine()
{
	HeadLine headline;
	if (!m_pMsgObj->GetFid(m_sFidNo, headline))
		return FALSE;
	else
	{
		m_iSize = headline.GetDataLen();
		return true;
	}
}

// -------------------------------------------------------------------------
// Process D36 - HLink
bool CSTFidIterator::ProcessHLink()
{
	HLink hlink;
	if (!m_pMsgObj->GetFid(m_sFidNo, hlink))
		return FALSE;
	else
	{
		m_iSize = hlink.GetDataLen();
		return true;
	}
}

// -------------------------------------------------------------------------
// Process D37 - MShrOrdLst
bool CSTFidIterator::ProcessMShrOrdLst()
{
	MShrOrdLst mshr;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), mshr))
		return FALSE;
	else
	{
		m_iSize = mshr.GetDataLen();
		return true;
	}
}

// -------------------------------------------------------------------------
// Process D38 - SprTbl
bool CSTFidIterator::ProcessSprTbl()
{

	SprTbl sprtbl;
	if (!m_pMsgObj->GetFid(m_sFidNo, sprtbl))
		return FALSE;
	else
	{
		m_iSize = sprtbl.GetDataLen();
		return true;
	}
}

// -------------------------------------------------------------------------
// Process Type D39 - QuoteList
bool CSTFidIterator::ProcessQuoteList()
{
	QuoteList quoteList;
	if (!m_pMsgObj->GetFid(m_sFidNo, quoteList))
		return FALSE;
	else
	{
		m_iSize = quoteList.GetDataLen();
		return true;
	}
}

// -------------------------------------------------------------------------
// Process Type D40 - MTCTicks
bool CSTFidIterator::ProcessMTCTicks()
{
	MTCTicks mtcTicks;
	if (!m_pMsgObj->GetFid(m_sFidNo, mtcTicks))
		return FALSE;
	else
	{
		m_iSize = mtcTicks.GetDataLen();
		return true;
	}
}


// -------------------------------------------------------------------------
// Process Type D41 - MTCV
bool CSTFidIterator::ProcessMTCV()
{
	MTCV mtcv;
	if (!m_pMsgObj->GetFid(m_sFidNo, mtcv))
		return FALSE;
	else
	{
		m_iSize = mtcv.GetDataLen();
		return true;
	}
}

// -------------------------------------------------------------------------
// Process Type D42 - YHeadLine
bool CSTFidIterator::ProcessYHeadLine()
{
	YHeadLine yheadline;
	if (!m_pMsgObj->GetFid(m_sFidNo, yheadline))
		return FALSE;
	else
	{
		m_iSize = yheadline.GetDataLen();
		return true;
	}
}

// -------------------------------------------------------------------------
// Process D43 - OBkrQue
bool CSTFidIterator::ProcessOBkrQue()
{
	OBkrQue obkrque;
	if (!m_pMsgObj->GetFid(m_sFidNo, obkrque)) 
		return FALSE;
	else
	{
		m_iSize = obkrque.GetDataLen();
		return true;
	}
}

// -------------------------------------------------------------------------
// Process Type D44 - KMultiple
bool CSTFidIterator::ProcessKMultiple()
{
	long l = 0;
	if (!m_pMsgObj->GetFid(m_sFidNo, l))
		return FALSE;
	else
	{
		m_iSize = sizeof(l);
		return true;
	}
}

// -------------------------------------------------------------------------
// Process Type D45 - 10KMultiple
bool CSTFidIterator::Process10KMultiple()
{
	long l = 0;
	if (!m_pMsgObj->GetFid(m_sFidNo, l))
		return FALSE;
	else
	{
		m_iSize = sizeof(l);
		return true;
	}
}



// -------------------------------------------------------------------------
// Process D43 - OBkrQue
bool CSTFidIterator::ProcessBQueVOSN()
{
	BQueVOSN bquevosn;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), bquevosn))
		return FALSE;
	else
	{
		m_iSize = bquevosn.GetDataLen();
		return true;
	}
}

// -------------------------------------------------------------------------
// Process D47 - MTCPTS
bool CSTFidIterator::ProcessMTCPTS()
{
	MTCPTS mtcPts;
	if (!m_pMsgObj->GetFid(m_sFidNo, mtcPts))
		return FALSE;
	else
	{
		m_iSize = mtcPts.GetDataLen();
		return true;
	}
}

// -------------------------------------------------------------------------
// Process D48 - MTCF
bool CSTFidIterator::ProcessMTCF()
{
	MTCF mtcf;
	if (!m_pMsgObj->GetFid(m_sFidNo, mtcf))
		return FALSE;
	else
	{
		m_iSize = mtcf.GetDataLen();
		return true;
	}
}