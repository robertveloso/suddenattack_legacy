# Microsoft Developer Studio Project File - Name="DLL_Server" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=DLL_Server - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DLL_Server.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DLL_Server.mak" CFG="DLL_Server - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DLL_Server - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DLL_Server - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DLL_Server - Win32 Final" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/SA/Johnson_Code/Johnson_Engine/Runtime/winbuild/server", ZGYBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DLL_Server - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\..\built\Debug\Runtime\DLL_Server"
# PROP BASE Intermediate_Dir "..\..\..\built\Debug\Runtime\DLL_Server"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\..\Johnson_Built\Johnson_Engine\Debug\Runtime\DLL_Server"
# PROP Intermediate_Dir "..\..\..\..\Johnson_Built\Johnson_Engine\Debug\Runtime\DLL_Server"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /ZI /Od /I "..\..\..\..\Johnson_Libs\STLPORT-4.0\STLPORT" /I "..\..\server\src" /I "..\..\physics\src" /I "..\..\client\src" /I "..\..\sound\src" /I "..\..\kernel\src" /I "..\..\shared\src\sys\win" /I "..\..\kernel\mem\src" /I "..\..\kernel\io\src" /I "..\..\kernel\src\sys\win" /I "..\..\kernel\net\src" /I "..\..\..\sdk\inc" /I "..\..\..\sdk\inc\compat" /I "..\..\Shared\src" /I "..\..\Model\src" /I "." /I "..\..\kernel\net" /I "..\..\World\src" /I "..\..\..\..\Johnson_Libs\stdlith" /I "..\..\..\libs\rezmgr" /I "..\..\..\..\Johnson_Libs\lith" /I "..\..\crtcompat" /I "..\..\mpm\src" /I "..\..\comm\src" /I "..\..\lithtemplate" /I "..\..\distrobj\src" /I "..\..\..\sdk\inc\autoview" /I "..\..\..\sdk\inc\physics" /I "..\..\..\sdk\inc\state_mgr" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "DE_SERVER_COMPILE" /D "DIRECTENGINE_COMPILE" /D "_MBCS" /D "STDLITH_ALLOC_OVERRIDE" /D "LT15_COMPAT" /D "NO_PRAGMA_LIBS" /D "__D3D" /D "__D3DREND" /FR /Yu"bdefs.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib dxguid.lib dplayx.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib LIB_Lith.lib Lib_StdLith.lib Lib_RezMgr.lib lib_info.lib /nologo /subsystem:windows /dll /profile /debug /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libc.lib" /out:"..\..\..\..\Lib\Debug\server.dll" /libpath:"..\..\..\..\Lib\Debug"
# SUBTRACT LINK32 /verbose /force
# Begin Custom Build
TargetPath=\SA\Johnson_Code\Lib\Debug\server.dll
InputPath=\SA\Johnson_Code\Lib\Debug\server.dll
SOURCE="$(InputPath)"

"$(SA_MAIN_DIR)\server.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	Rem IncrediBuild_OutputFile $(TargetPath) 
	echo copy $(TargetPath) $(SA_MAIN_DIR) 
	copy $(TargetPath) $(SA_MAIN_DIR) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "DLL_Server - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\..\built\Release\Runtime\DLL_Server"
