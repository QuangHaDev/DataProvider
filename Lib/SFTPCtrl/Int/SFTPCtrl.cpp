//
// SFTPCtrl.cpp
// 
// Created by Ha Do on March 12, 2019
// Modified by Ha Do on March 18, 2019
// Copyright (c) 2018-18 N2N-AFE (Hong Kong) Limited. All rights reserved.
//

#include "SFTPCtrl.h"
#include <sstream>
#include <algorithm>

SFTPCtrl::SFTPCtrl()
{
	mServerHost = "";
	mServerPort = 0L;
	mUser = "";
	mPassword = "";
	mProxyUrl = "";
	mProxyPort = 0L;
	mProxyUserPwd = "";
	mLocalDest = "";
}

SFTPCtrl::SFTPCtrl(char* serverHost, long serverPort, char* user, char* pwd, char* proxyUrl, long proxyPort, char* proxyUserPwd)
{
	mServerHost = serverHost;
	mServerPort = serverPort;
	mUser = user;
	mPassword = pwd;
	mProxyUrl = proxyUrl;
	mProxyPort = proxyPort;
	mProxyUserPwd = proxyUserPwd;
}

SFTPCtrl::~SFTPCtrl()
{

}

void SFTPCtrl::Main()
{
	CApplManClient::Log(LOG_NORMAL, "[SFTPCtrl] Thread Started [ID:0x%x]", ssThread::GetID());

	enum {
		AliveEvent = 200,
		ExitEvent = 300,
	};
	ssMultiWait multiWait;

	multiWait.Add(ssThread::m_CheckAliveEvent, AliveEvent);
	multiWait.Add(ssThread::m_ExitEvent, ExitEvent);
	while (!IsExit()) {
		switch (multiWait.Wait(1)) {
		case AliveEvent:
			AckAlive();
			break;
		case ExitEvent:
			CApplManClient::Log(LOG_NORMAL, "[SFTPCtrl] Exiting");
			break;
		default:
			break;
		}
	}
}

void SFTPCtrl::setLocalDest(char* localDes)
{
	mLocalDest = localDes;
}

bool SFTPCtrl::initConnection(CURL **curl, CURLM **multiHandle)
{
	bool rc = true;

	curl_global_init(CURL_GLOBAL_DEFAULT);

	*curl = curl_easy_init();
	if (*curl == NULL){
		CApplManClient::Log(LOG_ERROR, "Init connection failed: curl_easy_init()");
		rc = false;
	}

	*multiHandle = curl_multi_init();
	if (*multiHandle == NULL){
		CApplManClient::Log(LOG_ERROR, "Init connection failed: curl_multi_init()");
		rc = false;
	}

	if (*curl){
		curl_easy_setopt(*curl, CURLOPT_PORT, mServerPort);
		curl_easy_setopt(*curl, CURLOPT_USERNAME, mUser);
		curl_easy_setopt(*curl, CURLOPT_PASSWORD, mPassword);

		if (mProxyUrl != ""){
			curl_easy_setopt(*curl, CURLOPT_PROXY, mProxyUrl);
			curl_easy_setopt(*curl, CURLOPT_PROXYPORT, mProxyPort);
		}

		if (mProxyUserPwd != ""){
			curl_easy_setopt(*curl, CURLOPT_PROXYAUTH, CURLAUTH_ANY);
			curl_easy_setopt(*curl, CURLOPT_PROXYUSERPWD, mProxyUserPwd);
		}
	}

	return rc;
}

void SFTPCtrl::downloadAllFilesInFolder(char* folderPath)
{
	string url = handleFilePath(folderPath);
	bool rc = downloadAllFiles(url.c_str(), mLocalDest);
	if (rc){
		// TODO: callback when download all file
		//this->mOnDownloadCompleted();
	}
}

void SFTPCtrl::readFileContent(char* filePath)
{
	string content = "";

	CURL *curl = NULL;
	CURLM *multiHandle = NULL;
	bool isInitOK = initConnection(&curl, &multiHandle);

	string url = handleFilePath(filePath);
	string baseFileName = url.substr(url.find_last_of("/") + 1);
	if (!isFileName(baseFileName)){
		CApplManClient::Log(LOG_NORMAL, "Url is not a file path: %s", filePath);
	}
	else {
		if (curl) {
			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, stringWriter);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &content);
		}

		CApplManClient::Log(LOG_NORMAL, "Read file start: %s", url.c_str());

		if (isInitOK && performMultiHandle(multiHandle, curl) && content != ""){
			CApplManClient::Log(LOG_NORMAL, "Read file completed!");
			this->mOnDownloadCompleted(content);
		}
		else {
			CApplManClient::Log(LOG_ERROR, "Read file failed!");
		}

		urlCleanup(curl, multiHandle);
	}
}

