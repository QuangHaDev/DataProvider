/////////////////////////////////////////////////////////////////////////////
//          File:: Page.h
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
#ifndef __PAGE_H__
#define __PAGE_H__


#include "Definition.h"
#include <vector>
using namespace NGN2;


/////////////////////////////////////////////////////////////////////////////
// The CPage class is the base class of all pages
/////////////////////////////////////////////////////////////////////////////

typedef CMap<long,long,CObject*,CObject*&> CFieldsMap;
typedef unsigned long ULONG ;

#define DEFAULT_NO_OF_INIT_FIELDS	20

class AFX_EXT_CLASS CPage : public CObject
{
	DECLARE_DYNCREATE(CPage)

// Members
private :
	long		m_lPageNo;
	long		m_lNoOfFields;
	CObArray	m_arrayFields;
	BOOL		m_UpdateFlag;
	BOOL		m_WriteFlag;
	CFieldsMap	m_mapFields;
	POSITION	m_CurFieldPos;
	BYTE		m_bytRecType;
	BYTE		m_bytMarketCode;
	BYTE		m_bytSubMarket;
	CFieldsMap	m_mapDropFields;
	CString		m_strRicName;
	
	typedef struct _QE {
		unsigned short usQuoteLen;
		byte byteDataTL;
		byte * pbyQuote;
	} strQE;
	typedef std::vector<strQE*> QLFids;
	typedef std::vector<QLFids*> QLLines;
// Constructors & Destructors
public :
	// -------------------------------------------------------------------------
	// Default Constructor
	CPage();
	// -------------------------------------------------------------------------
	// Copy 
	CPage(const CPage* that);
	// -------------------------------------------------------------------------
	// Destructor
	virtual ~CPage();
	// -------------------------------------------------------------------------
	// Operator =
	CPage& operator=(const CPage& pSourcePage);

// Implementation
public :
	CPage(const CPage& that);
	void SetPageNo(long PageNo);
	long GetTotalNoOfFields();
	void RemoveFidFromDropList(FID_DEFINITION enumFieldID);
	BOOL IsFidExist(FID_DEFINITION enumFieldID);
	void SetRicName(CString strRicName) {m_strRicName = strRicName;}
	CString GetRicName() {return m_strRicName;}

	BOOL SetAField(unsigned char cInput,	FID_DEFINITION enumFieldID, long lIndex);												//UChar
	BOOL SetAField(short nInput,			FID_DEFINITION enumFieldID, long lIndex);												//Short
	BOOL SetAField(unsigned short uInput,	FID_DEFINITION enumFieldID, long lIndex);												//UShort
	BOOL SetAField(long lInput,				FID_DEFINITION enumFieldID, long lIndex);												//Long
	BOOL SetAField(ULONG ulInput,			FID_DEFINITION enumFieldID, long lIndex);												//ULong 
	BOOL SetAField(float fInput,			FID_DEFINITION enumFieldID, long lIndex);												//Float
	BOOL SetAField(double lfInput,			FID_DEFINITION enumFieldID, long lIndex);												//Double
																																
	BOOL SetAField(CharArray* pInput,		FID_DEFINITION enumFieldID, long lIndex, BOOL bCompare);								// CharArray
	BOOL SetAField(CharArray* pInput,		FID_DEFINITION enumFieldID, long lIndex, int iLineNo, BOOL bCompare);					// MCharArray
	BOOL SetAField(BYTE* pInput,			FID_DEFINITION enumFieldID, long lSizeOfBlock);											// General Type
	BOOL SetAField(BYTE* pInput,			FID_DEFINITION enumFieldID, int iBlockSize, int iTotalBlock, int iMaxNoOfBlock,			// Tran
											BOOL bRipple, unsigned char ucRipple);
	BOOL SetAField(BYTE* pInput,			FID_DEFINITION enumFieldID, long lPos, long iMaxNoOfBlock, long iSizeOfBlock);			// ShrOrdLst
	BOOL SetAField(BYTE* pInput,			FID_DEFINITION enumFieldID, long lPos, int iBlockSize, int iTotalBlock);				// PrcVol, HLink
	BOOL SetAField(BYTE* pInput,			FID_DEFINITION enumFieldID, long lPos, long iSizeOfBlock);								// Link, WLink, MtcPoints, MtcVal, MTCPTS, MTCF
	BOOL SetAField(BYTE* pInput,			FID_DEFINITION enumFieldID, long lIndex, int iSizeOfString);							// Headline
	BOOL SetAField(BYTE* pInput,			FID_DEFINITION enumFieldID, unsigned short usDataNe, byte byteDataTL);					// Quote List
	BOOL SetAField(BYTE* pInput,			FID_DEFINITION enumFieldID, long lPos, int iBlockSize, int iTotalBlock, int iHeaderSize, int iTotalSize);	// PrcVolwSeq

	BOOL DropMCharArrayLn(FID_DEFINITION enumFieldID, long lCurTl, long lNewTl);													// Drop lines for MCharArray
	FID_DEFINITION GetNextField();
	void FirstField();
	BOOL DropAField(FID_DEFINITION enumFieldID);

	void PreparePageInformation(void);
	BOOL GetAField(CharArray** pstrInput,	FID_DEFINITION enumFieldID, long* lLineNo);	// MCharArray, CharArray
	BOOL GetAField(BYTE** pInput,			FID_DEFINITION enumFieldID, bool & bDel );	// General Type, Tran, ShrOrdLst, Link, WLink, MtcPoints, MtcVal, HisVal, MTCPTS, MTCF
	BOOL GetAField(BYTE** pInput,			FID_DEFINITION enumFieldID, long* lLineNo);	// Headline
	BOOL GetAField(unsigned char& cInput,	FID_DEFINITION enumFieldID);				// char
	BOOL GetAField(short& nInput,			FID_DEFINITION enumFieldID);				// short
	BOOL GetAField(unsigned short& uInput,	FID_DEFINITION enumFieldID);				// unsigned short
	BOOL GetAField(long& lInput,			FID_DEFINITION enumFieldID);				// long 
	BOOL GetAField(ULONG& ulInput,			FID_DEFINITION enumFieldID);				// unsigned long
	BOOL GetAField(float& fInput,			FID_DEFINITION enumFieldID);				// float
	BOOL GetAField(double& lfInput,			FID_DEFINITION enumFieldID);				// double	
	BOOL GetAField(QuoteList* qlOutput,		FID_DEFINITION enumFieldID);				// Whole Quote List 
	BOOL GetAField(QuoteList* qlOutput,		FID_DEFINITION enumFieldID, byte byteLn);	// Quote line 
	BOOL GetAField(QuoteList* qlOutput,		FID_DEFINITION enumFieldID, byte byteLn, short sFid); // Quote Cell

	BOOL GetDropFIDList(CDWordArray& cdwaHeadlineList);									// Get Drop Fid List
	BOOL GetHeadLineFIDList(CDWordArray& cdwaHeadlineList, FID_DEFINITION enumFieldID);	// Get HeadLine List
	BOOL GetMCharArrayTL(FID_DEFINITION enumFieldID, long& NoOfElement);	// Get MCharArray TL

public:
	BYTE GetRecordType() {return m_bytRecType;}
	BYTE GetMarketCode() {return m_bytMarketCode;}
	BYTE GetSubMarket()  {return m_bytSubMarket;}

};
	
#endif //__PAGE_H__