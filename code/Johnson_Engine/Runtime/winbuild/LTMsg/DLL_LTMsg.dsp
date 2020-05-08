# Microsoft Developer Studio Project File - Name="DLL_LTMsg" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=DLL_LTMsg - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DLL_LTMsg.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DLL_LTMsg.mak" CFG="DLL_LTMsg - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DLL_LTMsg - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DLL_LTMsg - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DLL_LTMsg - Win32 Final" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DLL_LTMsg - Win32 Final DE" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DLL_LTMsg - Win32 Final ES" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DLL_LTMsg - Win32 Final IT" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DLL_LTMsg - Win32 Final FR" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/SA/Johnson_Code/Johnson_Engine/Runtime/winbuild/ltmsg", HGYBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DLL_LTMsg - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\built\Release\Runtime\DLL_LTMsg"
# PROP Intermediate_Dir "..\..\..\built\Release\Runtime\DLL_LTMsg"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /I "..\..\..\..\Libs\STLPORT-4.0\STLPORT" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "NO_PROFILE" /D "NOT_USING_FULL" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "Lang\EN" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /profile /debug /machine:I386 /out:"..\..\..\built\Release\Runtime\DLL_LTMsg/LTMsg.dll"
# Begin Custom Build
TargetPath=\SA\Johnson_Code\Johnson_Engine\built\Release\Runtime\DLL_LTMsg\LTMsg.dll
InputPath=\SA\Johnson_Code\Johnson_Engine\built\Release\Runtime\DLL_LTMsg\LTMsg.dll
SOURCE="$(InputPath)"

"$(SA_MAIN_DIR)\ltmsg.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	Rem IncrediBuild_OutputFile $(TargetPath) 
	echo copy $(TargetPath) $(SA_MAIN_DIR) 
	copy $(TargetPath) $(SA_MAIN_DIR) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "DLL_LTMsg - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\..\Johnson_Built\Johnson_Engine\Debug\Runtime\DLL_LTMsg"
# PROP Intermediate_Dir "..\..\..\..\Johnson_Built\Johnson_Engine\Debug\Runtime\DLL_LTMsg"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\..\..\..\Johnson_Libs\STLPORT-4.0\STLPORT" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "Lang\EN" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /profile /debug /machine:I386 /out:"..\..\..\..\Lib\Debug\LTMsg.dll"
# Begin Custom Build
TargetPath=\SA\Johnson_Code\Lib\Debug\LTMsg.dll
InputPath=\SA\Johnson_Code\Lib\Debug\LTMsg.dll
SOURCE="$(InputPath)"

"$(SA_MAIN_DIR)\ltmsg.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	Rem IncrediBuild_OutputFile $(TargetPath) 
	echo copy $(TargetPath) $(SA_MAIN_DIR) 
	copy $(TargetPath) $(SA_MAIN_DIR) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "DLL_LTMsg - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Final"
# PROP BASE Intermediate_Dir "Final"
# PROP BASE Ignore_Export_Lib 1
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\built\Final\Runtime\DLL_LTMsg"
# PROP Intermediate_Dir "..\..\..\built\Final\Runtime\DLL_LTMsg"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\..\Libs\STLPORT-4.0\STLPORT" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /I "..\..\..\..\Libs\STLPORT-4.0\STLPORT" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_FINAL" /D "NO_PROFILE" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "Lang\EN" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\..\..\built\Final\Runtime\DLL_LTMsg\ltmsg.dll"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libc.lib" /out:"..\..\..\built\Final\Runtime\DLL_LTMsg\ltmsg.dll" /Release
# SUBTRACT LINK32 /profile /pdb:none
# Begin Custom Build
TargetPath=\SA\Johnson_Code\Johnson_Engine\built\Final\Runtime\DLL_LTMsg\ltmsg.dll
InputPath=\SA\Johnson_Code\Johnson_Engine\built\Final\Runtime\DLL_LTMsg\ltmsg.dll
SOURCE="$(InputPath)"

"$(SA_MAIN_DIR)\ltmsg.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	Rem IncrediBuild_OutputFile $(TargetPath) 
	echo copy $(TargetPath) $(SA_MAIN_DIR) 
	copy $(TargetPath) $(SA_MAIN_DIR) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "DLL_LTMsg - Win32 Final DE"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DLL_LTMsg___Win32_Final_DE"
