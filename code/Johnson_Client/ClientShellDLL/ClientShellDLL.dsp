# Microsoft Developer Studio Project File - Name="ClientShellDLL" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ClientShellDLL - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ClientShellDLL.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ClientShellDLL.mak" CFG="ClientShellDLL - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ClientShellDLL - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ClientShellDLL - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ClientShellDLL - Win32 Final" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ClientShellDLL - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Built\Release\ClientShellDll\SA"
# PROP Intermediate_Dir "..\..\Built\Release\ClientShellDll\SA"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I ".\\" /I "..\..\shared" /I "..\..\Shared\SA" /I "..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\ClientShellShared" /I "..\..\clientres\shared" /I "..\..\clientres\SA" /I "..\..\..\Engine\sdk\inc" /I "..\..\..\libs\stdlith" /I "..\..\..\libs\butemgr" /I "..\..\..\libs\cryptmgr" /I "..\..\..\libs\lith" /I "..\..\..\libs\mfcstub" /I "..\..\libs\ltguimgr" /I "..\..\..\libs\regmgr" /I "..\..\libs\wonapi" /I "..\..\libs\serverdir" /D "NDEBUG" /D "_CLIENTBUILD" /D "WIN32" /D "_WINDOWS" /D "_NOLFBUILD" /D "NO_PRAGMA_LIBS" /D "NOPS2" /D "MEMTRACK_CLIENT" /D "NOT_USING_FULL" /D "DEBUG_SHOP" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 winmm.lib ole32.lib Ws2_32.lib kernel32.lib user32.lib gdi32.lib advapi32.lib ClientShellShared.lib ButeMgr.lib MFCStub.lib CryptMgr.lib Lib_StdLith.lib Lib_Lith.lib LtGuiMgr.lib ServerDir.lib RegMgr.lib LTMem.lib d3dx8.lib imm32.lib Shell32.lib ./CurseFilter.lib /nologo /subsystem:windows /dll /profile /debug /machine:I386 /nodefaultlib:"LIBCMTD" /out:"..\..\Built\Release\ClientShellDll\SA\CShell.dll" /libpath:"..\..\..\Libs\Built\Release" /libpath:"..\..\..\Engine\built\Release\Libs" /libpath:"..\..\Built\Release\Libs"
# SUBTRACT LINK32 /map
# Begin Custom Build
TargetPath=\Johnson_Work\Johnson_Code\Johnson_Client\Built\Release\ClientShellDll\SA\CShell.dll
InputPath=\Johnson_Work\Johnson_Code\Johnson_Client\Built\Release\ClientShellDll\SA\CShell.dll
SOURCE="$(InputPath)"

"$(SA_REZ_DIR)\cshell.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	Rem IncrediBuild_OutputFile $(TargetPath) 
	echo copy $(TargetPath) $(SA_REZ_DIR) 
	copy $(TargetPath) $(SA_REZ_DIR) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "ClientShellDLL - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\Johnson_Built\Johnson_Client\Debug\ClientShellDll\SA"
# PROP Intermediate_Dir "..\..\..\Johnson_Built\Johnson_Client\Debug\ClientShellDll\SA"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /ZI /Od /I "..\..\Shared\SA" /I ".\\" /I "..\..\..\Johnson_Libs\STLPORT-4.0\STLPORT" /I "..\ClientShellShared" /I "..\..\clientres\shared" /I "..\..\clientres\SA" /I "..\..\shared" /I "..\..\..\Johnson_Engine\sdk\inc" /I "..\..\..\Johnson_Libs\stdlith" /I "..\..\..\Johnson_Libs\butemgr" /I "..\..\..\Johnson_Libs\cryptmgr" /I "..\..\..\Johnson_Libs\lith" /I "..\..\..\Johnson_Libs\mfcstub" /I "..\..\libs\ltguimgr" /I "..\..\..\Johnson_Libs\regmgr" /I "..\..\libs\wonapi" /I "..\..\libs\serverdir" /D "_DEBUG" /D "_CLIENTBUILD" /D "WIN32" /D "_WINDOWS" /D "NOPS2" /D "MEMTRACK_CLIENT" /D "DEBUG_SHOP" /D "BIG_FAT_MODE_TEST" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib ole32.lib Ws2_32.lib kernel32.lib user32.lib gdi32.lib advapi32.lib ClientShellShared.lib ButeMgr.lib MFCStub.lib CryptMgr.lib Lib_StdLith.lib Lib_Lith.lib LtGuiMgr.lib ServerDir.lib RegMgr.lib LTMem.lib d3dx8.lib imm32.lib Shell32.lib CurseFilter.lib /nologo /subsystem:windows /dll /profile /debug /machine:I386 /nodefaultlib:"LIBCD" /out:"..\..\..\Lib\Debug\CShell.dll" /implib:"..\..\..\Lib\Debug\CShell.lib" /libpath:"..\..\..\Lib\Debug"

