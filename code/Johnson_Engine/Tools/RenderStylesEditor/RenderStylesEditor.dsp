# Microsoft Developer Studio Project File - Name="RenderStylesEditor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=RenderStylesEditor - Win32 Debug Viewer
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "RenderStylesEditor.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "RenderStylesEditor.mak" CFG="RenderStylesEditor - Win32 Debug Viewer"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "RenderStylesEditor - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "RenderStylesEditor - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "RenderStylesEditor - Win32 Debug Viewer" (based on "Win32 (x86) Application")
!MESSAGE "RenderStylesEditor - Win32 Release Viewer" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "RenderStylesEditor - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\built\Release\tools\RenderStylesEditor"
# PROP Intermediate_Dir "..\..\built\Release\tools\RenderStylesEditor"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\..\sdk\inc\physics" /I "..\..\sdk\inc" /I "..\..\..\libs\stdlith" /I "..\..\libs\ltamgr" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "_RS_EDITOR" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL" /d "_RS_EDITOR"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 d3dx8.lib d3d8.lib d3dxof.lib dxguid.lib ddraw.lib winmm.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libcmt" /nodefaultlib:"libcmtd" /libpath:"..\..\lib\release\\"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build
TargetPath=\Working\Engine\built\Release\tools\RenderStylesEditor\RenderStylesEditor.exe
InputPath=\Working\Engine\built\Release\tools\RenderStylesEditor\RenderStylesEditor.exe
SOURCE="$(InputPath)"

"..\..\..\development\tools\RenderStylesEditor.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\development\tools 
	copy $(TargetPath) ..\..\..\development\tools 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "RenderStylesEditor - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\built\debug\tools\RenderStylesEditor"
# PROP Intermediate_Dir "..\..\built\debug\tools\RenderStylesEditor"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\..\sdk\inc\physics" /I "..\..\sdk\inc" /I "..\..\..\libs\stdlith" /I "..\..\libs\ltamgr" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "_RS_EDITOR" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL" /d "_RS_EDITOR"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 d3dx8dt.lib d3d8.lib d3dxof.lib dxguid.lib ddraw.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcmtd" /pdbtype:sept /libpath:"..\..\lib\debug\\"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build
TargetPath=\Working\Engine\built\debug\tools\RenderStylesEditor\RenderStylesEditor.exe
InputPath=\Working\Engine\built\debug\tools\RenderStylesEditor\RenderStylesEditor.exe
SOURCE="$(InputPath)"

"..\..\..\development\tools\RenderStylesEditor.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\development\tools 
	copy $(TargetPath) ..\..\..\development\tools 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "RenderStylesEditor - Win32 Debug Viewer"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "RenderStylesEditor___Win32_Viewer_Debug"
# PROP BASE Intermediate_Dir "RenderStylesEditor___Win32_Viewer_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\built\Debug\tools\RenderStylesEditorViewer"
# PROP Intermediate_Dir "..\..\built\Debug\tools\RenderStylesEditorViewer"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\lib\inc\\" /I "..\..\..\..\engine\sdk\inc\physics" /I "..\..\..\..\engine\sdk\inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\..\sdk\inc\physics" /I "..\..\sdk\inc" /I "..\..\..\libs\stdlith" /I "..\..\libs\ltamgr" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "_RS_VIEWER" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL" /d "_RS_VIEWER"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"..\..\tmp\debug\RenderStylesViewer/RenderStylesViewer.bsc"
LINK32=link.exe
# ADD BASE LINK32 d3dx8dt.lib d3d8.lib d3dxof.lib dxguid.lib ddraw.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcmtd" /out:"..\..\bin\debug\RenderStylesEditor.exe" /pdbtype:sept /libpath:"..\..\lib\debug\\"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 d3dx8dt.lib d3d8.lib d3dxof.lib dxguid.lib ddraw.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcmtd" /out:"..\..\built\Debug\tools\RenderStylesEditorViewer\RenderStylesViewer.exe" /pdbtype:sept /libpath:"..\..\lib\debug\\"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build
TargetPath=\Working\Engine\built\Debug\tools\RenderStylesEditorViewer\RenderStylesViewer.exe
InputPath=\Working\Engine\built\Debug\tools\RenderStylesEditorViewer\RenderStylesViewer.exe
SOURCE="$(InputPath)"

