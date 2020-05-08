# Microsoft Developer Studio Project File - Name="ServerDir" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ServerDir - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ServerDir.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ServerDir.mak" CFG="ServerDir - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ServerDir - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ServerDir - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ServerDir - Win32 Final" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/SA/Johnson_Code/Johnson_Client/Libs/ServerDir", SBUBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ServerDir - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\built\Release\libs\ServerDir"
# PROP Intermediate_Dir "..\..\built\Release\libs\ServerDir"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SERVERDIR_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I ".\\" /I "..\..\..\Engine\sdk\inc" /I "..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\..\libs\wonapi" /I "..\..\ClientRes\Shared" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 wonapi.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /profile /debug /machine:I386 /libpath:"..\..\built\release\libs\wonapi"
# Begin Custom Build
TargetDir=\SA\Johnson_Code\Johnson_Client\built\Release\libs\ServerDir
TargetPath=\SA\Johnson_Code\Johnson_Client\built\Release\libs\ServerDir\ServerDir.dll
InputPath=\SA\Johnson_Code\Johnson_Client\built\Release\libs\ServerDir\ServerDir.dll
SOURCE="$(InputPath)"

BuildCmds= \
	Rem IncrediBuild_OutputFile $(TargetPath) \
	echo copy $(TargetPath) $(SA_MAIN_DIR) \
	copy $(TargetPath) $(SA_MAIN_DIR) \
	echo copy $(TargetPath) $(SA_TOOLS_DIR) \
	copy $(TargetPath) $(SA_TOOLS_DIR) \
	echo   copy   $(TargetDir)\ServerDir.lib   $(TargetDir)\..\serverdir.lib \
	copy   $(TargetDir)\ServerDir.lib    $(TargetDir)\..\serverdir.lib \
	

"$(SA_MAIN_DIR)\ServerDir.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(SA_TOOLS_DIR)\ServerDir.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(TargetDir)\..\serverdir.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "ServerDir - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\Johnson_Built\Johnson_Client\Debug\libs\ServerDir"
# PROP Intermediate_Dir "..\..\..\Johnson_Built\Johnson_Client\Debug\libs\ServerDir"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SERVERDIR_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\\" /I "..\..\..\Johnson_Engine\sdk\inc" /I "..\..\..\Johnson_Libs\STLPORT-4.0\STLPORT" /I "..\..\libs\wonapi" /I "..\..\ClientRes\Shared" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wonapi.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"..\..\..\Lib\Debug\ServerDir.dll" /implib:"..\..\..\Lib\Debug\ServerDir.lib" /pdbtype:sept /libpath:"..\..\..\Lib\Debug"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build
TargetPath=\SA\Johnson_Code\Lib\Debug\ServerDir.dll
InputPath=\SA\Johnson_Code\Lib\Debug\ServerDir.dll
SOURCE="$(InputPath)"

"$(SA_MAIN_DIR)\ServerDir.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	Rem IncrediBuild_OutputFile $(TargetPath) 
	echo copy $(TargetPath) $(SA_MAIN_DIR) 
	copy $(TargetPath) $(SA_MAIN_DIR) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "ServerDir - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ServerDir___Win32_Final"
# PROP BASE Intermediate_Dir "ServerDir___Win32_Final"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\built\Final\libs\ServerDir"
# PROP Intermediate_Dir "..\..\built\Final\libs\ServerDir"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GR /GX /O2 /I ".\\" /I "..\..\..\Engine\sdk\inc" /I "..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\..\libs\wonapi" /I "..\..\ClientRes\Shared" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /I ".\\" /I "..\..\..\Engine\sdk\inc" /I "..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\..\libs\wonapi" /I "..\..\ClientRes\Shared" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_FINAL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wonapi.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /libpath:"..\..\built\release\libs\wonapi"
# ADD LINK32 wonapi.lib kernel32.lib user32.lib gdi32.lib /nologo /dll /profile /debug /machine:I386 /nodefaultlib:"libc" /libpath:"..\..\built\final\libs\wonapi" /Release
# Begin Custom Build
TargetDir=\SA\Johnson_Code\Johnson_Client\built\Final\libs\ServerDir
TargetPath=\SA\Johnson_Code\Johnson_Client\built\Final\libs\ServerDir\ServerDir.dll
InputPath=\SA\Johnson_Code\Johnson_Client\built\Final\libs\ServerDir\ServerDir.dll
SOURCE="$(InputPath)"

BuildCmds= \
	Rem IncrediBuild_OutputFile $(TargetPath) \
	echo copy $(TargetPath) $(SA_MAIN_DIR) \
	copy $(TargetPath) $(SA_MAIN_DIR) \
	echo copy $(TargetPath) $(SA_TOOLS_DIR) \
	copy $(TargetPath) $(SA_TOOLS_DIR) \
	echo   copy   $(TargetDir)\ServerDir.lib   $(TargetDir)\..\serverdir.lib \
	copy   $(TargetDir)\ServerDir.lib   $(TargetDir)\..\serverdir.lib \
	

"$(SA_MAIN_DIR)\ServerDir.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(SA_TOOLS_DIR)\ServerDir.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(TargetDir)\..\ServerDir.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# Begin Target

# Name "ServerDir - Win32 Release"
# Name "ServerDir - Win32 Debug"
# Name "ServerDir - Win32 Final"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CServerDir_Titan.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TitanTest.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CServerDir_Titan.h
# End Source File
# Begin Source File

SOURCE=.\IServerDir.h
# End Source File
# Begin Source File

SOURCE=.\IServerDir_Titan.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TitanTest.h
# End Source File
# Begin Source File

SOURCE=.\WinSync.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
