# Microsoft Developer Studio Project File - Name="QuoteListTools" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=QuoteListTools - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "QuoteListTools.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "QuoteListTools.mak" CFG="QuoteListTools - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "QuoteListTools - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "QuoteListTools - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/NGN2/Lib/DCache/int/QuoteListTools", XVBFAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "QuoteListTools - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "../include" /I "..\..\..\SystemServices\INT\Include" /I "..\..\..\MsgObject\INT\Include" /I "..\..\..\DataCtrl\INT\Include" /I "..\..\..\Dcache\INT\Include" /I "..\..\..\DataManager\INT\Include" /I "..\..\..\TCPCtrl\INT\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 dcache.lib SystemServices.lib msgobj.lib DataCtrl.lib DataManager.lib TCPCtrl.lib /nologo /subsystem:windows /map /debug /machine:I386 /libpath:"../RELEASE" /libpath:"../../..\MsgObject\int\RELEASE" /libpath:"../../../SystemServices/int/RELEASE\\" /libpath:"../../..\DataCtrl\int\RELEASE" /libpath:"../../..\DataManager\int\release" /libpath:"../../..\TCPCtrl\int\RELEASE"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy ..\..\..\DataCtrl\INT\release\datactrl.dll release	copy ..\..\..\Dcache\INT\release\dcache.dll release	copy ..\..\..\TcpCtrl\INT\release\TcpCtrl.dll release
# End Special Build Tool

!ELSEIF  "$(CFG)" == "QuoteListTools - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../include" /I "../include" /I "..\..\..\SystemServices\INT\Include" /I "..\..\..\MsgObject\INT\Include" /I "..\..\..\DataCtrl\INT\Include" /I "..\..\..\Dcache\INT\Include" /I "..\..\..\DataManager\INT\Include" /I "..\..\..\TCPCtrl\INT\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 dcache.lib SystemServices.lib msgobj.lib DataCtrl.lib DataManager.lib TCPCtrl.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"../debug" /libpath:"../../..\MsgObject\int\debug" /libpath:"../../../SystemServices/int/debug\\" /libpath:"..\..\..\DataCtrl\INT\debug" /libpath:"..\..\..\Dcache\INT\debug" /libpath:"../../..\datamanager\int\debug" /libpath:"..\..\..\TCPCtrl\INT\debug"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy ..\..\..\DataCtrl\INT\debug\datactrl.dll debug	copy ..\..\..\Dcache\INT\debug\dcache.dll debug	copy ..\..\..\TcpCtrl\INT\debug\TcpCtrl.dll debug
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "QuoteListTools - Win32 Release"
# Name "QuoteListTools - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ABTPanelEx.cpp
# End Source File
# Begin Source File

SOURCE=.\DTFidIterator.cpp
# End Source File
# Begin Source File

SOURCE=.\JFile.cpp
# End Source File
# Begin Source File

SOURCE=.\JRegistry.cpp
# End Source File
# Begin Source File

SOURCE=.\LogFilterDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LogSchedule.cpp
# End Source File
# Begin Source File

SOURCE=.\MsgQueueManager.cpp
# End Source File
# Begin Source File

SOURCE=.\QTFidIterator.cpp
# End Source File
# Begin Source File

SOURCE=.\QuoteListTools.cpp
# End Source File
# Begin Source File

SOURCE=.\QuoteListTools.rc
# End Source File
# Begin Source File

SOURCE=.\QuoteListToolsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SelTextDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SendMessageDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\SocketServerDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StatisticDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StatisticPage.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\STFidIterator.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ABTPanelEx.h
# End Source File
# Begin Source File

SOURCE=.\DTFidIterator.h
# End Source File
# Begin Source File

SOURCE=.\JFile.h
# End Source File
# Begin Source File

SOURCE=.\JRegistry.h
# End Source File
# Begin Source File

SOURCE=.\LogFilterDlg.h
# End Source File
# Begin Source File

SOURCE=.\LogSchedule.h
# End Source File
# Begin Source File

SOURCE=.\MsgQueueManager.h
# End Source File
# Begin Source File

SOURCE=.\QTFidIterator.h
# End Source File
# Begin Source File

SOURCE=.\queue.h
# End Source File
# Begin Source File

SOURCE=.\QueueMgr.h
# End Source File
# Begin Source File

SOURCE=.\QuoteListTools.h
# End Source File
# Begin Source File

SOURCE=.\QuoteListToolsDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SelTextDlg.h
# End Source File
# Begin Source File

SOURCE=.\SendMessageDialog.h
# End Source File
# Begin Source File

SOURCE=.\SocketServerDlg.h
# End Source File
# Begin Source File

SOURCE=.\StatisticDlg.h
# End Source File
# Begin Source File

SOURCE=.\StatisticPage.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\STFidIterator.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\greenlit.ico
# End Source File
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\QuoteListTools.ico
# End Source File
# Begin Source File

SOURCE=.\res\QuoteListTools.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\res\red.avi
# End Source File
# End Target
# End Project
