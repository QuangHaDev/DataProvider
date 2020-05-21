//
// NewCoreSFTPDPHandler.cpp
// 
// Created by Ha Do on March 18, 2019
// Modified by Ha Do on March 18, 2019
// Copyright (c) 2018-18 N2N-AFE (Hong Kong) Limited. All rights reserved.
//

#include "stdafx.h"
#include "NewCoreSFTPHandler.h"
#include "SFTPDatasetDef.h"

NewCoreSFTPHandler::NewCoreSFTPHandler()
{
	m_SFTPCtrl = NULL;
	m_ProfileBuildMap = 1;
}
NewCoreSFTPHandler::~NewCoreSFTPHandler()
{
	if (m_SFTPCtrl){
		delete m_SFTPCtrl;
		m_SFTPCtrl = NULL;
	}
}
void NewCoreSFTPHandler::Main()
{
	CApplManClient::Log(LOG_NORMAL, "[NewCoreSFTPHandler] Thread Started [ID:0x%x]", ssThread::GetID());

	enum {
		TimerEvent = 100,
		AliveEvent = 200,
		ExitEvent = 300,
	};
	ssMultiWait multiWait;

	ssTimer timer;
	timer.Set(m_ReadCycle);
	multiWait.Add(ssThread::m_CheckAliveEvent, AliveEvent);
	multiWait.Add(ssThread::m_ExitEvent, ExitEvent);
	multiWait.Add(timer, TimerEvent);
	while (!IsExit()) {
		switch (multiWait.Wait(1)) {
		case AliveEvent:
			AckAlive();
			break;
		case ExitEvent:
			CApplManClient::Log(LOG_NORMAL, "[NewCoreSFTPHandler] Exiting");
			break;
		case TimerEvent:
			DownloadDataset();
			break;
		default:
			break;
		}
	}
}

void NewCoreSFTPHandler::ProcessDataReceive(COP_ITEM &item)
{
	CharArray ric;
	if (!item->GetFid((short)FID_RICNAME, ric)){
		CApplManClient::Log(LOG_NORMAL, "Cannot get Ricname from received data!");
	}
	else{
		char* ricname = (char*)ric.GetString();
		unsigned long pageNo = item->GetPageNo();
		m_LocalMapping[ricname] = pageNo;
	}
}

void NewCoreSFTPHandler::BroadcastSystemClock()
{
	COP_ITEM cop = new MsgObject();
	cop->SetMsgType(MGT_SYSTEMMESSAGE);
	cop->SetItemNo(SYS_PAGENUM);
	cop->SetDelay(0);
	cop->SetItemClass(0);

	UCHAR ucTimeZone = 0;
	cop->AddFid(FID_TIME_ZONE, &ucTimeZone);

	CTime tSystemTime = CTime::GetCurrentTime();
	BTimeSec bTimeSec;
	bTimeSec.SetHH(tSystemTime.GetHour());
	bTimeSec.SetMIN(tSystemTime.GetMinute());
	bTimeSec.SetSS(tSystemTime.GetSecond());
	cop->AddFid(FID_SYS_TIME, &bTimeSec);

	BDateYear bDateYear;
	bDateYear.SetYear(tSystemTime.GetYear());
	bDateYear.SetMonth(tSystemTime.GetMonth());
	bDateYear.SetDay(tSystemTime.GetDay());
	cop->AddFid(FID_SYS_DATE, &bDateYear);

	m_pCopItemQueue->AddItem(cop);
}

void NewCoreSFTPHandler::ClosingRun()
{
	m_ProfileList.clear();
	m_SeqNumPerDataset.clear();
	m_LocalMapping.clear();
	m_MapISINRicname.clear();
	//m_pCopItemQueue->Clear();
	//m_PageNumQueue.Clear();
}

void NewCoreSFTPHandler::ClosingRun(unsigned long ulPage)
{
	char rec = 0;
	if (!m_pDCache->GetAField(ulPage, rec, FID_REC_TYPE, -1)) {
		rec = 0;
	}

	// TODO: Check exception Fids

	switch (rec) {
	case 4:		//ClosingRunIndexes(lPage); break;
	case 8:		//ClosingRunDerivatives(lPage); break;
	case 1:
	case 2:		// Warrant
	case 5:
	case 6:
	default:
		ClosingRunEquity(ulPage);
		break;
	}
}

void NewCoreSFTPHandler::ClosingRunEquity(unsigned long pageNo)
{
	// TODO
}

void NewCoreSFTPHandler::setDCache(CDCache* pDCache)
{
	m_pDCache = pDCache;
}

void NewCoreSFTPHandler::setCopItemQueue(CFIFOx<COP_ITEM>* copItemQueue)
{
	m_pCopItemQueue = copItemQueue;
}

void NewCoreSFTPHandler::setSFTPServerHost(char* url)
{
	m_SFTPServerHost = url;
}

void NewCoreSFTPHandler::setSFTPServerPort(long port)
{
	m_SFTPServerPort = port;
}

void NewCoreSFTPHandler::setSFTPUserID(char* userID)
{
	m_SFTPUserID = userID;
}

