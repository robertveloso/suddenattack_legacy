# Microsoft Developer Studio Project File - Name="DEdit" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=DEdit - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dedit60.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dedit60.mak" CFG="DEdit - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DEdit - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "DEdit - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DEdit - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Built\Release\tools\DEdit"
# PROP Intermediate_Dir "..\..\Built\Release\tools\DEdit"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W2 /GX /O2 /Op /I "..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\shared\model\\" /I "mrcext" /I "mrcext\vc_headers" /I "..\..\sdk\inc" /I "." /I "..\preprocessor" /I "options" /I "..\shared\world\\" /I "misc" /I "dlgs" /I "controls" /I "frames" /I "project" /I "..\shared\engine" /I "regionedit" /I "draw" /I "docs" /I "views" /I "..\..\libs\rezmgr" /I "..\..\..\libs\stdlith" /I "..\..\..\libs\stdlith\sys\win" /I "trackers" /I "..\..\sdk\inc\physics" /I ".\modelview" /I ".\fileformats" /I ".\keyaggregate" /I ".\levelerror" /I ".\lightmap" /I "..\..\..\libs\genregmgr" /I "..\..\..\libs\dibmgr" /I "..\..\..\libs\regmgr" /I "..\..\..\libs\butemgr" /I "..\..\..\libs\lith" /I "..\..\libs\ltamgr" /I "..\..\..\libs\cryptmgr" /D "NDEBUG" /D "REZMGRFULL" /D "WIN32" /D "_WINDOWS" /D "DIRECTEDITOR_BUILD" /D "_MBCS" /D "STDLITH_MFC" /D "BDEFS_MFC" /D "PRE_FLOAT" /D "PREPROCESSOR_BUILD" /D "NO_PRAGMA_LIBS" /D "LT15_COMPAT" /D "_AFXDLL" /Fr /Yu"bdefs.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 d3drm.lib ddraw.lib d3dx8.lib d3d8.lib dxguid.lib winmm.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libcmtd.lib" /nodefaultlib:"libcmt.lib" /out:"..\..\Built\Release\tools\DEdit/dedit.exe"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build
TargetPath=\Working\ENGINE\Built\Release\tools\DEdit\dedit.exe
InputPath=\Working\ENGINE\Built\Release\tools\DEdit\dedit.exe
SOURCE="$(InputPath)"

"..\..\..\..\..\development\tools\dedit.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\..\..\development\tools 
	copy $(TargetPath) ..\..\..\..\..\development\tools 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\Built\debug\tools\DEdit"
# PROP Intermediate_Dir "..\..\Built\debug\tools\DEdit"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W2 /Gm /GX /ZI /Od /I "..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\shared\model\\" /I "mrcext" /I "mrcext\vc_headers" /I "..\..\sdk\inc" /I "." /I "..\preprocessor" /I "options" /I "..\shared\world\\" /I "misc" /I "dlgs" /I "controls" /I "frames" /I "project" /I "..\shared\engine" /I "regionedit" /I "draw" /I "docs" /I "views" /I "..\..\libs\rezmgr" /I "..\..\..\libs\stdlith" /I "..\..\..\libs\stdlith\sys\win" /I "trackers" /I "..\..\sdk\inc\physics" /I ".\modelview" /I ".\fileformats" /I ".\keyaggregate" /I ".\levelerror" /I ".\lightmap" /I "..\..\..\libs\genregmgr" /I "..\..\..\libs\dibmgr" /I "..\..\..\libs\regmgr" /I "..\..\..\libs\butemgr" /I "..\..\..\libs\lith" /I "..\..\libs\ltamgr" /I "..\..\..\libs\cryptmgr" /D "_DEBUG" /D "_AFXDLL" /D "REZMGRFULL" /D "WIN32" /D "_WINDOWS" /D "DIRECTEDITOR_BUILD" /D "_MBCS" /D "STDLITH_MFC" /D "BDEFS_MFC" /D "PRE_FLOAT" /D "PREPROCESSOR_BUILD" /D "NO_PRAGMA_LIBS" /D "LT15_COMPAT" /FR /Yu"bdefs.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 d3drm.lib odbc32.lib odbccp32.lib ddraw.lib d3dx8.lib d3d8.lib dxguid.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libcmtd.lib" /nodefaultlib:"msvcrt.lib" /nodefaultlib:"mfc42.lib" /nodefaultlib:"mfcs42.lib" /nodefaultlib:"libcmt.lib" /out:"..\..\Built\debug\tools\DEdit/dedit.exe"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build
TargetPath=\Working\ENGINE\Built\debug\tools\DEdit\dedit.exe
InputPath=\Working\ENGINE\Built\debug\tools\DEdit\dedit.exe
SOURCE="$(InputPath)"

"..\..\..\development\tools\dedit.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\development\tools 
	copy $(TargetPath) ..\..\..\development\tools 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "DEdit - Win32 Release"
# Name "DEdit - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Group "Options Pages"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\OptionsSheet.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dlgs\PropPageDisplay.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2 /I ".."

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

