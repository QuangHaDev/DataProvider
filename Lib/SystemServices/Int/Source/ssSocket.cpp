/////////////////////////////////////////////////////////////////////////////
//          File:: ssSocket.cpp
// Create Author:: Calvin Tam
// Create   Date:: 29 Nov 1998
//
//   Description:: ssSocket class member function
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
#define _WINSOCK_DEPRECATED_NO_WARNINGS	1
#include <afx.h>
#include "ssSocket.h"


// ----------------------------------------------------------------------
// Constructor
ssSocket::ssSocket()
{
	m_hSocket = INVALID_SOCKET;
	m_hASocket = INVALID_SOCKET;
}


// ----------------------------------------------------------------------
// Destructor
ssSocket::~ssSocket()
{
	Close();
}

//
// ********* ALL OTHER METHODS TO BE PLACED BELOW ***********
//

// ----------------------------------------------------------------------
// construct a win32 socket
BOOL ssSocket::Socket()
{
	m_hSocket = socket(AF_INET, SOCK_STREAM, PF_UNSPEC);
	if (m_hSocket == INVALID_SOCKET)
	{
		return FALSE;
	}

	return TRUE;
}


// ----------------------------------------------------------------------
// Create a socket (usually as a socket server)
BOOL ssSocket::Create(UINT uiPort)
{
	ASSERT(m_hSocket == INVALID_SOCKET);

	if (!Socket())
		return FALSE;

	SOCKADDR_IN sockAddr;
	memset(&sockAddr,0,sizeof(sockAddr));

	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	sockAddr.sin_port = htons((u_short)uiPort);

	if (SOCKET_ERROR == bind(m_hSocket, (SOCKADDR*)&sockAddr, sizeof(sockAddr)))
	{
		Close();
		return FALSE;
	}

	return TRUE;
}


// ----------------------------------------------------------------------
// Close a socket
void ssSocket::Close(void)
{
	if (m_hSocket != INVALID_SOCKET)
	{
		CancelBlockingCall();

		VERIFY(SOCKET_ERROR != closesocket(m_hSocket));
		m_hSocket = INVALID_SOCKET;
	}

	CloseASocket();
}


// ----------------------------------------------------------------------
// Close accepted socket
void ssSocket::CloseASocket()
{
	if (m_hASocket != INVALID_SOCKET)
	{
		VERIFY(SOCKET_ERROR != closesocket(m_hASocket));
		m_hASocket = INVALID_SOCKET;
	}
}


// ----------------------------------------------------------------------
// Connect to a specific socket (usually as a socket client)
BOOL ssSocket::Connect (LPTSTR lpServerName, UINT uiPort)
{
	ASSERT(lpServerName != NULL);

	if (m_hSocket == INVALID_SOCKET)
	{
		if (!Socket())
			return FALSE;
	}

	SOCKADDR_IN sockAddr;
	memset(&sockAddr,0,sizeof(sockAddr));

	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = inet_addr(lpServerName);

	if (sockAddr.sin_addr.s_addr == INADDR_NONE)
	{
		LPHOSTENT lphost;
		lphost = gethostbyname(lpServerName);
		if (lphost != NULL)
			sockAddr.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
		else
		{
			WSASetLastError(WSAEINVAL);
			return FALSE;
		}
	}

	sockAddr.sin_port = htons((u_short)uiPort);

	if (connect(m_hSocket, (SOCKADDR*)&sockAddr, sizeof(sockAddr)) == SOCKET_ERROR)
	{
		return FALSE;
	}
	return TRUE;
}


// ----------------------------------------------------------------------
// Read a message to the specific buffer.
//	It will block infinitely until either message is read or
//	::StopPending() is called
int ssSocket::Receive( void* lpBuf, int nBufLen )
{
	int iBufferLength;
	int iRet;

	iRet = ReceiveFixedSize ((LPSTR)&iBufferLength, sizeof(iBufferLength));
	if (iRet <= 0 || iBufferLength == 0)
		return iRet;

	if (iBufferLength > nBufLen)
		return -1;

	iRet = ReceiveFixedSize ((LPSTR)lpBuf, iBufferLength);
	if (iRet == 0)
		OnRecvRollback((LPSTR)&iBufferLength, sizeof(iBufferLength));
	return iRet;
}


