/////////////////////////////////////////////////////////////////////////////
//          File:: Page.cpp
// Create Author:: Raymond Yung
// Create   Date:: 1 September 00
//
//   Description:: CPage Class member function
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

#include "stdafx.h"
#include "DCache.h"
#include "ApplManClient.h"
#include <vector>
#include <map>
//#include "minilzo.h"

//#define HEAP_ALLOC(var,size) lzo_align_t __LZO_MMODEL var [ ((size) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t) ]

static bool blzo = false; 

//static bool blzo = (lzo_init() == LZO_E_OK);

#define lzostr "LZO1"

bool GzipString(const char *in,unsigned long len, char* &out)
{
	return false;
	/* 
	if( !blzo || in == NULL) {
		return false;
	}

	if( len < 3000 ) { // force compression if data bigger than 3kbytes
		if(!CDCache::g_bCompression || len < 150)
			return false;
	}
	
	static unsigned char tempBuff[30000];
	unsigned long destLen = 30000;
	if( destLen < len +12)
		return false;

	if( lzo1x_1_compress((unsigned char *)in,len,tempBuff,&destLen,wrkmem) == LZO_E_OK ) {
		out = new char[destLen+sizeof(unsigned long)*2+4];
		memcpy(out+sizeof(unsigned long)+8,tempBuff,destLen);
		memcpy(out, lzostr,4);
		memcpy(out+4,&destLen,sizeof(unsigned long));
		memcpy(out+4+ sizeof(unsigned long),&len,sizeof(unsigned long));
		return true;
	}
	return false;*/
}
bool GunzipString(const char *in, char* &out, unsigned long& olen)
{
	return false;
	/*
	ASSERT(out==NULL);
	if(!blzo || in == NULL || memcmp(in, lzostr, 4) != 0) {
		return false;
	}
	unsigned long size =0;	
	memcpy(&size,in+4,sizeof(unsigned long));
	memcpy(&olen,in+4+sizeof(unsigned long),sizeof(unsigned long));
	out = new char[olen];

    if( lzo1x_decompress((unsigned char *)in+4+sizeof(unsigned long)*2,size,(unsigned char *)out,&olen,NULL) != LZO_E_OK ) {
		delete out;
		out = NULL;
		olen = 0;
		return false;
	}
	return true;*/
}


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CPage, CObject)
// -------------------------------------------------------------------------
// Default Constructor
// -------------------------------------------------------------------------

CPage::CPage():	
	m_lPageNo(0L),
	m_lNoOfFields(0L),
	m_UpdateFlag(FALSE),
	m_WriteFlag(FALSE),
	m_bytRecType(0),
	m_bytMarketCode(0),
	m_bytSubMarket(0),
	m_CurFieldPos(NULL),
	m_strRicName("")
{
	m_mapFields.InitHashTable( DEFAULT_NO_OF_INIT_FIELDS );
}
// -------------------------------------------------------------------------
// Copy 
// -------------------------------------------------------------------------
CPage::CPage(const CPage* that)
{
	int MaxSizeOfFields = this->m_mapFields.GetHashTableSize();
	m_mapFields.InitHashTable(MaxSizeOfFields);
	CObject* tempObj; 
	for (int i=0; i<MaxSizeOfFields; i++)
	{	
		that->m_mapFields.Lookup( i, tempObj);
		m_mapFields[i] = tempObj;
	}
	m_lNoOfFields = that->m_lNoOfFields;
	m_lPageNo = that->m_lPageNo;
}

// -------------------------------------------------------------------------
// Destructor
// -------------------------------------------------------------------------
CPage::~CPage()
{

   // Remove the elements with even key values.
	long		nKey;
	CObject*	objTemp;
	POSITION	pos = m_mapFields.GetStartPosition();
	fidDatabase fDB;

	while (pos != NULL)
	{
		m_mapFields.GetNextAssoc( pos, nKey, objTemp );
		DropAField((FID_DEFINITION)nKey);
	}
	m_mapFields.RemoveAll();
}
// -------------------------------------------------------------------------
// operator =
CPage& CPage::operator=(const CPage& that)	
{
	m_lPageNo = that.m_lPageNo;
	return(*this);
}

// -------------------------------------------------------------------------
// Page Setting Functions
// -------------------------------------------------------------------------
void CPage::PreparePageInformation()
{
	return;
}
// -------------------------------------------------------------------------
void CPage::SetPageNo(long PageNo)
{
	m_lPageNo = PageNo;
}
// -------------------------------------------------------------------------
long CPage::GetTotalNoOfFields()
{
	return m_lNoOfFields;
}
// -------------------------------------------------------------------------
FID_DEFINITION CPage::GetNextField()
{
	long		nKey = FID_END;
	CObject*	objTemp;
	if (m_CurFieldPos != NULL)
		m_mapFields.GetNextAssoc( m_CurFieldPos, nKey, objTemp );
	return (FID_DEFINITION) nKey;
}
// -------------------------------------------------------------------------
void CPage::FirstField()
{
	m_CurFieldPos = m_mapFields.GetStartPosition();
}

// -------------------------------------------------------------------------
BOOL CPage::IsFidExist(FID_DEFINITION enumFieldID)
{
	CObject* tempObject = NULL;
	return m_mapFields.Lookup(enumFieldID, (CObject*&)tempObject);
}

// -------------------------------------------------------------------------
// Retrieve Functions
// -------------------------------------------------------------------------
// HeadlineList
BOOL CPage::GetHeadLineFIDList(CDWordArray& cdwaHeadlineList, FID_DEFINITION enumFieldID)	
{
	BOOL bReturnValue = FALSE;
	CObject* tempObject = NULL;
	CObject* tempObject1 = NULL;
	if (bReturnValue = m_mapFields.Lookup(enumFieldID, (CObject*&)tempObject))
	{
		CFieldsMap* cfmSubFields = (CFieldsMap*) tempObject;
		long		nKey;
		POSITION pos = cfmSubFields->GetStartPosition();
		while (pos != NULL)
		{
			cfmSubFields->GetNextAssoc( pos, nKey, tempObject1 );
			cdwaHeadlineList.Add(nKey);
			bReturnValue = TRUE;
		}		
	}
	return bReturnValue;
}

// -------------------------------------------------------------------------
BOOL CPage::GetDropFIDList(CDWordArray& cdwaDropFidList)	
{
	long		nKey;
	POSITION	posCurPage;
	BOOL		bReturnValue = FALSE;
	if ((posCurPage = m_mapDropFields.GetStartPosition())!=NULL)
	{
		CObject*		objTemp;
		while (posCurPage != NULL)
		{
			m_mapDropFields.GetNextAssoc( posCurPage, nKey, objTemp );
			cdwaDropFidList.Add(nKey);
			bReturnValue = TRUE;
		}
	}
	else
		bReturnValue = FALSE;
	return bReturnValue;
}

// -------------------------------------------------------------------------
BOOL CPage::GetAField(short& nOutput, FID_DEFINITION enumFieldID)
{
	BOOL bReturnValue = FALSE;
	CObject* tempObject = NULL;
	if (bReturnValue = m_mapFields.Lookup(enumFieldID, (CObject*&)tempObject))
		nOutput = *(short*) tempObject;
	return bReturnValue;
}