# PROP BASE Intermediate_Dir "..\..\..\built\Release\Runtime\DLL_Server"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\built\Release\Runtime\DLL_Server"
# PROP Intermediate_Dir "..\..\..\built\Release\Runtime\DLL_Server"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /O1 /I "..\..\server\src" /I "..\..\kernel\src\sys\win" /I "..\..\kernel\net\src" /I "..\..\..\sdk\inc" /I "..\..\..\sdk\inc\compat" /I "..\..\Shared\src" /I "..\..\Model\src" /I "." /I "..\..\kernel\net" /I "..\..\World\src" /I "..\..\..\..\libs\incs" /D "NDEBUG" /D "_ATL_STATIC_REGISTRY" /D "_ATL_MIN_CRT" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "DE_SERVER_COMPILE" /D "DIRECTENGINE_COMPILE" /D "_MBCS" /D "STDLITH_ALLOC_OVERRIDE" /D "LT15_COMPAT" /D "NO_PRAGMA_LIBS" /Yu"bdefs.h" /FD /c
# ADD CPP /nologo /MD /W3 /Zi /O1 /I "..\..\..\..\Libs\STLPORT-4.0\STLPORT" /I "..\..\server\src" /I "..\..\physics\src" /I "..\..\client\src" /I "..\..\sound\src" /I "..\..\kernel\src" /I "..\..\shared\src\sys\win" /I "..\..\kernel\mem\src" /I "..\..\kernel\io\src" /I "..\..\kernel\src\sys\win" /I "..\..\kernel\net\src" /I "..\..\..\sdk\inc" /I "..\..\..\sdk\inc\compat" /I "..\..\Shared\src" /I "..\..\Model\src" /I "." /I "..\..\kernel\net" /I "..\..\World\src" /I "..\..\..\..\libs\stdlith" /I "..\..\..\libs\rezmgr" /I "..\..\..\..\libs\lith" /I "..\..\crtcompat" /I "..\..\mpm\src" /I "..\..\comm\src" /I "..\..\lithtemplate" /I "..\..\distrobj\src" /I "..\..\..\sdk\inc\autoview" /I "..\..\..\sdk\inc\physics" /I "..\..\..\sdk\inc\state_mgr" /D "NDEBUG" /D "_ATL_STATIC_REGISTRY" /D "_ATL_MIN_CRT" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "DE_SERVER_COMPILE" /D "DIRECTENGINE_COMPILE" /D "_MBCS" /D "STDLITH_ALLOC_OVERRIDE" /D "LT15_COMPAT" /D "NO_PRAGMA_LIBS" /D "__D3D" /D "__D3DREND" /D "NO_PROFILE" /D "NOT_USING_FULL" /FR /Yu"bdefs.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 winmm.lib dxguid.lib dplayx.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib:"libcmtd.lib" /force /out:"release\server.dll"
# SUBTRACT BASE LINK32 /verbose
# ADD LINK32 winmm.lib dxguid.lib dplayx.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib LIB_Lith.lib Lib_StdLith.lib Lib_RezMgr.lib lib_info.lib /nologo /subsystem:windows /dll /profile /debug /machine:I386 /nodefaultlib:"libc.lib" /force /out:"..\..\..\built\Release\Runtime\DLL_Server\server.dll" /libpath:"..\..\..\built\Release\libs" /libpath:"..\..\..\..\libs\built\Release"
# SUBTRACT LINK32 /verbose
# Begin Custom Build
TargetPath=\SA\Johnson_Code\Johnson_Engine\built\Release\Runtime\DLL_Server\server.dll
InputPath=\SA\Johnson_Code\Johnson_Engine\built\Release\Runtime\DLL_Server\server.dll
SOURCE="$(InputPath)"

"$(SA_MAIN_DIR)\server.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	Rem IncrediBuild_OutputFile $(TargetPath) 
	echo copy $(TargetPath) $(SA_MAIN_DIR) 
	copy $(TargetPath) $(SA_MAIN_DIR) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "DLL_Server - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\..\built\Final\Runtime\DLL_Server"
