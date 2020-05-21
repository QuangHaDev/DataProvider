/////////////////////////////////////////////////////////////////////////////
//          File:: DCacheFidFidIterator.cpp
// Create Author:: Raymond Yung
// Create   Date:: 30 Semptember 2000
//
//   Description:: The implementation file of the CDCacheFidIterator.h
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
#include "DCacheFidIterator.h"

bool CDCacheFidIterator::ProcessFid()
{
	m_bytMsgType = m_pMsgObj->GetMsgType(); 	// Get Message Type
	m_lPageNo = m_pMsgObj->GetItemNo();			// Get Page No
	m_sFidNo = GetCurFidCode();					// Get Fid No
	m_DCache->RemoveFidFromDropList(m_lPageNo, (FID_DEFINITION) m_sFidNo);
	return ParsedFidIterator::ProcessFid();	
}
// -------------------------------------------------------------------------
// Process D1 - Char
bool CDCacheFidIterator::ProcessChar()
{
	char c = 0;
	if (!m_pMsgObj->GetFid(m_sFidNo, c))
		return FALSE;
	else
	{
		// 2013 - TranLog conversion
		if (m_sFidNo == 2143)
			return TRUE; // 2013 - second info is not stored.
		else
			return m_DCache->SetAField(m_lPageNo, (unsigned char)c, (FID_DEFINITION)m_sFidNo, 0, m_pMsgObj);
	}
}

// -------------------------------------------------------------------------
// Process D2 - Unicode
bool CDCacheFidIterator::ProcessUnicode()
{
	WCHAR c;
	if (!m_pMsgObj->GetFid(m_sFidNo, c))
		return FALSE;
	else
		return m_DCache->SetAField(m_lPageNo, (unsigned short)c, (FID_DEFINITION)m_sFidNo, 0, m_pMsgObj);
}

// -------------------------------------------------------------------------
// Process D3 - short
bool CDCacheFidIterator::ProcessShort()
{
	short sh = 0;
	if (!m_pMsgObj->GetFid(m_sFidNo, sh))
		return FALSE;
	else
		return m_DCache->SetAField(m_lPageNo, sh, (FID_DEFINITION)m_sFidNo, 0, m_pMsgObj);
}

// -------------------------------------------------------------------------
// Process D4 - unsigned short
bool CDCacheFidIterator::ProcessUShort()
{
	unsigned short ush = 0;
	if (!m_pMsgObj->GetFid(m_sFidNo, ush))
		return FALSE;
	else
		return m_DCache->SetAField(m_lPageNo, ush, (FID_DEFINITION)m_sFidNo, 0, m_pMsgObj);
}

// -------------------------------------------------------------------------
// Process D5 - long
bool CDCacheFidIterator::ProcessLong()
{
	long l = 0;
	if (!m_pMsgObj->GetFid(m_sFidNo, l))
		return FALSE;
	else
		return m_DCache->SetAField(m_lPageNo, l, (FID_DEFINITION)m_sFidNo, 0, m_pMsgObj);
}
// -------------------------------------------------------------------------
// Process D6 - unsigned long
bool CDCacheFidIterator::ProcessULong()
{
	unsigned long ul = 0;
	if (!m_pMsgObj->GetFid(m_sFidNo, ul))
		return FALSE;
	else
		return m_DCache->SetAField(m_lPageNo, ul, (FID_DEFINITION)m_sFidNo, 0, m_pMsgObj);
}

// -------------------------------------------------------------------------
// Process D7 & D8 - float
bool CDCacheFidIterator::ProcessFloat()
{
	float f = 0;
	if (!m_pMsgObj->GetFid(m_sFidNo, f))
		return FALSE;
	else
		return m_DCache->SetAField(m_lPageNo, f, (FID_DEFINITION)m_sFidNo, 0, m_pMsgObj);
}

// -------------------------------------------------------------------------
// Process D8 - unsigned float
bool CDCacheFidIterator::ProcessUFloat()
{
	float f = 0;
	if (!m_pMsgObj->GetFid(m_sFidNo, f))
		return FALSE;
	else
		return m_DCache->SetAField(m_lPageNo, f, (FID_DEFINITION)m_sFidNo, 0, m_pMsgObj);
}

// -------------------------------------------------------------------------
// Process D9 & D10 - double
bool CDCacheFidIterator::ProcessDouble()
{
	double d = 0;
	if (!m_pMsgObj->GetFid(m_sFidNo, d))
		return FALSE;
	else
		return m_DCache->SetAField(m_lPageNo, d, (FID_DEFINITION)m_sFidNo, 0, m_pMsgObj);
}

// -------------------------------------------------------------------------
// Process D10 - unsigned double
bool CDCacheFidIterator::ProcessUDouble()
{
	double d = 0;
	if (!m_pMsgObj->GetFid(m_sFidNo, d))
		return FALSE;
	else
		return m_DCache->SetAField(m_lPageNo, d, (FID_DEFINITION)m_sFidNo, 0, m_pMsgObj);
}

// -------------------------------------------------------------------------
// Process D11 - CharArray
bool CDCacheFidIterator::ProcessCharArray()
{
	CharArray ca;
	if (m_pMsgObj->GetFid(m_sFidNo, ca))
	{
		// 2013 - Tranlog conversion
		if (m_sFidNo == 1936)
			return TRUE;

		if (ca.GetLen()>0)
		{
			if ((ca.GetLen()==255) && (ca.GetPos()==0))
			{
				TRACE("Drop Field, Item No: %x, FID No: %d, CA Len=%d, Pos=%d\n", m_lPageNo, m_sFidNo, ca.GetLen(), ca.GetPos());
				return m_DCache->DropAField(m_lPageNo, (FID_DEFINITION)m_sFidNo, m_pMsgObj);
			}
			else
				return m_DCache->SetAField(m_lPageNo, &ca, (FID_DEFINITION)m_sFidNo, 0, TRUE, m_pMsgObj);
		}
		else
			TRACE("Item No: %x, FID No: %d, CHARARRAY Len = %d\n", m_lPageNo, m_sFidNo, ca.GetLen());
		ASSERT(ca.GetLen()>0);
	}
	return FALSE;
}

