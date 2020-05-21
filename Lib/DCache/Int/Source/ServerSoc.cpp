// ServerSoc.cpp: implementation of the CServerSoc class.
//
//////////////////////////////////////////////////////////////////////
#define _CRT_SECURE_NO_WARNINGS			1

#include "stdafx.h"
#include "ServerSoc.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CServerSoc::CServerSoc(int nPendConn) :
m_nPendConn(nPendConn)
{
}

CServerSoc::~CServerSoc()
{
}

bool CServerSoc::Open(long lPort)
{
	string strHost = "";
	m_socket = socket(PF_INET,SOCK_STREAM,0);
	if ( m_socket == INVALID_SOCKET )
		return false;
	initSockAddrIn(strHost, lPort, m_addrIn );
	if ( bind( m_socket, (const SOCKADDR *) &m_addrIn, sizeof(m_addrIn) ) == SOCKET_ERROR )
		return false;
	return true;
}

bool CServerSoc::Listen()
{
    if (listen(m_socket, m_nPendConn) != 0)
        return false;
    return true;
}

bool CServerSoc::Accept(CSoc &socket, char* pAddr)
{
    SOCKADDR addr;
    int size = sizeof(addr);
    memset( &addr, 0, sizeof(addr) );

    SOCKET sock = accept(m_socket, &addr, &size);
    if ( sock == INVALID_SOCKET )
        return false;
    else
    {
        socket.Create(sock, addr);
        if (pAddr != NULL)
            sprintf(pAddr, "%d.%d.%d.%d", socket.GetSockAddrIn()->sin_addr.S_un.S_un_b.s_b1,
                                          socket.GetSockAddrIn()->sin_addr.S_un.S_un_b.s_b2,
                                          socket.GetSockAddrIn()->sin_addr.S_un.S_un_b.s_b3,
                                          socket.GetSockAddrIn()->sin_addr.S_un.S_un_b.s_b4);
        return true;
    }
}
