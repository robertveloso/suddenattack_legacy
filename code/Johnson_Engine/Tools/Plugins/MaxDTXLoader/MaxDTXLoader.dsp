# Microsoft Developer Studio Project File - Name="MaxDTXLoader" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=MaxDTXLoader - Win32 Debug MAX40
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MaxDTXLoader.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MaxDTXLoader.mak" CFG="MaxDTXLoader - Win32 Debug MAX40"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MaxDTXLoader - Win32 Debug MAX30" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MaxDTXLoader - Win32 Release MAX30" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MaxDTXLoader - Win32 Debug MAX40" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MaxDTXLoader - Win32 Release MAX40" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MaxDTXLoader - Win32 Debug MAX30"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug MAX30"
# PROP BASE Intermediate_Dir "Debug MAX30"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\Built\Debug\tools\Plugins\MaxDTXLoader30"
# PROP Intermediate_Dir "..\..\..\Built\Debug\tools\Plugins\MaxDTXLoader30"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MAXDTXLOADER_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "..\..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\sdkfiles\max30sdk\include" /I "..\..\shared\engine" /I "..\..\..\..\libs\stdlith" /I "..\..\..\sdk\inc" /I "..\..\preprocessor" /I "..\..\shared\world" /I "..\..\dedit\misc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "DIRECTEDITOR_BUILD" /D "STDLITH_MFC" /D "BDEFS_MFC" /D "PRE_FLOAT" /D "PREPROCESSOR_BUILD" /D "NO_PRAGMA_LIBS" /D "LT15_COMPAT" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib core.lib bmm.lib maxutil.lib paramblk2.lib /nologo /dll /debug /machine:I386 /out:"..\..\..\Built\Debug\tools\Plugins\MaxDTXLoader30\MaxDTXLoader30.bmi" /pdbtype:sept /libpath:"..\sdkfiles\max30sdk\lib"
# SUBTRACT LINK32 /nodefaultlib
# Begin Custom Build
TargetPath=\Working\Engine\Built\Debug\tools\Plugins\MaxDTXLoader30\MaxDTXLoader30.bmi
InputPath=\Working\Engine\Built\Debug\tools\Plugins\MaxDTXLoader30\MaxDTXLoader30.bmi
SOURCE="$(InputPath)"

"..\..\..\..\development\tools\Plugins\MaxDTXLoader30.bmi" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\..\development\tools\Plugins 
	copy $(TargetPath) ..\..\..\..\development\tools\Plugins 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "MaxDTXLoader - Win32 Release MAX30"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release MAX30"
# PROP BASE Intermediate_Dir "Release MAX30"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\Built\Release\tools\Plugins\MaxDTXLoader30"
# PROP Intermediate_Dir "..\..\..\Built\Release\tools\Plugins\MaxDTXLoader30"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MAXDTXLOADER_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\sdkfiles\max30sdk\include" /I "..\..\shared\engine" /I "..\..\..\..\libs\stdlith" /I "..\..\..\sdk\inc" /I "..\..\preprocessor" /I "..\..\shared\world" /I "..\..\dedit\misc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib core.lib bmm.lib maxutil.lib paramblk2.lib /nologo /dll /machine:I386 /out:"..\..\..\Built\Release\tools\Plugins\MaxDTXLoader30\MaxDTXLoader30.bmi" /libpath:"..\sdkfiles\max30sdk\lib"
# Begin Custom Build
TargetPath=\Working\Engine\Built\Release\tools\Plugins\MaxDTXLoader30\MaxDTXLoader30.bmi
InputPath=\Working\Engine\Built\Release\tools\Plugins\MaxDTXLoader30\MaxDTXLoader30.bmi
SOURCE="$(InputPath)"

"..\..\..\..\development\tools\Plugins\MaxDTXLoader30.bmi" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\..\development\tools\Plugins 
	copy $(TargetPath) ..\..\..\..\development\tools\Plugins 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "MaxDTXLoader - Win32 Debug MAX40"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "MaxDTXLoader___Win32_Debug_MAX40"
