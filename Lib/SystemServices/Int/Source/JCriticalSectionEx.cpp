// JLock.cpp: implementation of the CJLock class.
//
//////////////////////////////////////////////////////////////////////
#include "JCriticalSectionEx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CJCriticalSectionEx::CJCriticalSectionEx()
{
	m_bIsEntered = false;
}

CJCriticalSectionEx::~CJCriticalSectionEx()
{
    m_bIsEntered = false;
}

void CJCriticalSectionEx::Enter()
{
	m_cs.Enter();
	m_bIsEntered = true;
}

bool CJCriticalSectionEx::TryEnter()
{
	bool bSuccess = (bool)m_cs.TryEnter();
	if( bSuccess )
	{
		m_bIsEntered = true;
	}
	else
	{
		m_bIsEntered = false;
	}
	return bSuccess;
}

void CJCriticalSectionEx::Leave()
{
	m_cs.Leave();
	m_bIsEntered = false;
}

bool CJCriticalSectionEx::IsEntered()
{
	return m_bIsEntered;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CJCriticalSectionEx::Lock::Lock(CJCriticalSectionEx &jcs) :
m_jcs(jcs)
{
	m_jcs.Enter();
}

CJCriticalSectionEx::Lock::~Lock()
{
    m_jcs.Leave();
}

bool CJCriticalSectionEx::Lock::TryLock()
{
	return m_jcs.TryEnter();
}

