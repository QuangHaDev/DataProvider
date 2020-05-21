/////////////////////////////////////////////////////////////////////////////
//   File       :: MCastBuffer.cpp
//   Author     :: John Lee
//   Date       :: 1 September 2000
//   Version    :: 0.10 (Beta)
// 
//
//   Description:: Implementation of the CMCastBuffer class.
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
#include "MCastBuffer.h"


CMCastBuffer::CMCastBuffer() : 
    ssCOPActiveBuffer<BUFFER_ITEM>(QUEUE_INIT_SIZE,QUEUE_INCREMENT,QUEUE_LIMIT)
{
}

CMCastBuffer::~CMCastBuffer()
{
    ExitThread();
}

void CMCastBuffer::AddCallback(PCALLBACKFUNC pCallback, void *pData)
{
    if (pCallback == NULL)
        return;
    CCallbackData data(pCallback, pData);
    Enter();
    m_CallbackList.push_back(data);
    Leave();
}

void  CMCastBuffer::RemoveCallback(PCALLBACKFUNC pCallback, void *pData)
{
    PCALLBACKLIST p;
    if (pCallback == NULL)
        return;
//    CCallbackData data(pCallback, pData);
    Enter();
    for (p=m_CallbackList.begin(); p!=m_CallbackList.end(); p++)
    {
        if (p->m_pCallback == pCallback)
        {
            m_CallbackList.erase(p);
            break;
        }
    }
//    m_CallbackList.remove(data);
    Leave();
}

void CMCastBuffer::ProcessCOP(COP_ITEM& item)
{
    PCALLBACKLIST p;
    Enter();
    for (p=m_CallbackList.begin(); p!=m_CallbackList.end(); p++)
        (*p->m_pCallback)(item, p->m_Data);
    Leave();
}

void CMCastBuffer::ProcessDone()
{
}

UINT CMCastBuffer::GetCount()
{
    return m_Buffer.GetCount();
}

UINT CMCastBuffer::GetMaxCount()
{
    return m_Buffer.GetMaxCount();
}

UINT CMCastBuffer::GetUpperBound()
{
    return m_Buffer.GetUpperBound();
}

BOOL CMCastBuffer::SetIncrement(UINT unIncre)
{
	return m_Buffer.SetIncrement(unIncre);
}

BOOL CMCastBuffer::SetUpperBound(UINT unBound)
{
	return m_Buffer.SetUpperBound(unBound);
}

BOOL CMCastBuffer::SetUpperLimit(UINT unLimit)
{
	return m_Buffer.SetUpperLimit(unLimit);
}

void CMCastBuffer::Stop()
{
    ExitThread();
    WaitForExit();
}