# ADD CPP /I ".."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dlgs\PropPageOptionsClipboard.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2 /I ".."

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

# ADD CPP /I ".."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Dlgs\PropPageOptionsControls.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgs\PropPageOptionsD3D.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Dlgs\PropPageOptionsMisc.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlgs\PropPageOptionsModels.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlgs\PropPageOptionsPrefabs.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgs\PropPageOptionsRun.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2 /I ".."

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

# ADD CPP /I ".."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UndoPage.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "Project Bar"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BaseImgDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ModelDlg.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NodeView.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PrefabDlg.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Controls\ProjectBar.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Dlgs\PropertiesDlg.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SoundDlg.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SpriteDlg.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TextureDlg.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WorldsDlg.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "Model"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\shared\model\ltaScene.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\shared\model\model.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\model\model_ops.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\model\modelallocations.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\model\transformmaker.cpp
# End Source File
# End Group
# Begin Group "Options"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Options\OptionsAdvancedSelect.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Options\OptionsBase.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Options\OptionsClassDialog.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Options\OptionsClipboard.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Options\OptionsControls.cpp
# End Source File
# Begin Source File

SOURCE=.\Options\OptionsDisplay.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Options\OptionsGenerateUniqueNames.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Options\OptionsMisc.cpp
# End Source File
# Begin Source File

SOURCE=.\Options\OptionsModels.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Options\OptionsObjectBrowser.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Options\OptionsPrefabs.cpp
# End Source File
# Begin Source File

SOURCE=.\Options\OptionsRun.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Options\OptionsViewports.cpp
# End Source File
# Begin Source File

SOURCE=.\Options\OptionsWindows.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "Level Error"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\LevelError\Detectors.cpp
# End Source File
# Begin Source File

SOURCE=.\LevelError\ErrorDetector.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=.\Dlgs\ErrorInformationDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LevelError\LevelError.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=.\LevelError\LevelErrorDB.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=.\Dlgs\LevelErrorDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlgs\LevelErrorOptionsDlg.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Dlgs\AddPatchDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RegionEdit\AdvancedSelect.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Dlgs\AdvancedSelectDlg.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AllTextureDlg.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Dlgs\AlphaFromColorDlg.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Dlgs\AutoTextureOptionsDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\world\BasePoly.cpp
# End Source File
# Begin Source File

SOURCE=.\baserezdlg.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\shared\engine\bdefs.cpp
# ADD CPP /Yc"bdefs.h"
# End Source File
# Begin Source File

SOURCE=..\shared\world\bezierpatch.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\bindkeydlg.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\shared\engine\bindmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlgs\CameraFOVDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\engine\classbind.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlgs\ClassDlg.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ClassList.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Dlgs\ClassListDlg.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Controls\ClassTree.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FileFormats\CObjInterface.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ColorPicker.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Dlgs\ColorSelectDlg.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ColorWell.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\controls\ColumnCtrl.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\shared\engine\concommand.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\engine\conparse.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\engine\converge.cpp
# End Source File
# Begin Source File

SOURCE=.\RegionEdit\CreatePrimitive.cpp
# End Source File
# Begin Source File

SOURCE=.\CylinderPrimDlg.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\draw\d3d_texturemgr.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Project\d_filemgr.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Dlgs\DebugDlg.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2 /I ".."

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

# ADD CPP /I ".."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DEdit.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dedit_concommand.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Options\DEditOptions.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Frames\DEDockFrameWnd.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\shared\engine\dhashtable.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlgs\DirDialog.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\draw\draw_d3d.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\draw\DrawBase.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\shared\engine\dsys_interface.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\engine\dtxmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\RegionEdit\edit_actions.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\shared\world\EditBrush.cpp
# End Source File
# Begin Source File

SOURCE=.\RegionEdit\EditClipboard.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Dlgs\EditEffectGroupDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RegionEdit\EditGrid.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Misc\EditHelpers.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\shared\world\EditObjects.cpp
# End Source File
# Begin Source File

SOURCE=.\Misc\EditorTransfer.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\shared\world\EditPatch.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\world\EditPlane.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\world\EditPoly.cpp
# End Source File
# Begin Source File

SOURCE=.\Project\EditProjectMgr.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\shared\world\EditRegion.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlgs\EventEditorDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlgs\EventListDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportObjFile.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FilePalette.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Misc\FileUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\Frames\flattoolbar.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Controls\FrameList.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dlgs\GenerateUniqueNamesDlg.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2 /I ".."

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

# ADD CPP /I ".."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\shared\engine\genericprop_setup.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\engine\genltstream.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\engine\geomroutines.cpp
# End Source File
# Begin Source File

