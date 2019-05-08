// SendDummyDataAppl.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vector>
#include<iostream>
#include "SendDummyDef.h"
#include "Multicast.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
static const char* SECTION_DATAFILE = "DataFile";
static const char* SECTION_SETTINGS = "Settings";
static const char* INI_FILE = "./SendDummyCfg.ini";

// The one and only application object
BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
		// Handle the CTRL-C signal. 
	case CTRL_C_EVENT:
		printf("Ctrl-C event\n\n");
		Beep(750, 300);
		return TRUE;

		// CTRL-CLOSE: confirm that the user wants to exit. 
	case CTRL_CLOSE_EVENT:
		Beep(600, 200);
		printf("Ctrl-Close event\n\n");
		return TRUE;

		// Pass other signals to the next handler. 
	case CTRL_BREAK_EVENT:
		Beep(900, 200);
		printf("Ctrl-Break event\n\n");
		return TRUE;

	case CTRL_LOGOFF_EVENT:
		Beep(1000, 200);
		printf("Ctrl-Logoff event\n\n");
		return FALSE;

	case CTRL_SHUTDOWN_EVENT:
		Beep(750, 500);
		printf("Ctrl-Shutdown event\n\n");
		return FALSE;

	default:
		return FALSE;
	}
}

using namespace std;
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	char szMultiFileName[512];
	char szIP[16];
	char szGroup[16];
	int nPort;
	ZeroMemory(szMultiFileName, 512);

	// Read ini file
	GetPrivateProfileString(SECTION_DATAFILE, "FileName", "Default", szMultiFileName, sizeof(szMultiFileName), INI_FILE);
	GetPrivateProfileString(SECTION_SETTINGS, "SendIP", "127.0.0.1", szIP, sizeof(szIP), INI_FILE);
	GetPrivateProfileString(SECTION_SETTINGS, "SendGroup", "225.9.9.9", szGroup, sizeof(szGroup), INI_FILE);
	nPort = GetPrivateProfileInt(SECTION_SETTINGS, "SendPort", 0, INI_FILE);

	CMulticast m_multicast;
	m_multicast.Start(nPort, szIP);
	cout << "Port: " << nPort << " IP: " << szIP << endl;
	m_multicast.AddGroup(szGroup);
	cout << "Group: " << szGroup << endl;
	// Split data file's name
	std::vector<char*> v;
	char* chars_array = strtok(szMultiFileName, ",");
	while (chars_array)
	{
		v.push_back(chars_array);
		chars_array = strtok(NULL, ",");
	}
	if (SetConsoleCtrlHandler(CtrlHandler, TRUE))
	{
		int count = 1;
		while (true)
		{
			for (std::vector<char*>::iterator it = v.begin(); it != v.end(); ++it)
			{
				HANDLE h = CreateFile(*it, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				if (h != INVALID_HANDLE_VALUE) {
					unsigned long lFileSize = 0;
					lFileSize = GetFileSize(h, NULL);
					if (lFileSize > 0) {
						char *pbuffer = (char*)malloc(lFileSize);
						char *pSendMsg = (char*)malloc(lFileSize);
						ZeroMemory(pSendMsg, lFileSize);
						if (pbuffer)
						{
							unsigned long lRead = 0;
							ReadFile(h, pbuffer, lFileSize, &lRead, NULL);

							// Remove character CR LF inside the buffer
							const char* pTempSource = pbuffer;
							char* pTempDest = pSendMsg;
							long lMsgSize = 0;
							while (pTempSource != pbuffer + lRead)
							{
								if (*pTempSource != 0x0d && *pTempSource != 0x0a)
								{
									memcpy(pTempDest, pTempSource, sizeof(char));
									pTempDest++;
									lMsgSize++;
								}
								pTempSource++;
							}
							m_multicast.Send(szGroup, nPort, pSendMsg, lMsgSize);
							cout << "Send OK!" << count << endl;
							++count;
							CloseHandle(h);
							free(pbuffer);
						}
						else {
							CloseHandle(h);
						}
					}
					else {
						CloseHandle(h);
					}
				}
			}
		}
	}
	return nRetCode;
}