!ELSEIF  "$(CFG)" == "ClientShellDLL - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Final"
# PROP BASE Intermediate_Dir "Final"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Built\Final\ClientShellDll\SA"
# PROP Intermediate_Dir "..\..\Built\Final\ClientShellDll\SA"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\..\Shared\SA" /I ".\\" /I "..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\ClientShellShared" /I "..\..\clientres\shared" /I "..\..\clientres\SA" /I "..\..\shared" /I "..\..\..\Engine\sdk\inc" /I "..\..\..\libs\stdlith" /I "..\..\..\libs\butemgr" /I "..\..\..\libs\cryptmgr" /I "..\..\..\libs\lith" /I "..\..\..\libs\mfcstub" /I "..\..\libs\ltguimgr" /I "..\..\..\libs\regmgr" /I "..\..\libs\wonapi" /I "..\..\libs\serverdir" /D "NDEBUG" /D "_CLIENTBUILD" /D "WIN32" /D "_WINDOWS" /D "_NOLFBUILD" /D "NO_PRAGMA_LIBS" /D "NOPS2" /D "_FINAL" /Yu"stdafx.h" /FD /Zm1000 /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /I "..\..\Shared\SA" /I ".\\" /I "..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\ClientShellShared" /I "..\..\clientres\shared" /I "..\..\clientres\SA" /I "..\..\shared" /I "..\..\..\Engine\sdk\inc" /I "..\..\..\libs\stdlith" /I "..\..\..\libs\butemgr" /I "..\..\..\libs\cryptmgr" /I "..\..\..\libs\lith" /I "..\..\..\libs\mfcstub" /I "..\..\libs\ltguimgr" /I "..\..\..\libs\regmgr" /I "..\..\libs\wonapi" /I "..\..\libs\serverdir" /D "NDEBUG" /D "_CLIENTBUILD" /D "WIN32" /D "_WINDOWS" /D "_NOLFBUILD" /D "NO_PRAGMA_LIBS" /D "NOPS2" /D "_FINAL" /D "MEMTRACK_CLIENT" /D "USE_FIRE_GUARD" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ole32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib:"LIBCMTD" /out:"..\..\Built\Final\ClientShellDll\SA\CShell.dll"
# SUBTRACT BASE LINK32 /pdb:none /map
# ADD LINK32 winmm.lib Ws2_32.lib kernel32.lib user32.lib gdi32.lib ole32.lib advapi32.lib ClientShellShared.lib ButeMgr.lib MFCStub.lib CryptMgr.lib Lib_StdLith.lib Lib_Lith.lib LtGuiMgr.lib ServerDir.lib RegMgr.lib LTMem.lib d3dx8.lib imm32.lib Shell32.lib ./CurseFilter.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libc" /out:"..\..\Built\Final\ClientShellDll\SA\CShell.dll" /libpath:"..\..\..\Libs\Built\Final" /libpath:"..\..\..\Engine\built\Final\libs" /libpath:"..\..\Built\Final\Libs" /Release
# SUBTRACT LINK32 /profile /pdb:none
# Begin Custom Build
TargetPath=\Johnson_Work\Johnson_Code\Johnson_Client\Built\Final\ClientShellDll\SA\CShell.dll
InputPath=\Johnson_Work\Johnson_Code\Johnson_Client\Built\Final\ClientShellDll\SA\CShell.dll
SOURCE="$(InputPath)"

"$(SA_REZ_DIR)\cshell.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	Rem IncrediBuild_OutputFile $(TargetPath) 
	echo copy $(TargetPath) $(SA_REZ_DIR) 
	copy $(TargetPath) $(SA_REZ_DIR) 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "ClientShellDLL - Win32 Release"
# Name "ClientShellDLL - Win32 Debug"
# Name "ClientShellDLL - Win32 Final"
# Begin Group "Source"

# PROP Default_Filter "*.cpp"
# Begin Source File

SOURCE=.\GameHIIME.cpp
# End Source File
# Begin Source File

SOURCE=.\GlobalModule.cpp
# End Source File
# Begin Source File

