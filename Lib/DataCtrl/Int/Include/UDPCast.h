/////////////////////////////////////////////////////////////////////////////
//          File:: UDPCast.h
// Create Author:: John Lee
// Create   Date:: 1 October 2000
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
#ifndef __UDPCAST_H__
#define __UDPCAST_H__

#include <afx.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#ifdef UDPCAST_EXPORTS
#define UDPCAST_API __declspec(dllexport)
#else
#define UDPCAST_API __declspec(dllimport)
#endif

/////////////////////////////////////////////////////////////////////////////
//
// Class      : CMCast
// Description: The parent class for both client and server
//
class UDPCAST_API CUDPCast
{
public:
    CUDPCast();
    virtual ~CUDPCast();

    enum {UDP_SUCCESS, UDP_ERROR, UDP_READ};

    int Start(const char *pIFAddr, unsigned short usPort);
    void Stop();
    int Initialize();
    inline int GetLastError() { return m_nStatus; }
    int Recv(char *pRecvBuf, int nBufSize, char *pRemote, int &nByteRecv);
    int Send(const char *pMsg, int nMsgLen);
    int SelectRead(long lMsec, long lSec=0);

protected:
    SOCKET m_Socket; //, m_SocketM;
    int m_nStatus;

private:
    CUDPCast(const CUDPCast& that);
    const CUDPCast& operator=(const CUDPCast& that);
};

#endif // __MCAST_H__