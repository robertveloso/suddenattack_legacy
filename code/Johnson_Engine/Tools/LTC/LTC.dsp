# Microsoft Developer Studio Project File - Name="LTC" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=LTC - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LTC.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LTC.mak" CFG="LTC - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LTC - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "LTC - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
RSC=rc.exe

!IF  "$(CFG)" == "LTC - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\built\Release\tools\LTC"
# PROP Intermediate_Dir "..\..\built\Release\tools\LTC"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\Libs\STLPORT-4.0\STLPORT" /I "..\..\sdk\inc" /I "..\..\libs\ltamgr" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# Begin Custom Build
TargetPath=\Proj\TO2\Source\Jupiter\built\Release\tools\LTC\LTC.exe
InputPath=\Proj\TO2\Source\Jupiter\built\Release\tools\LTC\LTC.exe
SOURCE="$(InputPath)"

"..\..\..\development\tools\LTC.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\development\tools
	copy $(TargetPath) ..\..\..\development\tools

# End Custom Build

!ELSEIF  "$(CFG)" == "LTC - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\built\debug\tools\LTC"
# PROP Intermediate_Dir "..\..\built\debug\tools\LTC"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\Libs\STLPORT-4.0\STLPORT" /I "..\..\sdk\inc" /I "..\..\libs\ltamgr" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"libcmt.lib" /pdbtype:sept
# SUBTRACT LINK32 /nodefaultlib
# Begin Custom Build
TargetPath=\Proj\TO2\Source\Jupiter\built\debug\tools\LTC\LTC.exe
InputPath=\Proj\TO2\Source\Jupiter\built\debug\tools\LTC\LTC.exe
SOURCE="$(InputPath)"

"..\..\..\development\tools\LTC.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\development\tools
	copy $(TargetPath) ..\..\..\development\tools

# End Custom Build

!ENDIF 

# Begin Target

# Name "LTC - Win32 Release"
# Name "LTC - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\commandlineparser.cpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\commandlineparser.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\ltamgr\ltabitfile.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\ltamgr\ltacompressedfile.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\ltamgr\ltafile.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\ltamgr\ltafilebuffer.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\ltamgr\lzsswindow.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "libs_debug"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\built\Debug\libs\LTAMgr\ltamgr.lib

!IF  "$(CFG)" == "LTC - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "LTC - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "libs_release"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\built\Release\libs\LTAMgr\ltamgr.lib

!IF  "$(CFG)" == "LTC - Win32 Release"

!ELSEIF  "$(CFG)" == "LTC - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