# PROP BASE Intermediate_Dir "MaxDTXLoader___Win32_Debug_MAX40"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\Built\Debug\tools\Plugins\MaxDTXLoader40"
# PROP Intermediate_Dir "..\..\..\Built\Debug\tools\Plugins\MaxDTXLoader40"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "..\..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\sdkfiles\max30sdk\include" /I "..\..\shared\engine" /I "..\..\..\..\libs\stdlith" /I "..\..\..\sdk\inc" /I "..\..\preprocessor" /I "..\..\shared\world" /I "..\..\dedit\misc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "DIRECTEDITOR_BUILD" /D "STDLITH_MFC" /D "BDEFS_MFC" /D "PRE_FLOAT" /D "PREPROCESSOR_BUILD" /D "NO_PRAGMA_LIBS" /D "LT15_COMPAT" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "..\..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\sdkfiles\max40sdk\include" /I "..\..\shared\engine" /I "..\..\..\..\libs\stdlith" /I "..\..\..\sdk\inc" /I "..\..\preprocessor" /I "..\..\shared\world" /I "..\..\dedit\misc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "DIRECTEDITOR_BUILD" /D "STDLITH_MFC" /D "BDEFS_MFC" /D "PRE_FLOAT" /D "PREPROCESSOR_BUILD" /D "NO_PRAGMA_LIBS" /D "LT15_COMPAT" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib core.lib bmm.lib maxutil.lib paramblk2.lib /nologo /dll /debug /machine:I386 /out:"..\..\..\Built\Debug\tools\Plugins\MaxDTXLoader30\MaxDTXLoader30.dle" /pdbtype:sept /libpath:"..\sdkfiles\max30sdk\lib"
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib core.lib bmm.lib maxutil.lib paramblk2.lib /nologo /dll /debug /machine:I386 /out:"..\..\..\Built\Debug\tools\Plugins\MaxDTXLoader40\MaxDTXLoader40.bmi" /pdbtype:sept /libpath:"..\sdkfiles\max40sdk\lib"
# SUBTRACT LINK32 /nodefaultlib
# Begin Custom Build
TargetPath=\Working\Engine\Built\Debug\tools\Plugins\MaxDTXLoader40\MaxDTXLoader40.bmi
InputPath=\Working\Engine\Built\Debug\tools\Plugins\MaxDTXLoader40\MaxDTXLoader40.bmi
SOURCE="$(InputPath)"

"..\..\..\..\development\tools\Plugins\MaxDTXLoader40.bmi" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\..\development\tools\Plugins 
	copy $(TargetPath) ..\..\..\..\development\tools\Plugins 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "MaxDTXLoader - Win32 Release MAX40"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MaxDTXLoader___Win32_Release_MAX40"
# PROP BASE Intermediate_Dir "MaxDTXLoader___Win32_Release_MAX40"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\Built\Release\tools\Plugins\MaxDTXLoader40"
# PROP Intermediate_Dir "..\..\..\Built\Release\tools\Plugins\MaxDTXLoader40"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\sdkfiles\max30sdk\include" /I "..\..\shared\engine" /I "..\..\..\..\libs\stdlith" /I "..\..\..\sdk\inc" /I "..\..\preprocessor" /I "..\..\shared\world" /I "..\..\dedit\misc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\sdkfiles\max40sdk\include" /I "..\..\shared\engine" /I "..\..\..\..\libs\stdlith" /I "..\..\..\sdk\inc" /I "..\..\preprocessor" /I "..\..\shared\world" /I "..\..\dedit\misc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib core.lib bmm.lib maxutil.lib paramblk2.lib /nologo /dll /machine:I386 /out:"..\..\..\Built\Release\tools\Plugins\MaxDTXLoader30\MaxDTXLoader30.dle" /libpath:"..\sdkfiles\max30sdk\lib"
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib core.lib bmm.lib maxutil.lib paramblk2.lib /nologo /dll /machine:I386 /out:"..\..\..\Built\Release\tools\Plugins\MaxDTXLoader40\MaxDTXLoader40.bmi" /libpath:"..\sdkfiles\max40sdk\lib"
# Begin Custom Build
TargetPath=\Working\Engine\Built\Release\tools\Plugins\MaxDTXLoader40\MaxDTXLoader40.bmi
InputPath=\Working\Engine\Built\Release\tools\Plugins\MaxDTXLoader40\MaxDTXLoader40.bmi
SOURCE="$(InputPath)"

"..\..\..\..\development\tools\Plugins\MaxDTXLoader40.bmi" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\..\development\tools\Plugins 
	copy $(TargetPath) ..\..\..\..\development\tools\Plugins 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "MaxDTXLoader - Win32 Debug MAX30"
# Name "MaxDTXLoader - Win32 Release MAX30"
# Name "MaxDTXLoader - Win32 Debug MAX40"
# Name "MaxDTXLoader - Win32 Release MAX40"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\DLLMain.cpp

!IF  "$(CFG)" == "MaxDTXLoader - Win32 Debug MAX30"

!ELSEIF  "$(CFG)" == "MaxDTXLoader - Win32 Release MAX30"

