# Microsoft Developer Studio Project File - Name="RenderStyle_PackerC" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=RenderStyle_PackerC - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "rspack.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "rspack.mak" CFG="RenderStyle_PackerC - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "RenderStyle_PackerC - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "RenderStyle_PackerC - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "RenderStyle_PackerC - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /O2 /I "..\..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\..\..\..\libs\stdlith" /I "..\..\..\sdk\inc" /I "..\..\..\libs\ltamgr" /I "..\\" /I "..\..\shared\model" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# Begin Custom Build
TargetPath=.\Release\rspack.exe
InputPath=.\Release\rspack.exe
SOURCE="$(InputPath)"

"..\..\..\..\development\tools" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\..\development\tools 
	copy $(TargetPath) ..\..\..\..\development\tools 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "RenderStyle_PackerC - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\..\..\proj\Debug\tools\RenderStyle_PackerC"
# PROP Intermediate_Dir "..\..\..\..\..\proj\Debug\tools\RenderStyle_PackerC"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\..\..\..\libs\stdlith" /I "..\..\..\sdk\inc" /I "..\..\..\libs\ltamgr" /I "..\\" /I "..\..\shared\model" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 d3dx8.lib gdi32.lib user32.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"libcmtd.lib" /nodefaultlib:"nafxcwd.lib" /out:"..\..\..\..\..\proj\Debug\RenderStyle_PackerC.exe" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build
TargetPath=\proj\proj\Debug\RenderStyle_PackerC.exe
InputPath=\proj\proj\Debug\RenderStyle_PackerC.exe
SOURCE="$(InputPath)"

"..\..\..\..\development\tools" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\..\development\tools 
	copy $(TargetPath) ..\..\..\..\development\tools 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "RenderStyle_PackerC - Win32 Release"
# Name "RenderStyle_PackerC - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\d3d_renderstyle.cpp
# End Source File
# Begin Source File

SOURCE=..\RenderStyle_LTA_Loader.cpp
# End Source File
# Begin Source File

SOURCE=..\renderstylepacker.cpp
# End Source File
# Begin Source File

SOURCE=.\rspack.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "libraries[RELEASE]"

# PROP Default_Filter ".lib"
# Begin Source File

SOURCE=..\..\..\built\Release\libs\LTAMgr\ltamgr.lib

!IF  "$(CFG)" == "RenderStyle_PackerC - Win32 Release"

!ELSEIF  "$(CFG)" == "RenderStyle_PackerC - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\libs\built\Release\Lib_StdLith\Lib_StdLith.lib

!IF  "$(CFG)" == "RenderStyle_PackerC - Win32 Release"

!ELSEIF  "$(CFG)" == "RenderStyle_PackerC - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\..\Program Files\Microsoft Visual Studio\VC98\Lib\ADVAPI32.LIB"
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\DXSDK81\lib\d3dx8.lib
# End Source File
# End Group
# Begin Group "libraries[DEBUG]"

# PROP Default_Filter ".lib"
# Begin Source File

SOURCE=..\..\..\built\Debug\libs\LTAMgr\ltamgr.lib

!IF  "$(CFG)" == "RenderStyle_PackerC - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "RenderStyle_PackerC - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\libs\built\Debug\Lib_StdLith\Lib_StdLith.lib

!IF  "$(CFG)" == "RenderStyle_PackerC - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "RenderStyle_PackerC - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