// -------------------------------------------------------------------------
// Process D12 - MCharArray
bool CDCacheFidIterator::ProcessMCharArray()
{
	MCharArray mca2;
	if (!m_pMsgObj->GetFid(m_sFidNo, mca2))
		return FALSE;
	else
	{
		bool bReturnValue = FALSE;
		if (mca2.GetNe()>0)
		{
			long lNoOfElement = 0;
			if (m_DCache->GetMCharArrayTL(m_lPageNo, (FID_DEFINITION)m_sFidNo, lNoOfElement))
				if (lNoOfElement>mca2.GetTL())
					m_DCache->DropMCharArrayLine(m_lPageNo, (FID_DEFINITION)m_sFidNo, lNoOfElement, mca2.GetTL());

			for (int i=0; i<mca2.GetNe(); i++)
				if (mca2.GetLn(i)<=mca2.GetTL())
					if (m_DCache->SetAField(m_lPageNo, mca2.GetCA(i), (FID_DEFINITION)m_sFidNo, 0, mca2.GetLn(i), TRUE, m_pMsgObj))
						bReturnValue = TRUE;
		}
		else
			TRACE("Item No: %x, FID No: %d, MCHARARRAY Ne=%d, Tl=%d\n", m_lPageNo, m_sFidNo, mca2.GetNe(), mca2.GetTL());
		ASSERT(mca2.GetNe()>0);
		return bReturnValue;
	}
}

// -------------------------------------------------------------------------
// Process D14 - BDateMonth
bool CDCacheFidIterator::ProcessBDateMonth()
{
	BDateMonth dm;
	if (!m_pMsgObj->GetFid(m_sFidNo, dm))
		return FALSE;
	else
		return m_DCache->SetAField(m_lPageNo, dm.GetData(), (FID_DEFINITION)m_sFidNo, dm.GetDataLen(), m_pMsgObj);
}

// -------------------------------------------------------------------------
// Process D13 - BDateYear
bool CDCacheFidIterator::ProcessBDateYear()
{
	BDateYear dy;
	if (!m_pMsgObj->GetFid(m_sFidNo, dy))
		return FALSE;
	else
		return m_DCache->SetAField(m_lPageNo, dy.GetData(), (FID_DEFINITION)m_sFidNo, dy.GetDataLen(), m_pMsgObj);
}

// -------------------------------------------------------------------------
// Process D15 - BTimeMin
bool CDCacheFidIterator::ProcessBTimeMin()
{
	BTimeMin tm;
	if (!m_pMsgObj->GetFid(m_sFidNo, tm))
		return FALSE;
	else
		return m_DCache->SetAField(m_lPageNo, tm.GetData(), (FID_DEFINITION)m_sFidNo, tm.GetDataLen(), m_pMsgObj);
}

// -------------------------------------------------------------------------
// Process D16 - BTimeSec
bool CDCacheFidIterator::ProcessBTimeSec()
{
	BTimeSec ts;
	if (!m_pMsgObj->GetFid(m_sFidNo, ts))
		return FALSE;
	else
		return m_DCache->SetAField(m_lPageNo, ts.GetData(), (FID_DEFINITION)m_sFidNo, ts.GetDataLen(), m_pMsgObj);
}

// -------------------------------------------------------------------------
// Process D17 - BTimeRanges
bool CDCacheFidIterator::ProcessBTimeRanges()
{
	BTimeRanges tr;
	if (!m_pMsgObj->GetFid(m_sFidNo, tr))
		return FALSE;
	else
	{
		if (tr.GetNe() == 0)
		{
			TRACE("Drop Field, Item No: %x, FID No: %d, BTimeRanges Ne=%d\n", m_lPageNo, m_sFidNo, tr.GetNe());
			return m_DCache->DropAField(m_lPageNo, (FID_DEFINITION)m_sFidNo, m_pMsgObj);
		}
		else
			return m_DCache->SetAField(m_lPageNo, tr.GetData(), (FID_DEFINITION)m_sFidNo, tr.GetDataLen(), m_pMsgObj);
	}
}

// -------------------------------------------------------------------------
// Process D18 - Tran
bool CDCacheFidIterator::ProcessTran()
{
	Tran tran;
	if (!m_pMsgObj->GetFid(m_sFidNo, tran))
		return FALSE;
	else
	{
		if (tran.GetNe()>0)
		{
			BOOL bRipple = (m_bytMsgType==MGT_FORCEUPDATE)?TRUE:FALSE; 
			int iBlockSize = (tran.GetDataLen() - UCHAR_SIZE)/tran.GetNe();
			if (m_sFidNo == FID_TRAN_LOG)
			{
				// 2013 - Dcache will not store FID_TRAN_LOG 1301
				// It will be converted into FID_TRAN_D10_1 2700
				return TRUE;
			}
			else
			{
				return m_DCache->SetAField(m_lPageNo, tran.GetData(),	(FID_DEFINITION)m_sFidNo, iBlockSize, tran.GetNe(), m_DCache->GetTlogRipple((FID_DEFINITION)m_sFidNo), bRipple, m_pMsgObj);
			}
		}
		else
		{
			TRACE("Drop Field, Item No: %x, FID No: %d, TRAN Ne=%d\n", m_lPageNo, m_sFidNo,tran.GetNe());
			return m_DCache->DropAField(m_lPageNo, (FID_DEFINITION)m_sFidNo, m_pMsgObj);
		}
	}	
	return FALSE;
}