void NewCoreSFTPHandler::setSFTPUserPw(char* userPw)
{
	m_SFTPUserPW = userPw;
}

void NewCoreSFTPHandler::setProfileList(std::vector<Profile> profileList)
{
	m_ProfileList = profileList;
}

void NewCoreSFTPHandler::setReadCycle(int readCycle)
{
	m_ReadCycle = readCycle;
}

void NewCoreSFTPHandler::setLocalPath(char* localPath)
{
	m_LocalPath = localPath;
}

void NewCoreSFTPHandler::DownloadDataset()
{
	if (!m_SFTPCtrl){
		m_SFTPCtrl = new SFTPCtrl(m_SFTPServerHost, m_SFTPServerPort, m_SFTPUserID, m_SFTPUserPW);
		m_SFTPCtrl->setLocalDest(m_LocalPath);
		m_SFTPCtrl->setOnDownloadCompleted(std::bind(&NewCoreSFTPHandler::OnDownloadCompleted, this, std::placeholders::_1));
		m_SFTPCtrl->Start();
	}
	
	for (Profile profile : m_ProfileList){
		char filePath[512];
		strcpy(filePath, profile.profilePath);
		strcat(filePath, profile.profileName);
		m_ProfileBuildMap = profile.profileBuildMap;
		if (profile.profileSaveLocal == 1){
			m_SFTPCtrl->downloadSingleFile(filePath);
		}
		else{
			m_SFTPCtrl->readFileContent(filePath);
		}
	}
}

void NewCoreSFTPHandler::OnDownloadCompleted(string& data)
{
	ProcessDataset(data);
}

void NewCoreSFTPHandler::ProcessDataset(string data)
{
	if (!ValidateRow(data)){
		return;
	}

	string transactionCode = GetTransactionCode(data);
	string sn = GetSeqNum(data);
	if (sn == "") return;

	int seqNum = stoi(sn);
	if (!ValidateSeqNum(transactionCode, seqNum)){
		return;
	}

	m_SeqNumPerDataset[transactionCode] = seqNum;

	if (transactionCode == CODE_CHANGE_OF_CONTRACT){
		Process_ChangeOfContract(data);
	}
	else if (transactionCode == CODE_D_TO_MBR){
		Process_DToMBR(data);
	}
	else if (transactionCode == CODE_SBR_TO_MBR){
		Process_SBRToMBR(data);
	}
	else if (transactionCode == CODE_DELISTED_INSTRUMENT){
		Process_DelistedInstrument(data);
	}
	else if (transactionCode == CODE_TRADING_SCHEDULE){
		Process_TradingSchedule(data);
	}
	else if (transactionCode == CODE_HOLIDAY){
		Process_Holiday(data);
	}
	else if (transactionCode == CODE_MEMBER_TRADER){
		Process_MemberTrader(data);
	}
	else if (transactionCode == CODE_MARKET_MAKING_CONTRACT){
		Process_MarketMakingContract(data);
	}
	else if (transactionCode == CODE_ETF_INDEX_COMPOSITION){
		Process_ETFIndexComposition(data);
	}
	else if (transactionCode == CODE_COUPON_PAYMENT_DATE){
		Process_CouponPaymentDate(data);
	}
	else{
		// do nothing
	}
}

void NewCoreSFTPHandler::Process_ChangeOfContract(string data)
{
	//TODO
}

void NewCoreSFTPHandler::Process_DToMBR(string data)
{
	COP_ITEM cop = new MsgObject();

	// get symbot short code
	string symbolShortCode = getFieldData(data, CODE_D_TO_MBR, DTOMBR_SEQ_SYMBOL_SHORT_CODE);
	char* ric = strdup(symbolShortCode.c_str());
	cop->AddFid(FID_RICNAME, &CharArray(ric, 255, 0));

	// check ProfileBuildMap in file config whether build or do not build local mapping
	string symbol = getFieldData(data, CODE_D_TO_MBR, DTOMBR_SEQ_SYMBOL);
	if (symbol != "" && symbolShortCode != "" && m_ProfileBuildMap == 1){
		m_MapISINRicname[symbolShortCode] = symbol;
	}

	if (m_MapISINRicname.find(symbolShortCode) != m_MapISINRicname.end()){
		// get underlying symbol code
		string uSymbolCode = getFieldData(data, CODE_D_TO_MBR, DTOMBR_SEQ_UNDERLYING_SYMBOLCODE);
		char* symbolcode = strdup(uSymbolCode.c_str());
		cop->AddFid(FID_GEN_TXT43, &CharArray(symbolcode, 255, 0));
	}

	int pageNo = GetPageNumberByRic(symbolShortCode);

	if (pageNo != -1){
		cop->SetPageNo(pageNo);
		cop->GetMessage();
		m_pCopItemQueue->AddItem(cop);
		m_pDCache->PutDataIntoDCache(cop);
	}
}

