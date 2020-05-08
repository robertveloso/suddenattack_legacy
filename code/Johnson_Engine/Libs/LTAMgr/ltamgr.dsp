# Microsoft Developer Studio Project File - Name="ltamgr" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=ltamgr - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ltamgr.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ltamgr.mak" CFG="ltamgr - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ltamgr - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ltamgr - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
RSC=rc.exe

!IF  "$(CFG)" == "ltamgr - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Built\Release\libs\LTAMgr"
# PROP Intermediate_Dir "..\..\Built\Release\libs\LTAMgr"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\..\sdk\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "ltamgr - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\Built\Debug\libs\LTAMgr"
# PROP Intermediate_Dir "..\..\Built\Debug\libs\LTAMgr"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\..\sdk\inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "ltamgr - Win32 Release"
# Name "ltamgr - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ltabitfile.cpp
# End Source File
# Begin Source File

SOURCE=.\ltacompressedfile.cpp
# End Source File
# Begin Source File

SOURCE=.\ltaconverter.cpp
# End Source File
# Begin Source File

SOURCE=.\ltafile.cpp
# End Source File
# Begin Source File

SOURCE=.\ltafilebuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\ltahuffmantree.cpp
# End Source File
# Begin Source File

SOURCE=.\ltaloadonlyalloc.cpp
# End Source File
# Begin Source File

SOURCE=.\ltanode.cpp
# End Source File
# Begin Source File

SOURCE=.\ltanodebuilder.cpp
# End Source File
# Begin Source File

SOURCE=.\ltanodeiterator.cpp
# End Source File
# Begin Source File

SOURCE=.\ltanodewriter.cpp
# End Source File
# Begin Source File

SOURCE=.\ltanodreader.cpp
# End Source File
# Begin Source File

SOURCE=.\ltareader.cpp
# End Source File
# Begin Source File

SOURCE=.\ltautil.cpp
# End Source File
# Begin Source File

SOURCE=.\ltawriter.cpp
# End Source File
# Begin Source File

SOURCE=.\lzsswindow.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\iltaallocator.h
# End Source File
# Begin Source File

SOURCE=.\ltabitfile.h
# End Source File
# Begin Source File

SOURCE=.\ltacompressedfile.h
# End Source File
# Begin Source File

SOURCE=.\ltaconverter.h
# End Source File
# Begin Source File

SOURCE=.\ltadefaultalloc.h
# End Source File
# Begin Source File

SOURCE=.\ltafile.h
# End Source File
# Begin Source File

SOURCE=.\ltafilebuffer.h
# End Source File
# Begin Source File

SOURCE=.\ltahuffmantree.h
# End Source File
# Begin Source File

SOURCE=.\ltalimits.h
# End Source File
# Begin Source File

SOURCE=.\ltaloadonlyalloc.h
# End Source File
# Begin Source File

SOURCE=.\ltamgr.h
# End Source File
# Begin Source File

SOURCE=.\ltanode.h
# End Source File
# Begin Source File

SOURCE=.\ltanodebuilder.h
# End Source File
# Begin Source File

SOURCE=.\ltanodeiterator.h
# End Source File
# Begin Source File

SOURCE=.\ltanodereader.h
# End Source File
# Begin Source File

SOURCE=.\ltanodewriter.h
# End Source File
# Begin Source File

SOURCE=.\ltareader.h
# End Source File
# Begin Source File

SOURCE=.\ltautil.h
# End Source File
# Begin Source File

SOURCE=.\ltawriter.h
# End Source File
# Begin Source File

SOURCE=.\lzsslimits.h
# End Source File
# Begin Source File

SOURCE=.\lzsswindow.h
# End Source File
# End Group
# End Target
# End Project
