# Microsoft Developer Studio Project File - Name="MaxModelExport" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=MaxModelExport - Win32 Debug MAX40
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MaxModelExport.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MaxModelExport.mak" CFG="MaxModelExport - Win32 Debug MAX40"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MaxModelExport - Win32 Debug MAX30" (based on "Win32 (x86) Application")
!MESSAGE "MaxModelExport - Win32 Release MAX30" (based on "Win32 (x86) Application")
!MESSAGE "MaxModelExport - Win32 Debug MAX40" (based on "Win32 (x86) Application")
!MESSAGE "MaxModelExport - Win32 Release MAX40" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MaxModelExport - Win32 Debug MAX30"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "MaxModelExport___Win32_MAX30_Debug"
# PROP BASE Intermediate_Dir "MaxModelExport___Win32_MAX30_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\built\Debug\tools\Plugins\MaxModelExport30"
# PROP Intermediate_Dir "..\..\..\built\Debug\tools\Plugins\MaxModelExport30"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "..\..\ltaparse" /I "..\..\..\sdk\inc" /I "..\sdkfiles\max30sdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /Gm /GX /Zi /Od /I "..\..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\sdkfiles\max30sdk\include" /I "..\..\..\sdk\inc" /I "..\..\..\libs\ltamgr" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "USES_MAX3_SDK" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib paramblk2.lib /nologo /base:"0x105b0000" /subsystem:windows /dll /debug /machine:I386 /out:"../../../../proj/Debug/MaxModelExport.dle" /pdbtype:sept /libpath:"..\sdkfiles\max30sdk\lib"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib paramblk2.lib /nologo /base:"0x105b0000" /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libcmtd" /out:"..\..\..\built\Debug\tools\Plugins\MaxModelExport30\MaxModelExport30.dle" /pdbtype:sept /libpath:"..\sdkfiles\max30sdk\lib"
# Begin Custom Build
TargetPath=\Working\Engine\built\Debug\tools\Plugins\MaxModelExport30\MaxModelExport30.dle
InputPath=\Working\Engine\built\Debug\tools\Plugins\MaxModelExport30\MaxModelExport30.dle
SOURCE="$(InputPath)"

"..\..\..\..\development\tools\Plugins\MaxModelExport30.dle" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\..\development\tools\Plugins 
	copy $(TargetPath) ..\..\..\..\development\tools\Plugins 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "MaxModelExport - Win32 Release MAX30"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MaxModelExport___Win32_MAX30_Release"
# PROP BASE Intermediate_Dir "MaxModelExport___Win32_MAX30_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\Built\Release\tools\Plugins\MaxModelExport30"
# PROP Intermediate_Dir "..\..\..\Built\Release\tools\Plugins\MaxModelExport30"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /GX /O2 /I "..\..\ltaparse" /I "..\..\..\sdk\inc" /I "..\sdkfiles\max30sdk\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /O2 /I "..\..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\sdkfiles\max30sdk\include" /I "..\..\..\sdk\inc" /I "..\..\..\libs\ltamgr" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "USES_MAX3_SDK" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib paramblk2.lib /nologo /base:"0x105b0000" /subsystem:windows /dll /machine:I386 /out:"../../../../proj/Release/MaxModelExport.dle" /libpath:"..\sdkfiles\max30sdk\lib" /release
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib paramblk2.lib /nologo /base:"0x105b0000" /subsystem:windows /dll /machine:I386 /nodefaultlib:"libcmt" /out:"..\..\..\Built\Release\tools\Plugins\MaxModelExport30\MaxModelExport30.dle" /libpath:"..\sdkfiles\max30sdk\lib" /release
# Begin Custom Build
TargetPath=\Working\Engine\Built\Release\tools\Plugins\MaxModelExport30\MaxModelExport30.dle
InputPath=\Working\Engine\Built\Release\tools\Plugins\MaxModelExport30\MaxModelExport30.dle
SOURCE="$(InputPath)"

"..\..\..\..\development\tools\Plugins\MaxModelExport30.dle" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\..\development\tools\Plugins 
	copy $(TargetPath) ..\..\..\..\development\tools\Plugins 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "MaxModelExport - Win32 Debug MAX40"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "MaxModelExport___Win32_MAX40_Debug"
# PROP BASE Intermediate_Dir "MaxModelExport___Win32_MAX40_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\Built\Debug\tools\Plugins\MaxModelExport40"
# PROP Intermediate_Dir "..\..\..\Built\Debug\tools\Plugins\MaxModelExport40"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "..\..\..\sdk\inc" /I "..\sdkfiles\max30sdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /Gm /GX /Zi /Od /I "..\..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\sdkfiles\max40sdk\include" /I "..\..\..\sdk\inc" /I "..\..\..\libs\ltamgr" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "USES_MAX4_SDK" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib paramblk2.lib /nologo /base:"0x105b0000" /subsystem:windows /dll /debug /machine:I386 /out:"..\..\..\bin\Debug\tools\Plugins\MaxModelExport30.dle" /pdbtype:sept /libpath:"..\sdkfiles\max30sdk\lib"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib paramblk2.lib /nologo /base:"0x105b0000" /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libcmtd" /out:"..\..\..\Built\Debug\tools\Plugins\MaxModelExport40\MaxModelExport40.dle" /pdbtype:sept /libpath:"..\sdkfiles\max40sdk\lib"
# Begin Custom Build
TargetPath=\Working\Engine\Built\Debug\tools\Plugins\MaxModelExport40\MaxModelExport40.dle
InputPath=\Working\Engine\Built\Debug\tools\Plugins\MaxModelExport40\MaxModelExport40.dle
SOURCE="$(InputPath)"

