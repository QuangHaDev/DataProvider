// Soc.cpp: implementation of the CSoc class.
//
//////////////////////////////////////////////////////////////////////
#define _WINSOCK_DEPRECATED_NO_WARNINGS	1

#include <afx.h>
#include <winsock2.h>
#include "stdafx.h"
#include "Soc.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSoc::CSoc() :
m_socket(INVALID_SOCKET)
{
}

CSoc::~CSoc()
{
    Close();
}

bool CSoc::Open(string host, long port)
{
	// get socket
	m_socket = socket(PF_INET,SOCK_STREAM,0);

	if ( m_socket == INVALID_SOCKET )
		return false;

	// init address in
	m_port = port;
	initSockAddrIn( host, port, m_addrIn );

	// connect to socket
	if ( connect( m_socket, (const SOCKADDR *) &m_addrIn, sizeof(m_addrIn) ) == SOCKET_ERROR )
		return false;
	
	return true;
}

void CSoc::Close()
{
	// close socket if open
	if ( m_socket != INVALID_SOCKET )
	{
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}
}

long CSoc::Recieve(LPTSTR pBuffer, DWORD dwLen)
{
	return recv(m_socket, pBuffer, dwLen, 0);
}

long CSoc::Send(LPCTSTR pBuffer, DWORD dwLen)
{
	return send(m_socket, pBuffer, dwLen, 0);
}

bool CSoc::IsValid()
{
    return (m_socket != INVALID_SOCKET);
}

bool CSoc::Create (SOCKET &sock, SOCKADDR &addr)
{
    if (sock == INVALID_SOCKET)
        return false;
	memcpy(&m_addrIn, (SOCKADDR *) &addr, sizeof(addr) );
	memcpy(&m_socket, &sock, sizeof(m_socket) );
    return true;
}


// static *****************************
bool CSoc::Init ()
{
	#if defined( _WIN32 )
	WSADATA WSAData;
	WSAStartup( MAKEWORD(2,0), &WSAData );
	#endif
	return true;
}

void CSoc::Destroy()
{
	#if defined( _WIN32 )
    WSACleanup();
    #endif
}

int CSoc::GetLastError()
{
    return WSAGetLastError();
}

bool CSoc::initSockAddrIn(string & host, long port, SOCKADDR_IN & addrIn)
{
	// init socket in struct
	memset(&addrIn,0,sizeof(addrIn));

	// set family
	addrIn.sin_family = AF_INET;

	if ( !host.empty() )
	{
		// retrieve the host data corresponding to the host name
		LPHOSTENT phost;
		if ((phost = gethostbyname(host.c_str())) != NULL) 
		{
			// assign the socket IP address. 
			memcpy ( (char FAR *) &(addrIn.sin_addr), 
					  phost->h_addr, 
					  phost->h_length );
		}
		else
		{
			// get inet address
			addrIn.sin_addr.s_addr = inet_addr(host.c_str());
		}
	}
	else
	{
		// set inet address
		addrIn.sin_addr.s_addr = htonl(INADDR_ANY);
	}

	// set port
	addrIn.sin_port = htons(port);

	return true;
}

SOCKET CSoc::GetSocket() const
{
    return m_socket;
}

void CSoc::SetSocket(SOCKET sock)
{
    m_socket = sock;
}

const SOCKADDR_IN* CSoc::GetSockAddrIn()
{
    return &m_addrIn;
}

int CSoc::SetOOB()
{
    BOOL b = TRUE;
    return setsockopt(m_socket, SOL_SOCKET, SO_OOBINLINE, (char*)&b, sizeof(b));
}
