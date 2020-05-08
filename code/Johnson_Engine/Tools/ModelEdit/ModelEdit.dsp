# Microsoft Developer Studio Project File - Name="ModelEdit" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ModelEdit - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ModelEdit.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ModelEdit.mak" CFG="ModelEdit - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ModelEdit - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ModelEdit - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ModelEdit - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\built\release\tools\ModelEdit"
# PROP Intermediate_Dir "..\..\built\release\tools\ModelEdit"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /I ".\\" /I "..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\shared\engine" /I "..\shared\model\\" /I "..\..\sdk\inc" /I "..\Model_Packer\lta2ltb_d3d" /I "..\..\..\libs\stdlith" /I "..\..\..\libs\regmgr" /I "..\..\libs\ltamgr" /I "..\..\..\libs\lith" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "MODEL_STANDALONE" /D "BDEFS_SIM" /D "LT15_COMPAT" /D "NO_PRAGMA_LIBS" /D "BDEFS_WINDOWS_H" /D "LTA2LTB_D3D" /D "_AFXDLL" /FR /Yu"precompile.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib d3d8.lib d3dx8.lib /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build
TargetPath=\Working\Engine\built\release\tools\ModelEdit\ModelEdit.exe
InputPath=\Working\Engine\built\release\tools\ModelEdit\ModelEdit.exe
SOURCE="$(InputPath)"

"..\..\..\development\tools\ModelEdit.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\development\tools 
	copy $(TargetPath) ..\..\..\development\tools 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "ModelEdit - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\built\Debug\tools\ModelEdit"
# PROP Intermediate_Dir "..\..\built\Debug\tools\ModelEdit"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\\" /I "..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\shared\engine" /I "..\shared\model\\" /I "..\..\sdk\inc" /I "..\Model_Packer\lta2ltb_d3d" /I "..\..\..\libs\stdlith" /I "..\..\..\libs\regmgr" /I "..\..\libs\ltamgr" /I "..\..\..\libs\lith" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "MODEL_STANDALONE" /D "BDEFS_SIM" /D "LT15_COMPAT" /D "NO_PRAGMA_LIBS" /D "BDEFS_WINDOWS_H" /D "_AFXDLL" /FR /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib d3d8.lib d3dx8.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build
TargetPath=\Working\Engine\built\Debug\tools\ModelEdit\ModelEdit.exe
InputPath=\Working\Engine\built\Debug\tools\ModelEdit\ModelEdit.exe
SOURCE="$(InputPath)"

"..\..\..\development\tools\ModelEdit.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\development\tools 
	copy $(TargetPath) ..\..\..\development\tools 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "ModelEdit - Win32 Release"
# Name "ModelEdit - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Group "ME_Headers"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\AboutDlg.h
# End Source File
# Begin Source File

SOURCE=.\AddChildModelDlg.h
# End Source File
# Begin Source File

SOURCE=.\AddSocketDlg.h
# End Source File
# Begin Source File

SOURCE=.\AnimFramerateDlg.h
# End Source File
# Begin Source File

SOURCE=.\AnimMergeDlg.h
# End Source File
# Begin Source File

SOURCE=.\animnumberdlg.h
# End Source File
# Begin Source File

SOURCE=.\CommandStringDlg.h
# End Source File
# Begin Source File

SOURCE=.\ContinuousDlg.h
# End Source File
# Begin Source File

SOURCE=.\DimensionsDlg.h
# End Source File
# Begin Source File

SOURCE=.\DirDialog.h
# End Source File
# Begin Source File

SOURCE=.\DragList.h
# End Source File
# Begin Source File

SOURCE=.\EditOBBDlg.h
# End Source File
# Begin Source File

SOURCE=.\ExportD3D_Dlg.h
# End Source File
# Begin Source File

SOURCE=.\GenLodDlg.h
# End Source File
# Begin Source File

SOURCE=.\gl_modelrender.h
# End Source File
# Begin Source File

SOURCE=.\ImportAnimation.h
# End Source File
# Begin Source File

SOURCE=.\ImportLODDlg.h
# End Source File
# Begin Source File

SOURCE=.\ImportStringKeysDlg.h
# End Source File
# Begin Source File

SOURCE=.\InvalidAnimDimsDlg.h
# End Source File
# Begin Source File

SOURCE=.\KeyframeStringDlg.h
# End Source File
# Begin Source File

SOURCE=.\KeyframeTimeDlg.h
# End Source File
# Begin Source File

SOURCE=.\loadltadlg.h
# End Source File
# Begin Source File

SOURCE=.\LODEdit.h
# End Source File
# Begin Source File

SOURCE=.\ModelEdit.h
# End Source File
# Begin Source File

SOURCE=.\ModelEditDlg.h
# End Source File
# Begin Source File

SOURCE=.\ModelInfoDlg.h
# End Source File
# Begin Source File

SOURCE=.\MyEdit.h
# End Source File
# Begin Source File

SOURCE=.\PieceLODGen.h
# End Source File
# Begin Source File

SOURCE=.\PieceLODGenDlg.h
# End Source File
# Begin Source File