# PROP BASE Intermediate_Dir "DLL_LTMsg___Win32_Final_DE"
# PROP BASE Ignore_Export_Lib 1
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\built\FinalDE\Runtime\DLL_LTMsg"
# PROP Intermediate_Dir "..\..\..\built\FinalDE\Runtime\DLL_LTMsg"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "..\..\..\..\Libs\STLPORT-4.0\STLPORT" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_FINAL" /FR /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "..\..\..\..\Libs\STLPORT-4.0\STLPORT" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_FINAL" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "Lang\EN" /d "NDEBUG"
# ADD RSC /l 0x409 /i "Lang\EN" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\..\..\built\Final\Runtime\DLL_LTMsg\ltmsg.dll"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\..\..\built\Final\Runtime\DLL_LTMsg\ltmsg.dll"
# Begin Custom Build
TargetPath=\SA\Johnson_Code\Johnson_Engine\built\Final\Runtime\DLL_LTMsg\ltmsg.dll
InputPath=\SA\Johnson_Code\Johnson_Engine\built\Final\Runtime\DLL_LTMsg\ltmsg.dll
SOURCE="$(InputPath)"

"$(GAME_MAIN_DIR)\ltmsg.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	Rem IncrediBuild_OutputFile $(TargetPath) 
	echo copy $(TargetPath) $(GAME_MAIN_DIR) 
	copy $(TargetPath) $(GAME_MAIN_DIR) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "DLL_LTMsg - Win32 Final ES"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DLL_LTMsg___Win32_Final_ES"
# PROP BASE Intermediate_Dir "DLL_LTMsg___Win32_Final_ES"
# PROP BASE Ignore_Export_Lib 1
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\built\FinalES\Runtime\DLL_LTMsg"
# PROP Intermediate_Dir "..\..\..\built\FinalES\Runtime\DLL_LTMsg"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "..\..\..\..\Libs\STLPORT-4.0\STLPORT" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_FINAL" /FR /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "..\..\..\..\Libs\STLPORT-4.0\STLPORT" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_FINAL" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "Lang\EN" /d "NDEBUG"
# ADD RSC /l 0x409 /i "Lang\EN" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\..\..\built\Final\Runtime\DLL_LTMsg\ltmsg.dll"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\..\..\built\Final\Runtime\DLL_LTMsg\ltmsg.dll"
# Begin Custom Build
TargetPath=\SA\Johnson_Code\Johnson_Engine\built\Final\Runtime\DLL_LTMsg\ltmsg.dll
InputPath=\SA\Johnson_Code\Johnson_Engine\built\Final\Runtime\DLL_LTMsg\ltmsg.dll
SOURCE="$(InputPath)"

"$(GAME_MAIN_DIR)\ltmsg.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	Rem IncrediBuild_OutputFile $(TargetPath) 
	echo copy $(TargetPath) $(GAME_MAIN_DIR) 
	copy $(TargetPath) $(GAME_MAIN_DIR) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "DLL_LTMsg - Win32 Final IT"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DLL_LTMsg___Win32_Final_IT"
# PROP BASE Intermediate_Dir "DLL_LTMsg___Win32_Final_IT"
# PROP BASE Ignore_Export_Lib 1
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\built\FinalIT\Runtime\DLL_LTMsg"
# PROP Intermediate_Dir "..\..\..\built\FinalIT\Runtime\DLL_LTMsg"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "..\..\..\..\Libs\STLPORT-4.0\STLPORT" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_FINAL" /FR /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "..\..\..\..\Libs\STLPORT-4.0\STLPORT" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_FINAL" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "Lang\EN" /d "NDEBUG"
# ADD RSC /l 0x409 /i "Lang\EN" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\..\..\built\Final\Runtime\DLL_LTMsg\ltmsg.dll"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\..\..\built\Final\Runtime\DLL_LTMsg\ltmsg.dll"
# Begin Custom Build
TargetPath=\SA\Johnson_Code\Johnson_Engine\built\Final\Runtime\DLL_LTMsg\ltmsg.dll
InputPath=\SA\Johnson_Code\Johnson_Engine\built\Final\Runtime\DLL_LTMsg\ltmsg.dll
SOURCE="$(InputPath)"

"$(GAME_MAIN_DIR)\ltmsg.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	Rem IncrediBuild_OutputFile $(TargetPath) 
	echo copy $(TargetPath) $(GAME_MAIN_DIR) 
	copy $(TargetPath) $(GAME_MAIN_DIR) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "DLL_LTMsg - Win32 Final FR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DLL_LTMsg___Win32_Final_FR"