SOURCE=.\GlobalsInit.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Johnson_Engine\sdk\inc\iltbaseclass.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\..\Johnson_Engine\sdk\inc\ltmodule.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\..\Johnson_Engine\sdk\inc\ltobjref.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\..\Johnson_Engine\sdk\inc\ltquatbase.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Lib_LogicObject\Room\Option.cpp
# End Source File
# Begin Source File

SOURCE=.\SAClientWeaponAllocator.cpp
# End Source File
# Begin Source File

SOURCE=.\SAGameClientShell.cpp
# End Source File
# Begin Source File

SOURCE=.\SAHUDMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\SAInterfaceMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\SALayoutMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Shared\SA\SAMissionButeMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\SAPlayerMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\SAPlayerStats.cpp
# End Source File
# Begin Source File

SOURCE=.\SAScreenMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Shared\SA\SAVersionMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Stdafx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Headers"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=..\..\Shared\ButeFileDefines.h
# End Source File
# Begin Source File

SOURCE=.\CurseFilter.h
# End Source File
# Begin Source File

SOURCE=.\GameHIIME.h
# End Source File
# Begin Source File

SOURCE=.\GlobalModule.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Engine\sdk\inc\iltbaseclass.h
# End Source File
# Begin Source File

SOURCE=..\..\Libs\ServerDir\IServerDir.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Engine\sdk\inc\ltmodule.h
# End Source File
# Begin Source File

SOURCE=..\..\..\engine\sdk\inc\ltobjref.h
# End Source File
# Begin Source File

SOURCE=..\..\Shared\LTPoly.h
# End Source File
# Begin Source File

SOURCE=.\SAClientWeaponAllocator.h
# End Source File
# Begin Source File

SOURCE=.\SAGameClientShell.h
# End Source File
# Begin Source File

SOURCE=.\SAHUDMgr.h
# End Source File
# Begin Source File

SOURCE=.\SAInterfaceMgr.h
# End Source File
# Begin Source File

SOURCE=.\SALayoutMgr.h
# End Source File
# Begin Source File

SOURCE=..\..\Shared\SA\SAMissionButeMgr.h
# End Source File
# Begin Source File

SOURCE=.\SAPlayerMgr.h
# End Source File
# Begin Source File

SOURCE=.\SAPlayerStats.h
# End Source File
# Begin Source File

SOURCE=.\SAScreenMgr.h
# End Source File
# Begin Source File

SOURCE=..\..\Shared\SA\SAVersionMgr.h
# End Source File
# Begin Source File

SOURCE=.\Stdafx.h
# End Source File
# End Group
# Begin Group "HUD - in Use"

# PROP Default_Filter "cpp;h;"
# Begin Group "HUD - NOT in Use"

# PROP Default_Filter "cpp;h;"
# Begin Source File

SOURCE=.\HUDAir.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDAir.h
# End Source File
# Begin Source File

SOURCE=.\HUDCarrying.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDCarrying.h
# End Source File
# Begin Source File

SOURCE=.\HUDDoomsday.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDDoomsday.h
# End Source File
# Begin Source File

SOURCE=.\HUDHiding.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDHiding.h
# End Source File
# Begin Source File

SOURCE=.\HUDHidingBar.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDHidingBar.h
# End Source File
# Begin Source File

SOURCE=.\HUDObjectives.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDObjectives.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\HUDAmmo.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDAmmo.h
# End Source File
# Begin Source File

SOURCE=.\HUDCameraBloodFX.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDCameraBloodFX.h
# End Source File
# Begin Source File

SOURCE=.\HUDChooser.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDChooser.h
# End Source File
# Begin Source File

SOURCE=.\HUDCompass.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDCompass.h
# End Source File
# Begin Source File

SOURCE=.\HUDCrosshair.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDCrosshair.h
# End Source File
# Begin Source File

SOURCE=.\HUDDamageDir.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDDamageDir.h
# End Source File
# Begin Source File

SOURCE=.\HUDFlashBangFX.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDFlashBangFX.h
# End Source File
# Begin Source File

SOURCE=.\HUDGameOver.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDGameOver.h
# End Source File
# Begin Source File

SOURCE=.\HUDGameState.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDGameState.h
# End Source File
# Begin Source File

SOURCE=.\HUDGogglesFX.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDGogglesFX.h
# End Source File
# Begin Source File

SOURCE=.\HUDMiniMap.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDMiniMap.h
# End Source File
# Begin Source File

SOURCE=.\HUDPlayerState.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDPlayerState.h
# End Source File
# Begin Source File

