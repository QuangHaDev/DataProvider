/////////////////////////////////////////////////////////////////////////////
//          File:: DCache.h
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

#ifndef __DCACHE_H__
#define __DCACHE_H__

#include "WriteDataThread.h"
#include "ssCountedObjPtr.h"
#include "MsgObject.h"
#include "Page.h"
#define CS_HASH_NUM 787

using namespace NGN2;

typedef CMap<long,long,CPage*,CPage*> CPagesMap;
typedef	CMap<long, long, long, long> CTLogRippleMap;

#ifdef _MAC
#undef AFX_EXT_API
#undef AFX_EXT_CLASS
#define AFX_EXT_API
#define AFX_EXT_CLASS
#endif

#define MAX_NO_OF_PAGES				10000
#define DEFAULT_NO_OF_INIT_PAGES	10000
#define WRITE_TO_DISK_TIMER			1800	
#define CLEAR_TO_DISK_TIMER			3000
#define CLEAR_DISK_TIMER			3
#define WRITE_TIMER					2		
#define WRITE_FID_TIMER				1
#define MAX_LEN						2048
#define DCACHEINI					"DCache.ini"
#define TLOGRIPFILENAME				"TLogRipple.txt"
#define FAULTFILENAME				"Fault.log"
#define MAX_HASH_TABLE_SIZE			2147483648
#define USHORT_SIZE					2
#define UCHAR_SIZE					1
#define TEMP_BUFFER					32

class DCacheMsgObject;
class CPageStatus;
class CCallback;
class CPageList;
class CObject;
class CWriteDataThread;
class CPage;
class CSink;
class CSource;

/////////////////////////////////////////////////////////////////////////////
//
// Class      : 
// Description:
//

class AFX_EXT_CLASS CDCache : public CObject
{
	DECLARE_DYNCREATE(CDCache)
public:
	enum {DROP_PAGE = -999};


public:
	// -------------------------------------------------------------------------
	// Default Constructor
//	CDCache(BOOL bInstance = FALSE, char* cInstance = NULL, CWordArray* cwaInstance = NULL, int iInstance = 7200, BOOL bInstance2 = FALSE, CTLogRippleMap* mapTLogRipple = NULL);
	CDCache(BOOL bInstance = FALSE, char* cInstance = NULL, CWordArray* cwaInstance = NULL, int iInstance = 7200, BOOL bInstance2 = FALSE);

	// -------------------------------------------------------------------------
	// Destructor
	virtual ~CDCache();

// Implementation
private:
	BOOL			FirstPage(POSITION&	posFirstPage);
	long			GetNextPageNo(POSITION&	posNextPage);
	void			UnlockCriticalSection();

public:
	// Retrieve functions
	static char*	GetVersionString(void);
	static char*	GetModuleString(void);
	char*			GetStatus(void);

	// DCache Setup function
	//void			SetIgnorePages(CString *pstrPageList);
	//void			SetIgnoreRecordType(CString *pstrTypeList);
	//void			SetIgnoreMarket(CString *pstrMarketList);

