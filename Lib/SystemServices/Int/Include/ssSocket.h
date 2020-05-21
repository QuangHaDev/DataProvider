/////////////////////////////////////////////////////////////////////////////
//          File:: ssSocket.h
// Create Author:: Calvin Tam
// Create   Date:: 29 Nov 1998
//
//   Description:: The ssSocket class declaration
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

#ifndef _ssSocket_H_
#define _ssSocket_H_

/////////////////////////////////////////////////////////////////////////////
//
// Class	   : ssSocket
// Description : The ssSocket abstract class encapsulates a Win32 socket
//				 object. It only provides blocking calls.
//
// Methods:
//	Socket()						- Constructs a win32 socket object.
//	~Socket()						- Destructs a socket object.
//	Create()						- Create a socket (usually as a socket
//									  server)
//	Close()							- Close a socket
//	CloseASocket()					- Close an accepted socket
//	Connect()						- connect to  a specific scoket (usually as a
//									  socket client)
//	Receive()						- Read a message to the specific buffer.
//	Send()							- Write a message to the socket
//	CancelBlockingCall()			- Not use now
//	Listen()						- listen for incoming connection request
//	Accept()						- accept a connection on a socket
//	ShutDown()						- Disable both Send and Receive calls
//	EventSelect()					- Set event when FD_ACCEPT, FD_READ or 
//									  FD_CLOSE network event is notified
//	NetworkEvent()					- return the network event
//  GetLastError()					- return last socket error
//
/////////////////////////////////////////////////////////////////////////////

#include <winsock2.h>


class ssSocket
{
public:
	// ----------------------------------------------------------------------
	// Constructor
	ssSocket();

	// ----------------------------------------------------------------------
	// Destructor
	virtual ~ssSocket();

	// ----------------------------------------------------------------------
	// Create a socket (usually as a socket server)
	BOOL Create(UINT uiPort);

	// ----------------------------------------------------------------------
	// Close a socket
	void Close(void);

	// Close the accepted socket (server only)
	void CloseASocket ();

	// ----------------------------------------------------------------------
	// Connect to a specific socket (usually as a socket client)
	BOOL Connect (LPTSTR lpServerName, UINT uiPort);

	// ----------------------------------------------------------------------
	// Read a message to the specific buffer.
	virtual int Receive( void* lpBuf, int nBufLen );	

	// ----------------------------------------------------------------------
	// Write a message to the socket
	virtual int Send( const void* lpBuf, int nBufLen );

	// ----------------------------------------------------------------------
	// Not use now
	void CancelBlockingCall();

	// ----------------------------------------------------------------------
	// Listen for incoming connection request
	BOOL Listen();

	// ----------------------------------------------------------------------
	// Accept a connection on a socket
	BOOL Accept();

	// ----------------------------------------------------------------------
	// Disable both Send and Receive calls
	BOOL ShutDown();

	// ----------------------------------------------------------------------
	// Return the last socket error
	int GetLastError() { return ::WSAGetLastError(); };

protected:
	// Overridables

	// ----------------------------------------------------------------------
	// Override this to alter the receive method
	// Return no. of bytes received or negative value if error
	// This return value should be equal to iDataLen if no error is found
	virtual int OnRecv ( void* lpBuf, int iDataLen );

	// ----------------------------------------------------------------------
	// Override this to rollback the data when necessary
	virtual void OnRecvRollback ( void* lpBuf, int iDataLen );

	// ----------------------------------------------------------------------
	// Override this to alter the send method
	// Return no. of bytes send or negative value if error
	int OnSend ( const void* lpBuf, int iBufLen );


private:
	// ----------------------------------------------------------------------
	// Read until nBufLen bytes received
	int ReceiveFixedSize ( void* lpBuf, int nBufLen );


protected:
	BOOL Socket();					// construct a win32 socket

	SOCKET		m_hSocket;			// base socket
	SOCKET		m_hASocket;			// Accepted socket
};

#endif // _ssSocket_H_
