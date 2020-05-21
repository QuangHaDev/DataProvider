//
// NewCoreSFTPDPApp.cpp
// 
// Created by Ha Do on March 18, 2019
// Modified by Ha Do on March 18, 2019
// Copyright (c) 2018-18 N2N-AFE (Hong Kong) Limited. All rights reserved.
//

#include "stdafx.h"
#include "NewCoreSFTPDPApp.h"
#include <functional>

#define SECTION_COMMON			"Common"
#define SECTION_NEWCORESFTPDP	"NewCoreSFTPDP"
#define SECTION_SETTING			"Settings"
#define SECTION_PROFILES		"Profiles"

NewCoreSFTPDPApp::NewCoreSFTPDPApp(char* szNCSFTPDPCfg, char* szGlobalCfg)
{
	InitData();

	strcpy_s(m_szNCSFTPDPCfg, szNCSFTPDPCfg);
	strcpy_s(m_szGlobalCfg, szGlobalCfg);

	Initialize();
}

NewCoreSFTPDPApp::~NewCoreSFTPDPApp()
{
	CApplManClient::Log(LOG_NORMAL, "~NewCoreSFTPDPApp!!!!");
	if (m_NCSFTPHandler) {
		delete m_NCSFTPHandler;
		m_NCSFTPHandler = NULL;
	}

	ExitInstance();

	if (!ssThread::IsExit()) {
		ssThread::ExitThread();
	}
	ssThread::WaitForExit(CHECKALIVE_TIMEOUT);

	if (m_pDCache != NULL) {
		m_pDCache->ForceWriteToDisk();
		delete m_pDCache;
		m_pDCache = NULL;
	}
}

void NewCoreSFTPDPApp::Main()
{
	CApplManClient::Log(LOG_NORMAL, "[Main] Thread Started [ID:0x%x]", ssThread::GetID());
	enum {
		TimerEvent = 100,
		AliveEvent = 200,
		RebuildEvent = 300,
		ExitEvent = 400,
		MinuteEvent = 500,
	};
	ssMultiWait multiWait;

	CString szSvrStatusText;
	ssTimer m_Timer;
	m_Timer.Set(3000);
	ssTimer m_MinuteTimer;
	m_MinuteTimer.Set(60000);

	multiWait.Add(m_RebuildEvent, RebuildEvent);
	multiWait.Add(ssThread::m_CheckAliveEvent, AliveEvent);
	multiWait.Add(ssThread::m_ExitEvent, ExitEvent);
	multiWait.Add(m_Timer, TimerEvent);
	multiWait.Add(m_MinuteTimer, MinuteEvent);

	bool bDoneClosing = false;
	while (!IsExit()) {
		switch (multiWait.Wait(0)) {
		case AliveEvent: {
							 // Handler.isAlive();
							 AckAlive();
		} break;

		case ExitEvent: {
							CApplManClient::Log(LOG_NORMAL, "[Main] Exiting");
							return;
		} break;

		case RebuildEvent: {
							   CApplManClient::RebuildCompleted();	// if rebuild completed, ack the ApplManClient()
		} break;

		case TimerEvent: {
							 int nDPS = m_SendCtrl.GetSvrStatus();
							 szSvrStatusText = GetSvrStatusName(nDPS);

							 CApplManClient::SetDataCtrlStatus(nDPS);
							 if (m_nDPStatus != nDPS) {
								 CApplManClient::Log(LOG_NORMAL, "State: %s", szSvrStatusText);
								 m_nDPStatus = nDPS;
							 }
							 m_NCSFTPHandler->BroadcastSystemClock();
		} break;

		case MinuteEvent: {
							  CTime t = CTime::GetCurrentTime();
							  int now = t.GetHour() * 100 + t.GetMinute();
							  if (now == m_ClosingRunTime) {
								  if (!bDoneClosing) {
									  bDoneClosing = true;
									  ClosingRun();
								  }
							  }
							  else {
								  bDoneClosing = false;
							  }
		} break;

		default: {
					 BackgroundCycle();
					 if (!SendData()){
						 Sleep(1);
					 }
		} break;
		}
	}
}

void NewCoreSFTPDPApp::Rebuild()
{
	CApplManClient::Log(LOG_NORMAL, "NewCoreSFTPDP Rebuild Started");
	m_RebuildEvent.Set();
}

