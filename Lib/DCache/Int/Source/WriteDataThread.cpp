/////////////////////////////////////////////////////////////////////////////
//          File:: WriteDataThread.cpp
// Create Author:: Raymond Yung
// Create   Date:: 1 September 00
//
//   Description:: CWriteDataThread Class member function
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
#define _CRT_SECURE_NO_WARNINGS			1

#include "DCache.h"
#include "WriteDataThread.h"
#include "ParsedFidIterator.h"
// -------------------------------------------------------------------------
// Default Constructor
// -------------------------------------------------------------------------

CWriteDataThread::CWriteDataThread(int iTimeToClearDisk, int iTimeToClearPage, int iTimeToWriteToDisk, int iTimeToWrite, 
		CString FileName, int iTimeToWriteFidToDisk, int iTimeToWriteFid, void* arg) : 
		ssThread("WriteDataThread"),
		m_DataFileName(FileName), m_data_fp(NULL), m_data2_fp(NULL), 
		m_ClearDataTimer(iTimeToClearDisk*1000), m_Timer3(iTimeToClearPage*1000), 
		m_WriteToDiskTimer(iTimeToWriteToDisk*1000), m_Timer(iTimeToWrite*1000),
		m_WriteFIDTimer(iTimeToWriteFidToDisk*1000), m_Timer2(iTimeToWriteFid*1000), 
		m_bWritingProgressFlag(FALSE), m_bWritingFIDProgressFlag(FALSE), m_bClearDiskFlag(FALSE), m_bStopThread(FALSE)
{
		m_ctToday = CTime::GetCurrentTime();
		ssThread::Start();
		pMyself = (CDCache*) arg;
}

// -------------------------------------------------------------------------
// Destructor
CWriteDataThread::~CWriteDataThread()
{
	if (!IsExit())
	{
		ssThread::ExitThread();
	}
}

// -------------------------------------------------------------------------
void CWriteDataThread::StartToWritePage()		
{
	pMyself->GetPageList(m_cdwaPageList);
	m_bWritingProgressFlag = TRUE;
}
// -------------------------------------------------------------------------
void CWriteDataThread::StartToWriteFid()		
{
	pMyself->GetPageList(m_cdwaPageListForWriteFid);
	m_bWritingFIDProgressFlag = TRUE;
}

// -------------------------------------------------------------------------
void CWriteDataThread::Main()
{
	ssMultiWait multiWait;
	multiWait.Add(m_Timer, TimerEvent);
	multiWait.Add(m_Timer2, Timer2Event);
	multiWait.Add(m_Timer3, Timer3Event);
	multiWait.Add(m_ClearDataTimer, ClearDataEvent);
	multiWait.Add(m_WriteToDiskTimer, WriteToDiskEvent);
	multiWait.Add(m_WriteFIDTimer, WriteFIDEvent);
	multiWait.Add(m_ExitEvent, ExitEvent);

	while (!IsExit())
	{
		switch (multiWait.Wait(2000))
		{
			case TimerEvent:
				{
					if (!m_bStopThread)
						ProcessWriteToDisk();
				}
				break;
			case Timer2Event:
				{
					if (!m_bStopThread)
						ProcessWriteFIDToDisk();
				}
				break;
			case Timer3Event:
				{
					if (!m_bStopThread)
						ProcessClearData();
				}
				break;
			case ExitEvent :
				{
					if (m_data_fp!=NULL)		
					{
						fflush(m_data_fp);
						fclose(m_data_fp);
					}
					if (m_data2_fp!=NULL)		
					{
						fflush(m_data2_fp);
						fclose(m_data2_fp);
					}
				}
				break;
			case ClearDataEvent : 
				{
					if (!m_bStopThread)
					{
						if (!m_bClearDiskFlag)
						{
							CTime Today = CTime::GetCurrentTime();
							if (Today.GetHour()==12)
							{
								if (Today.GetDay() != m_ctToday.GetDay())
								{									
									m_ctToday = CTime::GetCurrentTime();
									if (pMyself->GetPageList(m_cdwaClearPageList))
										m_bClearDiskFlag = TRUE;
								}
							}
						}
					}
				}
				break;
			case WriteToDiskEvent :
				{
					if (!m_bStopThread)
						if (!m_bWritingProgressFlag)
							if (pMyself->GetPageList(m_cdwaPageList))
								m_bWritingProgressFlag = TRUE;
				}
				break;
			case WriteFIDEvent :
				{
					if (!m_bStopThread)
						if (!m_bWritingFIDProgressFlag)
							if (pMyself->GetPageList(m_cdwaPageListForWriteFid))
								m_bWritingFIDProgressFlag = TRUE;
				}
				break;
			default :
			{
				if (multiWait.GetResult() != WAIT_IO_COMPLETION)
				{
				}
				break;
			}
		}
	}
	return;
}


// -------------------------------------------------------------------------

