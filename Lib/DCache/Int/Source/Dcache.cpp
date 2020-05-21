/////////////////////////////////////////////////////////////////////////////
//          File:: DCache.cpp
// Create Author:: Raymond Yung
// Create   Date:: 1 September 00
//
//   Description:: DCache Class member function
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
#include <afxdllx.h>
#include "DCache.h"
#include "Callback.h"
#include "PageList.h"
#include "DCacheFidIterator.h"
#include "Sink.h"
#include "Source.h"
#include "ApplManClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE 
static char THIS_FILE[] = __FILE__;
#endif

static AFX_EXTENSION_MODULE extensionDLL = { NULL, NULL };

bool CDCache::g_bCompression = false;

// -------------------------------------------------------------------------
// Default Constructor
// -------------------------------------------------------------------------
//CDCache::CDCache(BOOL bInstance, char* cInstance, CWordArray* cwaInstance, int iInstance, BOOL bInstance2, CTLogRippleMap* mapTLogRipple):
//m_WriteToDiskFlag(bInstance), m_lNoOfPage(0L), 	m_iNoOfFaults(0), m_strLastFault(""), m_bEnableTCPServer(0),
//m_iServerPort(10014), m_bLogTurnOn(bInstance2), m_LogFile_fp(NULL) ,m_mapTLogRipple(mapTLogRipple)
CDCache::CDCache(BOOL bInstance, char* cInstance, CWordArray* cwaInstance, int iInstance, BOOL bInstance2):
m_WriteToDiskFlag(bInstance), m_lNoOfPage(0L), 	m_iNoOfFaults(0), m_strLastFault(""), m_bEnableTCPServer(0),
m_iServerPort(10014), m_bLogTurnOn(bInstance2), m_LogFile_fp(NULL)
{
	if (m_bLogTurnOn) 
		if (!EnableLog()) 
			m_bLogTurnOn = FALSE;
	SetDropFidNo();

	memset(m_FidUpdateFlag, 0, FID_END/8+1);

	m_mapPages.InitHashTable( DEFAULT_NO_OF_INIT_PAGES );
	m_mapPagesName.InitHashTable( DEFAULT_NO_OF_INIT_PAGES );

	m_pCallback = new CCallback(this);

	char szTemp[MAX_LEN]="";
	char szPath[MAX_LEN] = "";
	CString m_IniFileName;
	GetCurrentDirectory(MAX_LEN, szPath);
	m_IniFileName.Format("%s\\%s",szPath, DCACHEINI);	 
//	CString TLogRipFileName;
//	TLogRipFileName.Format("%s\\%s",szPath, TLOGRIPFILENAME);	 
//	ReadTLogRipple(TLogRipFileName);
	if (cInstance==NULL)
	{
 		if (GetPrivateProfileString("MAIN", "DataFileName", "",szTemp, MAX_LEN, m_IniFileName) > 0)
			m_DataFileName = szTemp;
		else
			m_DataFileName = "c:\\DCACHE";
	}
	else
	{
		m_DataFileName = cInstance;
	}

	if (m_WriteToDiskFlag)
	{
		if (cwaInstance!=NULL)
		{
			int iNoOfFileds = 0;
			if ((iNoOfFileds = cwaInstance->GetUpperBound())>-1)
				for (int i = 0; i<=iNoOfFileds; i++)
					m_cwaWriteFIDList.Add(cwaInstance->GetAt(i));
		}
		m_iWriteFIDTimeIntv = iInstance;
		GetDataFromDisk();
		StartWriteDataThread();
	}

	HKEY hKey;
	DWORD dwEnableTCPServer = 0;
	DWORD dwSize = sizeof(dwEnableTCPServer);
	char szVer[] = "1.4.11.34";
	if ( RegOpenKeyEx(HKEY_LOCAL_MACHINE,
				 "SOFTWARE\\AFE\\DCache",
				 0,
				 KEY_QUERY_VALUE,
				 &hKey) == ERROR_SUCCESS )
	{
		if ( RegQueryValueEx( hKey, 
							  "EnableTCPServer", 
							  NULL, 
							  NULL, 
							  (LPBYTE) &dwEnableTCPServer, 
							  &dwSize) == ERROR_SUCCESS )
        {
			if(dwEnableTCPServer == 1) 
				m_bEnableTCPServer = true;
			else
				m_bEnableTCPServer = false;
        }

		dwSize = sizeof(dwEnableTCPServer);
		if ( RegQueryValueEx( hKey, 
							  "ServerPort", 
							  NULL, 
							  NULL, 
							  (LPBYTE) &dwEnableTCPServer, 
							  &dwSize) == ERROR_SUCCESS )
        {
			m_iServerPort = dwEnableTCPServer ;
        }

		RegCloseKey(hKey);
	}
	if ( RegCreateKey( HKEY_LOCAL_MACHINE,
					   "SOFTWARE\\AFE\\DCache",
					   &hKey) == ERROR_SUCCESS )
	{
		RegSetValueEx(hKey, "Version", 0, REG_SZ, (LPBYTE) szVer, strlen(szVer));
        dwEnableTCPServer = (m_bEnableTCPServer)?1:0;
		RegSetValueEx(hKey, "EnableTCPServer", 0, REG_DWORD, (LPBYTE) &dwEnableTCPServer, dwSize);
        dwEnableTCPServer = m_iServerPort;
        RegSetValueEx(hKey, "ServerPort", 0, REG_DWORD, (LPBYTE) &dwEnableTCPServer, dwSize);
		RegCloseKey(hKey);
	}
	if (m_bEnableTCPServer)
	{
		m_sink = new CSink(this);
		if (!m_sink->Start(m_iServerPort))
		{
			m_bEnableTCPServer = FALSE;
			delete m_sink;
		}
	}
	m_source = new CSource();

	m_bCB1301 = false;
}
// -------------------------------------------------------------------------
// Start the socket server;
// Return value: 0 - cannot start, 1 - start successfully, 2 - already exist
int CDCache::StartSocketServer(int iServerPort)
{
	int iReturnValue = 2;
	if (!m_bEnableTCPServer)
	{
		m_sink = new CSink(this);
		iReturnValue = m_sink->Start(iServerPort);
		m_bEnableTCPServer = (iReturnValue==1)?TRUE:FALSE;
	}
	return iReturnValue;
}
// -------------------------------------------------------------------------
// Stop the socket server;
// Return value: TRUE - Success, FALSE - Unsuccess
BOOL CDCache::StopSocketServer()
{
	BOOL bReturnValue = FALSE;
	if (m_bEnableTCPServer)
	{
		bReturnValue = TRUE;
		m_bEnableTCPServer = FALSE;
		delete m_sink;
	}
	return bReturnValue;
}

// -------------------------------------------------------------------------
void CDCache::ForceWriteToDisk()
{
	if (m_WriteToDiskFlag)
	{
		m_WriteDataThread->Stop();
		if (!m_WriteDataThread->GetWritePageProgress())
			m_WriteDataThread->StartToWritePage();

		while (m_WriteDataThread->GetNoOfPagesRemain()>0)
			m_WriteDataThread->ProcessWriteToDisk();

		if (m_cwaWriteFIDList.GetSize()>0)
		{
			if (!m_WriteDataThread->GetWriteFidProgress())
				m_WriteDataThread->StartToWriteFid();

			while (m_WriteDataThread->GetNoOfFidsRemain()>0)
				m_WriteDataThread->ProcessWriteFIDToDisk();
		}
		m_WriteDataThread->Start();
	}
}

// -------------------------------------------------------------------------

unsigned char CDCache::GetTlogRipple(FID_DEFINITION enumFieldID) 
{
	unsigned char ucNoOfRipple = 10;

	switch (enumFieldID)
	{
	case FID_TRAN_LOG1:  // 2100
	case FID_TRAN_LOG2:
	case FID_TRAN_LOG3:
	case FID_TRAN_LOG4:
	case FID_TRAN_LOG5:
	case FID_TRAN_LOG6:
	case FID_TRAN_LOG7:
	case FID_TRAN_LOG8:
	case FID_TRAN_LOG9:
	case FID_TRAN_LOG10:
	case FID_TRAN_D20_1: // 2715
	case FID_TRAN_D20_2:
	case FID_TRAN_D20_3:
	case FID_TRAN_D20_4:
	case FID_TRAN_D20_5:
		ucNoOfRipple = 20;
		break;
	case FID_TRAN_D12_1: // 2705
	case FID_TRAN_D12_2:
	case FID_TRAN_D12_3:
	case FID_TRAN_D12_4:
	case FID_TRAN_D12_5:
		ucNoOfRipple = 12;
		break;
	case FID_TRAN_D15_1: // 2710
	case FID_TRAN_D15_2:
	case FID_TRAN_D15_3:
	case FID_TRAN_D15_4:
	case FID_TRAN_D15_5:
		ucNoOfRipple = 15;
		break;
	default:
		ucNoOfRipple = 10;
		break;
	}
//	if (m_mapTLogRipple != NULL)
//	{
//		long lNoOfRipple = NULL;
//		if (m_mapTLogRipple->Lookup(enumFieldID, lNoOfRipple))
//			ucNoOfRipple = lNoOfRipple;
//	}
//	if (ucNoOfRipple > 20)
//		ucNoOfRipple = 20;
	return ucNoOfRipple;
}
// -------------------------------------------------------------------------
/*
void CDCache::AddTlogRipple(FID_DEFINITION enumFieldID, unsigned char ucNoOfRipple) 
{
	if (m_mapTLogRipple == NULL)
		m_mapTLogRipple = new CTLogRippleMap;

	m_mapTLogRipple->SetAt(enumFieldID, ucNoOfRipple) ;
}
// -------------------------------------------------------------------------

BOOL CDCache::ReadTLogRipple(CString filename)
{
	BOOL bReturnValue = FALSE;
    FILE *TLogRipple_fp;

    if ((TLogRipple_fp=fopen(filename, "r")) != NULL)
    {
        char date_str[FILENAME_MAX+1];
        long lFieldID, lNoOfRipple;
        while (!feof(TLogRipple_fp))
        {
            if (fgets(date_str, FILENAME_MAX+1, TLogRipple_fp) == NULL)
				continue;
            if (sscanf(date_str, "%lu,%lu", &lFieldID, &lNoOfRipple) != 2)
                continue;
			AddTlogRipple((FID_DEFINITION)lFieldID, lNoOfRipple) ;
			bReturnValue = TRUE;
*/
//        } /* while */
/*
        fclose(TLogRipple_fp);
    }
    else
	    CApplManClient::Trace(TRACE_LEVEL0, "Unable to read TLogRipple file! (%s) Default to normal trading day.",filename);

	if (bReturnValue)
		CApplManClient::Log(LOG_NORMAL, "Read TLogRipple file (%s) successfully.",filename);
    return bReturnValue;    
} 
*/
// -------------------------------------------------------------------------
// Destructor
CDCache::~CDCache()
{
	m_CriticalSection[0].Enter();
	if (m_LogFile_fp!=NULL)
	{
		fflush(m_LogFile_fp);
		fclose(m_LogFile_fp);
	}
	delete m_pCallback;

	if (m_bEnableTCPServer)
		delete m_sink;

	delete m_source;
	if (m_WriteToDiskFlag)
	{
		m_WriteDataThread->ExitThread();
		if (m_WriteDataThread->WaitForExit())
		{
			delete m_WriteDataThread;
			m_WriteDataThread = 0;
		}
	}


//	if (m_mapTLogRipple !=NULL)
//	{
//		m_mapTLogRipple->RemoveAll();
//		delete m_mapTLogRipple;
//	}
	// for clear m_mapPages
	{	
		long	nKey;
		CPage*	objTemp;
		POSITION pos = m_mapPages.GetStartPosition();

		while (pos != NULL)
		{
			m_mapPages.GetNextAssoc( pos, nKey, objTemp );
			CPage* RemovedPage = (CPage*) objTemp;
			m_mapPages.RemoveKey(nKey);
			delete RemovedPage;
		}		
		m_mapPages.RemoveAll();
	}
	// for clear m_mapPagesName
	{
		CString		nKey;
		CObject*	objTemp;
		POSITION pos = m_mapPagesName.GetStartPosition();

		while (pos != NULL)
		{
			m_mapPagesName.GetNextAssoc( pos, nKey, objTemp );
			long* RemovedPage = (long*) objTemp;
			m_mapPagesName.RemoveKey(nKey.GetBuffer(0));
			delete RemovedPage;
		}		
		m_mapPagesName.RemoveAll();
	}

	m_CriticalSection[0].Leave();	
}



//
// ********* ALL OTHER METHODS TO BE PLACED BELOW ***********
//

// -------------------------------------------------------------------------
// 
// Parameters:
// 
// Returns:
//

// -------------------------------------------------------------------------

IMPLEMENT_DYNCREATE(CDCache, CObject)
// -------------------------------------------------------------------------
// Return Value: TRUE if DCache contain page(s), FALSE if no page in DCache 
BOOL CDCache::GetPageList(CDWordArray& cdwaPageList)
{
	int			nKey;
	POSITION	posCurPage;
	BOOL		bReturnValue = FALSE;

	m_CriticalSection[0].Enter();
	if (FirstPage(posCurPage))
		while ((nKey=GetNextPageNo(posCurPage))!=-1)
		{
			cdwaPageList.Add(nKey);
			bReturnValue = TRUE;
		}
	m_CriticalSection[0].Leave();
	return bReturnValue;

}
// -------------------------------------------------------------------------
// Get Drop Page List 
// Return Value: TRUE if DCache contain page(s), FALSE if no page in DCache 
BOOL CDCache::GetDropPageList(CDWordArray& cdwaPageList)							
{
	long		nKey;
	POSITION	posCurPage;
	BOOL		bReturnValue = FALSE;

	m_CriticalSection[0].Enter();
	if ((posCurPage = m_mapDropPages.GetStartPosition())!=NULL)
	{
		CObject*		objTemp;
		while (posCurPage != NULL)
		{
			m_mapDropPages.GetNextAssoc( posCurPage, nKey, objTemp );
			cdwaPageList.Add(nKey);
			bReturnValue = TRUE;
		}
	}
	else
		bReturnValue = FALSE;
	m_CriticalSection[0].Leave();
	return bReturnValue;
}						
// -------------------------------------------------------------------------

BOOL CDCache::GetDropFidList(long lPageNo, CDWordArray& cdwaDropFidList)
{
	BOOL		bReturnValue = FALSE;

	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		if (pPage->GetDropFIDList(cdwaDropFidList))
			bReturnValue = TRUE;
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return bReturnValue;
}	
// -------------------------------------------------------------------------
// Enable the logging fault to the file
BOOL CDCache::EnableLog()
{
	char szPath[MAX_LEN] = "";
	CString strLogFileName;
	GetCurrentDirectory(MAX_LEN, szPath);
	strLogFileName.Format("%s\\%s",szPath, FAULTFILENAME);	 

#if _MSC_VER > 1500 
	if ((fopen_s(&m_LogFile_fp,strLogFileName.GetBuffer(0), "a+b")) != 0)
#else 
	if ((m_LogFile_fp = fopen(strLogFileName.GetBuffer(0), "a+b")) == NULL)
#endif
		m_bLogTurnOn = FALSE;
	else
		m_bLogTurnOn = TRUE;
	return m_bLogTurnOn;
}
// -------------------------------------------------------------------------
// Disable the logging fault to the file & close the log file 
void CDCache::DisableLog()
{
	m_bLogTurnOn = FALSE;
	if (m_LogFile_fp!=NULL)
	{
		fflush(m_LogFile_fp);
		fclose(m_LogFile_fp);
		m_LogFile_fp = NULL;
	}
}
// -------------------------------------------------------------------------
// Log fault
void CDCache::LogFault(char* szLastFault, BOOL bIncreaseCount)
{
	int		iWriteSize = 0;
	char	szFaultMsg[MAX_LEN];

	SYSTEMTIME SysTime;
	GetSystemTime(&SysTime);
	sprintf(szFaultMsg, "%04u/%02u/%02u %02u:%02u:%02u, %s\n", 
		SysTime.wYear, SysTime.wMonth, SysTime.wDay, 
		SysTime.wHour, SysTime.wMinute, SysTime.wSecond, szLastFault);

	if (m_LogFile_fp!=NULL)
	{
		fseek(m_LogFile_fp, 0, SEEK_END);
		iWriteSize = fwrite( &szFaultMsg, 1, strlen(szFaultMsg), m_LogFile_fp);
	}

	CApplManClient::Log(LOG_NORMAL, "%.250s", szLastFault);
	if (bIncreaseCount)
	{
		m_iNoOfFaults++;
		m_strLastFault = szFaultMsg;
	}
}

// -------------------------------------------------------------------------
// Return Value: TRUE if DCache contain page(s), FALSE if no page in DCache 
BOOL CDCache::FirstPage(POSITION& posFirstPage) 
{ 
	BOOL bReturnValue = FALSE;
	if ((posFirstPage = m_mapPages.GetStartPosition())!=NULL)
		 bReturnValue = TRUE;
	return bReturnValue;
}

// -------------------------------------------------------------------------
// Return Value: PageNo 
long CDCache::GetNextPageNo(POSITION& posNextPage)
{
	long		nKey = -1;
	CPage*		objTemp;
	if (posNextPage != NULL)
		m_mapPages.GetNextAssoc( posNextPage, nKey, objTemp );
	return nKey;
}
// -------------------------------------------------------------------------
// Check the page exist or not 
BOOL CDCache::IsPageExist(long lPageNo)
{
	CPage *pPage = NULL;
	return m_mapPages.Lookup(lPageNo, (CPage*)pPage);
}
// -------------------------------------------------------------------------
// Check the fid of page exist or not
BOOL CDCache::IsPageFidExist(long lPageNo, FID_DEFINITION enumFieldID)
{
	BOOL	bReturnValue = FALSE;
	CPage*	pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
		bReturnValue = pPage->IsFidExist(enumFieldID);
	return bReturnValue;
}

// -------------------------------------------------------------------------
// unsigned char 
BOOL CDCache::GetAField(long lPageNo, unsigned char& strOutput, FID_DEFINITION enumFieldID, long lIndex)	
{
	BOOL bReturnValue = FALSE;
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		if (pPage->GetAField(strOutput, (FID_DEFINITION) enumFieldID))
			bReturnValue = TRUE;
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return bReturnValue;
}
// -------------------------------------------------------------------------
// long 
BOOL CDCache::GetAField(long lPageNo, long& lOutput, FID_DEFINITION enumFieldID, long lIndex)	
{
	BOOL bReturnValue = FALSE;
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		if (pPage->GetAField(lOutput, (FID_DEFINITION) enumFieldID))
			bReturnValue = TRUE;
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return bReturnValue;
}
// -------------------------------------------------------------------------
// unsigned short
BOOL CDCache::GetAField(long lPageNo, unsigned long& lOutput, FID_DEFINITION enumFieldID, long lIndex)	
{
	BOOL bReturnValue = FALSE;
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		if (pPage->GetAField(lOutput, (FID_DEFINITION) enumFieldID))
			bReturnValue = TRUE;
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return bReturnValue;
}
// -------------------------------------------------------------------------
// short
BOOL CDCache::GetAField(long lPageNo, short& nOutput, FID_DEFINITION enumFieldID, long lIndex)	
{
	BOOL bReturnValue = FALSE;
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		if (pPage->GetAField(nOutput, (FID_DEFINITION) enumFieldID))
			bReturnValue = TRUE;
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return bReturnValue;
}
// -------------------------------------------------------------------------
// unsigned short
BOOL CDCache::GetAField(long lPageNo, unsigned short& nOutput,FID_DEFINITION enumFieldID, long lIndex)	
{
	BOOL bReturnValue = FALSE;
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		if (pPage->GetAField(nOutput, (FID_DEFINITION) enumFieldID))
			bReturnValue = TRUE;
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return bReturnValue;
}
// -------------------------------------------------------------------------
// float
BOOL CDCache::GetAField(long lPageNo, float& fOutput, FID_DEFINITION enumFieldID, long lIndex)	
{
	BOOL bReturnValue = FALSE;
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		if (pPage->GetAField(fOutput, (FID_DEFINITION) enumFieldID))
			bReturnValue = TRUE;
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return bReturnValue;
}
// -------------------------------------------------------------------------
// Double
BOOL CDCache::GetAField(long lPageNo, double& lfOutput, FID_DEFINITION enumFieldID, long lIndex)	
{
	BOOL bReturnValue = FALSE;
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		if (pPage->GetAField(lfOutput, (FID_DEFINITION) enumFieldID))
			bReturnValue = TRUE;
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();

	return bReturnValue;
}
// -------------------------------------------------------------------------
// char
BOOL CDCache::GetAField(long lPageNo, char& cOutput, FID_DEFINITION enumFieldID, long lIndex)	
{
	BOOL bReturnValue = FALSE;
	
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		if (pPage->GetAField((unsigned char&)cOutput, (FID_DEFINITION) enumFieldID))
			bReturnValue = TRUE;
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();

	return bReturnValue;
}


