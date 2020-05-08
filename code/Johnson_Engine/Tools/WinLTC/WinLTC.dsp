# Microsoft Developer Studio Project File - Name="WinLTC" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=WinLTC - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WinLTC.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WinLTC.mak" CFG="WinLTC - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WinLTC - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "WinLTC - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WinLTC - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\built\release\tools\WinLTC"
# PROP Intermediate_Dir "..\..\built\release\tools\WinLTC"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\..\sdk\inc" /I "..\..\libs\ltamgr" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
# Begin Custom Build
TargetPath=\Proj\TO2\Source\Jupiter\built\release\tools\WinLTC\WinLTC.exe
InputPath=\Proj\TO2\Source\Jupiter\built\release\tools\WinLTC\WinLTC.exe
SOURCE="$(InputPath)"

"..\..\..\development\tools\WinLTC.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\development\tools
	copy $(TargetPath) ..\..\..\development\tools

# End Custom Build

!ELSEIF  "$(CFG)" == "WinLTC - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\built\debug\tools\WinLTC"
# PROP Intermediate_Dir "..\..\built\debug\tools\WinLTC"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\..\sdk\inc" /I "..\..\libs\ltamgr" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcmtd.lib" /pdbtype:sept /libpath:"..\..\lib\debug\\"
# Begin Custom Build
TargetPath=\Proj\TO2\Source\Jupiter\built\debug\tools\WinLTC\WinLTC.exe
InputPath=\Proj\TO2\Source\Jupiter\built\debug\tools\WinLTC\WinLTC.exe
SOURCE="$(InputPath)"

"..\..\..\development\tools\WinLTC.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\development\tools
	copy $(TargetPath) ..\..\..\development\tools

# End Custom Build

!ENDIF 

# Begin Target

# Name "WinLTC - Win32 Release"
# Name "WinLTC - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\WinLTC.cpp
# End Source File
# Begin Source File

SOURCE=.\WinLTC.rc
# End Source File
# Begin Source File

SOURCE=.\WinLTCDlg.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\WinLTC.h
# End Source File
# Begin Source File

SOURCE=.\WinLTCDlg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\WinLTC.ico
# End Source File
# Begin Source File

SOURCE=.\res\WinLTC.rc2
# End Source File
# End Group
# Begin Group "libs_release"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\built\Release\libs\LTAMgr\ltamgr.lib

!IF  "$(CFG)" == "WinLTC - Win32 Release"

!ELSEIF  "$(CFG)" == "WinLTC - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "libs_debug"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\built\Debug\libs\LTAMgr\ltamgr.lib

!IF  "$(CFG)" == "WinLTC - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WinLTC - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
