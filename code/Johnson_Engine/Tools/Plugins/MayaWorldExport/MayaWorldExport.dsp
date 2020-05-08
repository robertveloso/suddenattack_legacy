# Microsoft Developer Studio Project File - Name="MayaWorldExport" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=MayaWorldExport - Win32 Debug Maya40
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MayaWorldExport.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MayaWorldExport.mak" CFG="MayaWorldExport - Win32 Debug Maya40"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MayaWorldExport - Win32 Debug Maya30" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MayaWorldExport - Win32 Release Maya30" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MayaWorldExport - Win32 Debug Maya40" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MayaWorldExport - Win32 Release Maya40" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MayaWorldExport - Win32 Debug Maya30"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug Maya30"
# PROP BASE Intermediate_Dir "Debug Maya30"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\Built\Debug\tools\Plugins\MayaWorldExport30"
# PROP Intermediate_Dir "..\..\..\Built\Debug\tools\Plugins\MayaWorldExport30"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "..\SDKFiles\Maya30SDK\include" /I "..\..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\shared" /I "..\..\..\libs\ltamgr" /I "..\..\..\sdk\inc" /D "USES_MAYA3_SDK" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "NT_PLUGIN" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Foundation.lib OpenMaya.lib OpenMayaAnim.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libcmtd.lib" /out:"..\..\..\Built\Debug\tools\Plugins\MayaWorldExport30\MayaWorldExport30.mll" /pdbtype:sept /libpath:"..\SDKFiles\Maya30SDK\lib"
# Begin Custom Build
TargetPath=\proj\TO2\Engine\Built\Debug\tools\Plugins\MayaWorldExport30\MayaWorldExport30.mll
InputPath=\proj\TO2\Engine\Built\Debug\tools\Plugins\MayaWorldExport30\MayaWorldExport30.mll
SOURCE="$(InputPath)"

BuildCmds= \
	echo copy $(TargetPath) ..\..\..\..\development\tools\Plugins \
	copy $(TargetPath) ..\..\..\..\development\tools\Plugins \
	echo copy LithTechWorldExportOptions.mel ..\..\..\Built\Debug\tools\Plugins \
	copy LithTechWorldExportOptions.mel ..\..\..\Built\Debug\tools\Plugins \
	echo copy LithTechWorldImportOptions.mel ..\..\..\Built\Debug\tools\Plugins \
	copy LithTechWorldImportOptions.mel ..\..\..\Built\Debug\tools\Plugins \
	echo copy ..\..\..\Built\Debug\tools\Plugins\LithTechWorldExportOptions.mel ..\..\..\..\development\tools\Plugins \
	copy ..\..\..\Built\Debug\tools\Plugins\LithTechWorldExportOptions.mel ..\..\..\..\development\tools\Plugins \
	echo copy ..\..\..\Built\Debug\tools\Plugins\LithTechWorldImportOptions.mel ..\..\..\..\development\tools\Plugins \
	copy ..\..\..\Built\Debug\tools\Plugins\LithTechWorldImportOptions.mel ..\..\..\..\development\tools\Plugins \
	

"..\..\..\..\development\tools\Plugins\MayaWorldExport30.mll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\Built\Debug\tools\Plugins\LithTechWorldExportOptions.mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\Built\Debug\tools\Plugins\LithTechWorldImportOptions.mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\..\development\tools\Plugins\LithTechWorldExportOptions.mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\..\development\tools\Plugins\LithTechWorldImportOptions.mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "MayaWorldExport - Win32 Release Maya30"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release Maya30"
# PROP BASE Intermediate_Dir "Release Maya30"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\Built\Release\tools\Plugins\MayaWorldExport30"
# PROP Intermediate_Dir "..\..\..\Built\Release\tools\Plugins\MayaWorldExport30"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\SDKFiles\Maya30SDK\include" /I "..\..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\shared" /I "..\..\..\libs\ltamgr" /I "..\..\..\sdk\inc" /D "USES_MAYA3_SDK" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "NT_PLUGIN" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Foundation.lib OpenMaya.lib OpenMayaAnim.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib:"libcmt.lib" /out:"..\..\..\Built\Release\tools\Plugins\MayaWorldExport30\MayaWorldExport30.mll" /libpath:"..\SDKFiles\Maya30SDK\lib"
# Begin Custom Build
TargetPath=\proj\TO2\Engine\Built\Release\tools\Plugins\MayaWorldExport30\MayaWorldExport30.mll
InputPath=\proj\TO2\Engine\Built\Release\tools\Plugins\MayaWorldExport30\MayaWorldExport30.mll
SOURCE="$(InputPath)"