// -------------------------------------------------------------------------
//  CharArray
BOOL CDCache::GetAField(long lPageNo, CharArray& caOutput, FID_DEFINITION enumFieldID, long lIndex)
{
	BOOL bReturnValue = FALSE;
	
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		CharArray* CA=NULL;
		long iLineNo = -2;
		if (pPage->GetAField(&CA, (FID_DEFINITION) enumFieldID, &iLineNo))
		{
			bReturnValue = TRUE;
			caOutput.Init(CA->GetData());
		}
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();

	return bReturnValue;
}
// -------------------------------------------------------------------------
//  MCharArray 
BOOL CDCache::GetAField(long lPageNo, MCharArray& mcaOutput, FID_DEFINITION enumFieldID, long lIndex)
{
	BOOL bReturnValue = FALSE;
	
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		if (lIndex == -1)
		{
			long iMaxSizeOfFields = -1;
			CharArray* CA=NULL;
			if (pPage->GetAField(&CA, (FID_DEFINITION) enumFieldID, &iMaxSizeOfFields))
			{	
				byte counter = 0;
				for (long i = 0; i<=iMaxSizeOfFields; i++)
				{
					if (pPage->GetAField(&CA, (FID_DEFINITION) enumFieldID, &i))
						if (CA !=NULL)
							if (CA->GetLen() > 0)
							{
								counter = (byte)i;
								mcaOutput.AddLine(counter, CA);
							}
				}
				BYTE bytTL = iMaxSizeOfFields+1;
				mcaOutput.SetTL(bytTL);
				bReturnValue = TRUE;
				lIndex = iMaxSizeOfFields;
			}
		}
		else
		{
			long iMaxSizeOfFields = -1;
			CharArray* CA=NULL;
			MCharArray mcaOutput;
			if (pPage->GetAField(&CA, (FID_DEFINITION) enumFieldID, &iMaxSizeOfFields))
			{
				if (pPage->GetAField(&CA, (FID_DEFINITION) enumFieldID, &lIndex))
					if (CA !=NULL)
						if (CA->GetLen() > 0)
						{
							mcaOutput.AddLine((byte)lIndex, CA);
							BYTE bytTL = iMaxSizeOfFields+1;
							mcaOutput.SetTL(bytTL);
							bReturnValue = TRUE;
						}
			}
		}
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();

	return bReturnValue;
}
// -------------------------------------------------------------------------
//  BDateYear 
BOOL CDCache::GetAField(long lPageNo, BDateYear& bdyOutput, FID_DEFINITION enumFieldID, long lIndex)	
{
	BOOL bReturnValue = FALSE;
	
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		BYTE* bytTempContent=NULL;
		bool bDel = false;
		if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) enumFieldID, bDel))
		{
			bdyOutput.Init(bytTempContent);						
			if(bDel)
				delete bytTempContent;
			bReturnValue = TRUE;
			
		}
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();

	return bReturnValue;
}
// -------------------------------------------------------------------------
//  BDateMonth 
BOOL CDCache::GetAField(long lPageNo, BDateMonth& bdmOutput, FID_DEFINITION enumFieldID, long lIndex)	
{
	BOOL bReturnValue = FALSE;
	
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		BYTE* bytTempContent=NULL;
		bool bDel = false;
		if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) enumFieldID, bDel))
		{
			bdmOutput.Init(bytTempContent);						
			if(bDel)
				delete bytTempContent;						
			bReturnValue = TRUE;
		}
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();

	return bReturnValue;
}
// -------------------------------------------------------------------------
//  BTimeMin 
BOOL CDCache::GetAField(long lPageNo, BTimeMin& btmOutput, FID_DEFINITION enumFieldID, long lIndex)	
{
	BOOL bReturnValue = FALSE;
	
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		BYTE* bytTempContent=NULL;
		bool bDel = false;
		if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) enumFieldID, bDel))
		{
			btmOutput.Init(bytTempContent);						
			if(bDel)
				delete bytTempContent;						
			bReturnValue = TRUE;
		}
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();

	return bReturnValue;
}
// -------------------------------------------------------------------------
//  BTimeSec 
BOOL CDCache::GetAField(long lPageNo, BTimeSec& btsOutput, FID_DEFINITION enumFieldID, long lIndex)	
{
	BOOL bReturnValue = FALSE;
	
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		BYTE* bytTempContent=NULL;
		bool bDel = false;
		if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) enumFieldID, bDel))

		{
			btsOutput.Init(bytTempContent);						
			if(bDel)
				delete bytTempContent;						
			bReturnValue = TRUE;
		}
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();

	return bReturnValue;
}
// -------------------------------------------------------------------------
//  BTimeRanges
BOOL CDCache::GetAField(long lPageNo, BTimeRanges& btrOutput,	FID_DEFINITION enumFieldID, long lIndex)
{
	BOOL bReturnValue = FALSE;
	
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		BYTE* bytTempContent=NULL;
		bool bDel = false;
		if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) enumFieldID, bDel))
		{
			btrOutput.Init(bytTempContent);						
			if(bDel)
				delete bytTempContent;						
			bReturnValue = TRUE;
		}
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();

	return bReturnValue;
}
// -------------------------------------------------------------------------
//  Tran 	 
BOOL CDCache::GetAField(long lPageNo, Tran& tOutput, FID_DEFINITION enumFieldID, long lIndex)	
{
	BOOL bReturnValue = FALSE;
	
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		BYTE* bytTempContent=NULL;
		bool bDel = false;
		if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) enumFieldID, bDel))
		{
			tOutput.Init(bytTempContent);						
			if(bDel)
				delete bytTempContent;						
			bReturnValue = TRUE;
		}
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();

	return bReturnValue;
}
// -------------------------------------------------------------------------
//  TranD 	 
BOOL CDCache::GetAField(long lPageNo, TranD& tOutput, FID_DEFINITION enumFieldID, long lIndex)	
{
	BOOL bReturnValue = FALSE;
	
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		BYTE* bytTempContent=NULL;
		bool bDel = false;
		if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) enumFieldID, bDel))
		{
			tOutput.Init(bytTempContent);						
			if(bDel)
				delete bytTempContent;						
			bReturnValue = TRUE;
		}
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();

	return bReturnValue;
}
// -------------------------------------------------------------------------
//  ShrOrdLst 
BOOL CDCache::GetAField(long lPageNo, ShrOrdLst& solOutput, FID_DEFINITION enumFieldID, long lIndex)	
{
	BOOL bReturnValue = FALSE;
	
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		BYTE* bytTempContent=NULL;
		bool bDel = false;
		if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) enumFieldID, bDel))
		{
			solOutput.Init(bytTempContent);						
			if(bDel)
				delete bytTempContent;						
			bReturnValue = TRUE;
		}
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();

	return bReturnValue;
}
// -------------------------------------------------------------------------
//  PrcVol 
BOOL CDCache::GetAField(long lPageNo, PrcVol& pvOutput, FID_DEFINITION enumFieldID, long lIndex)	
{
	BOOL bReturnValue = FALSE;
	
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		BYTE* bytTempContent=NULL;
		bool bDel = false;
		if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) enumFieldID, bDel))
		{
			pvOutput.Init(bytTempContent);						
			if(bDel)
				delete bytTempContent;						
			bReturnValue = TRUE;
		}
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();

	return bReturnValue;
}
// -------------------------------------------------------------------------
//  BkrQue 
BOOL CDCache::GetAField(long lPageNo, BkrQue& bqOutput, FID_DEFINITION enumFieldID, long lIndex)	
{
	BOOL bReturnValue = FALSE;
	
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		BYTE* bytTempContent=NULL;
		bool bDel = false;
		if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) enumFieldID, bDel))
		{
			bqOutput.Init(bytTempContent);						
			if(bDel)
				delete bytTempContent;						
			bReturnValue = TRUE;
		}
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();

	return bReturnValue;
}
// -------------------------------------------------------------------------
//  Link 
BOOL CDCache::GetAField(long lPageNo, Link& lOutput, FID_DEFINITION enumFieldID, long lIndex)	
{
	BOOL bReturnValue = FALSE;
	
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		BYTE* bytTempContent=NULL;
		bool bDel = false;
		if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) enumFieldID, bDel))
		{
			lOutput.Init(bytTempContent);						
			if(bDel)
				delete bytTempContent;						
			bReturnValue = TRUE;
		}
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();

	return bReturnValue;
}
// -------------------------------------------------------------------------
//  WLink 
BOOL CDCache::GetAField(long lPageNo, WLink& wlOutput, FID_DEFINITION enumFieldID, long lIndex)	
{
	BOOL bReturnValue = FALSE;
	
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		BYTE* bytTempContent=NULL;
		bool bDel = false;
		if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) enumFieldID, bDel))
		{
			wlOutput.Init(bytTempContent);						
			if(bDel)
				delete bytTempContent;						
			bReturnValue = TRUE;
		}
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();

	return bReturnValue;
}
// -------------------------------------------------------------------------
//  YLabel 
BOOL CDCache::GetAField(long lPageNo, YLabel& ylOutput, FID_DEFINITION enumFieldID, long lIndex)	
{
	BOOL bReturnValue = FALSE;
	
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		BYTE* bytTempContent=NULL;
		bool bDel = false;
		if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) enumFieldID, bDel))
		{
			ylOutput.Init(bytTempContent);						
			if(bDel)
				delete bytTempContent;						
			bReturnValue = TRUE;
		}
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();

	return bReturnValue;
}
// -------------------------------------------------------------------------
//  XMtcLabel 
BOOL CDCache::GetAField(long lPageNo, XMtcLabel& xmtclOutput, FID_DEFINITION enumFieldID, long lIndex)	
{
	BOOL bReturnValue = FALSE;
	
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		BYTE* bytTempContent=NULL;
		bool bDel = false;
		if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) enumFieldID, bDel))
		{
			xmtclOutput.Init(bytTempContent);						
			if(bDel)
				delete bytTempContent;						
			bReturnValue = TRUE;
		}
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();

	return bReturnValue;
}
// -------------------------------------------------------------------------
//  XDtcLabel
BOOL CDCache::GetAField(long lPageNo, XDtcLabel& xdtclOutput, FID_DEFINITION enumFieldID, long lIndex)	 
{
	BOOL bReturnValue = FALSE;
	
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		BYTE* bytTempContent=NULL;
		bool bDel = false;
		if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) enumFieldID, bDel))
		{
			xdtclOutput.Init(bytTempContent);						
			if(bDel)
				delete bytTempContent;						
			bReturnValue = TRUE;
		}
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();

	return bReturnValue;
}
// -------------------------------------------------------------------------
//  MtcPoints 
BOOL CDCache::GetAField(long lPageNo, MtcPoints& mtcpOutput, FID_DEFINITION enumFieldID, long lIndex)	
{
	BOOL bReturnValue = FALSE;
	
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		BYTE* bytTempContent=NULL;
		bool bDel = false;
		if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) enumFieldID, bDel))
		{
			mtcpOutput.Init(bytTempContent);						
			if(bDel)
				delete bytTempContent;						
			bReturnValue = TRUE;
		}
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();

	return bReturnValue;
}
// -------------------------------------------------------------------------
//  MtcVal 
BOOL CDCache::GetAField(long lPageNo, MtcVal& mtcvOutput, FID_DEFINITION enumFieldID, long lIndex)	
{
	BOOL bReturnValue = FALSE;
	
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		BYTE* bytTempContent=NULL;
		bool bDel = false;
		if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) enumFieldID, bDel))
		{
			mtcvOutput.Init(bytTempContent);						
			if(bDel)
				delete bytTempContent;						
			bReturnValue = TRUE;
		}
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();

	return bReturnValue;
}
// -------------------------------------------------------------------------
//  DtcPoints 
BOOL CDCache::GetAField(long lPageNo, DtcPoints& dtcpOutput, FID_DEFINITION enumFieldID, long lIndex)	
{
	BOOL bReturnValue = FALSE;
	
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		BYTE* bytTempContent=NULL;
		bool bDel = false;
		if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) enumFieldID, bDel))
		{
			dtcpOutput.Init(bytTempContent);						
			if(bDel)
				delete bytTempContent;						
			bReturnValue = TRUE;
		}
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();

	return bReturnValue;
}
// -------------------------------------------------------------------------
//  HisVal
BOOL CDCache::GetAField(long lPageNo, HisVal& hvOutput, FID_DEFINITION enumFieldID, long lIndex)	 
{
	BOOL bReturnValue = FALSE;
	
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		BYTE* bytTempContent=NULL;
		bool bDel = false;
		if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) enumFieldID, bDel))
		{
			hvOutput.Init(bytTempContent);						
			if(bDel)
				delete bytTempContent;						
			bReturnValue = TRUE;
		}
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();

	return bReturnValue;
}
// -------------------------------------------------------------------------
//  FieldList 
BOOL CDCache::GetAField(long lPageNo, FieldList& flOutput, FID_DEFINITION enumFieldID, long lIndex)	
{
	BOOL bReturnValue = FALSE;
	
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		BYTE* bytTempContent=NULL;
		bool bDel = false;
		if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) enumFieldID, bDel))
		{
			flOutput.Init(bytTempContent);						
			if(bDel)
				delete bytTempContent;						
			bReturnValue = TRUE;
		}
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();

	return bReturnValue;
}
// -------------------------------------------------------------------------
//  FieldLabel 
BOOL CDCache::GetAField(long lPageNo, FieldLabel& flOutput, FID_DEFINITION enumFieldID, long lIndex)	
{
	BOOL bReturnValue = FALSE;
	
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		BYTE* bytTempContent=NULL;
		bool bDel = false;
		if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) enumFieldID, bDel))
		{
			flOutput.Init(bytTempContent);						
			if(bDel)
				delete bytTempContent;						
			bReturnValue = TRUE;
		}
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();

	return bReturnValue;
}

// -------------------------------------------------------------------------
//  InfoMsg 
BOOL CDCache::GetAField(long lPageNo, InfoMsg& imOutput,	FID_DEFINITION enumFieldID, long lIndex)	
{
	BOOL bReturnValue = FALSE;
	
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		BYTE* bytTempContent=NULL;
		bool bDel = false;
		if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) enumFieldID, bDel))
		{
			imOutput.Init(bytTempContent);						
			if(bDel)
				delete bytTempContent;						
			bReturnValue = TRUE;
		}
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();

	return bReturnValue;
}
// -------------------------------------------------------------------------
//  HLink 
BOOL CDCache::GetAField(long lPageNo, HLink& hlOutput, FID_DEFINITION enumFieldID, long lIndex)	
{
	BOOL bReturnValue = FALSE;
	
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		BYTE* bytTempContent=NULL;
		bool bDel = false;
		if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) enumFieldID, bDel))
		{
			hlOutput.Init(bytTempContent);						
			if(bDel)
				delete bytTempContent;						
			bReturnValue = TRUE;
		}
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();

	return bReturnValue;
}
// -------------------------------------------------------------------------
//  SprTbl 
BOOL CDCache::GetAField(long lPageNo, SprTbl& stOutput,	FID_DEFINITION enumFieldID, long lIndex)	
{
	BOOL bReturnValue = FALSE;
	
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		BYTE* bytTempContent=NULL;
		bool bDel = false;
		if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) enumFieldID, bDel))
		{
			stOutput.Init(bytTempContent);						
			if(bDel)
				delete bytTempContent;						
			bReturnValue = TRUE;
		}
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();

	return bReturnValue;
}
// -------------------------------------------------------------------------
//   Whole Quote List
BOOL CDCache::GetAField(long lPageNo, QuoteList& qlOutput, FID_DEFINITION enumFieldID, long lIndex)
{
	BOOL bReturnValue = FALSE;
	
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		if (pPage->GetAField(&qlOutput, (FID_DEFINITION) enumFieldID))
		{
			bReturnValue = TRUE;
		}
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();

	return bReturnValue;
}
// -------------------------------------------------------------------------
//   Quote line list
BOOL CDCache::GetAField(long lPageNo, QuoteList& qlOutput, FID_DEFINITION enumFieldID, long lIndex, byte byteLn)
{
	BOOL bReturnValue = FALSE;
	
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		if (pPage->GetAField(&qlOutput, (FID_DEFINITION) enumFieldID, byteLn))
		{
			bReturnValue = TRUE;
		}
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();

	return bReturnValue;
}
// -------------------------------------------------------------------------
//   Quote cell
BOOL CDCache::GetAField(long lPageNo, QuoteList& qlOutput, FID_DEFINITION enumFieldID, long lIndex, byte byteLn, short sFid)
{
	BOOL bReturnValue = FALSE;
	
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		if (pPage->GetAField(&qlOutput, (FID_DEFINITION) enumFieldID, byteLn, sFid))
		{
			bReturnValue = TRUE;
		}
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();

	return bReturnValue;
}


// -------------------------------------------------------------------------
//  MTCTicks 
BOOL CDCache::GetAField(long lPageNo, MTCTicks& mtctOutput, FID_DEFINITION enumFieldID, long lIndex)	
{
	BOOL bReturnValue = FALSE;
	
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		BYTE* bytTempContent=NULL;
		bool bDel = false;
		if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) enumFieldID, bDel))
		{
			mtctOutput.InitByDoubleByteStream(bytTempContent);						
			bReturnValue = TRUE;
			if(bDel) delete bytTempContent;
		}
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();

	return bReturnValue;
}

// -------------------------------------------------------------------------
//  MTCV
BOOL CDCache::GetAField(long lPageNo, MTCV& mtcvOutput, FID_DEFINITION enumFieldID, long lIndex)	
{
	BOOL bReturnValue = FALSE;
	
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		BYTE* bytTempContent=NULL;
		bool bDel = false;
		if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) enumFieldID, bDel))
		{
			mtcvOutput.InitByDoubleByteStream(bytTempContent);						
			bReturnValue = TRUE;
			if(bDel) delete bytTempContent;
		}
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();

	return bReturnValue;
}


// -------------------------------------------------------------------------
//  OBkrQue 
BOOL CDCache::GetAField(long lPageNo, OBkrQue& bqOutput, FID_DEFINITION enumFieldID, long lIndex)	
{
	BOOL bReturnValue = FALSE;
	
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		BYTE* bytTempContent=NULL;
		bool bDel = false;
		if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) enumFieldID, bDel))
		{
			bqOutput.Init(bytTempContent);						
			if(bDel)
				delete bytTempContent;						
			bReturnValue = TRUE;
		}
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();

	return bReturnValue;
}

// -------------------------------------------------------------------------
//  BQueVOSN 
BOOL CDCache::GetAField(long lPageNo, BQueVOSN& bquevosnOutput,	FID_DEFINITION enumFieldID, long lIndex)
{
	BOOL bReturnValue = FALSE;
	
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		BYTE* bytTempContent=NULL;
		bool bDel = false;
		if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) enumFieldID, bDel))
		{
			bquevosnOutput.Init(bytTempContent);						
			if(bDel)
				delete bytTempContent;						
			bReturnValue = TRUE;
		}
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();

	return bReturnValue;
}