SOURCE=.\PieceMaterialDlg.h
# End Source File
# Begin Source File

SOURCE=.\PieceMerge.h
# End Source File
# Begin Source File

SOURCE=.\RenameNodeDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\RotationDlg.h
# End Source File
# Begin Source File

SOURCE=.\SetFOVDlg.h
# End Source File
# Begin Source File

SOURCE=.\SocketEdit.h
# End Source File
# Begin Source File

SOURCE=.\SolidRectWnd.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TimeOffsetDlg.h
# End Source File
# Begin Source File

SOURCE=.\TranslationDlg.h
# End Source File
# Begin Source File

SOURCE=.\UVImportDlg.h
# End Source File
# Begin Source File

SOURCE=.\WeightEditDlg.h
# End Source File
# Begin Source File

SOURCE=.\WeightSetSelectDlg.h
# End Source File
# Begin Source File

SOURCE=.\win_idle.h
# End Source File
# End Group
# Begin Group "ME dialogs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AboutDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AddSocketDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AnimFramerateDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AnimMergeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\animnumberdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CommandStringDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ContinuousDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DimensionsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DirDialog.cpp

!IF  "$(CFG)" == "ModelEdit - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ModelEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DragList.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportD3D_Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\GenLodDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ImportLODDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ImportStringKeysDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\InvalidAnimDimsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\KeyframeStringDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\KeyframeTimeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\loadltadlg.cpp

!IF  "$(CFG)" == "ModelEdit - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ModelEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ModelInfoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PieceLODGen.cpp
# End Source File
# Begin Source File

SOURCE=.\PieceLODGenDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PieceMaterialDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RenameDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RenameNodeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RotationDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StringDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TimeOffsetDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TranslationDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\UVImportDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\WeightEditDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\WeightSetSelectDlg.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\AddChildModelDlg.cpp

!IF  "$(CFG)" == "ModelEdit - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ModelEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\gl_modelrender.cpp

!IF  "$(CFG)" == "ModelEdit - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ModelEdit - Win32 Debug"

# ADD CPP /Yu"precompile.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ImportAnimation.cpp
# End Source File
# Begin Source File

SOURCE=.\KeyframeWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\LODEdit.cpp
# End Source File
# Begin Source File

SOURCE=..\..\sdk\inc\ltquatbase.cpp

!IF  "$(CFG)" == "ModelEdit - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ModelEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ModelEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\ModelEdit.rc
# End Source File
# Begin Source File

SOURCE=.\ModelEditDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MyEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\newgenlod.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\PieceMerge.cpp
# End Source File
# Begin Source File

SOURCE=.\RenderWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\RenderWnd.h
# End Source File
# Begin Source File

SOURCE=.\SetFOVDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SocketEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\win_idle.cpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap4.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00002.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00003.bmp
# End Source File
# Begin Source File

SOURCE=.\res\delete.bmp
# End Source File
# Begin Source File

SOURCE=.\res\deleteu.bmp
# End Source File
# Begin Source File

SOURCE=.\res\fplayd.bmp
# End Source File
# Begin Source File

SOURCE=.\res\fplayu.bmp
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\ModelEdit.ico
# End Source File
# Begin Source File

SOURCE=.\res\ModelEdit.rc2
# End Source File
# Begin Source File

SOURCE=.\res\movedown.bmp
# End Source File
# Begin Source File

SOURCE=.\res\moveup.bmp
# End Source File
# Begin Source File

SOURCE=.\res\moveupd.bmp
# End Source File
# Begin Source File

SOURCE=.\res\nextkeyd.bmp
# End Source File
# Begin Source File

SOURCE=.\res\nextkeyu.bmp
# End Source File
# Begin Source File

SOURCE=.\res\numberd.bmp
# End Source File
# Begin Source File

SOURCE=.\res\numberu.bmp
# End Source File
# Begin Source File

SOURCE=.\res\prevkeyd.bmp
# End Source File
# Begin Source File

SOURCE=.\res\prevkeyu.bmp
# End Source File
# Begin Source File

SOURCE=.\res\rplayd.bmp
# End Source File
# Begin Source File

SOURCE=.\res\rplayu.bmp
# End Source File
# Begin Source File

SOURCE=.\SolidRectWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\res\stopd.bmp
# End Source File
# Begin Source File

SOURCE=.\res\stopu.bmp
# End Source File
# End Group
# Begin Group "Src From Engine"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=..\shared\engine\bdefs.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\shared\engine\conparse.cpp

!IF  "$(CFG)" == "ModelEdit - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ModelEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\shared\engine\counter.cpp

!IF  "$(CFG)" == "ModelEdit - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ModelEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\shared\engine\dtxmgr.cpp

!IF  "$(CFG)" == "ModelEdit - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ModelEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\EditOBBDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\engine\genltstream.cpp

!IF  "$(CFG)" == "ModelEdit - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ModelEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\shared\engine\geomroutines.cpp

!IF  "$(CFG)" == "ModelEdit - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ModelEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\sdk\inc\LTEulerAngles.cpp