void NewCoreSFTPDPApp::InitData()
{
	m_nDPID = 0;
	m_nDPIns = 0;
	m_lTimespan = 0;
	m_NCSourcePort = 0;
	m_BackgroundCycleEnable = 1;
	m_BackgroundCycleSendRate = 1000;
	m_ClosingRunTime = 0700;
	m_ReadCycle = 30000;
	m_TargetDate = 0;
	m_SendPort = 7000;
}

CString NewCoreSFTPDPApp::GetSvrStatusName(int SvrStatus)
{
	CString sz_status;
	switch (SvrStatus)
	{
	case CDataCtrl::DC_STATUS_UNKNOWN:		sz_status = "Unknow"; break;
	case CDataCtrl::DC_STATUS_LISTEN:		sz_status = "Listen"; break;
	case CDataCtrl::DC_STATUS_RAISE_HAND:	sz_status = "Raise hand"; break;
	case CDataCtrl::DC_STATUS_PRIMARY:		sz_status = "Primary"; break;
	case CDataCtrl::DC_STATUS_DISABLE:		sz_status = "Disable"; break;
	}
	return sz_status;
}

void NewCoreSFTPDPApp::Initialize()
{
	InitInstance();
	Start();
}

void NewCoreSFTPDPApp::InitInstance()
{
	ReadHoliday();
	ReadConfigINI();

#pragma region Setup DCache
	CWordArray cwaSaveData;
	cwaSaveData.Add(FID_REC_TYPE);
	cwaSaveData.Add(FID_RICNAME);

	char szDCacheName[1024] = "./newCoreDCache";	// TODO: config if necessary
	m_pDCache = new CDCache(true, szDCacheName, &cwaSaveData);
#pragma endregion

	InitHandlers();
	InitDataCtrl();
}

void NewCoreSFTPDPApp::ExitInstance()
{
	m_ReceiveCtrl.DelMonitorChannel(m_szReceiveIP, m_NCSourcePort);
	m_SendCtrl.DelChannel(m_szSendLocalIP, m_NCSourcePort);

	CApplManClient::Log(LOG_NORMAL, "DataCtrl Stopped!");
}

bool NewCoreSFTPDPApp::ReadHoliday(CString filename)
{
	bool bReturnValue = false;
	FILE *holiday_fp;

	if ((holiday_fp = fopen(filename, "r")) != NULL) {
		char date_str[FILENAME_MAX + 1];
		unsigned long hdate;
		while (!feof(holiday_fp)) {
			if (fgets(date_str, FILENAME_MAX + 1, holiday_fp) == NULL)
				continue;
			if (sscanf(date_str, "%lu", &hdate) != 1)
				continue;

			m_mapHoliday[hdate] = NULL;
			bReturnValue = true;
		} /* while */
		fclose(holiday_fp);
	}
	else {
		CApplManClient::Log(LOG_ERROR, "Unable to read holiday file! (%s) Default to normal trading day.", filename);
	}

	if (bReturnValue) {
		CApplManClient::Log(LOG_NORMAL, "Read holiday file (%s) successfully.", filename);
	}
	return bReturnValue;
}

bool NewCoreSFTPDPApp::SendData()
{
	COP_ITEM copItem = NULL;
	if (!m_CopItemQueue.IsEmpty()) {
		if (m_CopItemQueue.GetItem(copItem)) {
			m_SendCtrl.Send(copItem);
			return true;
		}
	}
	return false;
}

void NewCoreSFTPDPApp::BackgroundCycle()
{
	if (m_BackgroundCycleEnable != 1 || m_SendCtrl.GetSvrStatus() != CDataCtrl::DC_STATUS_PRIMARY) {
		return;
	}

	long now = GetTickCount(); // GetTickCount overflows roughly every 49 days, to avoid this error reset system within 48 day.
	if (now - m_lTimespan <= m_BackgroundCycleSendRate) {
		return;
	}
	m_lTimespan = now;

	if (m_PageNumQueue.IsEmpty()) {
		BackgroundCyclePrepare();
	}
	BackgroundCycleProcess();

}