# ADD CPP /D "DIRECTEDITOR_BUILD" /D "STDLITH_MFC" /D "BDEFS_MFC" /D "PRE_FLOAT" /D "PREPROCESSOR_BUILD" /D "NO_PRAGMA_LIBS" /D "LT15_COMPAT"

!ELSEIF  "$(CFG)" == "MaxDTXLoader - Win32 Debug MAX40"

!ELSEIF  "$(CFG)" == "MaxDTXLoader - Win32 Release MAX40"

# ADD BASE CPP /D "DIRECTEDITOR_BUILD" /D "STDLITH_MFC" /D "BDEFS_MFC" /D "PRE_FLOAT" /D "PREPROCESSOR_BUILD" /D "NO_PRAGMA_LIBS" /D "LT15_COMPAT"
# ADD CPP /D "DIRECTEDITOR_BUILD" /D "STDLITH_MFC" /D "BDEFS_MFC" /D "PRE_FLOAT" /D "PREPROCESSOR_BUILD" /D "NO_PRAGMA_LIBS" /D "LT15_COMPAT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\shared\engine\dtxmgr.cpp

!IF  "$(CFG)" == "MaxDTXLoader - Win32 Debug MAX30"

!ELSEIF  "$(CFG)" == "MaxDTXLoader - Win32 Release MAX30"

# ADD CPP /D "DIRECTEDITOR_BUILD" /D "STDLITH_MFC" /D "BDEFS_MFC" /D "PRE_FLOAT" /D "PREPROCESSOR_BUILD" /D "NO_PRAGMA_LIBS" /D "LT15_COMPAT"

!ELSEIF  "$(CFG)" == "MaxDTXLoader - Win32 Debug MAX40"

!ELSEIF  "$(CFG)" == "MaxDTXLoader - Win32 Release MAX40"

# ADD BASE CPP /D "DIRECTEDITOR_BUILD" /D "STDLITH_MFC" /D "BDEFS_MFC" /D "PRE_FLOAT" /D "PREPROCESSOR_BUILD" /D "NO_PRAGMA_LIBS" /D "LT15_COMPAT"
# ADD CPP /D "DIRECTEDITOR_BUILD" /D "STDLITH_MFC" /D "BDEFS_MFC" /D "PRE_FLOAT" /D "PREPROCESSOR_BUILD" /D "NO_PRAGMA_LIBS" /D "LT15_COMPAT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\shared\engine\genltstream.cpp

!IF  "$(CFG)" == "MaxDTXLoader - Win32 Debug MAX30"

!ELSEIF  "$(CFG)" == "MaxDTXLoader - Win32 Release MAX30"

# ADD CPP /D "DIRECTEDITOR_BUILD" /D "STDLITH_MFC" /D "BDEFS_MFC" /D "PRE_FLOAT" /D "PREPROCESSOR_BUILD" /D "NO_PRAGMA_LIBS" /D "LT15_COMPAT"

!ELSEIF  "$(CFG)" == "MaxDTXLoader - Win32 Debug MAX40"

!ELSEIF  "$(CFG)" == "MaxDTXLoader - Win32 Release MAX40"

# ADD BASE CPP /D "DIRECTEDITOR_BUILD" /D "STDLITH_MFC" /D "BDEFS_MFC" /D "PRE_FLOAT" /D "PREPROCESSOR_BUILD" /D "NO_PRAGMA_LIBS" /D "LT15_COMPAT"
# ADD CPP /D "DIRECTEDITOR_BUILD" /D "STDLITH_MFC" /D "BDEFS_MFC" /D "PRE_FLOAT" /D "PREPROCESSOR_BUILD" /D "NO_PRAGMA_LIBS" /D "LT15_COMPAT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MaxDTXLoader.cpp

!IF  "$(CFG)" == "MaxDTXLoader - Win32 Debug MAX30"

!ELSEIF  "$(CFG)" == "MaxDTXLoader - Win32 Release MAX30"

# ADD CPP /D "DIRECTEDITOR_BUILD" /D "STDLITH_MFC" /D "BDEFS_MFC" /D "PRE_FLOAT" /D "PREPROCESSOR_BUILD" /D "NO_PRAGMA_LIBS" /D "LT15_COMPAT"

!ELSEIF  "$(CFG)" == "MaxDTXLoader - Win32 Debug MAX40"

!ELSEIF  "$(CFG)" == "MaxDTXLoader - Win32 Release MAX40"

