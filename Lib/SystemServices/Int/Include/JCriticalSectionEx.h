// JLock.h: interface for the CJLock class.
//
//////////////////////////////////////////////////////////////////////

#ifndef JCriticalSectionEx_H
#define JCriticalSectionEx_H

#if _MSC_VER > 1000
#pragma once
#endif

#include "ssCriticalSection.h"

class CJCriticalSectionEx
{
public:
    class Lock;
	CJCriticalSectionEx();
	virtual ~CJCriticalSectionEx();
    void Enter();
    void Leave();
	bool TryEnter();
	bool IsEntered();
private:
	ssCriticalSection m_cs;
	bool	m_bIsEntered;
};

class CJCriticalSectionEx::Lock
{
public:
    Lock(CJCriticalSectionEx &jcs);
    virtual ~Lock();
	bool TryLock();
private:
    CJCriticalSectionEx &m_jcs;
};

#endif
