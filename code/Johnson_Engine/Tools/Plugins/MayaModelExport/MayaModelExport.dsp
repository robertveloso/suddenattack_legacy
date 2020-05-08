# Microsoft Developer Studio Project File - Name="MayaModelExport" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=MayaModelExport - Win32 Debug Maya40
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MayaModelExport.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MayaModelExport.mak" CFG="MayaModelExport - Win32 Debug Maya40"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MayaModelExport - Win32 Debug Maya30" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MayaModelExport - Win32 Release Maya30" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MayaModelExport - Win32 Debug Maya40" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MayaModelExport - Win32 Release Maya40" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MayaModelExport - Win32 Debug Maya30"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug Maya30"
# PROP BASE Intermediate_Dir "Debug Maya30"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\Built\Debug\tools\Plugins\MayaModelExport30"
# PROP Intermediate_Dir "..\..\..\Built\Debug\tools\Plugins\MayaModelExport30"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "..\..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\SDKFiles\Maya30SDK\include" /I "..\..\..\libs\ltamgr" /I "..\..\..\sdk\inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "NT_PLUGIN" /D "USES_MAYA3_SDK" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Foundation.lib OpenMaya.lib OpenMayaAnim.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libcmtd.lib" /out:"..\..\..\Built\Debug\tools\Plugins\MayaModelExport30\MayaModelExport30.mll" /pdbtype:sept /libpath:"..\SDKFiles\Maya30SDK\lib"
# Begin Custom Build
TargetPath=\proj\TO2\Jupiter\Built\Debug\tools\Plugins\MayaModelExport30\MayaModelExport30.mll
InputPath=\proj\TO2\Jupiter\Built\Debug\tools\Plugins\MayaModelExport30\MayaModelExport30.mll
SOURCE="$(InputPath)"

BuildCmds= \
	echo copy $(TargetPath) ..\..\..\..\development\tools\Plugins \
	copy $(TargetPath) ..\..\..\..\development\tools\Plugins \
	echo copy LithTechModelExportOptions.mel ..\..\..\Built\Debug\tools\Plugins \
	copy LithTechModelExportOptions.mel ..\..\..\Built\Debug\tools\Plugins \
	echo copy ..\..\..\Built\Debug\tools\Plugins\LithTechModelExportOptions.mel ..\..\..\..\development\tools\Plugins \
	copy ..\..\..\Built\Debug\tools\Plugins\LithTechModelExportOptions.mel ..\..\..\..\development\tools\Plugins \
	

"..\..\..\..\development\tools\Plugins\MayaModelExport30.mll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\Built\Debug\tools\Plugins\LithTechModelExportOptions.mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\..\development\tools\Plugins\LithTechModelExportOptions.mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "MayaModelExport - Win32 Release Maya30"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release Maya30"
# PROP BASE Intermediate_Dir "Release Maya30"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\Built\Release\tools\Plugins\MayaModelExport30"
# PROP Intermediate_Dir "..\..\..\Built\Release\tools\Plugins\MayaModelExport30"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\SDKFiles\Maya30SDK\include" /I "..\..\..\libs\ltamgr" /I "..\..\..\sdk\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "NT_PLUGIN" /D "USES_MAYA3_SDK" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Foundation.lib OpenMaya.lib OpenMayaAnim.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib:"libcmt.lib" /out:"..\..\..\Built\Release\tools\Plugins\MayaModelExport30\MayaModelExport30.mll" /libpath:"..\SDKFiles\Maya30SDK\lib"
# Begin Custom Build
TargetPath=\proj\TO2\Jupiter\Built\Release\tools\Plugins\MayaModelExport30\MayaModelExport30.mll
InputPath=\proj\TO2\Jupiter\Built\Release\tools\Plugins\MayaModelExport30\MayaModelExport30.mll
SOURCE="$(InputPath)"

