========================================================================
       DYNAMIC LINK LIBRARY : DCache
========================================================================


AppWizard has created this DCache DLL for you.  

This file contains a summary of what you will find in each of the files that
make up your DCache application.

DCache.dsp
    This file (the project file) contains information at the project level and
    is used to build a single project or subproject. Other users can share the
    project (.dsp) file, but they should export the makefiles locally.

DCache.cpp
    This is the main DLL source file.

DCache.h
    This file contains your DLL exports.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named DCache.pch and a precompiled types file named StdAfx.obj.


/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" to indicate parts of the source code you
should add to or customize.


/////////////////////////////////////////////////////////////////////////////


16 Nov, 2001
- Support Message Type QuoteList, MTCTicks, MTCV and Yheadline

- Add a new function to support retrieve multiple fids in one page
		BOOL GetAField(long lPageNo, COP_ITEM& copOutput, CDWordArray* cdwaFIDList, long& lIndex);	

- Add a new function to support retrieve Drop Page List 
		BOOL GetDropPageList(CDWordArray& cdwaPageList);		
		
- Add a new function to support retrieve Drop Fid List
	BOOL GetDropFidList(long lPageNo, CDWordArray& cdwaPageList);		

- Add a new function to support Log Data to File
	BOOL LogDataToFile(CString strFileName);


06 March,2002
- Support to synchronize two DCache data by using TCP socket 
	- It is enabled or disabled by using reading the value from registry (HKEY_LOCAL_MACHINE\SOFTWARE\AFE\DCache\EnableTCPServer)
	- Open the server port by reading the value from registry (HKEY_LOCAL_MACHINE\SOFTWARE\AFE\DCache\ServerPort)
	- After completed synchronization, the server side will disconnect the client

- Add a new function to support to retrieve Data Type OBkrQue
	BOOL GetAField(long lPageNo, OBkrQue& obqOutput, FID_DEFINITION enumFieldID, long lIndex);

- Add a new function to support to retrieve data by TCP 
	void RetrieveDataByTCP(CString host, long port);

- Support the Data Type OBkrQue

7 May 2002
- Support the Data Type KMultiple
- Support the Data Type 10KMultiple
- Add Checking for QuoteList

28 June 2002
- Modify Data Type SprTbl to support partial field update

16 July 2002
- Fix the bug that do not write to disk when ONE fid in the fid array 
- Add new interface to start the socket server
- Add new interface to stop the socket server
- Add new interface to get the socket server status

1 August 2002
- Fix the bugs to start the socket server
- Force to write to disk when destructing DCache 

9 August 2002
- Remove "Force to write to disk when destructing DCache"
- Add new interface "void ForceWriteToDisk()" to write to disk immediately

15 August 2002
- Fix the bugs about function ForceWriteToDisk()
- Fix the bugs about data type TRAN to support comparing the ripple data with the original data

20 August 2002
- Change the alogrithm to handle the callback (using Map to seaching)

16 September 2002
- Support the Data Type BQueVOSN
- Support "Day To Live" 

10 October 2002
- Fix the bug for "Day To Live"

29 November 2002
- Ammend the function "GetMultiField" that can be used to retrieve the QuoteList data

16 December 2002
- Fix the bug of updating Quote List when TL decreased 

7 January 2003
- Fix the bug of updating Quote List when TL decreased 

8 February 2003
- Add new interface to check the page exist or not
- Add new interface to check the fid of page exist or not

9 April 2003
- Add new interface to check the fid has been updated or not 

22 April 2002
- Support the Data Type MTCPTS

6 Aug 2003
- Add FID_QL_06(1466) format to DcacheTools

15 Sep 2003
- Release the critical section if any crashing happened. To prevent deadlock
- Fix the bug when compare CharArray, MCharArray, Link, WLink, MtcPoints, MtcVal, MTCV, SprTbl, MTCTicks, MTCPTS and Healine 

22 Mar 2011
- Multiple critical section to avoid contention
