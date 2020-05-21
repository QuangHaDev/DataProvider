/////////////////////////////////////////////////////////////////////////////
//   File       :: MCast.cpp
//   Author     :: John Lee
//   Date       :: 1 September 2000
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

#include "MCast.h"


/////////////////////////////////////////////////////////////////////////////
// Class CMCast
CMCast::CMCast()
{
    m_nStatus = 0;
    Initialize();
}

CMCast::~CMCast()
{
//    shutdown(m_Socket, 0x00);
    // Close Winsock
//    closesocket(m_Socket);
//    WSACleanup();
    Stop();
    WSACleanup();
}

int CMCast::Initialize()
{
    // Initialize Winsock
    WSADATA WSAData;       
//    if (WSAStartup(MAKEWORD(1,1), &WSAData) != 0)
    if (WSAStartup(MAKEWORD(2,2), &WSAData) != 0)
    {
        m_nStatus = WSAGetLastError();
        return MC_ERROR;
    }
    return MC_SUCCESS;
}

void CMCast::Stop()
{
    shutdown(m_Socket, 0x00);
    // Close Winsock
    closesocket(m_Socket);
}

int CMCast::Start(int nPort, const char *pIFAddr, bool bPortRebind)
{
    SOCKADDR_IN local_sin;

    // Open a datagram socket
//    m_Socket = socket(AF_INET, SOCK_DGRAM, 0); 
    m_Socket = WSASocket(AF_INET, SOCK_DGRAM, 0, NULL, 0,
                WSA_FLAG_MULTIPOINT_C_LEAF
                | WSA_FLAG_MULTIPOINT_D_LEAF
                | WSA_FLAG_OVERLAPPED);
    if (m_Socket == INVALID_SOCKET) 
    { 
        WSACleanup();
        m_nStatus = WSAGetLastError();
        return MC_ERROR;
    } 

    // Bind local address
    // - have to specify if more than one
    local_sin.sin_family = AF_INET;
    if (pIFAddr == NULL)
        local_sin.sin_addr.s_addr = htonl(INADDR_ANY); 
    else
    {
        local_sin.sin_addr.s_addr = inet_addr(pIFAddr); 
        SetInterface(pIFAddr);
    }
    local_sin.sin_port = htons(nPort);

    if (bPortRebind)
    {
        int x = 1;
        setsockopt(m_Socket, SOL_SOCKET, SO_REUSEADDR, (char *)&x, sizeof(x));
    }

    if (bind(m_Socket, (struct sockaddr FAR *) &local_sin, sizeof(local_sin)) == SOCKET_ERROR) 
    { 
        m_nStatus = WSAGetLastError();
        return MC_ERROR;
    }
// Thread Priority Changes by Terence - Start
    // increase receive TCP buffer size
    int iSize = 65536*8;
    int iS = sizeof(iSize);
    setsockopt(m_Socket, SOL_SOCKET, SO_RCVBUF, (char *)&iSize, iS);
// Thread Priority Changes by Terence - End

    SetTTL(30);
    return MC_SUCCESS;
}

void CMCast::SetInterface(const char *pInterface)
{
    unsigned long addr = inet_addr(pInterface);
    setsockopt(m_Socket,IPPROTO_IP, IP_MULTICAST_IF, (char *)&addr, sizeof(addr));
}

#ifdef SSM
int CMCast::AddSSMGroup(const char *pGrpAddr, const char *pSrcAddr, const char *pIFAddr)
{
    int rc;
    struct ip_mreq_source mSrc;
    mSrc.imr_multiaddr.s_addr = inet_addr(pGrpAddr); 
    mSrc.imr_sourceaddr.s_addr = inet_addr(pSrcAddr);
    if (pIFAddr == NULL)
        mSrc.imr_interface.s_addr = INADDR_ANY;
    else
        mSrc.imr_interface.s_addr = inet_addr(pIFAddr);
    if ((rc = setsockopt(m_Socket, IPPROTO_IP, IP_ADD_SOURCE_MEMBERSHIP, (char *)&mSrc, sizeof(mSrc))) != 0)
    {
        m_nStatus = WSAGetLastError();
        return MC_ERROR;
    }

    return MC_SUCCESS;
}

