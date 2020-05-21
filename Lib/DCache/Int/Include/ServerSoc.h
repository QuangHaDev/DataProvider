// ServerSoc.h: interface for the CServerSoc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVERSOC_H__CDBA254C_B374_443C_B358_6C9F48C66624__INCLUDED_)
#define AFX_SERVERSOC_H__CDBA254C_B374_443C_B358_6C9F48C66624__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Soc.h"

class CServerSoc : public CSoc  
{
public:
	CServerSoc(int nPendConn);
	~CServerSoc();

    bool Open(long lPort);
    bool Listen();
    bool Accept(CSoc &socket, char* pAddr = NULL);

private:
    int m_nPendConn;
};

#endif // !defined(AFX_SERVERSOC_H__CDBA254C_B374_443C_B358_6C9F48C66624__INCLUDED_)
