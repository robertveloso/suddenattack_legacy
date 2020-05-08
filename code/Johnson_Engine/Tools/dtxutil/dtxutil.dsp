# Microsoft Developer Studio Project File - Name="dtxutil" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=dtxutil - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dtxutil.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dtxutil.mak" CFG="dtxutil - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dtxutil - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "dtxutil - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dtxutil - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\built\Release\tools\dtxutil"
# PROP Intermediate_Dir "..\..\built\Release\tools\dtxutil"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /I "..\..\..\Libs\STLPORT-4.0\STLPORT" /I "..\shared\engine" /I "..\..\..\libs\stdlith" /I "..\..\sdk\inc" /I "..\preprocessor" /I "..\shared\world" /I "..\dedit\misc" /D "_AFXDLL" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "DIRECTEDITOR_BUILD" /D "STDLITH_MFC" /D "BDEFS_MFC" /D "PRE_FLOAT" /D "PREPROCESSOR_BUILD" /D "NO_PRAGMA_LIBS" /D "LT15_COMPAT" /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"..\..\built\Release\tools\dxutil\dtxutil.exe"
# Begin Custom Build
TargetPath=\Working\Engine\built\Release\tools\dxutil\dtxutil.exe
InputPath=\Working\Engine\built\Release\tools\dxutil\dtxutil.exe
SOURCE="$(InputPath)"

"..\..\..\development\tools\dxutil.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\development\tools 
	copy $(TargetPath) ..\..\..\development\tools 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "dtxutil - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\built\debug\tools\dtxutil"
# PROP Intermediate_Dir "..\..\built\debug\tools\dtxutil"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\Libs\STLPORT-4.0\STLPORT" /I "..\shared\engine" /I "..\..\..\libs\stdlith" /I "..\..\sdk\inc" /I "..\preprocessor" /I "..\shared\world" /I "..\dedit\misc" /D "_AFXDLL" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "DIRECTEDITOR_BUILD" /D "STDLITH_MFC" /D "BDEFS_MFC" /D "PRE_FLOAT" /D "PREPROCESSOR_BUILD" /D "NO_PRAGMA_LIBS" /D "LT15_COMPAT" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"..\..\built\debug\tools\dxutil\dtxutil.exe" /pdbtype:sept
# Begin Custom Build
TargetPath=\Working\Engine\built\debug\tools\dxutil\dtxutil.exe
InputPath=\Working\Engine\built\debug\tools\dxutil\dtxutil.exe
SOURCE="$(InputPath)"

"..\..\..\development\tools\dxutil.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\development\tools 
	copy $(TargetPath) ..\..\..\development\tools 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "dtxutil - Win32 Release"
# Name "dtxutil - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\shared\engine\dtxmgr.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\engine\genltstream.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\engine\load_pcx.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\world\LTTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp

!IF  "$(CFG)" == "dtxutil - Win32 Release"

# SUBTRACT CPP /D "NDEBUG"

!ELSEIF  "$(CFG)" == "dtxutil - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\shared\engine\pixelformat.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\engine\streamsim.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "libs_debug"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Libs\built\Debug\Lib_StdLith\Lib_StdLith.lib

!IF  "$(CFG)" == "dtxutil - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "dtxutil - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "libs_release"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Libs\built\Release\Lib_StdLith\Lib_StdLith.lib

!IF  "$(CFG)" == "dtxutil - Win32 Release"

!ELSEIF  "$(CFG)" == "dtxutil - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