SOURCE=.\HUDProgressBar.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDProgressBar.h
# End Source File
# Begin Source File

SOURCE=.\HUDRadio.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDRadio.h
# End Source File
# Begin Source File

SOURCE=.\HUDRespawn.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDRespawn.h
# End Source File
# Begin Source File

SOURCE=.\HUDSensiCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDSensiCtrl.h
# End Source File
# Begin Source File

SOURCE=.\HUDTimer.cpp
# End Source File
# Begin Source File

SOURCE=.\HUDTimer.h
# End Source File
# End Group
# Begin Group "Screen - in Use"

# PROP Default_Filter "cpp;h;"
# Begin Group "Popup"

# PROP Default_Filter "cpp;h;"
# Begin Source File

SOURCE=.\PopupHelp.cpp
# End Source File
# Begin Source File

SOURCE=.\PopupHelp.h
# End Source File
# Begin Source File

SOURCE=.\PopupInvite.cpp
# End Source File
# Begin Source File

SOURCE=.\PopupInvite.h
# End Source File
# Begin Source File

SOURCE=.\PopupItemBuy.cpp
# End Source File
# Begin Source File

SOURCE=.\PopupItemBuy.h
# End Source File
# Begin Source File

SOURCE=.\PopupMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\PopupMenu.h
# End Source File
# Begin Source File

SOURCE=.\PopupNickName.cpp
# End Source File
# Begin Source File

SOURCE=.\PopupNickName.h
# End Source File
# Begin Source File

SOURCE=.\PopupOption_Ex.cpp
# End Source File
# Begin Source File

SOURCE=.\PopupOption_Ex.h
# End Source File
# Begin Source File

SOURCE=.\PopupPassword.cpp
# End Source File
# Begin Source File

SOURCE=.\PopupPassword.h
# End Source File
# Begin Source File

SOURCE=.\PopupRoomOption.cpp
# End Source File
# Begin Source File

SOURCE=.\PopupRoomOption.h
# End Source File
# Begin Source File

SOURCE=.\PopupScreenBase.cpp
# End Source File
# Begin Source File

SOURCE=.\PopupScreenBase.h
# End Source File
# Begin Source File

SOURCE=.\PopupScreenCreateRoom.cpp
# End Source File
# Begin Source File

SOURCE=.\PopupScreenCreateRoom.h
# End Source File
# Begin Source File

SOURCE=.\PopupScreenOption.cpp
# End Source File
# Begin Source File

SOURCE=.\PopupScreenOption.h
# End Source File
# Begin Source File

SOURCE=.\PopupSearchResult.cpp
# End Source File
# Begin Source File

SOURCE=.\PopupSearchResult.h
# End Source File
# Begin Source File

SOURCE=.\PopupSearchUser.cpp
# End Source File
# Begin Source File

SOURCE=.\PopupSearchUser.h
# End Source File
# Begin Source File

SOURCE=.\PopupUserInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\PopupUserInfo.h
# End Source File
# Begin Source File

SOURCE=.\PopupWeaponDetail.cpp
# End Source File
# Begin Source File

SOURCE=.\PopupWeaponDetail.h
# End Source File
# End Group
# Begin Group "Screen"

# PROP Default_Filter "cpp;h"
# Begin Source File

SOURCE=.\ScreenBase.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenBase.h
# End Source File
# Begin Source File

SOURCE=.\ScreenChannel.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenChannel.h
# End Source File
# Begin Source File

SOURCE=.\ScreenCharacter.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenCharacter.h
# End Source File
# Begin Source File

SOURCE=.\ScreenInventory.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenInventory.h
# End Source File
# Begin Source File

SOURCE=.\ScreenMain.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenMain.h
# End Source File
# Begin Source File

SOURCE=.\ScreenResult.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenResult.h
# End Source File
# Begin Source File

SOURCE=.\ScreenRoom.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenRoom.h
# End Source File
# Begin Source File

SOURCE=.\ScreenShop.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenShop.h
# End Source File
# End Group
# Begin Group "Controls"

# PROP Default_Filter "cpp;h;"
# Begin Source File

SOURCE=.\SAListChannel.cpp
# End Source File
# Begin Source File

SOURCE=.\SAListChannel.h
# End Source File
# Begin Source File

SOURCE=.\SAListRoom.cpp
# End Source File
# Begin Source File

SOURCE=.\SAListRoom.h
# End Source File
# Begin Source File

SOURCE=.\SAListTeam.cpp
# End Source File
# Begin Source File