BuildCmds= \
	echo copy $(TargetPath) ..\..\..\..\development\tools\Plugins \
	copy $(TargetPath) ..\..\..\..\development\tools\Plugins \
	echo copy LithTechModelExportOptions.mel ..\..\..\Built\Release\tools\Plugins \
	copy LithTechModelExportOptions.mel ..\..\..\Built\Release\tools\Plugins \
	echo copy ..\..\..\Built\Release\tools\Plugins\LithTechModelExportOptions.mel ..\..\..\..\development\tools\Plugins \
	copy ..\..\..\Built\Release\tools\Plugins\LithTechModelExportOptions.mel ..\..\..\..\development\tools\Plugins \
	

"..\..\..\..\development\tools\Plugins\MayaModelExport30.mll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\Built\Release\tools\Plugins\LithTechModelExportOptions.mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\..\development\tools\Plugins\LithTechModelExportOptions.mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "MayaModelExport - Win32 Debug Maya40"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "MayaModelExport___Win32_Maya40_Debug"
# PROP BASE Intermediate_Dir "MayaModelExport___Win32_Maya40_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\Built\Debug\tools\Plugins\MayaModelExport40"
# PROP Intermediate_Dir "..\..\..\Built\Debug\tools\Plugins\MayaModelExport40"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "..\SDKFiles\Maya30SDK\include" /I "..\..\..\libs\ltamgr" /I "..\..\..\sdk\inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "NT_PLUGIN" /D "USES_MAYA3_SDK" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "..\..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\SDKFiles\Maya40SDK\include" /I "..\..\..\libs\ltamgr" /I "..\..\..\sdk\inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "NT_PLUGIN" /D "USES_MAYA4_SDK" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"..\..\..\Built\Debug\tools\Plugins\MayaModelExport30/MayaModelExport.bsc"
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Foundation.lib OpenMaya.lib OpenMayaAnim.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libcmtd.lib" /out:"..\..\..\Built\Debug\tools\Plugins\MayaModelExport30.mll" /pdbtype:sept /libpath:"..\SDKFiles\Maya30SDK\lib"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Foundation.lib OpenMaya.lib OpenMayaAnim.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libcmtd.lib" /out:"..\..\..\Built\Debug\tools\Plugins\MayaModelExport40\MayaModelExport40.mll" /pdbtype:sept /libpath:"..\SDKFiles\Maya40SDK\lib"
# Begin Custom Build
TargetPath=\proj\TO2\Jupiter\Built\Debug\tools\Plugins\MayaModelExport40\MayaModelExport40.mll
InputPath=\proj\TO2\Jupiter\Built\Debug\tools\Plugins\MayaModelExport40\MayaModelExport40.mll
SOURCE="$(InputPath)"

BuildCmds= \
	echo copy $(TargetPath) ..\..\..\..\development\tools\Plugins \
	copy $(TargetPath) ..\..\..\..\development\tools\Plugins \
	echo copy LithTechModelExportOptions.mel ..\..\..\Built\Debug\tools\Plugins \
	copy LithTechModelExportOptions.mel ..\..\..\Built\Debug\tools\Plugins \
	echo copy ..\..\..\Built\Debug\tools\Plugins\LithTechModelExportOptions.mel ..\..\..\..\development\tools\Plugins \
	copy ..\..\..\Built\Debug\tools\Plugins\LithTechModelExportOptions.mel ..\..\..\..\development\tools\Plugins \
	