SOURCE=.\globalhotkeydb.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hotkey.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hotkeydb.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Dlgs\ImportBumpMapDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlgs\ImportCubeMapDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgs\ImportObjectDlg.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ImportObjFileFormat.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\keyconfigdlg.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\KeyAggregate\KeyDefaultAggregate.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\KeyAggregate\KeyDefaultFactory.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlgs\LevelItemsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlgs\leveltexturesdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlgs\LevelTexturesOptionsDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\engine\light_table.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlgs\LightAttenuation.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\engine\lightmap_compress.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\engine\lightmap_planes.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\engine\load_pcx.cpp
# End Source File
# Begin Source File

SOURCE=.\LoadLTADialog.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\sdk\inc\LTEulerAngles.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\sdk\inc\ltquatbase.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\shared\world\LTTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\Frames\MainFrm.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MapTextureCoordsDlg.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ModelView\meshshape.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\ModelView\meshshapelist.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\ModelView\modelmesh.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\ModelView\modelmgr.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\mtreectl.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MultiLineStringDlg.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dlgs\NameChangeReportDlg.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2 /I ".."

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

# ADD CPP /I ".."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\draw\Navigator.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dlgs\navigatordlg.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dlgs\NavigatorStoreDlg.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2 /I ".."

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

# ADD CPP /I ".."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Dlgs\NewProjectDlg.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2 /I ".."

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

# ADD CPP /I ".."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\shared\engine\nexus.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\world\node_ops.cpp
# End Source File
# Begin Source File

SOURCE=.\NodeViewTreeCtrl.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dlgs\ObjectBrowserDlg.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2 /I ".."

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

# ADD CPP /I ".."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ObjectImporter.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Dlgs\ObjectSearchDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlgs\ObjectTrackerDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PaletteEdit.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\shared\engine\parse_world_info.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\engine\pixelformat.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgs\PopupNoteDlg.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2 /I ".."

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

# ADD CPP /I ".."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\shared\world\PrefabMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\world\PrefabRef.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlgs\PrefabTrackerDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Controls\ProjectDirDlg.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Controls\ProjectTabControlBar.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\controls\ProjectTabCtrl.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2 /I ".."

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

# ADD CPP /I ".."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\controls\PropEditString.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Controls\PropEditStringList.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Docs\PropertyHelpers.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\shared\world\PropList.cpp
# End Source File
# Begin Source File

SOURCE=.\RegionEdit\Refs.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Docs\RegionDoc.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Frames\RegionFrame.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Views\RegionView.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Views\RegionViewInitTracker.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlgs\RenameResourceDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Controls\ResizeBar.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ResizeDlg.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ResNewDir.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ResourceMgr.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RotateSelection.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Dlgs\RotationEdit.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2 /I ".."

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

# ADD CPP /I ".."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\shared\world\s3tc_compress.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlgs\ScaleSelectDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlgs\SelectEffectGroupDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlgs\SelectEffectGroupDlg.h
# End Source File
# Begin Source File

SOURCE=.\Dlgs\SetPatchResDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\engine\simple_dalloc.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlgs\SolidAlphaDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgs\SpherePrimitiveDlg.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2 /I ".."

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

# ADD CPP /I ".."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Misc\Splash.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Dlgs\SpriteEditDlg.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\shared\engine\streamsim.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlgs\StringDlg.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Misc\Texture.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\shared\world\TexturedPlane.cpp
# End Source File
# Begin Source File

SOURCE=.\TextureProp.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Dlgs\TextureSearchReplaceDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlgs\TextureTrackerDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\UIEvent.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RegionEdit\undo_mgr.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Dlgs\UndoWarningDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\UpdateAllObjects.cpp
# End Source File
# Begin Source File

SOURCE=.\draw\UsefulDC.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\draw\UsefulDib.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\shared\world\UVtoOPQ.cpp
# End Source File
# Begin Source File

SOURCE=.\VectorButton.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\VectorEdit.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\shared\engine\version_info.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\engine\version_resource.cpp
# End Source File
# Begin Source File

SOURCE=.\draw\ViewDef.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RegionEdit\ViewFunctions.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RegionEdit\viewrender.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Dlgs\WorldInfoDlg.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\shared\world\WorldNode.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Group "Options H"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\Options\OptionsAdvancedSelect.h
# End Source File
# Begin Source File

SOURCE=.\Options\OptionsBase.h
# End Source File
# Begin Source File

SOURCE=.\Options\OptionsClassDialog.h
# End Source File
# Begin Source File

SOURCE=.\Options\OptionsClipboard.h
# End Source File
# Begin Source File

SOURCE=.\Options\OptionsControls.h
# End Source File
# Begin Source File

SOURCE=.\Options\OptionsDisplay.h
# End Source File
# Begin Source File

SOURCE=.\Options\OptionsGenerateUniqueNames.h
# End Source File
# Begin Source File

SOURCE=.\Options\OptionsMisc.h
# End Source File
# Begin Source File

SOURCE=.\Options\OptionsModels.h
# End Source File
# Begin Source File

SOURCE=.\Options\OptionsObjectBrowser.h
# End Source File
# Begin Source File

SOURCE=.\Options\OptionsRun.h
# End Source File
# Begin Source File

