//
// NewCoreSFTPDPMain.cpp
// 
// Created by Ha Do on March 18, 2019
// Modified by Ha Do on March 18, 2019
// Copyright (c) 2018-18 N2N-AFE (Hong Kong) Limited. All rights reserved.
//

#include "stdafx.h"
#include "ApplManClient.h"
#include "NewCoreSFTPDPApp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CWinApp theApp;
using namespace std;

int main(int argc, char* argv[])
{
	if (argc <= 1)
	{
		CApplManClient::Log(LOG_ERROR, "Usage: %s <AppId>", argv[0]);
		return -1;
	}

	if (!CApplManClient::Initialize((DWORD)atol(argv[1])))		// initialize the ApplManClient
		return -1;
	CApplManClient::SetTraceOff();
#ifdef _DEBUG
	CApplManClient::SetTraceOn();
#endif
	CApplManClient::Log(LOG_INFO, "NewCoreSFTPDPApp Startup!");

	char GlobalCfg[512], NewCoreSFTPDPCfg[512];

	strcpy_s(GlobalCfg, "../../Common/GlobalConfig.ini");
	strcpy_s(NewCoreSFTPDPCfg, "./Config/NewCoreSFTPDPCfg.ini");

	// NewCoreSFTPDPApp
	NewCoreSFTPDPApp* pNewCoreSFTPDPApp = new NewCoreSFTPDPApp(NewCoreSFTPDPCfg, GlobalCfg);

	BOOL bExit = FALSE;
	while (!bExit)
	{
		int iEvent = CApplManClient::WaitForEvents();
		switch (iEvent)
		{
		case CHECKALIVE: {
							 if (pNewCoreSFTPDPApp->IsAlive())
								 CApplManClient::AckAlive();
		}break;

		case SHUTDOWN:
		case REMOTE_SHUTDOWN: {
								  CApplManClient::Log(LOG_INFO, "Shutdown NewCoreSFTPDP!");
								  pNewCoreSFTPDPApp->ExitThread();
								  if (!pNewCoreSFTPDPApp->WaitForExit(CHECKALIVE_TIMEOUT))
									  CApplManClient::Log(LOG_ERROR, "Exit Thread Failed!");
								  bExit = TRUE;
		}break;

		case REBUILD:
			pNewCoreSFTPDPApp->Rebuild();
			break;
		default:
			break;
		}
	}

	delete pNewCoreSFTPDPApp;
	CApplManClient::Uninitialize();

	return 0;
}