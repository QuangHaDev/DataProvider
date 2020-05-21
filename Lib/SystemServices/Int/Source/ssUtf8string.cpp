/////////////////////////////////////////////////////////////////////////////////
//          File:: ssUTF8String.cpp
// Create Author:: Raymond Leung
// Create   Date:: 23 Oct 1998
//
//   Description:: ssUTF8String function
//
//
////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000 by AFE Solutions Ltd.
// All rights reserved.
//
// No portion of this software in any form may be used or reproduced in 
// any manner without written consent from AFE Solutions Ltd.
//
//
#include "ssUtf8String.h"

DWORD GetPrivateProfileUTF8String(
  LPCTSTR lpAppName,        // points to section name
  LPCTSTR lpKeyName,        // points to key name
  LPCTSTR lpDefault,        // points to default string
  LPTSTR lpReturnedString,  // points to destination buffer
  DWORD nSize,              // size of destination buffer
  LPCTSTR lpFileName        // points to initialization filename
)
{
	// !!! Need to verify the validity of input parameter !!!
	// !!! Not implemented yet; try hard to comply with GetPrivateProfileString ()

	WCHAR	wcszSectionName [MAX_KEY_NAME_LENGTH];
	WCHAR	wcszKeyName [MAX_KEY_NAME_LENGTH];
	WCHAR	wcszDefault [MAX_BUFFER_LENGTH];
	WCHAR	wcszFileName [MAX_KEY_NAME_LENGTH];
	WCHAR*	pwcszReturnedStr = new WCHAR [nSize];
	if (pwcszReturnedStr == 0) 
		return 0;

	// Initialize buffer
	memset (wcszSectionName, 0, MAX_KEY_NAME_LENGTH*sizeof(WCHAR));
	memset (wcszKeyName, 0, MAX_KEY_NAME_LENGTH*sizeof(WCHAR));
	memset (wcszDefault, 0, MAX_BUFFER_LENGTH*sizeof(WCHAR));
	memset (pwcszReturnedStr, 0, nSize*sizeof(WCHAR));
	memset (wcszFileName, 0, MAX_KEY_NAME_LENGTH*sizeof(WCHAR));

	bool bError = false;

	if (MultiByteToWideChar (CP_ACP, 0, lpAppName, -1, wcszSectionName, MAX_KEY_NAME_LENGTH) == 0)
		bError = true; //??? error handling
	if (MultiByteToWideChar (CP_ACP, 0, lpKeyName, -1, wcszKeyName, MAX_KEY_NAME_LENGTH) == 0)
		bError = true; //??? error handling
	if (MultiByteToWideChar (CP_ACP, 0, lpDefault, -1, wcszDefault, MAX_BUFFER_LENGTH) == 0)
		bError = true; //??? error handling
	if (MultiByteToWideChar (CP_ACP, 0, lpFileName, -1, wcszFileName, MAX_KEY_NAME_LENGTH) == 0)
		bError = true; //??? error handling

	if (bError)
	{
		delete pwcszReturnedStr;
		return 0;
	}
	DWORD nResult = 0;
	GetPrivateProfileStringW (wcszSectionName, wcszKeyName, wcszDefault, 
								pwcszReturnedStr, nSize, wcszFileName);

	nResult = WideCharToMultiByte (CP_UTF8, 0, pwcszReturnedStr, -1, 
									lpReturnedString, nSize, NULL, NULL);
	delete pwcszReturnedStr;
	return nResult;
}