SOURCE=.\OptionsSheet.h
# End Source File
# Begin Source File

SOURCE=.\Options\OptionsViewports.h
# End Source File
# Begin Source File

SOURCE=.\Options\OptionsWindows.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Dlgs\AddPatchDlg.h
# End Source File
# Begin Source File

SOURCE=.\regionedit\AdvancedSelect.h
# End Source File
# Begin Source File

SOURCE=.\AllTextureDlg.h
# End Source File
# Begin Source File

SOURCE=.\BaseImgDlg.h
# End Source File
# Begin Source File

SOURCE=.\baserezdlg.h
# End Source File
# Begin Source File

SOURCE=..\shared\world\bezierpatch.h
# End Source File
# Begin Source File

SOURCE=.\bindkeydlg.h
# End Source File
# Begin Source File

SOURCE=.\Dlgs\CameraFOVDlg.h
# End Source File
# Begin Source File

SOURCE=.\draw\CCS.h
# End Source File
# Begin Source File

SOURCE=.\Dlgs\ClassDlg.h
# End Source File
# Begin Source File

SOURCE=.\ClassList.h
# End Source File
# Begin Source File

SOURCE=.\Controls\ClassTree.h
# End Source File
# Begin Source File

SOURCE=.\ColorPicker.h
# End Source File
# Begin Source File

SOURCE=.\Dlgs\ColorSelectDlg.h
# End Source File
# Begin Source File

SOURCE=.\ColorWell.h
# End Source File
# Begin Source File

SOURCE=.\controls\ColumnCtrl.h
# End Source File
# Begin Source File

SOURCE=.\Dlgs\DebugDlg.h
# End Source File
# Begin Source File

SOURCE=.\dedit.h
# End Source File
# Begin Source File

SOURCE=.\DEditInternal.h
# End Source File
# Begin Source File

SOURCE=.\Options\DEditOptions.h
# End Source File
# Begin Source File

SOURCE=.\Frames\DEDockFrameWnd.h
# End Source File
# Begin Source File

SOURCE=.\LevelError\Detectors.h
# End Source File
# Begin Source File

SOURCE=.\Dlgs\DirDialog.h
# End Source File
# Begin Source File

SOURCE=.\draw\draw_d3d.h
# End Source File
# Begin Source File

SOURCE=.\draw\DrawBase.h
# End Source File
# Begin Source File

SOURCE=.\RegionEdit\edit_actions.h
# End Source File
# Begin Source File

SOURCE=..\shared\world\EditBrush.h
# End Source File
# Begin Source File

SOURCE=.\RegionEdit\EditClipboard.h
# End Source File
# Begin Source File

SOURCE=.\Dlgs\EditEffectGroupDlg.h
# End Source File
# Begin Source File

SOURCE=.\RegionEdit\EditGrid.h
# End Source File
# Begin Source File

SOURCE=.\Misc\EditHelpers.h
# End Source File
# Begin Source File

SOURCE=..\shared\world\EditObjects.h
# End Source File
# Begin Source File

SOURCE=.\Misc\EditorTransfer.h
# End Source File
# Begin Source File

SOURCE=..\shared\world\EditPatch.h
# End Source File
# Begin Source File

SOURCE=..\shared\world\EditPlane.h
# End Source File
# Begin Source File

SOURCE=..\shared\world\EditPoly.h
# End Source File
# Begin Source File

SOURCE=.\Project\EditProjectMgr.h
# End Source File
# Begin Source File

SOURCE=..\shared\world\EditRay.h
# End Source File
# Begin Source File

SOURCE=..\shared\world\EditRegion.h
# End Source File
# Begin Source File

SOURCE=..\shared\world\EditVert.h
# End Source File
# Begin Source File

SOURCE=.\LevelError\ErrorDetector.h
# End Source File
# Begin Source File

SOURCE=.\Dlgs\ErrorInformationDlg.h
# End Source File
# Begin Source File

SOURCE=.\Dlgs\EventEditorDlg.h
# End Source File
# Begin Source File

SOURCE=.\Dlgs\EventListDlg.h
# End Source File
# Begin Source File

SOURCE=.\eventnames.h
# End Source File
# Begin Source File

SOURCE=.\ExportObjFile.h
# End Source File
# Begin Source File

SOURCE=.\extendedvk.h
# End Source File
# Begin Source File

SOURCE=.\FilePalette.h
# End Source File
# Begin Source File

SOURCE=.\Misc\FileUtils.h
# End Source File
# Begin Source File

SOURCE=.\Frames\flattoolbar.h
# End Source File
# Begin Source File

SOURCE=.\Controls\FrameList.h
# End Source File
# Begin Source File

SOURCE=.\dlgs\GenerateUniqueNamesDlg.h
# End Source File
# Begin Source File

SOURCE=.\globalhotkeydb.h
# End Source File
# Begin Source File

SOURCE=.\hotkey.h
# End Source File
# Begin Source File

SOURCE=.\hotkeydb.h
# End Source File
# Begin Source File