# PROP BASE Intermediate_Dir "..\..\..\built\Final\Runtime\DLL_Server"
# PROP BASE Ignore_Export_Lib 1
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\built\Final\Runtime\DLL_Server"
# PROP Intermediate_Dir "..\..\..\built\Final\Runtime\DLL_Server"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /O1 /I "..\..\..\..\Libs\STLPORT-4.0\STLPORT" /I "..\..\server\src" /I "..\..\physics\src" /I "..\..\client\src" /I "..\..\sound\src" /I "..\..\kernel\src" /I "..\..\shared\src\sys\win" /I "..\..\kernel\mem\src" /I "..\..\kernel\io\src" /I "..\..\kernel\src\sys\win" /I "..\..\kernel\net\src" /I "..\..\..\sdk\inc" /I "..\..\..\sdk\inc\compat" /I "..\..\Shared\src" /I "..\..\Model\src" /I "." /I "..\..\kernel\net" /I "..\..\World\src" /I "..\..\..\..\libs\stdlith" /I "..\..\..\libs\rezmgr" /I "..\..\..\..\libs\lith" /I "..\..\crtcompat" /I "..\..\mpm\src" /I "..\..\comm\src" /I "..\..\lithtemplate" /I "..\..\distrobj\src" /I "..\..\..\sdk\inc\autoview" /I "..\..\..\sdk\inc\physics" /I "..\..\..\sdk\inc\state_mgr" /D "NDEBUG" /D "_ATL_STATIC_REGISTRY" /D "_ATL_MIN_CRT" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "DE_SERVER_COMPILE" /D "DIRECTENGINE_COMPILE" /D "_MBCS" /D "STDLITH_ALLOC_OVERRIDE" /D "LT15_COMPAT" /D "NO_PRAGMA_LIBS" /D "__D3D" /D "__D3DREND" /FR /Yu"bdefs.h" /FD /c
# ADD CPP /nologo /MT /W3 /Zi /O1 /I "..\..\..\..\Libs\STLPORT-4.0\STLPORT" /I "..\..\server\src" /I "..\..\physics\src" /I "..\..\client\src" /I "..\..\sound\src" /I "..\..\kernel\src" /I "..\..\shared\src\sys\win" /I "..\..\kernel\mem\src" /I "..\..\kernel\io\src" /I "..\..\kernel\src\sys\win" /I "..\..\kernel\net\src" /I "..\..\..\sdk\inc" /I "..\..\..\sdk\inc\compat" /I "..\..\Shared\src" /I "..\..\Model\src" /I "." /I "..\..\kernel\net" /I "..\..\World\src" /I "..\..\..\..\libs\stdlith" /I "..\..\..\libs\rezmgr" /I "..\..\..\..\libs\lith" /I "..\..\crtcompat" /I "..\..\mpm\src" /I "..\..\comm\src" /I "..\..\lithtemplate" /I "..\..\distrobj\src" /I "..\..\..\sdk\inc\autoview" /I "..\..\..\sdk\inc\physics" /I "..\..\..\sdk\inc\state_mgr" /D "NDEBUG" /D "_ATL_STATIC_REGISTRY" /D "_ATL_MIN_CRT" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "DE_SERVER_COMPILE" /D "DIRECTENGINE_COMPILE" /D "_MBCS" /D "STDLITH_ALLOC_OVERRIDE" /D "LT15_COMPAT" /D "NO_PRAGMA_LIBS" /D "__D3D" /D "__D3DREND" /D "_FINAL" /D "NO_PROFILE" /D "USE_FIRE_GUARD" /FR /Yu"bdefs.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 winmm.lib dxguid.lib dplayx.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib:"libc.lib" /force /out:"..\..\..\built\Release\Runtime\DLL_Server\server.dll"
# SUBTRACT BASE LINK32 /verbose
# ADD LINK32 winmm.lib dxguid.lib dplayx.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib LIB_Lith.lib Lib_StdLith.lib Lib_RezMgr.lib Lib_Info.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libc.lib" /def:".\server.def" /force /out:"..\..\..\built\Final\Runtime\DLL_Server\server.dll" /libpath:"..\..\..\built\final\libs" /libpath:"..\..\..\..\libs\built\final" /Release
# SUBTRACT LINK32 /profile /pdb:none
# Begin Custom Build
TargetPath=\SA\Johnson_Code\Johnson_Engine\built\Final\Runtime\DLL_Server\server.dll
InputPath=\SA\Johnson_Code\Johnson_Engine\built\Final\Runtime\DLL_Server\server.dll
SOURCE="$(InputPath)"

"$(SA_MAIN_DIR)\server.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	Rem IncrediBuild_OutputFile $(TargetPath) 
	echo copy $(TargetPath) $(SA_MAIN_DIR) 
	copy $(TargetPath) $(SA_MAIN_DIR) 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "DLL_Server - Win32 Debug"
# Name "DLL_Server - Win32 Release"
# Name "DLL_Server - Win32 Final"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\model\src\animtracker.cpp
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\bdefs.cpp
# ADD CPP /Yc"bdefs.h"
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\bindmgr.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\classbind.cpp
# End Source File
# Begin Source File

SOURCE=..\..\server\src\classmgr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\collision.cpp
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\compress.cpp
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\concommand.cpp
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\conparse.cpp
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\counter.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\model\src\sys\d3d\d3d_model_load.cpp
# End Source File
# Begin Source File

SOURCE=..\..\kernel\io\src\sys\win\de_file.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\world\src\de_mainworld.cpp
# End Source File
# Begin Source File

SOURCE=..\..\kernel\mem\src\sys\win\de_memory.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\world\src\de_nodes.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\world\src\de_objects.cpp
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\debugging.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\dhashtable.cpp
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\dsys_interface.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\dutil.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\engine_vars.cpp
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\findobj.cpp
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\ftserv.cpp
# End Source File
# Begin Source File

SOURCE=..\..\world\src\fullintersectline.cpp
# End Source File
# Begin Source File

