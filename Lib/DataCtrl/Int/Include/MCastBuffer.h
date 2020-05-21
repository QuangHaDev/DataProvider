/////////////////////////////////////////////////////////////////////////////
//   File       :: MCastBuffer.h
//   Author     :: John Lee
//   Date       :: 1 September 2000
//   Version    :: 0.10 (Beta)
// 
//
//   Description:: MCastBuffer class definition.
//                            
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
#ifndef __MCAST_BUFFER_H__
#define __MCAST_BUFFER_H__

#pragma warning (disable:4786)
#include <list>
#include "ssCOPActiveBuffer.h"
#include "MsgObject.h"
using namespace std;


//#define BUFFER_ITEM                     MsgObject
//typedef ssCountedObjPtr<BUFFER_ITEM>    COP_ITEM;
typedef void (*PCALLBACKFUNC)(COP_ITEM& item, void *pData);

//#define RECV_BUFFER_SIZE    4096
#define RECV_BUFFER_SIZE    32768
#define QUEUE_INIT_SIZE     1000
#define QUEUE_INCREMENT     100
#define QUEUE_LIMIT         100000

class CCallbackData
{
public:
    CCallbackData(PCALLBACKFUNC pCallback, void *pData) : m_pCallback(pCallback), m_Data(pData) {};
    virtual ~CCallbackData() {};

    void *m_Data;
    PCALLBACKFUNC m_pCallback;
};
typedef list<CCallbackData>             CALLBACKLIST;
typedef CALLBACKLIST::iterator          PCALLBACKLIST;

/////////////////////////////////////////////////////////////////////////////
//
// Class      : CMCastBuffer
// Description: Internal Message Buffer for IP Multicast
//
class CMCastBuffer : public ssCOPActiveBuffer<BUFFER_ITEM>, ssCriticalSection
{
public:
    CMCastBuffer();
    virtual ~CMCastBuffer();

public:
    void AddCallback(PCALLBACKFUNC pCallback, void *pData);
    void RemoveCallback(PCALLBACKFUNC pCallback, void *pData);
    UINT GetCount();
    UINT GetMaxCount();
    UINT GetUpperBound();
	BOOL SetIncrement(UINT unIncre);
	BOOL SetUpperBound(UINT unBound);
	BOOL SetUpperLimit(UINT unLimit);
    void Stop();

protected:
    long m_lNo;
    CALLBACKLIST m_CallbackList;

    virtual void ProcessCOP(COP_ITEM& item);
    virtual void ProcessDone();
};

#endif //__MCAST_BUFFER_H__