bool CDCacheFidIterator::GetProcessTran(short sFid, Tran &tran, COP_ITEM &copMsg)
{
	if (!m_pMsgObj->GetFid(sFid, tran))
		return false;
	copMsg = m_pMsgObj;
	return true;
}

bool CDCacheFidIterator::GetProcessChar(short sFid, char &c)
{
	if (!m_pMsgObj->GetFid(sFid, c))
		return false;
	return true;
}

bool CDCacheFidIterator::GetProcessCharArray(short sFid, CharArray &ca)
{
	if (!m_pMsgObj->GetFid(sFid, ca))
		return false;
	return true;
}

// -------------------------------------------------------------------------
// Process D20 - ShrOrdLst
bool CDCacheFidIterator::ProcessShrOrdLst()
{
	ShrOrdLst sol;
	if (!m_pMsgObj->GetFid(m_sFidNo, sol))
		return FALSE;
	else
		return m_DCache->SetAField(m_lPageNo, sol.GetData(), (FID_DEFINITION)m_sFidNo, sol.GetDataLen(), m_pMsgObj);
}

// -------------------------------------------------------------------------
// Process D21 - PRCVOL 
bool CDCacheFidIterator::ProcessPrcVol()
{
	PrcVol prc;
	if (!m_pMsgObj->GetFid(m_sFidNo, prc))
		return FALSE;
	else
	{
		if ((prc.GetPos()==0)&&(prc.GetNe()==0)&&(prc.GetTL()==0))
		{
			TRACE("Drop Field, Item No: %x, FID No: %d, PrcVol Ne=%d\n", m_lPageNo, m_sFidNo,prc.GetNe());
			return m_DCache->DropAField(m_lPageNo, (FID_DEFINITION)m_sFidNo, m_pMsgObj);
		}
		else
		{
			if (prc.GetNe()>0)
			{
				if (prc.GetPos()+prc.GetNe()<=prc.GetTL())
				{
					int iBlockSize = (prc.GetDataLen() - UCHAR_SIZE*3 ) / prc.GetNe() ;
					return m_DCache->SetAField(m_lPageNo, prc.GetData(), (FID_DEFINITION)m_sFidNo, prc.GetPos(), iBlockSize, prc.GetNe(), m_pMsgObj);
				}
			}
			TRACE("Item No: %x, FID No: %d, PRCVOL Ne=%d, Tl=%d, Pos=%d\n", m_lPageNo, m_sFidNo, prc.GetNe(),prc.GetTL(),prc.GetPos());
			ASSERT(prc.GetNe()>0);
			ASSERT(prc.GetPos()+prc.GetNe()<=prc.GetTL());
		}
		return FALSE;
	}	
}

// -------------------------------------------------------------------------
// Process D22 - BkrQue
bool CDCacheFidIterator::ProcessBkrQue()
{
	BkrQue bkrque;
	if (!m_pMsgObj->GetFid(m_sFidNo, bkrque)) 
		return FALSE;
	else
		if (bkrque.GetNe()==0)
		{
			TRACE("Drop Field, Item No: %x, FID No: %d, BkrQue Ne=%d\n", m_lPageNo, m_sFidNo, bkrque.GetNe());
			return m_DCache->DropAField(m_lPageNo, (FID_DEFINITION)m_sFidNo, m_pMsgObj);
		}
		else
			return m_DCache->SetAField(m_lPageNo, bkrque.GetData(), (FID_DEFINITION)m_sFidNo, bkrque.GetDataLen(), m_pMsgObj);
}

// -------------------------------------------------------------------------
// Process D23 - Link
bool CDCacheFidIterator::ProcessLink()
{
	Link link;
	if (!m_pMsgObj->GetFid(m_sFidNo, link))
		return FALSE;
	else
	{
		if ((link.GetNe()==0)&&(link.GetPos()==0)&&(link.GetTL()==0))
		{
			TRACE("Drop Field, Item No: %x, FID No: %d, Link Ne=%d\n", m_lPageNo, m_sFidNo, link.GetNe());
			return m_DCache->DropAField(m_lPageNo, (FID_DEFINITION)m_sFidNo, m_pMsgObj);
		}
		else
		{
			if (link.GetNe()>0)
			{
				if (link.GetNe()+link.GetPos()<=link.GetTL())
				{
					long lSizeOfBlock = (link.GetDataLen() - USHORT_SIZE*3) / link.GetNe();
					return m_DCache->SetAField(m_lPageNo, link.GetData(), (FID_DEFINITION)m_sFidNo, link.GetPos(), lSizeOfBlock, m_pMsgObj);
				}
			}
			TRACE("Item No: %x, FID No: %d, LINK Ne=%d, TL=%d, Pos=%d\n", m_lPageNo,m_sFidNo,link.GetNe(),link.GetTL(),link.GetPos());
			ASSERT(link.GetNe()>0);
			ASSERT(link.GetNe()+link.GetPos()<=link.GetTL());
		}
		return FALSE;
	}	
}