SOURCE=..\..\server\src\game_serialize.cpp
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\gamemath.cpp
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\genericprop_setup.cpp
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\genltstream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\geometry.cpp
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\geomroutines.cpp
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\impl_common.cpp
# End Source File
# Begin Source File

SOURCE=..\..\server\src\interlink.cpp
# End Source File
# Begin Source File

SOURCE=..\..\world\src\intersect_line.cpp
# End Source File
# Begin Source File

SOURCE=..\..\world\src\intersectsweptsphere.cpp
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\leech.cpp
# End Source File
# Begin Source File

SOURCE=..\..\world\src\light_table.cpp
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\lightmap_planes.cpp
# End Source File
# Begin Source File

SOURCE=..\..\kernel\net\src\localdriver.cpp
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\lthread.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\ltmessage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\server\src\ltmessage_server.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\ltmodule.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\ltobjref.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\ltquatbase.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\ltthread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\lttimer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\model\src\model.cpp
# End Source File
# Begin Source File

SOURCE=..\..\model\src\model_load.cpp
# End Source File
# Begin Source File

SOURCE=..\..\model\src\modelallocations.cpp
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\motion.cpp
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\moveobject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\moveplayer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\kernel\net\src\netmgr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\nexus.cpp
# End Source File
# Begin Source File

SOURCE=..\..\kernel\net\src\NFPacketDebug.cpp
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\objectmgr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\kernel\net\src\packet.cpp
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\parse_world_info.cpp
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\ratetracker.cpp
# End Source File
# Begin Source File

SOURCE=..\..\server\src\s_client.cpp
# End Source File
# Begin Source File

SOURCE=..\..\server\src\s_concommand.cpp
# End Source File
# Begin Source File

SOURCE=..\..\server\src\S_Intersect.cpp
# End Source File
# Begin Source File

SOURCE=..\..\server\src\s_net.cpp
# End Source File
# Begin Source File

SOURCE=..\..\server\src\s_object.cpp
# End Source File
# Begin Source File

SOURCE=.\server.def

!IF  "$(CFG)" == "DLL_Server - Win32 Debug"

!ELSEIF  "$(CFG)" == "DLL_Server - Win32 Release"

!ELSEIF  "$(CFG)" == "DLL_Server - Win32 Final"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\server.rc
# End Source File
# Begin Source File

SOURCE=..\..\server\src\server_consolestate.cpp
# End Source File
# Begin Source File

SOURCE=..\..\server\src\server_extradata.cpp
# End Source File
# Begin Source File

SOURCE=..\..\server\src\server_filemgr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\server_interface.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\server\src\server_loaderthread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\server\src\serverde_impl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\server\src\serverevent.cpp
# End Source File
# Begin Source File

SOURCE=..\..\server\src\servermgr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\server\src\sloaderthread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\server\src\smoveabstract.cpp
# End Source File
# Begin Source File

SOURCE=..\..\sound\src\sounddata.cpp

!IF  "$(CFG)" == "DLL_Server - Win32 Debug"

!ELSEIF  "$(CFG)" == "DLL_Server - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "DLL_Server - Win32 Final"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\server\src\soundtrack.cpp
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\spritecontrolimpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\stacktrace.cpp
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\stdlterror.cpp
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\streamsim.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\stringmgr.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\strtools.cpp
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\systeminfo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\timemgr.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\model\src\transformmaker.cpp
# End Source File
# Begin Source File

SOURCE=..\..\kernel\net\src\sys\win\udpdriver.cpp
# End Source File
# Begin Source File

SOURCE=..\..\sound\src\wave.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\world\src\world_blind_object_data.cpp
# End Source File
# Begin Source File

SOURCE=..\..\world\src\world_blocker_data.cpp
# End Source File
# Begin Source File

SOURCE=..\..\world\src\world_blocker_math.cpp
# End Source File
# Begin Source File

SOURCE=..\..\world\src\world_particle_blocker_data.cpp
# End Source File
# Begin Source File

SOURCE=..\..\world\src\world_tree.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=..\..\..\sdk\inc\physics\aabb.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\physics\aabb_tree.h
# End Source File
# Begin Source File

SOURCE=..\..\model\src\animtracker.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\appresource.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\bdefs.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\bindmgr.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\build_options.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\classbind.h
# End Source File
# Begin Source File

SOURCE=..\..\server\src\classmgr.h
# End Source File
# Begin Source File

SOURCE=..\..\client\src\client_filemgr.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\clienthack.h
# End Source File
# Begin Source File

