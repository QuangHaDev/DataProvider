/////////////////////////////////////////////////////////////////////////////
//          File:: MMTimer.cpp
//   Description:: Multimedia Timer class member functions
//
// Create   Date:: 2 Jan 1996
// Create Author:: Terence Lai
// Last  Modtime:: 2/04/98 1:59p                                            
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
#include "ssMMTimer.h"

//////////////////////////////////////////////////////////////////////
//    Function: ssMMTimerCB()
//
// Description: ssMMTimer call back function
//              - signal the Timer wakeup event
//
//  Parameters: IDEvent   - identifies timer event
//              uReserv   - not used
//              dwUser    - pointer of ACS_MMTimer
//              dwReserv1 - not used
//              dwReserv2 - not used 
//
//  Return:     Nil
//
VOID CALLBACK ssMMTimerCB(UINT  IDEvent, 
                            UINT  uReserv, 
                            DWORD dwUser, 
                            DWORD dwReserv1, 
                            DWORD dwReserv2)
{
    ssMMTimer* pTimer = (ssMMTimer *) dwUser;
    pTimer->Main();
}

//////////////////////////////////////////////////////////////////////
//
// ssMMTimer member functions
//

//////////////////////////////////////////////////////////////////////
//    Function: ssMMTimer::ssMMTimer()
//
// Description: ssTimer default constructor, 
//              - initialize private member variables
//
//  Parameters: VOID
//
ssMMTimer::ssMMTimer() : m_pEvent(0),
                         m_iMMtimerStatus(0),
                         m_iIDEvent(0),
                         m_iResolution(0)
{
}

//////////////////////////////////////////////////////////////////////
//    Function: ssMMTimer::Start()
//
// Description: Start MM Timer
//
//  Parameters: iDelay      - Timeout delay in milliseconds (1000ms default)
//              iResolution - MM Timer resolution in ms (50ms default)
//                            iResolution muse <= iDelay
//              iType       - Notification type, TIME_PERIODIC (default)
//                                               TIME_ONESHOT
//
//
//     Returns: 0                         - succeeded
//              InvalidDelayResolution    - failed
//              InvalidType               - failed
//              MMtimePeriodBeginFailed   - failed
//              MMtimeSetEventFailed      - failed
//
DWORD ssMMTimer::Start(ssEvent* pEvent,
                       UINT iDelay, 
                       UINT iResolution, 
                       UINT iType)
{
   if (pEvent == 0)
   {
      return(ssMMTimer::InvalidEvent);
   }
   else
   {
      m_pEvent = pEvent;
   }

   if ((iDelay < iResolution) && (iResolution >= SS_MMTIMER_MIN_RESOLUTION))
   {
      return(ssMMTimer::InvalidDelayResolution);
   }
   else
   {
      m_iResolution = iResolution;
   }

   if ((iType != TIME_ONESHOT) && (iType != TIME_PERIODIC))
   {
      return(ssMMTimer::InvalidType);
   }

   // set mm timer resolution
   if (timeBeginPeriod(m_iResolution) == TIMERR_NOCANDO) 
   {
      return(ssMMTimer::MMtimePeriodBeginFailed);
   }
   else
   {
      m_iMMtimerStatus = ssMMTimer::MMtimePeriodBegun;
   } 

   // start mm timer
   m_iIDEvent = timeSetEvent(iDelay,                           // delay
                             m_iResolution,                    // resolution
                             (LPTIMECALLBACK)ssMMTimerCB,    // callback
                             (DWORD) this,       // user data
                             TIME_PERIODIC);
   if (m_iIDEvent == 0)
   {
      return(ssMMTimer::MMtimeSetEventFailed);
   }
   else
   {
      m_iMMtimerStatus = ssMMTimer::MMtimeSet;
   }

   return(0);
}

//////////////////////////////////////////////////////////////////////
//    Function: ssMMTimer::Stop()
//
// Description: Stop MM Timer
//
//  Parameters: void
//
//     Returns: TRUE  - succeeded
//              FALSE - failed
//
BOOL ssMMTimer::Stop()
{
   BOOL bRetValue = TRUE;

   if ((m_iMMtimerStatus == ssMMTimer::MMtimeSet) &&
       (m_iIDEvent != 0))
   {
      if (timeKillEvent(m_iIDEvent) == TIMERR_NOCANDO)
      {
         bRetValue = FALSE;
      } 
      m_iMMtimerStatus = ssMMTimer::MMtimePeriodBegun;
   }

   if (m_iMMtimerStatus == ssMMTimer::MMtimePeriodBegun)
   {
      // cancel min. resolution   
      if (timeEndPeriod(m_iResolution) == TIMERR_NOCANDO)
      {
         bRetValue = FALSE;  
      }
      m_iMMtimerStatus = 0;
   }

   return(bRetValue);
}

//////////////////////////////////////////////////////////////////////
//    Function: ssMMTimer::~ssMMTimer()
//
// Description: ssMMTimer destructor
//
//  Parameters: void
//
ssMMTimer::~ssMMTimer()
{
   if ((m_iMMtimerStatus == ssMMTimer::MMtimeSet) &&
       (m_iIDEvent != 0))
   {
      timeKillEvent(m_iIDEvent);
      m_iMMtimerStatus = ssMMTimer::MMtimePeriodBegun;
   }

   if (m_iMMtimerStatus == ssMMTimer::MMtimePeriodBegun)
   {
      // cancel min. resolution   
      timeEndPeriod(m_iResolution);
   }
}

void ssMMTimer::Main()
{
   if (m_pEvent != 0)
   {
      m_pEvent->Set();
   }
};