// -------------------------------------------------------------------------
BOOL CPage::GetAField(unsigned short& uOutput, FID_DEFINITION enumFieldID)
{
	BOOL bReturnValue = FALSE;
	CObject* tempObject = NULL;		
	if (bReturnValue = m_mapFields.Lookup(enumFieldID, (CObject*&)tempObject))
		uOutput = *(unsigned short*)tempObject;
	return bReturnValue;
}
// -------------------------------------------------------------------------
BOOL CPage::GetAField(long& lOutput, FID_DEFINITION enumFieldID)
{
	BOOL bReturnValue = FALSE;
	CObject* tempObject = NULL;
	if (bReturnValue = m_mapFields.Lookup(enumFieldID, (CObject*&)tempObject))
		lOutput = *(long*)tempObject;
	return bReturnValue;
}
// -------------------------------------------------------------------------
BOOL CPage::GetAField(unsigned long& ulOutput, FID_DEFINITION enumFieldID)
{
	BOOL bReturnValue = FALSE;
	CObject* tempObject = NULL;
	if (bReturnValue = m_mapFields.Lookup(enumFieldID, (CObject*&)tempObject))
		ulOutput = *(unsigned long*)tempObject;
	return bReturnValue;
}
// -------------------------------------------------------------------------
BOOL CPage::GetAField(float& fOutput, FID_DEFINITION enumFieldID)
{
	BOOL bReturnValue = FALSE;
	CObject* tempObject = NULL;
	if (bReturnValue = m_mapFields.Lookup(enumFieldID, (CObject*&)tempObject))
		fOutput = *(float*)tempObject;
	return bReturnValue;
}
// -------------------------------------------------------------------------
BOOL CPage::GetAField(double& lfOutput, FID_DEFINITION enumFieldID)
{
	BOOL bReturnValue = FALSE;
	CObject* tempObject = NULL;
	if (bReturnValue = m_mapFields.Lookup(enumFieldID, (CObject*&)tempObject))
		lfOutput = *(double*)tempObject;
	return bReturnValue;
}
// -------------------------------------------------------------------------
BOOL CPage::GetAField(unsigned char& cOutput, FID_DEFINITION enumFieldID)
{
	BOOL bReturnValue = FALSE;
	CObject* tempObject = NULL;
	if (bReturnValue = m_mapFields.Lookup(enumFieldID, (CObject*&)tempObject))
		cOutput = *(unsigned char*)tempObject;
	return bReturnValue;
}
// -------------------------------------------------------------------------
// MCharArray, CharArray
BOOL CPage::GetAField(CharArray** pstrInput, FID_DEFINITION enumFieldID, long* lLineNo)	
{
	BOOL bReturnValue = FALSE;	
	CObject* tempObject = NULL;
	if (bReturnValue = m_mapFields.Lookup(enumFieldID, (CObject*&)tempObject))
	{
		if (*lLineNo == -2) // if CharArray
			*pstrInput = (CharArray*) tempObject ;
		else
		{
			CObArray*	arrayTempSubFields = (CObArray*) tempObject ;
			*lLineNo = (*lLineNo==-1)?arrayTempSubFields->GetUpperBound():*lLineNo;
			if (*lLineNo > arrayTempSubFields->GetUpperBound())
				*pstrInput = NULL;
			else
				*pstrInput = (CharArray*) arrayTempSubFields->GetAt(*lLineNo);
		}
	}
	return bReturnValue;
}

// -------------------------------------------------------------------------
// General Type, ShrOrdLst, Link, WLink, MtcPoints, MtcVal, HisVal, SprTbl, MTCPTS, MTCF
BOOL CPage::GetAField(BYTE** pInput, FID_DEFINITION enumFieldID, bool & bDel)
{
	BOOL bReturnValue = FALSE;
	CObject* tempObject = NULL;
	if (bReturnValue = m_mapFields.Lookup(enumFieldID, (CObject*&)tempObject)) {
		unsigned long len = 0;
		char * bytField = NULL;
		if( GunzipString((char*)tempObject,bytField,len) ) {
			*pInput = (BYTE *) bytField; 
			bDel = true;
			//*pInput = bytField;
		} else {
			if( bytField )
				delete bytField;
			*pInput = (BYTE*) tempObject; 
			bDel = false;
		}
	}
	return bReturnValue;
}

// -------------------------------------------------------------------------
// Headline
BOOL CPage::GetAField(BYTE** pInput, FID_DEFINITION enumFieldID, long* lLineNo)
{
	BOOL bReturnValue = FALSE;

	CObject* tempObject = NULL;
	if (m_mapFields.Lookup(enumFieldID, (CObject*&)tempObject))
	{
		CObject* tempObj;
		CFieldsMap*	cfmSubFields = (CFieldsMap*) tempObject;
		if (cfmSubFields->Lookup(*lLineNo, (CObject*&)tempObj))
		{
			*pInput = (BYTE*) tempObj;
			bReturnValue = TRUE;
		}
		else
			pInput = NULL;
	}
	return bReturnValue;
}


// -------------------------------------------------------------------------
// Whole Quote List 
BOOL CPage::GetAField(QuoteList* qlOutput, FID_DEFINITION enumFieldID)
{
	BOOL		bReturnValue	= FALSE;	
	CObject*	tempObject		= NULL;
	int			iTotalLine		= 0;

	if (bReturnValue = m_mapFields.Lookup(enumFieldID, (CObject*&)tempObject))
	{
		QLLines* arrayLines = (QLLines*)tempObject;
		iTotalLine = arrayLines->size();
		for (int iLine = 0; iLine < iTotalLine; iLine++)
		{
			QLFids* arrayFid = (QLFids*)arrayLines->at(iLine);
			if (arrayFid != NULL)
			{
				strQE* qlOrgList;
				int iTotalFid = arrayFid->size();
				for (int iFid = 0; iFid < iTotalFid; iFid++)
				{
					qlOrgList = (strQE*)arrayFid->at(iFid);
					if (qlOrgList != NULL)
					{

					/*	QuoteList NewQuoteList;
						NewQuoteList.AddQuote(qlOrgList->pbyQuote, qlOrgList->usQuoteLen);
						NewQuoteList.SetTL(qlOrgList->byteDataTL);

						QuoteList qlTempList(NewQuoteList.GetData());
						byte* pQuote; 
						unsigned short usQuoteLen;
						qlTempList.GetQuote(0, pQuote, usQuoteLen);*/
						qlOutput->AddQuote(qlOrgList->pbyQuote, qlOrgList->usQuoteLen);
						bReturnValue = TRUE;
					}
				}
			}
		}		
	}
	if (bReturnValue)
		qlOutput->SetTL((byte) iTotalLine);
	return bReturnValue;
}
// -------------------------------------------------------------------------
// Quote line 
BOOL CPage::GetAField(QuoteList* qlOutput, FID_DEFINITION enumFieldID, byte byteLn)
{
	BOOL		bReturnValue = FALSE;
	CObject*	tempObject		= NULL;
	int			iTotalLine		= 0;

	if (bReturnValue = m_mapFields.Lookup(enumFieldID, (CObject*&)tempObject))
	{
		QLLines* arrayLines = (QLLines*)tempObject;
		iTotalLine = arrayLines->size();
		if (iTotalLine >= byteLn) {
			QLFids* arrayFid = (QLFids*)arrayLines->at(byteLn);
			if (arrayFid != NULL)
			{
				int iTotalFid = arrayFid->size();
				for (int iFid = 0; iFid < iTotalFid; iFid++)
				{
					strQE* qlOrgList;
				
					qlOrgList = (strQE*)arrayFid->at(iFid);
					//qlOrgList = (QuoteList*)arrayFid->GetAt(iFid);
					if (qlOrgList != NULL)
					{
						/*QuoteList qlTempList(qlOrgList->GetData());
						byte* pQuote;
						unsigned short usQuoteLen;
						qlTempList.GetQuote(0, pQuote, usQuoteLen);
						qlOutput->AddQuote(pQuote, usQuoteLen);*/
						qlOutput->AddQuote(qlOrgList->pbyQuote, qlOrgList->usQuoteLen);
						bReturnValue = TRUE;
					}
					
				}
			}
		}
	}
	if (bReturnValue)
		qlOutput->SetTL((byte) iTotalLine);
	return bReturnValue;
}
// -------------------------------------------------------------------------
// Quote Cell
BOOL CPage::GetAField(QuoteList* qlOutput, FID_DEFINITION enumFieldID, byte byteLn, short sFid)
{
	BOOL		bReturnValue = FALSE;
	CObject*	tempObject		= NULL;
	int			iTotalLine		= 0;
	if (bReturnValue = m_mapFields.Lookup(enumFieldID, (CObject*&)tempObject))
	{
		QLLines* arrayLines = (QLLines*)tempObject;
		iTotalLine = arrayLines->size();
		if (iTotalLine>byteLn)
		{
			QLFids* arrayFid = (QLFids*)arrayLines->at(byteLn);
			if (arrayFid != NULL)
			{
				strQE* qlOrgList;
				if (arrayFid->size()>sFid)
				{
					qlOrgList = (strQE*)arrayFid->at(sFid);
					if (qlOrgList != NULL)
					{
						/*QuoteList qlTempList(qlOrgList->GetData());
						byte* pQuote; 
						unsigned short usQuoteLen;
						qlTempList.GetQuote(0, pQuote, usQuoteLen);
						qlOutput->AddQuote(pQuote, usQuoteLen);*/
						qlOutput->AddQuote(qlOrgList->pbyQuote, qlOrgList->usQuoteLen);
						bReturnValue = TRUE;
					}
				}
			}
		}
	}
	if (bReturnValue)
		qlOutput->SetTL((byte) iTotalLine);
	return bReturnValue;
}


