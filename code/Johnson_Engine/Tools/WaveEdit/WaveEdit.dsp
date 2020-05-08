# Microsoft Developer Studio Project File - Name="WaveEdit" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=WaveEdit - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WaveEdit.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WaveEdit.mak" CFG="WaveEdit - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WaveEdit - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "WaveEdit - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WaveEdit - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\built\Release\tools\WaveEdit"
# PROP Intermediate_Dir "..\..\built\Release\tools\WaveEdit"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\shared\engine" /I "..\..\sdk\inc" /I "..\..\sdk\inc\compat" /I "..\shared\world" /I "..\..\..\libs\stdlith" /D "BDEFS_WINDOWS_H" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "LT15_COMPAT" /D "NO_PRAGMA_LIBS" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libcmt.lib"
# Begin Custom Build
TargetPath=\Working\Engine\built\Release\tools\WaveEdit\WaveEdit.exe
InputPath=\Working\Engine\built\Release\tools\WaveEdit\WaveEdit.exe
SOURCE="$(InputPath)"

"..\..\..\development\tools\WaveEdit.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\development\tools 
	copy $(TargetPath) ..\..\..\development\tools 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "WaveEdit - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\built\Debug\tools\WaveEdit"
# PROP Intermediate_Dir "..\..\built\Debug\tools\WaveEdit"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\shared\engine" /I "..\..\sdk\inc" /I "..\..\sdk\inc\compat" /I "..\shared\world" /I "..\..\..\libs\stdlith" /D "BDEFS_WINDOWS_H" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "LT15_COMPAT" /D "NO_PRAGMA_LIBS" /FR /FD /GZ /c
# SUBTRACT CPP /X /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcmtd.lib" /pdbtype:sept
# Begin Custom Build
TargetPath=\Working\Engine\built\Debug\tools\WaveEdit\WaveEdit.exe
InputPath=\Working\Engine\built\Debug\tools\WaveEdit\WaveEdit.exe
SOURCE="$(InputPath)"

"..\..\..\development\tools\WaveEdit.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\development\tools 
	copy $(TargetPath) ..\..\..\development\tools 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "WaveEdit - Win32 Release"
# Name "WaveEdit - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\shared\engine\bdefs.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\engine\conparse.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\engine\genltstream.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\shared\engine\streamsim.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\engine\wave.cpp
# End Source File
# Begin Source File

SOURCE=.\WaveEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\WaveEdit.rc
# End Source File
# Begin Source File

SOURCE=.\WaveEditDlg.cpp
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

SOURCE=..\..\..\common\src\shared\engine\wave.h
# End Source File
# Begin Source File

SOURCE=.\WaveEdit.h
# End Source File
# Begin Source File

SOURCE=.\WaveEditDlg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\WaveEdit.ico
# End Source File
# Begin Source File

SOURCE=.\res\WaveEdit.rc2
# End Source File
# End Group
# Begin Group "libs_debug"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Libs\built\Debug\Lib_StdLith\Lib_StdLith.lib

!IF  "$(CFG)" == "WaveEdit - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WaveEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\built\Debug\Runtime\Lib_ILTSound\Lib_ILTSound.lib

!IF  "$(CFG)" == "WaveEdit - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WaveEdit - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "libs_release"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\built\Release\Runtime\Lib_ILTSound\Lib_ILTSound.lib

!IF  "$(CFG)" == "WaveEdit - Win32 Release"

!ELSEIF  "$(CFG)" == "WaveEdit - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Libs\built\Release\Lib_StdLith\Lib_StdLith.lib

!IF  "$(CFG)" == "WaveEdit - Win32 Release"

!ELSEIF  "$(CFG)" == "WaveEdit - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
