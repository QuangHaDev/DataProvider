// Recovery.cpp: implementation of the CRecovery class.
//
//////////////////////////////////////////////////////////////////////
#define _CRT_SECURE_NO_WARNINGS			1

#include "afx.h"
#include "Recovery.h"
#include "limits.h"
#include <fstream>
#include <iomanip>
using namespace std;

#define RECOVER_QUEUE_SIZE				m_nRecoveryWindowSize

RecoverBuffer::RecoverBuffer()
    : m_nLen(0)
{
    memset(m_szBuffer, 0, sizeof(m_szBuffer));
	//m_pTTL = new int;
}

RecoverBuffer::~RecoverBuffer()
{
/*	if (m_pTTL != NULL)
	{
		delete m_pTTL;
		m_pTTL = NULL;
	}
*/
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRecovery::CRecovery(bool bLog, int nRecoveryWindowSize)
    : m_ulCurSeq(ULONG_MAX), 
      m_ulOutSeq(0), 
	  m_ulOutSeqLast(0),
      m_ulRecover(0), 
      m_bLog(bLog),
      m_bFull(false),
      m_nRecoveryWindowSize(nRecoveryWindowSize)
{
//    CApplManClient::SetTraceOn();
}

CRecovery::~CRecovery()
{
    m_setRecover.clear();
}

int CRecovery::CheckSeq(unsigned long ulSeq, int nLen)
{
    int nRc;
    SYSTEMTIME systime;
    if (m_nRecoveryWindowSize <= 0)
        return SEQ_IN;
    if (m_ulCurSeq == ULONG_MAX)
    {
        m_ulCurSeq = ulSeq;
        return SEQ_IN;
    }
    if (ulSeq == m_ulCurSeq+1)
    {
        nRc = SEQ_IN;
        m_ulCurSeq = ulSeq;
        if (m_setRecover.size() > 0)
        {
            if (m_bLog)
            {
                GetLocalTime(&systime);
                char szLogFile[64];
                sprintf(szLogFile, "dctrl_seq_%02d%02d.log", systime.wMonth, systime.wDay);
                ofstream outFile2(szLogFile, ios::out|ios::app);
                outFile2 << systime.wHour << ":" << systime.wMinute << ":" << systime.wSecond;
                outFile2 << " RECOVER    (" << ulSeq << ")  size: " << nLen << endl;
                outFile2.close();
            }
            RECOVERPTR rptr;
            for (rptr = m_setRecover.begin(); rptr != m_setRecover.end(); rptr++)
            {
                if (rptr->m_ulSeqNo == m_ulCurSeq)
                {
                    m_setRecover.erase(rptr);
                    break;
                }
            }
        }
    }
    else if (ulSeq == m_ulCurSeq)
        nRc = SEQ_EQUAL;
    else
    {
        if (m_bLog)
        {
            GetLocalTime(&systime);
            char szLogFile[64];
            sprintf(szLogFile, "dctrl_seq_%02d%02d.log", systime.wMonth, systime.wDay);
            ofstream outFile2(szLogFile, ios::out|ios::app);
            outFile2 << systime.wHour << ":" << systime.wMinute << ":" << systime.wSecond;
            outFile2 << " out of seq (" << ulSeq << ")  expected (" << m_ulCurSeq+1 << ")" << endl;
            outFile2.close();
        }
        nRc = SEQ_OUT;
    }
    return nRc;
}

int CRecovery::AddData(unsigned long ulSeq, const unsigned char *data, int nLen)
{
    SYSTEMTIME systime;
    int nDiff = m_ulCurSeq - m_nRecoveryWindowSize;
    if (nDiff > 0)
    {
        if (ulSeq > nDiff && ulSeq <= m_ulCurSeq)
        {
            if (m_bLog)
            {
                GetLocalTime(&systime);
                char szLogFile[64];
                sprintf(szLogFile, "dctrl_seq_%02d%02d.log", systime.wMonth, systime.wDay);
                ofstream outFile2(szLogFile, ios::out|ios::app);
                outFile2 << systime.wHour << ":" << systime.wMinute << ":" << systime.wSecond;
                outFile2 << " Ignore packet within window size (" << ulSeq << ")" << endl;
                outFile2.close();
            }
            return WITHIN_WINDOWSIZE;
        }
    }
    m_bFull = false;
    RecoverBuffer recbuf;
    recbuf.m_ulSeqNo = ulSeq;
    memcpy(recbuf.m_szBuffer, data, nLen);
    recbuf.m_nLen = nLen;
    recbuf.m_nTTL = m_nRecoveryWindowSize;
//	*(recbuf.m_pTTL) = m_nRecoveryWindowSize;
    m_setRecover.insert(recbuf);
    if (m_setRecover.size() >= RECOVER_QUEUE_SIZE)
        return QUEUE_READY;
    return QUEUE_NOT_FULL;
}

bool CRecovery::GetData(unsigned char *data, int &nLen)
{
    bool bRc = false;
    if (m_setRecover.size() > 0)
    {
        RECOVERPTR rptr;
        int i = 0;
        for (rptr = m_setRecover.begin(); rptr != m_setRecover.end(); rptr++)
        {
            i++;
            if (rptr->m_ulSeqNo == m_ulCurSeq + 1)
            {
                if (!m_bFull)
                    m_ulRecover++;
                if (m_bLog)
                {
                    SYSTEMTIME systime;
                    GetLocalTime(&systime);
                    char szLogFile[64];
                    sprintf(szLogFile, "dctrl_seq_%02d%02d.log", systime.wMonth, systime.wDay);
                    ofstream outFile2(szLogFile, ios::out|ios::app);
                    outFile2 << systime.wHour << ":" << systime.wMinute << ":" << systime.wSecond;
                    outFile2 << (m_bFull?" FULL GET   (":" get        (") << rptr->m_ulSeqNo << ")  " 
                             << "size: " << rptr->m_nLen << endl;
                    outFile2.close();
                }
                memcpy(data, rptr->m_szBuffer, rptr->m_nLen);
                nLen = rptr->m_nLen;
                m_ulCurSeq = rptr->m_ulSeqNo;
                m_setRecover.erase(rptr);
                bRc = true;
                break;
            }
            if (rptr->m_ulSeqNo > m_ulCurSeq + 1)
                break;
        }
        if (m_setRecover.size() >= RECOVER_QUEUE_SIZE)
        {
            m_bFull = true;
            if (m_bLog)
            {
                SYSTEMTIME systime;
                GetLocalTime(&systime);
                char szLogFile[64];
                sprintf(szLogFile, "dctrl_seq_%02d%02d.log", systime.wMonth, systime.wDay);
                ofstream outFile2(szLogFile, ios::out|ios::app);
                outFile2 << systime.wHour << ":" << systime.wMinute << ":" << systime.wSecond;
                outFile2 << " Queue Full Check"<< endl;
                outFile2.close();
            }
            if (i > RECOVER_QUEUE_SIZE / 2)
                m_setRecover.erase(rptr, m_setRecover.end());
            else
                m_setRecover.erase(m_setRecover.begin(), rptr);
            if (m_setRecover.size() > 0)
            {
                rptr = m_setRecover.begin();
                if (m_bLog)
                {
                    SYSTEMTIME systime;
                    GetLocalTime(&systime);
                    char szLogFile[64];
                    sprintf(szLogFile, "dctrl_seq_%02d%02d.log", systime.wMonth, systime.wDay);
                    ofstream outFile2(szLogFile, ios::out|ios::app);
                    outFile2 << systime.wHour << ":" << systime.wMinute << ":" << systime.wSecond;
                    outFile2 << " queue full get (" << rptr->m_ulSeqNo << ")" << endl;
                    outFile2.close();
                }
                memcpy(data, rptr->m_szBuffer, rptr->m_nLen);
                nLen = rptr->m_nLen;
                m_ulOutSeq += (rptr->m_ulSeqNo - m_ulCurSeq - 1);
                m_ulCurSeq = rptr->m_ulSeqNo;
                m_setRecover.erase(rptr);
                bRc = true;
            }
        }
    }
    return bRc;
}

void CRecovery::TTLCheck()
{
    RECOVERPTR p, d;
    d = m_setRecover.end();
	int *pTTL;
    for (p = m_setRecover.begin(); p != m_setRecover.end();)
    {
//        p->m_nTTL--;
		RecoverBuffer recbuf;
		recbuf.m_ulSeqNo = p->m_ulSeqNo;
		memcpy(recbuf.m_szBuffer, p->m_szBuffer, p->m_nLen);
		recbuf.m_nLen = p->m_nLen;
		recbuf.m_nTTL = p->m_nTTL - 1;
		m_setRecover.erase(p);
		m_setRecover.insert(recbuf);
		p = m_setRecover.find(recbuf);

//		pTTL = p->m_pTTL;
//		(*pTTL)--;
        if (p->m_nTTL <= 0)
//		if (*(p->m_pTTL) <= 0)
        {
            d = p;
            if (m_bLog)
            {
                SYSTEMTIME systime;
                GetLocalTime(&systime);
                char szLogFile[64];
                sprintf(szLogFile, "dctrl_seq_%02d%02d.log", systime.wMonth, systime.wDay);
                ofstream outFile2(szLogFile, ios::out|ios::app);
                outFile2 << systime.wHour << ":" << systime.wMinute << ":" << systime.wSecond;
                outFile2 << " Packet TTL 0 and Remove from window (" << (*p).m_ulSeqNo << ")" << endl;
                outFile2.close();
            }
        }
        if (d != m_setRecover.end())
        {
            p++;
            m_setRecover.erase(d);
            d = m_setRecover.end();
            continue;
        }
        p++;
    }
}