int CMCast::DelSSMGroup(const char *pGrpAddr, const char *pSrcAddr, const char *pIFAddr)
{
    int rc;
    struct ip_mreq_source mSrc;
    mSrc.imr_multiaddr.s_addr = inet_addr(pGrpAddr);
    if (pIFAddr == NULL)
        mSrc.imr_interface.s_addr = INADDR_ANY;
    else
        mSrc.imr_interface.s_addr = inet_addr(pIFAddr);
    if ((rc = setsockopt(m_Socket, IPPROTO_IP, IP_DROP_SOURCE_MEMBERSHIP, (char *)&mSrc, sizeof(mSrc))) != 0)
    {
        m_nStatus = WSAGetLastError();
        return MC_ERROR;
    }

    return MC_SUCCESS;
}
#else
int CMCast::AddGroup(const char *pGrpAddr, const char *pIFAddr)
{
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(pGrpAddr);
    if (pIFAddr == NULL)
        mreq.imr_interface.s_addr = INADDR_ANY;
    else
        mreq.imr_interface.s_addr = inet_addr(pIFAddr);
    if (setsockopt(m_Socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq, sizeof(mreq)) == -1)
    {
        m_nStatus = WSAGetLastError();
        return MC_ERROR;
    }
/*
    if (pIFAddr != NULL)
        SetInterface(pIFAddr);
    struct sockaddr_in remote;
    remote.sin_family = AF_INET;
    remote.sin_port = htons(iPort);
    remote.sin_addr.s_addr = inet_addr(pGrpAddr);
    if ((m_SocketM = WSAJoinLeaf(m_Socket, (SOCKADDR*)&remote, sizeof(remote),
        NULL, NULL, NULL, NULL, JL_BOTH))== INVALID_SOCKET)
    {
        m_nStatus = WSAGetLastError();
        return MC_ERROR;
    }
*/
    return MC_SUCCESS;
}

int CMCast::DelGroup(const char *pGrpAddr, const char *pIFAddr)
{
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(pGrpAddr);
    if (pIFAddr == NULL)
        mreq.imr_interface.s_addr = INADDR_ANY;
    else
        mreq.imr_interface.s_addr = inet_addr(pIFAddr);
    if (setsockopt(m_Socket, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *)&mreq, sizeof(mreq)) == -1)
    {
        m_nStatus = WSAGetLastError();
        return MC_ERROR;
    }

    return MC_SUCCESS;
}
#endif

int CMCast::Recv(char *pRecvBuf, int nBufSize, char *pRemote, int &nByteRecv)
{
    int clilen;
    SOCKADDR_IN remote_sin;
    clilen = sizeof(SOCKADDR);
    nByteRecv = recvfrom(m_Socket, pRecvBuf, nBufSize, 0, (SOCKADDR *)&remote_sin, &clilen);
    if (nByteRecv < 0)
    {
        m_nStatus = WSAGetLastError();       
        return MC_ERROR;
    }
    if (pRemote != NULL)
        strcpy(pRemote, inet_ntoa(remote_sin.sin_addr));

    return MC_SUCCESS;
}

int CMCast::SelectRead(long lMsec, long lSec)
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
                return MC_READ;
        }
    }
    else
        return MC_ERROR;

    return MC_SUCCESS;
}

int CMCast::SetTTL(int nTTL)
{
    if (setsockopt(m_Socket, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&nTTL, sizeof(nTTL)) == SOCKET_ERROR)
    {
        m_nStatus = WSAGetLastError(); 
        return MC_ERROR;
    }
    return MC_SUCCESS;
}

int CMCast::Send(const char *pGrpAddr, int nPort, const char *pMsg, int nMsgLen)
{
    SOCKADDR_IN grp_sin;

    grp_sin.sin_family = AF_INET;
    grp_sin.sin_addr.s_addr = inet_addr(pGrpAddr);
    grp_sin.sin_port = htons(nPort);
    if (sendto(m_Socket, pMsg, nMsgLen, 0, (SOCKADDR *)&grp_sin, sizeof(grp_sin)) != nMsgLen)
    {
        m_nStatus = WSAGetLastError();
        return MC_ERROR;
    }
    return MC_SUCCESS;
}