// -------------------------------------------------------------------------
// Get MCharArray TL
BOOL CPage::GetMCharArrayTL(FID_DEFINITION enumFieldID, long& NoOfElement)
{
	BOOL bReturnValue = FALSE;	
	CObject* tempObject = NULL;
	if (bReturnValue = m_mapFields.Lookup(enumFieldID, (CObject*&)tempObject))
	{
		CObArray*	arrayTempSubFields = (CObArray*) tempObject ;
		NoOfElement = arrayTempSubFields->GetUpperBound()+1;
	}
	return bReturnValue;
}

// -------------------------------------------------------------------------
// Updating Functions
// -------------------------------------------------------------------------
BOOL CPage::SetAField(unsigned char cInput, FID_DEFINITION enumFieldID, long lIndex)
{
	BOOL bReturnValue = FALSE;
	CObject* tempObject = NULL;

	if (!m_mapFields.Lookup(enumFieldID, (CObject*&)tempObject))
	{
		unsigned char* cContent = new unsigned char;
		*cContent  = cInput;
		m_mapFields[enumFieldID] = (CObject*) cContent;
		m_lNoOfFields ++;
		bReturnValue = TRUE;
	}
	else
	{
		if (*(unsigned char*)tempObject != cInput)
		{
			*(unsigned char*)m_mapFields[enumFieldID] = cInput;
			bReturnValue = TRUE;
		}
	}
	if (bReturnValue)
	{
		switch (enumFieldID)
		{
			case FID_REC_TYPE:
				m_bytRecType = cInput;
				break;
			case FID_MKT_CODE:
				m_bytMarketCode = cInput;
				break;
			case FID_SUB_MKT:
				m_bytSubMarket = cInput;
				break;
			default:
				break;
		}
	}
	return bReturnValue;
}
// -------------------------------------------------------------------------
BOOL CPage::SetAField(short nInput, FID_DEFINITION enumFieldID, long lIndex)
{
	BOOL bReturnValue = FALSE;
	CObject* tempObject = NULL;
	if (!m_mapFields.Lookup(enumFieldID, (CObject*&)tempObject))
	{
		short* nContent = new short;
		*nContent  = nInput;
		m_mapFields[enumFieldID] = (CObject*) nContent;
		m_lNoOfFields ++;
		bReturnValue = TRUE;
	}
	else
	{
		if (*(short*)tempObject != nInput)
		{
			*(short*)m_mapFields[enumFieldID] = nInput;
			bReturnValue = TRUE;
		}
	}
	return bReturnValue;
}
// -------------------------------------------------------------------------
BOOL CPage::SetAField(unsigned short uInput, FID_DEFINITION enumFieldID, long lIndex)
{
	BOOL bReturnValue = FALSE;
	CObject* tempObject = NULL;
	if (!m_mapFields.Lookup(enumFieldID, (CObject*&)tempObject))
	{
		unsigned short* uContent = new unsigned short;
		*uContent  = uInput;
		m_mapFields[enumFieldID] = (CObject*) uContent;
		m_lNoOfFields ++;
		bReturnValue = TRUE;
	}
	else
	{
		if (*(unsigned short*)tempObject != uInput)
		{
			*(unsigned short*)m_mapFields[enumFieldID] = uInput;
			bReturnValue = TRUE;
		}
	}
	return bReturnValue;
}

// -------------------------------------------------------------------------
BOOL CPage::SetAField(long lInput, FID_DEFINITION enumFieldID, long lIndex)
{
	BOOL bReturnValue = FALSE;
	CObject* tempObject = NULL;
	if (!m_mapFields.Lookup(enumFieldID, (CObject*&)tempObject))
	{
		long* lContent = new long;
		*lContent  = lInput;
		m_mapFields[enumFieldID] = (CObject*) lContent;
		m_lNoOfFields ++;
		bReturnValue = TRUE;
	}
	else
	{
		if (*(long*)tempObject != lInput)
		{
			*(long*)m_mapFields[enumFieldID] = lInput;
			bReturnValue = TRUE;
		}
	}
	return bReturnValue;
}
// -------------------------------------------------------------------------
BOOL CPage::SetAField(ULONG ulInput, FID_DEFINITION enumFieldID, long lIndex)
{
	BOOL bReturnValue = FALSE;
	CObject* tempObject = NULL;
	if (!m_mapFields.Lookup(enumFieldID, (CObject*&)tempObject))
	{
		ULONG* ulContent = new unsigned long;
		*ulContent  = ulInput;
		m_mapFields[enumFieldID] = (CObject*) ulContent;
		m_lNoOfFields ++;
		bReturnValue = TRUE;
	}
	else
	{
		if (*(ULONG*)tempObject != ulInput)
		{
			*(ULONG*)m_mapFields[enumFieldID] = ulInput;
			bReturnValue = TRUE;
		}
	}
	return bReturnValue;
}
// -------------------------------------------------------------------------
BOOL CPage::SetAField(float fInput, FID_DEFINITION enumFieldID, long lIndex)
{
	BOOL bReturnValue = FALSE;
	CObject* tempObject = NULL;
	if (!m_mapFields.Lookup(enumFieldID, (CObject*&)tempObject))
	{
		float* fContent = new float;
		*fContent  = fInput;
		m_mapFields[enumFieldID] = (CObject*) fContent;
		m_lNoOfFields ++;
		bReturnValue = TRUE;
	}
	else
	{
		if (*(float*)tempObject != fInput)
		//if( memcmp(&fInput, tempObject, sizeof(float))!=0)
		{
			*(float*)m_mapFields[enumFieldID] = fInput;
			bReturnValue = TRUE;
		}
	}
	return bReturnValue;
}
// -------------------------------------------------------------------------
BOOL CPage::SetAField(double lfInput, FID_DEFINITION enumFieldID, long lIndex)
{
	BOOL bReturnValue = FALSE;
	CObject* tempObject = NULL;
	if (!m_mapFields.Lookup(enumFieldID, (CObject*&)tempObject))
	{
		double* lfContent = new double;
		*lfContent  = lfInput;
		m_mapFields[enumFieldID] = (CObject*) lfContent;
		m_lNoOfFields ++;
		bReturnValue = TRUE;
	}
	else
	{
		if (*(double*)tempObject != lfInput)
		//if( memcmp(&lfInput, tempObject, sizeof(double))!=0)
		{
			*(double*)m_mapFields[enumFieldID] = lfInput;
			bReturnValue = TRUE;
		}
	}
	return bReturnValue;
}

