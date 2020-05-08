# Microsoft Developer Studio Project File - Name="FxED" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=FxED - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "FxED.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FxED.mak" CFG="FxED - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FxED - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "FxED - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FxED - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\built\release\tools\fxed"
# PROP Intermediate_Dir "..\..\built\release\tools\fxed"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\..\sdk\inc" /I "..\..\sdk\inc\compat" /I "clientfx\shared\src" /I "..\..\libs\rezmgr" /I "..\..\..\libs\lith" /I "..\..\..\libs\butemgr" /I "..\..\..\libs\cryptmgr" /I "..\..\..\libs\stdlith" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "USE_PRAGMA_LIB" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386 /nodefaultlib:"mfc42d.lib" /nodefaultlib:"mfcs42d.lib" /nodefaultlib:"msvcrtd.lib"
# Begin Custom Build
TargetPath=\_Projects\PROJECT_SA\Engine\built\release\tools\fxed\FxED.exe
InputPath=\_Projects\PROJECT_SA\Engine\built\release\tools\fxed\FxED.exe
SOURCE="$(InputPath)"

"..\..\..\development\tools\FxEd.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\..\..\development\tools 
	copy $(TargetPath) ..\..\..\development\tools 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "FxED - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\built\Debug\tools\fxed"
# PROP Intermediate_Dir "..\..\built\Debug\tools\fxed"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\..\sdk\inc" /I "..\..\sdk\inc\compat" /I "clientfx\shared\src" /I "..\..\libs\rezmgr" /I "..\..\..\libs\lith" /I "..\..\..\libs\butemgr" /I "..\..\..\libs\cryptmgr" /I "..\..\..\libs\stdlith" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "USE_PRAGMA_LIB" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"msvcrt.lib" /nodefaultlib:"mfc42.lib" /nodefaultlib:"mfcs42.lib" /pdbtype:sept
# Begin Custom Build
TargetPath=\_Projects\PROJECT_SA\Engine\built\Debug\tools\fxed\FxED.exe
InputPath=\_Projects\PROJECT_SA\Engine\built\Debug\tools\fxed\FxED.exe
SOURCE="$(InputPath)"

"..\..\..\..\..\development\tools\FxEd.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\..\..\development\tools 
	copy $(TargetPath) ..\..\..\..\..\development\tools 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "FxED - Win32 Release"
# Name "FxED - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\ChooseClrAnimDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ChooseClrAnimList.cpp
# End Source File
# Begin Source File

SOURCE=.\ChooseKeyDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ChooseMotionAnimDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ColourKeyCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\ColourKeyDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CopyDataDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EditVectorDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FloatButton.cpp
# End Source File
# Begin Source File

SOURCE=.\FloatDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FloatSpinCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\FoundationWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\FxDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FxED.rc
# End Source File
# Begin Source File

SOURCE=.\FxMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\FxPropDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\GameInfoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ImportSpellsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\IntButton.cpp
# End Source File
# Begin Source File

SOURCE=.\IntDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\IntSpinCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\Key.cpp
# End Source File
# Begin Source File

SOURCE=.\KeyControl.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\Matrix.cpp
# End Source File
# Begin Source File

SOURCE=.\MotionKeysDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Phase.cpp
# End Source File
# Begin Source File

SOURCE=.\PhaseInfoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ResourceLocator.cpp
# End Source File
# Begin Source File

SOURCE=.\RezButton.cpp
# End Source File
# Begin Source File

SOURCE=.\RezDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ScaleAnimDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ScaleKeyCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\ScaleKeysDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SequencerWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\Spell.cpp
# End Source File
# Begin Source File

SOURCE=.\SpellBaseParmDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SpellCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\SpellDefTab.cpp
# End Source File
# Begin Source File

SOURCE=.\SpellDlgBar.cpp
# End Source File
# Begin Source File

SOURCE=.\SpellEd.cpp
# End Source File
# Begin Source File

