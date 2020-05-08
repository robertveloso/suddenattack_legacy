# Microsoft Developer Studio Project File - Name="ButeEdit" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ButeEdit - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ButeEdit.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ButeEdit.mak" CFG="ButeEdit - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ButeEdit - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ButeEdit - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ButeEdit - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\built\release\tools\ButeEdit"
# PROP Intermediate_Dir "..\..\built\release\tools\ButeEdit"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\Libs\STLPORT-4.0\STLPORT" /I "..\..\..\libs\butemgr" /I "..\..\..\libs\cryptmgr" /I "..\..\..\libs\stdlith" /I "..\..\..\libs\lith" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386 /nodefaultlib:"mfc42d.lib" /nodefaultlib:"mfcs42d.lib" /nodefaultlib:"msvcrtd.lib"
# Begin Custom Build
TargetPath=\Work\SuddenAttack\Engine\built\release\tools\ButeEdit\ButeEdit.exe
InputPath=\Work\SuddenAttack\Engine\built\release\tools\ButeEdit\ButeEdit.exe
SOURCE="$(InputPath)"

"..\..\..\development\tools\ButeEdit.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\development\tools 
	copy $(TargetPath) ..\..\..\development\tools 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "ButeEdit - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\built\debug\tools\ButeEdit"
# PROP Intermediate_Dir "..\..\built\debug\tools\ButeEdit"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\Libs\STLPORT-4.0\STLPORT" /I "..\..\..\libs\butemgr" /I "..\..\..\libs\cryptmgr" /I "..\..\..\libs\stdlith" /I "..\..\..\libs\lith" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# Begin Custom Build
TargetPath=\Work\SuddenAttack\Engine\built\debug\tools\ButeEdit\ButeEdit.exe
InputPath=\Work\SuddenAttack\Engine\built\debug\tools\ButeEdit\ButeEdit.exe
SOURCE="$(InputPath)"

"..\..\..\development\tools\ButeEdit.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\development\tools 
	copy $(TargetPath) ..\..\..\development\tools 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "ButeEdit - Win32 Release"
# Name "ButeEdit - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Modify Dialogs Source"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\ModifyBoolDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ModifyByteDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ModifyDlgBase.cpp
# End Source File
# Begin Source File

SOURCE=.\ModifyDoubleDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ModifyDWordDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ModifyFloatDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ModifyIntegerDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ModifyPointDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ModifyRangeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ModifyRectangleDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ModifyStringDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ModifyVectorDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\NewTagDlg.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\BEStringFunc.cpp
# End Source File
# Begin Source File

SOURCE=.\ButeEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\ButeEdit.rc
# End Source File
# Begin Source File

SOURCE=.\ButeEditDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\ButeEditView.cpp
# End Source File
# Begin Source File

SOURCE=.\LeftView.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Modify Dialogs Headers"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\ModifyBoolDlg.h
# End Source File
# Begin Source File

SOURCE=.\ModifyByteDlg.h
# End Source File
# Begin Source File

SOURCE=.\ModifyDlgBase.h
# End Source File
# Begin Source File

SOURCE=.\ModifyDoubleDlg.h
# End Source File
# Begin Source File

SOURCE=.\ModifyDWordDlg.h
# End Source File
# Begin Source File

SOURCE=.\ModifyFloatDlg.h
# End Source File
# Begin Source File

SOURCE=.\ModifyIntegerDlg.h
# End Source File
# Begin Source File

SOURCE=.\ModifyPointDlg.h
# End Source File
# Begin Source File

SOURCE=.\ModifyRangeDlg.h
# End Source File
# Begin Source File

SOURCE=.\ModifyRectangleDlg.h
# End Source File
# Begin Source File

SOURCE=.\ModifyStringDlg.h
# End Source File
# Begin Source File

SOURCE=.\ModifyVectorDlg.h
# End Source File
# Begin Source File

SOURCE=.\NewTagDlg.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\BEStringFunc.h
# End Source File
# Begin Source File

SOURCE=.\ButeEdit.h
# End Source File
# Begin Source File

SOURCE=.\ButeEditDoc.h
# End Source File
# Begin Source File

SOURCE=.\ButeEditView.h
# End Source File
# Begin Source File

SOURCE=.\LeftView.h
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

SOURCE=.\res\ButeEdit.ico
# End Source File
# Begin Source File

SOURCE=.\res\ButeEdit.rc2
# End Source File
# Begin Source File

SOURCE=.\res\ButeEditDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Group "Lib_Debug"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Libs\built\Debug\CryptMgrMfcDll.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\Libs\built\Debug\Lib_StdLith.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\Libs\built\Debug\Lib_ZLib.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\Libs\built\Debug\ButeMgrMfcDll.lib
# End Source File
# End Group
# Begin Group "Lib_Release"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Libs\built\Release\Lib_ZLib\Lib_ZLib.lib

!IF  "$(CFG)" == "ButeEdit - Win32 Release"

!ELSEIF  "$(CFG)" == "ButeEdit - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Libs\built\Release\ButeMgrMFCDll\ButeMgrMfcDll.lib

!IF  "$(CFG)" == "ButeEdit - Win32 Release"

!ELSEIF  "$(CFG)" == "ButeEdit - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\libs\built\Release\CryptMgrMfcDll\CryptMgrMfcDll.lib

!IF  "$(CFG)" == "ButeEdit - Win32 Release"

!ELSEIF  "$(CFG)" == "ButeEdit - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Libs\built\Release\Lib_StdLith\Lib_StdLith.lib

!IF  "$(CFG)" == "ButeEdit - Win32 Release"

!ELSEIF  "$(CFG)" == "ButeEdit - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\ButeEdit.reg
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
# Section ButeEdit : {72ADFD6C-2C39-11D0-9903-00A0C91BC942}
# 	1:27:CG_IDR_POPUP_BUTE_EDIT_VIEW:102
# End Section
