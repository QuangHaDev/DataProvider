// Soc.h: interface for the CSoc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOC_H__0C71B997_2780_4DEA_8ACF_9D613B117D76__INCLUDED_)
#define AFX_SOC_H__0C71B997_2780_4DEA_8ACF_9D613B117D76__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

using namespace std;

class CSoc  
{
public:
	CSoc();
	~CSoc();

protected:
	SOCKET			m_socket;
	SOCKADDR_IN		m_addrIn;
	long			m_port;

public:
    bool Open(string host, long port);
    void Close();
    bool IsValid();
    long Recieve(LPTSTR pBuffer, DWORD dwLen);
    long Send(LPCTSTR pBuffer, DWORD dwLen);
    bool Create (SOCKET &sock, SOCKADDR &addr);
    void SetSocket(SOCKET sock);
    SOCKET GetSocket() const;
    const SOCKADDR_IN* GetSockAddrIn();
    int SetOOB();

	static bool Init();
    static void Destroy();
    static int GetLastError();
    static bool initSockAddrIn(string & host, long port, SOCKADDR_IN & addrIn);
};

#endif // !defined(AFX_SOC_H__0C71B997_2780_4DEA_8ACF_9D613B117D76__INCLUDED_)
