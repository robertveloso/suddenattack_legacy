# Microsoft Developer Studio Project File - Name="HeightMaptoNormalMap" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=HeightMaptoNormalMap - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "HeightMaptoNormalMap.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "HeightMaptoNormalMap.mak" CFG="HeightMaptoNormalMap - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "HeightMaptoNormalMap - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "HeightMaptoNormalMap - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "HeightMaptoNormalMap - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\built\release\tools\HeightMaptoNormalMap"
# PROP Intermediate_Dir "..\..\built\release\tools\HeightMaptoNormalMap"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\..\sdk\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 d3dx8.lib d3d8.lib d3dxof.lib dxguid.lib ddraw.lib winmm.lib /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build
TargetPath=\proj\TO2\Engine\built\release\tools\HeightMaptoNormalMap\HeightMaptoNormalMap.exe
InputPath=\proj\TO2\Engine\built\release\tools\HeightMaptoNormalMap\HeightMaptoNormalMap.exe
SOURCE="$(InputPath)"

"..\..\..\development\tools\HeightMaptoNormalMap.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\development\tools
	copy $(TargetPath) ..\..\..\development\tools

# End Custom Build

!ELSEIF  "$(CFG)" == "HeightMaptoNormalMap - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\built\debug\tools\HeightMaptoNormalMap"
# PROP Intermediate_Dir "..\..\built\debug\tools\HeightMaptoNormalMap"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\..\sdk\inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 d3dx8dt.lib d3d8.lib d3dxof.lib dxguid.lib ddraw.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build
TargetPath=\proj\TO2\Engine\built\debug\tools\HeightMaptoNormalMap\HeightMaptoNormalMap.exe
InputPath=\proj\TO2\Engine\built\debug\tools\HeightMaptoNormalMap\HeightMaptoNormalMap.exe
SOURCE="$(InputPath)"

"..\..\..\development\tools\HeightMaptoNormalMap.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\development\tools
	copy $(TargetPath) ..\..\..\development\tools

# End Custom Build

!ENDIF 

# Begin Target

# Name "HeightMaptoNormalMap - Win32 Release"
# Name "HeightMaptoNormalMap - Win32 Debug"
# Begin Group "Source"

# PROP Default_Filter ".cpp"
# Begin Source File

SOURCE=.\d3d_device.cpp
# End Source File
# Begin Source File

SOURCE=.\d3d_shell.cpp
# End Source File
# Begin Source File

SOURCE=.\HeightMaptoNormalMap.cpp
# End Source File
# Begin Source File

SOURCE=.\UpdateDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Utilities.cpp
# End Source File
# End Group
# Begin Group "Headers"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=.\commandline_parser.h
# End Source File
# Begin Source File

SOURCE=.\d3d_device.h
# End Source File
# Begin Source File

SOURCE=.\d3d_shell.h
# End Source File
# Begin Source File

SOURCE=.\HeightMaptoNormalMap.h
# End Source File
# Begin Source File

SOURCE=.\UpdateDlg.h
# End Source File
# Begin Source File

SOURCE=.\Utilities.h
# End Source File
# End Group
# Begin Group "Resources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\HeightMaptoNormalMap.rc
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
# Begin Source File

SOURCE=.\HeightMaptoNormalMap.ico
# End Source File
# End Target
# End Project