	// Updating Database
	BOOL PutDataIntoDCache(COP_ITEM copInput);
	BOOL PutDataIntoDCache(COP_ITEM copInput, CWordArray& cwaUpdated);
	BOOL SetAField(long lPageNo, long lInput,			FID_DEFINITION enumFieldID, long lIndex);
	BOOL SetAField(long lPageNo, unsigned long lInput,	FID_DEFINITION enumFieldID, long lIndex);
	BOOL SetAField(long lPageNo, short nInput,			FID_DEFINITION enumFieldID, long lIndex);
	BOOL SetAField(long lPageNo, unsigned short nInput, FID_DEFINITION enumFieldID, long lIndex);
	BOOL SetAField(long lPageNo, float fInput,			FID_DEFINITION enumFieldID, long lIndex);
	BOOL SetAField(long lPageNo, double lfInput,		FID_DEFINITION enumFieldID, long lIndex);
	BOOL SetAField(long lPageNo, unsigned char cInput,	FID_DEFINITION enumFieldID, long lIndex);
	BOOL SetAField(long lPageNo, long lInput,			FID_DEFINITION enumFieldID, long lIndex, COP_ITEM copMsg);
	BOOL SetAField(long lPageNo, unsigned long lInput,	FID_DEFINITION enumFieldID, long lIndex, COP_ITEM copMsg);
	BOOL SetAField(long lPageNo, short nInput,			FID_DEFINITION enumFieldID, long lIndex, COP_ITEM copMsg);
	BOOL SetAField(long lPageNo, unsigned short nInput, FID_DEFINITION enumFieldID, long lIndex, COP_ITEM copMsg);
	BOOL SetAField(long lPageNo, float fInput,			FID_DEFINITION enumFieldID, long lIndex, COP_ITEM copMsg);
	BOOL SetAField(long lPageNo, double lfInput,		FID_DEFINITION enumFieldID, long lIndex, COP_ITEM copMsg);
	BOOL SetAField(long lPageNo, unsigned char cInput,	FID_DEFINITION enumFieldID, long lIndex, COP_ITEM copMsg);
	BOOL SetAField(long lPageNo, CharArray* pInput,		FID_DEFINITION enumFieldID, long lIndex, BOOL bCompare, COP_ITEM copMsg);										// CharArray
	BOOL SetAField(long lPageNo, CharArray* pInput,		FID_DEFINITION enumFieldID, long lIndex, int iLineNo, BOOL bCompare, COP_ITEM copMsg);							// MCharArray
	BOOL SetAField(long lPageNo, BYTE* pInput,			FID_DEFINITION enumFieldID, long lSizeOfBlock, COP_ITEM copMsg);												// General Type
	BOOL SetAField(long lPageNo, BYTE* pInput,			FID_DEFINITION enumFieldID, int  iBlockSize, int iTotalBlock, int iMaxNoOfBlock, BOOL bRipple, COP_ITEM copMsg);// Tran
	BOOL SetAField(long lPageNo, BYTE* pInput,			FID_DEFINITION enumFieldID, long lPos, long iMaxNoOfBlock, long iSizeOfBlock, COP_ITEM copMsg);					// ShrOrdLst
	BOOL SetAField(long lPageNo, BYTE* pInput,			FID_DEFINITION enumFieldID, long lPos, int iBlockSize, int iTotalBlock, COP_ITEM copMsg);						// PrcAskNuBid Type
	BOOL SetAField(long lPageNo, BYTE* pInput,			FID_DEFINITION enumFieldID, long lPos, long iSizeOfBlock, COP_ITEM copMsg);										// Link, WLink, MtcPoints, MtcVal, MTCPTS, MTCF
	BOOL SetAField(long lPageNo, BYTE* pInput,			FID_DEFINITION enumFieldID, long lIndex, int iSizeOfString, COP_ITEM copMsg);									// Headline
	BOOL SetAField(long lPageNo, BYTE* pInput,			FID_DEFINITION enumFieldID, long lIndex, unsigned short usDataNe, byte byteDataTL, COP_ITEM copMsg);			// Quote List
	BOOL SetAField(long lPageNo, BYTE* pInput,			FID_DEFINITION enumFieldID, long lPos, int iBlockSize, int iTotalBlock, int iHeaderSize, int iTotalSize, COP_ITEM copMsg);		// PrcVolwSeq Type

	BOOL SetAField(COP_ITEM copInput) {return PutDataIntoDCache(copInput);}
	BOOL DropMCharArrayLine(long lPageNo, FID_DEFINITION enumFieldID, long lCurTl, long lNewTl);
	BOOL DropAField(long lPageNo, FID_DEFINITION enumFieldID, COP_ITEM copMsg);

	// Monitoring Function 

	void ForceSingleCallback(CObject* pCallbackObject,
							void(*fpCallbackFunction)(long lPageNo, FID_DEFINITION enumFieldID, long lIndex, COP_ITEM copMsg, CObject *pCallbackObject));
	BOOL AddAPageWatcher(	long lPageNo, CObject* pCallbackObject, 
							void (*fpCallbackFunction)(long lPageNo, FID_DEFINITION enumFieldID, long lIndex, COP_ITEM copMsg, CObject *pCallbackObject));
	BOOL AddAFieldWatcher(	FID_DEFINITION enumFieldID, BYTE bytRecordType, BYTE bytMarketCode, BYTE bytSubMarket, CObject* pCallbackObject,
							void (*fpCallbackFunction)(long lPageNo, FID_DEFINITION enumFieldID, long lIndex, COP_ITEM copMsg, CObject *pCallbackObject));
	BOOL AddAPageAndFieldWatcher(long lPageNo, FID_DEFINITION enumFieldID, CObject* pCallbackObject,
								 void (*fpCallbackFunction)(long lPageNo, FID_DEFINITION enumFieldID, long lIndex, COP_ITEM copMsg, CObject *pCallbackObject));
	BOOL AddAPageAndFieldWatcher(long lPageNo, FID_DEFINITION enumFieldID,  BYTE bytRecordType, BYTE bytMarketCode, BYTE bytSubMarket, CObject* pCallbackObject,
								 void (*fpCallbackFunction)(long lPageNo, FID_DEFINITION enumFieldID, long lIndex, COP_ITEM copMsg, CObject *pCallbackObject));
	BOOL AddAAnyWatcher(	CObject* pCallbackObject,
							void (*fpCallbackFunction)(long lPageNo, FID_DEFINITION enumFieldID, long lIndex, COP_ITEM copMsg, CObject *pCallbackObject));
	BOOL AddAPageAndMultipleFieldWatcher(long lPageNo,  CWordArray& cwaFIDList, CObject* pCallbackObject,
								 void (*fpCallbackFunction)(long lPageNo, FID_DEFINITION enumFieldID, long lIndex, COP_ITEM copMsg, CObject *pCallbackObject));
	