void NewCoreSFTPDPApp::BackgroundCycleProcess()
{
	short sFid;

	if (!m_PageNumQueue.IsEmpty()) {
		m_PageNumQueue.GetItem(m_ulPageNo);
		//m_NCSFTPHandler->BackgroundCycle(m_ulPageNo);
		m_pDCache->GetFIDList(m_ulPageNo, m_cdwaFidList);

		while (m_cdwaFidList.GetSize() > 0) {
			sFid = m_cdwaFidList.GetAt(0);

			fidDatabase fd;
			short nFidType = fd.GetFidType(sFid);

			switch (nFidType) {
			case NGN_LINK:
			case NGN_TRAN:
			case NGN_QUOTELIST:  break;
			default: {
					   long lTmp = -1;
					   COP_ITEM cop;
					   if (m_pDCache->GetAField(m_ulPageNo, cop, (FID_DEFINITION)sFid, lTmp))
					   {
						   cop->SetItemNo(m_ulPageNo);
						   cop->SetDPID(m_nDPID);
						   cop->SetDPInsID(m_nDPIns);
						   cop->SetMsgType(MGT_VERIFYSYNC);
						   cop->GetMessage();
						   m_SendCtrl.Send(cop);
					   }
			} break;
			}
			m_cdwaFidList.RemoveAt(0);
		}
	}

	if (m_cdwaFidList.IsEmpty() && m_PageNumQueue.IsEmpty())
	{
		CApplManClient::Log(LOG_NORMAL, "[NewCoreSFTPDP] New Background Cycle DONE !");
	}
}

void NewCoreSFTPDPApp::BackgroundCyclePrepare()
{
	// Get all PageNum from DCache put to PageNumQueue.
	if (m_PageNumQueue.IsEmpty()) {
		CApplManClient::Log(LOG_NORMAL, "[NewCoreSFTPDP] New Background Cycle START !");
		m_pDCache->GetPageList(m_cdwaBackgroundList);
		while (m_cdwaBackgroundList.GetSize() > 0) {
			m_ulPageNo = m_cdwaBackgroundList[0];
			m_cdwaBackgroundList.RemoveAt(0);
			m_PageNumQueue.AddItem(m_ulPageNo);
		}
	}
}

void NewCoreSFTPDPApp::ClosingRun()
{
	if (IsHoliday()) // we don't do closing run in holiday
		return;

	CDWordArray m_cdwaPageList;
	if (!m_pDCache->GetPageList(m_cdwaPageList) || m_cdwaFidList.GetSize() == 0) {
		return;	// No page found
	}

	int size = m_cdwaPageList.GetSize() > 0 ? m_cdwaPageList.GetSize() : 0;
	CApplManClient::Log(LOG_NORMAL, "Closing Started: TotalPages = %d", size);

	if (m_NCSFTPHandler != NULL) {
		m_NCSFTPHandler->ClosingRun();
	}

	for (int i = 0; i<size; ++i) {
		if (m_NCSFTPHandler != NULL)
			m_NCSFTPHandler->ClosingRun(m_cdwaPageList[i]);
	}

	CApplManClient::Log(LOG_NORMAL, "Closing Completed");
}