# PROP BASE Intermediate_Dir "DLL_LTMsg___Win32_Final_FR"
# PROP BASE Ignore_Export_Lib 1
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\built\FinalFR\Runtime\DLL_LTMsg"
# PROP Intermediate_Dir "..\..\..\built\FinalFR\Runtime\DLL_LTMsg"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "..\..\..\..\Libs\STLPORT-4.0\STLPORT" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_FINAL" /FR /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "..\..\..\..\Libs\STLPORT-4.0\STLPORT" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_FINAL" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /i "Lang\EN" /d "NDEBUG"
# ADD RSC /l 0x409 /i "Lang\EN" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\..\..\built\Final\Runtime\DLL_LTMsg\ltmsg.dll"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\..\..\built\Final\Runtime\DLL_LTMsg\ltmsg.dll"
# Begin Custom Build
TargetPath=\SA\Johnson_Code\Johnson_Engine\built\Final\Runtime\DLL_LTMsg\ltmsg.dll
InputPath=\SA\Johnson_Code\Johnson_Engine\built\Final\Runtime\DLL_LTMsg\ltmsg.dll
SOURCE="$(InputPath)"

"$(GAME_MAIN_DIR)\ltmsg.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	Rem IncrediBuild_OutputFile $(TargetPath) 
	echo copy $(TargetPath) $(GAME_MAIN_DIR) 
	copy $(TargetPath) $(GAME_MAIN_DIR) 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "DLL_LTMsg - Win32 Release"
# Name "DLL_LTMsg - Win32 Debug"
# Name "DLL_LTMsg - Win32 Final"
# Name "DLL_LTMsg - Win32 Final DE"
# Name "DLL_LTMsg - Win32 Final ES"
# Name "DLL_LTMsg - Win32 Final IT"
# Name "DLL_LTMsg - Win32 Final FR"
# Begin Group "Localized Files"

# PROP Default_Filter ""
# Begin Group "DE"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Lang\DE\ltmsg.rc

!IF  "$(CFG)" == "DLL_LTMsg - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DLL_LTMsg - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DLL_LTMsg - Win32 Final"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DLL_LTMsg - Win32 Final DE"

!ELSEIF  "$(CFG)" == "DLL_LTMsg - Win32 Final ES"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DLL_LTMsg - Win32 Final IT"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DLL_LTMsg - Win32 Final FR"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "EN"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Lang\EN\ltmsg.rc

!IF  "$(CFG)" == "DLL_LTMsg - Win32 Release"

!ELSEIF  "$(CFG)" == "DLL_LTMsg - Win32 Debug"

!ELSEIF  "$(CFG)" == "DLL_LTMsg - Win32 Final"

!ELSEIF  "$(CFG)" == "DLL_LTMsg - Win32 Final DE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DLL_LTMsg - Win32 Final ES"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DLL_LTMsg - Win32 Final IT"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DLL_LTMsg - Win32 Final FR"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "ES"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Lang\ES\ltmsg.rc

!IF  "$(CFG)" == "DLL_LTMsg - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DLL_LTMsg - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DLL_LTMsg - Win32 Final"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DLL_LTMsg - Win32 Final DE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DLL_LTMsg - Win32 Final ES"

!ELSEIF  "$(CFG)" == "DLL_LTMsg - Win32 Final IT"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DLL_LTMsg - Win32 Final FR"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "IT"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Lang\IT\ltmsg.rc

!IF  "$(CFG)" == "DLL_LTMsg - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DLL_LTMsg - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DLL_LTMsg - Win32 Final"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DLL_LTMsg - Win32 Final DE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DLL_LTMsg - Win32 Final ES"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DLL_LTMsg - Win32 Final IT"

!ELSEIF  "$(CFG)" == "DLL_LTMsg - Win32 Final FR"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "FR"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Lang\FR\ltmsg.rc

!IF  "$(CFG)" == "DLL_LTMsg - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DLL_LTMsg - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DLL_LTMsg - Win32 Final"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DLL_LTMsg - Win32 Final DE"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DLL_LTMsg - Win32 Final ES"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DLL_LTMsg - Win32 Final IT"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "DLL_LTMsg - Win32 Final FR"

!ENDIF 

# End Source File
# End Group
# End Group
# Begin Source File

SOURCE=.\Init.cpp
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Target
# End Project