	BOOL RemoveAPageWatcher(const long lPageNo, CObject* pCallbackObject);
	BOOL RemoveAFieldWatcher(const FID_DEFINITION enumFieldID, BYTE bytRecordType, BYTE bytMarketCode, BYTE bytSubMarket, CObject* pCallbackObject);
	BOOL RemoveAPageAndFieldWatcher(const long lPageNo, const FID_DEFINITION enumFieldID, BYTE bytRecordType, BYTE bytMarketCode, BYTE bytSubMarket, CObject* pCallbackObject);
	BOOL RemoveAPageAndFieldWatcher(const long lPageNo, const FID_DEFINITION enumFieldID, CObject* pCallbackObject);
	BOOL RemoveAPageAndMultipleFieldWatcher(const long lPageNo, CWordArray& cwaFIDList, CObject* pCallbackObject);
	BOOL RemoveAAnyWatcher(CObject* pCallbackObject);

	void SetCallback1301(bool bEnable=false) {m_bCB1301 = bEnable;}
	// Retrieve Data
	BOOL GetAField(long lPageNo, unsigned char& bOutput,	FID_DEFINITION enumFieldID, long lIndex);	// WCHAR
	BOOL GetAField(long lPageNo, long& lOutput,				FID_DEFINITION enumFieldID, long lIndex);	// long 
	BOOL GetAField(long lPageNo, unsigned long& lOutput,	FID_DEFINITION enumFieldID, long lIndex);	// unsigned short
	BOOL GetAField(long lPageNo, short& nOutput,			FID_DEFINITION enumFieldID, long lIndex);	// short
	BOOL GetAField(long lPageNo, unsigned short& nOutput,	FID_DEFINITION enumFieldID, long lIndex);	// unsigned long
	BOOL GetAField(long lPageNo, float& fOutput,			FID_DEFINITION enumFieldID, long lIndex);	// float
	BOOL GetAField(long lPageNo, double& lfOutput,			FID_DEFINITION enumFieldID, long lIndex);	// Double
	BOOL GetAField(long lPageNo, char& cOutput,				FID_DEFINITION enumFieldID, long lIndex);	// char