void SFTPCtrl::downloadSingleFile(char* filePath)
{
	string url = handleFilePath(filePath);
	string baseFileName = url.substr(url.find_last_of("/") + 1);
	if (!isFileName(baseFileName)){
		CApplManClient::Log(LOG_NORMAL, "Url is not a file path: %s", url.c_str());
	}
	else{
		downloadFile(url.c_str(), mLocalDest);
	}
}

void SFTPCtrl::setOnDownloadCompleted(DownloadFileCallback onDownloadCompleted)
{
	mOnDownloadCompleted = onDownloadCompleted;
}

bool SFTPCtrl::isFileName(string name)
{
	bool rc = false;
	if (name.find(".") != string::npos){
		string ext = name.substr(name.find_last_of(".") + 1);
		if (strcmp(ext.c_str(), "txt") == 0){
			rc = true;
		}
		else if (strcmp(ext.c_str(), "cfg") == 0){
			rc = true;
		}
		else if (strcmp(ext.c_str(), "ini") == 0){
			rc = true;
		}
		else if (strcmp(ext.c_str(), "doc") == 0){
			rc = true;
		}
		else if (strcmp(ext.c_str(), "docx") == 0){
			rc = true;
		}
		else if (strcmp(ext.c_str(), "pdf") == 0){
			rc = true;
		}
		else if (strcmp(ext.c_str(), "rtf") == 0){
			rc = true;
		}
		else if (strcmp(ext.c_str(), "wpd") == 0){
			rc = true;
		}
		else{

		}
	}

	return rc;
}

size_t SFTPCtrl::fileWriter(char *buffer, size_t size, size_t nmemb, void *stream)
{
	//printf("Writing data.\n");
	FileInfo *out = (FileInfo *)stream;
	out->content.append(buffer);
	if (!out->stream) {
		/* open file for writing */
		out->stream = fopen(out->filename, "wb");
		if (!out->stream){
			CApplManClient::Log(LOG_NORMAL, "Write function: cannot open file to write!");
			return -1; /* failure, can't open file to write */
		}
	}
	return fwrite(buffer, size, nmemb, out->stream);
}

size_t SFTPCtrl::stringWriter(char *buffer, size_t size, size_t nmemb, string *stream)
{
	if (stream == nullptr) return 0;

	stream->append(buffer);

	return size*nmemb;
}

bool SFTPCtrl::performMultiHandle(CURLM* multi_handle, CURL* curl)
{
	bool rc = false;
	int still_running = 0;
	curl_multi_add_handle(multi_handle, curl);
	CURLMcode res = curl_multi_perform(multi_handle, &still_running);

	int repeats = 0;
	while (still_running) {
		CURLMcode mc; /* curl_multi_wait() return code */
		int numfds;

		/* wait for activity, timeout or "nothing" */
		mc = curl_multi_wait(multi_handle, NULL, 0, 1000, &numfds);

		if (mc != CURLM_OK) {
			CApplManClient::Log(LOG_NORMAL, "curl_multi_wait() failed, code %d", mc);
			break;
		}

		/* 'numfds' being zero means either a timeout or no file descriptors to
		wait for. Try timeout on first occurrence, then assume no file
		descriptors and no file descriptors to wait for means wait for 100
		milliseconds. */

		if (!numfds) {
			repeats++; /* count number of repeated zero numfds */
			if (repeats > 1) {
				Sleep(100); /* sleep 100 milliseconds */
			}
		}
		else{
			repeats = 0;
		}

		res = curl_multi_perform(multi_handle, &still_running);
	}

	if (res != CURLM_OK){
		rc = false;
		CApplManClient::Log(LOG_ERROR, "Perform multi_handle failed: code %d", res);
	}

	CURLMsg* msg; /* for picking up messages with the transfer status */
	int msgs_left; /* how many messages are left */

	while ((msg = curl_multi_info_read(multi_handle, &msgs_left))) {
		if (msg->msg == CURLMSG_DONE) {
			rc = true;
		}
	}

	return rc;
}