// -------------------------------------------------------------------------
// Process D24 - Wlink
bool CDCacheFidIterator::ProcessWlink()
{
	WLink wlink;
	if (!m_pMsgObj->GetFid(m_sFidNo, wlink))
		return FALSE;
	else
	{
		if ((wlink.GetNe()==0)&&(wlink.GetPos()==0)&&(wlink.GetTL()==0))
		{
			TRACE("Drop Field, Item No: %x, FID No: %d, WLink Ne=%d\n", m_lPageNo, m_sFidNo, wlink.GetNe());
			return m_DCache->DropAField(m_lPageNo, (FID_DEFINITION)m_sFidNo, m_pMsgObj);
		}
		else
		{
			if (wlink.GetNe()>0)
			{
				if (wlink.GetNe()+wlink.GetPos()<=wlink.GetTL())
				{
					long lSizeOfBlock = (wlink.GetDataLen() - USHORT_SIZE*3) / wlink.GetNe();
					return m_DCache->SetAField(m_lPageNo, wlink.GetData(), (FID_DEFINITION)m_sFidNo, wlink.GetPos(), lSizeOfBlock, m_pMsgObj);
				}
			}
			TRACE("Item No: %x, FID No: %d, WLINK Ne=%d, TL=%d, Pos=%d\n", m_lPageNo,m_sFidNo,wlink.GetNe(),wlink.GetTL(),wlink.GetPos());
			ASSERT(wlink.GetNe()>0);
			ASSERT(wlink.GetNe()+wlink.GetPos()<=wlink.GetTL());
		}
		return FALSE;
	}	
}

// -------------------------------------------------------------------------
// Process D25 - YLabel
bool CDCacheFidIterator::ProcessYLabel()
{
	YLabel ylabel;
	if (!m_pMsgObj->GetFid(m_sFidNo, ylabel))
		return FALSE;
	else	
		return m_DCache->SetAField(m_lPageNo, ylabel.GetData(), (FID_DEFINITION)m_sFidNo, ylabel.GetDataLen(), m_pMsgObj);
}

// -------------------------------------------------------------------------
// Process D26 - XMtcLabel
bool CDCacheFidIterator::ProcessXMtcLabel()
{
	XMtcLabel xmtcLbl;
	if (!m_pMsgObj->GetFid(m_sFidNo, xmtcLbl))
		return FALSE;
	else
	{
		if (xmtcLbl.GetNe()==0)
		{
			TRACE("Drop Field, Item No: %x, FID No: %d, XMtcLabel Ne=%d\n", m_lPageNo, m_sFidNo, xmtcLbl.GetNe());
			return m_DCache->DropAField(m_lPageNo, (FID_DEFINITION)m_sFidNo, m_pMsgObj);
		}
		else
			return m_DCache->SetAField(m_lPageNo, xmtcLbl.GetData(), (FID_DEFINITION)m_sFidNo, xmtcLbl.GetDataLen(), m_pMsgObj);
	}
}

// -------------------------------------------------------------------------
// Process D27 - XDtcLabel
bool CDCacheFidIterator::ProcessXDtcLabel()
{
	XDtcLabel xdtcLbl;
	if (!m_pMsgObj->GetFid(m_sFidNo, xdtcLbl))
		return FALSE;
	else
	{
		if (xdtcLbl.GetNe()==0)
		{
			TRACE("Drop Field, Item No: %x, FID No: %d, XDtcLabel Ne=%d\n", m_lPageNo, m_sFidNo, xdtcLbl.GetNe());
			return m_DCache->DropAField(m_lPageNo, (FID_DEFINITION)m_sFidNo, m_pMsgObj);
		}
		else
			return m_DCache->SetAField(m_lPageNo, xdtcLbl.GetData(), (FID_DEFINITION)m_sFidNo, xdtcLbl.GetDataLen(), m_pMsgObj);
	}
}

// -------------------------------------------------------------------------
// Process D28 - MtcPoints
bool CDCacheFidIterator::ProcessMtcPoints()
{
	MtcPoints mtcPts;
	if (!m_pMsgObj->GetFid(m_sFidNo, mtcPts))
		return FALSE;
	else
	{
		if ((mtcPts.GetNe()==0)&&(mtcPts.GetPos()==0)&&(mtcPts.GetTL()==0))
		{
			TRACE("Drop Field, Item No: %x, FID No: %d, mtcPts Ne=%d\n", m_lPageNo, m_sFidNo, mtcPts.GetNe());
			return m_DCache->DropAField(m_lPageNo, (FID_DEFINITION)m_sFidNo, m_pMsgObj);
		}
		else
		{
			if (mtcPts.GetNe()>0)
			{
				if (mtcPts.GetNe()+mtcPts.GetPos()<=mtcPts.GetTL())
				{
					long lSizeOfBlock = (mtcPts.GetDataLen() - USHORT_SIZE*3) / mtcPts.GetNe();
					return m_DCache->SetAField(m_lPageNo, mtcPts.GetData(), (FID_DEFINITION)m_sFidNo, mtcPts.GetPos(), lSizeOfBlock, m_pMsgObj);
				}
			}
			TRACE("Item No: %x, FID No: %d, MTCPOINTS Ne=%d, TL=%d, Pos=%d\n", m_lPageNo,m_sFidNo,mtcPts.GetNe(),mtcPts.GetTL(),mtcPts.GetPos());
			ASSERT(mtcPts.GetNe()>0);
			ASSERT(mtcPts.GetNe()+mtcPts.GetPos()<=mtcPts.GetTL());
			return FALSE;
		}
	}	
}