// -------------------------------------------------------------------------
//  MTCPTS 
BOOL CDCache::GetAField(long lPageNo, MTCPTS& mtcpOutput, FID_DEFINITION enumFieldID, long lIndex)	
{
	BOOL bReturnValue = FALSE;
	
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		BYTE* bytTempContent=NULL;
		bool bDel = false;
		if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) enumFieldID, bDel))
		{
			mtcpOutput.Init(bytTempContent);						
			if(bDel)
				delete bytTempContent;						
			bReturnValue = TRUE;
		}
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();

	return bReturnValue;
}
// -------------------------------------------------------------------------
//  MTCF 
BOOL CDCache::GetAField(long lPageNo, MTCF& mtcfOutput, FID_DEFINITION enumFieldID, long lIndex)	
{
	BOOL bReturnValue = FALSE;
	
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		BYTE* bytTempContent=NULL;
		bool bDel = false;
		if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) enumFieldID, bDel))
		{
			mtcfOutput.Init(bytTempContent);						
			if(bDel)
				delete bytTempContent;						
			bReturnValue = TRUE;
		}
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();

	return bReturnValue;
}// 
// -------------------------------------------------------------------------
// Retrieve a set of data
BOOL CDCache::GetAField(long lPageNo, COP_ITEM& copOutput, CDWordArray* cdwaFIDList, long& lIndex)
{
	BOOL bReturnValue = FALSE;

	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		int iNoOfFid = cdwaFIDList->GetSize();
		if (iNoOfFid > 0)
		{
			MsgObject* MsgObj = new MsgObject();
			int nKey = 0;
			fidDatabase fDB;


			for (int iFid = 0; iFid < iNoOfFid; iFid++)
			{
				nKey = cdwaFIDList->GetAt(iFid);
				switch (fDB.GetFidType(nKey))
				{
					case NGN_CHAR:
						{
							unsigned char ch;
							if (pPage->GetAField(ch, (FID_DEFINITION) nKey))
							{
								MsgObj->AddFid(nKey, (unsigned char*) &ch);	// char	
								bReturnValue = TRUE;
							}
						}
						break;
					//case NGN_UNICODE:
					//	{
					//		WCHAR uni;
					//		if (pPage->GetAField((unsigned short)uni, (FID_DEFINITION) nKey))
					//		{
					//			MsgObj->AddFid(nKey, &uni);// unicode 
					//			bReturnValue = TRUE;
					//		}
					//	}
					//	break;
					case NGN_SHORT:
						{
							short sh = 0;
							if (pPage->GetAField(sh, (FID_DEFINITION) nKey))
							{
								MsgObj->AddFid(nKey, (short*) &sh);			// Short	
								bReturnValue = TRUE;
							}
						}
						break;
					case NGN_USHORT:
						{
							unsigned short ush =0;
							if (pPage->GetAField(ush, (FID_DEFINITION) nKey))
							{
								MsgObj->AddFid(nKey, (unsigned short*) &ush);// uShort	
								bReturnValue = TRUE;
							}
						}
						break;
					case NGN_LONG:
					case NGN_KMULTIPLE:
					case NGN_10KMULTIPLE:
						{
							long l = 0;
							if (pPage->GetAField(l, (FID_DEFINITION) nKey))
							{
								MsgObj->AddFid(nKey, (long*) &l);			// long	
								bReturnValue = TRUE;
							}
						}
						break;
					case NGN_ULONG:
						{
							unsigned long ul = 0;
							if (pPage->GetAField(ul, (FID_DEFINITION) nKey))
							{
								MsgObj->AddFid(nKey, (unsigned long*) &ul);	// ulong			
								bReturnValue = TRUE;
							}
						}
						break;
					case NGN_FLOAT:
					case NGN_UFLOAT:
						{
							float f = 0;
							if (pPage->GetAField(f, (FID_DEFINITION) nKey))
							{
								MsgObj->AddFid(nKey, (float*) &f);			// float
								bReturnValue = TRUE;
							}
						}
						break;
					case NGN_DOUBLE:
					case NGN_UDOUBLE:
						{
							double d = 0;
							if (pPage->GetAField(d, (FID_DEFINITION) nKey))
							{
								MsgObj->AddFid(nKey, (double*) &d);			// double
								bReturnValue = TRUE;
							}
						}
						break;
					case NGN_CHARARRAY:
						{
							CharArray* CA=NULL;
							long iLineNo = -2;
							if (pPage->GetAField(&CA, (FID_DEFINITION) nKey, &iLineNo))
							{
								MsgObj->AddFid(nKey, CA);						// CharArray
								bReturnValue = TRUE;
							}
						}
						break;
					case NGN_MCHARARRAY:
						{
							if (lIndex == -1)
							{
								long iMaxSizeOfFields = -1;
								CharArray* CA=NULL;
								MCharArray mca;
								if (pPage->GetAField(&CA, (FID_DEFINITION) nKey, &iMaxSizeOfFields))
								{	
									byte counter = 0;
									for (long i = 0; i<=iMaxSizeOfFields; i++)
									{
										if (pPage->GetAField(&CA, (FID_DEFINITION) nKey, &i))
											if (CA !=NULL)
												if (CA->GetLen() > 0)
												{
													counter = (byte)i;
													mca.AddLine(counter, CA);
												}
									}
									BYTE bytTL = iMaxSizeOfFields+1;
									mca.SetTL(bytTL);
									MsgObj->AddFid(nKey, &mca);
									bReturnValue = TRUE;
									lIndex = iMaxSizeOfFields;
								}
							}
							else
							{
								long iMaxSizeOfFields = -1;
								CharArray* CA=NULL;
								MCharArray mca;
								if (pPage->GetAField(&CA, (FID_DEFINITION) nKey, &iMaxSizeOfFields))
								{
									if (pPage->GetAField(&CA, (FID_DEFINITION) nKey, &lIndex))
										if (CA !=NULL)
											if (CA->GetLen() > 0)
											{
												mca.AddLine((byte)lIndex, CA);
												BYTE bytTL = iMaxSizeOfFields+1;
												mca.SetTL(bytTL);
												MsgObj->AddFid(nKey, &mca);
												bReturnValue = TRUE;
											}
								}
							}
						}
						break;
					case NGN_BDATEYEAR:
						{
							BYTE* bytTempContent=NULL;
							bool bDel = false;
							if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
							{
								BDateYear dy(bytTempContent);						
								MsgObj->AddFid(nKey, &dy);
								bReturnValue = TRUE;
								if(bDel) delete bytTempContent;
							}
						}
						break;
					case NGN_BDATEMONTH:
						{
							BYTE* bytTempContent=NULL;
							bool bDel = false;
							if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
							{
								BDateMonth m(bytTempContent);
								MsgObj->AddFid(nKey, &m);
								bReturnValue = TRUE;
								if(bDel) delete bytTempContent;
							}
						}
						break;
					case NGN_BTIMEMIN:
						{
							BYTE* bytTempContent=NULL;
							bool bDel = false;
							if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
							{
								BTimeMin tm(bytTempContent);
								MsgObj->AddFid(nKey, &tm);
								bReturnValue = TRUE;
								if(bDel) delete bytTempContent;
							}
						}
						break;
					case NGN_BTIMESEC:
						{
							BYTE* bytTempContent=NULL;
							bool bDel = false;
							if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
							{
								BTimeSec s(bytTempContent);
								MsgObj->AddFid(nKey, &s);
								bReturnValue = TRUE;
								if(bDel) delete bytTempContent;
							}
						}
						break;
					case NGN_BTIMERANGES:
						{
							BYTE* bytTempContent=NULL;
							bool bDel = false;
							if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
							{
								BTimeRanges tr(bytTempContent);						
								MsgObj->AddFid(nKey, &tr);
								bReturnValue = TRUE;
								if(bDel) delete bytTempContent;
							}
						}
						break;
					case NGN_TRAN:
						{
							BYTE* bytTempContent=NULL;
							bool bDel = false;
							if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
							{
								Tran tran(bytTempContent);
								MsgObj->AddFid(nKey, &tran);
								bReturnValue = TRUE;
								if(bDel) delete bytTempContent;
							}
						}
						break;
					case NGN_SHRORDLST:
						{
							BYTE* bytTempContent=NULL;
							bool bDel = false;
							if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
							{
								ShrOrdLst sol(bytTempContent);
								MsgObj->AddFid(nKey, &sol);
								bReturnValue = TRUE;
								if(bDel) delete bytTempContent;
							}
						}
						break;
					case NGN_PRCVOL:
						{
							BYTE* bytTempContent=NULL;
							bool bDel = false;
							if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
							{
								PrcVol prc(bytTempContent);
								MsgObj->AddFid(nKey, &prc);
								bReturnValue = TRUE;
								if(bDel) delete bytTempContent;
							}
						}
						break;
					case NGN_BKRQUE:
						{
							BYTE* bytTempContent=NULL;
							bool bDel = false;
							if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
							{
								BkrQue bkrque(bytTempContent);
								MsgObj->AddFid(nKey, &bkrque);
								bReturnValue = TRUE;
								if(bDel) delete bytTempContent;
							}
						}
						break;
					case NGN_LINK:
						{
							BYTE* bytTempContent=NULL;
							bool bDel = false;
							if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
							{
								Link link(bytTempContent);
								MsgObj->AddFid(nKey, &link);
								bReturnValue = TRUE;
								if(bDel) delete bytTempContent;
							}
						}
						break;
					case NGN_WLINK:
						{
							BYTE* bytTempContent=NULL;
							bool bDel = false;
							if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
							{
								WLink wlink(bytTempContent);
								MsgObj->AddFid(nKey, &wlink);
								bReturnValue = TRUE;
								if(bDel) delete bytTempContent;
							}
						}
						break;
					case NGN_YLABEL:
						{
							BYTE* bytTempContent=NULL;
							bool bDel = false;
							if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
							{
								YLabel ylabel(bytTempContent);
								MsgObj->AddFid(nKey, &ylabel);
								bReturnValue = TRUE;
								if(bDel) delete bytTempContent;
							}
						}
						break;
					case NGN_XMTCLABEL:
						{
							BYTE* bytTempContent=NULL;
							bool bDel = false;
							if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
							{
								XMtcLabel xmtcLbl(bytTempContent);
								MsgObj->AddFid(nKey, &xmtcLbl);
								bReturnValue = TRUE;
								if(bDel) delete bytTempContent;
							}
						}
						break;
					case NGN_XDTCLABEL:
						{
							BYTE* bytTempContent=NULL;
							bool bDel = false;
							if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
							{
								XDtcLabel xdtcLbl(bytTempContent);
								MsgObj->AddFid(nKey, &xdtcLbl);
								bReturnValue = TRUE;
								if(bDel) delete bytTempContent;
							}
						}
						break;
					case NGN_MTCPOINTS:
						{
							BYTE* bytTempContent=NULL;
							bool bDel = false;
							if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
							{
								MtcPoints mtcPts(bytTempContent);
								MsgObj->AddFid(nKey, &mtcPts);
								bReturnValue = TRUE;
								if(bDel) delete bytTempContent;
							}
						}
						break;
					case NGN_MTCVAL:
						{
							BYTE* bytTempContent=NULL;
							bool bDel = false;
							if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
							{
								MtcVal mtcval(bytTempContent);
								MsgObj->AddFid(nKey, &mtcval);
								bReturnValue = TRUE;
								if(bDel) delete bytTempContent;
							}
						}
						break;
					case NGN_DTCPOINTS:
						{
							BYTE* bytTempContent=NULL;
							bool bDel = false;
							if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
							{
								DtcPoints dtcPts(bytTempContent);
								MsgObj->AddFid(nKey, &dtcPts);
								bReturnValue = TRUE;
								if(bDel) delete bytTempContent;
							}
						}
						break;
					case NGN_HISVAL:
						{
							BYTE* bytTempContent=NULL;
							bool bDel = false;
							if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
							{
								HisVal hisval(bytTempContent);
								MsgObj->AddFid(nKey, &hisval);
								bReturnValue = TRUE;
								if(bDel) delete bytTempContent;
							}
						}
						break;
					case NGN_FIELDLIST:
						{
							BYTE* bytTempContent=NULL;
							bool bDel = false;
							if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
							{
								FieldList fldlst(bytTempContent);
								MsgObj->AddFid(nKey, &fldlst);
								bReturnValue = TRUE;
								if(bDel) delete bytTempContent;
							}
						}
						break;
					case NGN_FIELDLABEL:
						{
							BYTE* bytTempContent=NULL;
							bool bDel = false;
							if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
							{
								FieldLabel fldlbl(bytTempContent);
								MsgObj->AddFid(nKey, &fldlbl);
								bReturnValue = TRUE;
								if(bDel) delete bytTempContent;
							}
						}
						break;
					case NGN_INFOMSG:
						{
							BYTE* bytTempContent=NULL;
							bool bDel = false;
							if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
							{
								InfoMsg infomsg(bytTempContent);
								MsgObj->AddFid(nKey, &infomsg);
								bReturnValue = TRUE;
								if(bDel) delete bytTempContent;
							}
						}
						break;
					case NGN_HLINK:
						{
							BYTE* bytTempContent=NULL;
							bool bDel = false;
							if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
							{
								HLink hlink(bytTempContent);
								MsgObj->AddFid(nKey, &hlink);
								bReturnValue = TRUE;
								if(bDel) delete bytTempContent;
							}
						}
						break;
					case NGN_HEADLINE:
						{
							BYTE* bytTempContent=NULL;
							if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, &lIndex))
							{
								HeadLine headline(bytTempContent);
								MsgObj->AddFid(nKey, &headline);
								bReturnValue = TRUE;
							}
						}
						break;
					case NGN_SPRTBL:
						{
							BYTE* bytTempContent=NULL;
							bool bDel = false;
							if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
							{
								SprTbl sprtbl(bytTempContent);
								MsgObj->AddFid(nKey, &sprtbl);
								bReturnValue = TRUE;
								if(bDel) delete bytTempContent;
							}
						}
						break;
/*					case NGN_QUOTELIST:
						{
							BYTE* bytTempContent=NULL;
							QuoteList qlOutput;
							if (pPage->GetAField(&qlOutput, (FID_DEFINITION) nKey))
							{
								MsgObj->AddFid(nKey, &qlOutput);
								bReturnValue = TRUE;
							}
						}
						break;
*/					case NGN_MTCTICKS:
						{
							BYTE* bytTempContent=NULL;
							bool bDel = false;
							if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
							{
								MTCTicks mtcticks;
								mtcticks.InitByDoubleByteStream(bytTempContent);
								MsgObj->AddFid(nKey, &mtcticks);
								bReturnValue = TRUE;
								if(bDel) delete bytTempContent;
							}
						}
						break;
					case NGN_MTCV:
						{
							BYTE* bytTempContent=NULL;
							bool bDel = false;
							if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
							{
								MTCV mtcv;
								mtcv.InitByDoubleByteStream(bytTempContent);
								MsgObj->AddFid(nKey, &mtcv);
								bReturnValue = TRUE;
								if(bDel) delete bytTempContent;
							}
						}
						break;
					case NGN_YHEADLINE:
						{
							BYTE* bytTempContent=NULL;
							if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, &lIndex))
							{
								YHeadLine yheadline(bytTempContent);
								MsgObj->AddFid(nKey, &yheadline);
								bReturnValue = TRUE;
							}
						}
						break;
					case NGN_OBKRQUE:
						{
							BYTE* bytTempContent=NULL;
							bool bDel = false;
							if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
							{
								OBkrQue obkrque(bytTempContent);
								MsgObj->AddFid(nKey, &obkrque);
								bReturnValue = TRUE;
								if(bDel) delete bytTempContent;
							}
						}
						break;
					case NGN_BQUEVOSN:
						{
							BYTE* bytTempContent=NULL;
							bool bDel = false;
							if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
							{
								BQueVOSN bquevosn(bytTempContent);
								MsgObj->AddFid(nKey, &bquevosn);
								bReturnValue = TRUE;
								if(bDel) delete bytTempContent;
							}
						}
						break;
					case NGN_MTCPTS:
						{
							BYTE* bytTempContent=NULL;
							bool bDel = false;
							if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
							{
								MTCPTS mtcPts(bytTempContent);
								MsgObj->AddFid(nKey, &mtcPts);
								bReturnValue = TRUE;
								if(bDel) delete bytTempContent;
							}
						}
						break;
					case NGN_MTCF:
						{
							BYTE* bytTempContent=NULL;
							bool bDel = false;
							if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
							{
								MTCF mtcf(bytTempContent);
								MsgObj->AddFid(nKey, &mtcf);
								bReturnValue = TRUE;
								if(bDel) delete bytTempContent;
							}
						}
						break;
					case NGN_PRCVOLWSEQ:
						{
							BYTE* bytTempContent=NULL;
							bool bDel = false;
							if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
							{
								PrcVolwSeq prc(bytTempContent);
								MsgObj->AddFid(nKey, &prc);
								bReturnValue = TRUE;
								if(bDel) delete bytTempContent;
							}
						}
						break;
					case NGN_TRAN_D:
						{
							BYTE* bytTempContent=NULL;
							bool bDel = false;
							if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
							{
								TranD tranD(bytTempContent);
								MsgObj->AddFid(nKey, &tranD);
								bReturnValue = TRUE;
								if(bDel) delete bytTempContent;
							}
						}
						break;
					default:
						// ERROR MESSAGE
						break;
				}
			}
			MsgObj->SetItemNo(lPageNo);
			copOutput = new MsgObject(MsgObj->GetMessage());
			delete MsgObj;
		}
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return bReturnValue;
}

// -------------------------------------------------------------------------
// All other type 
BOOL CDCache::GetAField(long lPageNo, COP_ITEM& copOutput, FID_DEFINITION enumFieldID, long& lIndex)	
{
	BOOL bReturnValue = FALSE;

	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		MsgObject* MsgObj = new MsgObject();
		int nKey = enumFieldID;
		fidDatabase fDB;
		switch (fDB.GetFidType(nKey))
		{
			case NGN_CHAR:
				{
					unsigned char ch;
					if (pPage->GetAField(ch, (FID_DEFINITION) nKey))
					{
						MsgObj->AddFid(nKey, (unsigned char*) &ch);	// char	
						bReturnValue = TRUE;
					}
				}
				break;
			//case NGN_UNICODE:
			//	{
			//		WCHAR uni;
			//		if (pPage->GetAField((unsigned short)uni, (FID_DEFINITION) nKey))
			//		{
			//			MsgObj->AddFid(nKey, &uni);// unicode 
			//			bReturnValue = TRUE;
			//		}
			//	}
			//	break;
			case NGN_SHORT:
				{
					short sh = 0;
					if (pPage->GetAField(sh, (FID_DEFINITION) nKey))
					{
						MsgObj->AddFid(nKey, (short*) &sh);			// Short	
						bReturnValue = TRUE;
					}
				}
				break;
			case NGN_USHORT:
				{
					unsigned short ush =0;
					if (pPage->GetAField(ush, (FID_DEFINITION) nKey))
					{
						MsgObj->AddFid(nKey, (unsigned short*) &ush);// uShort	
						bReturnValue = TRUE;
					}
				}
				break;
			case NGN_LONG:
			case NGN_KMULTIPLE:
			case NGN_10KMULTIPLE:
				{
					long l = 0;
					if (pPage->GetAField(l, (FID_DEFINITION) nKey))
					{
						MsgObj->AddFid(nKey, (long*) &l);			// long	
						bReturnValue = TRUE;
					}
				}
				break;
			case NGN_ULONG:
				{
					unsigned long ul = 0;
					if (pPage->GetAField(ul, (FID_DEFINITION) nKey))
					{
						MsgObj->AddFid(nKey, (unsigned long*) &ul);	// ulong			
						bReturnValue = TRUE;
					}
				}
				break;
			case NGN_FLOAT:
			case NGN_UFLOAT:
				{
					float f = 0;
					if (pPage->GetAField(f, (FID_DEFINITION) nKey))
					{
						MsgObj->AddFid(nKey, (float*) &f);			// float
						bReturnValue = TRUE;
					}
				}
				break;
			case NGN_DOUBLE:
			case NGN_UDOUBLE:
				{
					double d = 0;
					if (pPage->GetAField(d, (FID_DEFINITION) nKey))
					{
						MsgObj->AddFid(nKey, (double*) &d);			// double
						bReturnValue = TRUE;
					}
				}
				break;
			case NGN_CHARARRAY:
				{
					CharArray* CA=NULL;
					long iLineNo = -2;
					if (pPage->GetAField(&CA, (FID_DEFINITION) nKey, &iLineNo))
					{
						MsgObj->AddFid(nKey, CA);						// CharArray
						bReturnValue = TRUE;
					}
				}
				break;
			case NGN_MCHARARRAY:
				{
					if (lIndex == -1)
					{
						long iMaxSizeOfFields = -1;
						CharArray* CA=NULL;
						MCharArray mca;
						if (pPage->GetAField(&CA, (FID_DEFINITION) nKey, &iMaxSizeOfFields))
						{	
							byte counter = 0;
							for (long i = 0; i<=iMaxSizeOfFields; i++)
							{
								if (pPage->GetAField(&CA, (FID_DEFINITION) nKey, &i))
									if (CA !=NULL)
										if (CA->GetLen() > 0)
										{
											counter = (byte)i;
											mca.AddLine(counter, CA);
										}
							}
							BYTE bytTL = iMaxSizeOfFields+1;
							mca.SetTL(bytTL);
							MsgObj->AddFid(nKey, &mca);
							bReturnValue = TRUE;
							lIndex = iMaxSizeOfFields;
						}
					}
					else
					{
						long iMaxSizeOfFields = -1;
						CharArray* CA=NULL;
						MCharArray mca;
						if (pPage->GetAField(&CA, (FID_DEFINITION) nKey, &iMaxSizeOfFields))
						{
							if (pPage->GetAField(&CA, (FID_DEFINITION) nKey, &lIndex))
								if (CA !=NULL)
									if (CA->GetLen() > 0)
									{
										mca.AddLine((byte)lIndex, CA);
										BYTE bytTL = iMaxSizeOfFields+1;
										mca.SetTL(bytTL);
										MsgObj->AddFid(nKey, &mca);
										bReturnValue = TRUE;
									}
						}
					}
				}
				break;
			case NGN_BDATEYEAR:
				{
					BYTE* bytTempContent=NULL;
					bool bDel = false;
					if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
					{
						BDateYear dy(bytTempContent);						
						MsgObj->AddFid(nKey, &dy);
						bReturnValue = TRUE;
						if(bDel) delete bytTempContent;
					}
				}
				break;
			case NGN_BDATEMONTH:
				{
					BYTE* bytTempContent=NULL;
					bool bDel = false;
					if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
					{
						BDateMonth m(bytTempContent);
						MsgObj->AddFid(nKey, &m);
						bReturnValue = TRUE;
						if(bDel) delete bytTempContent;
					}
				}
				break;
			case NGN_BTIMEMIN:
				{
					BYTE* bytTempContent=NULL;
					bool bDel = false;
					if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
					{
						BTimeMin tm(bytTempContent);
						MsgObj->AddFid(nKey, &tm);
						bReturnValue = TRUE;
						if(bDel) delete bytTempContent;
					}
				}
				break;
			case NGN_BTIMESEC:
				{
					BYTE* bytTempContent=NULL;
					bool bDel = false;
					if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
					{
						BTimeSec s(bytTempContent);
						MsgObj->AddFid(nKey, &s);
						bReturnValue = TRUE;
						if(bDel) delete bytTempContent;
					}
				}
				break;
			case NGN_BTIMERANGES:
				{
					BYTE* bytTempContent=NULL;
					bool bDel = false;
					if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
					{
						BTimeRanges tr(bytTempContent);						
						MsgObj->AddFid(nKey, &tr);
						bReturnValue = TRUE;
						if(bDel) delete bytTempContent;
					}
				}
				break;
			case NGN_TRAN:
				{
					BYTE* bytTempContent=NULL;
					bool bDel = false;
					if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
					{
						Tran tran(bytTempContent);
						MsgObj->AddFid(nKey, &tran);
						bReturnValue = TRUE;
						if(bDel) delete bytTempContent;
					}
				}
				break;
			case NGN_SHRORDLST:
				{
					BYTE* bytTempContent=NULL;
					bool bDel = false;
					if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
					{
						ShrOrdLst sol(bytTempContent);
						MsgObj->AddFid(nKey, &sol);
						bReturnValue = TRUE;
						if(bDel) delete bytTempContent;
					}
				}
				break;
			case NGN_PRCVOL:
				{
					BYTE* bytTempContent=NULL;
					bool bDel = false;
					if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
					{
						PrcVol prc(bytTempContent);
						MsgObj->AddFid(nKey, &prc);
						bReturnValue = TRUE;
						if(bDel) delete bytTempContent;
					}
				}
				break;
			case NGN_BKRQUE:
				{
					BYTE* bytTempContent=NULL;
					bool bDel = false;
					if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
					{
						BkrQue bkrque(bytTempContent);
						MsgObj->AddFid(nKey, &bkrque);
						bReturnValue = TRUE;
						if(bDel) delete bytTempContent;
					}
				}
				break;
			case NGN_LINK:
				{
					BYTE* bytTempContent=NULL;
					bool bDel = false;
					if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
					{
						Link link(bytTempContent);
						MsgObj->AddFid(nKey, &link);
						bReturnValue = TRUE;
						if(bDel) delete bytTempContent;
					}
				}
				break;
			case NGN_WLINK:
				{
					BYTE* bytTempContent=NULL;
					bool bDel = false;
					if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
					{
						WLink wlink(bytTempContent);
						MsgObj->AddFid(nKey, &wlink);
						bReturnValue = TRUE;
						if(bDel) delete bytTempContent;
					}
				}
				break;
			case NGN_YLABEL:
				{
					BYTE* bytTempContent=NULL;
					bool bDel = false;
					if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
					{
						YLabel ylabel(bytTempContent);
						MsgObj->AddFid(nKey, &ylabel);
						bReturnValue = TRUE;
						if(bDel) delete bytTempContent;
					}
				}
				break;
			case NGN_XMTCLABEL:
				{
					BYTE* bytTempContent=NULL;
					bool bDel = false;
					if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
					{
						XMtcLabel xmtcLbl(bytTempContent);
						MsgObj->AddFid(nKey, &xmtcLbl);
						bReturnValue = TRUE;
						if(bDel) delete bytTempContent;
					}
				}
				break;
			case NGN_XDTCLABEL:
				{
					BYTE* bytTempContent=NULL;
					bool bDel = false;
							if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
					{
						XDtcLabel xdtcLbl(bytTempContent);
						MsgObj->AddFid(nKey, &xdtcLbl);
						bReturnValue = TRUE;
						if(bDel) delete bytTempContent;
					}
				}
				break;
			case NGN_MTCPOINTS:
				{
					BYTE* bytTempContent=NULL;
					bool bDel = false;
							if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
					{
						MtcPoints mtcPts(bytTempContent);
						MsgObj->AddFid(nKey, &mtcPts);
						bReturnValue = TRUE;
						if(bDel) delete bytTempContent;
					}
				}
				break;
			case NGN_MTCVAL:
				{
					BYTE* bytTempContent=NULL;
					bool bDel = false;
					if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
					{
						MtcVal mtcval(bytTempContent);
						MsgObj->AddFid(nKey, &mtcval);
						bReturnValue = TRUE;
						if(bDel) delete bytTempContent;
					}
				}
				break;
			case NGN_DTCPOINTS:
				{
					BYTE* bytTempContent=NULL;
					bool bDel = false;
					if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
					{
						DtcPoints dtcPts(bytTempContent);
						MsgObj->AddFid(nKey, &dtcPts);
						bReturnValue = TRUE;
						if(bDel) delete bytTempContent;
					}
				}
				break;
			case NGN_HISVAL:
				{
					BYTE* bytTempContent=NULL;
					bool bDel = false;
					if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
					{
						HisVal hisval(bytTempContent);
						MsgObj->AddFid(nKey, &hisval);
						bReturnValue = TRUE;
						if(bDel) delete bytTempContent;
					}
				}
				break;
			case NGN_FIELDLIST:
				{
					BYTE* bytTempContent=NULL;
					bool bDel = false;
					if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
					{
						FieldList fldlst(bytTempContent);
						MsgObj->AddFid(nKey, &fldlst);
						bReturnValue = TRUE;
						if(bDel) delete bytTempContent;
					}
				}
				break;
			case NGN_FIELDLABEL:
				{
					BYTE* bytTempContent=NULL;
					bool bDel = false;
					if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
					{
						FieldLabel fldlbl(bytTempContent);
						MsgObj->AddFid(nKey, &fldlbl);
						bReturnValue = TRUE;
						if(bDel) delete bytTempContent;
					}
				}
				break;
			case NGN_INFOMSG:
				{
					BYTE* bytTempContent=NULL;
					bool bDel = false;
					if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
					{
						InfoMsg infomsg(bytTempContent);
						MsgObj->AddFid(nKey, &infomsg);
						bReturnValue = TRUE;
						if(bDel) delete bytTempContent;
					}
				}
				break;
			case NGN_HLINK:
				{
					BYTE* bytTempContent=NULL;
					bool bDel = false;
					if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
					{
						HLink hlink(bytTempContent);
						MsgObj->AddFid(nKey, &hlink);
						bReturnValue = TRUE;
						if(bDel) delete bytTempContent;
					}
				}
				break;
			case NGN_HEADLINE:
				{
					BYTE* bytTempContent=NULL;
					if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, &lIndex))
					{
						HeadLine headline(bytTempContent);
						MsgObj->AddFid(nKey, &headline);
						bReturnValue = TRUE;
					}
				}
				break;
			case NGN_SPRTBL:
				{
					BYTE* bytTempContent=NULL;
					bool bDel = false;
					if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
					{
						SprTbl sprtbl(bytTempContent);
						MsgObj->AddFid(nKey, &sprtbl);
						bReturnValue = TRUE;
						if(bDel) delete bytTempContent;
					}
				}
				break;
