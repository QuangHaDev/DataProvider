/////////////////////////////////////////////////////////////////////////////
//          File:: ParsedFidIterator.cpp
// Create Author:: Sammy Ha
// Create   Date:: 30 Semptember 2000
//
//   Description:: The implementation file of the ParsedFidIterator.h
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

#include "MsgObject.h"
#include "ParsedFidIterator.h"
//#include "ApplManClient.h"

// -------------------------------------------------------------------------
// Constructor
ParsedFidIterator::ParsedFidIterator(const byte* pbyMsg)
{
	m_pMsgObj = NULL;
	m_pMsgObj = new MsgObject(pbyMsg);
}

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
void ParsedFidIterator::First()
{
	m_lCurParsedFid = 0;
	m_nFidType = m_pMsgObj->GetFidType((short) m_lCurParsedFid, m_nFid);
}

// -------------------------------------------------------------------------
// Move the counter by one
void ParsedFidIterator::Next()
{
	m_lCurParsedFid++;
	m_nFidType = m_pMsgObj->GetFidType((short) m_lCurParsedFid, m_nFid);
}

// -------------------------------------------------------------------------
// check whether the iterator reach the end
bool ParsedFidIterator::IsEnd() const
{
/*	short nFidsCount = m_pMsgObj->GetFidsCount();
	if (nFidsCount > 0)
		return m_lCurParsedFid >= nFidsCount;

	return FALSE;
*/
	if (m_nFidType == 0)
		return TRUE;
	return FALSE;
}

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
		case NGN_SPRTBL:
			retVal = ProcessSprTbl()	;
			break;
		case NGN_QUOTELIST:
			retVal = ProcessQuoteList()	;
			break;
		case NGN_MTCTICKS:
			retVal = ProcessMTCTicks()	;
			break;
		case NGN_MTCV:
			retVal = ProcessMTCV()	;
			break;
		case NGN_YHEADLINE:
			retVal = ProcessYHeadLine()	;
			break;
		case NGN_OBKRQUE:
			retVal = ProcessOBkrQue()	;
			break;
		case NGN_KMULTIPLE:
			retVal = ProcessKMultiple()	;
			break;
		case NGN_10KMULTIPLE:
			retVal = Process10KMultiple()	;
			break;
        case NGN_BQUEVOSN:
            retVal = ProcessBQueVOSN();
            break;
		case NGN_MTCPTS:
			retVal = ProcessMTCPTS()	;
			break;
		case NGN_MTCF:
			retVal = ProcessMTCF()	;
			break;
		case NGN_PRCVOLWSEQ:
			retVal = ProcessPrcVolwSeq()	;
			break;
		case NGN_TRAN_D:
			retVal = ProcessTranD()	;
			break;
		case NGN_TXN_D:
			retVal = ProcessTxnD()	;
			break;
		default:
			break;
	}

	return retVal;
}
