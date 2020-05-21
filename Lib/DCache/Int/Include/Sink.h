// Sink.h: interface for the CSink class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SINK_H__1D51EB2F_4B61_4E1A_B08F_AAB59F8FA333__INCLUDED_)
#define AFX_SINK_H__1D51EB2F_4B61_4E1A_B08F_AAB59F8FA333__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "serversoc.h"
using namespace NGN2;

class CDCache;
class CSink : public CServerSoc
{
public:
	CSink(CDCache*	pMyself);
	virtual ~CSink();

    static UINT APIENTRY ListenThreadFunc(LPVOID lpData);
    void ListenFunc();
    bool Start(int nPort);

protected:
    HANDLE m_hListenThread;
    HANDLE m_hExit;
    HANDLE m_hTimeout;

private:
	CDCache*	m_pMyself;
};

#endif // !defined(AFX_SINK_H__1D51EB2F_4B61_4E1A_B08F_AAB59F8FA333__INCLUDED_)