"..\..\..\development\tools\RenderStylesEditor.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\development\tools 
	copy $(TargetPath) ..\..\..\development\tools 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "RenderStylesEditor - Win32 Release Viewer"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "RenderStylesEditor___Win32_Viewer_Release"
# PROP BASE Intermediate_Dir "RenderStylesEditor___Win32_Viewer_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\built\Release\tools\RenderStylesEditorViewer"
# PROP Intermediate_Dir "..\..\built\Release\tools\RenderStylesEditorViewer"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\..\lib\inc\\" /I "..\..\..\..\engine\sdk\inc\physics" /I "..\..\..\..\engine\sdk\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\..\sdk\inc\physics" /I "..\..\sdk\inc" /I "..\..\..\libs\stdlith" /I "..\..\libs\ltamgr" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "_RS_VIEWER" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL" /d "_RS_VIEWER"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"..\..\tmp\release\RenderStylesViewer/RenderStylesViewer.bsc"
LINK32=link.exe
# ADD BASE LINK32 d3dx8.lib d3d8.lib d3dxof.lib dxguid.lib ddraw.lib winmm.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libcmt" /nodefaultlib:"libcmtd" /out:"..\..\bin\release\RenderStylesEditor.exe" /libpath:"..\..\lib\release\\"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 d3dx8.lib d3d8.lib d3dxof.lib dxguid.lib ddraw.lib winmm.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libcmt" /nodefaultlib:"libcmtd" /out:"..\..\built\Release\tools\RenderStylesEditorViewer\RenderStylesViewer.exe" /libpath:"..\..\lib\release\\"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build
TargetPath=\Working\Engine\built\Release\tools\RenderStylesEditorViewer\RenderStylesViewer.exe
InputPath=\Working\Engine\built\Release\tools\RenderStylesEditorViewer\RenderStylesViewer.exe
SOURCE="$(InputPath)"

"..\..\..\development\tools\RenderStylesEditor.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\development\tools 
	copy $(TargetPath) ..\..\..\development\tools 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "RenderStylesEditor - Win32 Release"
# Name "RenderStylesEditor - Win32 Debug"
# Name "RenderStylesEditor - Win32 Debug Viewer"
# Name "RenderStylesEditor - Win32 Release Viewer"
# Begin Group "Source"

# PROP Default_Filter ".cpp"
# Begin Source File

SOURCE=.\d3d_device.cpp
# End Source File
# Begin Source File

SOURCE=.\d3d_renderstatemgr.cpp
# End Source File
# Begin Source File

SOURCE=.\d3d_renderstyle.cpp
# End Source File
# Begin Source File

SOURCE=.\d3d_shell.cpp
# End Source File
# Begin Source File

SOURCE=.\d3dmeshrendobj_rigid.cpp
# End Source File
# Begin Source File

SOURCE=.\d3dmeshrendobj_skel.cpp
# End Source File
# Begin Source File

SOURCE=.\dtx_files.cpp
# End Source File
# Begin Source File

SOURCE=.\MatrixConfigDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RenderPassDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RenderStyle_LTA.cpp
# End Source File
# Begin Source File

SOURCE=".\RenderStylesEditor.cpp"
# End Source File
# Begin Source File

SOURCE=.\Utilities.cpp
# End Source File
# Begin Source File

SOURCE=.\vertexbuffercontroller.cpp
# End Source File
# Begin Source File

SOURCE=.\VertexShaderDlg.cpp
# End Source File
# End Group
# Begin Group "Headers"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=.\d3d_device.h
# End Source File
# Begin Source File

SOURCE=.\d3d_renderstatemgr.h
# End Source File
# Begin Source File

SOURCE=.\d3d_renderstyle.h
# End Source File
# Begin Source File

SOURCE=.\d3d_shell.h
# End Source File
# Begin Source File

SOURCE=.\d3d_vertstructs.h
# End Source File
# Begin Source File

SOURCE=.\d3dmeshrendobj_rigid.h
# End Source File
# Begin Source File

SOURCE=.\d3dmeshrendobj_skel.h
# End Source File
# Begin Source File