// -------------------------------------------------------------------------
// Process D29 - MtcVal
bool CDCacheFidIterator::ProcessMtcVal()
{
	MtcVal mtcval;
	if (!m_pMsgObj->GetFid(m_sFidNo, mtcval))
		return FALSE;
	else
	{
		if ((mtcval.GetNe()==0)&&(mtcval.GetPos()==0)&&(mtcval.GetTL()==0))
		{
			TRACE("Drop Field, Item No: %x, FID No: %d, mtcval Ne=%d\n", m_lPageNo, m_sFidNo, mtcval.GetNe());
			return m_DCache->DropAField(m_lPageNo, (FID_DEFINITION)m_sFidNo, m_pMsgObj);
		}
		else
		{
			if (mtcval.GetNe()>0)
			{
				if (mtcval.GetNe()+mtcval.GetPos()<=mtcval.GetTL())
				{
					long lSizeOfBlock = (mtcval.GetDataLen() - USHORT_SIZE*3)/ mtcval.GetNe();
					return m_DCache->SetAField(m_lPageNo, mtcval.GetData(), (FID_DEFINITION)m_sFidNo, mtcval.GetPos(), lSizeOfBlock, m_pMsgObj);
				}
			}
			TRACE("Item No: %x, FID No: %d, MTCVAL Ne=%d, TL=%d, Pos=%d\n", m_lPageNo,m_sFidNo,mtcval.GetNe(),mtcval.GetTL(),mtcval.GetPos());
			ASSERT(mtcval.GetNe()>0);
			ASSERT(mtcval.GetNe()+mtcval.GetPos()<=mtcval.GetTL());
			return FALSE;
		}
	}	
}

// -------------------------------------------------------------------------
// Process D30 - DtcPoints
bool CDCacheFidIterator::ProcessDtcPoints()
{
	DtcPoints dtcPts;
	if (!m_pMsgObj->GetFid(m_sFidNo, dtcPts))
		return FALSE;
	else
	{
		if (dtcPts.GetNe()==0)
		{
			TRACE("Drop Field, Item No: %x, FID No: %d, DtcPoints Ne=%d\n", m_lPageNo, m_sFidNo, dtcPts.GetNe());
			return m_DCache->DropAField(m_lPageNo, (FID_DEFINITION)m_sFidNo, m_pMsgObj);
		}
		else
			return m_DCache->SetAField(m_lPageNo, dtcPts.GetData(), (FID_DEFINITION)m_sFidNo, dtcPts.GetDataLen(), m_pMsgObj);
	}
}

// -------------------------------------------------------------------------
// Process D32 - FieldList
bool CDCacheFidIterator::ProcessFieldList()
{
	FieldList fldLst;
	if (!m_pMsgObj->GetFid(m_sFidNo, fldLst))
		return FALSE;
	else
	{
		if (fldLst.GetNe()==0)
		{
			TRACE("Drop Field, Item No: %x, FID No: %d, FieldList Ne=%d\n", m_lPageNo, m_sFidNo, fldLst.GetNe());
			return m_DCache->DropAField(m_lPageNo, (FID_DEFINITION)m_sFidNo, m_pMsgObj);
		}
		else
			return m_DCache->SetAField(m_lPageNo, fldLst.GetData(), (FID_DEFINITION)m_sFidNo, fldLst.GetDataLen(), m_pMsgObj);
	}
}

// -------------------------------------------------------------------------
// Process D33 - FieldLabel
bool CDCacheFidIterator::ProcessFieldLabel()
{
	FieldLabel fldLbl;
	if (!m_pMsgObj->GetFid(m_sFidNo, fldLbl))
		return FALSE;
	else
	{
		if (fldLbl.GetNe()==0)
		{
			TRACE("Drop Field, Item No: %x, FID No: %d, FieldLabel Ne=%d\n", m_lPageNo, m_sFidNo, fldLbl.GetNe());
			return m_DCache->DropAField(m_lPageNo, (FID_DEFINITION)m_sFidNo, m_pMsgObj);
		}
		else		
			return m_DCache->SetAField(m_lPageNo, fldLbl.GetData(), (FID_DEFINITION)m_sFidNo, fldLbl.GetDataLen(), m_pMsgObj);
	}
}

// -------------------------------------------------------------------------
// Process D34 - InfoMsg
bool CDCacheFidIterator::ProcessInfoMsg()
{
	InfoMsg infomsg;
	if (!m_pMsgObj->GetFid(m_sFidNo, infomsg))
		return FALSE;
	else
		return m_DCache->SetAField(m_lPageNo, infomsg.GetData(), (FID_DEFINITION)m_sFidNo, infomsg.GetDataLen(), m_pMsgObj);
}

// -------------------------------------------------------------------------
// Process D35 - HeadLine
bool CDCacheFidIterator::ProcessHeadLine()
{
	HeadLine headline;
	if (!m_pMsgObj->GetFid(m_sFidNo, headline))
		return FALSE;
	else
	{
		unsigned long nLn = 0;
		memcpy(&nLn, headline.GetData(), 4);
		return m_DCache->SetAField(m_lPageNo, headline.GetData(), (FID_DEFINITION)m_sFidNo, nLn, headline.GetDataLen(), m_pMsgObj);
	}	
}

// -------------------------------------------------------------------------
// Process D36 - HLink
bool CDCacheFidIterator::ProcessHLink()
{
	HLink hlink;
	if (!m_pMsgObj->GetFid(m_sFidNo, hlink))
		return FALSE;
	else
	{
		if ((hlink.GetNe()==0)&&(hlink.GetPos()==0)&&(hlink.GetTL()==0))
		{
			TRACE("Drop Field, Item No: %x, FID No: %d, HLink Ne=%d\n", m_lPageNo, m_sFidNo, hlink.GetNe());
			return m_DCache->DropAField(m_lPageNo, (FID_DEFINITION)m_sFidNo, m_pMsgObj);
		}
		else
		{
			if (hlink.GetNe()>0)
			{
				if (hlink.GetNe()+hlink.GetPos()<=hlink.GetTL())
				{
					int iBlockSize = (hlink.GetDataLen() - UCHAR_SIZE*3 ) / hlink.GetNe() ;
					return m_DCache->SetAField(m_lPageNo, hlink.GetData(), (FID_DEFINITION)m_sFidNo, hlink.GetDataLen(), m_pMsgObj);
				}
			}
			TRACE("Item No: %x, FID No: %d, HLINK NE=%d, TL=%d, Pos=%d\n", m_lPageNo,m_sFidNo,hlink.GetNe(),hlink.GetTL(),hlink.GetPos());
			ASSERT(hlink.GetNe()>0);
			ASSERT(hlink.GetNe()+hlink.GetPos()<=hlink.GetTL());
			return FALSE;
		}
	}	
}