SOURCE=.\SAListTeam.h
# End Source File
# Begin Source File

SOURCE=.\SAListText.cpp
# End Source File
# Begin Source File

SOURCE=.\SAListText.h
# End Source File
# Begin Source File

SOURCE=.\SAListUser.cpp
# End Source File
# Begin Source File

SOURCE=.\SAListUser.h
# End Source File
# Begin Source File

SOURCE=.\SAMapList.cpp
# End Source File
# Begin Source File

SOURCE=.\SAMapList.h
# End Source File
# Begin Source File

SOURCE=.\TextScroll.cpp
# End Source File
# Begin Source File

SOURCE=.\TextScroll.h
# End Source File
# End Group
# Begin Group "Screen - NOT in Use"

# PROP Default_Filter "cpp;h;"
# Begin Source File

SOURCE=.\ScreenAudio.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenAudio.h
# End Source File
# Begin Source File

SOURCE=.\ScreenConfigure.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenConfigure.h
# End Source File
# Begin Source File

SOURCE=.\ScreenControls.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenControls.h
# End Source File
# Begin Source File

SOURCE=.\ScreenCreateRoom.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenCreateRoom.h
# End Source File
# Begin Source File

SOURCE=.\ScreenCrosshair.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenCrosshair.h
# End Source File
# Begin Source File

SOURCE=.\ScreenDisplay.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenDisplay.h
# End Source File
# Begin Source File

SOURCE=.\ScreenEndCoopMission.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenEndCoopMission.h
# End Source File
# Begin Source File

SOURCE=.\ScreenEndDMMission.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenEndDMMission.h
# End Source File
# Begin Source File

SOURCE=.\ScreenEndMission.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenEndMission.h
# End Source File
# Begin Source File

SOURCE=.\ScreenFailure.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenFailure.h
# End Source File
# Begin Source File

SOURCE=.\ScreenGame.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenGame.h
# End Source File
# Begin Source File

SOURCE=.\ScreenHost.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenHost.h
# End Source File
# Begin Source File

SOURCE=.\ScreenHostDDOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenHostDDOptions.h
# End Source File
# Begin Source File

SOURCE=.\ScreenHostDMOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenHostDMOptions.h
# End Source File
# Begin Source File

SOURCE=.\ScreenHostLevels.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenHostLevels.h
# End Source File
# Begin Source File

SOURCE=.\ScreenHostMission.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenHostMission.h
# End Source File
# Begin Source File

SOURCE=.\ScreenHostOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenHostOptions.h
# End Source File
# Begin Source File

SOURCE=.\ScreenHostTDMOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenHostTDMOptions.h
# End Source File
# Begin Source File

SOURCE=.\ScreenHostWeapons.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenHostWeapons.h
# End Source File
# Begin Source File

SOURCE=.\ScreenJoin.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenJoin.h
# End Source File
# Begin Source File

SOURCE=.\ScreenJoinLAN.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenJoinLAN.h
# End Source File
# Begin Source File

SOURCE=.\ScreenJoystick.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenJoystick.h
# End Source File
# Begin Source File

SOURCE=.\ScreenKeyboard.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenKeyboard.h
# End Source File
# Begin Source File

SOURCE=.\ScreenLoad.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenLoad.h
# End Source File
# Begin Source File

SOURCE=.\ScreenMouse.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenMouse.h
# End Source File
# Begin Source File

SOURCE=.\ScreenMulti.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenMulti.h
# End Source File
# Begin Source File

SOURCE=.\ScreenOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenOptions.h
# End Source File
# Begin Source File

SOURCE=.\ScreenPerformance.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenPerformance.h
# End Source File
# Begin Source File

SOURCE=.\ScreenPlayer.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenPlayer.h
# End Source File
# Begin Source File

SOURCE=.\ScreenPlayerSkills.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenPlayerSkills.h
# End Source File
# Begin Source File

SOURCE=.\ScreenPlayerTeam.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenPlayerTeam.h
# End Source File
# Begin Source File

SOURCE=.\ScreenProfile.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenProfile.h
# End Source File
# Begin Source File

SOURCE=.\ScreenSave.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenSave.h
# End Source File
# Begin Source File

SOURCE=.\ScreenSingle.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenSingle.h
# End Source File
# Begin Source File

SOURCE=.\ScreenTeam.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenTeam.h
# End Source File
# Begin Source File

SOURCE=.\ScreenWeapon.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenWeapon.h
# End Source File
# End Group
# End Group
# End Target
# End Project