!IF  "$(CFG)" == "ModelEdit - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ModelEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\sdk\inc\LTEulerAngles.h
# End Source File
# Begin Source File

SOURCE=..\shared\engine\nexus.cpp

!IF  "$(CFG)" == "ModelEdit - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ModelEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\shared\engine\pixelformat.cpp

!IF  "$(CFG)" == "ModelEdit - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ModelEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\shared\engine\simple_dalloc.cpp

!IF  "$(CFG)" == "ModelEdit - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ModelEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\shared\engine\streamsim.cpp

!IF  "$(CFG)" == "ModelEdit - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ModelEdit - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "libs_debug"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\built\Debug\libs\ltamgr\ltamgr.lib

!IF  "$(CFG)" == "ModelEdit - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ModelEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\libs\built\Debug\RegMgr\RegMgr.lib

!IF  "$(CFG)" == "ModelEdit - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ModelEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\libs\built\Debug\Lib_Lith\Lib_Lith.lib

!IF  "$(CFG)" == "ModelEdit - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ModelEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Libs\built\Debug\Lib_StdLith\Lib_StdLith.lib

!IF  "$(CFG)" == "ModelEdit - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ModelEdit - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "libs_release"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\built\Release\libs\ltamgr\ltamgr.lib

!IF  "$(CFG)" == "ModelEdit - Win32 Release"

!ELSEIF  "$(CFG)" == "ModelEdit - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\libs\built\Release\RegMgr\RegMgr.lib

!IF  "$(CFG)" == "ModelEdit - Win32 Release"

!ELSEIF  "$(CFG)" == "ModelEdit - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\libs\built\Release\Lib_Lith\Lib_Lith.lib

!IF  "$(CFG)" == "ModelEdit - Win32 Release"

!ELSEIF  "$(CFG)" == "ModelEdit - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Libs\built\Release\Lib_StdLith\Lib_StdLith.lib

!IF  "$(CFG)" == "ModelEdit - Win32 Release"

!ELSEIF  "$(CFG)" == "ModelEdit - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "model"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\shared\model\animtracker.cpp

!IF  "$(CFG)" == "ModelEdit - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ModelEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\shared\model\animtracker.h
# End Source File
# Begin Source File

SOURCE=..\shared\model\debug.cpp

!IF  "$(CFG)" == "ModelEdit - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ModelEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\shared\model\debug.h
# End Source File
# Begin Source File

SOURCE=..\shared\model\estring.cpp

!IF  "$(CFG)" == "ModelEdit - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ModelEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\shared\model\estring.h
# End Source File
# Begin Source File

SOURCE=..\shared\model\exportsys.cpp

!IF  "$(CFG)" == "ModelEdit - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ModelEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\shared\model\exportsys.h
# End Source File
# Begin Source File

SOURCE=..\shared\model\ltaModel.cpp

!IF  "$(CFG)" == "ModelEdit - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ModelEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\shared\model\ltaModel.h
# End Source File
# Begin Source File

SOURCE=..\shared\model\ltaSave.cpp

!IF  "$(CFG)" == "ModelEdit - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ModelEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\shared\model\ltaSave.h
# End Source File
# Begin Source File

SOURCE=..\shared\model\ltaScene.cpp

!IF  "$(CFG)" == "ModelEdit - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ModelEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\shared\model\ltaScene.h
# End Source File
# Begin Source File

SOURCE=..\shared\model\ltb.h
# End Source File
# Begin Source File

SOURCE=..\shared\model\model.cpp

!IF  "$(CFG)" == "ModelEdit - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ModelEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\shared\model\model.h
# End Source File
# Begin Source File

SOURCE=..\shared\model\model_cleanup.cpp

!IF  "$(CFG)" == "ModelEdit - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ModelEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\shared\model\model_cleanup.h
# End Source File
# Begin Source File

SOURCE=..\shared\model\model_ops.cpp

!IF  "$(CFG)" == "ModelEdit - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ModelEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\shared\model\model_ops.h
# End Source File
# Begin Source File

SOURCE=..\shared\model\modelallocations.cpp

!IF  "$(CFG)" == "ModelEdit - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ModelEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\shared\model\modelallocations.h
# End Source File
# Begin Source File

SOURCE=..\shared\model\transformmaker.cpp

!IF  "$(CFG)" == "ModelEdit - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ModelEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\shared\model\transformmaker.h
# End Source File
# End Group
# Begin Group "TreeControl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ltwintreeicon.h
# End Source File
# Begin Source File

SOURCE=.\ltwintreeitem.cpp

!IF  "$(CFG)" == "ModelEdit - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ModelEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ltwintreeitem.h
# End Source File
# Begin Source File

SOURCE=.\ltwintreeitemiter.cpp

!IF  "$(CFG)" == "ModelEdit - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ModelEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ltwintreeitemiter.h
# End Source File
# Begin Source File

SOURCE=.\ltwintreekey.h
# End Source File
# Begin Source File

SOURCE=.\ltwintreemgr.cpp

!IF  "$(CFG)" == "ModelEdit - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "ModelEdit - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ltwintreemgr.h
# End Source File
# End Group
# End Target
# End Project
