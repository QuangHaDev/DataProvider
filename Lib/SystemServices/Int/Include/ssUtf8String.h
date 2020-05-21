/////////////////////////////////////////////////////////////////////////////
//          File:: ssUtf8String.h
// Create Author:: Raymond Leung
// Create   Date:: 23 Oct 1998
//
//   Description:: ssUtf8String class declaration
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

#ifndef __ssUTF8STRING_H__
#define __ssUTF8STRING_H__

#include "ssCommon.h"

#define MAX_KEY_NAME_LENGTH 256
#define MAX_BUFFER_LENGTH 1024

DWORD GetPrivateProfileUTF8String(
  LPCTSTR lpAppName,        // points to section name
  LPCTSTR lpKeyName,        // points to key name
  LPCTSTR lpDefault,        // points to default string
  LPTSTR lpReturnedString,  // points to destination buffer
  DWORD nSize,              // size of destination buffer
  LPCTSTR lpFileName        // points to initialization filename
);


#endif // __ssUTF8STRING_H__