SOURCE=.\Dlgs\ImportBumpMapDlg.h
# End Source File
# Begin Source File

SOURCE=.\Dlgs\ImportCubeMapDlg.h
# End Source File
# Begin Source File

SOURCE=.\dlgs\ImportObjectDlg.h
# End Source File
# Begin Source File

SOURCE=.\ImportObjFileFormat.h
# End Source File
# Begin Source File

SOURCE=.\keyconfigdlg.h
# End Source File
# Begin Source File

SOURCE=.\KeyAggregate\KeyDefaultAggregate.h
# End Source File
# Begin Source File

SOURCE=.\KeyAggregate\KeyDefaultFactory.h
# End Source File
# Begin Source File

SOURCE=.\LevelError\LevelError.h
# End Source File
# Begin Source File

SOURCE=.\LevelError\LevelErrorDB.h
# End Source File
# Begin Source File

SOURCE=.\Dlgs\LevelErrorDlg.h
# End Source File
# Begin Source File

SOURCE=.\Dlgs\LevelErrorOptionsDlg.h
# End Source File
# Begin Source File

SOURCE=.\Dlgs\LevelItemsDlg.h
# End Source File
# Begin Source File

SOURCE=.\Dlgs\LevelTexturesDlg.h
# End Source File
# Begin Source File

SOURCE=.\Dlgs\LevelTexturesOptionsDlg.h
# End Source File
# Begin Source File

SOURCE=.\Dlgs\LightAttenuation.h
# End Source File
# Begin Source File

SOURCE=.\LoadLTADialog.h
# End Source File
# Begin Source File

SOURCE=..\..\sdk\inc\LTEulerAngles.h
# End Source File
# Begin Source File

SOURCE=.\Frames\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\MapTextureCoordsDlg.h
# End Source File
# Begin Source File

SOURCE=.\KeyAggregate\MaxKeyAggregate.h
# End Source File
# Begin Source File

SOURCE=.\ModelView\meshshape.h
# End Source File
# Begin Source File

SOURCE=.\ModelView\meshshapelist.h
# End Source File
# Begin Source File

SOURCE=.\ModelDlg.h
# End Source File
# Begin Source File

SOURCE=..\ModelEdit\ModelEditDlg.h
# End Source File
# Begin Source File

SOURCE=.\ModelView\modelmesh.h
# End Source File
# Begin Source File

SOURCE=.\ModelView\modelmgr.h
# End Source File
# Begin Source File

SOURCE=.\mtreectl.h
# End Source File
# Begin Source File

SOURCE=.\MultiLineStringDlg.h
# End Source File
# Begin Source File

SOURCE=.\dlgs\NameChangeReportDlg.h
# End Source File
# Begin Source File

SOURCE=.\draw\Navigator.h
# End Source File
# Begin Source File

SOURCE=.\dlgs\navigatordlg.h
# End Source File
# Begin Source File

SOURCE=.\dlgs\NavigatorStoreDlg.h
# End Source File
# Begin Source File

SOURCE=.\Dlgs\NewProjectDlg.h
# End Source File
# Begin Source File

SOURCE=.\NodeView.h
# End Source File
# Begin Source File

SOURCE=.\NodeViewTreeCtrl.h
# End Source File
# Begin Source File

SOURCE=.\dlgs\ObjectBrowserDlg.h
# End Source File
# Begin Source File

SOURCE=.\ObjectImporter.h
# End Source File
# Begin Source File

SOURCE=.\Dlgs\ObjectSearchDlg.h
# End Source File
# Begin Source File

SOURCE=.\Dlgs\ObjectTrackerDlg.h
# End Source File
# Begin Source File

SOURCE=.\draw\Orientation.h
# End Source File
# Begin Source File

SOURCE=.\PaletteEdit.h
# End Source File
# Begin Source File

SOURCE=.\patchhandle.h
# End Source File
# Begin Source File

SOURCE=.\dlgs\PopupNoteDlg.h
# End Source File
# Begin Source File

SOURCE=.\PrefabDlg.h
# End Source File
# Begin Source File

SOURCE=..\shared\world\PrefabMgr.h
# End Source File
# Begin Source File

SOURCE=..\shared\world\PrefabRef.h
# End Source File
# Begin Source File

SOURCE=.\Dlgs\PrefabTrackerDlg.h
# End Source File
# Begin Source File

SOURCE=.\Controls\ProjectBar.h
# End Source File
# Begin Source File

SOURCE=.\Controls\ProjectDirDlg.h
# End Source File
# Begin Source File

SOURCE=.\Controls\ProjectTabControlBar.h
# End Source File
# Begin Source File

SOURCE=.\controls\ProjectTabCtrl.h
# End Source File
# Begin Source File

SOURCE=.\controls\PropEditString.h
# End Source File
# Begin Source File

SOURCE=.\Controls\PropEditStringList.h
# End Source File
# Begin Source File

SOURCE=.\Dlgs\PropertiesDlg.h
# End Source File
# Begin Source File

