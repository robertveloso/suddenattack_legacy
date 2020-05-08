# Microsoft Developer Studio Project File - Name="Lib_Packet" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Lib_Packet - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Lib_Packet.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Lib_Packet.mak" CFG="Lib_Packet - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Lib_Packet - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Lib_Packet - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Lib_Packet - Win32 Final" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/SA/Johnson_Code/Johnson_Client/ClientShellDLL/Lib_Packet", TVSBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Lib_Packet - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Built\Release\ClientShellDLL\Packet"
# PROP Intermediate_Dir "..\..\Built\Release\ClientShellDLL\Packet"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /Zi /O2 /I ".\\" /I "..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\..\clientres\shared" /I "..\..\shared" /I "..\..\..\Engine\sdk\inc" /I "..\..\..\libs\stdlith" /I "..\..\..\libs\butemgr" /I "..\..\..\libs\cryptmgr" /I "..\..\..\libs\lith" /I "..\..\..\libs\mfcstub" /I "..\..\libs\ltguimgr" /I "..\..\..\libs\regmgr" /I "..\\" /I "..\..\libs\serverdir" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Lib_Packet - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\Johnson_Built\Johnson_Client\Debug\ClientShellDLL\Packet"
# PROP Intermediate_Dir "..\..\..\Johnson_Built\Johnson_Client\Debug\ClientShellDLL\Packet"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\Lib\Debug\Lib_Packet.lib"

!ELSEIF  "$(CFG)" == "Lib_Packet - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Lib_Packet___Win32_Final"
# PROP BASE Intermediate_Dir "Lib_Packet___Win32_Final"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Built\Final\ClientShellDLL\Packet"
# PROP Intermediate_Dir "..\..\Built\Final\ClientShellDLL\Packet"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /Zi /O2 /I ".\\" /I "..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\..\clientres\shared" /I "..\..\shared" /I "..\..\..\Engine\sdk\inc" /I "..\..\..\libs\stdlith" /I "..\..\..\libs\butemgr" /I "..\..\..\libs\cryptmgr" /I "..\..\..\libs\lith" /I "..\..\..\libs\mfcstub" /I "..\..\libs\ltguimgr" /I "..\..\..\libs\regmgr" /I "..\\" /I "..\..\libs\serverdir" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_FINAL" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "Lib_Packet - Win32 Release"
# Name "Lib_Packet - Win32 Debug"
# Name "Lib_Packet - Win32 Final"
# Begin Group "Root"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Service"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Service\Channel.h
# End Source File
# Begin Source File

SOURCE=.\Service\Game.h
# End Source File
# Begin Source File

SOURCE=.\Service\Item.h
# End Source File
# Begin Source File

SOURCE=.\Service\Play.h
# End Source File
# Begin Source File

SOURCE=.\Service\Player.h
# End Source File
# Begin Source File

SOURCE=.\Service\Room.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Builder.cpp
# End Source File
# Begin Source File

SOURCE=.\Builder.h
# End Source File
# Begin Source File

SOURCE=.\Login.h
# End Source File
# Begin Source File

SOURCE=.\Manager.h
# End Source File
# Begin Source File

SOURCE=.\Readme.txt
# End Source File
# End Target
# End Project