// -------------------------------------------------------------------------
// Process D38 - SprTbl
bool CDCacheFidIterator::ProcessSprTbl()
{
	SprTbl sprtbl;
	if (!m_pMsgObj->GetFid(m_sFidNo, sprtbl))
		return FALSE;
	else
	{
		if (sprtbl.GetNe()==0)
		{
			TRACE("Drop Field, Item No: %x, FID No: %d, SprTbl Ne=%d\n", m_lPageNo, m_sFidNo, sprtbl.GetNe());
			return m_DCache->DropAField(m_lPageNo, (FID_DEFINITION)m_sFidNo, m_pMsgObj);
		}
		else
		{
			if (sprtbl.GetNe()>0)
			{
				if (sprtbl.GetNe()+sprtbl.GetPos()<=sprtbl.GetTL())
				{
					long lSizeOfBlock = (sprtbl.GetDataLen() - USHORT_SIZE*3) / sprtbl.GetNe();
					return m_DCache->SetAField(m_lPageNo, sprtbl.GetData(), (FID_DEFINITION)m_sFidNo, sprtbl.GetPos(), lSizeOfBlock, m_pMsgObj);
				}
			}
			TRACE("Item No: %x, FID No: %d, SPRTBL Ne=%d, TL=%d, Pos=%d\n", m_lPageNo,m_sFidNo,sprtbl.GetNe(),sprtbl.GetTL(),sprtbl.GetPos());
			ASSERT(sprtbl.GetNe()>0);
			ASSERT(sprtbl.GetNe()+sprtbl.GetPos()<=sprtbl.GetTL());
			return FALSE;
		}
	}
}

// -------------------------------------------------------------------------
// Process D18 - Tran
bool CDCacheFidIterator::ProcessQuoteList()
{
	QuoteList quotelist;
	if (!m_pMsgObj->GetFid(m_sFidNo, quotelist))
		return FALSE;
	else
	{
		if (quotelist.GetTL()>0)
		{
			if (quotelist.GetNE()>0)
				return m_DCache->SetAField(m_lPageNo, quotelist.GetData(), (FID_DEFINITION)m_sFidNo, 0, quotelist.GetNE(), quotelist.GetTL(), m_pMsgObj);
			else
				return FALSE;
		}
		else
		{
			TRACE("Drop Field, Item No: %x, FID No: %d, QuoteList TL=%d\n", m_lPageNo, m_sFidNo, quotelist.GetTL());
			return m_DCache->DropAField(m_lPageNo, (FID_DEFINITION)m_sFidNo, m_pMsgObj);
		}
	}	
	return FALSE;
}

// -------------------------------------------------------------------------
// Process Type D40 - MTCTicks
bool CDCacheFidIterator::ProcessMTCTicks()
{
	MTCTicks mtcticks;
	if (!m_pMsgObj->GetFid(m_sFidNo, mtcticks))
		return FALSE;
	else
	{
		if ((mtcticks.GetNe()==0)&&(mtcticks.GetPos()==0)&&(mtcticks.GetTL()==0))
		{
			TRACE("Drop Field, Item No: %x, FID No: %d, MTCTicks Ne=%d\n", m_lPageNo, m_sFidNo, mtcticks.GetNe());
			return m_DCache->DropAField(m_lPageNo, (FID_DEFINITION)m_sFidNo, m_pMsgObj);
		}
		else
		{
			if (mtcticks.GetNe()>0)
			{
				if (mtcticks.GetNe()+mtcticks.GetPos()<=mtcticks.GetTL())
				{
					long lSizeOfBlock = 2 + 2 + 2 + 2;
					return m_DCache->SetAField(m_lPageNo, mtcticks.ToDoubleByteStream(), (FID_DEFINITION)m_sFidNo, mtcticks.GetPos(), lSizeOfBlock, m_pMsgObj);
				}
			}
			TRACE("Item No: %x, FID No: %d, MTCTicks Ne=%d, TL=%d, Pos=%d\n", m_lPageNo,m_sFidNo,mtcticks.GetNe(),mtcticks.GetTL(),mtcticks.GetPos());
			ASSERT(mtcticks.GetNe()>0);
			ASSERT(mtcticks.GetNe()+mtcticks.GetPos()<=mtcticks.GetTL());
		}
		return FALSE;
	}	
}