// -------------------------------------------------------------------------
// this function is used to set a field that stores one string only 
// CharArray
BOOL CPage::SetAField(CharArray* pInput, FID_DEFINITION enumFieldID, long lIndex, BOOL bCompare)
{
	BYTE*		pString			= NULL;
	CObject*	tempObject		= NULL;
	BOOL		bEqual			= FALSE;	
	BOOL		bReturnValue	= FALSE;
	BOOL		bClearFullField = ((pInput->GetLen()== 255) && (pInput->GetPos()== 0))?TRUE:FALSE;
	BOOL		bFullFieldUpdate= (pInput->GetPos()==255)?TRUE:FALSE;
	int			iNewStringPos	= (pInput->GetPos()==255)?0:(int) pInput->GetPos();
	int			iNewStringLen	= (int) pInput->GetLen();
	int			iSizeOfString	= iNewStringLen+iNewStringPos;
	CharArray*	caCurContent=NULL;

	if (!m_mapFields.Lookup(enumFieldID, (CObject*&)tempObject))
	{
		if (bClearFullField)
			return FALSE;
		m_lNoOfFields ++;
		pString = new BYTE[iSizeOfString];
		memset(pString, ' ', iSizeOfString);
	}
	else
	{
		CharArray*	caCurContent = (CharArray*) tempObject ;
		int			iCurStringLen = (int) caCurContent->GetLen();
		// Full field clear
		if (bClearFullField)
		{
			m_mapFields[enumFieldID] = NULL;
			m_mapFields.RemoveKey(enumFieldID);
			delete caCurContent;
			return TRUE;
		}
		// Full Field updated
		if (bFullFieldUpdate)
			if (caCurContent->GetLen()!=iNewStringLen)
			{
				iCurStringLen = 0;
				bCompare = FALSE;
			}

		if (bCompare)
			if ((iSizeOfString==iCurStringLen) && (memcmp( pInput->GetString(), (caCurContent->GetString())+iNewStringPos, iNewStringLen) == 0))
				return FALSE;

		if (!bEqual)
		{	
			iSizeOfString = (iSizeOfString<iCurStringLen)?iCurStringLen:iSizeOfString;
			pString = new BYTE[iSizeOfString];
			memset(pString, ' ', iSizeOfString);
			memcpy(pString, caCurContent->GetString(), iCurStringLen);
			if (caCurContent!=NULL)
				delete caCurContent;
		}
	}

	if (!bEqual)
	{
		CharArray*	pContent = new CharArray; 
		memcpy(pString+iNewStringPos, pInput->GetString(), iNewStringLen);
		pContent->SetLen(iSizeOfString);
		pContent->SetSq(pInput->GetSq());
		pContent->SetString(pString);
		pContent->SetPos(255);
		m_mapFields[enumFieldID] = (CObject*) pContent;
		bReturnValue = TRUE;
		delete[] pString;
	}
	return bReturnValue;
}


// MCharArray
// -------------------------------------------------------------------------
BOOL CPage::SetAField(CharArray* pInput, FID_DEFINITION enumFieldID, long lIndex, int iLineNo, BOOL bCompare)
{
	BYTE*		pString			= NULL;
	CObject*	tempObject		= NULL;
	BOOL		bEqual			= FALSE;
	BOOL		bReturnValue	= FALSE;
	BOOL		bClearFullField = ((pInput->GetLen()== 255) && (pInput->GetPos()== 0))?TRUE:FALSE;
	BOOL		bFullFieldUpdate= (pInput->GetPos()==255)?TRUE:FALSE;
	int			iNewStringPos	= (pInput->GetPos()==255)?0:(int) pInput->GetPos();
	int			iNewStringLen	= (int) pInput->GetLen();
	int			iSizeOfString	= iNewStringPos+iNewStringLen;
	CObArray*	arraySubFields;
	CharArray*	caCurContent=NULL;

	if (!m_mapFields.Lookup(enumFieldID, (CObject*&)tempObject))
	{
		if (bClearFullField)
			return FALSE;
		pString = new BYTE[iSizeOfString];
		memset(pString, ' ', iSizeOfString);
		//initialise all lines
		arraySubFields = new CObArray;
		arraySubFields->SetSize(iLineNo+1);
		for (int i=0; i<=iLineNo; i++)
		{
			CObject* CObTempObj = arraySubFields->GetAt(i);
			CObTempObj = NULL;
		}
		m_lNoOfFields ++;
	}
	else
	{
		arraySubFields = (CObArray*) tempObject;
		int iMaxSizeOfFields = arraySubFields->GetUpperBound();
		if (iMaxSizeOfFields>=iLineNo)
		{
			CObject* CObTempObj = arraySubFields->GetAt(iLineNo);
			if (CObTempObj != NULL)
			{
				caCurContent = (CharArray*) CObTempObj ;
				int	iCurStringLen = (int) caCurContent->GetLen();

				if (bClearFullField)
				{
					arraySubFields->SetAt(iLineNo, NULL); 
					delete caCurContent;
					arraySubFields->RemoveAt(iLineNo);
					if (arraySubFields->GetUpperBound()==-1)
					{
						m_mapFields.RemoveKey(enumFieldID);
						delete arraySubFields;
					}
					return TRUE;
				}

				// Full Field updated
				if (bFullFieldUpdate)
					if (caCurContent->GetLen()!=iNewStringLen)
						bCompare = FALSE;

				if (bCompare)
					if ((iSizeOfString==iCurStringLen) && (memcmp( pInput->GetString(), (caCurContent->GetString())+iNewStringPos, iNewStringLen) == 0))
						return FALSE;

				// Full Field updated
				if (bFullFieldUpdate)
				{
					arraySubFields->SetAt(iLineNo, NULL); 
					iCurStringLen = 0;
				}


				if (!bEqual)
				{	
					iSizeOfString = (iSizeOfString<iCurStringLen)?iCurStringLen:iSizeOfString;
					pString = new BYTE[iSizeOfString];
					memset(pString, ' ', iSizeOfString);
					memcpy(pString, caCurContent->GetString(), iCurStringLen);
					if (caCurContent!=NULL)
						delete caCurContent;
				}
			}
			else
			{
				pString = new BYTE[iSizeOfString];
				memset(pString, ' ', iSizeOfString);
			}
		}
		else
		{
			pString = new BYTE[iSizeOfString];
			memset(pString, ' ', iSizeOfString);
			arraySubFields->SetAtGrow( iLineNo, NULL); 
			//initialise the extened lines
			for (int i=iMaxSizeOfFields+1; i<iLineNo; i++)
			{
				CObject* CObTempObj = arraySubFields->GetAt(i);
				CObTempObj = NULL;
			}
		}
	}
	if (!bEqual)
	{
		CharArray*	pContent = new CharArray; 
		memcpy(pString+iNewStringPos, pInput->GetString(), iNewStringLen);
		pContent->SetLen(iSizeOfString);
		pContent->SetSq(pInput->GetSq());
		pContent->SetString(pString);
		pContent->SetPos(255);
		arraySubFields->SetAt(iLineNo, (CObject*)pContent);
 		m_mapFields[enumFieldID] = (CObject*) arraySubFields;
		delete[] pString;
		bReturnValue = TRUE;
	}
	return bReturnValue;
}


// Tran (Ripple) Type
// -------------------------------------------------------------------------
BOOL CPage::SetAField(BYTE* pInput,	FID_DEFINITION enumFieldID, int iBlockSize, int iTotalBlock, int iMaxNoOfBlock, BOOL bRipple, unsigned char ucRipple)
{
	BOOL bReturnValue = FALSE;
	int iTotalSize = UCHAR_SIZE + iMaxNoOfBlock*(iBlockSize);
	CObject* tempObject = NULL;
	BYTE* bytTempField = NULL;
	bool bbytTempField_is_tempObject = false;
	if (!m_mapFields.Lookup(enumFieldID, (CObject*&)tempObject))
	{
		bytTempField = new BYTE [iTotalSize];

		memset(bytTempField, 0x00, iTotalSize);
		m_lNoOfFields++;
		//m_mapFields[enumFieldID] = (CObject*) bytTempField;
		//tempObject = (CObject*) bytTempField;
	} else {
	
		unsigned long len = 0;
		if (! GunzipString((char*)tempObject,(char* &)bytTempField,len)  )
		{
			if(bytTempField)
				delete bytTempField;
			bytTempField = (BYTE*)tempObject;
			bbytTempField_is_tempObject = true;
		}
	}
	
	if (bRipple)
	{
		if (memcmp( pInput+UCHAR_SIZE, bytTempField+UCHAR_SIZE, iBlockSize) != 0)
		{
			memmove(bytTempField+UCHAR_SIZE+iBlockSize, bytTempField+UCHAR_SIZE, (iMaxNoOfBlock-1)*iBlockSize);
			memcpy(bytTempField+UCHAR_SIZE, pInput+UCHAR_SIZE, iBlockSize);
			//m_mapFields[enumFieldID] = (CObject*) bytTempField;

			unsigned char tempNe;
			memcpy(&tempNe, bytTempField, UCHAR_SIZE);
			//if (tempNe < 10)
			if (tempNe < ucRipple)
			{
				tempNe++;
				memcpy(bytTempField, &tempNe, 1);
			}
			char * pZipped =NULL; 
			if( GzipString((char*)bytTempField,iTotalSize,pZipped)) {
				m_mapFields[enumFieldID] = (CObject*) pZipped;
				delete bytTempField;
			} else
				m_mapFields[enumFieldID] = (CObject*) bytTempField;
			
			if(!bbytTempField_is_tempObject ) {
				BYTE * tempByte = (BYTE*) tempObject;
				delete  tempByte; // memeory leak
			}
			bReturnValue = TRUE;
		} else {
			if( !bbytTempField_is_tempObject ) {
				delete [] bytTempField;
			}
		}
	}
	else
	{
		int iCurNe=0;
		memcpy(&iCurNe, bytTempField, UCHAR_SIZE);

//		if( iTotalBlock > iMaxNoOfBlock ) {
//			CApplManClient::Trace(TRACE_LEVEL1,"DCache[Ripple Incorrect] FID:%d, %d->%d",enumFieldID,iMaxNoOfBlock,iTotalBlock);
//			iTotalBlock = iMaxNoOfBlock ;
//		}

		if ((iCurNe != iTotalBlock)||(memcmp( pInput, bytTempField, UCHAR_SIZE + iTotalBlock*(iBlockSize)) != 0))
		{
			bReturnValue = TRUE;
			memset(bytTempField, 0x00, iTotalSize);	
			memcpy(bytTempField, pInput, UCHAR_SIZE + iTotalBlock*(iBlockSize));
			char * pZipped =NULL; 
			if( GzipString((char*)bytTempField,iTotalSize,pZipped)) {			
				m_mapFields[enumFieldID] = (CObject*) pZipped;
				delete [] bytTempField;
			} else
				m_mapFields[enumFieldID] = (CObject*) bytTempField;
			
			if(!bbytTempField_is_tempObject ) {
				BYTE * tempByte = (BYTE*) tempObject;
				delete  tempByte; // memeory leak
			}
		} else {
			if( !bbytTempField_is_tempObject ) {
				delete [] bytTempField;
			}
		}
	}
	return bReturnValue;
}