"..\..\..\..\development\tools\Plugins\MaxModelExport40.dle" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\..\development\tools\Plugins 
	copy $(TargetPath) ..\..\..\..\development\tools\Plugins 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "MaxModelExport - Win32 Release MAX40"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MaxModelExport___Win32_MAX40_Release"
# PROP BASE Intermediate_Dir "MaxModelExport___Win32_MAX40_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\Built\Release\tools\Plugins\MaxModelExport40"
# PROP Intermediate_Dir "..\..\..\Built\Release\tools\Plugins\MaxModelExport40"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /GX /O2 /I "..\..\..\sdk\inc" /I "..\sdkfiles\max30sdk\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /O2 /I "..\..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\sdkfiles\max40sdk\include" /I "..\..\..\sdk\inc" /I "..\..\..\libs\ltamgr" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "USES_MAX4_SDK" /FR /YX /FD /c
# SUBTRACT CPP /X
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib paramblk2.lib /nologo /base:"0x105b0000" /subsystem:windows /dll /machine:I386 /out:"..\..\..\bin\Release\Plugins\MaxModelExport30.dle" /libpath:"..\sdkfiles\max30sdk\lib" /release
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib paramblk2.lib /nologo /base:"0x105b0000" /subsystem:windows /dll /machine:I386 /nodefaultlib:"libcmt" /out:"..\..\..\Built\Release\tools\Plugins\MaxModelExport40\MaxModelExport40.dle" /libpath:"..\sdkfiles\max40sdk\lib" /release
# Begin Custom Build
TargetPath=\Working\Engine\Built\Release\tools\Plugins\MaxModelExport40\MaxModelExport40.dle
InputPath=\Working\Engine\Built\Release\tools\Plugins\MaxModelExport40\MaxModelExport40.dle
SOURCE="$(InputPath)"

"..\..\..\..\development\tools\Plugins\MaxModelExport40.dle" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\..\development\tools\Plugins 
	copy $(TargetPath) ..\..\..\..\development\tools\Plugins 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "MaxModelExport - Win32 Debug MAX30"
# Name "MaxModelExport - Win32 Release MAX30"
# Name "MaxModelExport - Win32 Debug MAX40"
# Name "MaxModelExport - Win32 Release MAX40"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\DLLMain.cpp
# End Source File
# Begin Source File

SOURCE=.\Exporter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\ltquatbase.cpp
# End Source File
# Begin Source File

SOURCE=.\MaxModelExport.cpp
# End Source File
# Begin Source File

SOURCE=.\MaxModelExport30.def

!IF  "$(CFG)" == "MaxModelExport - Win32 Debug MAX30"

!ELSEIF  "$(CFG)" == "MaxModelExport - Win32 Release MAX30"

!ELSEIF  "$(CFG)" == "MaxModelExport - Win32 Debug MAX40"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MaxModelExport - Win32 Release MAX40"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MaxModelExport40.def

!IF  "$(CFG)" == "MaxModelExport - Win32 Debug MAX30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MaxModelExport - Win32 Release MAX30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MaxModelExport - Win32 Debug MAX40"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MaxModelExport - Win32 Release MAX40"

# PROP BASE Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Exporter.h
# End Source File
# Begin Source File

SOURCE=.\MaxModelExport.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Group "MAX NonSDK Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MAXNonSDKHeaders\BIPEXP.H
# End Source File
# Begin Source File

SOURCE=.\MAXNonSDKHeaders\Phyexp.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MaxModelExport.rc
# End Source File
# End Group
# Begin Group "libs_debug"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\built\Debug\libs\LTAMgr\ltamgr.lib

!IF  "$(CFG)" == "MaxModelExport - Win32 Debug MAX30"

!ELSEIF  "$(CFG)" == "MaxModelExport - Win32 Release MAX30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MaxModelExport - Win32 Debug MAX40"

!ELSEIF  "$(CFG)" == "MaxModelExport - Win32 Release MAX40"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "libs_release"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\built\Release\libs\LTAMgr\ltamgr.lib

!IF  "$(CFG)" == "MaxModelExport - Win32 Debug MAX30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MaxModelExport - Win32 Release MAX30"

!ELSEIF  "$(CFG)" == "MaxModelExport - Win32 Debug MAX40"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MaxModelExport - Win32 Release MAX40"

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=..\readme.txt
# End Source File
# End Target
# End Project