SOURCE=.\SpellEdDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\SpellEdView.cpp
# End Source File
# Begin Source File

SOURCE=.\SpellMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\SpellResolutionDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SpellsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SpellTabCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\Splash.cpp
# End Source File
# Begin Source File

SOURCE=.\StandardDefs.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StringDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TimeBarDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Track.cpp
# End Source File
# Begin Source File

SOURCE=.\TrackScroll.cpp
# End Source File
# Begin Source File

SOURCE=.\TrackWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\VectorButton.cpp
# End Source File
# Begin Source File

SOURCE=.\VectorCombo.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AlterAttributes.h
# End Source File
# Begin Source File

SOURCE=.\AlterAttributesDlg.h
# End Source File
# Begin Source File

SOURCE=.\BaseEffect.h
# End Source File
# Begin Source File

SOURCE=.\BaseSpellDef.h
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\ChooseClrAnimDlg.h
# End Source File
# Begin Source File

SOURCE=.\ChooseClrAnimList.h
# End Source File
# Begin Source File

SOURCE=.\ChooseKeyDlg.h
# End Source File
# Begin Source File

SOURCE=.\ChooseMotionAnimDlg.h
# End Source File
# Begin Source File

SOURCE=.\ColourKeyCtrl.h
# End Source File
# Begin Source File

SOURCE=.\ColourKeyDlg.h
# End Source File
# Begin Source File

SOURCE=.\CopyDataDlg.h
# End Source File
# Begin Source File

SOURCE=.\EditVectorDlg.h
# End Source File
# Begin Source File

SOURCE=.\EffectTypes.h
# End Source File
# Begin Source File

SOURCE=.\FloatButton.h
# End Source File
# Begin Source File

SOURCE=.\FloatDlg.h
# End Source File
# Begin Source File

SOURCE=.\FloatSpinCtrl.h
# End Source File
# Begin Source File

SOURCE=.\FoundationWnd.h
# End Source File
# Begin Source File

SOURCE=.\FxDlg.h
# End Source File
# Begin Source File

SOURCE=.\FxMgr.h
# End Source File
# Begin Source File

SOURCE=.\FxPropDlg.h
# End Source File
# Begin Source File

SOURCE=.\GameInfoDlg.h
# End Source File
# Begin Source File

SOURCE=.\ImportSpellsDlg.h
# End Source File
# Begin Source File

SOURCE=.\IntButton.h
# End Source File
# Begin Source File

SOURCE=.\IntDlg.h
# End Source File
# Begin Source File

SOURCE=.\IntSpinCtrl.h
# End Source File
# Begin Source File

SOURCE=.\Key.h
# End Source File
# Begin Source File

SOURCE=.\KeyControl.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\Matrix.h
# End Source File
# Begin Source File

SOURCE=.\MotionKeysDlg.h
# End Source File
# Begin Source File

SOURCE=.\Phase.h
# End Source File
# Begin Source File

SOURCE=.\PhaseInfoDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\ResourceLocator.h
# End Source File
# Begin Source File

SOURCE=.\RezButton.h
# End Source File
# Begin Source File

SOURCE=.\RezDlg.h
# End Source File
# Begin Source File

SOURCE=.\ScaleAnimDlg.h
# End Source File
# Begin Source File

SOURCE=.\ScaleKeyCtrl.h
# End Source File
# Begin Source File

SOURCE=.\ScaleKeysDlg.h
# End Source File
# Begin Source File

SOURCE=.\SequencerWnd.h
# End Source File
# Begin Source File

SOURCE=.\Spell.h
# End Source File
# Begin Source File

SOURCE=.\SpellBaseParmDlg.h
# End Source File
# Begin Source File

SOURCE=.\SpellCtrl.h
# End Source File
# Begin Source File

SOURCE=.\SpellDefTab.h
# End Source File
# Begin Source File

SOURCE=.\SpellDlgBar.h
# End Source File
# Begin Source File