// for General Type
// -------------------------------------------------------------------------
BOOL CPage::SetAField(BYTE* bytInput,  FID_DEFINITION enumFieldID, long lSizeOfBlock)
{
	BOOL		bReturnValue	= FALSE;
	BOOL		bEqual			= FALSE;
	CObject*	tempObject = NULL;
	bool bbytTempContent_is_tempObject = false;
	if (!m_mapFields.Lookup(enumFieldID, (CObject*&)tempObject))
	{
		m_lNoOfFields ++;
	}
	else
	{
		BYTE* byteTempContent = NULL;
		unsigned long len = 0;
		if( !GunzipString((char*)tempObject,(char* &)byteTempContent,len)) {
			if(byteTempContent)
				delete byteTempContent;
			byteTempContent = (BYTE*)tempObject;
			bbytTempContent_is_tempObject = true;
		}

		if (memcmp( bytInput, byteTempContent, lSizeOfBlock) != 0)
		{
			delete[] byteTempContent;
			if(!bbytTempContent_is_tempObject) {
				byteTempContent = (BYTE*)tempObject;
				delete byteTempContent;
			}
			byteTempContent = NULL;
			m_mapFields[enumFieldID] = NULL;
		}	
		else
			bEqual = TRUE;
	}

	if (!bEqual)
	{
		BYTE* bytContent = new BYTE[lSizeOfBlock];
		memset(bytContent, 0x00, lSizeOfBlock);
		memcpy(bytContent, bytInput, lSizeOfBlock);

		char * pZipped =NULL; 
		if( GzipString((char*)bytContent,lSizeOfBlock,pZipped)) {
			m_mapFields[enumFieldID] = (CObject*) pZipped;
			delete [] bytContent;
		} else
			m_mapFields[enumFieldID] = (CObject*) bytContent;
		bReturnValue = TRUE;
	}
	return bReturnValue;
}


// PrcVol, HLink Type
// -------------------------------------------------------------------------
BOOL CPage::SetAField(BYTE* pInput,	FID_DEFINITION enumFieldID, long lPos, int iBlockSize, int iTotalBlock)
{
	BOOL	bReturnValue	= FALSE;
	BOOL	bEqual			= FALSE;
	BYTE*	bytTempField;
	CObject* tempObject		= NULL;
	unsigned char tempTl;
	unsigned char tempNe	= iTotalBlock;
	unsigned char tempPos	= 1;
	unsigned char CurNe		= 0;
	unsigned char CurPos	= 1;
	int iHeaderSize = UCHAR_SIZE*3; 
	memcpy(&tempTl, pInput, UCHAR_SIZE);
	int iTotalSize = iHeaderSize + (tempTl)*(iBlockSize);

	if (!m_mapFields.Lookup(enumFieldID, (CObject*&)tempObject))
	{
		bytTempField = new BYTE [iTotalSize];
		memset(bytTempField, 0x00, iTotalSize);
		m_lNoOfFields++;
		m_mapFields[enumFieldID] = (CObject*) bytTempField;
		CurPos = (unsigned char)lPos;
	}
	else
	{
		BYTE* bytField = (BYTE*) tempObject;
		unsigned char sCurrentTl;
		memcpy(&sCurrentTl, bytField, UCHAR_SIZE);
		if ((tempTl == sCurrentTl) && (memcmp( pInput+iHeaderSize, bytField+iHeaderSize+(iBlockSize*lPos), iTotalBlock*iBlockSize) == 0))
			bEqual = TRUE;
		else
		{
			memcpy(&CurNe, bytField+UCHAR_SIZE, UCHAR_SIZE);
			memcpy(&CurPos, bytField+UCHAR_SIZE*2, UCHAR_SIZE);
			if (tempTl > sCurrentTl)
			{
				bytTempField = new BYTE [iTotalSize];
				memset(bytTempField, 0x00, iTotalSize);
				memcpy(bytTempField+iHeaderSize, bytField+iHeaderSize, sCurrentTl*iBlockSize);
				delete[] bytField;
			}
			else if (tempTl < sCurrentTl)
			{
				bytTempField = new BYTE [iTotalSize];
				memset(bytTempField, 0x00, iTotalSize);
				memcpy(bytTempField+iHeaderSize, bytField+iHeaderSize, tempTl*iBlockSize);
				CurNe = ((CurNe+CurPos)>tempTl)?tempTl-CurPos:CurNe;
				delete[] bytField;
			}
			else
			{
				bytTempField = bytField;
				tempTl = sCurrentTl;
			}
		}
	}

	if (!bEqual)
	{
		memcpy(bytTempField+iHeaderSize+lPos*iBlockSize, pInput+iHeaderSize, iTotalBlock*iBlockSize);
		tempNe = ((tempNe+lPos)>(CurNe+CurPos))? tempNe+lPos:CurNe+CurPos;

		memcpy(bytTempField, &tempTl, UCHAR_SIZE);
		memcpy(bytTempField+UCHAR_SIZE, &tempNe, UCHAR_SIZE);
		m_mapFields[enumFieldID] = (CObject*) bytTempField;
		bReturnValue = TRUE;
	}
	return bReturnValue;
}

