/////////////////////////////////////////////////////////////////////////////
//          File:: ssCharacterSet.h
// Create Author:: Calvin Tam
// Create   Date:: 09 Dec 2000
//
//   Description:: ssCharacterSet class declaration
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

#ifndef __ssCharacterSet_h__
#define __ssCharacterSet_h__

#include "ssCommon.h"

class ssCharacterSet
{
public:

	enum { CP_BIG5 = 950, CP_GB = 936 };

	// ---------------------------------------------------------
	// If the function succeeds, the return value is the number
	// of bytes written to the buffer pointed to by szUTF8.
	// The number includes the byte for the null terminator.
	// If the function fails, the return value is zero. To get
	// extended error information, call GetLastError(). 
	static int Big5ToUTF8 (
		const char* szBig5,		// Big5 String
		char* szUTF8,			// UTF8 Buffer
		int nUTF8Length			// size of buffer
	);


	// ---------------------------------------------------------
	// If the function succeeds, the return value is the number
	// of bytes written to the buffer pointed to by szBig5.
	// The number includes the byte for the null terminator.
	// If the function fails, the return value is zero. To get
	// extended error information, call GetLastError(). 
	static int UTF8ToBig5 (
		const char* szUTF8,		// UTF8 String
		char* szBig5,			// Big5 Buffer
		int nBig5Length			// size of buffer
	);

	static int UTF8ToGB (
		const char* szUTF8,		// UTF8 String
		char* szGB,			// GB Buffer
		int nGBLength			// size of buffer
	);

	static int MultiByteToMultiByte (
		const char* szString1,
		UINT CodePage1,
		char* szString2,
		int nStr2BufferSize,
		UINT CodePage2
	);
};

#endif // __ssCharacterSet_h__