SOURCE=.\Docs\PropertyHelpers.h
# End Source File
# Begin Source File

SOURCE=.\dlgs\PropPageDisplay.h
# End Source File
# Begin Source File

SOURCE=.\dlgs\PropPageOptionsClipboard.h
# End Source File
# Begin Source File

SOURCE=.\Dlgs\PropPageOptionsControls.h
# End Source File
# Begin Source File

SOURCE=.\dlgs\PropPageOptionsD3D.h
# End Source File
# Begin Source File

SOURCE=.\Dlgs\PropPageOptionsLighting.h
# End Source File
# Begin Source File

SOURCE=.\Dlgs\PropPageOptionsMisc.h
# End Source File
# Begin Source File

SOURCE=.\Dlgs\PropPageOptionsModels.h
# End Source File
# Begin Source File

SOURCE=.\Dlgs\PropPageOptionsPrefabs.h
# End Source File
# Begin Source File

SOURCE=.\dlgs\PropPageOptionsRun.h
# End Source File
# Begin Source File

SOURCE=.\RegionEdit\Refs.h
# End Source File
# Begin Source File

SOURCE=.\Docs\RegionDoc.h
# End Source File
# Begin Source File

SOURCE=.\Frames\RegionFrame.h
# End Source File
# Begin Source File

SOURCE=.\Views\RegionView.h
# End Source File
# Begin Source File

SOURCE=.\Dlgs\RenameResourceDlg.h
# End Source File
# Begin Source File

SOURCE=.\Controls\ResizeBar.h
# End Source File
# Begin Source File

SOURCE=.\ResizeDlg.h
# End Source File
# Begin Source File

SOURCE=.\ResNewDir.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\ResourceMgr.h
# End Source File
# Begin Source File

SOURCE=.\RotateSelection.h
# End Source File
# Begin Source File

SOURCE=.\Dlgs\RotationEdit.h
# End Source File
# Begin Source File

SOURCE=.\Dlgs\ScaleSelectDlg.h
# End Source File
# Begin Source File

SOURCE=.\Dlgs\SetPatchResDlg.h
# End Source File
# Begin Source File

SOURCE=.\Dlgs\SolidAlphaDlg.h
# End Source File
# Begin Source File

SOURCE=.\SoundDlg.h
# End Source File
# Begin Source File

SOURCE=.\dlgs\SpherePrimitiveDlg.h
# End Source File
# Begin Source File

SOURCE=.\Misc\Splash.h
# End Source File
# Begin Source File

SOURCE=.\SpriteDlg.h
# End Source File
# Begin Source File

SOURCE=.\Dlgs\SpriteEditDlg.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# Begin Source File

SOURCE=.\Dlgs\StringDlg.h
# End Source File
# Begin Source File

SOURCE=.\Misc\Texture.h
# End Source File
# Begin Source File

SOURCE=.\TextureDlg.h
# End Source File
# Begin Source File

SOURCE=..\shared\world\TexturedPlane.h
# End Source File
# Begin Source File

SOURCE=.\TextureProp.h
# End Source File
# Begin Source File

SOURCE=.\Dlgs\TextureSearchReplaceDlg.h
# End Source File
# Begin Source File

SOURCE=.\Dlgs\TextureTrackerDlg.h
# End Source File
# Begin Source File

SOURCE=.\UIEvent.h
# End Source File
# Begin Source File

SOURCE=.\RegionEdit\undo_mgr.h
# End Source File
# Begin Source File

SOURCE=.\UndoPage.h
# End Source File
# Begin Source File

SOURCE=.\Dlgs\UndoWarningDlg.h
# End Source File
# Begin Source File

SOURCE=.\draw\UsefulDC.h
# End Source File
# Begin Source File

SOURCE=.\draw\UsefulDib.h
# End Source File
# Begin Source File

SOURCE=..\shared\world\UVtoOPQ.h
# End Source File
# Begin Source File

SOURCE=.\VectorButton.h
# End Source File
# Begin Source File

SOURCE=.\VectorEdit.h
# End Source File
# Begin Source File

SOURCE=.\draw\ViewDef.h
# End Source File
# Begin Source File

SOURCE=.\Dlgs\WorldInfoDlg.h
# End Source File
# Begin Source File

SOURCE=..\shared\world\WorldNode.h
# End Source File
# Begin Source File

SOURCE=.\WorldsDlg.h
# End Source File
# End Group
# Begin Group "mrcext"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MRCEXT\CFGTBAR.H
# End Source File
# Begin Source File

SOURCE=.\MRCEXT\DRAGDOCK.CPP
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\MRCEXT\FRAMEWND.CPP
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\MRCEXT\MDIFLOAT.CPP
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\MRCEXT\MDIFLOAT.H
# End Source File
# Begin Source File

SOURCE=.\MRCEXT\MRCButton.h
# End Source File
# Begin Source File

SOURCE=.\MRCEXT\MRCEXT.H
# End Source File
# Begin Source File