SOURCE=.\SpellEd.h
# End Source File
# Begin Source File

SOURCE=.\SpellEdDoc.h
# End Source File
# Begin Source File

SOURCE=.\SpellEdView.h
# End Source File
# Begin Source File

SOURCE=.\SpellMgr.h
# End Source File
# Begin Source File

SOURCE=..\SpellParser.h
# End Source File
# Begin Source File

SOURCE=.\SpellResolutionDlg.h
# End Source File
# Begin Source File

SOURCE=.\SpellsDlg.h
# End Source File
# Begin Source File

SOURCE=.\SpellTabCtrl.h
# End Source File
# Begin Source File

SOURCE=.\Splash.h
# End Source File
# Begin Source File

SOURCE=.\StandardDefs.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\StringDlg.h
# End Source File
# Begin Source File

SOURCE=.\TimeBarDlg.h
# End Source File
# Begin Source File

SOURCE=.\Track.h
# End Source File
# Begin Source File

SOURCE=.\TrackScroll.h
# End Source File
# Begin Source File

SOURCE=.\TrackWnd.h
# End Source File
# Begin Source File

SOURCE=.\Vector.h
# End Source File
# Begin Source File

SOURCE=.\VectorButton.h
# End Source File
# Begin Source File

SOURCE=.\VectorCombo.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\closed1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\closed1.ico
# End Source File
# Begin Source File

SOURCE=.\res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\left.cur
# End Source File
# Begin Source File

SOURCE=.\res\open1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\open1.ico
# End Source File
# Begin Source File

SOURCE=.\res\right1.cur
# End Source File
# Begin Source File

SOURCE=.\res\spell1.ico
# End Source File
# Begin Source File

SOURCE=.\res\SpellEd.ico
# End Source File
# Begin Source File

SOURCE=.\res\SpellEd.rc2
# End Source File
# Begin Source File

SOURCE=.\res\SpellEdDoc.ico
# End Source File
# Begin Source File

SOURCE=.\Splsh16.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\wrap.bmp
# End Source File
# End Group
# Begin Group "libs_debug"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Libs\built\Debug\ButeMgrMfcDll.lib

!IF  "$(CFG)" == "FxED - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FxED - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Libs\built\Debug\Lib_StdLith.lib

!IF  "$(CFG)" == "FxED - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FxED - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\libs\built\Debug\Lib_Lith.lib

!IF  "$(CFG)" == "FxED - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FxED - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Libs\built\Debug\CryptMgrMfcDll.lib

!IF  "$(CFG)" == "FxED - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FxED - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\built\Debug\libs\Lib_RezMgr\Lib_RezMgr.lib

!IF  "$(CFG)" == "FxED - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FxED - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "libs_release"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\built\Release\libs\Lib_RezMgr\Lib_RezMgr.lib

!IF  "$(CFG)" == "FxED - Win32 Release"

!ELSEIF  "$(CFG)" == "FxED - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Libs\built\Release\ButeMgrMfcDll.lib

!IF  "$(CFG)" == "FxED - Win32 Release"

!ELSEIF  "$(CFG)" == "FxED - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Libs\built\Release\Lib_StdLith.lib

!IF  "$(CFG)" == "FxED - Win32 Release"

!ELSEIF  "$(CFG)" == "FxED - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\libs\built\Release\Lib_Lith.lib

!IF  "$(CFG)" == "FxED - Win32 Release"

!ELSEIF  "$(CFG)" == "FxED - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\libs\built\Release\CryptMgrMfcDll.lib

!IF  "$(CFG)" == "FxED - Win32 Release"

!ELSEIF  "$(CFG)" == "FxED - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=..\..\FxED_BuildNotes.txt
# End Source File
# End Target
# End Project
# Section FxED : {72ADFD78-2C39-11D0-9903-00A0C91BC942}
# 	1:10:IDB_SPLASH:104
# 	2:21:SplashScreenInsertKey:4.0
# End Section
