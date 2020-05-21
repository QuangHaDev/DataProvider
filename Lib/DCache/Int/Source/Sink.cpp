// Sink.cpp: implementation of the CSink class.
//
//////////////////////////////////////////////////////////////////////
#include <iostream>
#include <afxmt.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "msgobject.h"
#include "Sink.h"
#include "DCache.h"
#include <process.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSink::	CSink(CDCache*	pMyself):
CServerSoc(5)
{
    m_hExit = CreateEvent(NULL, TRUE, FALSE, NULL);
    m_hTimeout = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_pMyself = pMyself;
}

CSink::~CSink()
{
    SetEvent(m_hTimeout);
    SetEvent(m_hExit);
    WaitForSingleObject(m_hListenThread, INFINITE);
}

bool CSink::Start(int nPort)
{
    if (Open(nPort))
    {
        Listen();
        UINT nThreadId;
        m_hListenThread = (HANDLE)_beginthreadex(NULL, 0, ListenThreadFunc, (void *)this, 0, &nThreadId);
        return true;
    }
    return false;
}

UINT CSink::ListenThreadFunc(LPVOID lpData)
{
    ((CSink*)lpData)->ListenFunc();
    return 0;
}
void CSink::ListenFunc()
{
    CSoc soc;
    ULONG ul = 1;
    int nRet;
    char szAddr[16];
    while (WaitForSingleObject(m_hExit, 1000))
    {
        nRet = ioctlsocket(m_socket, FIONBIO, &ul);
        if (Accept(soc, szAddr))
        {
			ULONG ul = 0;
			ioctlsocket(soc.GetSocket(), FIONBIO, &ul);
			CDWordArray	cdwaPageList;
			if (m_pMyself->GetPageList(cdwaPageList))
			{
				BOOL bStop = FALSE;
				do 
				{
					if (bStop)
						break;
					if (cdwaPageList.GetSize()>0)
					{
						long lPageNo = cdwaPageList.GetAt(0);
						cdwaPageList.RemoveAt(0);

						COP_ITEM copReturnPage;

						CDWordArray cdwaFIDList;
						fidDatabase fDB; 
						if (m_pMyself->GetFIDList(lPageNo, cdwaFIDList))
						{
							Sleep(1);
							for (int i=0; i<cdwaFIDList.GetSize();i++)
							{
								if (bStop)
									break;
								switch (fDB.GetFidType((short) cdwaFIDList.GetAt(i)))
								{
									case NGN_YHEADLINE:
									case NGN_HEADLINE:
										{
											CDWordArray cdwaHeadlineList;
											if (m_pMyself->GetHeadlineList(lPageNo, cdwaHeadlineList, (FID_DEFINITION)cdwaFIDList.GetAt(i)))
											{
												for (int iLine=0; iLine< cdwaHeadlineList.GetSize(); iLine++)
												{
													long lTemp = cdwaHeadlineList.GetAt(iLine);
													if (m_pMyself->GetAField(lPageNo, copReturnPage, (FID_DEFINITION)cdwaFIDList.GetAt(i), lTemp))
													{
														copReturnPage->GetMessage();
														short sMsgLen = copReturnPage->GetMsgLen();																						
														int nRet = send(soc.GetSocket(), (const char *)&sMsgLen, 2, 0);
														nRet = send(soc.GetSocket(), (const char *)copReturnPage->GetMessage(), sMsgLen, 0);
														int nError;
														if (nRet == SOCKET_ERROR && (nError = GetLastError()) != WSA_IO_PENDING)
														{
															bStop = TRUE;
															break;
														}
													}
												}
											}
										}
										break;
									default:
										long lTemp = -1;
										if (m_pMyself->GetAField(lPageNo, copReturnPage, (FID_DEFINITION)cdwaFIDList.GetAt(i), (long&) lTemp))
										{
											copReturnPage->GetMessage();
											short sMsgLen = copReturnPage->GetMsgLen();
											int nRet = send(soc.GetSocket(), (const char *)&sMsgLen, 2, 0);
											nRet = send(soc.GetSocket(), (const char *)copReturnPage->GetMessage(), sMsgLen, 0);
											int nError;
											if (nRet == SOCKET_ERROR && (nError = GetLastError()) != WSA_IO_PENDING)
											{
												bStop = TRUE;
												break;
											}
										}								
										break;
								}
							}
						}

					}
				} while (cdwaPageList.GetSize()>0);
			}
			closesocket(soc.GetSocket());
        }
    }
}
