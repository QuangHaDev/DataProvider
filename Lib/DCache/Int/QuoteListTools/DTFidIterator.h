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
#ifndef __DTFIDITERATOR_H__
#define __DTFIDITERATOR_H__


#include "ParsedFidIterator.h"

#pragma warning (disable:4800)
class CQuoteListToolsDlg;
class CDTFidIterator : public ParsedFidIterator
{
	enum {
		ULong = 1,
		UFloat, 
		UDouble, 
		CCharArray, 
		ECharrArray, 
		GBCharArray,
		UChar, 
		DateYear,
		Other
	};

// Members
private :
	CQuoteListToolsDlg*	m_QuoteListToolsDlg;
	long				m_lPageNo;
	byte				m_bytMsgType;
	short				m_sFidNo;
	BOOL				m_bCheckFid;
	BOOL				m_bShowQuoteList;
	short				m_sCompareFidNo;
// Constructors & Destructors
public :
	// -------------------------------------------------------------------------
	// Default Constructor
	CDTFidIterator(const byte* pbyMsg):ParsedFidIterator(pbyMsg){}


// Implementation
public :
	 void	SetMyDialog(CQuoteListToolsDlg* tmpQuoteListToolsDlg) {m_QuoteListToolsDlg = tmpQuoteListToolsDlg;}
	 void	SetInit(BOOL bShowQuoteList, BOOL bCheckFid = FALSE, short sCompareFidNo = -1) {m_bShowQuoteList = bShowQuoteList;m_bCheckFid = bCheckFid; m_sCompareFidNo = sCompareFidNo;}
	 short 	GetCurFidCode() {return ParsedFidIterator::GetCurFidCode();}
	 void	LogMsgToStatusList(CString strOutput, int iCodePage = 950);

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
	// Process Type D19 - Txn
	virtual bool ProcessTxn();

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
	// Process Type D31 - HisVal
	virtual bool ProcessHisVal();

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
	// Process Type D37 - MShrOrdLst
	virtual bool ProcessMShrOrdLst();

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

	void PrintChar(char& c);
	void PrintUnicode(WCHAR& c);
	void PrintShort(short& sh);
	void PrintUShort(unsigned short& ush);
	void PrintLong(long& l);
	void PrintULong(unsigned long& ul);
	void PrintFloat(float& f);
	void PrintUFloat(float& f);
	void PrintDouble(double& d );
	void PrintUDouble(double& d );
	void PrintCharArray(CharArray& caOld);
	void PrintGBCharArray(CharArray& caOld);
	void PrintMCharArray(MCharArray& mcaOld);
	void PrintBDateYear(BDateYear& dy);
	void PrintBDateMonth(BDateMonth& dm);
	void PrintBTimeMin(BTimeMin& tm);
	void PrintBTimeSec(BTimeSec& ts);
	void PrintBTimeRanges(BTimeRanges& tr);
	void PrintTxn(Txn& txn);
	void PrintTran(Tran& tran);
	void PrintShrOrdLst(ShrOrdLst& sol);
	void PrintPrcVol(PrcVol& prc);
	void PrintBkrQue(BkrQue& bkrque);
	void PrintLink(Link& link);
	void PrintWlink(WLink& wlink);
	void PrintYLabel(YLabel& ylabel);
	void PrintXMtcLabel(XMtcLabel& xmtcLbl);
	void PrintXDtcLabel(XDtcLabel& xdtcLbl);
	void PrintMtcPoints(MtcPoints& mtcPts);
	void PrintMtcVal(MtcVal& mtcval);
	void PrintDtcPoints(DtcPoints& dtcPts);
	void PrintHisVal(HisVal& hisval);
	void PrintFieldList(FieldList& fldLst);
	void PrintFieldLabel(FieldLabel& fldLblOld);
	void PrintInfoMsg(InfoMsg& infomsgOld);
	void PrintHeadLine(HeadLine& headline);
	void PrintHLink(HLink& hlink);
	void PrintMShrOrdLst(MShrOrdLst& mshr);
	void PrintSprTbl(SprTbl& sprtbl);
	void PrintQuoteList(QuoteList& quoteList);
	void PrintMTCTicks(MTCTicks& mtcTicks);
	void PrintMTCV(MTCV& mtcv);
	void PrintYHeadLine(YHeadLine& yheadline);
	void PrintOBkrQue(OBkrQue& obkrque);
	void PrintKMultiple(long& l);
	void Print10KMultiple(long& l);
    void PrintBQueVOSN(BQueVOSN& bquevosn);
	void PrintMTCPTS(MTCPTS& mtcPts);
	void PrintMTCF(MTCF& mtcf);
	void PrintPrcVolwSeq(PrcVolwSeq& prc);

	void PrintQuote(int iFidType, unsigned char& yLen, void*& pFV);
	void PrintQuote(short& sFid, unsigned char& yLen, void*& pFV);
	void LogMessage(CString strOutput);
	void LogMessageWithBlankLine(CString strOutput, int iCodePage = 950);

};



#endif //__DTFIDITERATOR_H__
  