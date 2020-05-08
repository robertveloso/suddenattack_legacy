# Microsoft Developer Studio Project File - Name="ltaview" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ltaview - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ltaview.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ltaview.mak" CFG="ltaview - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ltaview - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ltaview - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ltaview - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\built\Release\tools\ltaview"
# PROP Intermediate_Dir "..\..\built\Release\tools\ltaview"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\libs\STLPORT-4.0\STLPORT" /I ".\ltwintree" /I ".\ltatree" /I "..\..\sdk\inc" /I "..\..\libs\ltamgr" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
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
TargetPath=\Proj\TO2\Source\Jupiter\built\Release\tools\ltaview\ltaview.exe
InputPath=\Proj\TO2\Source\Jupiter\built\Release\tools\ltaview\ltaview.exe
SOURCE="$(InputPath)"

"..\..\..\development\tools\ltaview.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\development\tools
	copy $(TargetPath) ..\..\..\development\tools

# End Custom Build

!ELSEIF  "$(CFG)" == "ltaview - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\built\debug\tools\ltaview"
# PROP Intermediate_Dir "..\..\built\debug\tools\ltaview"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\libs\STLPORT-4.0\STLPORT" /I ".\ltwintree" /I ".\ltatree" /I "..\..\sdk\inc" /I "..\..\libs\ltamgr" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# Begin Custom Build
TargetPath=\Proj\TO2\Source\Jupiter\built\debug\tools\ltaview\ltaview.exe
InputPath=\Proj\TO2\Source\Jupiter\built\debug\tools\ltaview\ltaview.exe
SOURCE="$(InputPath)"

"..\..\..\development\tools\ltaview.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\development\tools
	copy $(TargetPath) ..\..\..\development\tools

# End Custom Build

!ENDIF 

# Begin Target

# Name "ltaview - Win32 Release"
# Name "ltaview - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\LTATree\LTATreeMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\ltaview.cpp
# End Source File
# Begin Source File

SOURCE=.\ltaview.rc
# End Source File
# Begin Source File

SOURCE=.\LTWinTree\ltwintreeitem.cpp
# End Source File
# Begin Source File

SOURCE=.\LTWinTree\ltwintreeitemiter.cpp
# End Source File
# Begin Source File

SOURCE=.\LTWinTree\ltwintreemgr.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\LTATree\LTAIcon.h
# End Source File
# Begin Source File

SOURCE=.\LTATree\LTATreeMgr.h
# End Source File
# Begin Source File

SOURCE=.\ltaview.h
# End Source File
# Begin Source File

SOURCE=.\LTWinTree\ltwintreeicon.h
# End Source File
# Begin Source File

SOURCE=.\LTWinTree\ltwintreeitem.h
# End Source File
# Begin Source File

SOURCE=.\LTWinTree\ltwintreeitemiter.h
# End Source File
# Begin Source File

SOURCE=.\LTWinTree\ltwintreekey.h
# End Source File
# Begin Source File

SOURCE=.\LTWinTree\ltwintreemgr.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\flag.ico
# End Source File
# Begin Source File

SOURCE=.\res\header.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00002.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00003.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00004.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00005.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00006.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00007.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00008.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00009.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00010.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00011.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00012.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00013.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00014.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00015.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00016.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00017.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00018.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00019.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00020.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00021.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00022.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon2.ico
# End Source File
# Begin Source File

SOURCE=.\res\Launcher.ico
# End Source File
# Begin Source File

SOURCE=.\res\ltaview.ico
# End Source File
# Begin Source File

SOURCE=.\res\ltaview.rc2
# End Source File
# Begin Source File

SOURCE=.\res\polygon.ico
# End Source File
# Begin Source File

SOURCE=.\res\shades.ico
# End Source File
# Begin Source File

SOURCE=.\res\world.ico
# End Source File
# End Group
# Begin Group "libs_debug"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\built\Debug\libs\LTAMgr\ltamgr.lib

!IF  "$(CFG)" == "ltaview - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ltaview - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "libs_release"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\built\Release\libs\LTAMgr\ltamgr.lib

!IF  "$(CFG)" == "ltaview - Win32 Release"

!ELSEIF  "$(CFG)" == "ltaview - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