// PrcVolwSeq Type
// -------------------------------------------------------------------------
BOOL CPage::SetAField(BYTE* pInput,	FID_DEFINITION enumFieldID, long lPos, int iBlockSize, int iTotalBlock, int iHeaderSize, int iTotSize)
{
	BOOL	bReturnValue	= FALSE;
	BOOL	bEqual			= FALSE;
	BYTE*	bytTempField;
	CObject* tempObject		= NULL;
	unsigned char tempTl;
	unsigned char tempNe	= iTotalBlock;
	unsigned char tempPos	= 1;
	unsigned char CurNe		= 0;
	unsigned char CurPos	= 1;
	unsigned long ulCurSeq	= 0;
	memcpy(&tempTl, pInput, UCHAR_SIZE);
	memcpy(&ulCurSeq, pInput+UCHAR_SIZE*3, 4);
	int iTotalSize = iHeaderSize + (tempTl)*(iBlockSize);

	if (!m_mapFields.Lookup(enumFieldID, (CObject*&)tempObject))
	{
		bytTempField = new BYTE [iTotalSize];
		memset(bytTempField, 0x00, iTotalSize);
		m_lNoOfFields++;
		m_mapFields[enumFieldID] = (CObject*) bytTempField;
		CurPos = (unsigned char)lPos;
	}
	else
	{
		BYTE* bytField = (BYTE*) tempObject;
		unsigned char sCurrentTl;
		memcpy(&sCurrentTl, bytField, UCHAR_SIZE);
		if ((tempTl == sCurrentTl) && (memcmp( pInput+iHeaderSize, bytField+iHeaderSize+(iBlockSize*lPos), iTotalBlock*iBlockSize) == 0) && 
			(memcmp(pInput+3, bytField+3, 4)==0))
			bEqual = TRUE;
		else
		{
			memcpy(&CurNe, bytField+UCHAR_SIZE, UCHAR_SIZE);
			memcpy(&CurPos, bytField+UCHAR_SIZE*2, UCHAR_SIZE);
			if (tempTl > sCurrentTl)
			{
				bytTempField = new BYTE [iTotalSize];
				memset(bytTempField, 0x00, iTotalSize);
				memcpy(bytTempField+iHeaderSize, bytField+iHeaderSize, sCurrentTl*iBlockSize);
				delete[] bytField;
			}
			else if (tempTl < sCurrentTl)
			{
				bytTempField = new BYTE [iTotalSize];
				memset(bytTempField, 0x00, iTotalSize);
				memcpy(bytTempField+iHeaderSize, bytField+iHeaderSize, tempTl*iBlockSize);
				CurNe = ((CurNe+CurPos)>tempTl)?tempTl-CurPos:CurNe;
				delete[] bytField;
			}
			else
			{
				bytTempField = bytField;
				tempTl = sCurrentTl;
			}
		}
	}

	if (!bEqual)
	{
		memcpy(bytTempField+iHeaderSize+lPos*iBlockSize, pInput+iHeaderSize, iTotalBlock*iBlockSize);
		tempNe = ((tempNe+lPos)>(CurNe+CurPos))? tempNe+lPos:CurNe+CurPos;

		memcpy(bytTempField, &tempTl, UCHAR_SIZE);
		memcpy(bytTempField+UCHAR_SIZE, &tempNe, UCHAR_SIZE);
		memcpy(bytTempField+UCHAR_SIZE*3, &ulCurSeq, 4);
		m_mapFields[enumFieldID] = (CObject*) bytTempField;
		bReturnValue = TRUE;
	}
	return bReturnValue;
}


// Link, WLink, MtcPoints, MtcVal, MTCV, SprTbl, MTCTicks, MTCPTS, MTCV, MTCF
// -------------------------------------------------------------------------
BOOL CPage::SetAField(BYTE* pInput,	FID_DEFINITION enumFieldID, long lPos, long lSizeOfBlock)
{
	BOOL	bReturnValue	= FALSE;
	BOOL	bEqual			= FALSE;
	BYTE*	bytTempField = NULL;
	unsigned short	tempTl;
	unsigned short	tempNe;
	unsigned short	tempPos = 0;
	unsigned short	CurPos = 0;
	unsigned short  CurNe = 0;
	int iHeaderSize	= USHORT_SIZE*3 ;
	CObject * tempObject = NULL;

	memcpy(&tempTl, pInput, USHORT_SIZE);
	memcpy(&tempNe, pInput+USHORT_SIZE, USHORT_SIZE);
	int iTotalSize = iHeaderSize + (tempTl)*(lSizeOfBlock);

	if (!m_mapFields.Lookup(enumFieldID, (CObject*&)tempObject))
	{
		bytTempField = new BYTE [iTotalSize];
		fidDatabase fDB;
		if (fDB.GetFidType(enumFieldID) == NGN_MTCV)
		{
			unsigned short usInvalidPt = MTCV::MTC_INVALID_POINT;
			char strTempData[16];
			memcpy(strTempData,   &usInvalidPt, 2);
			memcpy(strTempData+2, &usInvalidPt, 2);
			memcpy(strTempData+4, &usInvalidPt, 2);
			memcpy(strTempData+6, &usInvalidPt, 2);
			memset(strTempData+8, 0x00, 8);
			memset(bytTempField, 0x00, 6);
			for (int iDataLen=6; iDataLen+16<=iTotalSize; iDataLen+=16)
				memcpy(bytTempField+iDataLen, strTempData, 16);
		}
		else if (fDB.GetFidType(enumFieldID) == NGN_MTCTICKS)
		{
			unsigned short usInvalidPt = MTCV::MTC_INVALID_POINT;
			char strTempData[8];
			memcpy(strTempData,   &usInvalidPt, 2);
			memcpy(strTempData+2, &usInvalidPt, 2);
			memcpy(strTempData+4, &usInvalidPt, 2);
			memcpy(strTempData+6, &usInvalidPt, 2);
			memset(bytTempField, 0x00, 6);
			for (int iDataLen=6; iDataLen+8<=iTotalSize; iDataLen+=8)
				memcpy(bytTempField+iDataLen, strTempData, 8);
		}
		else if (fDB.GetFidType(enumFieldID) == NGN_MTCF)
		{
			float fInvalidPt = MTCF::MTCF_INVALID_POINT;
			char strTempData[24];
			memcpy(strTempData,   &fInvalidPt, 4);
			memcpy(strTempData+4, &fInvalidPt, 4);
			memcpy(strTempData+8, &fInvalidPt, 4);
			memcpy(strTempData+12, &fInvalidPt, 4);
			memset(strTempData+16, 0x00, 8);
			memset(bytTempField, 0x00, 6);
			for (int iDataLen=6; iDataLen+24<=iTotalSize; iDataLen+=24)
				memcpy(bytTempField+iDataLen, strTempData, 24);
		}
		else
			memset(bytTempField, 0x00, iTotalSize);
		m_lNoOfFields++;


/*		
		m_mapFields[enumFieldID] = (CObject*) bytTempField;*/

		CurPos = (unsigned short) lPos;
	}
	else
	{
		unsigned long len = 0;
		BYTE* bytField = NULL;
		bool bytField_is_tempObject = false;
		bool bytTempField_is_bytField = false;
		if( !GunzipString((char*)tempObject,(char* &)bytField,len) ) {
			if(bytField)
				delete bytField;
			bytField = (BYTE*)tempObject;
			bytField_is_tempObject = true;
		}

		
		unsigned short sCurrentTl;
		memcpy(&sCurrentTl, bytField, USHORT_SIZE);
		if ((tempTl == sCurrentTl) && (memcmp( pInput+iHeaderSize, bytField+iHeaderSize+(lSizeOfBlock*lPos), tempNe*lSizeOfBlock) == 0)) {
			bEqual = TRUE;
		} else
		{
			memcpy(&CurNe, bytField+USHORT_SIZE, USHORT_SIZE);
			memcpy(&CurPos, bytField+USHORT_SIZE*2, USHORT_SIZE);

			if (tempTl > sCurrentTl)
			{
				bytTempField = new BYTE [iTotalSize];
				memset(bytTempField, 0x00, (int)iTotalSize);
				memcpy(bytTempField+iHeaderSize, bytField+iHeaderSize, (int)sCurrentTl*lSizeOfBlock);
				delete [] bytField;

			}
			else if(tempTl < sCurrentTl)
			{
				bytTempField = new BYTE [iTotalSize];
				memset(bytTempField, 0x00, (int)iTotalSize);
				memcpy(bytTempField+iHeaderSize, bytField+iHeaderSize, (int)tempTl*lSizeOfBlock);
				CurNe = ((CurNe+CurPos)>tempTl)?tempTl-CurPos:CurNe;
				delete [] bytField;
			}
			else
			{
				bytTempField = bytField;
				bytTempField_is_bytField = true;
				tempTl = sCurrentTl;
			}
		}


		if(!bytTempField_is_bytField) {
			if(!bytField_is_tempObject) {
				if ( bytField != nullptr)
					delete bytField;
			}			
		}			

		if(!bytField_is_tempObject){
			BYTE * bytDel = (BYTE*) tempObject;
			if(!bEqual)
				delete bytDel;
		}

	}
	
	if (!bEqual)
	{
		memcpy(bytTempField+iHeaderSize+(lSizeOfBlock*(lPos)), pInput+iHeaderSize, (int)tempNe*lSizeOfBlock);
		tempNe = ((tempNe+lPos)>(CurNe+CurPos))? tempNe+lPos:CurNe+CurPos;
		memcpy(bytTempField, &tempTl, USHORT_SIZE);
		memcpy(bytTempField+USHORT_SIZE, &tempNe, USHORT_SIZE);
		char * pZipped =NULL; 
		if( GzipString((char*)bytTempField,iTotalSize,pZipped)) {
			m_mapFields[enumFieldID] = (CObject*) pZipped;
			delete (char*) bytTempField;
		} else 
			m_mapFields[enumFieldID] = (CObject*) bytTempField;

		bReturnValue = TRUE;
	}
	return bReturnValue;
}

	

