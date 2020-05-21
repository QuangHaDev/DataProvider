/////////////////////////////////////////////////////////////////////////////
//   File       :: UDPCast.cpp
//   Author     :: John Lee
//   Date       :: 13 October  2000
//   Version    :: 0.10 (Beta)
// 
//
//   Description:: Implementation of the CMCast class.
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
#define _CRT_SECURE_NO_WARNINGS			1
#define _WINSOCK_DEPRECATED_NO_WARNINGS	1

#include "UDPCast.h"

#define UDP_PORT 34952

/////////////////////////////////////////////////////////////////////////////
// Class CMCast
CUDPCast::CUDPCast()
{
    m_nStatus = 0;
    Initialize();
}

CUDPCast::~CUDPCast()
{
//    Stop();
}

int CUDPCast::Initialize()
{
    // Initialize Winsock
    WSADATA WSAData;       
    if (WSAStartup(MAKEWORD(2,2), &WSAData) != 0)
    {
        m_nStatus = WSAGetLastError();
        return UDP_ERROR;
    }
    return UDP_SUCCESS;
}

void CUDPCast::Stop()
{
    if (m_Socket != NULL)
    {
        shutdown(m_Socket, 0x00);
        closesocket(m_Socket);
    }
    WSACleanup();
}

int CUDPCast::Start(const char *pIFAddr, unsigned short usPort)
{
    SOCKADDR_IN local_sin;

    // Open a datagram socket
    m_Socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_Socket == INVALID_SOCKET) 
    { 
        WSACleanup();
        m_nStatus = WSAGetLastError();
        return UDP_ERROR;
    } 

    // Bind local address
    // - have to specify if more than one
    local_sin.sin_family = AF_INET;
    if (pIFAddr == NULL)
        local_sin.sin_addr.s_addr = htonl(INADDR_ANY); 
    else
        local_sin.sin_addr.s_addr = inet_addr(pIFAddr); 
    if (usPort == 0)
        local_sin.sin_port = htons(UDP_PORT);
    else
        local_sin.sin_port = htons(usPort);

    int x = 1;
    setsockopt(m_Socket, SOL_SOCKET, SO_REUSEADDR, (char *)&x, sizeof(x));

    if (bind(m_Socket, (struct sockaddr FAR *) &local_sin, sizeof(local_sin)) == SOCKET_ERROR) 
    { 
        m_nStatus = WSAGetLastError();
        return UDP_ERROR;
    }

    BOOL bBroadcast = TRUE;
    if (setsockopt(m_Socket, SOL_SOCKET,  SO_BROADCAST, (char*)&bBroadcast, sizeof(BOOL)))
    {
        m_nStatus = WSAGetLastError();
        return UDP_ERROR;
    }
    int BufSize = 32768;
    if (setsockopt(m_Socket, SOL_SOCKET,  SO_RCVBUF, (char*)&BufSize, sizeof(int)))
    {
        m_nStatus = WSAGetLastError();
        return UDP_ERROR;
    }
    return UDP_SUCCESS;
}

int CUDPCast::Recv(char *pRecvBuf, int nBufSize, char *pRemote, int &nByteRecv)
{
    int clilen;
    SOCKADDR_IN remote_sin;
    clilen = sizeof(SOCKADDR);
    nByteRecv = recvfrom(m_Socket, pRecvBuf, nBufSize, 0, (SOCKADDR *)&remote_sin, &clilen);
    if (nByteRecv < 0)
    {
        m_nStatus = WSAGetLastError();       
        return UDP_ERROR;
    }
    strcpy(pRemote, inet_ntoa(remote_sin.sin_addr));

    return UDP_SUCCESS;
}

int CUDPCast::Send(const char *pMsg, int nMsgLen)
{
    SOCKADDR_IN grp_sin;

    memset(&grp_sin, 0, sizeof(grp_sin));
    grp_sin.sin_family = AF_INET;
    grp_sin.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    grp_sin.sin_port = htons(UDP_PORT);
    if (sendto(m_Socket, pMsg, nMsgLen, 0, (SOCKADDR *)&grp_sin, sizeof(grp_sin)) != nMsgLen)
    {
        m_nStatus = WSAGetLastError();
        return UDP_ERROR;
    }
    return UDP_SUCCESS;
}

int CUDPCast::SelectRead(long lMsec, long lSec)
{
    fd_set fdread;
    int ret;
    int i = 0;

    struct timeval tm, *ptm;
    tm.tv_sec = lSec;
    tm.tv_usec = lMsec;
    FD_ZERO(&fdread);
    FD_SET(m_Socket, &fdread);
    if (lMsec == -1)
        ptm = NULL;
    else
        ptm = &tm;
    if ((ret = select(0, &fdread, NULL, NULL, ptm)) != SOCKET_ERROR)
    {
        if (ret > 0)
        {
            if (FD_ISSET(m_Socket, &fdread))
                return UDP_READ;
        }
    }
    else
        return UDP_ERROR;

    return UDP_SUCCESS;
}