/*			case NGN_QUOTELIST:
				{
					BYTE* bytTempContent=NULL;
					QuoteList qlTempOutput;
					if (pPage->GetAField(&qlTempOutput, (FID_DEFINITION) nKey))
					{
						QuoteList qlOutput(qlTempOutput.GetData());
						MsgObj->AddFid(nKey, &qlOutput);
						bReturnValue = TRUE;
					}
				}
				break;
*/			case NGN_MTCTICKS:
				{
					BYTE* bytTempContent=NULL;
					bool bDel = false;
					if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
					{
						MTCTicks mtcticks;
						mtcticks.InitByDoubleByteStream(bytTempContent);
						MsgObj->AddFid(nKey, &mtcticks);
						bReturnValue = TRUE;
						if(bDel) delete bytTempContent;
					}
				}
				break;
			case NGN_MTCV:
				{
					BYTE* bytTempContent=NULL;
					bool bDel = false;
					if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
					{
						MTCV mtcv;
						mtcv.InitByDoubleByteStream(bytTempContent);
						MsgObj->AddFid(nKey, &mtcv);
						bReturnValue = TRUE;
						if(bDel) delete bytTempContent;
					}
				}
				break;
			case NGN_YHEADLINE:
				{
					BYTE* bytTempContent=NULL;
					if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, &lIndex))
					{
						YHeadLine yheadline(bytTempContent);
						MsgObj->AddFid(nKey, &yheadline);
						bReturnValue = TRUE;
					}
				}
				break;
			case NGN_OBKRQUE:
				{
					BYTE* bytTempContent=NULL;
					bool bDel = false;
					if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
					{
						OBkrQue obkrque(bytTempContent);
						MsgObj->AddFid(nKey, &obkrque);
						bReturnValue = TRUE;
						if(bDel) delete bytTempContent;
					}
				}
				break;
			case NGN_BQUEVOSN:
				{
					BYTE* bytTempContent=NULL;
					bool bDel = false;
					if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
					{
						BQueVOSN bquevosn(bytTempContent);
						MsgObj->AddFid(nKey, &bquevosn);
						bReturnValue = TRUE;
						if(bDel) delete bytTempContent;
					}
				}
				break;			
			case NGN_MTCPTS:
				{
					BYTE* bytTempContent=NULL;
					bool bDel = false;
					if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
					{
						MTCPTS mtcPts(bytTempContent);
						MsgObj->AddFid(nKey, &mtcPts);
						bReturnValue = TRUE;
						if(bDel) delete bytTempContent;
					}
				}
				break;
			case NGN_MTCF:
				{
					BYTE* bytTempContent=NULL;
					bool bDel = false;
							if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
					{
						MTCF mtcf(bytTempContent);
						MsgObj->AddFid(nKey, &mtcf);
						bReturnValue = TRUE;
						if(bDel) delete bytTempContent;
					}
				}
				break;
			case NGN_PRCVOLWSEQ:
				{
					BYTE* bytTempContent=NULL;
					bool bDel = false;
							if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
					{
						PrcVolwSeq prc(bytTempContent);
						MsgObj->AddFid(nKey, &prc);
						bReturnValue = TRUE;
						if(bDel) delete bytTempContent;
					}
				}
				break;
			case NGN_TRAN_D:
				{
					BYTE* bytTempContent=NULL;
					bool bDel = false;
					if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
					{
						TranD tranD(bytTempContent);
						MsgObj->AddFid(nKey, &tranD);
						bReturnValue = TRUE;
						if(bDel) delete bytTempContent;
					}
				}
				break;
			default:
				// ERROR MESSAGE
				break;
		}
		MsgObj->SetItemNo(lPageNo);
		copOutput = new MsgObject(MsgObj->GetMessage());
		delete MsgObj;
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return bReturnValue;
}
// -------------------------------------------------------------------------
// Get MCharArray TL
BOOL CDCache::GetMCharArrayTL(long lPageNo, FID_DEFINITION enumFieldID, long& NoOfElement)
{
	BOOL bReturnValue = FALSE;
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		if (pPage->GetMCharArrayTL((FID_DEFINITION) enumFieldID, NoOfElement))
			bReturnValue = TRUE;
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return bReturnValue;
}
// -------------------------------------------------------------------------
// Get Multi Field
BOOL CDCache::GetMultiField(long lPageNo, CWordArray& cwaFIDList, COP_ITEM& copReturnPage)
{
	BOOL bReturnValue = FALSE;

	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		MsgObject* MsgObj = new MsgObject();
		int		nKey;
		fidDatabase fDB;
		int iTotalFID = cwaFIDList.GetUpperBound();
		for(int iCounter=0;iCounter<=iTotalFID;iCounter++)
		{
			nKey = cwaFIDList.GetAt(iCounter);
			switch (fDB.GetFidType(nKey))
			{
				case NGN_CHAR:
					{
						unsigned char ch;
						if (pPage->GetAField((unsigned char)ch, (FID_DEFINITION) nKey))
						{
							MsgObj->AddFid(nKey, (unsigned char*) &ch);	// char	
							bReturnValue = TRUE;
						}
					}
					break;
				//case NGN_UNICODE:
				//	{
				//		WCHAR uni;
				//		if (pPage->GetAField((unsigned short)uni, (FID_DEFINITION) nKey))
				//		{
				//			MsgObj->AddFid(nKey, &uni);					// unicode 
				//			bReturnValue = TRUE;
				//		}
				//	}
				//	break;
				case NGN_SHORT:
					{
						short sh = 0;
						if (pPage->GetAField(sh, (FID_DEFINITION) nKey))
						{
							MsgObj->AddFid(nKey, (short*) &sh);			// Short	
							bReturnValue = TRUE;
						}
					}
					break;
				case NGN_USHORT:
					{
						unsigned short ush = 0;
						if (pPage->GetAField(ush, (FID_DEFINITION) nKey))
						{
							MsgObj->AddFid(nKey, (unsigned short*) &ush);// uShort	
							bReturnValue = TRUE;
						}
					}
					break;
				case NGN_LONG:
				case NGN_KMULTIPLE:
				case NGN_10KMULTIPLE:
					{
						long l=0;
						if (pPage->GetAField(l, (FID_DEFINITION) nKey))
						{
							MsgObj->AddFid(nKey, (long*) &l);			// long	
							bReturnValue = TRUE;
						}
					}
					break;
				case NGN_ULONG:
					{
						unsigned long ul =0;
						if (pPage->GetAField(ul, (FID_DEFINITION) nKey))
						{					
							MsgObj->AddFid(nKey, (unsigned long*) &ul);	// ulong			
							bReturnValue = TRUE;
						}
					}
					break;
				case NGN_FLOAT:
				case NGN_UFLOAT:
					{
						float f=0;
						if (pPage->GetAField(f, (FID_DEFINITION) nKey))
						{
							MsgObj->AddFid(nKey, (float*) &f);			// float
							bReturnValue = TRUE;
						}
					}
					break;
				case NGN_DOUBLE:
				case NGN_UDOUBLE:
					{
						double d=0;
						if (pPage->GetAField(d, (FID_DEFINITION) nKey))
						{
							MsgObj->AddFid(nKey, (double*) &d);			// double
							bReturnValue = TRUE;
						}
					}
					break;
				case NGN_CHARARRAY:
					{
						CharArray* CA=NULL;
						long iLineNo = -2;
						if (pPage->GetAField(&CA, (FID_DEFINITION) nKey, &iLineNo))
						{
							MsgObj->AddFid(nKey, CA);					// CharArray
							bReturnValue = TRUE;
						}
					}
					break;
				case NGN_MCHARARRAY:
					{
						long iMaxSizeOfFields = -1;
						CharArray* CA=NULL;
						MCharArray mca;
						if (pPage->GetAField(&CA, (FID_DEFINITION) nKey, &iMaxSizeOfFields))
						{	
							byte counter = 0;
							for (long i = 0; i<=iMaxSizeOfFields; i++)
							{
								if (pPage->GetAField(&CA, (FID_DEFINITION) nKey, &i))
									if (CA !=NULL)
										if (CA->GetLen() > 0)
										{
											counter = (byte)i;
											mca.AddLine(counter, CA);
										}
							}
							BYTE bytTL = iMaxSizeOfFields+1;
							mca.SetTL(bytTL);
							MsgObj->AddFid(nKey, &mca);
							bReturnValue = TRUE;
						}
					}
					break;
				case NGN_BDATEYEAR:
					{
						BYTE* bytTempContent;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							BDateYear dy(bytTempContent);						
							MsgObj->AddFid(nKey, &dy);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_BDATEMONTH:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							BDateMonth m(bytTempContent);
							MsgObj->AddFid(nKey, &m);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_BTIMEMIN:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							BTimeMin tm(bytTempContent);
							MsgObj->AddFid(nKey, &tm);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_BTIMESEC:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							BTimeSec s(bytTempContent);
							MsgObj->AddFid(nKey, &s);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_BTIMERANGES:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							BTimeRanges tr(bytTempContent);						
							MsgObj->AddFid(nKey, &tr);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_TRAN:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							Tran tran(bytTempContent);
							MsgObj->AddFid(nKey, &tran);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_SHRORDLST:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							ShrOrdLst sol(bytTempContent);
							MsgObj->AddFid(nKey, &sol);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_PRCVOL:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							PrcVol prc(bytTempContent);
							MsgObj->AddFid(nKey, &prc);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_BKRQUE:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							BkrQue bkrque(bytTempContent);
							MsgObj->AddFid(nKey, &bkrque);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_LINK:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							Link link(bytTempContent);
							MsgObj->AddFid(nKey, &link);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_WLINK:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							WLink wlink(bytTempContent);
							MsgObj->AddFid(nKey, &wlink);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_YLABEL:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							YLabel ylabel(bytTempContent);
							MsgObj->AddFid(nKey, &ylabel);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_XMTCLABEL:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							XMtcLabel xmtcLbl(bytTempContent);
							MsgObj->AddFid(nKey, &xmtcLbl);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_XDTCLABEL:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							XDtcLabel xdtcLbl(bytTempContent);
							MsgObj->AddFid(nKey, &xdtcLbl);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_MTCPOINTS:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							MtcPoints mtcPts(bytTempContent);
							MsgObj->AddFid(nKey, &mtcPts);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_MTCVAL:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							MtcVal mtcval(bytTempContent);
							MsgObj->AddFid(nKey, &mtcval);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_DTCPOINTS:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							DtcPoints dtcPts(bytTempContent);
							MsgObj->AddFid(nKey, &dtcPts);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_HISVAL:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							HisVal hisval(bytTempContent);
							MsgObj->AddFid(nKey, &hisval);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_FIELDLIST:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							FieldList fldlst(bytTempContent);
							MsgObj->AddFid(nKey, &fldlst);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_FIELDLABEL:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							FieldLabel fldlbl(bytTempContent);
							MsgObj->AddFid(nKey, &fldlbl);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_INFOMSG:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
						 	InfoMsg infomsg(bytTempContent);
							MsgObj->AddFid(nKey, &infomsg);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_HLINK:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
						 	HLink hlink(bytTempContent);
							MsgObj->AddFid(nKey, &hlink);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
				case NGN_HEADLINE:
					break;
				case NGN_SPRTBL:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
						 	SprTbl sprtbl(bytTempContent);
							MsgObj->AddFid(nKey, &sprtbl);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
/*				case NGN_QUOTELIST:
					{
						BYTE* bytTempContent=NULL;
						QuoteList qlOutput;
						if (pPage->GetAField(&qlOutput, (FID_DEFINITION) nKey))
						{
							MsgObj->AddFid(nKey, &qlOutput);
							bReturnValue = TRUE;
						}
					}
					break;
*/				case NGN_MTCTICKS:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							MTCTicks mtcticks;
							mtcticks.InitByDoubleByteStream(bytTempContent);
							MsgObj->AddFid(nKey, &mtcticks);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_MTCV:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							MTCV mtcv;
							mtcv.InitByDoubleByteStream(bytTempContent);
							MsgObj->AddFid(nKey, &mtcv);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_YHEADLINE:
					break;
				case NGN_OBKRQUE:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							OBkrQue obkrque(bytTempContent);
							MsgObj->AddFid(nKey, &obkrque);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_BQUEVOSN:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							BQueVOSN bquevosn(bytTempContent);
							MsgObj->AddFid(nKey, &bquevosn);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_MTCPTS:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							MTCPTS mtcPts(bytTempContent);
							MsgObj->AddFid(nKey, &mtcPts);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_MTCF:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							MTCF mtcf(bytTempContent);
							MsgObj->AddFid(nKey, &mtcf);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_PRCVOLWSEQ:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							PrcVolwSeq prc(bytTempContent);
							MsgObj->AddFid(nKey, &prc);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_TRAN_D:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							TranD tranD(bytTempContent);
							MsgObj->AddFid(nKey, &tranD);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				default:
					//ERROR MESSAGE
					break;
			}
		}
		MsgObj->SetItemNo(lPageNo);
		copReturnPage = new MsgObject(MsgObj->GetMessage());
		delete MsgObj;
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return bReturnValue;
}

int  CDCache::GetMultiField(long lPageNo, CWordArray& cwaFIDList,  CWordArray& cwOutFIDList, COP_ITEM& copReturnPage)
{
	int  iReturnNoFids = 0; 

	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		MsgObject* MsgObj = new MsgObject();
		int		nKey;
		fidDatabase fDB;
		int iTotalFID = cwaFIDList.GetUpperBound();
		
		for(int iCounter=0;iCounter<=iTotalFID;iCounter++)
		{
			iReturnNoFids ++;	

			nKey = cwaFIDList.GetAt(iCounter);
			switch (fDB.GetFidType(nKey))
			{
				case NGN_CHAR:
					{
						unsigned char ch;
						if (pPage->GetAField((unsigned char)ch, (FID_DEFINITION) nKey))
						{
							MsgObj->AddFid(nKey, (unsigned char*) &ch);	// char	
					
						}else
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					}
					break;
				//case NGN_UNICODE:
				//	{
				//		WCHAR uni;
				//		if (pPage->GetAField((unsigned short)uni, (FID_DEFINITION) nKey))
				//		{
				//			MsgObj->AddFid(nKey, &uni);					// unicode 
				//		
				//		}else
				//		{
				//			cwOutFIDList.Add(nKey);
				//			iReturnNoFids--;
				//		}
				//	}
				//	break;
				case NGN_SHORT:
					{
						short sh = 0;
						if (pPage->GetAField(sh, (FID_DEFINITION) nKey))
						{
							MsgObj->AddFid(nKey, (short*) &sh);			// Short	
						
						}else
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					}
					break;
				case NGN_USHORT:
					{
						unsigned short ush = 0;
						if (pPage->GetAField(ush, (FID_DEFINITION) nKey))
						{
							MsgObj->AddFid(nKey, (unsigned short*) &ush);// uShort	
							
						}else
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					}
					break;
				case NGN_LONG:
				case NGN_KMULTIPLE:
				case NGN_10KMULTIPLE:
					{
						long l=0;
						if (pPage->GetAField(l, (FID_DEFINITION) nKey))
						{
							MsgObj->AddFid(nKey, (long*) &l);			// long	
						
						}else
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					}
					break;
				case NGN_ULONG:
					{
						unsigned long ul =0;
						if (pPage->GetAField(ul, (FID_DEFINITION) nKey))
						{					
							MsgObj->AddFid(nKey, (unsigned long*) &ul);	// ulong			
						
						}else
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					}
					break;
				case NGN_FLOAT:
				case NGN_UFLOAT:
					{
						float f=0;
						if (pPage->GetAField(f, (FID_DEFINITION) nKey))
						{
							MsgObj->AddFid(nKey, (float*) &f);			// float
							
						}else
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					}
					break;
				case NGN_DOUBLE:
				case NGN_UDOUBLE:
					{
						double d=0;
						if (pPage->GetAField(d, (FID_DEFINITION) nKey))
						{
							MsgObj->AddFid(nKey, (double*) &d);			// double
						
						}else
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					}
					break;
				case NGN_CHARARRAY:
					{
						CharArray* CA=NULL;
						long iLineNo = -2;
						if (pPage->GetAField(&CA, (FID_DEFINITION) nKey, &iLineNo))
						{
							MsgObj->AddFid(nKey, CA);					// CharArray
						
						}else
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					}
					break;
				case NGN_MCHARARRAY:
					{
						long iMaxSizeOfFields = -1;
						CharArray* CA=NULL;
						MCharArray mca;
						if (pPage->GetAField(&CA, (FID_DEFINITION) nKey, &iMaxSizeOfFields))
						{	
							byte counter = 0;
							for (long i = 0; i<=iMaxSizeOfFields; i++)
							{
								if (pPage->GetAField(&CA, (FID_DEFINITION) nKey, &i))
									if (CA !=NULL)
										if (CA->GetLen() > 0)
										{
											counter = (byte)i;
											mca.AddLine(counter, CA);
										}
							}
							BYTE bytTL = iMaxSizeOfFields+1;
							mca.SetTL(bytTL);
							MsgObj->AddFid(nKey, &mca);
						
						}else
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					}
					break;
				case NGN_BDATEYEAR:
					{
						BYTE* bytTempContent;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							BDateYear dy(bytTempContent);						
							MsgObj->AddFid(nKey, &dy);
							if(bDel) delete bytTempContent;
						
						}else
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					}
					break;
				case NGN_BDATEMONTH:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							BDateMonth m(bytTempContent);
							MsgObj->AddFid(nKey, &m);
							if(bDel) delete bytTempContent;						
						}else
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					}
					break;
				case NGN_BTIMEMIN:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							BTimeMin tm(bytTempContent);
							MsgObj->AddFid(nKey, &tm);
							if(bDel) delete bytTempContent;
						}else
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					}
					break;
				case NGN_BTIMESEC:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							BTimeSec s(bytTempContent);
							MsgObj->AddFid(nKey, &s);
							if(bDel) delete bytTempContent;
						}else
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					}
					break;
				case NGN_BTIMERANGES:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							BTimeRanges tr(bytTempContent);						
							MsgObj->AddFid(nKey, &tr);
							if(bDel) delete bytTempContent;
						}else
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					}
					break;
				case NGN_TRAN:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							Tran tran(bytTempContent);
							MsgObj->AddFid(nKey, &tran);
							if(bDel) delete bytTempContent;
						}else
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					}
					break;
				case NGN_SHRORDLST:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							ShrOrdLst sol(bytTempContent);
							MsgObj->AddFid(nKey, &sol);
							if(bDel) delete bytTempContent;
						}else
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					}
					break;
				case NGN_PRCVOL:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							PrcVol prc(bytTempContent);
							MsgObj->AddFid(nKey, &prc);
							if(bDel) delete bytTempContent;
						}else
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					}
					break;
				case NGN_BKRQUE:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							BkrQue bkrque(bytTempContent);
							MsgObj->AddFid(nKey, &bkrque);
							if(bDel) delete bytTempContent;
						}else
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					}
					break;
				case NGN_LINK:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							Link link(bytTempContent);
							MsgObj->AddFid(nKey, &link);
							if(bDel) delete bytTempContent;
						}else
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					}
					break;
				case NGN_WLINK:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							WLink wlink(bytTempContent);
							MsgObj->AddFid(nKey, &wlink);
							if(bDel) delete bytTempContent;
						}else
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					}
					break;
				case NGN_YLABEL:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							YLabel ylabel(bytTempContent);
							MsgObj->AddFid(nKey, &ylabel);
							if(bDel) delete bytTempContent;
						}else
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					}
					break;
				case NGN_XMTCLABEL:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							XMtcLabel xmtcLbl(bytTempContent);
							MsgObj->AddFid(nKey, &xmtcLbl);
							if(bDel) delete bytTempContent;
						}else
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					}
					break;
				case NGN_XDTCLABEL:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							XDtcLabel xdtcLbl(bytTempContent);
							MsgObj->AddFid(nKey, &xdtcLbl);
							if(bDel) delete bytTempContent;
						}else
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					}
					break;
				case NGN_MTCPOINTS:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							MtcPoints mtcPts(bytTempContent);
							MsgObj->AddFid(nKey, &mtcPts);
							if(bDel) delete bytTempContent;
						}else
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					}
					break;
				case NGN_MTCVAL:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							MtcVal mtcval(bytTempContent);
							MsgObj->AddFid(nKey, &mtcval);
							if(bDel) delete bytTempContent;
						}else
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					}
					break;
				case NGN_DTCPOINTS:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							DtcPoints dtcPts(bytTempContent);
							MsgObj->AddFid(nKey, &dtcPts);
							if(bDel) delete bytTempContent;
						}else
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					}
					break;
				case NGN_HISVAL:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							HisVal hisval(bytTempContent);
							MsgObj->AddFid(nKey, &hisval);
							if(bDel) delete bytTempContent;
						}else
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					}
					break;
				case NGN_FIELDLIST:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							FieldList fldlst(bytTempContent);
							MsgObj->AddFid(nKey, &fldlst);
							if(bDel) delete bytTempContent;
						}else
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					}
					break;
				case NGN_FIELDLABEL:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							FieldLabel fldlbl(bytTempContent);
							MsgObj->AddFid(nKey, &fldlbl);
							if(bDel) delete bytTempContent;
						}else
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					}
					break;
				case NGN_INFOMSG:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
						 	InfoMsg infomsg(bytTempContent);
							MsgObj->AddFid(nKey, &infomsg);
							if(bDel) delete bytTempContent;
						}else
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					}
					break;
				case NGN_HLINK:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
						 	HLink hlink(bytTempContent);
							MsgObj->AddFid(nKey, &hlink);
							if(bDel) delete bytTempContent;
						}else
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					}
				case NGN_HEADLINE:
					{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					break;
				case NGN_SPRTBL:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
						 	SprTbl sprtbl(bytTempContent);
							MsgObj->AddFid(nKey, &sprtbl);
							if(bDel) delete bytTempContent;
						}else
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					}
					break;