	BOOL GetAField(long lPageNo, CharArray& caOutput,		FID_DEFINITION enumFieldID, long lIndex);	//  CharArray
	BOOL GetAField(long lPageNo, MCharArray& mcaOutput,		FID_DEFINITION enumFieldID, long lIndex);	//  MCharArray 
	BOOL GetAField(long lPageNo, BDateYear& bdyOutput,		FID_DEFINITION enumFieldID, long lIndex);	//  BDateYear 
	BOOL GetAField(long lPageNo, BDateMonth& bdmOutput,		FID_DEFINITION enumFieldID, long lIndex);	//  BDateMonth 
	BOOL GetAField(long lPageNo, BTimeMin& btmOutput,		FID_DEFINITION enumFieldID, long lIndex);	//  BTimeMin 
	BOOL GetAField(long lPageNo, BTimeSec& btsOutput,		FID_DEFINITION enumFieldID, long lIndex);	//  BTimeSec 
	BOOL GetAField(long lPageNo, BTimeRanges& btrOutput,	FID_DEFINITION enumFieldID, long lIndex);	//  BTimeRanges 
	BOOL GetAField(long lPageNo, Tran& tOutput,				FID_DEFINITION enumFieldID, long lIndex);	//  Tran 
	BOOL GetAField(long lPageNo, ShrOrdLst& solOutput,		FID_DEFINITION enumFieldID, long lIndex);	//  ShrOrdLst 
	BOOL GetAField(long lPageNo, PrcVol& pvOutput,			FID_DEFINITION enumFieldID, long lIndex);	//  PrcVol 
	BOOL GetAField(long lPageNo, BkrQue& bqOutput,			FID_DEFINITION enumFieldID, long lIndex);	//  BkrQue 
	BOOL GetAField(long lPageNo, Link& lOutput,				FID_DEFINITION enumFieldID, long lIndex);	//  Link 
	BOOL GetAField(long lPageNo, WLink& wlOutput,			FID_DEFINITION enumFieldID, long lIndex);	//  WLink 
	BOOL GetAField(long lPageNo, YLabel& ylOutput,			FID_DEFINITION enumFieldID, long lIndex);	//  YLabel 
	BOOL GetAField(long lPageNo, XMtcLabel& xmtclOutput,	FID_DEFINITION enumFieldID, long lIndex);	//  XMtcLabel 
	BOOL GetAField(long lPageNo, XDtcLabel& xdtclOutput,	FID_DEFINITION enumFieldID, long lIndex);	//  XDtcLabel 
	BOOL GetAField(long lPageNo, MtcPoints& mtcpOutput,		FID_DEFINITION enumFieldID, long lIndex);	//  MtcPoints 
	BOOL GetAField(long lPageNo, MtcVal& mtcvOutput,		FID_DEFINITION enumFieldID, long lIndex);	//  MtcVal 
	BOOL GetAField(long lPageNo, DtcPoints& dtcpOutput,		FID_DEFINITION enumFieldID, long lIndex);	//  DtcPoints 
	BOOL GetAField(long lPageNo, HisVal& hvOutput,			FID_DEFINITION enumFieldID, long lIndex);	//  HisVal 
	BOOL GetAField(long lPageNo, FieldList& flOutput,		FID_DEFINITION enumFieldID, long lIndex);	//  FieldList 
	BOOL GetAField(long lPageNo, FieldLabel& flOutput,		FID_DEFINITION enumFieldID, long lIndex);	//  FieldLabel 
	BOOL GetAField(long lPageNo, InfoMsg& imOutput,			FID_DEFINITION enumFieldID, long lIndex);	//  InfoMsg 
	BOOL GetAField(long lPageNo, HLink& hlOutput,			FID_DEFINITION enumFieldID, long lIndex);	//  HLink 
	BOOL GetAField(long lPageNo, SprTbl& stOutput,			FID_DEFINITION enumFieldID, long lIndex);	//  SprTbl 
	BOOL GetAField(long lPageNo, QuoteList& qlOutput,		FID_DEFINITION enumFieldID, long lIndex);	//	Whole Quote List
	BOOL GetAField(long lPageNo, QuoteList& qlOutput,		FID_DEFINITION enumFieldID, long lIndex, byte byteLn);				// Quote line list
	BOOL GetAField(long lPageNo, QuoteList& qlOutput,		FID_DEFINITION enumFieldID, long lIndex, byte byteLn, short sFid);	// Quote cell
	BOOL GetAField(long lPageNo, MTCTicks& mtctOutput,		FID_DEFINITION enumFieldID, long lIndex);	//  MTCTicks 
	BOOL GetAField(long lPageNo, MTCV& mtcvOutput,			FID_DEFINITION enumFieldID, long lIndex);	//  MTCV
	BOOL GetAField(long lPageNo, OBkrQue& obqOutput,		FID_DEFINITION enumFieldID, long lIndex);	//  OBkrQue 
	BOOL GetAField(long lPageNo, BQueVOSN& bquevosnOutput,	FID_DEFINITION enumFieldID, long lIndex);	//  BQueVOSN 
	BOOL GetAField(long lPageNo, MTCPTS& mtcpOutput,		FID_DEFINITION enumFieldID, long lIndex);	//  MTCPTS 
	BOOL GetAField(long lPageNo, MTCF& mtcfOutput,			FID_DEFINITION enumFieldID, long lIndex);	//  MTCF 

	BOOL GetAField(long lPageNo, TranD& tOutput,			FID_DEFINITION enumFieldID, long lIndex);	//  TranD

	BOOL GetAField(long lPageNo, COP_ITEM& copOutput,		FID_DEFINITION enumFieldID, long& lIndex);	// All other type 
	BOOL GetAField(long lPageNo, COP_ITEM& copOutput,		CDWordArray* cdwaFIDList,   long& lIndex);	// Retrieve a set of data

	BOOL GetFIDList(long lPageNo, CDWordArray& cdwaFIDList);											// Get FID List 
	BOOL GetPageList(CDWordArray& cdwaPageList);														// Get Page List 
	BOOL GetHeadlineList(long lPageNo, CDWordArray& cdwaHeadlineList, FID_DEFINITION enumFieldID);		// Get HeadLine List
	BOOL GetMCharArrayTL(long lPageNo, FID_DEFINITION enumFieldID, long& NoOfElement);					// Get MCharArray TL
	BOOL GetDropFidList(long lPageNo, CDWordArray& cdwaPageList);										// Get Drop Fid List 