SOURCE=..\..\client\src\clientmgr.h
# End Source File
# Begin Source File

SOURCE=..\..\client\src\clientshell.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\collision.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\physics\collision_data.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\physics\collision_mgr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\physics\collision_object.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\concommand.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\conparse.h
# End Source File
# Begin Source File

SOURCE=..\..\client\src\console.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\physics\coordinate_frame.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\counter.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\sys\win\d3dddstructs.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\io\src\sys\win\de_file.h
# End Source File
# Begin Source File

SOURCE=..\..\world\src\de_mainworld.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\mem\src\de_memory.h
# End Source File
# Begin Source File

SOURCE=..\..\world\src\de_objects.h
# End Source File
# Begin Source File

SOURCE=..\..\world\src\de_sprite.h
# End Source File
# Begin Source File

SOURCE=..\..\world\src\de_world.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\DebugGeometry.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\dhashtable.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\dsys.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\dsys_interface.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\dtxmgr.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\dutil.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\ftbase.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\ftserv.h
# End Source File
# Begin Source File

SOURCE=..\..\world\src\fullintersectline.h
# End Source File
# Begin Source File

SOURCE=..\..\server\src\game_serialize.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\gamemath.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\genericprop_setup.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\genltstream.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\geometry.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\geomroutines.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\iaggregate.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\iclientshell.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\iltbenchmark.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\iltclient.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\iltcsbase.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\iltdirectmusic.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\iltdrawprim.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\iltinfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\iltmessage.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\iltrefcount.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\iltserver.h
# End Source File
# Begin Source File

SOURCE=..\..\sound\src\iltsound.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\iltspritecontrol.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\iltstream.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\ilttexinterface.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\impl_common.h
# End Source File
# Begin Source File

SOURCE=..\..\server\src\interlink.h
# End Source File
# Begin Source File

SOURCE=..\..\world\src\intersect_line.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\iservershell.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\ithread.h
# End Source File
# Begin Source File

SOURCE=..\..\world\src\light_table.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\lightmap_planes.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\listqueue.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\lithtech.h
# End Source File
# Begin Source File

SOURCE=..\..\world\src\loadstatus.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\net\src\localdriver.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\ltanimtracker.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\ltassert.h
# End Source File
# Begin Source File

SOURCE=..\..\model\src\ltb.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\ltbasedefs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\ltbasetypes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\ltcodes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\ltengineobjects.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\lthread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\ltinteger.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\ltlink.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\ltmatrix.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\ltmessage.h
# End Source File
# Begin Source File

SOURCE=..\..\server\src\ltmessage_server.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\ltmodule.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\ltplane.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\ltpvalue.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\ltquatbase.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\ltrect.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\ltrenderstyle.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\ltserverobj.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\ltthread.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\lttimer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\ltvector.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\physics\math_phys.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\physics\matrix.h
# End Source File
# Begin Source File

SOURCE=..\..\model\src\model.h
# End Source File
# Begin Source File

SOURCE=..\..\model\src\model_ops.h
# End Source File
# Begin Source File

SOURCE=..\..\model\src\modelallocations.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\motion.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\moveobject.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\moveplayer.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\musicmgr.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\net\src\netmgr.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\nexus.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\net\src\NFPacketDebug.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\objectmgr.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\packetdefs.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\parse_world_info.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\pixelformat.h
# End Source File
# Begin Source File

SOURCE=..\..\client\src\predict.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\physics\quaternion.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\ratetracker.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\refobj.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\render.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\renderinfostruct.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\RenderObject.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\sys\win\renderstruct.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=..\..\..\libs\rezmgr\rezfile.h
# End Source File
# Begin Source File

SOURCE=..\..\..\libs\rezmgr\rezhash.h
# End Source File
# Begin Source File

SOURCE=..\..\..\libs\rezmgr\rezmgr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\libs\rezmgr\reztypes.h
# End Source File
# Begin Source File

SOURCE=..\..\server\src\s_client.h
# End Source File
# Begin Source File

SOURCE=..\..\server\src\s_concommand.h
# End Source File
# Begin Source File

SOURCE=..\..\server\src\s_net.h
# End Source File
# Begin Source File

SOURCE=..\..\server\src\s_object.h
# End Source File
# Begin Source File

SOURCE=..\..\server\src\server_consolestate.h
# End Source File
# Begin Source File

SOURCE=..\..\server\src\server_extradata.h
# End Source File
# Begin Source File

