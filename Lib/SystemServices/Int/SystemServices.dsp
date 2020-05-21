# Microsoft Developer Studio Project File - Name="SystemServices" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=SystemServices - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SystemServices.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SystemServices.mak" CFG="SystemServices - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SystemServices - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "SystemServices - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/NGN2/Lib/SystemServices/INT", RCNBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SystemServices - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "./Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "SystemServices - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /ZI /Od /I "./Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "SystemServices - Win32 Release"
# Name "SystemServices - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\ssCharacterSet.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ssCountedObjPtr.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ssCriticalSection.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ssEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ssMMTimer.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ssMultiWait.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ssMutex.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ssSharedMemory.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ssSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ssThread.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ssTimer.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ssUtf8string.cpp
# End Source File
# End Group
# Begin Group "Include Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Include\ssAll.h
# End Source File
# Begin Source File

SOURCE=.\Include\ssBitTable.h
# End Source File
# Begin Source File

SOURCE=.\Include\ssCharacterSet.h
# End Source File
# Begin Source File

SOURCE=.\Include\ssCommon.h
# End Source File
# Begin Source File

SOURCE=.\Include\ssCOPActiveBuffer.h
# End Source File
# Begin Source File

SOURCE=.\Include\ssCOPBuffer.h
# End Source File
# Begin Source File

SOURCE=.\Include\ssCOPMultiActiveBuffer.h
# End Source File
# Begin Source File

SOURCE=.\Include\ssCountedObjPtr.h
# End Source File
# Begin Source File

SOURCE=.\Include\ssCriticalSection.h
# End Source File
# Begin Source File

SOURCE=.\Include\ssDefines.h
# End Source File
# Begin Source File

SOURCE=.\Include\ssEvent.h
# End Source File
# Begin Source File

SOURCE=.\Include\ssMMTimer.h
# End Source File
# Begin Source File

SOURCE=.\Include\ssMultiWait.h
# End Source File
# Begin Source File

SOURCE=.\Include\ssMutex.h
# End Source File
# Begin Source File

SOURCE=.\Include\ssSharedMemory.h
# End Source File
# Begin Source File

SOURCE=.\Include\ssSocket.h
# End Source File
# Begin Source File

SOURCE=.\Include\ssThread.h
# End Source File
# Begin Source File

SOURCE=.\Include\ssTimer.h
# End Source File
# Begin Source File

SOURCE=.\Include\ssUtf8String.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Readme.txt
# End Source File
# End Target
# End Project
