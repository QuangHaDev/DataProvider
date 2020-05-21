/////////////////////////////////////////////////////////////////////////////
//          File:: ssCharacterSet.h
// Create Author:: Calvin Tam
// Create   Date:: 09 Dec 2000
//
//   Description:: ssCharacterSet class member function
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

#include "ssCharacterSet.h"

int ssCharacterSet::Big5ToUTF8 (
		const char* szBig5,		// Big5 String
		char* szUTF8,			// UTF8 Buffer
		int nUTF8Length			// size of buffer
	)
{
	return MultiByteToMultiByte (szBig5, CP_BIG5, szUTF8, nUTF8Length, CP_UTF8);
}


int ssCharacterSet::UTF8ToBig5 (
		const char* szUTF8,		// UTF8 String
		char* szBig5,			// Big5 Buffer
		int nBig5Length			// size of buffer
	)
{
	return MultiByteToMultiByte (szUTF8, CP_UTF8, szBig5, nBig5Length, CP_BIG5);
}

int ssCharacterSet::UTF8ToGB (
		const char* szUTF8,		// UTF8 String
		char* szGB,			// GB Buffer
		int nGBLength			// size of buffer
	)
{

	return MultiByteToMultiByte (szUTF8, CP_UTF8, szGB, nGBLength, CP_GB);
}

int ssCharacterSet::MultiByteToMultiByte (
		const char* szString1,
		UINT CodePage1,
		char* szString2,
		int nStr2BufferSize,
		UINT CodePage2
	)
{
	const int unicodeStrLength = 1024;
	WCHAR unicodeStr[unicodeStrLength];
	int unicodeLen = ::MultiByteToWideChar(CodePage1, 0, szString1, -1, unicodeStr, unicodeStrLength);
	return ::WideCharToMultiByte (CodePage2, 0, unicodeStr, unicodeLen, szString2, nStr2BufferSize, 0, 0);
}