void NewCoreSFTPDPApp::ReadConfigINI()
{
#pragma region Global Config
	m_nDPIns = GetPrivateProfileInt(SECTION_COMMON, "DPIns", 0, m_szGlobalCfg);
	GetPrivateProfileString(SECTION_COMMON, "ReceiveIP", "", m_szReceiveIP, sizeof(m_szReceiveIP), m_szGlobalCfg);
	GetPrivateProfileString(SECTION_COMMON, "SendIP", "", m_szSendLocalIP, sizeof(m_szSendLocalIP), m_szGlobalCfg);
	CApplManClient::Log(LOG_NORMAL, "GlobalConfig: DPIns:%d ReceiveIP:%s SendIP:%s", m_nDPIns, m_szReceiveIP, m_szSendLocalIP);

	m_SFTPServerPort = GetPrivateProfileInt(SECTION_NEWCORESFTPDP, "SFTPServerPort", 0, m_szGlobalCfg);
	GetPrivateProfileString(SECTION_NEWCORESFTPDP, "SFTPServerIP", "", m_SFTPServerIP, sizeof(m_SFTPServerIP), m_szGlobalCfg);
	GetPrivateProfileString(SECTION_NEWCORESFTPDP, "SFTPUserID", "", m_SFTPServerUserID, sizeof(m_SFTPServerUserID), m_szGlobalCfg);
	GetPrivateProfileString(SECTION_NEWCORESFTPDP, "SFTPUserPW", "", m_SFTPServerPwd, sizeof(m_SFTPServerPwd), m_szGlobalCfg);
	GetPrivateProfileString(SECTION_NEWCORESFTPDP, "SFTPPath", "", m_SFTPServerPath, sizeof(m_SFTPServerPath), m_szGlobalCfg);
	CApplManClient::Log(LOG_NORMAL, "GlobalConfig: SFTPServerIP:%s SFTPServerPort:%d SFTPUserID:%s SFTPUserPW:%s SFTPPath:%s",
		m_SFTPServerIP, m_NCSourcePort, m_SFTPServerUserID, m_SFTPServerPwd, m_SFTPServerPath);

	m_NCSourcePort = GetPrivateProfileInt(SECTION_NEWCORESFTPDP, "NCSrcPort", 0, m_szGlobalCfg);
	GetPrivateProfileString(SECTION_NEWCORESFTPDP, "NCSrcGroup1", "", m_NCSoourceGroup1, sizeof(m_NCSoourceGroup1), m_szGlobalCfg);
	GetPrivateProfileString(SECTION_NEWCORESFTPDP, "NCSrcGroup2", "", m_NCSoourceGroup2, sizeof(m_NCSoourceGroup2), m_szGlobalCfg);
	CApplManClient::Log(LOG_NORMAL, "GlobalConfig: NCSrcPort:%d NCSrcGroup1:%s NCSrcGroup2:%s",
		m_NCSourcePort, m_NCSoourceGroup1, m_NCSoourceGroup2);
#pragma endregion


#pragma region App Local Config
	m_nDPID = GetPrivateProfileInt(SECTION_SETTING, "DPID", 0, m_szNCSFTPDPCfg);
	m_nDPIns = GetPrivateProfileInt(SECTION_SETTING, "DPIns", 0, m_szNCSFTPDPCfg);
	GetPrivateProfileString(SECTION_SETTING, "SendIP", "", m_szSendLocalIP, sizeof(m_szSendLocalIP), m_szNCSFTPDPCfg);
	GetPrivateProfileString(SECTION_SETTING, "SendGroup", "", m_SendGroup, sizeof(m_SendGroup), m_szNCSFTPDPCfg);
	m_BackgroundCycleEnable = GetPrivateProfileInt(SECTION_SETTING, "BackgroundCycleEnable", 1, m_szNCSFTPDPCfg);
	m_BackgroundCycleSendRate = GetPrivateProfileInt(SECTION_SETTING, "BackgroundCycleSendRate", 1000, m_szNCSFTPDPCfg);
	m_ClosingRunTime = GetPrivateProfileInt(SECTION_SETTING, "ClosingRunTime", 0700, m_szNCSFTPDPCfg);
	CApplManClient::Log(LOG_NORMAL, "NCSFTPDPConfig: DPID:%d DPIns:%d SendIP:%s SendGroup:%s BackgroundCycleEnable:%d BackgroundCycleSendRate:%d ClosingRunTime:%d",
		m_nDPID, m_nDPIns, m_szSendLocalIP, m_SendGroup, m_BackgroundCycleEnable, m_BackgroundCycleSendRate, m_ClosingRunTime);

	m_SFTPServerPort = GetPrivateProfileInt(SECTION_NEWCORESFTPDP, "SFTPServerPort", 0, m_szNCSFTPDPCfg);
	GetPrivateProfileString(SECTION_NEWCORESFTPDP, "SFTPServerIP", "", m_SFTPServerIP, sizeof(m_SFTPServerIP), m_szNCSFTPDPCfg);
	GetPrivateProfileString(SECTION_NEWCORESFTPDP, "SFTPUserID", "", m_SFTPServerUserID, sizeof(m_SFTPServerUserID), m_szNCSFTPDPCfg);
	GetPrivateProfileString(SECTION_NEWCORESFTPDP, "SFTPUserPW", "", m_SFTPServerPwd, sizeof(m_SFTPServerPwd), m_szNCSFTPDPCfg);
	GetPrivateProfileString(SECTION_NEWCORESFTPDP, "SFTPPath", "", m_SFTPServerPath, sizeof(m_SFTPServerPath), m_szNCSFTPDPCfg);
	CApplManClient::Log(LOG_NORMAL, "NCSFTPDPConfig: SFTPServerIP:%s SFTPServerPort:%d SFTPUserID:%s SFTPUserPW:%s SFTPPath:%s",
		m_SFTPServerIP, m_SFTPServerPort, m_SFTPServerUserID, m_SFTPServerPwd, m_SFTPServerPath);

	m_NCSourcePort = GetPrivateProfileInt(SECTION_NEWCORESFTPDP, "NCSrcPort", 0, m_szNCSFTPDPCfg);
	GetPrivateProfileString(SECTION_NEWCORESFTPDP, "NCSrcGroup1", "", m_NCSoourceGroup1, sizeof(m_NCSoourceGroup1), m_szNCSFTPDPCfg);
	GetPrivateProfileString(SECTION_NEWCORESFTPDP, "NCSrcGroup2", "", m_NCSoourceGroup2, sizeof(m_NCSoourceGroup2), m_szNCSFTPDPCfg);
	CApplManClient::Log(LOG_NORMAL, "NCSFTPDPConfig: NCSrcPort:%d NCSrcGroup1:%s NCSrcGroup2:%s",
		m_NCSourcePort, m_NCSoourceGroup1, m_NCSoourceGroup2);

	m_ReadCycle = GetPrivateProfileInt(SECTION_PROFILES, "ReadCycle", 0, m_szNCSFTPDPCfg);
	m_TargetDate = GetPrivateProfileInt(SECTION_PROFILES, "TargetDate", 0, m_szNCSFTPDPCfg);
	GetPrivateProfileString(SECTION_PROFILES, "LocalPath", "", m_LocalPath, sizeof(m_LocalPath), m_szNCSFTPDPCfg);
	CApplManClient::Log(LOG_NORMAL, "NCSFTPDPConfig: ReadCycle:%d TargetDate:%d LocalPath:%s",
		m_ReadCycle, m_TargetDate, m_LocalPath);

	GetPrivateProfileString(SECTION_PROFILES, "ProfileName1", "", m_ProfileName1, sizeof(m_ProfileName1), m_szNCSFTPDPCfg);
	GetPrivateProfileString(SECTION_PROFILES, "ProfilePath1", "", m_ProfilePath1, sizeof(m_ProfilePath1), m_szNCSFTPDPCfg);
	m_ProfileBuildMap1 = GetPrivateProfileInt(SECTION_PROFILES, "ProfileBuildMap1", 0, m_szNCSFTPDPCfg);
	m_ProfileSaveLocal1 = GetPrivateProfileInt(SECTION_PROFILES, "ProfileSaveLocal1", 0, m_szNCSFTPDPCfg);
	CApplManClient::Log(LOG_NORMAL, "NCSFTPDPConfig: ProfileName1:%s ProfilePath1:%s ProfileBuildMap1:%d ProfileSaveLocal1:%d",
		m_ProfileName1, m_ProfilePath1, m_ProfileBuildMap1, m_ProfileSaveLocal1);

	GetPrivateProfileString(SECTION_PROFILES, "ProfileName2", "", m_ProfileName2, sizeof(m_ProfileName2), m_szNCSFTPDPCfg);
	GetPrivateProfileString(SECTION_PROFILES, "ProfilePath2", "", m_ProfilePath2, sizeof(m_ProfilePath2), m_szNCSFTPDPCfg);
	m_ProfileBuildMap2 = GetPrivateProfileInt(SECTION_PROFILES, "ProfileBuildMap2", 0, m_szNCSFTPDPCfg);
	m_ProfileSaveLocal2 = GetPrivateProfileInt(SECTION_PROFILES, "ProfileSaveLocal2", 0, m_szNCSFTPDPCfg);
	CApplManClient::Log(LOG_NORMAL, "NCSFTPDPConfig: ProfileName2:%s ProfilePath2:%s ProfileBuildMap2:%d ProfileSaveLocal2:%d",
		m_ProfileName2, m_ProfilePath2, m_ProfileBuildMap2, m_ProfileSaveLocal2);

#pragma endregion

}

