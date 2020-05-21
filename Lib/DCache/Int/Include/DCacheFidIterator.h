/////////////////////////////////////////////////////////////////////////////
//          File:: DCACHEFidIterator.h
// Create Author:: Raymond Yung
// Create   Date:: 1 September 00
//
//   Description:: DCache Class Declaration
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
#ifndef __DCACHEFIDITERATOR_H__
#define __DCACHEFIDITERATOR_H__


#include "DCache.h"
#include "ParsedFidIterator.h"

using namespace NGN2;

#pragma warning (disable:4800)

class CDCacheFidIterator : public ParsedFidIterator
{

// Members
private :
	CDCache*		m_DCache;
	long			m_lPageNo;
	byte			m_bytMsgType;
	short			m_sFidNo;
// Constructors & Destructors
public :
	// -------------------------------------------------------------------------
	// Default Constructor
	CDCacheFidIterator(const byte* pbyMsg):ParsedFidIterator(pbyMsg){}


// Implementation
public :
	 void	SetMyDCache(CDCache* tmpDCache) {m_DCache = tmpDCache;}
	 short 	GetCurFidCode() {return ParsedFidIterator::GetCurFidCode();}

public:
	bool ProcessFid();
	
	// -------------------------------------------------------------------------
	// Process Type D1
	virtual bool ProcessChar();

	// -------------------------------------------------------------------------
	// Process Type D2
	virtual bool ProcessUnicode();

	// -------------------------------------------------------------------------
	// Process Type D3
	virtual bool ProcessShort();

	// -------------------------------------------------------------------------
	// Process Type D4
	virtual bool ProcessUShort();

	// -------------------------------------------------------------------------
	// Process Type D5
	virtual bool ProcessLong();

	// -------------------------------------------------------------------------
	// Process Type D6
	virtual bool ProcessULong();

	// -------------------------------------------------------------------------
	// Process Type D7
	virtual bool ProcessFloat();

	// -------------------------------------------------------------------------
	// Process Type D8
	virtual bool ProcessUFloat();

	// -------------------------------------------------------------------------
	// Process Type D9
	virtual bool ProcessDouble();

	// -------------------------------------------------------------------------
	// Process Type D10
	virtual bool ProcessUDouble();

	// -------------------------------------------------------------------------
	// Process Type D11 - CharArray
	virtual bool ProcessCharArray();

	// -------------------------------------------------------------------------
	// Process Type D12 - MCharArray
	virtual bool ProcessMCharArray();

	// -------------------------------------------------------------------------
	// Process Type D13 - BDateYear
	virtual bool ProcessBDateYear();

	// -------------------------------------------------------------------------
	// Process Type D14 - BDateMonth
	virtual bool ProcessBDateMonth();

	// -------------------------------------------------------------------------
	// Process Type D15 - BTimeMin
	virtual bool ProcessBTimeMin();

	// -------------------------------------------------------------------------
	// Process Type D16 - BTimeSec
	virtual bool ProcessBTimeSec();

	// -------------------------------------------------------------------------
	// Process Type D17 - BTimeRanges
	virtual bool ProcessBTimeRanges();

	// -------------------------------------------------------------------------
	// Process Type D18 - Tran
	virtual bool ProcessTran();

	// -------------------------------------------------------------------------
	// Process Type D20 - ShrOrdLst
	virtual bool ProcessShrOrdLst();

	// -------------------------------------------------------------------------
	// Process Type D21 - PrcVol
	virtual bool ProcessPrcVol();

	// -------------------------------------------------------------------------
	// Process Type D22 - BkrQue
	virtual bool ProcessBkrQue();

	// -------------------------------------------------------------------------
	// Process Type D23 - Link
	virtual bool ProcessLink();

	// -------------------------------------------------------------------------
	// Process Type D24 - Wlink
	virtual bool ProcessWlink();

	// -------------------------------------------------------------------------
	// Process Type D25 - YLabel
	virtual bool ProcessYLabel();

	// -------------------------------------------------------------------------
	// Process Type D26 - XMtcLabel
	virtual bool ProcessXMtcLabel();

	// -------------------------------------------------------------------------
	// Process Type D27 - XDtcLabel
	virtual bool ProcessXDtcLabel();

	// -------------------------------------------------------------------------
	// Process Type D28 - MtcPoints
	virtual bool ProcessMtcPoints();

	// -------------------------------------------------------------------------
	// Process Type D29 - MtcVal
	virtual bool ProcessMtcVal();

	// -------------------------------------------------------------------------
	// Process Type D30 - DtcPoints
	virtual bool ProcessDtcPoints();

	// -------------------------------------------------------------------------
	// Process Type D32 - FieldList
	virtual bool ProcessFieldList();

	// -------------------------------------------------------------------------
	// Process Type D33 - FieldLabel
	virtual bool ProcessFieldLabel();

	// -------------------------------------------------------------------------
	// Process Type D34 - InfoMsg
	virtual bool ProcessInfoMsg();

	// -------------------------------------------------------------------------
	// Process Type D35 - NewsLine
	virtual bool ProcessHeadLine();

	// -------------------------------------------------------------------------
	// Process Type D36 - HLink
	virtual bool ProcessHLink();

	// -------------------------------------------------------------------------
	// Process Type D38 - SprTbl
	virtual bool ProcessSprTbl();

	// -------------------------------------------------------------------------
	// Process Type D39 - QuoteList
	virtual bool ProcessQuoteList();

	// -------------------------------------------------------------------------
	// Process Type D40 - MTCTicks
	virtual bool ProcessMTCTicks();

	// -------------------------------------------------------------------------
	// Process Type D41 - MTCV
	virtual bool ProcessMTCV();

	// -------------------------------------------------------------------------
	// Process Type D42 - YHeadLine
	virtual bool ProcessYHeadLine();

	// -------------------------------------------------------------------------
	// Process D43 - OBkrQue
	virtual	bool ProcessOBkrQue();

	// -------------------------------------------------------------------------
	// Process Type D44 - KMultiple
	virtual bool ProcessKMultiple();

	// -------------------------------------------------------------------------
	// Process Type D45 - 10KMultiple
	virtual bool Process10KMultiple();

	// -------------------------------------------------------------------------
	// Process Type D46 - BQueVOSN
    virtual bool ProcessBQueVOSN();

	// -------------------------------------------------------------------------
	// Process Type D47 - MTCPTS
	virtual bool ProcessMTCPTS();

	// -------------------------------------------------------------------------
	// Process Type D48 - MTCF
	virtual bool ProcessMTCF();

	// -------------------------------------------------------------------------
	// Process Type D49 - PrcVolwSeq
	virtual bool ProcessPrcVolwSeq();

	// Process Type D51 - Tran
	virtual bool ProcessTranD();

	bool GetProcessTran(short sFid, Tran &Tran, COP_ITEM &copMsg);
	bool GetProcessChar(short sFid, char &c);
	bool SetTranD(short sFid, TranD &tranD);
	bool GetProcessCharArray(short sFid, CharArray &ca);
};



#endif //__DCACHEFIDITERATOR_H__
  