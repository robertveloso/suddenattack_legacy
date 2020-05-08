# Microsoft Developer Studio Project File - Name="ltmem" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=ltmem - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ltmem.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ltmem.mak" CFG="ltmem - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ltmem - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ltmem - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "ltmem - Win32 Final" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/SA/Johnson_Code/Johnson_Engine/libs/ltmem", KQWBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ltmem - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\built\Release\libs\Lib_ltmem"
# PROP Intermediate_Dir "..\..\built\Release\libs\Lib_ltmem"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /GX /Zi /O2 /Ob2 /I "..\..\..\libs" /I "..\..\sdk\inc" /I "..\..\runtime\shared\src" /I "..\..\runtime\kernel\src" /I "..\..\..\libs\stdlith" /I "..\..\runtime\kernel\mem\src" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "NO_PROFILE" /D "NOT_USING_FULL" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\built\Release\libs\ltmem.lib"

!ELSEIF  "$(CFG)" == "ltmem - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\Johnson_Built\Johnson_Engine\Debug\libs\Lib_ltmem"
# PROP Intermediate_Dir "..\..\..\Johnson_Built\Johnson_Engine\Debug\libs\Lib_ltmem"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\Johnson_Libs\stdlith" /I "..\..\sdk\inc" /I "..\..\runtime\shared\src" /I "..\..\runtime\kernel\src" /I "..\..\runtime\kernel\mem\src" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\Lib\Debug\ltmem.lib"

!ELSEIF  "$(CFG)" == "ltmem - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Final"
# PROP BASE Intermediate_Dir "Final"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\built\Final\libs\Lib_ltmem"
# PROP Intermediate_Dir "..\..\built\Final\libs\Lib_ltmem"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "..\..\..\libs" /I "..\..\sdk\inc" /I "..\..\runtime\shared\src" /I "..\..\runtime\kernel\src" /I "..\..\..\libs\stdlith" /I "..\..\runtime\kernel\mem\src" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /Ob2 /I "..\..\..\libs" /I "..\..\sdk\inc" /I "..\..\runtime\shared\src" /I "..\..\runtime\kernel\src" /I "..\..\..\libs\stdlith" /I "..\..\runtime\kernel\mem\src" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_FINAL" /D "NO_PROFILE" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\built\Final\libs\ltmem.lib"

!ENDIF 

# Begin Target

# Name "ltmem - Win32 Release"
# Name "ltmem - Win32 Debug"
# Name "ltmem - Win32 Final"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ltmem.cpp
# End Source File
# Begin Source File

SOURCE=.\ltmemdebug.cpp
# End Source File
# Begin Source File

SOURCE=.\ltmemheap.cpp
# End Source File
# Begin Source File

SOURCE=.\ltmemstats.cpp
# End Source File
# Begin Source File

SOURCE=.\ltmemtrack.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\generalheap.h
# End Source File
# Begin Source File

SOURCE=.\generalheapgroup.h
# End Source File
# Begin Source File

SOURCE=.\lilfixedheap.h
# End Source File
# Begin Source File

SOURCE=.\lilfixedheapgroup.h
# End Source File
# Begin Source File

SOURCE=..\..\sdk\inc\ltmem.h
# End Source File
# Begin Source File

SOURCE=.\ltmemdebug.h
# End Source File
# Begin Source File

SOURCE=.\ltmemheap.h
# End Source File
# Begin Source File

SOURCE=.\ltmemtrack.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Readme.txt
# End Source File
# End Target
# End Project
