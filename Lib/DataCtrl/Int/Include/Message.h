/////////////////////////////////////////////////////////////////////////////
//   File       :: Message.h
//   Author     :: John Lee
//   Date       :: 1 September 2000
//   Version    :: 0.10 (Beta)
// 
//
//   Description:: CMCastMessage class definition.
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
#ifndef __CMESSAGE_H__
#define __CMESSAGE_H__

#define MSG_MAX_LENGTH  4096

#ifdef DATACTRL_EXPORTS
#define DATACTRL_API __declspec(dllexport)
#else
#define DATACTRL_API __declspec(dllimport)
#endif

class DATACTRL_API CMCastMessage
{
public:
    CMCastMessage();
    CMCastMessage(int iLen, char *p);
    virtual ~CMCastMessage();

    int AddMessage(int n, const char *p);
    int GetLength() { return m_iLen; }
	int GetNoMsg() { return m_iNoMsg; }
    const char *GetData() { return m_p; }
    void Reset();
    void VirtualReset();
    void SetLimit(int iLimit);
    int ForceAdd(int iLen, const char *p);

private:
    int m_iLen;
    char m_p[MSG_MAX_LENGTH * 2];
	int m_iNoMsg;
    int m_iLimit;
    int m_nVirtualSize;
};


#endif // __CMESSAGE_H__