/*				case NGN_QUOTELIST:
					{
						BYTE* bytTempContent=NULL;
						QuoteList qlOutput;
						if (pPage->GetAField(&qlOutput, (FID_DEFINITION) nKey))
						{
							MsgObj->AddFid(nKey, &qlOutput);
						
						}else
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					}
					break;
*/				case NGN_MTCTICKS:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							MTCTicks mtcticks;
							mtcticks.InitByDoubleByteStream(bytTempContent);
							MsgObj->AddFid(nKey, &mtcticks);
							if(bDel) delete bytTempContent;
						}else
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					}
					break;
				case NGN_MTCV:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							MTCV mtcv;
							mtcv.InitByDoubleByteStream(bytTempContent);
							MsgObj->AddFid(nKey, &mtcv);
							if(bDel) delete bytTempContent;
						}else
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					}
					break;
				case NGN_YHEADLINE:
					{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					break;
				case NGN_OBKRQUE:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							OBkrQue obkrque(bytTempContent);
							MsgObj->AddFid(nKey, &obkrque);
							if(bDel) delete bytTempContent;
						}else
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					}
					break;
				case NGN_BQUEVOSN:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							BQueVOSN bquevosn(bytTempContent);
							MsgObj->AddFid(nKey, &bquevosn);
							if(bDel) delete bytTempContent;
						}else
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					}
					break;
				case NGN_MTCPTS:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							MTCPTS mtcPts(bytTempContent);
							MsgObj->AddFid(nKey, &mtcPts);
							if(bDel) delete bytTempContent;
						}else
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					}
					break;
				case NGN_MTCF:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							MTCF mtcf(bytTempContent);
							MsgObj->AddFid(nKey, &mtcf);
							if(bDel) delete bytTempContent;	
						}else
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					}
					break;
				case NGN_PRCVOLWSEQ:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							PrcVolwSeq prc(bytTempContent);
							MsgObj->AddFid(nKey, &prc);
							if(bDel) delete bytTempContent;
						}else
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					}
					break;
				case NGN_TRAN_D:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							TranD tranD(bytTempContent);
							MsgObj->AddFid(nKey, &tranD);
							if(bDel) delete bytTempContent;
						}else
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					}
					break;
				default:
					//ERROR MESSAGE
						{
							cwOutFIDList.Add(nKey);
							iReturnNoFids--;
						}
					break;
			}
		}
		MsgObj->SetItemNo(lPageNo);
		copReturnPage = new MsgObject(MsgObj->GetMessage());
		delete MsgObj;
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return iReturnNoFids;
}

// -------------------------------------------------------------------------
// Get the FID list of the specific page 
BOOL CDCache::GetFIDList(long lPageNo, CDWordArray& cdwaFIDList)
{
	BOOL bReturnValue = FALSE;
	
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		pPage->FirstField();
		int nKey;

		while ((nKey=pPage->GetNextField())!=FID_END)
		{
			cdwaFIDList.Add(nKey);
			bReturnValue = TRUE;
		}
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return bReturnValue;
}