wstring SFTPCtrl::convertStrToWstr(string str)
{
	size_t strl = strlen(str.c_str()) + 1;
	wstring wc(strl, L'#');
	mbstowcs(&wc[0], str.c_str(), strl);

	return wc;
}

string SFTPCtrl::handleFilePath(char* filePath)
{
	string fPath = filePath;
	replace(fPath.begin(), fPath.end(), '\\', '/');
	if (fPath[0] != '/' && mServerHost[strlen(mServerHost) - 1] != '/'){
		fPath = '/' + fPath;
	}
	string path = mServerHost + fPath;

	return path;
}

bool SFTPCtrl::downloadAllFiles(const char* url, const char* des)
{
	bool rc = true;

	string realPath = url;		// path pass to curl
	string path = realPath;		// path to get file/folder name - remove "/" if path end with it
	int length = strlen(url);
	bool endWithCross = (realPath[length - 1] == '/');
	if (endWithCross){
		path = realPath.substr(0, length - 1);
	}

	string baseFileName = path.substr(path.find_last_of("/") + 1);
	bool isDowndloadFile = isFileName(baseFileName);
	if (!isDowndloadFile && !endWithCross){
		realPath += "/";
	}

	CURL *curl = NULL;
	CURLM *multiHandle = NULL;
	rc = initConnection(&curl, &multiHandle);

	//printf("Start perform:\n url = %s\n des = %s\n -----------\n", url, des);

	if (isDowndloadFile){
		// download file
		rc = downloadFile(realPath.c_str(), des);
	}
	else {
		// list folder untill found file
		string content = "";
		if (curl){
			curl_easy_setopt(curl, CURLOPT_URL, realPath.c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, stringWriter);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &content);
			curl_easy_setopt(curl, CURLOPT_DIRLISTONLY, 1L);
		}

		bool isPerformOk = performMultiHandle(multiHandle, curl);
		urlCleanup(curl, multiHandle);

		if (isPerformOk){
			char localPath[255];
			strcpy(localPath, des);
			strcat(localPath, baseFileName.c_str());
			strcat(localPath, "/");

			wstring dirPath = convertStrToWstr(localPath);
			if (CreateDirectory((LPCSTR)dirPath.c_str(), NULL)){
				//printf("Created directory: %s\n", localPath);
			}

			// parse content to get list file/folder
			istringstream stream(content);
			string item;
			while (std::getline(stream, item)) {
				string itemUrl = realPath + item;
				if (itemUrl[itemUrl.length() - 1] == '\r'){
					itemUrl = itemUrl.substr(0, itemUrl.length() - 1);
				}
				rc = downloadAllFiles(itemUrl.c_str(), localPath);	// recursive
			}
		}
	}

	return rc;
}

bool SFTPCtrl::downloadFile(const char* url, const char* localDest)
{
	bool rc = true;

	string path = url;
	string fileName = path.substr(path.find_last_of("/") + 1);

	char localFilePath[255];
	strcpy(localFilePath, localDest);
	strcat(localFilePath, fileName.c_str());

	FileInfo ftpfile;
	ftpfile.filename = localFilePath;
	ftpfile.stream = NULL;

	CURL *curl = NULL;
	CURLM *multiHandle = NULL;
	rc = initConnection(&curl, &multiHandle);

	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fileWriter);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ftpfile);
	}

	CApplManClient::Log(LOG_NORMAL, "Download start: %s", url);

	if (performMultiHandle(multiHandle, curl) && ftpfile.content != ""){
		CApplManClient::Log(LOG_NORMAL, "Download completed!");
		this->mOnDownloadCompleted(ftpfile.content);
	}
	else {
		CApplManClient::Log(LOG_NORMAL, "Download failed!");
		rc = false;
	}

	urlCleanup(curl, multiHandle);

	if (ftpfile.stream)
		fclose(ftpfile.stream);

	return rc;
}

void SFTPCtrl::urlCleanup(CURL *curl, CURLM *multiHandle)
{
	curl_multi_remove_handle(multiHandle, curl);
	curl_easy_cleanup(curl);
	curl_multi_cleanup(multiHandle);
	curl_global_cleanup();
	multiHandle = NULL;
	curl = NULL;
}