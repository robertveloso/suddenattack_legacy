# Microsoft Developer Studio Project File - Name="MaxWorldExport" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=MaxWorldExport - Win32 Debug MAX40
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MaxWorldExport.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MaxWorldExport.mak" CFG="MaxWorldExport - Win32 Debug MAX40"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MaxWorldExport - Win32 Debug MAX30" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MaxWorldExport - Win32 Release MAX30" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MaxWorldExport - Win32 Debug MAX40" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MaxWorldExport - Win32 Release MAX40" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MaxWorldExport - Win32 Debug MAX30"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "MaxWorldExport___Win32_MAX30_Debug"
# PROP BASE Intermediate_Dir "MaxWorldExport___Win32_MAX30_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\Built\Debug\tools\Plugins\MaxWorldExport30"
# PROP Intermediate_Dir "..\..\..\Built\Debug\tools\Plugins\MaxWorldExport30"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "..\sdkfiles\max30sdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "..\..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\sdkfiles\max30sdk\include" /I "..\..\..\libs\ltamgr" /I "..\..\..\sdk\inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /FD /GZ /Zm1000 /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib maxutil.lib core.lib geom.lib bmm.lib mesh.lib paramblk2.lib mnmath.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\..\..\..\proj\Debug\MaxWorldExport.dle" /pdbtype:sept /libpath:"..\sdkfiles\max30sdk\lib"
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib maxutil.lib core.lib geom.lib bmm.lib mesh.lib paramblk2.lib mnmath.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libcmtd.lib" /out:"..\..\..\Built\Debug\tools\Plugins\MaxWorldExport30\MaxWorldExport30.dle" /pdbtype:sept /libpath:"..\sdkfiles\max30sdk\lib"
# Begin Custom Build
TargetPath=\Working\Engine\Built\Debug\tools\Plugins\MaxWorldExport30\MaxWorldExport30.dle
InputPath=\Working\Engine\Built\Debug\tools\Plugins\MaxWorldExport30\MaxWorldExport30.dle
SOURCE="$(InputPath)"

"..\..\..\..\development\tools\Plugins\MaxWorldExport30.dle" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\..\development\tools\Plugins 
	copy $(TargetPath) ..\..\..\..\development\tools\Plugins 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "MaxWorldExport - Win32 Release MAX30"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MaxWorldExport___Win32_MAX30_Release"
# PROP BASE Intermediate_Dir "MaxWorldExport___Win32_MAX30_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\Built\Release\tools\Plugins\MaxWorldExport30"
# PROP Intermediate_Dir "..\..\..\Built\Release\tools\Plugins\MaxWorldExport30"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\sdkfiles\max30sdk\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\sdkfiles\max30sdk\include" /I "..\..\..\libs\ltamgr" /I "..\..\..\sdk\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib maxutil.lib core.lib geom.lib bmm.lib mesh.lib paramblk2.lib mnmath.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\..\..\..\proj\Release\MaxWorldExport.dle" /libpath:"..\sdkfiles\max30sdk\lib"
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib maxutil.lib core.lib geom.lib bmm.lib mesh.lib paramblk2.lib mnmath.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib:"libcmt.lib" /out:"..\..\..\Built\Release\tools\Plugins\MaxWorldExport30\MaxWorldExport30.dle" /libpath:"..\sdkfiles\max30sdk\lib"
# Begin Custom Build
TargetPath=\Working\Engine\Built\Release\tools\Plugins\MaxWorldExport30\MaxWorldExport30.dle
InputPath=\Working\Engine\Built\Release\tools\Plugins\MaxWorldExport30\MaxWorldExport30.dle
SOURCE="$(InputPath)"

"..\..\..\..\development\tools\Plugins\MaxWorldExport30.dle" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\..\development\tools\Plugins 
	copy $(TargetPath) ..\..\..\..\development\tools\Plugins 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "MaxWorldExport - Win32 Debug MAX40"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "MaxWorldExport___Win32_MAX40_Debug"
# PROP BASE Intermediate_Dir "MaxWorldExport___Win32_MAX40_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\Built\Debug\tools\Plugins\MaxWorldExport40"
# PROP Intermediate_Dir "..\..\..\Built\Debug\tools\Plugins\MaxWorldExport40"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "..\sdkfiles\max30sdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "..\..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\sdkfiles\max40sdk\include" /I "..\..\..\libs\ltamgr" /I "..\..\..\sdk\inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib maxutil.lib core.lib geom.lib bmm.lib mesh.lib paramblk2.lib mnmath.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\..\..\..\proj\Debug\MaxWorldExport30.dle" /pdbtype:sept /libpath:"..\sdkfiles\max30sdk\lib"
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib maxutil.lib core.lib geom.lib bmm.lib mesh.lib paramblk2.lib mnmath.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libcmtd.lib" /out:"..\..\..\Built\Debug\tools\Plugins\MaxWorldExport40\MaxWorldExport40.dle" /pdbtype:sept /libpath:"..\sdkfiles\max40sdk\lib"
# Begin Custom Build
TargetPath=\Working\Engine\Built\Debug\tools\Plugins\MaxWorldExport40\MaxWorldExport40.dle
InputPath=\Working\Engine\Built\Debug\tools\Plugins\MaxWorldExport40\MaxWorldExport40.dle
SOURCE="$(InputPath)"

