# Microsoft Developer Studio Project File - Name="LTDMTest" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=LTDMTest - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LTDMTest.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LTDMTest.mak" CFG="LTDMTest - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LTDMTest - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "LTDMTest - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LTDMTest - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\tmp\release\ltdmtest"
# PROP Intermediate_Dir "..\..\tmp\release\ltdmtest"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\lib\inc" /I "..\..\..\..\Runtime\sdk\inc" /I "..\..\..\..\Source\STLPORT-4.0\STLPORT" /I "..\..\..\libs\lith" /I "..\..\sdk\inc" /I "..\..\libs\rezmgr" /I "..\..\..\libs\stdlith" /I "..\..\runtime\controlfilemgr" /D "NDEBUG" /D "LT_OK 0" /D "DRESULT unsigned long" /D "LTRESULT unsigned long" /D "LT15_COMPAT" /D "NO_PRAGMA_LIBS" /D "WIN32" /D "_WINDOWS" /D "NOLITHTECH" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 dxguid.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libcmt.lib" /out:"..\..\bin\release\LTDMTest.exe"

!ELSEIF  "$(CFG)" == "LTDMTest - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\tmp\debug\ltdmtest"
# PROP Intermediate_Dir "..\..\tmp\debug\ltdmtest"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /GX /ZI /Od /I "..\..\..\libs\lith" /I "..\..\sdk\inc" /I "..\..\libs\rezmgr" /I "..\..\..\libs\stdlith" /I "..\..\runtime\controlfilemgr" /I "..\..\..\..\Source\STLPORT-4.0\STLPORT" /D "_DEBUG" /D "LT15_COMPAT" /D "NO_PRAGMA_LIBS" /D "WIN32" /D "_WINDOWS" /D "NOLITHTECH" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 dxguid.lib nafxcwd.lib libcmtd.lib dxerr8.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcmtd.lib nafxcwd.lib" /out:"..\..\bin\debug\LTDMTest.exe" /pdbtype:sept /libpath:"..\..\..\libs\built\debug\lib_lith" /libpath:"..\..\built\debug\libs\lib_rezmgr" /verbose:lib
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "LTDMTest - Win32 Release"
# Name "LTDMTest - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\Runtime\controlfilemgr\controlfilemgr.cpp
# End Source File
# Begin Source File

SOURCE=.\ltdirectmusic_impl.cpp
# End Source File
# Begin Source File

SOURCE=.\ltdirectmusiccontrolfile.cpp
# End Source File
# Begin Source File

SOURCE=.\ltdirectmusicloader.cpp
# End Source File
# Begin Source File

SOURCE=.\LTDMTest.cpp
# End Source File
# Begin Source File

SOURCE=.\LTDMTest.rc
# End Source File
# Begin Source File

SOURCE=.\LTDMTestDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\Runtime\controlfilemgr\controlfilemgr.h
# End Source File
# Begin Source File

SOURCE=.\ltdirectmusic_impl.h
# End Source File
# Begin Source File

SOURCE=.\ltdirectmusiccontrolfile.h
# End Source File
# Begin Source File

SOURCE=.\ltdirectmusicloader.h
# End Source File
# Begin Source File

SOURCE=.\LTDMTest.h
# End Source File
# Begin Source File

SOURCE=.\LTDMTestDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\winsync.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\LTDMTest.ico
# End Source File
# Begin Source File

SOURCE=.\res\LTDMTest.rc2
# End Source File
# End Group
# Begin Group "SampleControlFiles"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\samplecontrolfile.txt
# End Source File
# End Group
# Begin Group "Lib_Debug"

# PROP Default_Filter "lib"
# Begin Source File

SOURCE=.\RezMgrd.lib
# End Source File
# Begin Source File

SOURCE=.\lithd.lib
# End Source File
# End Group
# Begin Group "Lib_Release"

# PROP Default_Filter "lib"
# Begin Source File

SOURCE=.\RezMgr.lib
# End Source File
# Begin Source File

SOURCE=.\lith.lib
# End Source File
# End Group
# End Target
# End Project