void NewCoreSFTPHandler::Process_SBRToMBR(string data)
{
	COP_ITEM cop = new MsgObject();

	// get symbot short code
	string symbolShortCode = getFieldData(data, CODE_SBR_TO_MBR, SBRTOMBR_SEQ_SYMBOL_SHORT_CODE);
	char* ric = strdup(symbolShortCode.c_str());
	cop->AddFid(FID_RICNAME, &CharArray(ric, 255, 0));

	// check ProfileBuildMap in file config whether build or do not build local mapping
	string symbol = getFieldData(data, CODE_SBR_TO_MBR, SBRTOMBR_SEQ_SYMBOL);
	if (symbol != "" && symbolShortCode != "" && m_ProfileBuildMap == 1){
		m_MapISINRicname[symbolShortCode] = symbol;
	}

	string parValue = getFieldData(data, CODE_SBR_TO_MBR, SBRTOMBR_SEQ_PAR_VALUE);
	long lparValue = stol(parValue);
	cop->AddFid(FID_REV_UL21, &lparValue);

	int pageNo = GetPageNumberByRic(symbolShortCode);

	if (pageNo != -1){
		cop->SetPageNo(pageNo);
		cop->GetMessage();
		m_pCopItemQueue->AddItem(cop);
		m_pDCache->PutDataIntoDCache(cop);
	}
}

void NewCoreSFTPHandler::Process_DelistedInstrument(string data)
{
	// TODO
}

void NewCoreSFTPHandler::Process_TradingSchedule(string data)
{
	// TODO
}

void NewCoreSFTPHandler::Process_Holiday(string data)
{
	// TODO
}

void NewCoreSFTPHandler::Process_MemberTrader(string data)
{
	// TODO
}

void NewCoreSFTPHandler::Process_MarketMakingContract(string data)
{
	// TODO
}

void NewCoreSFTPHandler::Process_ETFIndexComposition(string data)
{
	// TODO
}

void NewCoreSFTPHandler::Process_CouponPaymentDate(string data)
{
	// TODO
}

bool NewCoreSFTPHandler::ValidateRow(string data)
{
	if (data == TAIL || GetSeqNum(data) == TAIL){
		return false;
	}

	// check size
	int dataSize = data.length();
	if (dataSize == 0){
		return false;
	}

	string transactionCode = GetTransactionCode(data);

	// TODO: define file size
	map<int, int> seqToLength;
	if (g_FileMapData.find(transactionCode) != g_FileMapData.end()){
		seqToLength = g_FileMapData[transactionCode];

		int length = 0;
		for (map<int, int>::iterator it = seqToLength.begin(); it != seqToLength.end(); ++it){
			length += it->second;
		}

		if (dataSize < length){
			CApplManClient::Log(LOG_ERROR, "Missing data: %s, %d/%d", transactionCode.c_str(), dataSize, length);
			return false;
		}
	}

	return true;
}

string NewCoreSFTPHandler::GetSeqNum(string data)
{
	string seqNum = "";
	if (data.length() >= (LENGTH_TRANSACTION_CODE + LENGTH_SEQUENCE_NUMBER)){
		seqNum = data.substr(LENGTH_TRANSACTION_CODE, LENGTH_SEQUENCE_NUMBER);
	}
	return seqNum;
}

string NewCoreSFTPHandler::GetTransactionCode(string data)
{
	string transactionCode = "";
	if (data.length() >= LENGTH_TRANSACTION_CODE){
		transactionCode = data.substr(0, LENGTH_TRANSACTION_CODE);
	}
	return transactionCode;
}

bool NewCoreSFTPHandler::ValidateSeqNum(string transCode, int seqNum)
{
	bool rc = true;
	if (m_SeqNumPerDataset.find(transCode) != m_SeqNumPerDataset.end()){
		int lastSeq = m_SeqNumPerDataset[transCode];

		if (seqNum <= lastSeq){
			CApplManClient::Trace(TRACE_LEVEL0, "[NewCoreSFTPDP] Duplicate Dataset SeqNum: %d - Transaction Code: %s", seqNum, transCode.c_str());
			rc = false;
		}
		else if (seqNum > lastSeq + 1){
			CApplManClient::Log(LOG_ERROR, "[NewCoreSFTPDP] Lost Dataset from SeqNum: %d to %d - Transaction Code : %s", lastSeq, seqNum, transCode.c_str());
		}
		else{
			// normal
		}
	}
	return rc;
}

long NewCoreSFTPHandler::GetPageNumberByRic(string ric)
{
	long pageNo = -1;
	if (m_LocalMapping.find(ric) != m_LocalMapping.end()){
		pageNo = m_LocalMapping[ric];
	}

	return pageNo;
}

string NewCoreSFTPHandler::getFieldData(string data, string transCode, int seq)
{
	string fieldData = "";
	map<int, int> seqToLength;
	if (g_FileMapData.find(transCode) != g_FileMapData.end()){
		seqToLength = g_FileMapData[transCode];

		int pos = 0;
		for (map<int, int>::iterator it = seqToLength.begin(); it != seqToLength.end(); ++it){
			if (it->first != seq){
				pos += it->second;
			}
			else{
				fieldData = data.substr(pos, it->second);
				break;
			}
		}
	}

	return fieldData;
}