# ADD BASE CPP /D "DIRECTEDITOR_BUILD" /D "STDLITH_MFC" /D "BDEFS_MFC" /D "PRE_FLOAT" /D "PREPROCESSOR_BUILD" /D "NO_PRAGMA_LIBS" /D "LT15_COMPAT"
# ADD CPP /D "DIRECTEDITOR_BUILD" /D "STDLITH_MFC" /D "BDEFS_MFC" /D "PRE_FLOAT" /D "PREPROCESSOR_BUILD" /D "NO_PRAGMA_LIBS" /D "LT15_COMPAT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MaxDTXLoader30.def

!IF  "$(CFG)" == "MaxDTXLoader - Win32 Debug MAX30"

!ELSEIF  "$(CFG)" == "MaxDTXLoader - Win32 Release MAX30"

!ELSEIF  "$(CFG)" == "MaxDTXLoader - Win32 Debug MAX40"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MaxDTXLoader - Win32 Release MAX40"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MaxDTXLoader40.def

!IF  "$(CFG)" == "MaxDTXLoader - Win32 Debug MAX30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MaxDTXLoader - Win32 Release MAX30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MaxDTXLoader - Win32 Debug MAX40"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MaxDTXLoader - Win32 Release MAX40"

# PROP BASE Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\shared\engine\pixelformat.cpp

!IF  "$(CFG)" == "MaxDTXLoader - Win32 Debug MAX30"

!ELSEIF  "$(CFG)" == "MaxDTXLoader - Win32 Release MAX30"

# ADD CPP /D "DIRECTEDITOR_BUILD" /D "STDLITH_MFC" /D "BDEFS_MFC" /D "PRE_FLOAT" /D "PREPROCESSOR_BUILD" /D "NO_PRAGMA_LIBS" /D "LT15_COMPAT"

!ELSEIF  "$(CFG)" == "MaxDTXLoader - Win32 Debug MAX40"

!ELSEIF  "$(CFG)" == "MaxDTXLoader - Win32 Release MAX40"

# ADD BASE CPP /D "DIRECTEDITOR_BUILD" /D "STDLITH_MFC" /D "BDEFS_MFC" /D "PRE_FLOAT" /D "PREPROCESSOR_BUILD" /D "NO_PRAGMA_LIBS" /D "LT15_COMPAT"
# ADD CPP /D "DIRECTEDITOR_BUILD" /D "STDLITH_MFC" /D "BDEFS_MFC" /D "PRE_FLOAT" /D "PREPROCESSOR_BUILD" /D "NO_PRAGMA_LIBS" /D "LT15_COMPAT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\shared\engine\streamsim.cpp

!IF  "$(CFG)" == "MaxDTXLoader - Win32 Debug MAX30"

!ELSEIF  "$(CFG)" == "MaxDTXLoader - Win32 Release MAX30"

# ADD CPP /D "DIRECTEDITOR_BUILD" /D "STDLITH_MFC" /D "BDEFS_MFC" /D "PRE_FLOAT" /D "PREPROCESSOR_BUILD" /D "NO_PRAGMA_LIBS" /D "LT15_COMPAT"

!ELSEIF  "$(CFG)" == "MaxDTXLoader - Win32 Debug MAX40"

!ELSEIF  "$(CFG)" == "MaxDTXLoader - Win32 Release MAX40"

# ADD BASE CPP /D "DIRECTEDITOR_BUILD" /D "STDLITH_MFC" /D "BDEFS_MFC" /D "PRE_FLOAT" /D "PREPROCESSOR_BUILD" /D "NO_PRAGMA_LIBS" /D "LT15_COMPAT"
# ADD CPP /D "DIRECTEDITOR_BUILD" /D "STDLITH_MFC" /D "BDEFS_MFC" /D "PRE_FLOAT" /D "PREPROCESSOR_BUILD" /D "NO_PRAGMA_LIBS" /D "LT15_COMPAT"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\MaxDTXLoader.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\MaxDTXLoader.rc
# End Source File
# End Group
# Begin Group "libs_debug"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\Libs\built\Debug\Lib_StdLith\Lib_StdLith.lib

!IF  "$(CFG)" == "MaxDTXLoader - Win32 Debug MAX30"

!ELSEIF  "$(CFG)" == "MaxDTXLoader - Win32 Release MAX30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MaxDTXLoader - Win32 Debug MAX40"

!ELSEIF  "$(CFG)" == "MaxDTXLoader - Win32 Release MAX40"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "libs_release"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\Libs\built\Release\Lib_StdLith\Lib_StdLith.lib

!IF  "$(CFG)" == "MaxDTXLoader - Win32 Debug MAX30"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MaxDTXLoader - Win32 Release MAX30"

!ELSEIF  "$(CFG)" == "MaxDTXLoader - Win32 Debug MAX40"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MaxDTXLoader - Win32 Release MAX40"

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=..\readme.txt
# End Source File
# End Target
# End Project
