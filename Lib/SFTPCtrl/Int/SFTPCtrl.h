//
// SFTPCtrl.h
// 
// Created by Ha Do on March 12, 2019
// Modified by Ha Do on March 18, 2019
// Copyright (c) 2018-18 N2N-AFE (Hong Kong) Limited. All rights reserved.
//

#pragma once
#include "ssThread.h"
#include "include/curl.h"
#include "ApplManClient.h"
#include "ssMultiWait.h"
#include <functional>

using namespace std;
class SFTPCtrl : public ssThread
{
public:
	typedef	std::function<void(void)> CallbackFunc;
	typedef	std::function<void(string&)> DownloadFileCallback;

	typedef struct _FileInfo {
		const char *filename;
		FILE *stream;
		string content;
	} FileInfo;

	SFTPCtrl();
	SFTPCtrl(char* serverHost, long serverPort, char* user, char* pwd, char* proxyUrl = "", long proxyPort = 0L, char* proxyUserPwd = "");
	~SFTPCtrl();
	void Main();

	void setLocalDest(char* localDes);

	void downloadSingleFile(char* filePath);
	void downloadAllFilesInFolder(char* folderPath);
	void readFileContent(char* filePath);

	void setOnDownloadCompleted(DownloadFileCallback onDownloadCompleted);

private:
	char*	mServerHost;
	long	mServerPort;
	char*	mUser;
	char*	mPassword;
	char*	mProxyUrl;
	long	mProxyPort;
	char*	mProxyUserPwd;
	char*	mLocalDest;

	DownloadFileCallback mOnDownloadCompleted;
	DownloadFileCallback mOnReadFileCompleted;

	static size_t fileWriter(char *buffer, size_t size, size_t nmemb, void *stream);
	static size_t stringWriter(char *buffer, size_t size, size_t nmemb, string *stream);

	bool isFileName(string name);
	bool performMultiHandle(CURLM* multi_handle, CURL* curl);
	wstring convertStrToWstr(string str);
	string handleFilePath(char* filePath);

	bool downloadAllFiles(const char* url, const char* des);
	bool downloadFile(const char* url, const char* localDest);

	bool initConnection(CURL **curl, CURLM **multiHandle);
	void urlCleanup(CURL *curl, CURLM *multiHandle);
};

