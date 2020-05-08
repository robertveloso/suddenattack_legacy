# Microsoft Developer Studio Project File - Name="PropHelpGen" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=PropHelpGen - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "PropHelpGen.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PropHelpGen.mak" CFG="PropHelpGen - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PropHelpGen - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "PropHelpGen - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PropHelpGen - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Built\Release\tools\PropHelpGen"
# PROP Intermediate_Dir "..\..\Built\Release\tools\PropHelpGen"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\Libs\STLPORT-4.0\STLPORT" /I ".\\" /I "..\shared" /I "..\shared\engine" /I "..\..\sdk\inc" /I "..\..\..\libs\stdlith" /I "..\..\..\libs\butemgr" /I "..\..\..\libs\cryptmgr" /I "..\..\..\libs\lith" /D "NDEBUG" /D "_CONSOLE" /D "WIN32" /D "DIRECTEDITOR_BUILD" /D "_MBCS" /D "BDEFS_SIM" /D "STDLITH_MFC" /D "BDEFS_MFC" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 /nologo /subsystem:console /machine:I386
# SUBTRACT LINK32 /incremental:yes
# Begin Custom Build
TargetPath=\Working\Engine\Built\Release\tools\PropHelpGen\PropHelpGen.exe
InputPath=\Working\Engine\Built\Release\tools\PropHelpGen\PropHelpGen.exe
SOURCE="$(InputPath)"

"..\..\..\development\tools\PropHelpGen.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\development\tools 
	copy $(TargetPath) ..\..\..\development\tools 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "PropHelpGen - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\Built\debug\tools\PropHelpGen"
# PROP Intermediate_Dir "..\..\Built\debug\tools\PropHelpGen"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\Libs\STLPORT-4.0\STLPORT" /I ".\\" /I "..\shared" /I "..\shared\engine" /I "..\..\sdk\inc" /I "..\..\..\libs\stdlith" /I "..\..\..\libs\butemgr" /I "..\..\..\libs\cryptmgr" /I "..\..\..\libs\lith" /D "_DEBUG" /D "_WINDOWS" /D "STDLITH_MFC" /D "BDEFS_MFC" /D "WIN32" /D "DIRECTEDITOR_BUILD" /D "_MBCS" /D "_AFXDLL" /D "BDEFS_SIM" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"nafxcwd.lib" /nodefaultlib:"libcmtd.lib" /pdbtype:sept
# Begin Custom Build
TargetPath=\Working\Engine\Built\debug\tools\PropHelpGen\PropHelpGen.exe
InputPath=\Working\Engine\Built\debug\tools\PropHelpGen\PropHelpGen.exe
SOURCE="$(InputPath)"

"..\..\..\development\tools\PropHelpGen.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\development\tools 
	copy $(TargetPath) ..\..\..\development\tools 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "PropHelpGen - Win32 Release"
# Name "PropHelpGen - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\shared\engine\bdefs.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\shared\engine\bindmgr.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\shared\engine\classbind.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\PropHelpGen.cpp
# End Source File
# Begin Source File

SOURCE=.\PropHelpGen.rc
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\shared\engine\classbind.h
# End Source File
# Begin Source File

SOURCE=.\PropHelpGen.h
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
# End Group
# Begin Group "libs_debug"

# PROP Default_Filter "*.lib"
# Begin Source File

SOURCE=..\..\..\Libs\built\Debug\Lib_StdLith\Lib_StdLith.lib

!IF  "$(CFG)" == "PropHelpGen - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "PropHelpGen - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\libs\built\Debug\Lib_Lith\Lib_Lith.lib

!IF  "$(CFG)" == "PropHelpGen - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "PropHelpGen - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Libs\built\Debug\ButeMgrMFCDll\ButeMgrMfcDll.lib

!IF  "$(CFG)" == "PropHelpGen - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "PropHelpGen - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Libs\built\Debug\CryptMgrMfcDll\CryptMgrMfcDll.lib

!IF  "$(CFG)" == "PropHelpGen - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "PropHelpGen - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "libs_release"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Libs\built\Release\Lib_StdLith\Lib_StdLith.lib

!IF  "$(CFG)" == "PropHelpGen - Win32 Release"

!ELSEIF  "$(CFG)" == "PropHelpGen - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\libs\built\Release\Lib_Lith\Lib_Lith.lib

!IF  "$(CFG)" == "PropHelpGen - Win32 Release"

!ELSEIF  "$(CFG)" == "PropHelpGen - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Libs\built\Release\ButeMgrMFCDll\ButeMgrMfcDll.lib

!IF  "$(CFG)" == "PropHelpGen - Win32 Release"

!ELSEIF  "$(CFG)" == "PropHelpGen - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\libs\built\Release\CryptMgrMfcDll\CryptMgrMfcDll.lib

!IF  "$(CFG)" == "PropHelpGen - Win32 Release"

!ELSEIF  "$(CFG)" == "PropHelpGen - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