// -------------------------------------------------------------------------
// Get all the FID of the specific page except Headline
BOOL CDCache::GetAPage(long lPageNo, COP_ITEM& pReturnPageCop)
{
	BOOL bReturnValue = FALSE;
	
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		MsgObject* MsgObj = new MsgObject();
		pPage->FirstField();
		int		nKey;
		fidDatabase fDB;

		while ((nKey=pPage->GetNextField())!=FID_END)
		{
			switch (fDB.GetFidType(nKey))
			{
				case NGN_CHAR:
					{
						unsigned char ch;
						if (pPage->GetAField((unsigned char)ch, (FID_DEFINITION) nKey))
						{
							MsgObj->AddFid(nKey, (unsigned char*) &ch);	// char	
							bReturnValue = TRUE;
						}
					}
					break;
				//case NGN_UNICODE:
				//	{
				//		WCHAR uni;
				//		if (pPage->GetAField((unsigned short)uni, (FID_DEFINITION) nKey))
				//		{
				//			MsgObj->AddFid(nKey, &uni);// unicode 
				//			bReturnValue = TRUE;
				//		}
				//	}
				//	break;
				case NGN_SHORT:
					{
						short sh = 0;
						if (pPage->GetAField(sh, (FID_DEFINITION) nKey))
						{
							MsgObj->AddFid(nKey, (short*) &sh);			// Short	
							bReturnValue = TRUE;
						}
					}
					break;
				case NGN_USHORT:
					{
						unsigned short ush = 0;
						if (pPage->GetAField(ush, (FID_DEFINITION) nKey))
						{
							MsgObj->AddFid(nKey, (unsigned short*) &ush);// uShort	
							bReturnValue = TRUE;
						}
					}
					break;
				case NGN_LONG:
				case NGN_KMULTIPLE:
				case NGN_10KMULTIPLE:
					{
						long l=0;
						if (pPage->GetAField(l, (FID_DEFINITION) nKey))
						{
							MsgObj->AddFid(nKey, (long*) &l);			// long	
							bReturnValue = TRUE;
						}
					}
					break;
				case NGN_ULONG:
					{
						unsigned long ul =0;
						if (pPage->GetAField(ul, (FID_DEFINITION) nKey))
						{					
							MsgObj->AddFid(nKey, (unsigned long*) &ul);	// ulong			
							bReturnValue = TRUE;
						}
					}
					break;
				case NGN_FLOAT:
				case NGN_UFLOAT:
					{
						float f=0;
						if (pPage->GetAField(f, (FID_DEFINITION) nKey))
						{
							MsgObj->AddFid(nKey, (float*) &f);			// float
							bReturnValue = TRUE;
						}
					}
					break;
				case NGN_DOUBLE:
				case NGN_UDOUBLE:
					{
						double d=0;
						if (pPage->GetAField(d, (FID_DEFINITION) nKey))
						{
							MsgObj->AddFid(nKey, (double*) &d);			// double
							bReturnValue = TRUE;
						}
					}
					break;
				case NGN_CHARARRAY:
					{
						CharArray* CA=NULL;
						long iLineNo = -2;
						if (pPage->GetAField(&CA, (FID_DEFINITION) nKey, &iLineNo))
						{
							MsgObj->AddFid(nKey, CA);						// CharArray
							bReturnValue = TRUE;
						}
					}
					break;
				case NGN_MCHARARRAY:
					{
						long iMaxSizeOfFields = -1;
						CharArray* CA=NULL;
						MCharArray mca;
						if (pPage->GetAField(&CA, (FID_DEFINITION) nKey, &iMaxSizeOfFields))
						{	
							byte counter = 0;
							for (long i = 0; i<=iMaxSizeOfFields; i++)
							{
								if (pPage->GetAField(&CA, (FID_DEFINITION) nKey, &i))
									if (CA !=NULL)
										if (CA->GetLen() > 0)
										{
											counter = (byte)i;
											mca.AddLine(counter, CA);
										}
							}
							BYTE bytTL = iMaxSizeOfFields+1;
							mca.SetTL(bytTL);
							MsgObj->AddFid(nKey, &mca);
							bReturnValue = TRUE;
						}
					}
					break;
				case NGN_BDATEYEAR:
					{
						BYTE* bytTempContent;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							BDateYear dy(bytTempContent);						
							MsgObj->AddFid(nKey, &dy);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_BDATEMONTH:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							BDateMonth m(bytTempContent);
							MsgObj->AddFid(nKey, &m);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_BTIMEMIN:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							BTimeMin tm(bytTempContent);
							MsgObj->AddFid(nKey, &tm);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_BTIMESEC:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							BTimeSec s(bytTempContent);
							MsgObj->AddFid(nKey, &s);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_BTIMERANGES:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							BTimeRanges tr(bytTempContent);						
							MsgObj->AddFid(nKey, &tr);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_TRAN:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							Tran tran(bytTempContent);
							MsgObj->AddFid(nKey, &tran);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_SHRORDLST:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							ShrOrdLst sol(bytTempContent);
							MsgObj->AddFid(nKey, &sol);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_PRCVOL:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							PrcVol prc(bytTempContent);
							MsgObj->AddFid(nKey, &prc);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_BKRQUE:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							BkrQue bkrque(bytTempContent);
							MsgObj->AddFid(nKey, &bkrque);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_LINK:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							Link link(bytTempContent);
							MsgObj->AddFid(nKey, &link);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_WLINK:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							WLink wlink(bytTempContent);
							MsgObj->AddFid(nKey, &wlink);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_YLABEL:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							YLabel ylabel(bytTempContent);
							MsgObj->AddFid(nKey, &ylabel);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_XMTCLABEL:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							XMtcLabel xmtcLbl(bytTempContent);
							MsgObj->AddFid(nKey, &xmtcLbl);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_XDTCLABEL:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							XDtcLabel xdtcLbl(bytTempContent);
							MsgObj->AddFid(nKey, &xdtcLbl);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_MTCPOINTS:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							MtcPoints mtcPts(bytTempContent);
							MsgObj->AddFid(nKey, &mtcPts);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_MTCVAL:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							MtcVal mtcval(bytTempContent);
							MsgObj->AddFid(nKey, &mtcval);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_DTCPOINTS:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							DtcPoints dtcPts(bytTempContent);
							MsgObj->AddFid(nKey, &dtcPts);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_HISVAL:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							HisVal hisval(bytTempContent);
							MsgObj->AddFid(nKey, &hisval);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_FIELDLIST:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							FieldList fldlst(bytTempContent);
							MsgObj->AddFid(nKey, &fldlst);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_FIELDLABEL:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							FieldLabel fldlbl(bytTempContent);
							MsgObj->AddFid(nKey, &fldlbl);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_INFOMSG:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
						 	InfoMsg infomsg(bytTempContent);
							MsgObj->AddFid(nKey, &infomsg);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_HLINK:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
						 	HLink hlink(bytTempContent);
							MsgObj->AddFid(nKey, &hlink);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_HEADLINE:
					break;
				case NGN_SPRTBL:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
						 	SprTbl sprtbl(bytTempContent);
							MsgObj->AddFid(nKey, &sprtbl);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
/*				case NGN_QUOTELIST:
					break;
*/				case NGN_MTCTICKS:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							MTCTicks mtcticks;
							mtcticks.InitByDoubleByteStream(bytTempContent);
							MsgObj->AddFid(nKey, &mtcticks);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_MTCV:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							MTCV mtcv;
							mtcv.InitByDoubleByteStream(bytTempContent);
							MsgObj->AddFid(nKey, &mtcv);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_YHEADLINE:
					break;
				case NGN_OBKRQUE:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							OBkrQue obkrque(bytTempContent);
							MsgObj->AddFid(nKey, &obkrque);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_BQUEVOSN:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							BQueVOSN bquevosn(bytTempContent);
							MsgObj->AddFid(nKey, &bquevosn);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_MTCPTS:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							MTCPTS mtcPts(bytTempContent);
							MsgObj->AddFid(nKey, &mtcPts);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_MTCF:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							MTCF mtcf(bytTempContent);
							MsgObj->AddFid(nKey, &mtcf);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_PRCVOLWSEQ:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							PrcVolwSeq prc(bytTempContent);
							MsgObj->AddFid(nKey, &prc);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				case NGN_TRAN_D:
					{
						BYTE* bytTempContent=NULL;
						bool bDel = false;
						if (pPage->GetAField(&bytTempContent, (FID_DEFINITION) nKey, bDel))
						{
							TranD tranD(bytTempContent);
							MsgObj->AddFid(nKey, &tranD);
							bReturnValue = TRUE;
							if(bDel) delete bytTempContent;
						}
					}
					break;
				default:
					//ERROR MESSAGE
					break;
			}
		}
		MsgObj->SetItemNo(lPageNo);
		pReturnPageCop = new MsgObject(MsgObj->GetMessage());
		delete MsgObj;
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return bReturnValue;
}
// -------------------------------------------------------------------------
// Drop A page 
BOOL CDCache::DropAPage(long lPageNo, COP_ITEM copMsg)
{
	BOOL bReturnValue = FALSE;
	CPage *pPage = NULL;
	
	if (copMsg.isNull() || (void*)copMsg==NULL)
	{
		MsgObject* MsgObj = new MsgObject();
		MsgObj->SetMsgType(MGT_DROP);	
		MsgObj->SetItemNo(lPageNo);
		copMsg = new MsgObject(MsgObj->GetMessage());
		delete MsgObj;
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	m_CriticalSection[0].Enter();

	CObject* tempObject = NULL;
	if (!m_mapDropPages.Lookup(lPageNo, (CObject*&)tempObject))
		m_mapDropPages[lPageNo] = tempObject;

	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		CString strRicName = pPage->GetRicName();
		CObject* TempObj = NULL;
		long*	lPhysicalPageNo	= NULL;
		if (m_mapPagesName.Lookup(strRicName.GetBuffer(0), TempObj))
		{
			lPhysicalPageNo	 = (long*) TempObj;
			m_mapPagesName.RemoveKey(pPage->GetRicName().GetBuffer(0));
			delete lPhysicalPageNo;
		}
		
		BYTE bytRecordType=0, bytMarketCode=0, bytSubMarket=0;
		bytRecordType=pPage->GetRecordType();
		bytMarketCode=pPage->GetMarketCode();
		bytSubMarket=pPage->GetSubMarket();
		m_mapPages.RemoveKey(lPageNo);
		delete pPage;

		bReturnValue = TRUE;
		m_CriticalSection[0].Leave();
		m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	
		m_pCallback->DoCallbackFunctions(lPageNo, FID_END, DROP_PAGE, copMsg, bytRecordType, bytMarketCode, bytSubMarket);
		m_pCallback->DoCallbackFunctions(lPageNo, FID_END, DROP_PAGE, copMsg, bytRecordType, bytMarketCode, 255);
		return bReturnValue;
	}
	m_CriticalSection[0].Leave();
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return bReturnValue;
}

// -------------------------------------------------------------------------
// GetRicName 
CString CDCache::GetRicName (long lPageNo)
{
	CString strRicName = "";
	CPage *pPage = NULL;
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();

	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
		strRicName = pPage->GetRicName();
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();

	return strRicName;
}

// -------------------------------------------------------------------------
// Long 
BOOL CDCache::SetAField(long lPageNo, long lInput,
							 FID_DEFINITION enumFieldID, long lIndex)
{
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();

	CPage *pDBPage = NULL;
	if (!m_mapPages.Lookup(lPageNo, (CPage*)pDBPage))
	{
		if ((pDBPage=new CPage) == NULL)
		{
			// Show Alert
			m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
			return(FALSE);
		}
		m_lNoOfPage++;
		pDBPage->SetPageNo(lPageNo);

		m_CriticalSection[0].Enter();
		m_mapPages[lPageNo] = pDBPage;
		m_CriticalSection[0].Leave();
	}

	if (pDBPage->SetAField(lInput, enumFieldID, lIndex))
	{
		BYTE bytRecordType=0, bytMarketCode=0, bytSubMarket=0;
		bytRecordType=pDBPage->GetRecordType();
		bytMarketCode=pDBPage->GetMarketCode();
		bytMarketCode=pDBPage->GetSubMarket();
		m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
		// Trigger the relative callback functions
		MsgObject* MsgObj = new MsgObject();
		MsgObj->AddFid(enumFieldID, (long*) &lInput);		// 	long
		MsgObj->SetItemNo(lPageNo);
		COP_ITEM copMsg = new MsgObject(MsgObj->GetMessage());
		if (MsgObj)
		{
			delete MsgObj;
			MsgObj = NULL;
		}
		m_pCallback->DoCallbackFunctions(lPageNo, enumFieldID, lIndex, copMsg, bytRecordType, bytMarketCode, bytSubMarket);
		return(TRUE);
	}

	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return(FALSE);
}
// -------------------------------------------------------------------------
// unsigned long
BOOL CDCache::SetAField(long lPageNo, unsigned long ulInput,
							 FID_DEFINITION enumFieldID, long lIndex)
{
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();

	CPage *pDBPage = NULL; 
	if (!m_mapPages.Lookup(lPageNo, (CPage*)pDBPage))
	{
		if ((pDBPage=new CPage) == NULL)
		{
			// Show Alert
			m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
			return(FALSE);
		}
		m_lNoOfPage++;
		pDBPage->SetPageNo(lPageNo);
		m_CriticalSection[0].Enter();
		m_mapPages[lPageNo] = pDBPage;
		m_CriticalSection[0].Leave();
	}

	if (pDBPage->SetAField(ulInput, enumFieldID, lIndex))
	{
		// Trigger the relative callback functions
		BYTE bytRecordType=0, bytMarketCode=0, bytSubMarket=0;
		bytRecordType=pDBPage->GetRecordType();
		bytMarketCode=pDBPage->GetMarketCode();
		bytMarketCode=pDBPage->GetSubMarket();
		m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
		MsgObject* MsgObj = new MsgObject();
		MsgObj->AddFid(enumFieldID, (unsigned long*) &ulInput);	//	ulong
		MsgObj->SetItemNo(lPageNo);
		COP_ITEM copMsg = new MsgObject(MsgObj->GetMessage());
		if (MsgObj)
		{
			delete MsgObj;
			MsgObj = NULL;
		}
		m_pCallback->DoCallbackFunctions(lPageNo, enumFieldID, lIndex, copMsg, bytRecordType, bytMarketCode, bytSubMarket);
		return(TRUE);
	}

	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return(FALSE);
}
// -------------------------------------------------------------------------
// short
BOOL CDCache::SetAField(long lPageNo, short nInput,
							 FID_DEFINITION enumFieldID, long lIndex)
{
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();

	CPage *pDBPage = NULL;
	if (!m_mapPages.Lookup(lPageNo, (CPage*)pDBPage))
	{
		if ((pDBPage=new CPage) == NULL)
		{
			// Show Alert
			m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
			return(FALSE);
		}
		m_lNoOfPage++;
		pDBPage->SetPageNo(lPageNo);
		m_CriticalSection[0].Enter();
		m_mapPages[lPageNo] = pDBPage;
		m_CriticalSection[0].Leave();
	}
	if (pDBPage->SetAField(nInput, enumFieldID, lIndex))
	{
		BYTE bytRecordType=0, bytMarketCode=0, bytSubMarket=0;
		bytRecordType=pDBPage->GetRecordType();
		bytMarketCode=pDBPage->GetMarketCode();
		bytMarketCode=pDBPage->GetSubMarket();
		m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();

		MsgObject* MsgObj = new MsgObject();
		MsgObj->AddFid(enumFieldID, (short*) &nInput);		// Short	
		MsgObj->SetItemNo(lPageNo);
		COP_ITEM copMsg = new MsgObject(MsgObj->GetMessage());
		if (MsgObj)
		{
			delete MsgObj;
			MsgObj = NULL;
		}
		// Trigger the relative callback functions
		m_pCallback->DoCallbackFunctions(lPageNo, enumFieldID, lIndex, copMsg, bytRecordType, bytMarketCode, bytSubMarket);
		return(TRUE);
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return(FALSE);
}
// -------------------------------------------------------------------------
// unsigned short
BOOL CDCache::SetAField(long lPageNo, unsigned short uInput,
							 FID_DEFINITION enumFieldID, long lIndex)
{
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();



	CPage *pDBPage = NULL;
	if (!m_mapPages.Lookup(lPageNo, (CPage*)pDBPage))
	{
		if ((pDBPage=new CPage) == NULL)
		{
			// Show Alert
			m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
			return(FALSE);
		}
		pDBPage->SetPageNo(lPageNo);
		m_CriticalSection[0].Enter();
		m_mapPages[lPageNo] = pDBPage;
		m_CriticalSection[0].Leave();
	}

	if (pDBPage->SetAField(uInput, enumFieldID, lIndex))
	{
		// Trigger the relative callback functions
		BYTE bytRecordType=0, bytMarketCode=0, bytSubMarket=0;
		bytRecordType=pDBPage->GetRecordType();
		bytMarketCode=pDBPage->GetMarketCode();
		bytMarketCode=pDBPage->GetSubMarket();
		m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
		MsgObject* MsgObj = new MsgObject();
		MsgObj->AddFid(enumFieldID, (unsigned short*) &uInput);		//	uShort	
		MsgObj->SetItemNo(lPageNo);
		COP_ITEM copMsg = new MsgObject(MsgObj->GetMessage());
		if (MsgObj)
		{
			delete MsgObj;
			MsgObj = NULL;
		}
		m_pCallback->DoCallbackFunctions(lPageNo, enumFieldID, lIndex, copMsg, bytRecordType, bytMarketCode, bytSubMarket);
		return(TRUE);
	}

	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return(FALSE);
}
// -------------------------------------------------------------------------
// float
BOOL CDCache::SetAField(long lPageNo, float fInput,
							 FID_DEFINITION enumFieldID, long lIndex)
{
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();


	CPage *pDBPage = NULL;
	if (!m_mapPages.Lookup(lPageNo, (CPage*)pDBPage))
	{
		if ((pDBPage=new CPage) == NULL)
		{
			// Show Alert
			m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
			return(FALSE);
		}
		pDBPage->SetPageNo(lPageNo);
		m_CriticalSection[0].Enter();
		m_mapPages[lPageNo] = pDBPage;
		m_CriticalSection[0].Leave();
	}

	if (pDBPage->SetAField(fInput, enumFieldID, lIndex))
	{
		// Trigger the relative callback functions
		BYTE bytRecordType=0, bytMarketCode=0, bytSubMarket=0;
		bytRecordType=pDBPage->GetRecordType();
		bytMarketCode=pDBPage->GetMarketCode();
		bytMarketCode=pDBPage->GetSubMarket();
		m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
		MsgObject* MsgObj = new MsgObject();
		MsgObj->AddFid(enumFieldID, (float*) &fInput);		// uShort	
		MsgObj->SetItemNo(lPageNo);
		COP_ITEM copMsg = new MsgObject(MsgObj->GetMessage());
		if (MsgObj)
		{
			delete MsgObj;
			MsgObj = NULL;
		}
		m_pCallback->DoCallbackFunctions(lPageNo, enumFieldID, lIndex, copMsg, bytRecordType, bytMarketCode, bytSubMarket);
		return(TRUE);
	}

	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return(FALSE);
}
// -------------------------------------------------------------------------
// double
BOOL CDCache::SetAField(long lPageNo, double dInput,
							 FID_DEFINITION enumFieldID, long lIndex)
{	
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();



	CPage *pDBPage = NULL;
	if (!m_mapPages.Lookup(lPageNo, (CPage*)pDBPage))
	{
		if ((pDBPage=new CPage) == NULL)
		{
			// Show Alert
			m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
			return(FALSE);
		}
		pDBPage->SetPageNo(lPageNo);
		m_CriticalSection[0].Enter();
		m_mapPages[lPageNo] = pDBPage;
		m_CriticalSection[0].Leave();
	}

	if (pDBPage->SetAField(dInput, enumFieldID, lIndex))
	{
		// Trigger the relative callback functions
		BYTE bytRecordType=0, bytMarketCode=0, bytSubMarket=0;
		bytRecordType=pDBPage->GetRecordType();
		bytMarketCode=pDBPage->GetMarketCode();
		bytMarketCode=pDBPage->GetSubMarket();
		m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
		MsgObject* MsgObj = new MsgObject();
		MsgObj->AddFid(enumFieldID, (double*) &dInput);		//	double	
		MsgObj->SetItemNo(lPageNo);
		COP_ITEM copMsg = new MsgObject(MsgObj->GetMessage());
		if (MsgObj)
		{
			delete MsgObj;
			MsgObj = NULL;
		}
		m_pCallback->DoCallbackFunctions(lPageNo, enumFieldID, lIndex, copMsg, bytRecordType, bytMarketCode, bytSubMarket);
		return(TRUE);
	}

	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return(FALSE);
}
// -------------------------------------------------------------------------
// unsigned char
BOOL CDCache::SetAField(long lPageNo, unsigned char cInput,
							 FID_DEFINITION enumFieldID, long lIndex)
{
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();



	CPage *pDBPage = NULL;
	if (!m_mapPages.Lookup(lPageNo, (CPage*)pDBPage))
	{
		if ((pDBPage=new CPage) == NULL)
		{
			// Show Alert
			m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
			return(FALSE);
		}
		m_lNoOfPage++;
		pDBPage->SetPageNo(lPageNo);
		m_CriticalSection[0].Enter();
		m_mapPages[lPageNo] = pDBPage;
		m_CriticalSection[0].Leave();
	}

	if (pDBPage->SetAField(cInput, enumFieldID, lIndex))
	{
		// Trigger the relative callback functions
		BYTE bytRecordType=0, bytMarketCode=0, bytSubMarket=0;
		bytRecordType=pDBPage->GetRecordType();
		bytMarketCode=pDBPage->GetMarketCode();
		bytMarketCode=pDBPage->GetSubMarket();
		m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
		MsgObject* MsgObj = new MsgObject();
		MsgObj->AddFid(enumFieldID, (unsigned char*) &cInput);		// char	
		MsgObj->SetItemNo(lPageNo);
		COP_ITEM copMsg = new MsgObject(MsgObj->GetMessage());
		if (MsgObj)
		{
			delete MsgObj;
			MsgObj = NULL;
		}
		m_pCallback->DoCallbackFunctions(lPageNo, enumFieldID, lIndex, copMsg, bytRecordType, bytMarketCode, bytSubMarket);
		return(TRUE);
	}

	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return(FALSE);
}

// -------------------------------------------------------------------------
// long 
BOOL CDCache::SetAField(long lPageNo, long lInput,
							 FID_DEFINITION enumFieldID, long lIndex, COP_ITEM copMsg)
{
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();



	CPage *pDBPage = NULL;
	if (!m_mapPages.Lookup(lPageNo, (CPage*)pDBPage))
	{
		if ((pDBPage=new CPage) == NULL)
		{
			// Show Alert
			m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
			return(FALSE);
		}
		m_lNoOfPage++;
		pDBPage->SetPageNo(lPageNo);
		m_CriticalSection[0].Enter();
		m_mapPages[lPageNo] = pDBPage;
		m_CriticalSection[0].Leave();
	}

	if (pDBPage->SetAField(lInput, enumFieldID, lIndex))
	{
		BYTE bytRecordType=0, bytMarketCode=0, bytSubMarket=0;
		bytRecordType=pDBPage->GetRecordType();
		bytMarketCode=pDBPage->GetMarketCode();
		bytMarketCode=pDBPage->GetSubMarket();
		m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
		// Trigger the relative callback functions
		m_pCallback->DoCallbackFunctions(lPageNo, enumFieldID, lIndex, copMsg, bytRecordType, bytMarketCode, bytSubMarket);
		return(TRUE);
	}

	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return(FALSE);
}

// -------------------------------------------------------------------------
// unsigned long
BOOL CDCache::SetAField(long lPageNo, unsigned long ulInput,
							 FID_DEFINITION enumFieldID, long lIndex, COP_ITEM copMsg)
{
	
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();



	CPage *pDBPage = NULL;
	if (!m_mapPages.Lookup(lPageNo, (CPage*)pDBPage))
	{
		if ((pDBPage=new CPage) == NULL)
		{
			// Show Alert
			m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
			return(FALSE);
		}
		m_lNoOfPage++;
		pDBPage->SetPageNo(lPageNo);
		m_CriticalSection[0].Enter();
		m_mapPages[lPageNo] = pDBPage;
		m_CriticalSection[0].Leave();
	}

	if (pDBPage->SetAField(ulInput, enumFieldID, lIndex))
	{
		// Trigger the relative callback functions
		BYTE bytRecordType=0, bytMarketCode=0, bytSubMarket=0;
		bytRecordType=pDBPage->GetRecordType();
		bytMarketCode=pDBPage->GetMarketCode();
		bytMarketCode=pDBPage->GetSubMarket();
		m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
		m_pCallback->DoCallbackFunctions(lPageNo, enumFieldID, lIndex, copMsg, bytRecordType, bytMarketCode, bytSubMarket);
		return(TRUE);
	}

	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return(FALSE);
}
// -------------------------------------------------------------------------
// short
BOOL CDCache::SetAField(long lPageNo, short nInput,
							 FID_DEFINITION enumFieldID, long lIndex, COP_ITEM copMsg)
{
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();



	CPage *pDBPage = NULL;
	if (!m_mapPages.Lookup(lPageNo, (CPage*)pDBPage))
	{
		if ((pDBPage=new CPage) == NULL)
		{
			// Show Alert
			m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
			return(FALSE);
		}
		m_lNoOfPage++;
		pDBPage->SetPageNo(lPageNo);
		m_CriticalSection[0].Enter();
		m_mapPages[lPageNo] = pDBPage;
		m_CriticalSection[0].Leave();
	}
	if (pDBPage->SetAField(nInput, enumFieldID, lIndex))
	{
		BYTE bytRecordType=0, bytMarketCode=0, bytSubMarket=0;
		bytRecordType=pDBPage->GetRecordType();
		bytMarketCode=pDBPage->GetMarketCode();
		bytMarketCode=pDBPage->GetSubMarket();
		m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
		// Trigger the relative callback functions
		m_pCallback->DoCallbackFunctions(lPageNo, enumFieldID, lIndex, copMsg, bytRecordType, bytMarketCode, bytSubMarket);
		return(TRUE);
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return(FALSE);
}


// -------------------------------------------------------------------------
// unsigned short
BOOL CDCache::SetAField(long lPageNo, unsigned short uInput,
							 FID_DEFINITION enumFieldID, long lIndex, COP_ITEM copMsg)
{
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();



	CPage *pDBPage = NULL;
	if (!m_mapPages.Lookup(lPageNo, (CPage*)pDBPage))
	{
		if ((pDBPage=new CPage) == NULL)
		{
			// Show Alert
			m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
			return(FALSE);
		}
		pDBPage->SetPageNo(lPageNo);
		m_CriticalSection[0].Enter();
		m_mapPages[lPageNo] = pDBPage;
		m_CriticalSection[0].Leave();
	}

	if (pDBPage->SetAField(uInput, enumFieldID, lIndex))
	{
		// Trigger the relative callback functions
		BYTE bytRecordType=0, bytMarketCode=0, bytSubMarket=0;
		bytRecordType=pDBPage->GetRecordType();
		bytMarketCode=pDBPage->GetMarketCode();
		bytMarketCode=pDBPage->GetSubMarket();
		m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
		m_pCallback->DoCallbackFunctions(lPageNo, enumFieldID, lIndex, copMsg, bytRecordType, bytMarketCode, bytSubMarket);
		return(TRUE);
	}

	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return(FALSE);
}
// -------------------------------------------------------------------------
// float
BOOL CDCache::SetAField(long lPageNo, float fInput,
							 FID_DEFINITION enumFieldID, long lIndex, COP_ITEM copMsg)
{
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();



	CPage *pDBPage = NULL;
	if (!m_mapPages.Lookup(lPageNo, (CPage*)pDBPage))
	{
		if ((pDBPage=new CPage) == NULL)
		{
			// Show Alert
			m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
			return(FALSE);
		}
		pDBPage->SetPageNo(lPageNo);
		m_CriticalSection[0].Enter();
		m_mapPages[lPageNo] = pDBPage;
		m_CriticalSection[0].Leave();
	}

	if (pDBPage->SetAField(fInput, enumFieldID, lIndex))
	{
		// Trigger the relative callback functions
		BYTE bytRecordType=0, bytMarketCode=0, bytSubMarket=0;
		bytRecordType=pDBPage->GetRecordType();
		bytMarketCode=pDBPage->GetMarketCode();
		bytMarketCode=pDBPage->GetSubMarket();
		m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
		m_pCallback->DoCallbackFunctions(lPageNo, enumFieldID, lIndex, copMsg, bytRecordType, bytMarketCode, bytSubMarket);
		return(TRUE);
	}

	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return(FALSE);
}

// -------------------------------------------------------------------------
// double
BOOL CDCache::SetAField(long lPageNo, double dInput,
							 FID_DEFINITION enumFieldID, long lIndex, COP_ITEM copMsg)
{
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();



	CPage *pDBPage = NULL;
	if (!m_mapPages.Lookup(lPageNo, (CPage*)pDBPage))
	{
		if ((pDBPage=new CPage) == NULL)
		{
			// Show Alert
			m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
			return(FALSE);
		}
		pDBPage->SetPageNo(lPageNo);
		m_CriticalSection[0].Enter();
		m_mapPages[lPageNo] = pDBPage;
		m_CriticalSection[0].Leave();
	}

	if (pDBPage->SetAField(dInput, enumFieldID, lIndex))
	{
		// Trigger the relative callback functions
		BYTE bytRecordType=0, bytMarketCode=0, bytSubMarket=0;
		bytRecordType=pDBPage->GetRecordType();
		bytMarketCode=pDBPage->GetMarketCode();
		bytMarketCode=pDBPage->GetSubMarket();
		m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
		m_pCallback->DoCallbackFunctions(lPageNo, enumFieldID, lIndex, copMsg, bytRecordType, bytMarketCode, bytSubMarket);
		return(TRUE);
	}

	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return(FALSE);
}

// -------------------------------------------------------------------------
// unsigned char
BOOL CDCache::SetAField(long lPageNo, unsigned char cInput,
							 FID_DEFINITION enumFieldID, long lIndex, COP_ITEM copMsg)
{
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();



	CPage *pDBPage = NULL;
	if (!m_mapPages.Lookup(lPageNo, (CPage*)pDBPage))
	{
		if ((pDBPage=new CPage) == NULL)
		{
			// Show Alert
			m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
			return(FALSE);
		}
		m_lNoOfPage++;
		pDBPage->SetPageNo(lPageNo);
		m_CriticalSection[0].Enter();
		m_mapPages[lPageNo] = pDBPage;
		m_CriticalSection[0].Leave();
	}

	if (pDBPage->SetAField(cInput, enumFieldID, lIndex))
	{
		// Trigger the relative callback functions
		BYTE bytRecordType=0, bytMarketCode=0, bytSubMarket=0;
		bytRecordType=pDBPage->GetRecordType();
		bytMarketCode=pDBPage->GetMarketCode();
		bytMarketCode=pDBPage->GetSubMarket();
		m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
		m_pCallback->DoCallbackFunctions(lPageNo, enumFieldID, lIndex, copMsg, bytRecordType, bytMarketCode, bytSubMarket);
		return(TRUE);
	}

	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return(FALSE);
}

// -------------------------------------------------------------------------
// CharArray
BOOL CDCache::SetAField(long lPageNo, CharArray* pInput, FID_DEFINITION enumFieldID, long lIndex, BOOL bCompare, COP_ITEM copMsg)
{	
	BOOL bRicName = FALSE;

	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();



	CPage *pDBPage = NULL;
	if (!m_mapPages.Lookup(lPageNo, (CPage*)pDBPage))
	{
		if ((pDBPage=new CPage) == NULL)
		{
			// Show Alert
			m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
			return(FALSE);
		}
		m_lNoOfPage++;
		pDBPage->SetPageNo(lPageNo);
		m_CriticalSection[0].Enter();
		m_mapPages[lPageNo] = pDBPage;
		m_CriticalSection[0].Leave();
	}

	CharArray* caOrg;
	CharArray caNew;
	CString strOrg = "";
	if (copMsg->GetFid(FID_RICNAME, caNew))
	{
		bRicName = TRUE;
		long lLine = -2;
		if (pDBPage->GetAField(&caOrg, FID_RICNAME, &lLine))
			strOrg = caOrg->GetString();
	}

	if (pDBPage->SetAField(pInput, enumFieldID, lIndex, bCompare))
	{
		if (bRicName)
		{
//			SetPageNo((const char*)caNew.GetString(), (const char*)((strOrg=="")?NULL:strOrg.GetBuffer(0)), lPageNo);
			pDBPage->SetRicName(caNew.GetString());
		}

		// Trigger the relative callback functions
		BYTE bytRecordType=0, bytMarketCode=0, bytSubMarket=0;
		bytRecordType=pDBPage->GetRecordType();
		bytMarketCode=pDBPage->GetMarketCode();
		bytMarketCode=pDBPage->GetSubMarket();
		m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();

		if( bRicName )
		{
			SetPageNo((const char*)caNew.GetString(), (const char*)((strOrg=="")?NULL:strOrg.GetBuffer(0)), lPageNo);
		}

		m_pCallback->DoCallbackFunctions(lPageNo, enumFieldID, lIndex, copMsg, bytRecordType, bytMarketCode, bytSubMarket);
		return(TRUE);
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return(FALSE);
}

// -------------------------------------------------------------------------
// MCharArray
BOOL CDCache::SetAField(long lPageNo, CharArray* pInput, FID_DEFINITION enumFieldID, long lIndex, int iLineNo, BOOL bCompare, COP_ITEM copMsg)
{
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();



	CPage *pDBPage = NULL;
	if (!m_mapPages.Lookup(lPageNo, (CPage*)pDBPage))
	{
		if ((pDBPage=new CPage) == NULL)
		{
			// Show Alert
			m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
			return(FALSE);
		}
		m_lNoOfPage++;
		pDBPage->SetPageNo(lPageNo);
		m_CriticalSection[0].Enter();
		m_mapPages[lPageNo] = pDBPage;
		m_CriticalSection[0].Leave();
	}

	if (pDBPage->SetAField(pInput, enumFieldID, lIndex, iLineNo, bCompare))
	{
		// Trigger the relative callback functions
		BYTE bytRecordType=0, bytMarketCode=0, bytSubMarket=0;
		bytRecordType=pDBPage->GetRecordType();
		bytMarketCode=pDBPage->GetMarketCode();
		bytMarketCode=pDBPage->GetSubMarket();
		m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
		m_pCallback->DoCallbackFunctions(lPageNo, enumFieldID, iLineNo, copMsg, bytRecordType, bytMarketCode, bytSubMarket);
		return(TRUE);
	}

	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return(FALSE);
}

// -------------------------------------------------------------------------											
// General Type
BOOL CDCache::SetAField(long lPageNo, BYTE* pInput,	FID_DEFINITION enumFieldID, long lSizeOfBlock, COP_ITEM copMsg) 
{
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();



	CPage *pDBPage = NULL;
	if (!m_mapPages.Lookup(lPageNo, (CPage*)pDBPage))
	{
		if ((pDBPage=new CPage) == NULL)
		{
			// Show Alert
			m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
			return(FALSE);
		}
		m_lNoOfPage++;
		pDBPage->SetPageNo(lPageNo);
		m_CriticalSection[0].Enter();
		m_mapPages[lPageNo] = pDBPage;
		m_CriticalSection[0].Leave();
	}

	if (pDBPage->SetAField(pInput, enumFieldID, lSizeOfBlock))
	{
		// Trigger the relative callback functions
		BYTE bytRecordType=0, bytMarketCode=0, bytSubMarket=0;
		bytRecordType=pDBPage->GetRecordType();
		bytMarketCode=pDBPage->GetMarketCode();
		bytMarketCode=pDBPage->GetSubMarket();
		m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
		m_pCallback->DoCallbackFunctions(lPageNo, enumFieldID, 0, copMsg, bytRecordType, bytMarketCode, bytSubMarket);
		return(TRUE);
	}

	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave(); 
	return(FALSE);
}

// -------------------------------------------------------------------------
// Tran
BOOL CDCache::SetAField(long lPageNo, BYTE* pInput,	FID_DEFINITION enumFieldID, int iBlockSize, int iTotalBlock, int iMaxNoOfBlock, BOOL bRipple, COP_ITEM copMsg)
{
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();



	CPage *pDBPage = NULL;
	if (!m_mapPages.Lookup(lPageNo, (CPage*)pDBPage))
	{
		if ((pDBPage=new CPage) == NULL)
		{
			// Show Alert
			m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
			return(FALSE);
		}
		m_lNoOfPage++;
		pDBPage->SetPageNo(lPageNo);
		m_CriticalSection[0].Enter();
		m_mapPages[lPageNo] = pDBPage;
		m_CriticalSection[0].Leave();
	}

	if (pDBPage->SetAField(pInput,	enumFieldID, iBlockSize, iTotalBlock, iMaxNoOfBlock, bRipple, GetTlogRipple(enumFieldID)))
	{

		// Trigger the relative callback functions
		BYTE bytRecordType=0, bytMarketCode=0, bytSubMarket=0;
		bytRecordType=pDBPage->GetRecordType();
		bytMarketCode=pDBPage->GetMarketCode();
		bytMarketCode=pDBPage->GetSubMarket();
		m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
		long lPos = (bRipple)?1:-1;
		m_pCallback->DoCallbackFunctions(lPageNo, enumFieldID, lPos, copMsg, bytRecordType, bytMarketCode, bytSubMarket);
		return(TRUE);
	}

	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return(FALSE);
}


// PrcAskNuBid Type
// -------------------------------------------------------------------------
BOOL CDCache::SetAField(long lPageNo, BYTE* pInput,	FID_DEFINITION enumFieldID, long lPos, int iBlockSize, int iTotalBlock, COP_ITEM copMsg)				
{
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();



	CPage *pDBPage = NULL;
	if (!m_mapPages.Lookup(lPageNo, (CPage*)pDBPage))
	{
		if ((pDBPage=new CPage) == NULL)
		{
			// Show Alert
			m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
			return(FALSE);
		}
		m_lNoOfPage++;
		pDBPage->SetPageNo(lPageNo);
		m_CriticalSection[0].Enter();
		m_mapPages[lPageNo] = pDBPage;
		m_CriticalSection[0].Leave();
	}

	if (pDBPage->SetAField(pInput, enumFieldID, lPos, iBlockSize, iTotalBlock))
	{
		// Trigger the relative callback functions
		BYTE bytRecordType=0, bytMarketCode=0, bytSubMarket=0;
		bytRecordType=pDBPage->GetRecordType();
		bytMarketCode=pDBPage->GetMarketCode();
		bytMarketCode=pDBPage->GetSubMarket();
		m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
		m_pCallback->DoCallbackFunctions(lPageNo, enumFieldID, lPos, copMsg, bytRecordType, bytMarketCode, bytSubMarket);
		return(TRUE);
	}

	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return(FALSE);
}

// PrcVolwSeq Type
// -------------------------------------------------------------------------
BOOL CDCache::SetAField(long lPageNo, BYTE* pInput,	FID_DEFINITION enumFieldID, long lPos, int iBlockSize, int iTotalBlock, int iHeaderSize, int iTotalSize, COP_ITEM copMsg)				
{
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();



	CPage *pDBPage = NULL;
	if (!m_mapPages.Lookup(lPageNo, (CPage*)pDBPage))
	{
		if ((pDBPage=new CPage) == NULL)
		{
			// Show Alert
			m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
			return(FALSE);
		}
		m_lNoOfPage++;
		pDBPage->SetPageNo(lPageNo);
		m_CriticalSection[0].Enter();
		m_mapPages[lPageNo] = pDBPage;
		m_CriticalSection[0].Leave();
	}

	if (pDBPage->SetAField(pInput, enumFieldID, lPos, iBlockSize, iTotalBlock, iHeaderSize, iTotalSize))
	{
		// Trigger the relative callback functions
		BYTE bytRecordType=0, bytMarketCode=0, bytSubMarket=0;
		bytRecordType=pDBPage->GetRecordType();
		bytMarketCode=pDBPage->GetMarketCode();
		bytMarketCode=pDBPage->GetSubMarket();
		m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
		m_pCallback->DoCallbackFunctions(lPageNo, enumFieldID, lPos, copMsg, bytRecordType, bytMarketCode, bytSubMarket);
		return(TRUE);
	}

	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return(FALSE);
}

// -------------------------------------------------------------------------
// Link, WLink, MtcPoints, MtcVal, MTCV, SprTbl, MTCTicks, MTCPTS, MTCF
BOOL CDCache::SetAField(long lPageNo, BYTE* pInput,	FID_DEFINITION enumFieldID, long lPos, long iSizeOfBlock, COP_ITEM copMsg)				
{
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();



	CPage *pDBPage = NULL;
	if (!m_mapPages.Lookup(lPageNo, (CPage*)pDBPage))
	{
		if ((pDBPage=new CPage) == NULL)
		{
			// Show Alert
			m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
			return(FALSE);
		}
		m_lNoOfPage++;
		pDBPage->SetPageNo(lPageNo);
		m_CriticalSection[0].Enter();
		m_mapPages[lPageNo] = pDBPage;
		m_CriticalSection[0].Leave();
	}

	if (pDBPage->SetAField(pInput, enumFieldID, lPos, iSizeOfBlock))
	{
		// Trigger the relative callback functions
		BYTE bytRecordType=0, bytMarketCode=0, bytSubMarket=0;
		bytRecordType=pDBPage->GetRecordType();
		bytMarketCode=pDBPage->GetMarketCode();
		bytMarketCode=pDBPage->GetSubMarket();
		m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
		m_pCallback->DoCallbackFunctions(lPageNo, enumFieldID, lPos, copMsg, bytRecordType, bytMarketCode, bytSubMarket);
		return(TRUE);
	}

	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return(FALSE);
}

// -------------------------------------------------------------------------
// Headline
BOOL CDCache::SetAField(long lPageNo, BYTE* pInput,	FID_DEFINITION enumFieldID, long lIndex, int iSizeOfString, COP_ITEM copMsg)									
{
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();



	CPage *pDBPage = NULL;
	if (!m_mapPages.Lookup(lPageNo, (CPage*)pDBPage))
	{
		if ((pDBPage=new CPage) == NULL)
		{
			// Show Alert
			m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
			return(FALSE);
		}
		m_lNoOfPage++;
		pDBPage->SetPageNo(lPageNo);
		m_CriticalSection[0].Enter();
		m_mapPages[lPageNo] = pDBPage;
		m_CriticalSection[0].Leave();
	}

	if (pDBPage->SetAField(pInput, enumFieldID, lIndex, iSizeOfString))
	{
		// Trigger the relative callback functions
		BYTE bytRecordType=0, bytMarketCode=0, bytSubMarket=0;
		bytRecordType=pDBPage->GetRecordType();
		bytMarketCode=pDBPage->GetMarketCode();
		bytMarketCode=pDBPage->GetSubMarket();
		m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
		m_pCallback->DoCallbackFunctions(lPageNo, enumFieldID, lIndex, copMsg, bytRecordType, bytMarketCode, bytSubMarket);
		return(TRUE);
	}

	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return(FALSE);
}

// -------------------------------------------------------------------------
// Quote List
BOOL CDCache::SetAField(long lPageNo, BYTE* pInput,	FID_DEFINITION enumFieldID, long lIndex, unsigned short usDataNe, byte byteDataTL, COP_ITEM copMsg)													// Quote List
{
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();



	CPage *pDBPage = NULL;
	if (!m_mapPages.Lookup(lPageNo, (CPage*)pDBPage))
	{
		if ((pDBPage=new CPage) == NULL)
		{
			// Show Alert
			m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
			return(FALSE);
		}
		m_lNoOfPage++;
		pDBPage->SetPageNo(lPageNo);
		m_CriticalSection[0].Enter();
		m_mapPages[lPageNo] = pDBPage;
		m_CriticalSection[0].Leave();
	}

	if (pDBPage->SetAField(pInput, enumFieldID, usDataNe, byteDataTL))
	{
		// Trigger the relative callback functions
		BYTE bytRecordType=0, bytMarketCode=0, bytSubMarket=0;
		bytRecordType=pDBPage->GetRecordType();
		bytMarketCode=pDBPage->GetMarketCode();
		bytMarketCode=pDBPage->GetSubMarket();
		m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
		m_pCallback->DoCallbackFunctions(lPageNo, enumFieldID, lIndex, copMsg, bytRecordType, bytMarketCode, bytSubMarket);
		return(TRUE);
	}

	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return(FALSE);
}

// -------------------------------------------------------------------------
// Drop MCharArrayLine
BOOL CDCache::DropMCharArrayLine(long lPageNo, FID_DEFINITION enumFieldID, long lCurTl, long lNewTl)
{
	BOOL bReturnValue = FALSE;
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pDBPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pDBPage))
	{
		if (pDBPage->DropMCharArrayLn(enumFieldID, lCurTl, lNewTl))
			bReturnValue = TRUE;
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return bReturnValue;
}
// -------------------------------------------------------------------------
// Drop a field
BOOL CDCache::DropAField(long lPageNo, FID_DEFINITION enumFieldID, COP_ITEM copMsg)
{
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();

	CPage *pDBPage = NULL;

	if (!m_mapPages.Lookup(lPageNo, (CPage*)pDBPage))
	{
		if ((pDBPage=new CPage) == NULL)
		{
			// Show Alert
			m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
			return(FALSE);
		}
		pDBPage->SetPageNo(lPageNo);
		m_CriticalSection[0].Enter();
		m_mapPages[lPageNo] = pDBPage;
		m_CriticalSection[0].Leave();
	}	

	if (pDBPage->DropAField(enumFieldID))
	{
		// Trigger the relative callback functions
		BYTE bytRecordType=0, bytMarketCode=0, bytSubMarket=0;
		bytRecordType=pDBPage->GetRecordType();
		bytMarketCode=pDBPage->GetMarketCode();
		bytMarketCode=pDBPage->GetSubMarket();
		m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
		m_pCallback->DoCallbackFunctions(lPageNo, enumFieldID, DROP_PAGE, copMsg, bytRecordType, bytMarketCode, bytSubMarket);
		return(TRUE);
	}

	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return(FALSE);
}

// -------------------------------------------------------------------------
BOOL CDCache::AddAPageWatcher(long lPageNo, CObject* pCallbackObject,
		void (*fpCallbackFunction)(long lPageNo, FID_DEFINITION enumFieldID,
								long lIndex, COP_ITEM copMsg, CObject *pCallbackObject))
{
	BOOL bResult;

	//m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	bResult = m_pCallback->AddItem(lPageNo, FID_END, 0, 0, 0,
		pCallbackObject, fpCallbackFunction);
	//m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return(bResult);
}
// -------------------------------------------------------------------------
BOOL CDCache::AddAFieldWatcher(FID_DEFINITION enumFieldID, BYTE bytRecordType, BYTE bytMarketCode, BYTE bytSubMarket, CObject* pCallbackObject,
		void (*fpCallbackFunction)(long lPageNo, FID_DEFINITION enumFieldID,
								long lIndex, COP_ITEM copMsg, CObject *pCallbackObject))
{
	BOOL bResult;

	//m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	bResult = m_pCallback->AddItem(-1, enumFieldID, bytRecordType, bytMarketCode, bytSubMarket,
		pCallbackObject, fpCallbackFunction);
	//m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return(bResult);
}

// -------------------------------------------------------------------------
BOOL CDCache::AddAPageAndMultipleFieldWatcher(long lPageNo,
		CWordArray& cwaFIDList, CObject *pCallbackObject,
		void (*fpCallbackFunction)(long lPageNo, FID_DEFINITION enumFieldID,
								long lIndex, COP_ITEM copMsg, CObject *pCallbackObject))
{
	BOOL bResult = TRUE;

	short enumFieldID;
	int i, size = cwaFIDList.GetSize();

	//m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	for( i=0;i<size;++i) {
		enumFieldID = cwaFIDList.GetAt(i);
		bResult &= m_pCallback->AddItem(lPageNo, (FID_DEFINITION)enumFieldID, 0, 0, 0, 
			pCallbackObject, fpCallbackFunction);
		if( bResult == FALSE)
			break;
	}
	if( bResult == FALSE) {
		for( int j = 0; j< i;++j) {
			enumFieldID = cwaFIDList.GetAt(j);
			m_pCallback->RemoveItem(lPageNo, (FID_DEFINITION)enumFieldID, 0, 0, 0 , pCallbackObject);
		}
	}
	//m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return(bResult);
}

