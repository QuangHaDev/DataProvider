#include <afxdisp.h>
#include "ssDailyTimer.h"

ssDailyTimer::ssDailyTimer():ssTimer()
{

}

ssDailyTimer::~ssDailyTimer()
{

}

BOOL ssDailyTimer::Set(
		const COleDateTime& time,
		PTIMERAPCROUTINE pCompletionRoutine,		// pointer completion routine
		LPVOID pArgToCompletionRoutine,				// pointer to structure that pass to completion routine
		BOOL bResume)								// Resume from suspend mode
{
	m_Time.SetTime(time.GetHour(), time.GetMinute(), time.GetSecond());
	COleDateTime curTime=COleDateTime::GetCurrentTime();
	COleDateTime stTime(curTime.GetYear(), curTime.GetMonth(), curTime.GetDay(),
		time.GetHour(), time.GetMinute(), time.GetSecond());
	if (curTime>=stTime)
	{
		stTime+=COleDateTimeSpan(1, 0, 0, 0);
	}
	FILETIME lfileTime, fileTime;
	SYSTEMTIME sysTime;
	LARGE_INTEGER value;
	if (stTime.GetAsSystemTime(sysTime)&& 
		SystemTimeToFileTime(&sysTime, &lfileTime) &&
		LocalFileTimeToFileTime(&lfileTime, &fileTime))
	{
		value.LowPart=fileTime.dwLowDateTime;
		value.HighPart=fileTime.dwHighDateTime;
		return ssTimer::Set(&value, 0, pCompletionRoutine, 
			pArgToCompletionRoutine, bResume);
	}
	else
	{
		return FALSE;
	}
}

BOOL ssDailyTimer::Set(PTIMERAPCROUTINE pCompletionRoutine,	// pointer completion routine
						LPVOID pArgToCompletionRoutine,			// pointer to structure that pass to completion routine
						BOOL bResume)
{
	COleDateTime curTime=COleDateTime::GetCurrentTime();
	COleDateTime stTime(curTime.GetYear(), curTime.GetMonth(), curTime.GetDay(),
		m_Time.GetHour(), m_Time.GetMinute(), m_Time.GetSecond());
	if (curTime>=stTime)
	{
		stTime+=COleDateTimeSpan(1, 0, 0, 0);
	}
	FILETIME lfileTime, fileTime;
	SYSTEMTIME sysTime;
	LARGE_INTEGER value;
	if (stTime.GetAsSystemTime(sysTime)&& 
		SystemTimeToFileTime(&sysTime, &lfileTime) &&
		LocalFileTimeToFileTime(&lfileTime, &fileTime))
	{
		value.LowPart=fileTime.dwLowDateTime;
		value.HighPart=fileTime.dwHighDateTime;
		return ssTimer::Set(&value, 0, pCompletionRoutine, 
			pArgToCompletionRoutine, bResume);
	}
	else
	{
		return FALSE;
	}
}