// Headline
// -------------------------------------------------------------------------
BOOL CPage::SetAField(BYTE* pInput, FID_DEFINITION enumFieldID, long lIndex, int iSizeOfString)
{
	BOOL	bReturnValue = FALSE;
	BOOL	bEqual		 = FALSE;
	unsigned long ulLineNo;
	CObject* tempObject = NULL;

 	memcpy(&ulLineNo, pInput, 4);
	if (!m_mapFields.Lookup(enumFieldID, (CObject*&)tempObject))
	{
		//initialise all lines
		CFieldsMap* cfmSubFields = new CFieldsMap;
		cfmSubFields->InitHashTable( DEFAULT_NO_OF_INIT_FIELDS );

		m_lNoOfFields ++;
		m_mapFields[enumFieldID] = (CObject*) cfmSubFields;
		tempObject = (CObject*) cfmSubFields;
	}
	else
	{
		CObject* CObTempObj;
		CFieldsMap*	cfmTempSubFields = (CFieldsMap*) tempObject;
		if (cfmTempSubFields->Lookup(ulLineNo, CObTempObj))
		{
			BYTE* bytTempContent = (BYTE*) CObTempObj;
			if (memcmp( pInput, bytTempContent, iSizeOfString) == 0)
				bEqual = TRUE;
			else
				delete[] bytTempContent;
		}
	}
	if (!bEqual)
	{
		CFieldsMap*	cfmTempSubFields = (CFieldsMap*) tempObject;
		BYTE*	pString;
		fidDatabase fDB;
		if (fDB.GetFidType(enumFieldID) == NGN_YHEADLINE)
		{
			YHeadLine yhlTemp(pInput);
			pString = new BYTE[yhlTemp.GetDataLen()];
		}
		else
		{
			HeadLine hlTemp(pInput);
			pString = new BYTE[hlTemp.GetDataLen()];
		}
		memcpy(pString, pInput, iSizeOfString);
		cfmTempSubFields->SetAt(ulLineNo, (CObject*&)pString);
		m_mapFields[enumFieldID] = (CObject*) cfmTempSubFields;
		bReturnValue = TRUE;
	}
	return bReturnValue;
}




// Quote List 
// -------------------------------------------------------------------------
BOOL CPage::SetAField(BYTE* pInput,	FID_DEFINITION enumFieldID, unsigned short usDataNe, byte byteDataTL)
{

#ifdef _DEBUG
	int col = 11;
	switch (enumFieldID)
	{
	case 1460: col = 13; break;
	case 1463: col = 20; break;
	case 1464: col = 19; break;
	case 1465: col = 11; break;
	case 1466:  col = 35; break;
	case 1467:  col = 19; break;
		break;
	}
#endif

	//if (enumFieldID  > 1465)
	//	return TRUE;
	CObject*		tempObject		= NULL;
	BOOL			bReturnValue	= FALSE;
	byte			byteLn, byteLen;
	short			sFid;
	byte*			pbyQuote;
	void*			pFv;
	//CObArray*		arrayLines = NULL;
	//CObArray*		arrayFids = NULL;

	QLLines * arrayLines = NULL;
	QLFids * arrayFids = NULL;

	QuoteList		quotelist(pInput);
	unsigned short	usQuoteLen;

	try {
		//  Create a new Fid for Quote List
		if (!m_mapFields.Lookup(enumFieldID, (CObject*&)tempObject))
		{
			//initialise all lines
			arrayLines = new QLLines((int)byteDataTL);

			for (int i = 0; i < byteDataTL; i++)
				arrayLines->at(i) = (QLFids*)NULL;
	
			m_lNoOfFields++;
			m_mapFields[enumFieldID] = (CObject*)arrayLines;
		}
		else
		{
			arrayLines = (QLLines*)tempObject;
			int iMaxLines = arrayLines->size();
			// if TL increase, add lines
			if (iMaxLines < (int)byteDataTL)
			{
				for (int i = iMaxLines; i < byteDataTL; i++)
					arrayLines->push_back(NULL);
			}
		}

		for (int iNoOfQuote = 0; iNoOfQuote < usDataNe; iNoOfQuote++)
		{
			if (quotelist.GetQuote(iNoOfQuote, byteLn, sFid, byteLen, pFv) &&
				quotelist.GetQuote(iNoOfQuote, pbyQuote, usQuoteLen))
			{		
				int nbyteLn = (int)byteLn;
				int nFid = (int)sFid;

				if (byteDataTL > byteLn)
				{
					int iMaxLines = arrayLines->size();

					// if TL decrease, remove lines
					if (iMaxLines > byteDataTL)
					{
						int iMaxBefore = iMaxLines;
						
						for (int i = iMaxLines - 1; i >= byteDataTL; i--)
						{
							arrayFids = arrayLines->at(i);
							if (arrayFids != NULL)
							{
								int iMaxFids = arrayFids->size();

								QLFids::iterator qlf_iter = arrayFids->begin();
								while (qlf_iter != arrayFids->end())
								{
									strQE* CObTempObj = *qlf_iter;
									if (CObTempObj != NULL)
									{
										delete [] CObTempObj->pbyQuote;
										delete CObTempObj;
									}
									qlf_iter++;
								}
								arrayFids->clear();
								delete arrayFids;
								arrayFids = NULL;
							}
							arrayLines->pop_back();
						}
						
						bReturnValue = TRUE;
						iMaxLines = arrayLines->size();
						//printf("%u : %d Remove Line (iMaxLines:%d : byteDataTL:%d) after %d \n", m_lPageNo, enumFieldID, iMaxBefore, byteDataTL, iMaxLines);
					}

					if (iMaxLines >= byteDataTL)
					{
						QLFids* CObTempObj = arrayLines->at(nbyteLn);
						if (CObTempObj == NULL)
						{
							arrayFids = new QLFids;
							arrayLines->at(nbyteLn)= arrayFids;
						}
						else {
							arrayFids = CObTempObj;
						}
						// Drop Line 
						if ((unsigned short)sFid == USHRT_MAX)
						{
							QLFids::iterator qlf_iter = arrayFids->begin();
							while (qlf_iter != arrayFids->end())
							{
								strQE* CObTempObj = *qlf_iter;
								if (CObTempObj != NULL)
								{
									delete [] CObTempObj->pbyQuote;
									delete CObTempObj;
								}
								qlf_iter++;
							}

							arrayFids->clear();
							delete arrayFids;
							arrayLines->at(byteLn) =  NULL;
							arrayFids = NULL;
							bReturnValue = TRUE;
							continue;
						}

						if (arrayFids->size() > nFid)
						{
							//strQE* tempQuoteList = NULL;
							strQE* CObTempObj = arrayFids->at(nFid);
							if (CObTempObj != NULL) {
								// Drop Cell 
								if (byteLen == 0)
								{
									if (CObTempObj != NULL) {
										delete [] CObTempObj->pbyQuote;
										delete CObTempObj;
									}
									arrayFids->at(nFid)= NULL;
									bReturnValue = TRUE;
									continue;
								}

								// Compare Content
								if (CObTempObj->usQuoteLen == usQuoteLen)
									if (memcmp(CObTempObj->pbyQuote, pbyQuote, usQuoteLen) == 0)
										continue; // match and continue

								delete [] CObTempObj->pbyQuote;
								delete CObTempObj;
								CObTempObj = NULL;
								arrayFids->at(nFid) = NULL;
							}
						}
						// create new cell & insert new cell
						strQE * qe = new strQE;
						qe->pbyQuote = new byte[usQuoteLen];
						qe->usQuoteLen = usQuoteLen;
						qe->byteDataTL = byteDataTL;
						memcpy(qe->pbyQuote, pbyQuote, usQuoteLen);

						//extends vector
						if (arrayFids->size() <= nFid) {
							int diff = nFid - arrayFids->size() + 1;
							for (int s = 0; s < diff; s++)
								arrayFids->push_back(NULL);
						}

						arrayFids->at(nFid) =qe;		
						
					}
					else {
						printf("%u : %d unexpected data (iMaxLines:%d : byteDataTL:%d \n", m_lPageNo, enumFieldID, iMaxLines, byteDataTL);
					}
					
					bReturnValue = TRUE;
				}
			}
			else
			{
				TRACE("QuoteList::GetQuote() Error\n");
			}
		}

	}
	catch (...){

		QuoteList quote(pInput);

		for (int i = 0; i < quote.GetNE(); i++)
		{
			if (quotelist.GetQuote(i, byteLn, sFid, byteLen, pFv))
			{
				printf("Quote[%d,%d,size=%d]", byteLn, sFid, byteLen);
			}
		}
//		quote.GetData();
	
	}
	return bReturnValue;
}



	
// Drop lines for MCharArray
// -------------------------------------------------------------------------
BOOL CPage::DropMCharArrayLn(FID_DEFINITION enumFieldID, long lCurTl, long lNewTl)
{
	CObject*	tempObject		= NULL;
	CObArray*	arraySubFields;
	CharArray*	caCurContent=NULL;

	if (!m_mapFields.Lookup(enumFieldID, (CObject*&)tempObject))
	{
		return (FALSE);
	}
	else
	{
		if (tempObject == nullptr)
			return FALSE;

		arraySubFields = (CObArray*) tempObject;

		for (int iLineNo=lCurTl-1; iLineNo>=lNewTl; iLineNo--)
		{
			CObject* CObTempObj = arraySubFields->GetAt(iLineNo);
			if (CObTempObj != NULL)
			{
				caCurContent = (CharArray*) CObTempObj ;
				arraySubFields->SetAt(iLineNo, NULL); 
				delete caCurContent;
				arraySubFields->RemoveAt(iLineNo);
				if (arraySubFields->GetUpperBound()==-1)
				{
					m_mapFields.RemoveKey(enumFieldID);
					delete arraySubFields;
				}
			}
		}
	}
	return (TRUE);

}
// -------------------------------------------------------------------------

