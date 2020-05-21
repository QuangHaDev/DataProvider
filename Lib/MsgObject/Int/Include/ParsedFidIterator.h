/////////////////////////////////////////////////////////////////////////////
//          File:: ParsedFidIterator.h
// Create Author:: Sammy Ha
// Create   Date:: 30 September 2000
//
//   Description:: Message object class to wrap messages 
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
#ifndef __ParsedFidIterator_h__
#define __ParsedFidIterator_h__

#include "MsgObject.h"

/////////////////////////////////////////////////////////////////////////////
//
// Class      : ParsedFidIterator
// Description: ParsedFidIterator is to hold any array of parsed fids and provide 
//				methods for others to traverse the array
//
class ParsedFidIterator
{
public:
	// -------------------------------------------------------------------------
	// Constructor - create the iterator by a byte stream
	ParsedFidIterator(const byte* pbyMsg);

	// -------------------------------------------------------------------------
	// Default Destructor
//	virtual ~ParsedFidIterator();

// Operations
	// -------------------------------------------------------------------------
	// Move First
	void First();

	// -------------------------------------------------------------------------
	// Move Next
	void Next();

	// -------------------------------------------------------------------------
	// Check whether it is the end
	bool IsEnd() const;

	// -------------------------------------------------------------------------
	// Process the Current Fid
	bool ProcessFid();

	// -------------------------------------------------------------------------
	// Interface to get the m_nFid
	short GetCurFidCode() {return m_nFid;}

protected:
// Virtual functions for process different types of ParsedFid Data
// User can overwrite it as wish
	// -------------------------------------------------------------------------
	// Process Type D1
	virtual bool ProcessChar(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D2
	virtual bool ProcessUnicode(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D3
	virtual bool ProcessShort(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D4
	virtual bool ProcessUShort(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D5
	virtual bool ProcessLong(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D6
	virtual bool ProcessULong(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D7
	virtual bool ProcessFloat(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D8
	virtual bool ProcessUFloat(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D9
	virtual bool ProcessDouble(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D10
	virtual bool ProcessUDouble(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D11 - CharArray
	virtual bool ProcessCharArray(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D12 - MCharArray
	virtual bool ProcessMCharArray(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D13 - BDateYear
	virtual bool ProcessBDateYear(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D14 - BDateMonth
	virtual bool ProcessBDateMonth(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D15 - BTimeMin
	virtual bool ProcessBTimeMin(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D16 - BTimeSec
	virtual bool ProcessBTimeSec(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D17 - BTimeRanges
	virtual bool ProcessBTimeRanges(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D18 - Tran
	virtual bool ProcessTran(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D19 - Txn
	virtual bool ProcessTxn(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D20 - ShrOrdLst
	virtual bool ProcessShrOrdLst(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D21 - PrcVol
	virtual bool ProcessPrcVol(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D22 - BkrQue
	virtual bool ProcessBkrQue(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D23 - Link
	virtual bool ProcessLink(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D24 - Wlink
	virtual bool ProcessWlink(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D25 - YLabel
	virtual bool ProcessYLabel(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D26 - XMtcLabel
	virtual bool ProcessXMtcLabel(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D27 - XDtcLabel
	virtual bool ProcessXDtcLabel(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D28 - MtcPoints
	virtual bool ProcessMtcPoints(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D29 - MtcVal
	virtual bool ProcessMtcVal(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D30 - DtcPoints
	virtual bool ProcessDtcPoints(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D31 - HisVal
	virtual bool ProcessHisVal(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D32 - FieldList
	virtual bool ProcessFieldList(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D33 - FieldLabel
	virtual bool ProcessFieldLabel(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D34 - InfoMsg
	virtual bool ProcessInfoMsg(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D35 - NewsLine
	virtual bool ProcessHeadLine(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D36 - HLink
	virtual bool ProcessHLink(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D37 - MShrOrdLst
	virtual bool ProcessMShrOrdLst(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D38 - SprTbl
	virtual bool ProcessSprTbl(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D39 - QuoteList
	virtual bool ProcessQuoteList(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D40 - MTCTicks
	virtual bool ProcessMTCTicks(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D41 - MTCV
	virtual bool ProcessMTCV(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D42 - YHeadLine
	virtual bool ProcessYHeadLine(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D43 - OBkrQue
	virtual bool ProcessOBkrQue(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D44 - KMultiple
	virtual bool ProcessKMultiple(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D45 - 10KMultiple
	virtual bool Process10KMultiple(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D46 - BQueVOSN
	virtual bool ProcessBQueVOSN(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D47 - MTCPTS
	virtual bool ProcessMTCPTS(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D48 - MTCF
	virtual bool ProcessMTCF(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D49 - PrcVolwSeq
	virtual bool ProcessPrcVolwSeq(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D51 - TranD
	virtual bool ProcessTranD(){return TRUE;}

	// -------------------------------------------------------------------------
	// Process Type D52 - TxnD
	virtual bool ProcessTxnD(){return TRUE;}

protected:
// Attributes
	COP_ITEM	m_pMsgObj;
	long		m_lCurParsedFid;

private:
	short		m_nFid;
	short		m_nFidType;
};

#endif