void CWriteDataThread::ProcessWriteToDisk()
{

	if (m_bWritingProgressFlag)
	{
		try {
			m_csWritePages.Enter();

			if (m_data_fp==NULL)		
			{
				CString TempFileName = m_DataFileName +".tmp" ;
				if ((fopen_s(&m_data_fp, TempFileName.GetBuffer(0), "w+b")) != 0) // fopen_s return 0 if ok
				{
					//Error Message
				}
				else
					setvbuf(m_data_fp, m_buf, _IOFBF, sizeof(m_buf));
					//setbuf(m_data_fp, m_buf);
			}
			long lPageNo = 0;
			if (m_data_fp!=NULL)		
			{
				int  iCounter = 1;
				unsigned char cWritePageFlag;
				do 
				{
					cWritePageFlag = 0;

					if (m_cdwaPageList.GetSize()>0)
					{
						lPageNo = m_cdwaPageList.GetAt(0);
						m_cdwaPageList.RemoveAt(0);
						if (pMyself->GetAField(lPageNo, cWritePageFlag, FID_WRT_DSK, 0))
							if (cWritePageFlag==1)
							{
								COP_ITEM copReturnPage;
								CDWordArray cdwaFIDList;
								if (pMyself->GetFIDList(lPageNo, cdwaFIDList))
								{
									int iTotalFID = cdwaFIDList.GetSize();
									for(int iCounter=0;iCounter<iTotalFID;iCounter++)
									{
										fidDatabase fDB;
										if (fDB.GetFidType((FID_DEFINITION)cdwaFIDList.GetAt(iCounter)) == NGN_QUOTELIST )
											ProcessQuoteListData(lPageNo, (FID_DEFINITION)cdwaFIDList.GetAt(iCounter));
										else 
										{
											long lTemp = -1;
											if (pMyself->GetAField(lPageNo, copReturnPage, (FID_DEFINITION)cdwaFIDList.GetAt(iCounter), lTemp))
												WriteDataToFile(copReturnPage);
										}
									}
								}
							}
					}
				} while ((iCounter++<MAX_PAGE_IN_EACH_TIME) && (m_cdwaPageList.GetSize()>0));
			}

			if (m_cdwaPageList.GetSize()==0)
			{
				char strLastMessage[] = "DCACHE_COMPLETED";
				short messlen = 16;
				fwrite(&messlen, 1, 2, m_data_fp);
				fwrite( &strLastMessage, 1, 16, m_data_fp);
				fflush(m_data_fp);
				fclose(m_data_fp);
				m_data_fp = NULL;
				m_bWritingProgressFlag = FALSE;
				CString TempFileName = m_DataFileName +".tmp" ;
				CString NewFileName  = m_DataFileName +".dat" ;
				CString OldFileName  = m_DataFileName +".old" ;
				if (::CopyFile(NewFileName, OldFileName,  FALSE))
					::DeleteFile(NewFileName);
				if (::CopyFile(TempFileName, NewFileName,  FALSE))
					::DeleteFile(TempFileName);
			}
			m_csWritePages.Leave();
		} catch(...) {
			m_csWritePages.Leave();
			char szFaultMsg[MAX_LEN];
			sprintf(szFaultMsg, "(DCACHE) CWriteDataThread - ProcessWriteToDisk Failed\n ");
			if(pMyself)
				pMyself->LogFault((char*)&szFaultMsg);
		}
		//m_csWritePages.Leave();
	}
}
// -------------------------------------------------------------------------
void CWriteDataThread::ProcessQuoteListData(long lPageNo, FID_DEFINITION sCurFid)
{
	long lTemp = -1;
	QuoteList qlistTemp,qlist;
	if (pMyself->GetAField(lPageNo, qlistTemp, (FID_DEFINITION)sCurFid,lTemp))
	{
		qlist.Init(qlistTemp.GetData());
		byte yLn, yLen;
		short sFid;
		void* pFV;;


		for (int iElement=0; iElement<qlist.GetNE(); iElement++)
		{
			QuoteList qlist2;
			if (qlist.GetQuote(iElement, yLn, sFid, yLen, pFV))
			{
				qlist2.SetTL(qlist.GetTL());
				if( qlist2.AddQuote(yLn, sFid, yLen, pFV) )
				{
					COP_ITEM cop = new MsgObject;
					cop()->SetItemNo(lPageNo);
					cop()->AddFid((FID_DEFINITION)sCurFid, &qlist2);
					
					WriteDataToFile(cop);
				}
			}
		}
	}
}

// -------------------------------------------------------------------------
void CWriteDataThread::WriteDataToFile(COP_ITEM cop)
{
	try {
		size_t WriteSize;

		cop()->GetMessage();
		short sMsgLen = cop->GetMsgLen();
		fseek(m_data_fp, 0, SEEK_END);
		WriteSize = fwrite( &sMsgLen , 1, 2, m_data_fp);
		ASSERT(WriteSize!=0);
		WriteSize = fwrite( cop->GetMessage(), 1, sMsgLen, m_data_fp);
		ASSERT(WriteSize!=0);
	} catch(...) {
		char szFaultMsg[MAX_LEN];
		sprintf(szFaultMsg, "(DCACHE) CWriteDataThread - WriteDataToFile Failed\n ");
		if(pMyself)
			pMyself->LogFault((char*)&szFaultMsg);
	}
}

