/////////////////////////////////////////////////////////////////////////////
//          File:: ssMMTimer.h
//   Description:: Class definition of Multimedia Timer OS Service abstract class
//
// Create   Date:: 2 Jan 1996
// Create Author:: Terence Lai
// Last  Modtime:: 26 Sep 2001
// Last   Author:: Terence Lai
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1996-2001 by 
// AFE Solutions Limited
// All rights reserved.
//
// No portion of this software in any form may be used or reproduced in 
// any manner without written consent from Reuters Information Technology Inc.
//

#ifndef  __ssMMTimer_H__
#define  __ssMMTimer_H__

#include "ssCommon.h"
#include "ssEvent.h"
#include "MMSYSTEM.H"

const UINT SS_MMTIMER_DEFAULT_DELAY      = 1000;
const UINT SS_MMTIMER_DEFAULT_RESOLUTION = 50;
const UINT SS_MMTIMER_MIN_RESOLUTION     = 16;

/////////////////////////////////////////////////////////////////////////////
//
//       Class: ssMMTimer
//
// Description: 
//          MM Timer abstract class encapsulates WinNT Multimedia timer. 
//          Designing of a concrete MM Timer thread requires over-riding the 
//          Main() function and implementing a thread procedure.
//          
//          MM Timer callback function will call the ssMMTimer::Main()
//
//	    Methods:
//          ssMMTimer()        Constructs an MM Timer object.
//          ~ssMMTimer()       Destructs an MM Timer object
//          Start()              Start Win32 MM Timer
//          Stop()               Kill Win32 MM Timer
//
class ssMMTimer
{
public:
   // -----------------------------------------------------------------------
   // startup and shutdown methods
   ssMMTimer();

   DWORD Start(ssEvent* pEvent,
               UINT iDelay=SS_MMTIMER_DEFAULT_DELAY,
               UINT iResolution=SS_MMTIMER_DEFAULT_RESOLUTION,
               UINT iType=TIME_PERIODIC);

   BOOL  Stop();

   virtual ~ssMMTimer();

   enum // mm timer errors
   {
      InvalidDelayResolution=1,
      InvalidType,
      InvalidEvent,
      MMtimePeriodBeginFailed,
      MMtimeSetEventFailed,
   };

   void Main();

protected:

private:
   // Copy Constructor   
   ssMMTimer(const ssMMTimer& that);

   // Operator =
   const ssMMTimer&  operator = (const ssMMTimer& that); 

   enum // mm timer status
   {
      MMtimePeriodBegun=1,
      MMtimeSet,
   };

   UINT  m_iMMtimerStatus;
   UINT  m_iIDEvent;
   UINT  m_iResolution;

   ssEvent* m_pEvent;
};

#endif  // __ssMMTimer_H__
