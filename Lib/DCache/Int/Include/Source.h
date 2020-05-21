// Source.h: interface for the CSource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOURCE_H__D65E7F7A_78D6_4694_A37B_0774B57A4D20__INCLUDED_)
#define AFX_SOURCE_H__D65E7F7A_78D6_4694_A37B_0774B57A4D20__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "soc.h"
using namespace NGN2;

class CDCache;
class CSource  
{
public:
	CSource();
	virtual ~CSource();

    void	Main();
    void	Start(string host, long port, CDCache* pMyself);
    void	Stop();
    BOOL 	CheckAlive() {return m_bAlive;}
    static UINT APIENTRY ThreadFunc(LPVOID lpVoid);

private:
	CDCache* m_DCache;
    CSoc	m_socket;
    HANDLE	m_hThread;
    string	m_strHost;
    int		m_nPort;
    HANDLE	m_hExit;
    int		m_nRecv;
	BOOL	m_bAlive;
};

#endif // !defined(AFX_SOURCE_H__D65E7F7A_78D6_4694_A37B_0774B57A4D20__INCLUDED_)