SOURCE=.\dtx_files.h
# End Source File
# Begin Source File

SOURCE=.\genrenobj_model.h
# End Source File
# Begin Source File

SOURCE=.\ltb.h
# End Source File
# Begin Source File

SOURCE=.\MatrixConfigDlg.h
# End Source File
# Begin Source File

SOURCE=.\renderobject.h
# End Source File
# Begin Source File

SOURCE=.\RenderPassDlg.h
# End Source File
# Begin Source File

SOURCE=.\renderstyle.h
# End Source File
# Begin Source File

SOURCE=".\RenderStylesEditor.h"
# End Source File
# Begin Source File

SOURCE=.\Utilities.h
# End Source File
# Begin Source File

SOURCE=.\vertexbuffercontroller.h
# End Source File
# Begin Source File

SOURCE=.\VertexShaderDlg.h
# End Source File
# End Group
# Begin Group "Resources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\RenderStylesEditor.ico"
# End Source File
# Begin Source File

SOURCE=".\RenderStylesEditor.rc"
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# End Group
# Begin Group "libs_release"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\built\Release\libs\LTAMgr\ltamgr.lib

!IF  "$(CFG)" == "RenderStylesEditor - Win32 Release"

!ELSEIF  "$(CFG)" == "RenderStylesEditor - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "RenderStylesEditor - Win32 Debug Viewer"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "RenderStylesEditor - Win32 Release Viewer"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Libs\built\Release\Lib_StdLith\Lib_StdLith.lib

!IF  "$(CFG)" == "RenderStylesEditor - Win32 Release"

!ELSEIF  "$(CFG)" == "RenderStylesEditor - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "RenderStylesEditor - Win32 Debug Viewer"

!ELSEIF  "$(CFG)" == "RenderStylesEditor - Win32 Release Viewer"

!ENDIF 

# End Source File
# End Group
# Begin Group "libs_debug"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\built\Debug\libs\LTAMgr\ltamgr.lib

!IF  "$(CFG)" == "RenderStylesEditor - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "RenderStylesEditor - Win32 Debug"

!ELSEIF  "$(CFG)" == "RenderStylesEditor - Win32 Debug Viewer"

!ELSEIF  "$(CFG)" == "RenderStylesEditor - Win32 Release Viewer"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Libs\built\Debug\Lib_StdLith\Lib_StdLith.lib

!IF  "$(CFG)" == "RenderStylesEditor - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "RenderStylesEditor - Win32 Debug"

!ELSEIF  "$(CFG)" == "RenderStylesEditor - Win32 Debug Viewer"

!ELSEIF  "$(CFG)" == "RenderStylesEditor - Win32 Release Viewer"

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\anteyes.ro
# End Source File
# Begin Source File

SOURCE=.\antheadnoeyes.ro
# End Source File
# Begin Source File

SOURCE=.\box.ro
# End Source File
# Begin Source File

SOURCE=.\box_with_basisvectors.ro
# End Source File
# Begin Source File

SOURCE=.\cylinder.ro
# End Source File
# Begin Source File

SOURCE=.\cylinder_with_basisvectors.ro
# End Source File
# Begin Source File

SOURCE=.\helpview.bmp
# End Source File
# Begin Source File

SOURCE=.\highpolyshere.ro
# End Source File
# Begin Source File

SOURCE=.\highpolyshere_with_basisvectors.ro
# End Source File
# Begin Source File

SOURCE=.\idr_main.ico
# End Source File
# Begin Source File

SOURCE=.\lowpolyshere.ro
# End Source File
# Begin Source File

SOURCE=.\lowpolyshere_with_basisvectors.ro
# End Source File
# Begin Source File

SOURCE=.\rs_viewer.ico
# End Source File
# Begin Source File

SOURCE=.\skinnedthing.ro
# End Source File
# Begin Source File

SOURCE=.\skinnedthing_with_basisvectors.ro
# End Source File
# Begin Source File

SOURCE=.\teapot.ro
# End Source File
# Begin Source File

SOURCE=.\viewport.bmp
# End Source File
# Begin Source File

SOURCE=.\wall.ro
# End Source File
# End Target
# End Project