"..\..\..\..\development\tools\Plugins\MayaModelExport40.mll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\Built\Debug\tools\Plugins\LithTechModelExportOptions.mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\..\development\tools\Plugins\LithTechModelExportOptions.mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "MayaModelExport - Win32 Release Maya40"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MayaModelExport___Win32_Maya40_Release"
# PROP BASE Intermediate_Dir "MayaModelExport___Win32_Maya40_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\Built\Release\tools\Plugins\MayaModelExport40"
# PROP Intermediate_Dir "..\..\..\Built\Release\tools\Plugins\MayaModelExport40"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\SDKFiles\Maya30SDK\include" /I "..\..\..\libs\ltamgr" /I "..\..\..\sdk\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "NT_PLUGIN" /D "USES_MAYA3_SDK" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\SDKFiles\Maya40SDK\include" /I "..\..\..\libs\ltamgr" /I "..\..\..\sdk\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "NT_PLUGIN" /D "USES_MAYA4_SDK" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"..\..\..\Built\Release\tools\Plugins\MayaModelExport30/MayaModelExport.bsc"
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Foundation.lib OpenMaya.lib OpenMayaAnim.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib:"libcmt.lib" /out:"..\..\..\Built\Release\tools\Plugins\MayaModelExport30.mll" /libpath:"..\SDKFiles\Maya30SDK\lib"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Foundation.lib OpenMaya.lib OpenMayaAnim.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib:"libcmt.lib" /out:"..\..\..\Built\Release\tools\Plugins\MayaModelExport40\MayaModelExport40.mll" /libpath:"..\SDKFiles\Maya40SDK\lib"
# Begin Custom Build
TargetPath=\proj\TO2\Jupiter\Built\Release\tools\Plugins\MayaModelExport40\MayaModelExport40.mll
InputPath=\proj\TO2\Jupiter\Built\Release\tools\Plugins\MayaModelExport40\MayaModelExport40.mll
SOURCE="$(InputPath)"

BuildCmds= \
	echo copy $(TargetPath) ..\..\..\..\development\tools\Plugins \
	copy $(TargetPath) ..\..\..\..\development\tools\Plugins \
	echo copy LithTechModelExportOptions.mel ..\..\..\Built\Release\tools\Plugins \
	copy LithTechModelExportOptions.mel ..\..\..\Built\Release\tools\Plugins \
	echo copy ..\..\..\Built\Release\tools\Plugins\LithTechModelExportOptions.mel ..\..\..\..\development\tools\Plugins \
	copy ..\..\..\Built\Release\tools\Plugins\LithTechModelExportOptions.mel ..\..\..\..\development\tools\Plugins \
	

"..\..\..\..\development\tools\Plugins\MayaModelExport40.mll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\Built\Release\tools\Plugins\LithTechModelExportOptions.mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\..\development\tools\Plugins\LithTechModelExportOptions.mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# Begin Target

# Name "MayaModelExport - Win32 Debug Maya30"
# Name "MayaModelExport - Win32 Release Maya30"
# Name "MayaModelExport - Win32 Debug Maya40"
# Name "MayaModelExport - Win32 Release Maya40"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\DllMain.cpp
# End Source File
# Begin Source File

SOURCE=.\ModelExportCommand.cpp
# End Source File
# Begin Source File

SOURCE=.\ModelFileTranslator.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ModelExportCommand.h
# End Source File
# Begin Source File

SOURCE=.\ModelFileTranslator.h
# End Source File
# End Group
# Begin Group "Common Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\sdk\inc\ltquatbase.cpp
# End Source File
# End Group
# Begin Group "libs_debug"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\built\Debug\libs\LTAMgr\ltamgr.lib

!IF  "$(CFG)" == "MayaModelExport - Win32 Debug Maya30"

!ELSEIF  "$(CFG)" == "MayaModelExport - Win32 Release Maya30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MayaModelExport - Win32 Debug Maya40"

!ELSEIF  "$(CFG)" == "MayaModelExport - Win32 Release Maya40"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "libs_release"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\built\Release\libs\LTAMgr\ltamgr.lib

!IF  "$(CFG)" == "MayaModelExport - Win32 Debug Maya30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MayaModelExport - Win32 Release Maya30"

!ELSEIF  "$(CFG)" == "MayaModelExport - Win32 Debug Maya40"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MayaModelExport - Win32 Release Maya40"

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\LithTechModelExportOptions.mel
# End Source File
# End Target
# End Project
