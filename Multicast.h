//
// Multicast.h
// 
// Created by Bui Minh Duc on August 10, 2018
// Modified by Bui Minh Duc on August 10, 2018
// Copyright (c) 2018 N2N-AFE (Hong Kong) Limited. All rights reserved.
//
#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
class CMulticast
{
//-------------------------------------------------------------------------------------------------
public:
	CMulticast();
	virtual ~CMulticast();
	enum { MC_SUCCESS, MC_ERROR, MC_READ };

	#pragma region Func
	inline int GetLastError() { return m_nStatus; }
	int Initialize();
	virtual int Start(int nPort, const char *pIFAddr = NULL, bool bPortRebind = true);
	virtual void Stop();
	void SetInterface(const char *pInterface);

	// Join the multicast group from which to receive datagrams.
	int AddGroup(const char *pGrpAddr, const char *pIFAddr = NULL);
	// Leave the multicast group from which to receive datagrams.
	int DelGroup(const char *pGrpAddr, const char *pIFAddr = NULL);

	// Receive data from the multicasting group server.
	virtual int Recv(char *pRecvBuf, int nBufSize, char *pRemote, int &nByteRecv);
	virtual int SelectRead(long lMsec = 0, long lSec = 0);

	// Server
	// Set time to live
	virtual int SetTTL(int nTTL);
	// Send a message to the multicasting address with specified port.
	virtual int Send(const char *pGrpAddr, int nPort, const char *pMsg, int nMsgLen);
	#pragma endregion

	#pragma region Params

	#pragma endregion

//-------------------------------------------------------------------------------------------------
protected:
	#pragma region Func

	#pragma endregion

	#pragma region Params
	SOCKET m_Socket;
	int m_nStatus;
	#pragma endregion
	
//-------------------------------------------------------------------------------------------------
private:
	#pragma region Func
	void InitData();
	void DeinitData();
	#pragma endregion

	#pragma region Params

	#pragma endregion
	
	
};