void CPage::RemoveFidFromDropList(FID_DEFINITION enumFieldID)
{
	m_mapDropFields.RemoveKey(enumFieldID);
}
													
// Drop a field
// -------------------------------------------------------------------------
BOOL CPage::DropAField(FID_DEFINITION enumFieldID)
{
	CObject*	objTemp;
	fidDatabase fDB;

	BOOL bReturnValue = FALSE;


	if (bReturnValue = m_mapFields.Lookup(enumFieldID, (CObject*&)objTemp))
	{
		if (objTemp != NULL)
		{
			if (m_mapFields.RemoveKey(enumFieldID))
			{
				CObject* tempObject = NULL;
				if (!m_mapDropFields.Lookup(enumFieldID, (CObject*&)tempObject))
					m_mapDropFields[enumFieldID] = tempObject;

				switch (fDB.GetFidType(enumFieldID))
				{
					case NGN_CHAR:
						{
							unsigned char* cContent = (unsigned char*) objTemp ;
							delete cContent;
							bReturnValue = TRUE;
						}
						break;
					case NGN_SHORT:
						{
							short* nContent = (short*) objTemp;
							delete nContent;
							bReturnValue = TRUE;
						}
						break;
					case NGN_UNICODE:
					case NGN_USHORT:
						{
							unsigned short* nContent = (unsigned short*) objTemp;
							delete nContent;
							bReturnValue = TRUE;
						}
						break;
					case NGN_LONG:
					case NGN_KMULTIPLE:
					case NGN_10KMULTIPLE:
						{
							long* lContent = (long*) objTemp;
							delete lContent;
							bReturnValue = TRUE;
						}
						break;
					case NGN_ULONG:
						{
							unsigned long* ulContent = (unsigned long*) objTemp;
							delete ulContent;
							bReturnValue = TRUE;
						}
						break;
					case NGN_FLOAT:
					case NGN_UFLOAT:
						{
							float* fContent = (float*) objTemp;
							delete fContent;
							bReturnValue = TRUE;
						}
						break;
					case NGN_DOUBLE:
					case NGN_UDOUBLE:
						{
							double* dContent = (double*) objTemp;
							delete dContent;
							bReturnValue = TRUE;
						}
						break;
					case NGN_CHARARRAY:
						{
							CharArray*	pContent = (CharArray*) objTemp;
							delete pContent;
							bReturnValue = TRUE;
						}
						break;
					case NGN_MCHARARRAY:
						{
							CObArray*	arrayTempSubFields = (CObArray*) objTemp;
							int iMaxSizeOfFields = arrayTempSubFields->GetUpperBound();
							for (int i = 0; i<=iMaxSizeOfFields; i++)
							{
								CObject* CObTempObj = arrayTempSubFields->GetAt(i);
								if (CObTempObj != NULL)
								{
									CharArray* cTempContent = (CharArray*) CObTempObj ;
									delete cTempContent;
								}
							}
							delete arrayTempSubFields;
							bReturnValue = TRUE;
						}
						break;
					case NGN_BDATEYEAR:
					case NGN_BDATEMONTH:
					case NGN_BTIMEMIN:
					case NGN_BTIMESEC:
					case NGN_BTIMERANGES:
					case NGN_TRAN:
					case NGN_SHRORDLST:
					case NGN_PRCVOL:
					case NGN_BKRQUE:
					case NGN_LINK:
					case NGN_WLINK:
					case NGN_YLABEL:
					case NGN_XMTCLABEL:
					case NGN_XDTCLABEL:
					case NGN_MTCPOINTS:
					case NGN_MTCVAL:
					case NGN_DTCPOINTS:
					case NGN_HISVAL:
					case NGN_FIELDLIST:
					case NGN_FIELDLABEL:
					case NGN_INFOMSG:
					case NGN_HLINK:
					case NGN_SPRTBL:
					case NGN_MTCTICKS:
					case NGN_MTCV:
					case NGN_OBKRQUE:
					case NGN_BQUEVOSN:
					case NGN_MTCPTS:
					case NGN_MTCF:
					case NGN_PRCVOLWSEQ:
					case NGN_TRAN_D:
						{
							BYTE* pContent = (BYTE*) objTemp;
							delete [] pContent;
							bReturnValue = TRUE;
						}
						break;
					case NGN_HEADLINE:
					case NGN_YHEADLINE:
						{
							CFieldsMap*	cfmSubFields = (CFieldsMap*) objTemp;
							long		nKey;
							POSITION pos = cfmSubFields->GetStartPosition();
							while (pos != NULL)
							{
								CObject* tempObject = NULL;
								cfmSubFields->GetNextAssoc( pos, nKey, tempObject );
								cfmSubFields->RemoveKey(nKey);
								if (tempObject != NULL)
								{
									BYTE* cTempContent = (BYTE*) tempObject ;
									delete [] cTempContent;
								}
							}		
							cfmSubFields->RemoveAll();
							delete cfmSubFields;
							bReturnValue = TRUE;
						}
						break;

					case NGN_QUOTELIST:
						{
							
							QLLines* arrayLines = (QLLines*) objTemp;
							if (arrayLines != NULL)
							{
								int iMaxLines = arrayLines->size();

								for (int iRemovedLines = 0; iRemovedLines < iMaxLines; iRemovedLines++)
								{
									QLFids* arrayFids = (QLFids*)arrayLines->at(iRemovedLines);
									if (arrayFids!=NULL)
									{
										int iMaxFids = arrayFids->size();
										for (int iRemovedFid = 0; iRemovedFid<iMaxFids; iRemovedFid++)
										{
											strQE* CObTempObj = arrayFids->at(iRemovedFid);
											if (CObTempObj != NULL)
											{
												strQE* cTempContent = (strQE*)CObTempObj;
												delete [] cTempContent->pbyQuote;
												delete cTempContent;
											}
										}
										arrayFids->clear();
										delete arrayFids;
									}
								}
								arrayLines->clear(); 
								delete arrayLines;
							}
						}
						break;
					default:
						break;
				}
			}
		}
	}		
	return bReturnValue;
}