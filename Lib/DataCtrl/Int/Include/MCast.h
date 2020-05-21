/////////////////////////////////////////////////////////////////////////////
//          File:: MCast.h
// Create Author:: John Lee
// Create   Date:: 1 September 2000
//
//   Description:: IP Multicast
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
#ifndef __MCAST_H__
#define __MCAST_H__

#include <afx.h>
#include <winsock2.h>
#include <ws2tcpip.h>
//#define SSM
#ifdef SSM
    #include "ssm.h"
#endif

#ifdef MCAST_EXPORTS
#define MCAST_API __declspec(dllexport)
#define EXPIMP_TEMPLATE
#else
#define MCAST_API __declspec(dllimport)
#define EXPIMP_TEMPLATE extern
#endif

/////////////////////////////////////////////////////////////////////////////
//
// Class      : CMCast
// Description: The parent class for both client and server
//
class MCAST_API CMCast
{
public:
    CMCast();
    virtual ~CMCast();

    enum {MC_SUCCESS, MC_ERROR, MC_READ};
public:
    inline int GetLastError() { return m_nStatus; }
    int Initialize();

protected:
    SOCKET m_Socket; //, m_SocketM;
    int m_nStatus;

private:
    CMCast(const CMCast& that);
    const CMCast& operator=(const CMCast& that);

public:
    virtual int Start(int nPort, const char *pIFAddr=NULL, bool bPortRebind=true);
	virtual void Stop();
    void SetInterface(const char *pInterface);
    // Client
#ifdef SSM
    int AddSSMGroup(const char *pGrpAddr, const char *pSrcAddr, const char *pIFAddr=NULL);
    int DelSSMGroup(const char *pGrpAddr, const char *pSrcAddr, const char *pIFAddr=NULL);
#else
    // Join the multicast group from which to receive datagrams.
	int AddGroup(const char *pGrpAddr, const char *pIFAddr = NULL);
    // Leave the multicast group from which to receive datagrams.
	int DelGroup(const char *pGrpAddr, const char *pIFAddr = NULL);
#endif
    // Receive data from the multicasting group server.
	virtual int Recv(char *pRecvBuf, int nBufSize, char *pRemote, int &nByteRecv);
	virtual int SelectRead(long lMsec = 0, long lSec = 0);

    // Server
    // Set time to live
	virtual int SetTTL(int nTTL);
    // Send a message to the multicasting address with specified port.
	virtual int Send(const char *pGrpAddr, int nPort, const char *pMsg, int nMsgLen);
};

#endif // __MCAST_H__