SOURCE=.\MRCEXT\MRCPRIV.H
# End Source File
# Begin Source File

SOURCE=.\MRCEXT\MRCRESRC.H
# End Source File
# Begin Source File

SOURCE=.\MRCEXT\MRCSTAFX.H
# End Source File
# Begin Source File

SOURCE=.\MRCEXT\SIZECONT.CPP
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\MRCEXT\SIZEDLG.CPP
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\MRCEXT\SIZEDOCK.CPP
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\MRCEXT\SZTOOLBA.CPP
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "Trackers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Trackers\RVCallback.h
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVCommand.cpp
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVCommand.h
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTracker.cpp
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTracker.h
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerBrushSize.cpp
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerBrushSize.h
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerCurveEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerCurveEdit.h
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerDrawPoly.cpp
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerDrawPoly.h
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerExtrudePoly.cpp
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerExtrudePoly.h
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerFarDist.cpp
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerFarDist.h
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerFocus.cpp
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerFocus.h
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerGridSize.cpp
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerGridSize.h
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerMarkerMove.cpp
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerMarkerMove.h
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerMenuItem.cpp
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerMenuItem.h
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerNavArcRotate.cpp
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerNavArcRotate.h
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerNavDrag.cpp
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerNavDrag.h
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerNavMove.cpp
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerNavMove.h
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerNavOrbit.cpp
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerNavOrbit.h
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerNavRotate.cpp
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerNavRotate.h
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerNodeMove.cpp
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerNodeMove.h
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerNodeRotate.cpp
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerNodeRotate.h
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerObjectSize.cpp
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerObjectSize.h
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerPolyScale.cpp
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerPolyScale.h
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerTag.cpp
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerTag.h
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerTextMove.cpp
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerTextMove.h
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerTextWrap.cpp
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerTextWrap.h
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerVertMove.cpp
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerVertMove.h
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerVertScale.cpp
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerVertScale.h
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerVertSnap.cpp
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerVertSnap.h
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerZoom.cpp
# End Source File
# Begin Source File

SOURCE=.\Trackers\RVTrackerZoom.h
# End Source File
# Begin Source File

SOURCE=.\Trackers\StdRVTrackers.h
# End Source File
# Begin Source File

SOURCE=.\Trackers\TrackerMgr.cpp

!IF  "$(CFG)" == "DEdit - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Trackers\TrackerMgr.h
# End Source File
# Begin Source File

SOURCE=.\Trackers\UITracker.h
# End Source File
# End Group
# Begin Group "libs"

# PROP Default_Filter "*.lib"
# Begin Source File

SOURCE=..\shared\world\s3tc.lib
# End Source File
# End Group
# Begin Group "libs_debug"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Libs\built\Debug\CryptMgrMfcDll\CryptMgrMfcDll.lib

!IF  "$(CFG)" == "DEdit - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\libs\built\Debug\dibmgr_mfcdll\dibmgr_mfcdll.lib

!IF  "$(CFG)" == "DEdit - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Libs\built\Debug\ButeMgrMFCDll\ButeMgrMfcDll.lib

!IF  "$(CFG)" == "DEdit - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Libs\built\Debug\Lib_StdLith\Lib_StdLith.lib

!IF  "$(CFG)" == "DEdit - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\libs\built\Debug\GenRegMgr\genregmgr.lib

!IF  "$(CFG)" == "DEdit - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\libs\built\Debug\RegMgr\regmgr.lib

!IF  "$(CFG)" == "DEdit - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\built\Debug\libs\LTAMgr\ltamgr.lib

!IF  "$(CFG)" == "DEdit - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\built\Debug\libs\Lib_RezMgr\Lib_RezMgr.lib

!IF  "$(CFG)" == "DEdit - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\libs\built\Debug\Lib_Lith\Lib_Lith.lib

!IF  "$(CFG)" == "DEdit - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "libs_release"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\libs\built\Release\CryptMgrMfcDll\CryptMgrMfcDll.lib

!IF  "$(CFG)" == "DEdit - Win32 Release"

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\libs\built\Release\dibmgr_mfcdll\dibmgr_mfcdll.lib

!IF  "$(CFG)" == "DEdit - Win32 Release"

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Libs\built\Release\ButeMgrMFCDll\ButeMgrMfcDll.lib

!IF  "$(CFG)" == "DEdit - Win32 Release"

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Libs\built\Release\Lib_StdLith\Lib_StdLith.lib

!IF  "$(CFG)" == "DEdit - Win32 Release"

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\libs\built\Release\GenRegMgr\genregmgr.lib

!IF  "$(CFG)" == "DEdit - Win32 Release"

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\libs\built\Release\RegMgr\regmgr.lib

!IF  "$(CFG)" == "DEdit - Win32 Release"

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\libs\built\Release\Lib_Lith\Lib_Lith.lib

!IF  "$(CFG)" == "DEdit - Win32 Release"

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\built\Release\libs\LTAMgr\ltamgr.lib

