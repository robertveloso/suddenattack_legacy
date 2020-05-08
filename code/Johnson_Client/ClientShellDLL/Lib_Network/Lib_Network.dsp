# Microsoft Developer Studio Project File - Name="Lib_Network" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Lib_Network - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Lib_Network.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Lib_Network.mak" CFG="Lib_Network - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Lib_Network - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Lib_Network - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Lib_Network - Win32 Final" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/SA/Johnson_Code/Johnson_Client/ClientShellDLL/Lib_Network", LTSBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Lib_Network - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Built\Release\ClientShellDLL\Network"
# PROP Intermediate_Dir "..\..\Built\Release\ClientShellDLL\Network"
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

!ELSEIF  "$(CFG)" == "Lib_Network - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\Johnson_Built\Johnson_Client\Debug\ClientShellDLL\Network"
# PROP Intermediate_Dir "..\..\..\Johnson_Built\Johnson_Client\Debug\ClientShellDLL\Network"
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
# ADD LIB32 /nologo /out:"..\..\..\Lib\Debug\Lib_Network.lib"

!ELSEIF  "$(CFG)" == "Lib_Network - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Lib_Network___Win32_Final"
# PROP BASE Intermediate_Dir "Lib_Network___Win32_Final"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Built\Final\ClientShellDLL\Network"
# PROP Intermediate_Dir "..\..\Built\Final\ClientShellDLL\Network"
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

# Name "Lib_Network - Win32 Release"
# Name "Lib_Network - Win32 Debug"
# Name "Lib_Network - Win32 Final"
# Begin Group "Root"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Etc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\NJean\Session\AliveManager.h
# End Source File
# Begin Source File

SOURCE=.\ClientGate.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientGate.h
# End Source File
# Begin Source File

SOURCE=.\NJean\Network\CommSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\NJean\Network\CommSocket.h
# End Source File
# Begin Source File

SOURCE=.\NJean\Session\HttpResponser.cpp
# End Source File
# Begin Source File

SOURCE=.\NJean\Session\HttpResponser.h
# End Source File
# Begin Source File

SOURCE=.\DataStructure\InnerManager.h
# End Source File
# Begin Source File

SOURCE=.\DataStructure\InnerObject.h
# End Source File
# Begin Source File

SOURCE=.\DataStructure\List.h
# End Source File
# Begin Source File

SOURCE=.\NJean\Network\ListenSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\NJean\Network\ListenSocket.h
# End Source File
# Begin Source File

SOURCE=.\DataStructure\MemoryPool.h
# End Source File
# Begin Source File

SOURCE=.\DataStructure\MemoryPoolSafty.h
# End Source File
# Begin Source File

SOURCE=.\NJean\Session\Message.cpp
# End Source File
# Begin Source File

SOURCE=.\NJean\Session\Message.h
# End Source File
# Begin Source File

SOURCE=.\NJean\Session\MessageQueue.cpp
# End Source File
# Begin Source File

SOURCE=.\NJean\Session\MessageQueue.h
# End Source File
# Begin Source File

SOURCE=.\NJean\Network\Network.h
# End Source File
# Begin Source File

SOURCE=.\NJean\Application\Packet.h
# End Source File
# Begin Source File

SOURCE=.\NJean\Session\PacketAssembler.cpp
# End Source File
# Begin Source File

SOURCE=.\NJean\Session\PacketAssembler.h
# End Source File
# Begin Source File

SOURCE=.\NJean\Session\PacketBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\NJean\Session\PacketBuffer.h
# End Source File
# Begin Source File

SOURCE=.\NJean\Session\PacketCheckSum.h
# End Source File
# Begin Source File

SOURCE=.\NJean\Session\PacketMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\NJean\Session\PacketMessage.h
# End Source File
# Begin Source File

SOURCE=.\NJean\Application\Peer.cpp
# End Source File
# Begin Source File

SOURCE=.\NJean\Application\Peer.h
# End Source File
# Begin Source File

SOURCE=.\NJean\Session\QueryBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\NJean\Session\QueryBuffer.h
# End Source File
# Begin Source File

SOURCE=.\NJean\Session\QueryMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\NJean\Session\QueryMessage.h
# End Source File
# Begin Source File

SOURCE=.\NJean\Session\Responser.cpp
# End Source File
# Begin Source File

SOURCE=.\NJean\Session\Responser.h
# End Source File
# Begin Source File

SOURCE=.\NJean\Session\SendPacket.cpp
# End Source File
# Begin Source File

SOURCE=.\NJean\Session\SendPacket.h
# End Source File
# Begin Source File

SOURCE=.\NJean\Session\SessionGate.cpp
# End Source File
# Begin Source File

SOURCE=.\NJean\Session\SessionGate.h
# End Source File
# Begin Source File

SOURCE=.\NJean\Session\SessionManager.cpp
# End Source File
# Begin Source File

SOURCE=.\NJean\Session\SessionManager.h
# End Source File
# Begin Source File

SOURCE=.\NJean\Session\SignalMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\NJean\Session\SignalMessage.h
# End Source File
# Begin Source File

SOURCE=.\NJean\Session\SignalQueue.cpp
# End Source File
# Begin Source File

SOURCE=.\NJean\Session\SignalQueue.h
# End Source File
# Begin Source File

SOURCE=.\NJean\Network\Socket.h
# End Source File
# Begin Source File

SOURCE=.\DataStructure\SortList.h
# End Source File
# Begin Source File

SOURCE=.\NJean\Network\TcpAsyncSelect.cpp
# End Source File
# Begin Source File

SOURCE=.\NJean\Network\TcpAsyncSelect.h
# End Source File
# Begin Source File

SOURCE=.\NJean\Session\Timer.cpp
# End Source File
# Begin Source File

SOURCE=.\NJean\Session\Timer.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Readme.txt
# End Source File
# End Target
# End Project
