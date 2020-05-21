#include <afxdisp.h>
#include "ssDailyTimerUTC.h"

ssDailyTimerUTC::ssDailyTimerUTC() :ssTimer()
{

}

ssDailyTimerUTC::~ssDailyTimerUTC()
{

}

BOOL ssDailyTimerUTC::Set(
	const COleDateTime& time,
	PTIMERAPCROUTINE pCompletionRoutine,		// pointer completion routine
	LPVOID pArgToCompletionRoutine,				// pointer to structure that pass to completion routine
	BOOL bResume)								// Resume from suspend mode
{
	m_Time.SetTime(time.GetHour(), time.GetMinute(), time.GetSecond());
	SYSTEMTIME st;
	GetSystemTime(&st);
	COleDateTime curTime(st);

	COleDateTime stTime(curTime.GetYear(), curTime.GetMonth(), curTime.GetDay(),
		time.GetHour(), time.GetMinute(), time.GetSecond());
	if (curTime >= stTime)
	{
		stTime += COleDateTimeSpan(1, 0, 0, 0);
	}
	FILETIME lfileTime;
	SYSTEMTIME sysTime;
	LARGE_INTEGER value;

	if (stTime.GetAsSystemTime(sysTime) &&
		SystemTimeToFileTime(&sysTime, &lfileTime))
	{
		value.LowPart = lfileTime.dwLowDateTime;
		value.HighPart = lfileTime.dwHighDateTime;
		return ssTimer::Set(&value, 0, pCompletionRoutine,
			pArgToCompletionRoutine, bResume);
	}
	else
	{
		return FALSE;
	}

}

BOOL ssDailyTimerUTC::Set(PTIMERAPCROUTINE pCompletionRoutine,	// pointer completion routine
	LPVOID pArgToCompletionRoutine,			// pointer to structure that pass to completion routine
	BOOL bResume)
{
	SYSTEMTIME st;
	GetSystemTime(&st);
	COleDateTime curTime(st);

	COleDateTime stTime(curTime.GetYear(), curTime.GetMonth(), curTime.GetDay(),
		m_Time.GetHour(), m_Time.GetMinute(), m_Time.GetSecond());
	
	if (curTime >= stTime)
	{
		stTime += COleDateTimeSpan(1, 0, 0, 0);
	}
	FILETIME lfileTime;
	SYSTEMTIME sysTime;
	LARGE_INTEGER value;
	if (stTime.GetAsSystemTime(sysTime) &&
		SystemTimeToFileTime(&sysTime, &lfileTime))
	{
		value.LowPart = lfileTime.dwLowDateTime;
		value.HighPart = lfileTime.dwHighDateTime;
		return ssTimer::Set(&value, 0, pCompletionRoutine,
			pArgToCompletionRoutine, bResume);
	}
	else
	{
		return FALSE;
	}
}