void NewCoreSFTPDPApp::InitDataCtrl()
{
	char szName[64];
	HOSTENT *pHost;
	char *p;
	struct in_addr *pInAddr;
	gethostname(szName, sizeof(szName));
	pHost = gethostbyname(szName);
	pInAddr = (struct in_addr *)*pHost->h_addr_list;
	p = inet_ntoa(*pInAddr);

	if (!strcmp(m_szSendLocalIP, "default"))
		strcpy(m_szSendLocalIP, p);

	if (m_SendCtrl.Start(m_SendPort, m_szSendLocalIP) == CMCast::MC_SUCCESS) {
		m_SendCtrl.SetSvrDPID(m_nDPID, m_nDPIns);
		m_SendCtrl.AddChannel(m_SendGroup, m_SendPort);
	}
	else {
		CApplManClient::Log(LOG_ERROR, "NewCoreSFTPDP - DataCtrl(Send) Fail to Start!");
	}

	if (m_ReceiveCtrl.Start(m_NCSourcePort, m_szReceiveIP) == CMCast::MC_SUCCESS) {
		m_ReceiveCtrl.SetSvrDPID(m_nDPID, m_nDPIns);
		m_ReceiveCtrl.AddMonitorChannel(m_NCSoourceGroup1, m_NCSourcePort);
		m_ReceiveCtrl.RegisterCallback(&NewCoreSFTPDPApp::HandleDataReceiveNCDP, m_NCSFTPHandler);
	}
	else {
		CApplManClient::Log(LOG_ERROR, "NewCoreSFTPDP - DataCtrl(Receive) Fail to Start!");
	}
}

