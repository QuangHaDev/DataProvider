/////////////////////////////////////////////////////////////////////////////
//          File:: ssCommon.h
// Create Author:: Terence Lai
// Create   Date:: 13 Aug 1998
//
//   Description:: System Service common header
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
#ifndef __ssCommon_H__
#define __ssCommon_H__

#ifndef _WIN32_WINNT
	// assumed Winnt 4.0, comment out if not NT 4.0
//	#define _WIN32_WINNT 0x0400
//	#define _WIN32_WINNT 0x500
#define _WIN32_WINNT 0x501
#endif

// comment out the following line to compile WITHOUT MFC
#define COMPILE_WITH_MFC
#ifdef COMPILE_WITH_MFC

	#include <afx.h>
	#include <afxtempl.h>
	#define _INC_WINDOWS // avoid pull in <windows.h>

#else

	#define _X86_
	#include "windef.h"

	#include <assert.h>
	#define ASSERT assert

#endif

#include <wtypes.h>
//#include <iostream>
#include "ssDefines.h"

//#define new DEBUG_NEW

#endif // __ssCommon_H__