	BOOL DropAPage(long lPageNo, COP_ITEM copMsg=NULL);
	long GetIDN_RIC(char* ptrInput); 
	CString GetRicName (long lPageNo);
	BOOL GetAPage(long lPageNo, COP_ITEM& pReturnPageCop);
	BOOL GetMultiField(long lPageNo, CWordArray& m_cwaFIDList, COP_ITEM& copReturnPage);
	int  GetMultiField(long lPageNo, CWordArray& cwaFIDList,  CWordArray& cwOutFIDList, COP_ITEM& copReturnPage);
	BOOL GetDropPageList(CDWordArray& cdwaPageList);													// Get Drop Page List 
	void RemovePageFromDropList(long lPageNo);
	BOOL GetWritingProgress();
	BOOL EnableLog();	
	void DisableLog();
	int  GetFaultCounter() {return m_iNoOfFaults;}
	BOOL LogDataToFile(CString strFileName);
	void RemoveFidFromDropList(long lPageNo, FID_DEFINITION enumFieldID);
	int	 StartSocketServer(int iServerPort);															// Start the socket server;
	BOOL StopSocketServer();																			// Stop the socket server;
	BOOL GetSocketServerStatus() {return m_bEnableTCPServer;}
	void ForceWriteToDisk();

	CString GetLastFault() {return m_strLastFault;}
	BOOL ReadData(FILE* data_fp);
	void RetrieveDataByTCP(CString host, long port);
	BOOL CheckTCPConnectionAlive();
	BOOL IsPageExist(long lPageNo);
	BOOL IsPageFidExist(long lPageNo, FID_DEFINITION enumFieldID);

	unsigned char GetTlogRipple(FID_DEFINITION enumFieldID) ;
//	void AddTlogRipple(FID_DEFINITION enumFieldID, unsigned char ucNoOfRipple) ;
//	BOOL ReadTLogRipple(CString filename);

	BOOL IsFidUpdated(FID_DEFINITION Fid);
	//BOOL TryEnterCriticalSection()	{return m_CriticalSection.TryEnter(); }
	BOOL TryEnterCriticalSection(long lPageNo);
	//void LeaveCriticalSection()		{ m_CriticalSection.Leave(); }
	void LeaveCriticalSection(long lPageNo);
	bool EnableDataCompression(bool bEnable =true);
protected:
	friend class CCallback;
	friend class CWriteDataThread;
	void LogFault(char* szLastFault, BOOL bIncreaseCount = TRUE);
private :
// Operations
	void StartWriteDataThread();
	void StopWriteDataThread();
	void GetDataFromDisk();
	void SetPageNo(const char* szString, const char* szOrgString, long lPageNo);
	void SetDropFidNo(FID_DEFINITION enumFieldID = FID_REV_LST5) {m_drop_fid = enumFieldID;}
	FID_DEFINITION GetDropFidNo() {return m_drop_fid;}
	
	void ProcessTranConv(void *p);
	

// Attributes
private :
	FID_DEFINITION		m_drop_fid;
	ssCriticalSection	m_CriticalSection[CS_HASH_NUM+1];
	CPagesMap			m_mapPages;
	CMapStringToOb 		m_mapPagesName;
	CCallback*			m_pCallback;
	CWriteDataThread*	m_WriteDataThread;
	long				m_lNoOfPage ;
	BOOL				m_WriteToDiskFlag;
	CPageList*			m_pIgnorePages;
	CPageList*			m_pIgnoredRecordType;
	CPageList*			m_pIgnoreMarket;
	CString				m_IniFileName;	
	CString				m_DataFileName;	
	int					m_iWriteFIDTimeIntv;
	int					m_iNoOfFaults;
	CString				m_strLastFault;
	BOOL				m_bLogTurnOn;
	FILE*				m_LogFile_fp;
	CFieldsMap			m_mapDropPages;
	CSink*				m_sink;
	CSource*			m_source;	
	BOOL				m_bEnableTCPServer;
	int					m_iServerPort;
	unsigned char		m_FidUpdateFlag[FID_END/8+1];
//	CTLogRippleMap*		m_mapTLogRipple;

	bool				m_bCB1301;

public :
	CWordArray			m_cwaWriteFIDList;
	static bool			g_bCompression;
};

// Initialize the DLL, register the classes etc
extern "C" AFX_EXT_API void WINAPI InitDCacheDLL();

#undef AFX_DATA
#define AFX_DATA

/////////////////////////////////////////////////////////////////////////////
#endif // __DCache_H__
