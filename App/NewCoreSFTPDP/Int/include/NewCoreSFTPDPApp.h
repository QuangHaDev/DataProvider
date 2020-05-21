//
// NewCoreSFTPDPApp.h
// 
// Created by Ha Do on March 18, 2019
// Modified by Ha Do on March 18, 2019
// Copyright (c) 2018-18 N2N-AFE (Hong Kong) Limited. All rights reserved.
//

#pragma once
#include <map>
#include "ssThread.h"
#include "DataCtrl.h"
#include "Dcache.h"
#include "MsgObject.h"
#include "NewCoreSFTPHandler.h"
#include "ApplManClient.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////
class NewCoreSFTPDPApp : public ssThread
{
public: 
	NewCoreSFTPDPApp(char* szNCSFTPDPCfg, char* szGlobalCfg);
	virtual ~NewCoreSFTPDPApp();
	void Main();
	void Rebuild();

private:
#pragma region Function
	void Initialize();
	void InitInstance();
	void ExitInstance();
	void InitDataCtrl();
	void InitHandlers();

	bool IsHoliday(unsigned long ulDate = 0);
	bool ReadHoliday(CString filename = "../../Common/Holiday.txt");
	bool SendData();
	void BackgroundCycle();
	void BackgroundCyclePrepare();
	void BackgroundCycleProcess();
	void ClosingRun();
#pragma endregion

private:
#pragma region Function
	void InitData();
	CString GetSvrStatusName(int SvrStatus);
	void ReadConfigINI();

	static void HandleDataReceiveNCDP(COP_ITEM &item, void* pData);
	void InitProfileList();
	void GenericServerHost();
#pragma endregion

#pragma region Params
	ULONG		m_ulOffsetStart;
	ULONG		m_ulOffsetEnd;

	CDCache*	m_pDCache;
	CDataCtrl	m_ReceiveCtrl;
	CDataCtrl	m_SendCtrl;
	char		m_SFTPUrl[512];

	// Global config
	char	m_szNCSFTPDPCfg[512];
	char	m_szGlobalCfg[512];
	int		m_nDPIns;
	char	m_szReceiveIP[256];
	char	m_szSendLocalIP[256];
	int		m_SendPort;

	char	m_SFTPServerIP[256];
	long	m_SFTPServerPort;
	char	m_SFTPServerUserID[256];
	char	m_SFTPServerPwd[256];
	char	m_SFTPServerPath[256];

	int		m_NCSourcePort;
	char	m_NCSoourceGroup1[256];
	char	m_NCSoourceGroup2[256];
	
	// App local config
	int		m_nDPID;
	char	m_SendGroup[256];
	int		m_BackgroundCycleEnable;
	int		m_BackgroundCycleSendRate;
	int		m_ClosingRunTime;

	int		m_ReadCycle;
	int		m_TargetDate;
	char	m_LocalPath[256];

	char	m_ProfileName1[128];
	char	m_ProfilePath1[256];
	int		m_ProfileBuildMap1;
	int		m_ProfileSaveLocal1;

	char	m_ProfileName2[128];
	char	m_ProfilePath2[256];
	int		m_ProfileBuildMap2;
	int		m_ProfileSaveLocal2;

	int		m_nDPStatus;

	// background cycle
	long								m_lTimespan;
	CDWordArray							m_cdwaBackgroundList;
	CDWordArray							m_cdwaFidList;

	CMap<long, long, long, long>		m_mapHoliday;
	ssEvent								m_RebuildEvent;
	NewCoreSFTPHandler*					m_NCSFTPHandler;
	CFIFOx<unsigned long>				m_PageNumQueue;
	CFIFOx<COP_ITEM>					m_CopItemQueue;
	unsigned long						m_ulPageNo;

	fidDatabase db;
#pragma endregion
};