// -------------------------------------------------------------------------
BOOL CDCache::AddAPageAndFieldWatcher(long lPageNo,
		FID_DEFINITION enumFieldID, CObject *pCallbackObject,
		void (*fpCallbackFunction)(long lPageNo, FID_DEFINITION enumFieldID,
								long lIndex, COP_ITEM copMsg, CObject *pCallbackObject))
{
	BOOL bResult;

	//m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	bResult = m_pCallback->AddItem(lPageNo, enumFieldID, 0, 0, 0, 
		pCallbackObject, fpCallbackFunction);
	//m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return(bResult);
}

// -------------------------------------------------------------------------
BOOL CDCache::AddAPageAndFieldWatcher(long lPageNo,
		FID_DEFINITION enumFieldID, BYTE bytRecordType, BYTE bytMarketCode, BYTE bytSubMarket, CObject *pCallbackObject,
		void (*fpCallbackFunction)(long lPageNo, FID_DEFINITION enumFieldID,
								long lIndex, COP_ITEM copMsg, CObject *pCallbackObject))
{
	BOOL bResult;

	//m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	bResult = m_pCallback->AddItem(lPageNo, enumFieldID, bytRecordType, bytMarketCode, bytSubMarket,
		pCallbackObject, fpCallbackFunction);
	//m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return(bResult);
}

// -------------------------------------------------------------------------
void CDCache::ForceSingleCallback(CObject *pCallbackObject,
	void(*fpCallbackFunction)(long lPageNo, FID_DEFINITION enumFieldID,
	long lIndex, COP_ITEM copMsg, CObject *pCallbackObject))
{
	BOOL bResult;
	m_pCallback->ForceSingleCallback(pCallbackObject, fpCallbackFunction);
}

// -------------------------------------------------------------------------
BOOL CDCache::AddAAnyWatcher(CObject *pCallbackObject,
		void (*fpCallbackFunction)(long lPageNo, FID_DEFINITION enumFieldID,
								long lIndex, COP_ITEM copMsg, CObject *pCallbackObject))
{
	BOOL bResult;

	//m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	bResult = m_pCallback->AddItem(-1, FID_END, 0, 0, 0, 
		pCallbackObject, fpCallbackFunction);
	//m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return(bResult);
}
// -------------------------------------------------------------------------
BOOL CDCache::RemoveAPageWatcher(const long lPageNo, CObject* pCallbackObject)
{
	BOOL bResult;

	//m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	bResult = m_pCallback->RemoveItem(lPageNo, FID_END, 0, 0, 0, pCallbackObject);
	//m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return(bResult);
}
// -------------------------------------------------------------------------
BOOL CDCache::RemoveAFieldWatcher(const FID_DEFINITION enumFieldID, BYTE bytRecordType, BYTE bytMarketCode, BYTE bytSubMarket, CObject* pCallbackObject)
{
	BOOL bResult;

	//m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	bResult = m_pCallback->RemoveItem(-1, enumFieldID, bytRecordType, bytMarketCode, bytSubMarket, pCallbackObject);
	//m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return(bResult);
}

// -------------------------------------------------------------------------
BOOL CDCache::RemoveAPageAndMultipleFieldWatcher(const long lPageNo,
		CWordArray& cwaFIDList, CObject* pCallbackObject)
{
	BOOL bResult = TRUE;
	int size = cwaFIDList.GetSize();
	short enumFieldID;
	//m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	for(int i =0;i<size;++i) {
		enumFieldID = cwaFIDList.GetAt(i);
		bResult &= m_pCallback->RemoveItem(lPageNo, (FID_DEFINITION)enumFieldID, 0, 0, 0, pCallbackObject);
	}
	//m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return(bResult);
}

// -------------------------------------------------------------------------
BOOL CDCache::RemoveAPageAndFieldWatcher(const long lPageNo, const FID_DEFINITION enumFieldID, BYTE bytRecordType, BYTE bytMarketCode, BYTE bytSubMarket, CObject* pCallbackObject)
{
	BOOL bResult;

	//m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	bResult = m_pCallback->RemoveItem(lPageNo, enumFieldID, bytRecordType, bytMarketCode, bytSubMarket, pCallbackObject);
	//m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return(bResult);
}
// -------------------------------------------------------------------------
BOOL CDCache::RemoveAPageAndFieldWatcher(const long lPageNo,
		const FID_DEFINITION enumFieldID,  CObject* pCallbackObject)
{
	BOOL bResult;

	//m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	bResult = m_pCallback->RemoveItem(lPageNo, enumFieldID, 0, 0, 0, pCallbackObject);
	//m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return(bResult);
}
// -------------------------------------------------------------------------
BOOL CDCache::RemoveAAnyWatcher(CObject* pCallbackObject)
{
	BOOL bResult;
	
	//m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	bResult = m_pCallback->RemoveItem(-1, FID_END, 0, 0, 0,pCallbackObject);
	//m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return(bResult);
}
// -------------------------------------------------------------------------
char* CDCache::GetVersionString()
{
	return("1.4.11.34");
}
// -------------------------------------------------------------------------
char* CDCache::GetModuleString()
{
	return("Module... : DCache Ver.1.4.11.34 07/02/2013");
}
// -------------------------------------------------------------------------
char* CDCache::GetStatus()
{
	static char szBuffer[512];

	sprintf(szBuffer, "Total no of pages = (%ld)", m_lNoOfPage);
	return(szBuffer);
}
// -------------------------------------------------------------------------
void CDCache::RemoveFidFromDropList(long lPageNo, FID_DEFINITION enumFieldID)
{
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();

	CPage *pDBPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pDBPage))
		pDBPage->RemoveFidFromDropList(enumFieldID);
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
}
BOOL CDCache::PutDataIntoDCache(COP_ITEM copInput, CWordArray& cwaUpdated)
{
	long lPageNo = copInput->GetItemNo();		// Get Page No;
	
	if (copInput->GetMsgType()==MGT_DROP)
	{
		try {
			FieldList fldLst;
			if (copInput->GetFid(GetDropFidNo(), fldLst))
			{
				unsigned short nFd;
				for (int i=0; i<fldLst.GetNe(); i++)
				{
					if (fldLst.GetField(i, nFd))
					{
						if (nFd == FID_TRAN_LOG)
						{
							nFd = FID_TRAN_D10_1; // 2700
						}
						DropAField(lPageNo, (FID_DEFINITION)nFd, copInput); // CS0
					}
				}
			}
			else
			{
				DropAPage(lPageNo, copInput); // CS0
			}
		}
		catch (...)
		{
			m_CriticalSection[0].Leave();
			m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
			char szFaultMsg[MAX_LEN];
			sprintf(szFaultMsg, "(DCACHE) Received Drop Msg whose Page No(%d); msg has been igonred.",lPageNo);
			LogFault((char*)&szFaultMsg);
			return FALSE;
		}
	}
	else
	{
		short CurrentFidCode = 0;
		RemovePageFromDropList(lPageNo);
		CDCacheFidIterator PI(copInput->GetMessage());
		try {
//			unsigned char LS_bits;
			bool bTranlog1301 = false;

			PI.SetMyDCache(this);
			PI.First();
			while (!PI.IsEnd())
			{
				CurrentFidCode = PI.GetCurFidCode();
				BOOL bProcessSuccess = PI.ProcessFid();
				if( bProcessSuccess ) 
				{
					cwaUpdated.Add(CurrentFidCode);

					// 2013 - Tranlog conversion
					if (CurrentFidCode == FID_TRAN_LOG)
					{
						bTranlog1301 = true;
						CurrentFidCode = 2700;
					}
				}

				/*
				if (CurrentFidCode <= FID_END)
				{
					LS_bits = m_FidUpdateFlag[CurrentFidCode/8] << (CurrentFidCode%8+1);
					LS_bits = LS_bits >> (CurrentFidCode%8+1);
					if (bProcessSuccess)
						m_FidUpdateFlag[CurrentFidCode/8] = (m_FidUpdateFlag[CurrentFidCode/8]>>(7-CurrentFidCode%8)|0x01)<<(7-CurrentFidCode%8) | LS_bits;
					else 
						m_FidUpdateFlag[CurrentFidCode/8] = (m_FidUpdateFlag[CurrentFidCode/8]>>(7-CurrentFidCode%8)&0xFE)<<(7-CurrentFidCode%8) | LS_bits;
				}*/
				
				PI.Next();
				CurrentFidCode = 0;
			}
			if (bTranlog1301)
			{
				ProcessTranConv(&PI);
			}
		}
		catch (...)
		{
			m_CriticalSection[0].Leave();
			m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
			char szFaultMsg[MAX_LEN];
			sprintf(szFaultMsg, "(DCACHE) Received msg Page(%d) Fid(%d); Relevant fid in page has been dropped!",lPageNo, CurrentFidCode);
			LogFault((char*)&szFaultMsg);
		}
	}
	return(TRUE);
}

