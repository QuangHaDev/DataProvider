//
// Multicast.cpp
// 
// Created by Bui Minh Duc on August 10, 2018
// Modified by Bui Minh Duc on August 10, 2018
// Copyright (c) 2018 N2N-AFE (Hong Kong) Limited. All rights reserved.
//
#include "stdafx.h"
#include "Multicast.h"

///////////////////////////////////////////////////////////////////////////////////////////
CMulticast::CMulticast()
{
	InitData();
	Initialize();
}

CMulticast::~CMulticast()
{
	DeinitData();
	Stop();
	WSACleanup();
}

//-----------------------------------------------------------------------------------------
#pragma region Delegate

#pragma endregion

//-----------------------------------------------------------------------------------------
#pragma region Public
int CMulticast::Initialize()
{
	int nRet = MC_SUCCESS;
	// Initialize Winsock
	WSADATA WSAData;
	//    if (WSAStartup(MAKEWORD(1,1), &WSAData) != 0)
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0)
	{
		m_nStatus = WSAGetLastError();
		nRet = MC_ERROR;
	}
	return nRet;
}
void CMulticast::Stop()
{
	shutdown(m_Socket, 0x00);
	// Close Winsock
	closesocket(m_Socket);
}
int CMulticast::Start(int nPort, const char *pIFAddr, bool bPortRebind)
{
	int nRet = MC_SUCCESS;
	SOCKADDR_IN local_sin;

	// Open a datagram socket
	m_Socket = WSASocket(AF_INET, SOCK_DGRAM, 0, NULL, 0,
		WSA_FLAG_MULTIPOINT_C_LEAF
		| WSA_FLAG_MULTIPOINT_D_LEAF
		| WSA_FLAG_OVERLAPPED);
	if (m_Socket == INVALID_SOCKET)
	{
		WSACleanup();
		m_nStatus = WSAGetLastError();
		nRet = MC_ERROR;
	}
	else
	{
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
			nRet = MC_ERROR;
		}
		else
		{
			// increase receive TCP buffer size
			int iSize = 65536 * 8;
			int iS = sizeof(iSize);
			setsockopt(m_Socket, SOL_SOCKET, SO_RCVBUF, (char *)&iSize, iS);
			SetTTL(30);
		}
	}
	return nRet;
}
void CMulticast::SetInterface(const char *pInterface)
{
	unsigned long addr = inet_addr(pInterface);
	setsockopt(m_Socket, IPPROTO_IP, IP_MULTICAST_IF, (char *)&addr, sizeof(addr));
}
int CMulticast::AddGroup(const char *pGrpAddr, const char *pIFAddr)
{
	int nRet = MC_SUCCESS;
	struct ip_mreq mreq;
	mreq.imr_multiaddr.s_addr = inet_addr(pGrpAddr);
	if (pIFAddr == NULL)
		mreq.imr_interface.s_addr = INADDR_ANY;
	else
		mreq.imr_interface.s_addr = inet_addr(pIFAddr);
	if (setsockopt(m_Socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq, sizeof(mreq)) == -1)
	{
		m_nStatus = WSAGetLastError();
		nRet = MC_ERROR;
	}
	return nRet;
}
int CMulticast::DelGroup(const char *pGrpAddr, const char *pIFAddr)
{
	int nRet = MC_SUCCESS;
	struct ip_mreq mreq;
	mreq.imr_multiaddr.s_addr = inet_addr(pGrpAddr);
	if (pIFAddr == NULL)
		mreq.imr_interface.s_addr = INADDR_ANY;
	else
		mreq.imr_interface.s_addr = inet_addr(pIFAddr);
	if (setsockopt(m_Socket, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *)&mreq, sizeof(mreq)) == -1)
	{
		m_nStatus = WSAGetLastError();
		nRet = MC_ERROR;
	}
	return nRet;
}
int CMulticast::Recv(char *pRecvBuf, int nBufSize, char *pRemote, int &nByteRecv)
{
	int nRet = MC_SUCCESS;
	int clilen;
	SOCKADDR_IN remote_sin;
	clilen = sizeof(SOCKADDR);

	nByteRecv = recvfrom(m_Socket, pRecvBuf, nBufSize, 0, (SOCKADDR *)&remote_sin, &clilen);
	if (nByteRecv < 0)
	{
		m_nStatus = WSAGetLastError();
		nRet = MC_ERROR;
	}
	if (pRemote != NULL)
		strcpy(pRemote, inet_ntoa(remote_sin.sin_addr));
	return nRet;
}
int CMulticast::SelectRead(long lMsec, long lSec)
{
	int nRet = MC_SUCCESS;
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
				nRet = MC_READ;
		}
	}
	else
		nRet = MC_ERROR;
	return nRet;
}
int CMulticast::SetTTL(int nTTL)
{
	int nRet = MC_SUCCESS;
	if (setsockopt(m_Socket, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&nTTL, sizeof(nTTL)) == SOCKET_ERROR)
	{
		m_nStatus = WSAGetLastError();
		nRet = MC_ERROR;
	}
	return nRet;
}
int CMulticast::Send(const char *pGrpAddr, int nPort, const char *pMsg, int nMsgLen)
{
	int nRet = MC_SUCCESS;
	SOCKADDR_IN grp_sin;

	grp_sin.sin_family = AF_INET;
	grp_sin.sin_addr.s_addr = inet_addr(pGrpAddr);
	grp_sin.sin_port = htons(nPort);
	if (sendto(m_Socket, pMsg, nMsgLen, 0, (SOCKADDR *)&grp_sin, sizeof(grp_sin)) != nMsgLen)
	{
		m_nStatus = WSAGetLastError();
		nRet = MC_ERROR;
	}
	return nRet;
}
#pragma endregion

//-----------------------------------------------------------------------------------------
#pragma region Protected

#pragma endregion

//-----------------------------------------------------------------------------------------
#pragma region Private
void CMulticast::InitData()
{
	m_nStatus = 0;
}
void CMulticast::DeinitData()
{
}
#pragma endregion

