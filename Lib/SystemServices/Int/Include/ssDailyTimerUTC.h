#ifndef _ssDailyTimerUTC_H
#define _ssDailyTimerUTC_H

#if _MSC_VER > 1000
#pragma once
#endif

#include "ssTimer.h"

class ssDailyTimerUTC : public ssTimer
{
public:
	ssDailyTimerUTC();
	virtual ~ssDailyTimerUTC();

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
