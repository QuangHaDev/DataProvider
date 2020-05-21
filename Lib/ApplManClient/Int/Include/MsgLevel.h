/////////////////////////////////////////////////////////////////////////////
//          File:: MsgLevel.h
// Create Author:: Nick Fung
// Create   Date:: 09 Nov 2000
//
//   Description:: ApplManClient class declaration
//
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000 by AFE Solutions Ltd.
// All rights reserved.
//
// No portion of this software in any form may be used or reproduced in 
// any manner without written consent from AFE Solutions Ltd.
//
//
/////////////////////////////////////////////////////////////////////////////
// $Date: 10/08/15 3:04p $ 
// $Author: Sulfred_lee $ 

#ifndef __MsgLevel_H__
#define __MsgLevel_H__

#define MESSAGE_LENGTH	256
#define TEMP_BUFFER	32

// Event ID that would signal by CApplManClient
enum
{
	SHUTDOWN = 0,
	CHECKALIVE,
	REMOTE_SHUTDOWN,
	REBUILD,
	TRACEON,
	TRACEOFF,
	NUMBER_WAITEVENT
};

// Event ID that would reply by CApplManClient to ApplManager
enum
{
	REPLY_CHECKALIVE = 0,
	REPLY_REBUILD, 
	REPLY_REBUILD_COMPLETED,
	NUMBER_REPLYEVENT
};

// Error Message Level
enum
{
	LOG_INFO = 0,	// this will reset major and fatal
	LOG_MAJOR,		// 
	LOG_FATAL,
	LOG_NORMAL,
	LOG_ERROR,
};

enum
{
	TRACE_LEVEL0 = 0,
	TRACE_LEVEL1,
	TRACE_LEVEL2,
	TRACE_LEVEL3,
	TRACE_LEVEL4,
	TRACE_LEVEL5,
	TRACE_LEVEL6,
	TRACE_LEVEL7,
	TRACE_LEVEL8,
	TRACE_LEVEL9,
	
	};

#endif // __MsgLevel_H__