# Microsoft Developer Studio Project File - Name="Model_Packer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=Model_Packer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Model_Packer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Model_Packer.mak" CFG="Model_Packer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Model_Packer - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Model_Packer - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Model_Packer - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\built\Release\tools\Model_Packer"
# PROP Intermediate_Dir "..\..\built\Release\tools\Model_Packer"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\..\..\libs\stdlith" /I "..\shared\engine" /I "..\ModelEdit\\" /I "..\shared\model" /I "." /I "..\..\ModelEdit" /I "..\..\sdk\inc" /I "..\..\sdk\inc\compat" /I "..\..\libs\ltamgr" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_CONSOLE" /D "BDEFS_WINDOWS_H" /D "LTA2LTB_D3D" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib shell32.lib d3dx8.lib /nologo /subsystem:console /machine:I386
# Begin Custom Build
TargetPath=\Working\Engine\built\Release\tools\Model_Packer\Model_Packer.exe
InputPath=\Working\Engine\built\Release\tools\Model_Packer\Model_Packer.exe
SOURCE="$(InputPath)"

"..\..\..\development\tools\Model_Packer.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\development\tools 
	copy $(TargetPath) ..\..\..\development\tools 
	copy $(TargetPath) c:\bin 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Model_Packer - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\built\debug\tools\Model_Packer"
# PROP Intermediate_Dir "..\..\built\debug\tools\Model_Packer"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\..\..\libs\stdlith" /I "..\shared\engine" /I "..\ModelEdit\\" /I "..\shared\model" /I "." /I "..\..\ModelEdit" /I "..\..\sdk\inc" /I "..\..\sdk\inc\compat" /I "..\..\libs\ltamgr" /D "_DEBUG" /D "_CONSOLE" /D "BDEFS_WINDOWS_H" /D "LTA2LTB_D3D" /FR /YX /FD /I /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib shell32.lib d3dx8.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# Begin Custom Build
TargetPath=\Working\Engine\built\debug\tools\Model_Packer\Model_Packer.exe
InputPath=\Working\Engine\built\debug\tools\Model_Packer\Model_Packer.exe
SOURCE="$(InputPath)"

"..\..\..\development\tools\Model_Packer.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\development\tools 
	copy $(TargetPath) ..\..\..\development\tools 
	copy $(TargetPath) c:\bin 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "Model_Packer - Win32 Release"
# Name "Model_Packer - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\shared\engine\geomroutines.cpp
# End Source File
# Begin Source File

SOURCE=.\lta2ltb_d3d.cpp
# End Source File
# Begin Source File

SOURCE=..\..\sdk\inc\ltquatbase.cpp
# End Source File
# Begin Source File

SOURCE=.\manifold.cpp

!IF  "$(CFG)" == "Model_Packer - Win32 Release"

!ELSEIF  "$(CFG)" == "Model_Packer - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Model_Packer.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\commandline_parser.h
# End Source File
# Begin Source File

SOURCE=.\lta2ltb_d3d.h
# End Source File
# Begin Source File

SOURCE=.\ltb_file.h
# End Source File
# Begin Source File

SOURCE=.\manifold.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\Model_Packer.rc
# End Source File
# End Group
# Begin Group "lta"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\shared\model\ltaSave.cpp
# End Source File
# End Group
# Begin Group "model"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\shared\model\ltaModel.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\model\ltaScene.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\model\model.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\model\model_cleanup.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\model\model_ops.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\model\model_save.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\model\modelallocations.cpp
# End Source File
# Begin Source File

SOURCE=..\ModelEdit\newgenlod.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\model\transformmaker.cpp
# End Source File
# End Group
# Begin Group "misc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\shared\engine\conparse.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\engine\genltstream.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\engine\nexus.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\engine\streamsim.cpp
# End Source File
# End Group
# Begin Group "libs_debug"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\built\Debug\libs\LTAMgr\ltamgr.lib

!IF  "$(CFG)" == "Model_Packer - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Model_Packer - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Libs\built\Debug\Lib_StdLith\Lib_StdLith.lib

!IF  "$(CFG)" == "Model_Packer - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Model_Packer - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "libs_release"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\built\Release\libs\LTAMgr\ltamgr.lib

!IF  "$(CFG)" == "Model_Packer - Win32 Release"

!ELSEIF  "$(CFG)" == "Model_Packer - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Libs\built\Release\Lib_StdLith\Lib_StdLith.lib

!IF  "$(CFG)" == "Model_Packer - Win32 Release"

!ELSEIF  "$(CFG)" == "Model_Packer - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=..\lta2abcpc\ReadMe.txt
# End Source File
# End Target
# End Project