// ----------------------------------------------------------------------
// Write a message to the pipe
//	It will block infinitely until either messge is written or
//	::StopPending() is called
int ssSocket::Send( const void* lpBuf, int nBufLen )
{
	const int BUFFER_SIZE = 4096;
	char cBuffer[BUFFER_SIZE];
	char* pBufPointer = (char*)lpBuf;

	int iDataSizeWidth = sizeof(nBufLen);
	
	memcpy (cBuffer, &nBufLen, iDataSizeWidth);

	int iTotalByteSent = 0;
	int iSendSize;
	int iNumSent;
	while ( pBufPointer - (LPSTR) lpBuf <  nBufLen )
	{
		iSendSize = min ((LPSTR) lpBuf + nBufLen - pBufPointer,
			BUFFER_SIZE - iDataSizeWidth);
		memcpy (cBuffer+iDataSizeWidth, pBufPointer, iSendSize);
		
		iNumSent =  OnSend (cBuffer, iSendSize+iDataSizeWidth);
		if (iNumSent <= 0)
		{
			return iNumSent;
		}

		iTotalByteSent += (iNumSent - iDataSizeWidth);
		pBufPointer += iSendSize;
		iDataSizeWidth = 0;
	}

	return iTotalByteSent;
}


// ----------------------------------------------------------------------
// Not use now
void ssSocket::CancelBlockingCall()
{
}


// ----------------------------------------------------------------------
// Listen for incoming connection request
BOOL ssSocket::Listen()
{
	if (SOCKET_ERROR == listen(m_hSocket, 1))
	{
		return FALSE;
	}

	return TRUE;
}


// ----------------------------------------------------------------------
// Accept a connection on a socket
BOOL ssSocket::Accept()
{
	ASSERT (m_hSocket != INVALID_SOCKET);

	m_hASocket = accept (m_hSocket, NULL, NULL);
	if (m_hASocket == INVALID_SOCKET)
	{
		return FALSE;
	}

	return TRUE;
}


// ----------------------------------------------------------------------
// Disable both Send and Receive calls
BOOL ssSocket::ShutDown()
{
	if (m_hSocket == INVALID_SOCKET)
		return TRUE;

	if (SOCKET_ERROR == shutdown(m_hSocket,2))
	{
		return FALSE;
	}

	return TRUE;
}


// ----------------------------------------------------------------------
// Read until nBufLen bytes received
int ssSocket::ReceiveFixedSize ( void* lpBuf, int nBufLen )
{
	int iRet;
	int iNumReceived = 0;
	while (iNumReceived < nBufLen)
	{
		iRet = OnRecv ((LPSTR) ((LPSTR)lpBuf+iNumReceived), nBufLen - iNumReceived);
		if (iRet <= 0)
		{
			return iRet;
		}

		iNumReceived += iRet;
	}

	return iNumReceived;
}


// ----------------------------------------------------------------------
// Override this to alter the receive method
// Return no. of bytes received or negative value if error
int ssSocket::OnRecv ( void* lpBuf, int iBufLen )
{
	if (m_hASocket == INVALID_SOCKET)
	{
		return ::recv (m_hSocket, (LPSTR) lpBuf, iBufLen, 0);
	}
	else
	{
		int iRecv = ::recv (m_hASocket, (LPSTR) lpBuf, iBufLen, 0);
		if (iRecv <= 0)
			m_hASocket = INVALID_SOCKET;
		return iRecv;
	}
}


// ----------------------------------------------------------------------
// Override this to alter the send method
// Return no. of bytes send or negative value if error
int ssSocket::OnSend ( const void* lpBuf, int iBufLen )
{
	if (m_hASocket == INVALID_SOCKET)
	{
		return ::send (m_hSocket, (LPSTR) lpBuf, iBufLen, 0);
	}
	else
	{
		int iSent = ::send (m_hASocket, (LPSTR) lpBuf, iBufLen, 0);
		if (iSent <= 0)
			m_hASocket = INVALID_SOCKET;
		return iSent;
	}
}


// ----------------------------------------------------------------------
// Override this to rollback the data when necessary
void ssSocket::OnRecvRollback ( void* lpBuf, int iDataLen )
{
	// Since ::recv will block, this function should do nothing
}
