// JLock.h: interface for the CJLock class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JLOCK_H__FD08DBCE_9679_48D2_9DBE_77C540C9EA61__INCLUDED_)
#define AFX_JLOCK_H__FD08DBCE_9679_48D2_9DBE_77C540C9EA61__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CJCriticalSection
{
public:
    class Lock;
	CJCriticalSection();
	virtual ~CJCriticalSection();
    void Enter();
    void Leave();

private:
    CRITICAL_SECTION m_cs;
    
    CJCriticalSection(const CJCriticalSection &rhs);
    CJCriticalSection &operator=(const CJCriticalSection &rhs);
};

class CJCriticalSection::Lock
{
public:
    Lock(CJCriticalSection &jcs);
    ~Lock();
private:
    CJCriticalSection &m_jcs;
    Lock(const Lock &rhs);
    Lock &operator=(const Lock &rhs);
};

#endif // !defined(AFX_JLOCK_H__FD08DBCE_9679_48D2_9DBE_77C540C9EA61__INCLUDED_)
