# Microsoft Developer Studio Project File - Name="DLL_dx8" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=DLL_dx8 - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DLL_dx8.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DLL_dx8.mak" CFG="DLL_dx8 - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DLL_dx8 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DLL_dx8 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DLL_dx8 - Win32 Final" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/SA/Johnson_Code/Johnson_Engine/Runtime/sound/src/sys/s_dx8", OBYBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DLL_dx8 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\..\..\built\Release\Runtime\DLL_dx8"
# PROP Intermediate_Dir "..\..\..\..\..\built\Release\Runtime\DLL_dx8"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /Ob2 /I "..\..\..\..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\..\..\..\..\sdk\inc" /I "..\..\..\..\..\..\libs\lith" /I "..\..\\" /I "..\..\..\..\shared\src\sys\win" /I "..\..\..\..\kernel\src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "NO_PROFILE" /D "NOT_USING_FULL" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dxguid.lib winmm.lib msacm32.lib eaxguid.lib /nologo /subsystem:windows /dll /profile /debug /machine:I386 /out:"..\..\..\..\..\built\Release\Runtime\DLL_dx8\SndDrv.dll"
# Begin Custom Build
TargetPath=\SA\Johnson_Code\Johnson_Engine\built\Release\Runtime\DLL_dx8\SndDrv.dll
InputPath=\SA\Johnson_Code\Johnson_Engine\built\Release\Runtime\DLL_dx8\SndDrv.dll
SOURCE="$(InputPath)"

"$(SA_MAIN_DIR)\SndDrv.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	Rem IncrediBuild_OutputFile $(TargetPath) 
	echo copy $(TargetPath) $(SA_MAIN_DIR) 
	copy $(TargetPath) $(SA_MAIN_DIR) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "DLL_dx8 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\..\..\..\Johnson_Built\Johnson_Engine\Debug\Runtime\DLL_dx8"
# PROP Intermediate_Dir "..\..\..\..\..\..\Johnson_Built\Johnson_Engine\Debug\Runtime\DLL_dx8"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\..\..\Johnson_Libs\STLPORT-4.0\STLPORT" /I "..\..\..\..\..\sdk\inc" /I "..\..\..\..\..\..\Johnson_Libs\lith" /I "..\..\\" /I "..\..\..\..\shared\src\sys\win" /I "..\..\..\..\kernel\src" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dxguid.lib winmm.lib msacm32.lib eaxguid.lib /nologo /subsystem:windows /dll /profile /debug /machine:I386 /nodefaultlib:"libcd.lib" /out:"..\..\..\..\..\..\Lib\Debug\SndDrv.dll"
# SUBTRACT LINK32 /nodefaultlib
# Begin Custom Build
TargetPath=\SA\Johnson_Code\Lib\Debug\SndDrv.dll
InputPath=\SA\Johnson_Code\Lib\Debug\SndDrv.dll
SOURCE="$(InputPath)"

"$(SA_MAIN_DIR)\SndDrv.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	Rem IncrediBuild_OutputFile $(TargetPath) 
	echo copy $(TargetPath) $(SA_MAIN_DIR) 
	copy $(TargetPath) $(SA_MAIN_DIR) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "DLL_dx8 - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Final"
# PROP BASE Intermediate_Dir "Final"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\..\..\built\Final\Runtime\DLL_dx8"
# PROP Intermediate_Dir "..\..\..\..\..\built\Final\Runtime\DLL_dx8"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O2 /I "..\..\..\..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\..\..\..\..\sdk\inc" /I "..\..\..\..\..\..\libs\lith" /I "..\..\\" /I "..\..\..\..\shared\src\sys\win" /I "..\..\..\..\kernel\src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /Ob2 /I "..\..\..\..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\..\..\..\..\sdk\inc" /I "..\..\..\..\..\..\libs\lith" /I "..\..\\" /I "..\..\..\..\shared\src\sys\win" /I "..\..\..\..\kernel\src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_FINAL" /D "NO_PROFILE" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dxguid.lib winmm.lib msacm32.lib eaxguid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\..\..\..\..\built\Final\Runtime\DLL_dx8\SndDrv.dll"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib dxguid.lib winmm.lib msacm32.lib eaxguid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libc.lib" /out:"..\..\..\..\..\built\Final\Runtime\DLL_dx8\SndDrv.dll" /Release
# SUBTRACT LINK32 /profile /pdb:none
# Begin Custom Build
TargetPath=\SA\Johnson_Code\Johnson_Engine\built\Final\Runtime\DLL_dx8\SndDrv.dll
InputPath=\SA\Johnson_Code\Johnson_Engine\built\Final\Runtime\DLL_dx8\SndDrv.dll
SOURCE="$(InputPath)"

"$(SA_MAIN_DIR)\SndDrv.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	Rem IncrediBuild_OutputFile $(TargetPath) 
	echo copy $(TargetPath) $(SA_MAIN_DIR) 
	copy $(TargetPath) $(SA_MAIN_DIR) 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "DLL_dx8 - Win32 Release"
# Name "DLL_dx8 - Win32 Debug"
# Name "DLL_dx8 - Win32 Final"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\..\kernel\src\sys\win\counter.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\parsewavefile.cpp
# End Source File
# Begin Source File

SOURCE=.\s_dx8.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\eax.h
# End Source File
# Begin Source File

SOURCE=.\s_dx8.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