// -------------------------------------------------------------------------
// Process Type D41 - MTCV
bool CDCacheFidIterator::ProcessMTCV()
{
	MTCV mtcv;
	if (!m_pMsgObj->GetFid(m_sFidNo, mtcv))
		return FALSE;
	else
	{
		if ((mtcv.GetNe()==0)&&(mtcv.GetPos()==0)&&(mtcv.GetTL()==0))
		{
			TRACE("Drop Field, Item No: %x, FID No: %d, MTCV Ne=%d\n", m_lPageNo, m_sFidNo, mtcv.GetNe());
			return m_DCache->DropAField(m_lPageNo, (FID_DEFINITION)m_sFidNo, m_pMsgObj);
		}
		else
		{
			if (mtcv.GetNe()>0)
			{
				if (mtcv.GetNe()+mtcv.GetPos()<=mtcv.GetTL())
				{
					long lSizeOfBlock = 2 + 2 + 2 + 2 + 8;
					return m_DCache->SetAField(m_lPageNo, mtcv.ToDoubleByteStream(), (FID_DEFINITION)m_sFidNo, mtcv.GetPos(), lSizeOfBlock, m_pMsgObj);
				}
			}
			TRACE("Item No: %x, FID No: %d, MTCV Ne=%d, TL=%d, Pos=%d\n", m_lPageNo,m_sFidNo,mtcv.GetNe(),mtcv.GetTL(),mtcv.GetPos());
			ASSERT(mtcv.GetNe()>0);
			ASSERT(mtcv.GetNe()+mtcv.GetPos()<=mtcv.GetTL());
		}
		return FALSE;
	}	
}

// -------------------------------------------------------------------------
// Process Type D42 - YHeadLine
bool CDCacheFidIterator::ProcessYHeadLine()
{
	YHeadLine yheadline;
	if (!m_pMsgObj->GetFid(m_sFidNo, yheadline))
		return FALSE;
	else
	{
		unsigned long nLn = 0; 
		memcpy(&nLn, yheadline.GetData(), 4);
		return m_DCache->SetAField(m_lPageNo, yheadline.GetData(), (FID_DEFINITION)m_sFidNo, nLn, yheadline.GetDataLen(), m_pMsgObj);
	}	
}


// -------------------------------------------------------------------------
// Process D43 - OBkrQue
bool CDCacheFidIterator::ProcessOBkrQue()
{
	OBkrQue obkrque;
	if (!m_pMsgObj->GetFid(m_sFidNo, obkrque)) 
		return FALSE;
	else
		if (obkrque.GetNe()==0)
		{
			TRACE("Drop Field, Item No: %x, FID No: %d, OBkrQue Ne=%d\n", m_lPageNo, m_sFidNo, obkrque.GetNe());
			return m_DCache->DropAField(m_lPageNo, (FID_DEFINITION)m_sFidNo, m_pMsgObj);
		}
		else
			return m_DCache->SetAField(m_lPageNo, obkrque.GetData(), (FID_DEFINITION)m_sFidNo, obkrque.GetDataLen(), m_pMsgObj);
}

// -------------------------------------------------------------------------
// Process Type D44 - KMultiple
bool CDCacheFidIterator::ProcessKMultiple()
{
	long l = 0;
	if (!m_pMsgObj->GetFid(m_sFidNo, l))
		return FALSE;
	else
		return m_DCache->SetAField(m_lPageNo, l, (FID_DEFINITION)m_sFidNo, 0, m_pMsgObj);
}

// -------------------------------------------------------------------------
// Process Type D45 - 10KMultiple
bool CDCacheFidIterator::Process10KMultiple()
{
	long l = 0;
	if (!m_pMsgObj->GetFid(m_sFidNo, l))
		return FALSE;
	else
		return m_DCache->SetAField(m_lPageNo, l, (FID_DEFINITION)m_sFidNo, 0, m_pMsgObj);
}


// -------------------------------------------------------------------------
// Process Type D46 - BQueVOSN
bool CDCacheFidIterator::ProcessBQueVOSN()
{

	BQueVOSN bquevosn;

	if (!m_pMsgObj->GetFid(GetCurFidCode(), bquevosn))
		return FALSE;
	else
	{
		if (bquevosn.GetNe()==0)
		{
			TRACE("Drop Field, Item No: %x, FID No: %d, BQueVOSN Ne=%d\n", m_lPageNo, m_sFidNo, bquevosn.GetNe());
			return m_DCache->DropAField(m_lPageNo, (FID_DEFINITION)m_sFidNo, m_pMsgObj);
		}
		else
			return m_DCache->SetAField(m_lPageNo, bquevosn.GetData(), (FID_DEFINITION)m_sFidNo, bquevosn.GetDataLen(), m_pMsgObj);
	}
}

// -------------------------------------------------------------------------
// Process D47 - MTCPTS
bool CDCacheFidIterator::ProcessMTCPTS()
{
	MTCPTS mtcPts;
	if (!m_pMsgObj->GetFid(m_sFidNo, mtcPts))
		return FALSE;
	else
	{
		if ((mtcPts.GetNe()==0)&&(mtcPts.GetPos()==0)&&(mtcPts.GetTL()==0))
		{
			TRACE("Drop Field, Item No: %x, FID No: %d, mtcPts Ne=%d\n", m_lPageNo, m_sFidNo, mtcPts.GetNe());
			return m_DCache->DropAField(m_lPageNo, (FID_DEFINITION)m_sFidNo, m_pMsgObj);
		}
		else
		{
			if (mtcPts.GetNe()>0)
			{
				if (mtcPts.GetNe()+mtcPts.GetPos()<=mtcPts.GetTL())
				{
					long lSizeOfBlock = (mtcPts.GetDataLen() - USHORT_SIZE*3) / mtcPts.GetNe();
					return m_DCache->SetAField(m_lPageNo, mtcPts.GetData(), (FID_DEFINITION)m_sFidNo, mtcPts.GetPos(), lSizeOfBlock, m_pMsgObj);
				}
			}
			TRACE("Item No: %x, FID No: %d, MTCPTS Ne=%d, TL=%d, Pos=%d\n", m_lPageNo,m_sFidNo,mtcPts.GetNe(),mtcPts.GetTL(),mtcPts.GetPos());
			ASSERT(mtcPts.GetNe()>0);
			ASSERT(mtcPts.GetNe()+mtcPts.GetPos()<=mtcPts.GetTL());
			return FALSE;
		}
	}	
}

