// Source.cpp: implementation of the CSource class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DCache.h"
#include "Source.h"
#include <process.h>
#include <sstream>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSource::CSource() :
m_nRecv(0)
{
	m_bAlive = FALSE;
    m_socket.Init();
    m_hExit = CreateEvent(NULL, TRUE, FALSE, NULL);
}

CSource::~CSource()
{
    Stop();
    m_socket.Destroy();
}

void CSource::Start(string host, long port, CDCache* pMyself)
{
    UINT nID;
    ResetEvent(m_hExit);
    m_strHost = host;
    m_nPort = port;
	m_DCache = (CDCache*)pMyself;
    m_hThread = (HANDLE)_beginthreadex(NULL,1,ThreadFunc,this,1, &nID);
}

void CSource::Stop()
{
    SetEvent(m_hExit);
    m_socket.Close();
    WaitForSingleObject(m_hThread, INFINITE);
}

UINT CSource::ThreadFunc(LPVOID lpVoid)
{
    ((CSource*)lpVoid)->Main();
    return 0;
}

void CSource::Main()
{
    ostringstream ostr;
	if (m_socket.Open(m_strHost, m_nPort))
    {
		char szBuffer[32767];
		char * szBuffer3= new char[65535*10];
		short sMsgLen = 0;
		int nRecv	= 0;
		int m_nRecv = 0;
		BOOL bCompleted = TRUE;
		nRecv = m_socket.Recieve(szBuffer, sizeof(szBuffer));
		while (nRecv > 0)
		{
			m_bAlive = TRUE;
			memcpy(szBuffer3+m_nRecv, szBuffer, nRecv);
			m_nRecv += nRecv;
			if (bCompleted)
				memcpy(&sMsgLen, szBuffer3, 2);
			if (m_nRecv >= sMsgLen+2)
			{ 
				while (m_nRecv >= sMsgLen+2)
				{
					COP_ITEM copMsg = new BUFFER_ITEM((const unsigned char *)szBuffer3+2);
					m_DCache->PutDataIntoDCache(copMsg);

					m_nRecv -= sMsgLen+2;
					memmove(szBuffer3, szBuffer3+sMsgLen+2, m_nRecv);
					if (m_nRecv>2)
						memcpy(&sMsgLen, szBuffer3, 2);
					else
						bCompleted = TRUE;
				}
			}
			else
			{
				bCompleted = FALSE;
			}
			memset(szBuffer, 0, sizeof(szBuffer));
			nRecv = m_socket.Recieve(szBuffer, sizeof(szBuffer));
		}
		delete[] szBuffer3;
		m_bAlive = FALSE;
	}
	m_socket.Close();
}

