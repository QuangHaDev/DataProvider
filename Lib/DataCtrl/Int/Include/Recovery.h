// Recovery.h: interface for the CRecovery class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECOVERY_H__637E207F_B81A_455F_B214_ED62BD0FFE7B__INCLUDED_)
#define AFX_RECOVERY_H__637E207F_B81A_455F_B214_ED62BD0FFE7B__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#pragma warning(disable: 4786)
#include <set>
using namespace std;

class RecoverBuffer
{
public:
    RecoverBuffer();
    ~RecoverBuffer();

    unsigned long m_ulSeqNo;
    unsigned char m_szBuffer[4096];
    int m_nLen;
    int m_nTTL;
//	int *m_pTTL;
};

struct RecoverCompare
{
    bool operator()(const RecoverBuffer& x, const RecoverBuffer& y)
    {
        return x.m_ulSeqNo < y.m_ulSeqNo;
    }
};

class CRecovery  
{
public:
    CRecovery(bool bLog, int nRecoveryWindowSize);
    virtual ~CRecovery();

    int CheckSeq(unsigned long ulSeq, int nLen);
    int AddData(unsigned long ulSeq, const unsigned char *data, int nLen);
    bool GetData(unsigned char *data, int &nLen);
    void TTLCheck();

    typedef set<RecoverBuffer, RecoverCompare> RECOVERSET;
    typedef RECOVERSET::const_iterator RECOVERPTR;
    enum {SEQ_IN, SEQ_OUT, SEQ_EQUAL};
    enum {QUEUE_NOT_FULL, QUEUE_READY, WITHIN_WINDOWSIZE};

    unsigned long m_ulCurSeq;
    RECOVERSET m_setRecover;

    unsigned long m_ulOutSeq;
	unsigned long m_ulOutSeqLast;
    unsigned long m_ulRecover;
    bool m_bLog;
    bool m_bFull;
    int m_nRecoveryWindowSize;

};

#endif // !defined(AFX_RECOVERY_H__637E207F_B81A_455F_B214_ED62BD0FFE7B__INCLUDED_)