// -------------------------------------------------------------------------

void CWriteDataThread::ProcessWriteFIDToDisk()
{
	if (m_bWritingFIDProgressFlag)
	{
		try {
			m_csWriteFids.Enter();
			if (pMyself->m_cwaWriteFIDList.GetUpperBound()>-1)
			{
				if (m_data2_fp==NULL)		
				{
					CString TempFileName = m_DataFileName +"_.tmp" ;
					if ((m_data2_fp=fopen(TempFileName.GetBuffer(0), "w+b")) == NULL)
					{
						//Error Message
					}
					else
						setbuf(m_data2_fp, m_buf2);
				}
				long lPageNo = 0;
				if (m_data2_fp!=NULL)		
				{
					if (m_cdwaPageListForWriteFid.GetSize()>0)
					{
						COP_ITEM copReturnPage;
						size_t WriteSize;
						lPageNo = m_cdwaPageListForWriteFid.GetAt(0);
						m_cdwaPageListForWriteFid.RemoveAt(0);

						int iTotalFID = pMyself->m_cwaWriteFIDList.GetSize();
						for(int iCounter=0;iCounter<iTotalFID;iCounter++)
						{
							long lTemp = -1;
							if (pMyself->GetAField(lPageNo, copReturnPage, (FID_DEFINITION)pMyself->m_cwaWriteFIDList.GetAt(iCounter), lTemp))
							{
								copReturnPage->GetMessage();
								short sMsgLen = copReturnPage->GetMsgLen();
								fseek(m_data2_fp, 0, SEEK_END);
								WriteSize = fwrite( &sMsgLen , 1, 2, m_data2_fp);
								WriteSize = fwrite( copReturnPage->GetMessage(), 1, sMsgLen, m_data2_fp);
							}
						}
					}
				}

				if (m_cdwaPageListForWriteFid.GetSize()==0)
				{
					char strLastMessage[] = "DCACHE_COMPLETED";
					short messlen = 16;
					fwrite(&messlen, 1, 2, m_data2_fp);
					fwrite( &strLastMessage, 1, 16, m_data2_fp);
					fflush(m_data2_fp);
					fclose(m_data2_fp);
					m_data2_fp = NULL;
					m_bWritingFIDProgressFlag = FALSE;

					CString TempFileName = m_DataFileName +"_.tmp" ;
					CString NewFileName = m_DataFileName +"_.dat" ;
					CString OldFileName = m_DataFileName +"_.old" ;
					if (::CopyFile(NewFileName, OldFileName,  FALSE))
						::DeleteFile(NewFileName);
					if (::CopyFile(TempFileName, NewFileName,  FALSE))
						::DeleteFile(TempFileName);
				}
			}
			m_csWriteFids.Leave();
		} catch(...) {			\
			m_csWriteFids.Leave();
			char szFaultMsg[MAX_LEN];
			sprintf(szFaultMsg, "(DCACHE) CWriteDataThread - ProcessWriteFIDToDisk Failed\n ");
			if(pMyself)
				pMyself->LogFault((char*)&szFaultMsg);
		}
	}

}

// -------------------------------------------------------------------------
void CWriteDataThread::ProcessClearData()
{
	if (m_bClearDiskFlag)
	{
		try {
			int  iCounter = 1;
			unsigned char cDaysToLive;
			do 
			{
				cDaysToLive = 0;

				if (m_cdwaClearPageList.GetSize()>0)
				{
					long lPageNo = m_cdwaClearPageList.GetAt(0);
					m_cdwaClearPageList.RemoveAt(0);
					if (pMyself->GetAField(lPageNo, cDaysToLive, FID_DTL, 0))
					{
						if (cDaysToLive==1)
						{
							pMyself->DropAPage(lPageNo);
						}
						else if (cDaysToLive>1)
						{
							MsgObject* MsgObj = new MsgObject();
							cDaysToLive--;

							MsgObj->AddFid(FID_DTL, &cDaysToLive);
							MsgObj->SetMsgType(MGT_VERIFYSYNC);
							MsgObj->SetItemNo(lPageNo);
							COP_ITEM copReturnPage = new MsgObject(MsgObj->GetMessage()); 
							pMyself->PutDataIntoDCache(copReturnPage);
							delete MsgObj;						
						}
					}
				}
				else
					m_bClearDiskFlag = FALSE;
			} while ((iCounter++<MAX_PAGE_IN_EACH_TIME) && (m_cdwaClearPageList.GetSize()>0));
		} catch(...) {
			char szFaultMsg[MAX_LEN];
			sprintf(szFaultMsg, "(DCACHE) CWriteDataThread - ProcessClearData Failed\n ");
			if(pMyself)
				pMyself->LogFault((char*)&szFaultMsg);
		}
	}

}