void NewCoreSFTPDPApp::InitHandlers()
{
	// init NewCore SFTP handler
	m_NCSFTPHandler = new NewCoreSFTPHandler();
	m_NCSFTPHandler->setDCache(m_pDCache);
	m_NCSFTPHandler->setCopItemQueue(&m_CopItemQueue);

	// generic server host before send to handler
	GenericServerHost();
	m_NCSFTPHandler->setSFTPServerHost(m_SFTPUrl);
	m_NCSFTPHandler->setSFTPServerPort(m_SFTPServerPort);
	m_NCSFTPHandler->setSFTPUserID(m_SFTPServerUserID);
	m_NCSFTPHandler->setSFTPUserPw(m_SFTPServerPwd);
	m_NCSFTPHandler->setReadCycle(m_ReadCycle);
	m_NCSFTPHandler->setLocalPath(m_LocalPath);
	InitProfileList();

	m_NCSFTPHandler->Start();
}

void NewCoreSFTPDPApp::HandleDataReceiveNCDP(COP_ITEM &item, void* pData)
{
	((NewCoreSFTPHandler*)pData)->ProcessDataReceive(item);
}

void NewCoreSFTPDPApp::InitProfileList()
{
	Profile profile1;
	profile1.profilePath = m_ProfilePath1;
	profile1.profileName = m_ProfileName1;
	profile1.profileBuildMap = m_ProfileBuildMap1;
	profile1.profileSaveLocal = m_ProfileSaveLocal1;

	Profile profile2;
	profile2.profilePath = m_ProfilePath2;
	profile2.profileName = m_ProfileName2;
	profile2.profileBuildMap = m_ProfileBuildMap2;
	profile2.profileSaveLocal = m_ProfileSaveLocal2;

	std::vector<Profile> profileList;
	profileList.push_back(profile1);
	profileList.push_back(profile2);
	m_NCSFTPHandler->setProfileList(profileList);
}

bool NewCoreSFTPDPApp::IsHoliday(unsigned long ulDate)
{
	CTime t = CTime::GetCurrentTime();
	bool bHoliday = (t.GetDayOfWeek() == 1 || t.GetDayOfWeek() == 7);

	if (!bHoliday){
		unsigned long	ulTmpDate = ulDate;
		long			tmpObj = NULL;

		if (ulTmpDate == 0) {
			ulTmpDate = t.GetYear() * 10000 + t.GetMonth() * 100 + t.GetDay();
		}
		bHoliday = m_mapHoliday.Lookup(ulDate, tmpObj);
	}
	return bHoliday;
}

void NewCoreSFTPDPApp::GenericServerHost()
{
	string serverIP = m_SFTPServerIP;
	if (serverIP[serverIP.length() - 1] == '/'){
		serverIP = serverIP.substr(0, serverIP.length() - 1);
	}
	strcpy(m_SFTPUrl, serverIP.c_str());
	if (m_SFTPServerPath[0] == '/'){
		strcat(m_SFTPUrl, m_SFTPServerPath);
	}
	else{
		strcat(m_SFTPUrl, "/");
		strcat(m_SFTPUrl, m_SFTPServerPath);
	}
}