!IF  "$(CFG)" == "DEdit - Win32 Release"

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\built\Release\libs\Lib_RezMgr\Lib_RezMgr.lib

!IF  "$(CFG)" == "DEdit - Win32 Release"

!ELSEIF  "$(CFG)" == "DEdit - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\res\autosave.ico
# End Source File
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\CLASSHLP.BUT
# End Source File
# Begin Source File

SOURCE=.\res\classlis.bmp
# End Source File
# Begin Source File

SOURCE=.\res\clipboard.ico
# End Source File
# Begin Source File

SOURCE=.\res\controls.ico
# End Source File
# Begin Source File

SOURCE=.\res\copy.ico
# End Source File
# Begin Source File

SOURCE=.\res\cur00001.cur
# End Source File
# Begin Source File

SOURCE=.\res\cur00002.cur
# End Source File
# Begin Source File

SOURCE=.\res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\res\cursor2.cur
# End Source File
# Begin Source File

SOURCE=.\res\cursor_r.cur
# End Source File
# Begin Source File

SOURCE=.\res\d3d.ico
# End Source File
# Begin Source File

SOURCE=.\res\DEdit.ico
# End Source File
# Begin Source File

SOURCE=.\dedit.rc
# End Source File
# Begin Source File

SOURCE=.\res\DEditDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\DemoSkyWorldModel.dtx
# End Source File
# Begin Source File

SOURCE=.\res\DirectEditor.ico
# End Source File
# Begin Source File

SOURCE=.\res\DirLight.dtx
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

SOURCE=.\res\InsideDef.dtx
# End Source File
# Begin Source File

SOURCE=.\res\Light.dtx
# End Source File
# Begin Source File

SOURCE=.\res\lighting.ico
# End Source File
# Begin Source File

SOURCE=.\res\loadanim.avi
# End Source File
# Begin Source File

SOURCE=.\res\misc.ico
# End Source File
# Begin Source File

SOURCE=.\res\models.ico
# End Source File
# Begin Source File

SOURCE=.\res\node_sta.bmp
# End Source File
# Begin Source File

SOURCE=.\res\nodeicons.bmp
# End Source File
# Begin Source File

SOURCE=.\res\nodeview.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ObjectLight.dtx
# End Source File
# Begin Source File

SOURCE=.\res\optionsrun.ico
# End Source File
# Begin Source File

SOURCE=.\res\OutsideDef.dtx
# End Source File
# Begin Source File

SOURCE=.\res\paste.ico
# End Source File
# Begin Source File

SOURCE=.\res\physics.ico
# End Source File
# Begin Source File

SOURCE=.\res\saveicon.ico
# End Source File
# Begin Source File

SOURCE=.\res\SkyPointer.dtx
# End Source File
# Begin Source File

SOURCE=.\Splsh16.bmp
# End Source File
# Begin Source File

SOURCE=.\res\sprite.ico
# End Source File
# Begin Source File

SOURCE=.\res\Terrain.dtx
# End Source File
# Begin Source File

SOURCE=.\res\texture.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\undo.ico
# End Source File
# Begin Source File

SOURCE=.\res\viewport.ico
# End Source File
# Begin Source File

SOURCE=.\res\world.ico
# End Source File
# Begin Source File

SOURCE=.\res\worldedi.bmp
# End Source File
# End Group
# End Target
# End Project
# Section DEdit : {ADF7CBC1-C7CF-11D0-99E3-0060970987C3}
# 	2:10:mlistctl.h:mlistctl.h
# 	2:12:mlistctl.cpp:mlistctl.cpp
# 	2:18:CLASS: CMyListCtrl:CMyListCtrl
# 	2:19:Application Include:dedit.h
# End Section
# Section DEdit : {B6805000-A509-11CE-A5B0-00AA006BBF16}
# 	1:24:CG_IDR_POPUP_REGION_VIEW:120
# 	1:24:CG_IDR_POPUP_SCHEMA_VIEW:178
# 	1:21:CG_IDR_POPUP_CODE_DLG:161
# 	1:23:CG_IDR_POPUP_SPRITE_DLG:152
# 	1:28:CG_IDR_POPUP_WORLD_NODE_VIEW:175
# 	1:23:CG_IDR_POPUP_SOUNDS_DLG:124
# 	1:23:CG_IDR_POPUP_WORLDS_DLG:109
# 	1:25:CG_IDR_POPUP_TEXTURES_DLG:113
# 	1:23:CG_IDR_POPUP_DEDIT_VIEW:123
# 	1:23:CG_IDR_POPUP_MODELS_DLG:125
# End Section
# Section DEdit : {9538483F-4C51-11D0-9030-00A024805738}
# 	1:17:CG_IDS_DISK_SPACE:115
# 	1:19:CG_IDS_PHYSICAL_MEM:114
# 	1:25:CG_IDS_DISK_SPACE_UNAVAIL:116
# 	2:10:SysInfoKey:1234
# End Section
