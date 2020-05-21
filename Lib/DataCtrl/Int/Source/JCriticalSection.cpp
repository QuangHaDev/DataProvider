// JLock.cpp: implementation of the CJLock class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "JCriticalSection.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CJCriticalSection::CJCriticalSection()
{
    ::InitializeCriticalSection(&m_cs);
}

CJCriticalSection::~CJCriticalSection()
{
    ::DeleteCriticalSection(&m_cs);
}

void CJCriticalSection::Enter()
{
    ::EnterCriticalSection(&m_cs);
}

void CJCriticalSection::Leave()
{
    ::LeaveCriticalSection(&m_cs);
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CJCriticalSection::Lock::Lock(CJCriticalSection &jcs) :
m_jcs(jcs)
{
    m_jcs.Enter();
}

CJCriticalSection::Lock::~Lock()
{
    m_jcs.Leave();
}
