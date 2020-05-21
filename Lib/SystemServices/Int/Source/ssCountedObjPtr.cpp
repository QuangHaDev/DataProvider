/////////////////////////////////////////////////////////////////////////////
//          File:: ssCountedObjPtr.cpp
// Create Author:: Terence Lai
// Create   Date:: 09 Apr 2001
//
//   Description:: provide a static count on CountedBody
//
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001 by AFE Solutions Ltd.
// All rights reserved.
//
// No portion of this software in any form may be used or reproduced in 
// any manner without written consent from AFE Solutions Ltd.
//
//
#include "ssCountedObjPtr.h"

DWORD ssCountedBodyStatic::m_dwCountedBodyWaitForDeleteCount = 0;