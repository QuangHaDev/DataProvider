/////////////////////////////////////////////////////////////////////////////
//   File       :: Message.cpp
//   Author     :: John Lee
//   Date       :: 1 September 2000
//   Version    :: 0.10 (Beta)
// 
//
//   Description:: Implementation of the CMCastMessage class.
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
#include "afx.h"
#include "Message.h"

CMCastMessage::CMCastMessage() :
m_iLen(0), m_nVirtualSize(0), m_iNoMsg(0)
{
    m_iLimit = 512;
}

CMCastMessage::CMCastMessage(int iLen, char *p) :
m_iLen(0), m_nVirtualSize(0), m_iNoMsg(0)
{
    AddMessage(iLen, p);
}

CMCastMessage::~CMCastMessage()
{
}

int CMCastMessage::AddMessage(int iLen, const char *p)
{
    ASSERT(iLen > 0);
    if (iLen > MSG_MAX_LENGTH)
        return 2;
    if (m_iLen != 0)
    {
        if ((iLen + m_iLen) > m_iLimit)
            return 1;
        else if ((iLen + m_nVirtualSize) > m_iLimit)
            return 1;
    }
    memcpy((m_p + m_iLen), p, iLen);
    m_iLen += iLen;
    m_nVirtualSize += iLen;
	m_iNoMsg++;
    return 0;
}

int CMCastMessage::ForceAdd(int iLen, const char *p)
{
    int iRc = 0;
    if ( (iLen + m_iLen) > m_iLimit )
        iRc = 1;
    else if ( (iLen + m_nVirtualSize) > m_iLimit )
        iRc = 1;

    if ( (iLen + m_iLen) > MSG_MAX_LENGTH )
        return 3;
    else if ( (iLen + m_nVirtualSize) > MSG_MAX_LENGTH )
        return 3;

    ASSERT(m_iLen >= 0);
    if (m_iLen < 0)
        m_iLen = 0;
    memcpy((m_p + m_iLen), p, iLen);
    m_iLen += iLen;
    m_nVirtualSize += iLen;
	m_iNoMsg++;
    return iRc;
}

void CMCastMessage::Reset()
{
    VirtualReset();
    m_nVirtualSize = 0;
	m_iNoMsg = 0;
}

void CMCastMessage::VirtualReset()
{
    memset(m_p, 0, MSG_MAX_LENGTH);
    m_iLen = 0;
}

void CMCastMessage::SetLimit(int iLimit)
{
    ASSERT(iLimit < MSG_MAX_LENGTH);
    ASSERT(iLimit > 0);
    if ((iLimit > MSG_MAX_LENGTH) || (iLimit <= 0))
        m_iLimit = MSG_MAX_LENGTH;
    else
        m_iLimit = iLimit;
}