// -------------------------------------------------------------------------
// Process D48 - MTCF
bool CDCacheFidIterator::ProcessMTCF()
{
	MTCF mtcf;
	if (!m_pMsgObj->GetFid(m_sFidNo, mtcf))
		return FALSE;
	else
	{
		if ((mtcf.GetNe()==0)&&(mtcf.GetPos()==0)&&(mtcf.GetTL()==0))
		{
			TRACE("Drop Field, Item No: %x, FID No: %d, mtcf Ne=%d\n", m_lPageNo, m_sFidNo, mtcf.GetNe());
			return m_DCache->DropAField(m_lPageNo, (FID_DEFINITION)m_sFidNo, m_pMsgObj);
		}
		else
		{
			if (mtcf.GetNe()>0)
			{
				if (mtcf.GetNe()+mtcf.GetPos()<=mtcf.GetTL())
				{
					long lSizeOfBlock = (mtcf.GetDataLen() - USHORT_SIZE*3) / mtcf.GetNe();
					return m_DCache->SetAField(m_lPageNo, mtcf.GetData(), (FID_DEFINITION)m_sFidNo, mtcf.GetPos(), lSizeOfBlock, m_pMsgObj);
				}
			}
			TRACE("Item No: %x, FID No: %d, MTCF Ne=%d, TL=%d, Pos=%d\n", m_lPageNo,m_sFidNo,mtcf.GetNe(),mtcf.GetTL(),mtcf.GetPos());
			ASSERT(mtcf.GetNe()>0);
			ASSERT(mtcf.GetNe()+mtcf.GetPos()<=mtcf.GetTL());
			return FALSE;
		}
	}	
}


// -------------------------------------------------------------------------
// Process D49 - PrcVolwSeq 
bool CDCacheFidIterator::ProcessPrcVolwSeq()
{
	PrcVolwSeq prc;
	if (!m_pMsgObj->GetFid(m_sFidNo, prc))
		return FALSE;
	else
	{
		if ((prc.GetPos()==0)&&(prc.GetNe()==0)&&(prc.GetTL()==0))
		{
			TRACE("Drop Field, Item No: %x, FID No: %d, PrcVolwSeq Ne=%d\n", m_lPageNo, m_sFidNo,prc.GetNe());
			return m_DCache->DropAField(m_lPageNo, (FID_DEFINITION)m_sFidNo, m_pMsgObj);
		}
		else
		{
			if (prc.GetNe()>0)
			{
				if (prc.GetPos()+prc.GetNe()<=prc.GetTL())
				{
					int iBlockSize = (prc.GetDataLen() - (UCHAR_SIZE*3 + 4)) / prc.GetNe() ;
					int iHeaderSize = UCHAR_SIZE*3 + 4;
					int iTotalSize = prc.GetDataLen();
					return m_DCache->SetAField(m_lPageNo, prc.GetData(), (FID_DEFINITION)m_sFidNo, prc.GetPos(), iBlockSize, prc.GetNe(), iHeaderSize, iTotalSize, m_pMsgObj);
				}
			}
			TRACE("Item No: %x, FID No: %d, PrcVolwSeq Ne=%d, Tl=%d, Pos=%d, Seq=%d\n", m_lPageNo, m_sFidNo, prc.GetNe(),prc.GetTL(),prc.GetPos(), prc.GetSq());
			ASSERT(prc.GetNe()>0);
			ASSERT(prc.GetPos()+prc.GetNe()<=prc.GetTL());
		}
		return FALSE;
	}	
}

// -------------------------------------------------------------------------
// Process D51 - TranD
bool CDCacheFidIterator::ProcessTranD()
{
	TranD tranD;
	if (!m_pMsgObj->GetFid(m_sFidNo, tranD))
		return FALSE;
	else
	{
		if (tranD.GetNe()>0)
		{
			BOOL bRipple = (m_bytMsgType==MGT_FORCEUPDATE)?TRUE:FALSE; 
			int iBlockSize = (tranD.GetDataLen() - UCHAR_SIZE)/tranD.GetNe();
			return m_DCache->SetAField(m_lPageNo, tranD.GetData(),	(FID_DEFINITION)m_sFidNo, iBlockSize, tranD.GetNe(), m_DCache->GetTlogRipple((FID_DEFINITION)m_sFidNo), bRipple, m_pMsgObj);
		}
		else
		{
			TRACE("Drop Field, Item No: %x, FID No: %d, TRAN Ne=%d\n", m_lPageNo, m_sFidNo,tranD.GetNe());
			return m_DCache->DropAField(m_lPageNo, (FID_DEFINITION)m_sFidNo, m_pMsgObj);
		}
	}	
	return FALSE;
}

bool CDCacheFidIterator::SetTranD(short sFid, TranD &tranD)
{
	if (tranD.GetNe()>0)
	{
		BOOL bRipple = (m_bytMsgType==MGT_FORCEUPDATE)?TRUE:FALSE; 
		int iBlockSize = (tranD.GetDataLen() - UCHAR_SIZE)/tranD.GetNe();
		return m_DCache->SetAField(m_lPageNo, tranD.GetData(),	(FID_DEFINITION)sFid, iBlockSize, tranD.GetNe(), m_DCache->GetTlogRipple((FID_DEFINITION)m_sFidNo), bRipple, m_pMsgObj);
	}
	else
	{
		TRACE("Drop Field, Item No: %x, FID No: %d, TRAN Ne=%d\n", m_lPageNo, m_sFidNo,tranD.GetNe());
		return m_DCache->DropAField(m_lPageNo, (FID_DEFINITION)sFid, m_pMsgObj);
	}
}
