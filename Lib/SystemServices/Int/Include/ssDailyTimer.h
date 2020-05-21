#ifndef _ssDailyTimer_H
#define _ssDailyTimer_H

#if _MSC_VER > 1000
#pragma once
#endif

#include "ssTimer.h"

class ssDailyTimer : public ssTimer  
{
public:
	ssDailyTimer();
	virtual ~ssDailyTimer();

	BOOL Set(
		const COleDateTime& time,
		PTIMERAPCROUTINE pCompletionRoutine = 0,	// pointer completion routine
		LPVOID pArgToCompletionRoutine = 0,			// pointer to structure that pass to completion routine
		BOOL bResume = FALSE);						// Resume from suspend mode

	BOOL Set(PTIMERAPCROUTINE pCompletionRoutine = 0,	// pointer completion routine
		LPVOID pArgToCompletionRoutine = 0,			// pointer to structure that pass to completion routine
		BOOL bResume = FALSE);
protected:
	COleDateTime m_Time;
};

#endif