"..\..\..\..\development\tools\Plugins\MaxWorldExport40.dle" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\..\development\tools\Plugins 
	copy $(TargetPath) ..\..\..\..\development\tools\Plugins 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "MaxWorldExport - Win32 Release MAX40"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MaxWorldExport___Win32_MAX40_Release"
# PROP BASE Intermediate_Dir "MaxWorldExport___Win32_MAX40_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\Built\Release\tools\Plugins\MaxWorldExport40"
# PROP Intermediate_Dir "..\..\..\Built\Release\tools\Plugins\MaxWorldExport40"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\sdkfiles\max30sdk\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\sdkfiles\max40sdk\include" /I "..\..\..\libs\ltamgr" /I "..\..\..\sdk\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib maxutil.lib core.lib geom.lib bmm.lib mesh.lib paramblk2.lib mnmath.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\..\..\..\proj\Release\MaxWorldExport30.dle" /libpath:"..\sdkfiles\max30sdk\lib"
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib maxutil.lib core.lib geom.lib bmm.lib mesh.lib paramblk2.lib mnmath.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib:"libcmt.lib" /out:"..\..\..\Built\Release\tools\Plugins\MaxWorldExport40\MaxWorldExport40.dle" /libpath:"..\sdkfiles\max40sdk\lib"
# Begin Custom Build
TargetPath=\Working\Engine\Built\Release\tools\Plugins\MaxWorldExport40\MaxWorldExport40.dle
InputPath=\Working\Engine\Built\Release\tools\Plugins\MaxWorldExport40\MaxWorldExport40.dle
SOURCE="$(InputPath)"

"..\..\..\..\development\tools\Plugins\MaxWorldExport40.dle" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\..\development\tools\Plugins 
	copy $(TargetPath) ..\..\..\..\development\tools\Plugins 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "MaxWorldExport - Win32 Debug MAX30"
# Name "MaxWorldExport - Win32 Release MAX30"
# Name "MaxWorldExport - Win32 Debug MAX40"
# Name "MaxWorldExport - Win32 Release MAX40"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\DLLMain.cpp
# End Source File
# Begin Source File

SOURCE=.\Exporter.cpp
# End Source File
# Begin Source File

SOURCE=.\LTAFile.cpp
# End Source File
# Begin Source File

SOURCE=.\MaxWorldExport.cpp
# End Source File
# Begin Source File

SOURCE=.\MaxWorldExport30.def

!IF  "$(CFG)" == "MaxWorldExport - Win32 Debug MAX30"

!ELSEIF  "$(CFG)" == "MaxWorldExport - Win32 Release MAX30"

!ELSEIF  "$(CFG)" == "MaxWorldExport - Win32 Debug MAX40"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MaxWorldExport - Win32 Release MAX40"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MaxWorldExport40.def

!IF  "$(CFG)" == "MaxWorldExport - Win32 Debug MAX30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MaxWorldExport - Win32 Release MAX30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MaxWorldExport - Win32 Debug MAX40"

!ELSEIF  "$(CFG)" == "MaxWorldExport - Win32 Release MAX40"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\LTAFile.h
# End Source File
# Begin Source File

SOURCE=.\MaxWorldExport.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\MaxWorldExport.rc
# End Source File
# End Group
# Begin Group "libs_debug"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\built\Debug\libs\LTAMgr\ltamgr.lib

!IF  "$(CFG)" == "MaxWorldExport - Win32 Debug MAX30"

!ELSEIF  "$(CFG)" == "MaxWorldExport - Win32 Release MAX30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MaxWorldExport - Win32 Debug MAX40"

!ELSEIF  "$(CFG)" == "MaxWorldExport - Win32 Release MAX40"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "libs_release"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\built\Release\libs\LTAMgr\ltamgr.lib

!IF  "$(CFG)" == "MaxWorldExport - Win32 Debug MAX30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MaxWorldExport - Win32 Release MAX30"

!ELSEIF  "$(CFG)" == "MaxWorldExport - Win32 Debug MAX40"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MaxWorldExport - Win32 Release MAX40"

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=..\readme.txt
# End Source File
# End Target
# End Project