BuildCmds= \
	echo copy $(TargetPath) ..\..\..\..\development\tools\Plugins \
	copy $(TargetPath) ..\..\..\..\development\tools\Plugins \
	echo copy LithTechWorldExportOptions.mel ..\..\..\Built\Release\tools\Plugins \
	copy LithTechWorldExportOptions.mel ..\..\..\Built\Release\tools\Plugins \
	echo copy LithTechWorldImportOptions.mel ..\..\..\Built\Release\tools\Plugins \
	copy LithTechWorldImportOptions.mel ..\..\..\Built\Release\tools\Plugins \
	echo copy ..\..\..\Built\Release\tools\Plugins\LithTechWorldExportOptions.mel ..\..\..\..\development\tools\Plugins \
	copy ..\..\..\Built\Release\tools\Plugins\LithTechWorldExportOptions.mel ..\..\..\..\development\tools\Plugins \
	echo copy ..\..\..\Built\Release\tools\Plugins\LithTechWorldImportOptions.mel ..\..\..\..\development\tools\Plugins \
	copy ..\..\..\Built\Release\tools\Plugins\LithTechWorldImportOptions.mel ..\..\..\..\development\tools\Plugins \
	

"..\..\..\..\development\tools\Plugins\MayaWorldExport30.mll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\Built\Release\tools\Plugins\LithTechWorldExportOptions.mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\Built\Release\tools\Plugins\LithTechWorldImportOptions.mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\..\development\tools\Plugins\LithTechWorldExportOptions.mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\..\development\tools\Plugins\LithTechWorldImportOptions.mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "MayaWorldExport - Win32 Debug Maya40"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "MayaWorldExport___Win32_Maya40_Debug"
# PROP BASE Intermediate_Dir "MayaWorldExport___Win32_Maya40_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\Built\Debug\tools\Plugins\MayaWorldExport40"
# PROP Intermediate_Dir "..\..\..\Built\Debug\tools\Plugins\MayaWorldExport40"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "..\SDKFiles\Maya30SDK\include" /I "..\..\..\libs\ltamgr" /I "..\..\..\sdk\inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "NT_PLUGIN" /D "USES_MAYA3_SDK" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "..\SDKFiles\Maya40SDK\include" /I "..\..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\shared" /I "..\..\..\libs\ltamgr" /I "..\..\..\sdk\inc" /D "USES_MAYA4_SDK" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "NT_PLUGIN" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Foundation.lib OpenMaya.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libcmtd.lib" /out:"..\..\..\Built\Debug\tools\Plugins\MayaWorldExport30.mll" /pdbtype:sept /libpath:"..\SDKFiles\Maya30SDK\lib"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Foundation.lib OpenMaya.lib OpenMayaAnim.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libcmtd.lib" /out:"..\..\..\Built\Debug\tools\Plugins\MayaWorldExport40\MayaWorldExport40.mll" /pdbtype:sept /libpath:"..\SDKFiles\Maya40SDK\lib"
# Begin Custom Build
TargetPath=\proj\TO2\Engine\Built\Debug\tools\Plugins\MayaWorldExport40\MayaWorldExport40.mll
InputPath=\proj\TO2\Engine\Built\Debug\tools\Plugins\MayaWorldExport40\MayaWorldExport40.mll
SOURCE="$(InputPath)"

BuildCmds= \
	echo copy $(TargetPath) ..\..\..\..\development\tools\Plugins \
	copy $(TargetPath) ..\..\..\..\development\tools\Plugins \
	echo copy LithTechWorldExportOptions.mel ..\..\..\Built\Debug\tools\Plugins \
	copy LithTechWorldExportOptions.mel ..\..\..\Built\Debug\tools\Plugins \
	echo copy LithTechWorldImportOptions.mel ..\..\..\Built\Debug\tools\Plugins \
	copy LithTechWorldImportOptions.mel ..\..\..\Built\Debug\tools\Plugins \
	echo copy ..\..\..\Built\Debug\tools\Plugins\LithTechWorldExportOptions.mel ..\..\..\..\development\tools\Plugins \
	copy ..\..\..\Built\Debug\tools\Plugins\LithTechWorldExportOptions.mel ..\..\..\..\development\tools\Plugins \
	echo copy ..\..\..\Built\Debug\tools\Plugins\LithTechWorldImportOptions.mel ..\..\..\..\development\tools\Plugins \
	copy ..\..\..\Built\Debug\tools\Plugins\LithTechWorldImportOptions.mel ..\..\..\..\development\tools\Plugins \
	