// -------------------------------------------------------------------------
BOOL CDCache::PutDataIntoDCache(COP_ITEM copInput)
{
	long lPageNo = copInput->GetItemNo();		// Get Page No;
	
	if (copInput->GetMsgType()==MGT_DROP)
	{
		try {
			FieldList fldLst;
			if (copInput->GetFid(GetDropFidNo(), fldLst))
			{
				unsigned short nFd;
				for (int i=0; i<fldLst.GetNe(); i++)
				{
					if (fldLst.GetField(i, nFd))
					{
						if (nFd == FID_TRAN_LOG)
						{
							nFd = FID_TRAN_D10_1; // 2700
						}
						DropAField(lPageNo, (FID_DEFINITION)nFd, copInput); // CS0
					}
				}
			}
			else
			{
				DropAPage(lPageNo, copInput); // CS0
			}
		}
		catch (...)
		{
			char szFaultMsg[MAX_LEN];
			sprintf(szFaultMsg, "(DCACHE) Received Drop Msg whose Page No(%d); msg has been igonred.",lPageNo);
			LogFault((char*)&szFaultMsg);
			//m_CriticalSection.Leave();
			//UnlockCriticalSection();
			return FALSE;
		}
	}
	else
	{
		short CurrentFidCode = 0;
		RemovePageFromDropList(lPageNo);
		CDCacheFidIterator PI(copInput->GetMessage());
		try {
			unsigned char LS_bits;
			bool bTranlog1301 = false; 

			PI.SetMyDCache(this);
			PI.First();
			while (!PI.IsEnd())
			{
				CurrentFidCode = PI.GetCurFidCode();
				BOOL bProcessSuccess = PI.ProcessFid();
				// 2013 - Tranlog conversion
				if (CurrentFidCode == FID_TRAN_LOG)
				{
					bTranlog1301 = true;
					CurrentFidCode = 2700;
				}

				if (CurrentFidCode <= FID_END)
				{
					LS_bits = m_FidUpdateFlag[CurrentFidCode/8] << (CurrentFidCode%8+1);
					LS_bits = LS_bits >> (CurrentFidCode%8+1);
					if (bProcessSuccess)
						m_FidUpdateFlag[CurrentFidCode/8] = (m_FidUpdateFlag[CurrentFidCode/8]>>(7-CurrentFidCode%8)|0x01)<<(7-CurrentFidCode%8) | LS_bits;
					else 
						m_FidUpdateFlag[CurrentFidCode/8] = (m_FidUpdateFlag[CurrentFidCode/8]>>(7-CurrentFidCode%8)&0xFE)<<(7-CurrentFidCode%8) | LS_bits;
				}
				
				PI.Next();
				CurrentFidCode = 0;
			}
			if (bTranlog1301)
			{
				ProcessTranConv(&PI);
			}
		}
		catch (...)
		{
			char szFaultMsg[MAX_LEN];
			sprintf(szFaultMsg, "(DCACHE) Received msg Page(%d) Fid(%d); Relevant fid in page has been dropped!",lPageNo, CurrentFidCode);
			LogFault((char*)&szFaultMsg);
			//m_CriticalSection.Leave();
			//UnlockCriticalSection();
		}
	}
	return(TRUE);
}

void CDCache::ProcessTranConv(void *p)
{
	if (p == NULL)
		return;

	CDCacheFidIterator *pIter = (CDCacheFidIterator *)p;
	Tran tran;
	char cSecond;

	unsigned long sq = 0;
	BTimeMin tmMin;
	double vl = 0;
	float nm = 0;
	char fg = 0;
	unsigned char ucSec;
	COP_ITEM copMsg;
	TranD td;
	bool bTranReady = false;
	if ( pIter->GetProcessTran(1301, tran, copMsg) )
	{
		// convert FID 2143 into second for TranD
		if (copMsg->GetMsgType() == MGT_FORCEUPDATE)
		{
			if ( !pIter->GetProcessChar(2143, cSecond) )
			{
				cSecond = (unsigned char)255;
			}

			if ( tran.GetTranElement(0, sq, tmMin, vl, nm, (byte&)fg) )
			{
				BTimeSec ts(tmMin.GetHH(), tmMin.GetMIN(), cSecond);
				td.AddTranElement(sq, ts, vl, nm, fg);
			}
			bTranReady = true;
		}
		else
		{
			CharArray ca;
			if ( pIter->GetProcessCharArray(1936, ca) )
			{
				byte bLen = ca.GetLen();
				byte bPos = ca.GetPos();
				byte bSq = ca.GetSq();
				byte *pData = (byte*)ca.GetString();
				unsigned long ulSeq = *(unsigned long *)pData;
				unsigned char ucNe = *(unsigned char *)(pData+4);				

				if (ucNe == tran.GetNe())
				{
					bool bCont = false;
					for (int iNe=0; iNe<tran.GetNe(); iNe++)
					{
						tran.GetTranElement(iNe, sq, tmMin, vl, nm, (byte&)fg);
						if (iNe == 0 && ulSeq == sq)
						{
							bCont = true;
						}
						if (bCont)
						{
							ucSec = *(pData+4+1+iNe);
							BTimeSec ts(tmMin.GetHH(), tmMin.GetMIN(), ucSec);
							td.AddTranElement(sq, ts, vl, nm, fg);
						}
					}
					if (bCont)
					{
						bTranReady = true;
					}
//					else
//					{
//						CApplManClient::Log(LOG_ERROR, "ProcessTranConv SEQ mismatch");
//					}
				}
//				else
//				{
//					CApplManClient::Log(LOG_ERROR, "ProcessTranConv NE mismatch");
//				}
			}
			else
			{
				ucSec = 255;
				for (int iNe=0; iNe<tran.GetNe(); iNe++)
				{
					tran.GetTranElement(iNe, sq, tmMin, vl, nm, (byte&)fg);
					BTimeSec ts(tmMin.GetHH(), tmMin.GetMIN(), ucSec);
					td.AddTranElement(sq, ts, vl, nm, fg);
				}
				bTranReady = true;
			}
		}

		if (bTranReady)
		{
			if (m_bCB1301)
			{
				pIter->SetTranD(FID_TRAN_D10_1, td); // FID 2700
			}
			else
			{
				// cop
				COP_ITEM copTran = new MsgObject;
				copTran->SetItemNo(copMsg->GetItemNo());
				copTran->SetMsgType(copMsg->GetMsgType());
				copTran->AddFid(FID_TRAN_D10_1, &td);
				copTran->SetDPID(copMsg->GetDPID());
				copTran->SetDPInsID(copMsg->GetDPInsID());
				copTran->GetMessage();

				short CurrentFidCode;
				CDCacheFidIterator PI(copTran->GetMessage());
				PI.SetMyDCache(this);
				PI.First();
				while (!PI.IsEnd())
				{
					CurrentFidCode = PI.GetCurFidCode();
					BOOL bProcessSuccess = PI.ProcessFid();
					PI.Next();
				}
			}
		}
	}
}

// -------------------------------------------------------------------------
void CDCache::UnlockCriticalSection()
{
	/*
	char szFaultMsg[MAX_LEN];
	CString strError = "(DCACHE) Unlock Critical Section - ";
	if (m_pCallback->ReleaseCriticalSection()>0)
		strError += "Callback &";
	if (m_CriticalSection.LeaveAll()>0)
		strError += "DCache";
	sprintf(szFaultMsg, "%s",strError.GetBuffer(0));
	LogFault((char*)&szFaultMsg, FALSE);*/
}
// -------------------------------------------------------------------------
BOOL CDCache::IsFidUpdated(FID_DEFINITION Fid)
{	
	BOOL bReturnValue = FALSE;
	if (Fid < FID_END)
		bReturnValue = (BOOL)(m_FidUpdateFlag[Fid/8]>>(7-Fid%8)&0x01);
	return bReturnValue;
}
// -------------------------------------------------------------------------
/*
void CDCache::SetIgnorePages(CString *pstrPageList)
{
	m_pIgnorePages->SetPages(pstrPageList);
}
*/
// -------------------------------------------------------------------------
void CDCache::RemovePageFromDropList(long lPageNo)
{
	m_CriticalSection[0].Enter();
	m_mapDropPages.RemoveKey(lPageNo);
	m_CriticalSection[0].Leave();
}

// -------------------------------------------------------------------------
// Read data from Disk
// -------------------------------------------------------------------------
void CDCache::GetDataFromDisk()
{
	FILE *data_fp = NULL;
	BOOL bReadSuccessFlag = FALSE;
	CString NewFileName[2];
	CString OldFileName[2];

	NewFileName[0] = m_DataFileName +".dat" ;
	OldFileName[0] = m_DataFileName +".old" ;
	NewFileName[1] = m_DataFileName +"_.dat" ;
	OldFileName[1] = m_DataFileName +"_.old" ;

	for(int count=0; count<2; count++)
	{
#if _MSC_VER > 1500
		if ((fopen_s(&data_fp,NewFileName[count].GetBuffer(0), "rb")) ==0)
#else
		if ((data_fp=fopen(NewFileName[count].GetBuffer(0), "rb")) != NULL)
#endif
		{
			char strLastMessage[] = "DCACHE_COMPLETED";
			char strTestMessage[30] = "";
			fseek( data_fp , -16, SEEK_END);
			fread( &strTestMessage, 1, 16, data_fp);
			strTestMessage[17] = 0x00;
			if (strcmp(strTestMessage, strLastMessage)==0)
				bReadSuccessFlag = ReadData(data_fp);
			fclose(data_fp);
		}
		if (!bReadSuccessFlag)
#if _MSC_VER > 1500
			if ((fopen_s(&data_fp,OldFileName[count].GetBuffer(0), "rb"))== 0)
#else
			if ((data_fp=fopen(OldFileName[count].GetBuffer(0), "rb")) != NULL)
#endif
			{
				char strLastMessage[] = "DCACHE_COMPLETED";
				char strTestMessage[30] = "";
				fseek( data_fp , -16, SEEK_END);			
				fread( &strTestMessage, 1, 16, data_fp);
				strTestMessage[17] = 0x00;
				if (strcmp(strTestMessage, strLastMessage)==0)
					bReadSuccessFlag = ReadData(data_fp);
				fclose(data_fp);
			}
	}
} 

// -------------------------------------------------------------------------
BOOL CDCache::ReadData(FILE* data_fp)
{
	if (data_fp!=NULL)		
	{
		size_t	ReadSize;
		BOOL	bEndFlag = FALSE;
		char	strLastMessage[] = "DCACHE_COMPLETED";
		rewind(data_fp);
		while (!bEndFlag)
		{	
			short sMsgLen = 0;
			ReadSize = fread( &sMsgLen , 1,  2, data_fp);
			ASSERT (ReadSize==2);
			BYTE* bytTempBuffer = new BYTE[sMsgLen];
			ReadSize = fread( bytTempBuffer, 1, sMsgLen, data_fp);
			if ((memcmp(strLastMessage, bytTempBuffer, sMsgLen)==0)||feof(data_fp)!=0)
				bEndFlag = TRUE;
			else
			{
				COP_ITEM copReturnPage = new MsgObject(bytTempBuffer);
				copReturnPage->GetMessage();
				ASSERT(sMsgLen==copReturnPage->GetMsgLen());
				if (sMsgLen==copReturnPage->GetMsgLen())
					PutDataIntoDCache(copReturnPage);
				else
					return FALSE;
			}
			delete[] bytTempBuffer;
		}
	} 
	return TRUE;
		
}
// -------------------------------------------------------------------------
void CDCache::StartWriteDataThread()
{
	m_WriteDataThread = new CWriteDataThread(CLEAR_TO_DISK_TIMER, CLEAR_DISK_TIMER, WRITE_TO_DISK_TIMER, WRITE_TIMER, 
											 m_DataFileName, m_iWriteFIDTimeIntv, WRITE_FID_TIMER, this);
}

// -------------------------------------------------------------------------
BOOL CDCache::GetWritingProgress()
{
	BOOL bReturnValue = FALSE;
	if (m_WriteToDiskFlag)
		bReturnValue = m_WriteDataThread->GetProgress();
	return bReturnValue;
}

// -------------------------------------------------------------------------
void CDCache::StopWriteDataThread()
{
	m_WriteDataThread->ExitThread();
	if (m_WriteDataThread->WaitForExit())
	{
		delete m_WriteDataThread;
		m_WriteDataThread = 0;
	}
}

// -------------------------------------------------------------------------
long CDCache::GetIDN_RIC(char* pstrPageName)
{
	long		lPageNo			= -1;
	CObject*	TempObj			= NULL;
	long*		lPhysicalPageNo	= NULL;
	CString		csPageName		= pstrPageName;

	m_CriticalSection[0].Enter();
	if (m_mapPagesName.Lookup(csPageName.GetBuffer(0), TempObj))
	{
		lPhysicalPageNo = (long*) TempObj;
		lPageNo = *lPhysicalPageNo;
	}
	m_CriticalSection[0].Leave();
	
	return lPageNo;
}
// -------------------------------------------------------------------------

BOOL CDCache::GetHeadlineList(long lPageNo, CDWordArray& cdwaHeadlineList, FID_DEFINITION enumFieldID)
{
	BOOL bReturnValue = FALSE;
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	CPage *pPage = NULL;
	if (m_mapPages.Lookup(lPageNo, (CPage*)pPage))
	{
		if (pPage->GetHeadLineFIDList(cdwaHeadlineList, enumFieldID))
			bReturnValue = TRUE;
	}
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
	return bReturnValue;
}
// -------------------------------------------------------------------------
void CDCache::SetPageNo(const char* szString, const char* szOrgString, long lPageNo)
{
	CObject* TempObj		= NULL;
	long*	lPhysicalPageNo	= NULL;
	long*	lNewPhyPageNo   = NULL;
	CString	csPageName		= szString;
	CString	csOrgPageName	= szOrgString;
	BOOL	bNewPage		= TRUE;

	m_CriticalSection[0].Enter();
	if (szOrgString!=NULL)
		if (m_mapPagesName.Lookup(csOrgPageName.GetBuffer(0), TempObj))
		{
			lPhysicalPageNo	 = (long*) TempObj;
			m_mapPagesName.RemoveKey(csOrgPageName.GetBuffer(0));
			delete lPhysicalPageNo;
		}

	if (m_mapPagesName.Lookup(csPageName.GetBuffer(0), TempObj))
	{
		lPhysicalPageNo = (long*) TempObj;
		if (*lPhysicalPageNo != lPageNo)
		{
			m_CriticalSection[0].Leave();
			// release this CS if not it will cause deadlock
			DropAPage(*lPhysicalPageNo);
			m_CriticalSection[0].Enter();
		}
		else
			bNewPage = FALSE;
	}

	if (bNewPage)
	{
		lNewPhyPageNo = new long;
		*lNewPhyPageNo = lPageNo;
		m_mapPagesName.SetAt(csPageName.GetBuffer(0), (CObject*)lNewPhyPageNo) ;
	}
	m_CriticalSection[0].Leave();
}
// -------------------------------------------------------------------------


BOOL CDCache::LogDataToFile(CString strFileName)
{
	BOOL bReturnValue = FALSE;
	char * buf = new char[1024*16];
	FILE* data_fp = NULL;
	CDWordArray	cdwaPageList;
	
#if _MSC_VER > 1500
	if ((fopen_s(&data_fp, strFileName.GetBuffer(0), "w+b")) == 0)
		//setvbuf(data_fp, buf, _IOFBF, sizeof(buf));
		setvbuf(data_fp, buf, _IOFBF, 1024*16);

#else
	if ((data_fp=fopen(strFileName.GetBuffer(0), "w+b")) != NULL)	
		setbuf(data_fp, buf);

#endif
	

	if (data_fp!=NULL)		
	{
		if (GetPageList(cdwaPageList))
		{
			do 
			{
				if (cdwaPageList.GetSize()>0)
				{
					long lPageNo = cdwaPageList.GetAt(0);
					cdwaPageList.RemoveAt(0);

					COP_ITEM copReturnPage;
					char cCheckSum = 0;
					size_t WriteSize;

					CDWordArray cdwaFIDList;
					fidDatabase fDB; 
					if (GetFIDList(lPageNo, cdwaFIDList))
					{
						for (int i=0; i<cdwaFIDList.GetSize();i++)
						{
							BOOL bLog = FALSE;
							switch (fDB.GetFidType((short) cdwaFIDList.GetAt(i)))
							{
								case NGN_YHEADLINE:
								case NGN_HEADLINE:
									{
										CDWordArray cdwaHeadlineList;
										if (GetHeadlineList(lPageNo, cdwaHeadlineList, (FID_DEFINITION)cdwaFIDList.GetAt(i)))
										{
											for (int iLine=0; iLine< cdwaHeadlineList.GetSize(); iLine++)
											{
												long lTemp = cdwaHeadlineList.GetAt(iLine);
												if (GetAField(lPageNo, copReturnPage, (FID_DEFINITION)cdwaFIDList.GetAt(i), lTemp))
												{
													copReturnPage->GetMessage();
													short sMsgLen = copReturnPage->GetMsgLen();
													fseek(data_fp, 0, SEEK_END);
													WriteSize = fwrite( &sMsgLen , 1, 2, data_fp);
													WriteSize = fwrite( copReturnPage->GetMessage(), 1, sMsgLen, data_fp);
												}
											}
										}
									}
									break;
								case NGN_QUOTELIST:
									{
										QuoteList qlistTemp,qlist;
										if (GetAField(lPageNo, qlistTemp, (FID_DEFINITION)cdwaFIDList.GetAt(i),-1))
										{
											qlist.Init(qlistTemp.GetData());
											byte yLn, yLen;
											short sFid;
											void* pFV;;

											for (int iElement=0; iElement<qlist.GetNE(); iElement++)
											{
												bool bQuoteUpdate = false;
												QuoteList qlist2;
												if (qlist.GetQuote(iElement, yLn, sFid, yLen, pFV))
												{
													qlist2.SetTL(qlist.GetTL());
													if( qlist2.AddQuote(yLn, sFid, yLen, pFV) )
														bQuoteUpdate = true;
												}

												if( bQuoteUpdate ) {
													COP_ITEM cop = new MsgObject;
													cop()->SetItemNo(lPageNo);
													cop()->AddFid((FID_DEFINITION)cdwaFIDList.GetAt(i), &qlist2);
													cop()->GetMessage();

													short sMsgLen = cop->GetMsgLen();
													fseek(data_fp, 0, SEEK_END);
													WriteSize = fwrite( &sMsgLen , 1, 2, data_fp);
													ASSERT(WriteSize!=0);
													WriteSize = fwrite( cop->GetMessage(), 1, sMsgLen, data_fp);
													ASSERT(WriteSize!=0);
												}
											}
										}
									}
									break;
								default:
									long lTemp = -1;
									if (GetAField(lPageNo, copReturnPage, (FID_DEFINITION)cdwaFIDList.GetAt(i), (long&) lTemp))
									{
										copReturnPage->GetMessage();
										short sMsgLen = copReturnPage->GetMsgLen();
										fseek(data_fp, 0, SEEK_END);
										WriteSize = fwrite( &sMsgLen , 1, 2, data_fp);
										WriteSize = fwrite( copReturnPage->GetMessage(), 1, sMsgLen, data_fp);
									}								
									break;
							}

						}
					}

				}
			} while (cdwaPageList.GetSize()>0);
		}
	}

	if (cdwaPageList.GetSize()==0)
	{
		char strLastMessage[] = "DCACHE_COMPLETED";
		short messlen = 16;
		fwrite(&messlen, 1, 2, data_fp);
		fwrite( &strLastMessage, 1, 16, data_fp);
		fflush(data_fp);
		fclose(data_fp);
		bReturnValue = TRUE;
	}
	delete []buf;
	return bReturnValue;
}	
// -------------------------------------------------------------------------
void CDCache::RetrieveDataByTCP(CString host, long port)
{
	m_source->Start(host.GetBuffer(0), port, this);
}

// -------------------------------------------------------------------------
BOOL CDCache::CheckTCPConnectionAlive()
{
	return m_source->CheckAlive();
}

bool CDCache::EnableDataCompression(bool bEnable)
{
	bool bC = g_bCompression;
	g_bCompression = bEnable;
	return bC;
}

BOOL CDCache::TryEnterCriticalSection(long lPageNo)
{
	//return m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].TryEnter();
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Enter();
	return TRUE;
}

void CDCache::LeaveCriticalSection(long lPageNo)
{
	m_CriticalSection[(unsigned long)lPageNo%CS_HASH_NUM+1].Leave();
}


// -------------------------------------------------------------------------
extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("DCACHE.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(extensionDLL, hInstance))
			return 0;

		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		new CDynLinkLibrary(extensionDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("DCACHE.DLL Terminating!\n");
		// Terminate the library before destructors are called
		AfxTermExtensionModule(extensionDLL);
	}
	return 1;   // ok
}
// -------------------------------------------------------------------------
extern "C" void WINAPI InitDCacheDLL()
{
	// create a new CDynLinkLibrary for this app
	new CDynLinkLibrary(extensionDLL);
	// nothing more to do
}








