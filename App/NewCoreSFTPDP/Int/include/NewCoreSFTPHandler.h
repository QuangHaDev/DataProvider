//
// NewCoreSFTPDPHandler.h
// 
// Created by Ha Do on March 18, 2019
// Modified by Ha Do on March 18, 2019
// Copyright (c) 2018-18 N2N-AFE (Hong Kong) Limited. All rights reserved.
//

#pragma once
#include <map>
#include "ssThread.h"
#include "FIFOx.h"
#include "MsgObject.h"
#include "Dcache.h"
#include <string>
#include "SFTPCtrl.h"

using namespace std;

#define SYS_PAGENUM 0xFE000000

typedef struct _Profile{
	char* profileName;
	char* profilePath;
	bool profileBuildMap = 1;
	bool profileSaveLocal = 1;
} Profile;

class NewCoreSFTPHandler : public ssThread
{
public:
	NewCoreSFTPHandler();
	~NewCoreSFTPHandler();
	void Main();

	void ProcessDataReceive(COP_ITEM &item);
	void BroadcastSystemClock();
	void ClosingRun();
	void ClosingRun(unsigned long ulPage);
	void setDCache(CDCache* pDCache);
	void setCopItemQueue(CFIFOx<COP_ITEM>* copItemQueue);

	void setSFTPServerHost(char* url);
	void setSFTPServerPort(long port);
	void setSFTPUserID(char* userID);
	void setSFTPUserPw(char* userPw);
	void setProfileList(std::vector<Profile> profileList);
	void setReadCycle(int readCycle);
	void setLocalPath(char* localPath);

private:

#pragma region Params
	// sftp data
	char*		m_SFTPServerHost;
	long		m_SFTPServerPort;
	char*		m_SFTPUserID;
	char*		m_SFTPUserPW;
	int			m_ReadCycle;
	char*		m_LocalPath;

	SFTPCtrl*	m_SFTPCtrl;
	int			m_ProfileBuildMap;

	fidDatabase db;
	CDCache*	m_pDCache;

	std::vector<Profile>				m_ProfileList;
	std::map<string, int>				m_SeqNumPerDataset;
	std::map<string, unsigned long>		m_LocalMapping;
	std::map<string, string>			m_MapISINRicname;
	CFIFOx<COP_ITEM>*					m_pCopItemQueue;
	CFIFOx<unsigned long>				m_PageNumQueue;
#pragma endregion

#pragma region Function
	void DownloadDataset();
	void OnDownloadCompleted(string& data);

	void ClosingRunEquity(unsigned long pageNo);

	void ProcessDataset(string data);
	void Process_ChangeOfContract(string data);
	void Process_DToMBR(string data);
	void Process_SBRToMBR(string data);
	void Process_DelistedInstrument(string data);
	void Process_TradingSchedule(string data);
	void Process_Holiday(string data);
	void Process_MemberTrader(string data);
	void Process_MarketMakingContract(string data);
	void Process_ETFIndexComposition(string data);
	void Process_CouponPaymentDate(string data);

	bool ValidateRow(string data);
	string GetSeqNum(string data);
	string GetTransactionCode(string data);
	bool ValidateSeqNum(string transCode, int seqNum);
	long GetPageNumberByRic(string ric);
	string getFieldData(string data, string transCode, int seq);

#pragma endregion
};