"..\..\..\..\development\tools\Plugins\MayaWorldExport40.mll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\Built\Debug\tools\Plugins\LithTechWorldExportOptions.mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\Built\Debug\tools\Plugins\LithTechWorldImportOptions.mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\..\development\tools\Plugins\LithTechWorldExportOptions.mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\..\development\tools\Plugins\LithTechWorldImportOptions.mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "MayaWorldExport - Win32 Release Maya40"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MayaWorldExport___Win32_Maya40_Release"
# PROP BASE Intermediate_Dir "MayaWorldExport___Win32_Maya40_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\Built\Release\tools\Plugins\MayaWorldExport40"
# PROP Intermediate_Dir "..\..\..\Built\Release\tools\Plugins\MayaWorldExport40"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\SDKFiles\Maya30SDK\include" /I "..\..\..\libs\ltamgr" /I "..\..\..\sdk\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "NT_PLUGIN" /D "USES_MAYA3_SDK" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\SDKFiles\Maya40SDK\include" /I "..\..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\shared" /I "..\..\..\libs\ltamgr" /I "..\..\..\sdk\inc" /D "USES_MAYA4_SDK" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "NT_PLUGIN" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Foundation.lib OpenMaya.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib:"libcmt.lib" /out:"..\..\..\Built\Release\tools\Plugins\MayaWorldExport30.mll" /libpath:"..\SDKFiles\Maya30SDK\lib"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Foundation.lib OpenMaya.lib OpenMayaAnim.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib:"libcmt.lib" /out:"..\..\..\Built\Release\tools\Plugins\MayaWorldExport40\MayaWorldExport40.mll" /libpath:"..\SDKFiles\Maya40SDK\lib"
# Begin Custom Build
TargetPath=\proj\TO2\Engine\Built\Release\tools\Plugins\MayaWorldExport40\MayaWorldExport40.mll
InputPath=\proj\TO2\Engine\Built\Release\tools\Plugins\MayaWorldExport40\MayaWorldExport40.mll
SOURCE="$(InputPath)"

BuildCmds= \
	echo copy $(TargetPath) ..\..\..\..\development\tools\Plugins \
	copy $(TargetPath) ..\..\..\..\development\tools\Plugins \
	echo copy LithTechWorldExportOptions.mel ..\..\..\Built\Release\tools\Plugins \
	copy LithTechWorldExportOptions.mel ..\..\..\Built\Release\tools\Plugins \
	echo copy LithTechWorldImportOptions.mel ..\..\..\Built\Release\tools\Plugins \
	copy LithTechWorldImportOptions.mel ..\..\..\Built\Release\tools\Plugins \
	echo copy ..\..\..\Built\Release\tools\Plugins\LithTechWorldExportOptions.mel ..\..\..\..\development\tools\Plugins \
	copy ..\..\..\Built\Release\tools\Plugins\LithTechWorldExportOptions.mel ..\..\..\..\development\tools\Plugins \
	echo copy ..\..\..\Built\Release\tools\Plugins\LithTechWorldImportOptions.mel ..\..\..\..\development\tools\Plugins \
	copy ..\..\..\Built\Release\tools\Plugins\LithTechWorldImportOptions.mel ..\..\..\..\development\tools\Plugins \
	

"..\..\..\..\development\tools\Plugins\MayaWorldExport40.mll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\Built\Release\tools\Plugins\LithTechWorldExportOptions.mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\Built\Release\tools\Plugins\LithTechWorldImportOptions.mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\..\development\tools\Plugins\LithTechWorldExportOptions.mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\..\..\development\tools\Plugins\LithTechWorldImportOptions.mel" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# Begin Target

# Name "MayaWorldExport - Win32 Debug Maya30"
# Name "MayaWorldExport - Win32 Release Maya30"
# Name "MayaWorldExport - Win32 Debug Maya40"
# Name "MayaWorldExport - Win32 Release Maya40"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\DllMain.cpp
# End Source File
# Begin Source File

SOURCE=.\LTAFile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\LTEulerAngles.cpp
# End Source File
# Begin Source File

SOURCE=.\WorldExportCommand.cpp
# End Source File
# Begin Source File

SOURCE=.\WorldExportFileTranslator.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\WorldImport.cpp
# End Source File
# Begin Source File

SOURCE=.\WorldImportCommand.cpp
# End Source File
# Begin Source File

SOURCE=.\WorldImportFileTranslator.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\LTAFile.h
# End Source File
# Begin Source File

SOURCE=.\WorldExportCommand.h
# End Source File
# Begin Source File

SOURCE=.\WorldExportFileTranslator.h
# End Source File
# Begin Source File

SOURCE=..\Shared\WorldImport.h
# End Source File
# Begin Source File

SOURCE=.\WorldImportCommand.h
# End Source File
# Begin Source File

SOURCE=.\WorldImportFileTranslator.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "libs_debug"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\built\Debug\libs\LTAMgr\ltamgr.lib

!IF  "$(CFG)" == "MayaWorldExport - Win32 Debug Maya30"

!ELSEIF  "$(CFG)" == "MayaWorldExport - Win32 Release Maya30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MayaWorldExport - Win32 Debug Maya40"

!ELSEIF  "$(CFG)" == "MayaWorldExport - Win32 Release Maya40"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "libs_release"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\built\Release\libs\LTAMgr\ltamgr.lib

!IF  "$(CFG)" == "MayaWorldExport - Win32 Debug Maya30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MayaWorldExport - Win32 Release Maya30"

!ELSEIF  "$(CFG)" == "MayaWorldExport - Win32 Debug Maya40"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MayaWorldExport - Win32 Release Maya40"

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\LithTechWorldExportOptions.mel
# End Source File
# Begin Source File

SOURCE=.\LithTechWorldImportOptions.mel
# End Source File
# End Target
# End Project
