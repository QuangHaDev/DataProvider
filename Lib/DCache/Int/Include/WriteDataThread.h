/////////////////////////////////////////////////////////////////////////////
//          File:: WriteDataThread.h
// Create Author:: Raymond Yung
// Create   Date:: 1 September 00
//
//   Description:: CWriteDataThread Class Declaration
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
#ifndef __WriteDataThread_H__
#define __WriteDataThread_H__

#include "ssAll.h"
#include "MsgObject.h"
using namespace NGN2;

#define MAX_PAGE_IN_EACH_TIME	20
#define BUFFER_SIZE				1024*16

class CDCache;
class CWriteDataThread : public ssThread
{
public:
	// -------------------------------------------------------------------------
	// Default Constructor
	CWriteDataThread(int iTimeToClearDisk,	int iTimeToClearPage,		int iTimeToWriteToDisk, int iTimeToWrite, 
					 CString FileName,		int iTimeToWriteFidToDisk,	int iTimeToWriteFid,	void* arg);

	// -------------------------------------------------------------------------
	// Destructor
	~CWriteDataThread();

	virtual void Main();
	BOOL GetProgress() {return (m_bWritingProgressFlag||m_bWritingFIDProgressFlag);}
	BOOL GetWritePageProgress() {return m_bWritingProgressFlag;}
	BOOL GetWriteFidProgress()	{return m_bWritingFIDProgressFlag;}
	int  GetNoOfPagesRemain()	{return m_cdwaPageList.GetSize();}
	int  GetNoOfFidsRemain()	{return m_cdwaPageListForWriteFid.GetSize();}
	void Stop()					{m_bStopThread=TRUE;}
	void Start()				{m_bStopThread=FALSE;}
	void StartToWritePage();
	void StartToWriteFid();

	void ProcessQuoteListData(long lPageNo, FID_DEFINITION sFid);
	void WriteDataToFile(COP_ITEM cop);
	void ProcessWriteToDisk();
	void ProcessWriteFIDToDisk();

private:
	void ProcessClearData();

private:
	enum
	{
		ClearDataEvent   = 10,
		Timer3Event		 = 11,
		WriteToDiskEvent = 14,
		TimerEvent		 = 15,
		ExitEvent		 = 30,
		WriteFIDEvent	 = 40,
		Timer2Event		 = 50
	};
	char				m_buf[BUFFER_SIZE];
	char				m_buf2[BUFFER_SIZE];
	CTime				m_ctToday;
	FILE*				m_data_fp;
	FILE*				m_data2_fp;
	BOOL				m_bWritingProgressFlag;
	BOOL				m_bWritingFIDProgressFlag;
	BOOL				m_bClearDiskFlag;
	BOOL				m_bStopThread;
	CString				m_DataFileName;
	ssTimer				m_Timer;
	ssTimer				m_Timer2;
	ssTimer				m_Timer3;
	ssTimer				m_ClearDataTimer;
	ssTimer				m_WriteToDiskTimer;
	ssTimer				m_WriteFIDTimer;
	CDCache*			pMyself;
	CDWordArray			m_cdwaPageList;
	CDWordArray			m_cdwaPageListForWriteFid;
	CDWordArray			m_cdwaClearPageList;
	ssCriticalSection	m_csWritePages;		   
	ssCriticalSection	m_csWriteFids;		   
};


#endif