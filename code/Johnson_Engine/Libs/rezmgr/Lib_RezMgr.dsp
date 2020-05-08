# Microsoft Developer Studio Project File - Name="Lib_RezMgr" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Lib_RezMgr - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Lib_RezMgr.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Lib_RezMgr.mak" CFG="Lib_RezMgr - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Lib_RezMgr - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Lib_RezMgr - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Lib_RezMgr - Win32 Final" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/SA/Johnson_Code/Johnson_Engine/libs/rezmgr", BRWBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Lib_RezMgr - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\built\Release\libs\Lib_RezMgr"
# PROP Intermediate_Dir "..\..\built\Release\libs\Lib_RezMgr"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /Ob2 /I "..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\..\..\libs\lith" /I "..\..\sdk\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "NO_PROFILE" /D "NOT_USING_FULL" /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\built\Release\libs\Lib_RezMgr.lib"

!ELSEIF  "$(CFG)" == "Lib_RezMgr - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\Johnson_Built\Johnson_Engine\Debug\libs\Lib_RezMgr"
# PROP Intermediate_Dir "..\..\..\Johnson_Built\Johnson_Engine\Debug\libs\Lib_RezMgr"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GX /ZI /Od /I "..\..\..\Johnson_Libs\STLPORT-4.0\STLPORT" /I "..\..\..\Johnson_Libs\lith" /I "..\..\sdk\inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\Lib\Debug\Lib_RezMgr.lib"

!ELSEIF  "$(CFG)" == "Lib_RezMgr - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Final"
# PROP BASE Intermediate_Dir "Final"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\built\Final\libs\Lib_RezMgr"
# PROP Intermediate_Dir "..\..\built\Final\libs\Lib_RezMgr"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\..\..\libs\lith" /I "..\..\sdk\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /Ob2 /I "..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\..\..\libs\lith" /I "..\..\sdk\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_FINAL" /D "NO_PROFILE" /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\built\Final\libs\Lib_RezMgr.lib"

!ENDIF 

# Begin Target

# Name "Lib_RezMgr - Win32 Release"
# Name "Lib_RezMgr - Win32 Debug"
# Name "Lib_RezMgr - Win32 Final"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\rezfile.cpp
# End Source File
# Begin Source File

SOURCE=.\rezhash.cpp
# End Source File
# Begin Source File

SOURCE=.\rezmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\rezutils.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\rezfile.h
# End Source File
# Begin Source File

SOURCE=.\rezhash.h
# End Source File
# Begin Source File

SOURCE=.\rezmgr.h
# End Source File
# Begin Source File

SOURCE=.\reztypes.h
# End Source File
# Begin Source File

SOURCE=.\rezutils.h
# End Source File
# End Group
# End Target
# End Project