SOURCE=..\..\server\src\server_filemgr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\server_interface.h
# End Source File
# Begin Source File

SOURCE=..\..\server\src\server_loaderthread.h
# End Source File
# Begin Source File

SOURCE=..\..\server\src\serverde_impl.h
# End Source File
# Begin Source File

SOURCE=..\..\server\src\serverevent.h
# End Source File
# Begin Source File

SOURCE=..\..\server\src\serverexception.h
# End Source File
# Begin Source File

SOURCE=..\..\server\src\servermgr.h
# End Source File
# Begin Source File

SOURCE=..\..\server\src\serverobj.h
# End Source File
# Begin Source File

SOURCE=..\..\client\src\setupobject.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\sys\win\shellbind.h
# End Source File
# Begin Source File

SOURCE=..\..\server\src\sloaderthread.h
# End Source File
# Begin Source File

SOURCE=..\..\server\src\smoveabstract.h
# End Source File
# Begin Source File

SOURCE=..\..\sound\src\soundbuffer.h
# End Source File
# Begin Source File

SOURCE=..\..\sound\src\sounddata.h
# End Source File
# Begin Source File

SOURCE=..\..\sound\src\soundinstance.h
# End Source File
# Begin Source File

SOURCE=..\..\server\src\soundtrack.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\stacktrace.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\staticfifo.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\stdlterror.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\streamsim.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\stringmgr.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\syscounter.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\sysddstructs.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\io\src\sysfile.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sysstreamsim.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\syssysteminfo.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\systhread.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\systimer.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\net\src\sysudpdriver.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\timemgr.h
# End Source File
# Begin Source File

SOURCE=..\..\model\src\transformmaker.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\physics\triangle.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\net\src\sys\win\udpdriver.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\physics\vector.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\version_info.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\version_resource.h
# End Source File
# Begin Source File

SOURCE=..\..\world\src\visit_pvs.h
# End Source File
# Begin Source File

SOURCE=..\..\sound\src\wave.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\winbind.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\sys\win\winstdlterror.h
# End Source File
# Begin Source File

SOURCE=..\..\world\src\world_blind_object_data.h
# End Source File
# Begin Source File

SOURCE=..\..\world\src\world_blocker_data.h
# End Source File
# Begin Source File

SOURCE=..\..\world\src\world_client.h
# End Source File
# Begin Source File

SOURCE=..\..\world\src\world_client_bsp.h
# End Source File
# Begin Source File

SOURCE=..\..\world\src\world_particle_blocker_data.h
# End Source File
# Begin Source File

SOURCE=..\..\world\src\world_tree.h
# End Source File
# Begin Source File

SOURCE=..\..\world\src\worldtreehelper.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Modules"

# PROP Default_Filter ""
# Begin Group "World"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\world\src\world_interface.h
# End Source File
# Begin Source File

SOURCE=..\..\world\src\world_server.h
# End Source File
# Begin Source File

SOURCE=..\..\server\src\world_server_bsp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\world\src\world_server_bsp.h
# End Source File
# Begin Source File

SOURCE=..\..\world\src\world_shared_bsp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\world\src\world_shared_bsp.h
# End Source File
# End Group
# Begin Group "Game Interfaces"

# PROP Default_Filter ""
# Begin Group "ILTModel"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\sdk\inc\iltmodel.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\modellt_impl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\server\src\server_iltmodel.cpp
# End Source File
# End Group
# Begin Group "ILTCommon"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\sdk\inc\iltcommon.h
# End Source File
# Begin Source File

SOURCE=..\..\server\src\server_iltcommon.cpp
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\shared_iltcommon.cpp
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\shared_iltcommon.h
# End Source File
# End Group
# Begin Group "ILTSoundMgr"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\sdk\inc\iltsoundmgr.h
# End Source File
# Begin Source File

SOURCE=..\..\server\src\server_iltsoundmgr.cpp
# End Source File
# End Group
# Begin Group "ILTTransform"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\sdk\inc\ilttransform.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\transformlt_impl.cpp
# End Source File
# End Group
# Begin Group "ILTPhysics"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\sdk\inc\iltphysics.h
# End Source File
# Begin Source File

SOURCE=..\..\server\src\server_iltphysics.cpp
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\shared_iltphysics.cpp
# End Source File
# End Group
# End Group
# Begin Source File

SOURCE=..\..\shared\src\interface_linkage.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# End Target
# End Project
