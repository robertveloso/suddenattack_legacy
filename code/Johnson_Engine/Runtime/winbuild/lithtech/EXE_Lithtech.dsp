# Microsoft Developer Studio Project File - Name="Exe_LithTech" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Exe_LithTech - WIN32 DEBUG
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "EXE_Lithtech.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "EXE_Lithtech.mak" CFG="Exe_LithTech - WIN32 DEBUG"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Exe_LithTech - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Exe_LithTech - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "Exe_LithTech - Win32 Final" (based on "Win32 (x86) Application")
!MESSAGE "Exe_LithTech - Win32 Release Public Dev" (based on "Win32 (x86) Application")
!MESSAGE "Exe_LithTech - Win32 Eval" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/SA/Johnson_Code/Johnson_Engine/Runtime/winbuild/lithtech", MFYBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\built\Release\Runtime\Exe_LithTech"
# PROP Intermediate_Dir "..\..\..\built\Release\Runtime\Exe_LithTech"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /Zi /O2 /Ob2 /I "..\..\..\..\Libs\STLPORT-4.0\STLPORT" /I "..\..\..\..\libs\stdlith" /I "..\..\..\..\libs\lith" /I "..\..\..\libs\rezmgr" /I "..\..\controlfilemgr" /I "..\..\crtcompat" /I "..\..\lithtemplate" /I "." /I "..\..\client\src" /I "..\..\kernel\mem\src" /I "..\..\kernel\io\src" /I "..\..\kernel\src" /I "..\..\kernel\src\sys\win" /I "..\..\model\src" /I "..\..\sound\src" /I "..\..\server\src" /I "..\..\shared\src\sys\win" /I "..\..\shared\src" /I "..\..\world\src" /I "..\..\..\sdk\inc" /I "..\..\..\sdk\inc\compat" /I "..\..\kernel\net\src" /I "..\..\render_b\src" /I "..\..\mpm\src" /I "..\..\comm\src" /I "..\..\ui\src" /I "..\..\distrobj\src" /I "..\..\state_mgr\src" /I "..\..\physics\src" /I "..\..\render_a\src\sys\d3d" /I "..\..\..\sdk\inc\physics" /I "..\..\..\sdk\inc\state_mgr" /D "NDEBUG" /D "DE_LOCAL_SERVERBIND" /D "DE_CLIENT_COMPILE" /D "DSNDMGR_NO_MFC" /D "DIRECTENGINE_COMPILE" /D "_MBCS" /D "__D3D" /D "__D3DREND" /D "MODEL_SUPPORT_ABC" /D "WIN32" /D "_WINDOWS" /D "NO_PROFILE" /FR /Yu"bdefs.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 ddraw.lib d3dx8.lib d3d8.lib dxguid.lib winmm.lib atl.lib dplayx.lib dinput8.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Lib_Lith.lib Lib_stdLith.lib Lib_RezMgr.lib Lib_Zlib.lib Lib_ILTSound.lib Lib_D3DRender.lib Lib_UI.lib Lib_NullRender.lib Lib_Info.lib LtMem.lib ../../../CrashRpt.lib /nologo /subsystem:windows /profile /debug /machine:I386 /nodefaultlib:"libc.lib" /out:"..\..\..\built\Release\Runtime\EXE_Lithtech\Lithtech.exe" /libpath:"..\..\..\built\Release\libs" /libpath:"..\..\..\..\libs\built\Release"
# Begin Custom Build
TargetPath=\SA\Johnson_Code\Johnson_Engine\built\Release\Runtime\EXE_Lithtech\Lithtech.exe
InputPath=\SA\Johnson_Code\Johnson_Engine\built\Release\Runtime\EXE_Lithtech\Lithtech.exe
SOURCE="$(InputPath)"

"$(SA_MAIN_DIR)\SuddenAttack.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	Rem IncrediBuild_OutputFile $(TargetPath) 
	echo copy $(TargetPath) $(SA_MAIN_DIR) 
	copy $(TargetPath) $(SA_MAIN_DIR) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\..\Johnson_Built\Johnson_Engine\Debug\Runtime\Exe_LithTech"
# PROP Intermediate_Dir "..\..\..\..\Johnson_Built\Johnson_Engine\Debug\Runtime\Exe_LithTech"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Zi /Od /I "..\..\..\..\Johnson_Libs\STLPORT-4.0\STLPORT" /I "..\..\..\..\Johnson_Libs\stdlith" /I "..\..\..\..\Johnson_Libs\lith" /I "..\..\..\libs\rezmgr" /I "..\..\controlfilemgr" /I "..\..\crtcompat" /I "..\..\lithtemplate" /I "." /I "..\..\client\src" /I "..\..\kernel\mem\src" /I "..\..\kernel\io\src" /I "..\..\kernel\src" /I "..\..\kernel\src\sys\win" /I "..\..\model\src" /I "..\..\sound\src" /I "..\..\server\src" /I "..\..\shared\src\sys\win" /I "..\..\shared\src" /I "..\..\world\src" /I "..\..\..\sdk\inc" /I "..\..\..\sdk\inc\compat" /I "..\..\kernel\net\src" /I "..\..\render_b\src" /I "..\..\mpm\src" /I "..\..\comm\src" /I "..\..\ui\src" /I "..\..\distrobj\src" /I "..\..\state_mgr\src" /I "..\..\physics\src" /I "..\..\render_a\src\sys\d3d" /I "..\..\..\sdk\inc\physics" /I "..\..\..\sdk\inc\state_mgr" /I "..\..\..\..\Johnson_Libs\mfcstub" /D "_DEBUG" /D "DE_LOCAL_SERVERBIND" /D "DE_CLIENT_COMPILE" /D "DSNDMGR_NO_MFC" /D "DIRECTENGINE_COMPILE" /D "_MBCS" /D "__D3D" /D "__D3DREND" /D "MODEL_SUPPORT_ABC" /D "WIN32" /D "_WINDOWS" /Fr /Yu"bdefs.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 d3dx8.lib d3d8.lib dxguid.lib winmm.lib atl.lib dplayx.lib dinput8.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Lib_Lith.lib Lib_stdLith.lib Lib_RezMgr.lib Lib_Zlib.lib Lib_ILTSound.lib Lib_D3DRender.lib Lib_UI.lib Lib_NullRender.lib Lib_Info.lib LtMem.lib CrashRpt.lib /nologo /subsystem:windows /profile /debug /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libcmtd.lib" /out:"..\..\..\..\Lib\Debug\SuddenAttack.exe" /libpath:"..\..\..\..\Lib\Debug"
# Begin Custom Build
TargetPath=\SA\Johnson_Code\Lib\Debug\SuddenAttack.exe
InputPath=\SA\Johnson_Code\Lib\Debug\SuddenAttack.exe
SOURCE="$(InputPath)"

"$(SA_MAIN_DIR)\SuddenAttack.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	Rem IncrediBuild_OutputFile $(TargetPath) 
	echo copy $(TargetPath) $(SA_MAIN_DIR) 
	copy $(TargetPath) $(SA_MAIN_DIR) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Final"
# PROP BASE Intermediate_Dir "Final"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\built\Final\Runtime\Exe_LithTech"
# PROP Intermediate_Dir "..\..\..\built\Final\Runtime\Exe_LithTech"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /Zi /O2 /Ob2 /I "..\..\..\..\Libs\STLPORT-4.0\STLPORT" /I "..\..\..\..\libs\stdlith" /I "..\..\..\..\libs\lith" /I "..\..\..\libs\rezmgr" /I "..\..\controlfilemgr" /I "..\..\crtcompat" /I "..\..\lithtemplate" /I "." /I "..\..\client\src" /I "..\..\kernel\mem\src" /I "..\..\kernel\io\src" /I "..\..\kernel\src" /I "..\..\kernel\src\sys\win" /I "..\..\model\src" /I "..\..\sound\src" /I "..\..\server\src" /I "..\..\shared\src\sys\win" /I "..\..\shared\src" /I "..\..\world\src" /I "..\..\..\sdk\inc" /I "..\..\..\sdk\inc\compat" /I "..\..\kernel\net\src" /I "..\..\render_b\src" /I "..\..\mpm\src" /I "..\..\comm\src" /I "..\..\ui\src" /I "..\..\distrobj\src" /I "..\..\state_mgr\src" /I "..\..\physics\src" /I "..\..\render_a\src\sys\d3d" /I "..\..\..\sdk\inc\physics" /I "..\..\..\sdk\inc\state_mgr" /D "NDEBUG" /D "DE_LOCAL_SERVERBIND" /D "DE_CLIENT_COMPILE" /D "DSNDMGR_NO_MFC" /D "DIRECTENGINE_COMPILE" /D "_MBCS" /D "__D3D" /D "__D3DREND" /D "MODEL_SUPPORT_ABC" /D "WIN32" /D "_WINDOWS" /FR /Yu"bdefs.h" /FD /c
# ADD CPP /nologo /MT /W3 /Zi /O2 /Ob2 /I "..\..\..\..\Libs\STLPORT-4.0\STLPORT" /I "..\..\..\..\libs\stdlith" /I "..\..\..\..\libs\lith" /I "..\..\..\libs\rezmgr" /I "..\..\controlfilemgr" /I "..\..\crtcompat" /I "..\..\lithtemplate" /I "." /I "..\..\client\src" /I "..\..\kernel\mem\src" /I "..\..\kernel\io\src" /I "..\..\kernel\src" /I "..\..\kernel\src\sys\win" /I "..\..\model\src" /I "..\..\sound\src" /I "..\..\server\src" /I "..\..\shared\src\sys\win" /I "..\..\shared\src" /I "..\..\world\src" /I "..\..\..\sdk\inc" /I "..\..\..\sdk\inc\compat" /I "..\..\kernel\net\src" /I "..\..\render_b\src" /I "..\..\mpm\src" /I "..\..\comm\src" /I "..\..\ui\src" /I "..\..\distrobj\src" /I "..\..\state_mgr\src" /I "..\..\physics\src" /I "..\..\render_a\src\sys\d3d" /I "..\..\..\sdk\inc\physics" /I "..\..\..\sdk\inc\state_mgr" /D "NDEBUG" /D "DE_LOCAL_SERVERBIND" /D "DE_CLIENT_COMPILE" /D "DSNDMGR_NO_MFC" /D "DIRECTENGINE_COMPILE" /D "_MBCS" /D "__D3D" /D "__D3DREND" /D "MODEL_SUPPORT_ABC" /D "WIN32" /D "_WINDOWS" /D "_FINAL" /D "NO_PROFILE" /D "USE_FIRE_GUARD" /FR /Yu"bdefs.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ddraw.lib d3dx8.lib d3d8.lib dxguid.lib winmm.lib atl.lib dplayx.lib dinput.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib" /out:"..\..\..\built\Final\Runtime\EXE_Lithtech\Lithtech.exe"
# ADD LINK32 ddraw.lib d3dx8.lib d3d8.lib dxguid.lib winmm.lib atl.lib dplayx.lib dinput8.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib Lib_Lith.lib Lib_stdLith.lib Lib_RezMgr.lib Lib_Zlib.lib Lib_ILTSound.lib Lib_D3DRender.lib Lib_UI.lib Lib_NullRender.lib Lib_Info.lib LtMem.lib ../../../CrashRpt.lib ../../../HGameGuard.lib ../../../HGameGuardDll.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc.lib" /out:"..\..\..\built\Final\Runtime\EXE_Lithtech\SuddenAttack.exe" /libpath:"..\..\..\built\final\libs" /libpath:"..\..\..\..\libs\built\final" /Release
# SUBTRACT LINK32 /profile /pdb:none
# Begin Custom Build
TargetPath=\SA\Johnson_Code\Johnson_Engine\built\Final\Runtime\EXE_Lithtech\SuddenAttack.exe
InputPath=\SA\Johnson_Code\Johnson_Engine\built\Final\Runtime\EXE_Lithtech\SuddenAttack.exe
SOURCE="$(InputPath)"

"$(SA_MAIN_DIR)\SuddenAttack.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	Rem IncrediBuild_OutputFile $(TargetPath) 
	echo copy $(TargetPath) $(SA_MAIN_DIR) 
	copy $(TargetPath) $(SA_MAIN_DIR) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Exe_LithTech___Win32_Release_Public_Dev"
# PROP BASE Intermediate_Dir "Exe_LithTech___Win32_Release_Public_Dev"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\built\ReleasePublicDev\Runtime\Exe_LithTech"
# PROP Intermediate_Dir "..\..\..\built\ReleasePublicDev\Runtime\Exe_LithTech"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /O2 /Ob2 /I "..\..\..\..\Libs\STLPORT-4.0\STLPORT" /I "..\..\..\..\libs\stdlith" /I "..\..\..\..\libs\lith" /I "..\..\..\libs\rezmgr" /I "..\..\controlfilemgr" /I "..\..\crtcompat" /I "..\..\lithtemplate" /I "." /I "..\..\client\src" /I "..\..\kernel\mem\src" /I "..\..\kernel\io\src" /I "..\..\kernel\src" /I "..\..\kernel\src\sys\win" /I "..\..\model\src" /I "..\..\sound\src" /I "..\..\server\src" /I "..\..\shared\src\sys\win" /I "..\..\shared\src" /I "..\..\world\src" /I "..\..\..\sdk\inc" /I "..\..\..\sdk\inc\compat" /I "..\..\kernel\net\src" /I "..\..\render_b\src" /I "..\..\mpm\src" /I "..\..\comm\src" /I "..\..\ui\src" /I "..\..\distrobj\src" /I "..\..\state_mgr\src" /I "..\..\physics\src" /I "..\..\render_a\src\sys\d3d" /I "..\..\..\sdk\inc\physics" /I "..\..\..\sdk\inc\state_mgr" /D "NDEBUG" /D "DE_LOCAL_SERVERBIND" /D "DE_CLIENT_COMPILE" /D "DSNDMGR_NO_MFC" /D "DIRECTENGINE_COMPILE" /D "_MBCS" /D "__D3D" /D "__D3DREND" /D "MODEL_SUPPORT_ABC" /D "WIN32" /D "_WINDOWS" /FR /Yu"bdefs.h" /FD /c
# ADD CPP /nologo /MD /W3 /O2 /Ob2 /I "..\..\..\..\Libs\STLPORT-4.0\STLPORT" /I "..\..\..\..\libs\stdlith" /I "..\..\..\..\libs\lith" /I "..\..\..\libs\rezmgr" /I "..\..\controlfilemgr" /I "..\..\crtcompat" /I "..\..\lithtemplate" /I "." /I "..\..\client\src" /I "..\..\kernel\mem\src" /I "..\..\kernel\io\src" /I "..\..\kernel\src" /I "..\..\kernel\src\sys\win" /I "..\..\model\src" /I "..\..\sound\src" /I "..\..\server\src" /I "..\..\shared\src\sys\win" /I "..\..\shared\src" /I "..\..\world\src" /I "..\..\..\sdk\inc" /I "..\..\..\sdk\inc\compat" /I "..\..\kernel\net\src" /I "..\..\render_b\src" /I "..\..\mpm\src" /I "..\..\comm\src" /I "..\..\ui\src" /I "..\..\distrobj\src" /I "..\..\state_mgr\src" /I "..\..\physics\src" /I "..\..\render_a\src\sys\d3d" /I "..\..\..\sdk\inc\physics" /I "..\..\..\sdk\inc\state_mgr" /D "_PUBLIC_DEVELOPMENT" /D "NDEBUG" /D "DE_LOCAL_SERVERBIND" /D "DE_CLIENT_COMPILE" /D "DSNDMGR_NO_MFC" /D "DIRECTENGINE_COMPILE" /D "_MBCS" /D "__D3D" /D "__D3DREND" /D "MODEL_SUPPORT_ABC" /D "WIN32" /D "_WINDOWS" /FR /Yu"bdefs.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ddraw.lib d3dx8.lib d3d8.lib dxguid.lib winmm.lib atl.lib dplayx.lib dinput8.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib" /out:"..\..\..\built\Release\Runtime\EXE_Lithtech\Lithtech.exe"
# SUBTRACT BASE LINK32 /debug
# ADD LINK32 ddraw.lib d3dx8.lib d3d8.lib dxguid.lib winmm.lib atl.lib dplayx.lib dinput8.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib" /out:"..\..\..\built\ReleasePublicDev\Runtime\Exe_LithTech\LithtechDev.exe"
# SUBTRACT LINK32 /debug
# Begin Custom Build
TargetPath=\SA\Johnson_Code\Johnson_Engine\built\ReleasePublicDev\Runtime\Exe_LithTech\LithtechDev.exe
InputPath=\SA\Johnson_Code\Johnson_Engine\built\ReleasePublicDev\Runtime\Exe_LithTech\LithtechDev.exe
SOURCE="$(InputPath)"

"$(GAME_MAIN_DIR)\LithtechDev.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	Rem IncrediBuild_OutputFile $(TargetPath) 
	echo copy $(TargetPath) $(GAME_MAIN_DIR) 
	copy $(TargetPath) $(GAME_MAIN_DIR) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Exe_LithTech___Win32_Eval"
# PROP BASE Intermediate_Dir "Exe_LithTech___Win32_Eval"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\built\Eval\Runtime\Exe_LithTech"
# PROP Intermediate_Dir "..\..\..\built\Eval\Runtime\Exe_LithTech"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /O2 /Ob2 /I "..\..\..\..\Libs\STLPORT-4.0\STLPORT" /I "..\..\..\..\libs\stdlith" /I "..\..\..\..\libs\lith" /I "..\..\..\libs\rezmgr" /I "..\..\controlfilemgr" /I "..\..\crtcompat" /I "..\..\lithtemplate" /I "." /I "..\..\client\src" /I "..\..\kernel\mem\src" /I "..\..\kernel\io\src" /I "..\..\kernel\src" /I "..\..\kernel\src\sys\win" /I "..\..\model\src" /I "..\..\sound\src" /I "..\..\server\src" /I "..\..\shared\src\sys\win" /I "..\..\shared\src" /I "..\..\world\src" /I "..\..\..\sdk\inc" /I "..\..\..\sdk\inc\compat" /I "..\..\kernel\net\src" /I "..\..\render_b\src" /I "..\..\mpm\src" /I "..\..\comm\src" /I "..\..\ui\src" /I "..\..\distrobj\src" /I "..\..\state_mgr\src" /I "..\..\physics\src" /I "..\..\render_a\src\sys\d3d" /I "..\..\..\sdk\inc\physics" /I "..\..\..\sdk\inc\state_mgr" /D "NDEBUG" /D "DE_LOCAL_SERVERBIND" /D "DE_CLIENT_COMPILE" /D "DSNDMGR_NO_MFC" /D "DIRECTENGINE_COMPILE" /D "_MBCS" /D "__D3D" /D "__D3DREND" /D "MODEL_SUPPORT_ABC" /D "WIN32" /D "_WINDOWS" /FR /Yu"bdefs.h" /FD /c
# ADD CPP /nologo /MD /W3 /O2 /Ob2 /I "..\..\..\..\Libs\STLPORT-4.0\STLPORT" /I "..\..\..\..\libs\stdlith" /I "..\..\..\..\libs\lith" /I "..\..\..\libs\rezmgr" /I "..\..\controlfilemgr" /I "..\..\crtcompat" /I "..\..\lithtemplate" /I "." /I "..\..\client\src" /I "..\..\kernel\mem\src" /I "..\..\kernel\io\src" /I "..\..\kernel\src" /I "..\..\kernel\src\sys\win" /I "..\..\model\src" /I "..\..\sound\src" /I "..\..\server\src" /I "..\..\shared\src\sys\win" /I "..\..\shared\src" /I "..\..\world\src" /I "..\..\..\sdk\inc" /I "..\..\..\sdk\inc\compat" /I "..\..\kernel\net\src" /I "..\..\render_b\src" /I "..\..\mpm\src" /I "..\..\comm\src" /I "..\..\ui\src" /I "..\..\distrobj\src" /I "..\..\state_mgr\src" /I "..\..\physics\src" /I "..\..\render_a\src\sys\d3d" /I "..\..\..\sdk\inc\physics" /I "..\..\..\sdk\inc\state_mgr" /D "NDEBUG" /D "DE_LOCAL_SERVERBIND" /D "DE_CLIENT_COMPILE" /D "DSNDMGR_NO_MFC" /D "DIRECTENGINE_COMPILE" /D "_MBCS" /D "__D3D" /D "__D3DREND" /D "MODEL_SUPPORT_ABC" /D "WIN32" /D "_WINDOWS" /D "COMPILE_JUPITER_EVAL" /FR /Yu"bdefs.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ddraw.lib d3dx8.lib d3d8.lib dxguid.lib winmm.lib atl.lib dplayx.lib dinput8.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib" /out:"..\..\..\built\Release\Runtime\EXE_Lithtech\Lithtech.exe"
# SUBTRACT BASE LINK32 /debug
# ADD LINK32 ddraw.lib d3dx8.lib d3d8.lib dxguid.lib winmm.lib atl.lib dplayx.lib dinput8.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib" /out:"..\..\..\built\Eval\Runtime\EXE_Lithtech\Lithtech.exe"
# SUBTRACT LINK32 /debug
# Begin Custom Build
TargetPath=\SA\Johnson_Code\Johnson_Engine\built\Eval\Runtime\EXE_Lithtech\Lithtech.exe
InputPath=\SA\Johnson_Code\Johnson_Engine\built\Eval\Runtime\EXE_Lithtech\Lithtech.exe
SOURCE="$(InputPath)"

"$(GAME_MAIN_DIR)\Lithtech.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	Rem IncrediBuild_OutputFile $(TargetPath) 
	echo copy $(TargetPath) $(GAME_MAIN_DIR) 
	copy $(TargetPath) $(GAME_MAIN_DIR) 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "Exe_LithTech - Win32 Release"
# Name "Exe_LithTech - Win32 Debug"
# Name "Exe_LithTech - Win32 Final"
# Name "Exe_LithTech - Win32 Release Public Dev"
# Name "Exe_LithTech - Win32 Eval"
# Begin Group "LithSharedNoLonger"

# PROP Default_Filter ""
# Begin Group "ControlFileMgr"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\controlfilemgr\controlfilemgr.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\controlfilemgr\controlfilemgr.h
# End Source File
# End Group
# Begin Group "LithTemplate"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\libs\lithtemplate\ltt_buffer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\libs\lithtemplate\ltt_list_circular.h
# End Source File
# End Group
# End Group
# Begin Group "Source"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=..\..\model\src\animtracker.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\shared\src\bdefs.cpp
# ADD CPP /Yc"bdefs.h"
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\bindmgr.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\binkvideomgrimpl.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\shared\src\classbind.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\server\src\classmgr.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\client.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\client\src\client_formatmgr.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\client\src\client_graphmgr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\client\src\client_ticks.cpp
# End Source File
# Begin Source File

SOURCE=..\..\client\src\clientde_impl.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\client\src\clientmgr.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\client\src\clientshell.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\client\src\cmoveabstract.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\client\src\cnet.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\client\src\cobject.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\shared\src\collision.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\shared\src\compress.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\shared\src\concommand.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\shared\src\conparse.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\client\src\console.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=..\..\client\src\consolecommands.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\counter.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\client\src\cutil.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\model\src\sys\d3d\d3d_model_load.cpp
# End Source File
# Begin Source File

SOURCE=..\..\render_b\src\sys\d3d\d3ddrawprim.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\render_b\src\sys\d3d\d3dtexinterface.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\kernel\io\src\sys\win\de_file.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=..\..\world\src\de_mainworld.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\kernel\mem\src\sys\win\de_memory.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=..\..\world\src\de_nodes.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=..\..\world\src\de_objects.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\shared\src\debuggeometry.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\debugging.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=..\..\client\src\debuggraphmgr.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\dhashtable.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\sys\win\dstreamopenqueuemgr.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\dsys_interface.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\dtxmgr.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\dutil.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\engine_vars.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\client\src\errorlog.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\shared\src\findobj.cpp
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\ftclient.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\shared\src\ftserv.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\world\src\fullintersectline.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\server\src\game_serialize.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\shared\src\gamemath.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\shared\src\genericprop_setup.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\shared\src\genltstream.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\shared\src\geometry.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\shared\src\geomroutines.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\shared\src\impl_common.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\input.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\InputSim.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\interface_helpers.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=..\..\server\src\interlink.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\world\src\intersect_line.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\world\src\intersectsweptsphere.cpp
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\leech.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\world\src\light_table.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\shared\src\lightmap_compress.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\shared\src\lightmap_planes.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\client\src\linesystem.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\load_pcx.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=..\..\kernel\net\src\localdriver.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\client\src\ltbenchmark_impl.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\ltdirectmusic_impl.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\ltdirectmusiccontrolfile.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\ltdirectmusicloader.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\lterror.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\lthread.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=..\..\client\src\ltinfo_impl.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=..\..\kernel\mem\src\ltmemory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\ltmessage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\client\src\ltmessage_client.cpp
# End Source File
# Begin Source File

SOURCE=..\..\server\src\ltmessage_server.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\ltmodule.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Gm-
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

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

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\client\src\memorywatch.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\model\src\model.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\model\src\model_load.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\model\src\modelallocations.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\shared\src\motion.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\shared\src\moveobject.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\shared\src\moveplayer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\musicdriver.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=..\..\kernel\net\src\netmgr.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\shared\src\nexus.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\kernel\net\src\NFPacketDebug.cpp
# End Source File
# Begin Source File

SOURCE=..\..\model\src\sys\null\nullmodel_load.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\shared\src\objectmgr.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\kernel\net\src\packet.cpp
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\parse_world_info.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\client\src\particlesystem.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\shared\src\pixelformat.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\client\src\polygrid.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\client\src\predict.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\shared\src\ratetracker.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\render.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=..\..\server\src\s_client.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\server\src\s_concommand.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\server\src\S_Intersect.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\server\src\s_net.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\server\src\s_object.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\server\src\server_consolestate.cpp
# End Source File
# Begin Source File

SOURCE=..\..\server\src\server_extradata.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\server\src\server_filemgr.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\server\src\serverde_impl.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\server\src\serverevent.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\server\src\servermgr.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\client\src\setupobject.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\client\src\shellnet.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\client\src\shelltransfer.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\client\src\shellutil.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\server\src\smoveabstract.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\sound\src\soundbuffer.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\sound\src\sounddata.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\sound\src\soundinstance.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\server\src\soundtrack.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\client\src\sprite.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\shared\src\spritecontrolimpl.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\shared\src\stacktrace.cpp
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\stdlterror.cpp
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\streamsim.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\stringmgr.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\strtools.cpp
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\systeminfo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\text_mgr.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\timemgr.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=..\..\model\src\transformmaker.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\kernel\net\src\sys\win\udpdriver.cpp
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\version_info.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\version_resource.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\videomgr.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=..\..\client\src\volumeeffect.cpp
# End Source File
# Begin Source File

SOURCE=..\..\client\src\watermark.cpp
# End Source File
# Begin Source File

SOURCE=..\..\sound\src\wave.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=..\..\client\src\sys\win\winclientde_impl.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=..\..\client\src\sys\win\winconsole_impl.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=..\..\world\src\world_tree.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# End Group
# Begin Group "Headers"

# PROP Default_Filter "h"
# Begin Group "SDK"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\sdk\inc\cui.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\cuifont.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\cuiformattedpolystring.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\cuimessage.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\cuipolystring.h
# End Source File
# Begin Source File

SOURCE=..\..\ui\src\cuipolystring_impl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\cuitypes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\iaggregate.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\iclientshell.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\iltbaseclass.h
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

SOURCE=..\..\..\sdk\inc\iltfontmanager.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\iltinfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\iltmemory.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\iltmessage.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\iltrefcount.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\iltrenderstyles.h
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

SOURCE=..\..\..\sdk\inc\iltwidgetmanager.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\iservershell.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\ithread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\lithtech.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\ltanimtracker.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\ltassert.h
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

SOURCE=..\..\..\sdk\inc\lterror.h
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

SOURCE=..\..\..\sdk\inc\ltmodule.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\ltobjectcreate.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\ltplane.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\ltproperty.h
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

SOURCE=..\..\..\sdk\inc\ltrotation.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\ltserverobj.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\ltvector.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\server_interface.h
# End Source File
# End Group
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

SOURCE=..\..\kernel\src\sys\win\bink.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\binkvideomgrimpl.h
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

SOURCE=..\..\client\src\client_formatmgr.h
# End Source File
# Begin Source File

SOURCE=..\..\client\src\client_graphmgr.h
# End Source File
# Begin Source File

SOURCE=..\..\client\src\client_ticks.h
# End Source File
# Begin Source File

SOURCE=..\..\client\src\clientde_impl.h
# End Source File
# Begin Source File

SOURCE=..\..\client\src\clientexception.h
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

SOURCE=..\..\render_a\src\sys\d3d\clipline.h
# End Source File
# Begin Source File

SOURCE=..\..\client\src\cmoveabstract.h
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

SOURCE=..\..\shared\src\colorops.h
# End Source File
# Begin Source File

SOURCE=..\..\render_a\src\sys\d3d\common_draw.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\compress.h
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

SOURCE=..\..\client\src\consolecommands.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\physics\coordinate_frame.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\counter.h
# End Source File
# Begin Source File

SOURCE=..\..\render_a\src\sys\d3d\d3d_convar.h
# End Source File
# Begin Source File

SOURCE=..\..\render_a\src\sys\d3d\d3d_device.h
# End Source File
# Begin Source File

SOURCE=..\..\render_a\src\sys\d3d\d3d_draw.h
# End Source File
# Begin Source File

SOURCE=..\..\render_a\src\sys\d3d\d3d_renderstatemgr.h
# End Source File
# Begin Source File

SOURCE=..\..\render_a\src\sys\d3d\d3d_renderstyle.h
# End Source File
# Begin Source File

SOURCE=..\..\render_a\src\sys\d3d\d3d_utils.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\sys\win\d3dddstructs.h
# End Source File
# Begin Source File

SOURCE=..\..\render_b\src\sys\d3d\d3ddrawprim.h
# End Source File
# Begin Source File

SOURCE=..\..\render_b\src\sys\d3d\d3dtexinterface.h
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

SOURCE=..\..\client\src\debuggraphmgr.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\dhashtable.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\sys\win\dstreamopenqueuemgr.h
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

SOURCE=..\..\client\src\errorlog.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\findobj.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\ftbase.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\ftclient.h
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

SOURCE=..\..\kernel\src\sys\win\GameUtil.h
# End Source File
# Begin Source File

SOURCE=..\..\render_b\src\gendrawprim.h
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

SOURCE=..\..\kernel\src\sys\win\HError.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\HGameGuard.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\impl_common.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\input.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\interface_helpers.h
# End Source File
# Begin Source File

SOURCE=..\..\server\src\interlink.h
# End Source File
# Begin Source File

SOURCE=..\..\world\src\intersect_line.h
# End Source File
# Begin Source File

SOURCE=..\..\world\src\light_table.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\lightmap_compress.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\lightmap_planes.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\lightmapdefs.h
# End Source File
# Begin Source File

SOURCE=..\..\client\src\linesystem.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\load_pcx.h
# End Source File
# Begin Source File

SOURCE=..\..\world\src\loadstatus.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\net\src\localdriver.h
# End Source File
# Begin Source File

SOURCE=..\..\physics\src\lt_collision_mgr.h
# End Source File
# Begin Source File

SOURCE=..\..\model\src\ltb.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\ltbbox.h
# End Source File
# Begin Source File

SOURCE=..\..\client\src\ltbenchmark_impl.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\ltdirectmusic_impl.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\ltdirectmusiccontrolfile.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\ltdirectmusicloader.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\lthread.h
# End Source File
# Begin Source File

SOURCE=..\..\client\src\ltinfo_impl.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\ltmessage.h
# End Source File
# Begin Source File

SOURCE=..\..\client\src\ltmessage_client.h
# End Source File
# Begin Source File

SOURCE=..\..\server\src\ltmessage_server.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\ltmutex.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\ltthread.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\lttimer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\physics\math_phys.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\physics\matrix.h
# End Source File
# Begin Source File

SOURCE=..\..\client\src\memorywatch.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\misctools.h
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

SOURCE=..\..\kernel\src\sys\win\musicdriver.h
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

SOURCE=..\..\kernel\src\sys\win\NFMngr.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\net\src\NFPacketDebug.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\objectmgr.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\net\src\packet.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\packetdefs.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\parse_world_info.h
# End Source File
# Begin Source File

SOURCE=..\..\client\src\particlesystem.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\pixelformat.h
# End Source File
# Begin Source File

SOURCE=..\..\client\src\polygrid.h
# End Source File
# Begin Source File

SOURCE=..\..\client\src\predict.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\physics\quaternion.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\rad.h
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

SOURCE=..\..\..\sdk\inc\physics\rigid_body.h
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

SOURCE=..\..\kernel\src\sys\win\smack.h
# End Source File
# Begin Source File

SOURCE=..\..\server\src\smoveabstract.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\net\src\sys\win\socket.h
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

SOURCE=..\..\client\src\sprite.h
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

SOURCE=..\..\shared\src\strtools.h
# End Source File
# Begin Source File

SOURCE=..\..\client\src\sysclient.h
# End Source File
# Begin Source File

SOURCE=..\..\client\src\sysclientde_impl.h
# End Source File
# Begin Source File

SOURCE=..\..\client\src\sysconsole_impl.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\syscounter.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\sysddstructs.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sysdebugging.h
# End Source File
# Begin Source File

SOURCE=..\..\render_b\src\sysdrawprim.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\io\src\sysfile.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sysinput.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sysstreamsim.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\systeminfo.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\systhread.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\systimer.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sysvideo.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\text_mgr.h
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

SOURCE=..\..\shared\src\varsetter.h
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

SOURCE=..\..\kernel\src\sys\win\videomgr.h
# End Source File
# Begin Source File

SOURCE=..\..\client\src\volumeeffect.h
# End Source File
# Begin Source File

SOURCE=..\..\client\src\watermark.h
# End Source File
# Begin Source File

SOURCE=..\..\sound\src\wave.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\winbind.h
# End Source File
# Begin Source File

SOURCE=..\..\client\src\sys\win\winclientde_impl.h
# End Source File
# Begin Source File

SOURCE=..\..\client\src\sys\win\winconsole_impl.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\sys\win\winstdlterror.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\sys\win\winsync.h
# End Source File
# Begin Source File

SOURCE=..\..\world\src\world_tree.h
# End Source File
# Begin Source File

SOURCE=..\..\world\src\worldtreehelper.h
# End Source File
# End Group
# Begin Group "Modules"

# PROP Default_Filter ""
# Begin Group "World"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\world\src\world_blind_object_data.cpp
# End Source File
# Begin Source File

SOURCE=..\..\world\src\world_blind_object_data.h
# End Source File
# Begin Source File

SOURCE=..\..\world\src\world_blocker_data.cpp
# End Source File
# Begin Source File

SOURCE=..\..\world\src\world_blocker_data.h
# End Source File
# Begin Source File

SOURCE=..\..\world\src\world_blocker_math.cpp
# End Source File
# Begin Source File

SOURCE=..\..\world\src\world_blocker_math.h
# End Source File
# Begin Source File

SOURCE=..\..\world\src\world_client.h
# End Source File
# Begin Source File

SOURCE=..\..\client\src\world_client_bsp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\world\src\world_client_bsp.h
# End Source File
# Begin Source File

SOURCE=..\..\world\src\world_interface.h
# End Source File
# Begin Source File

SOURCE=..\..\world\src\world_particle_blocker_data.cpp
# End Source File
# Begin Source File

SOURCE=..\..\world\src\world_particle_blocker_data.h
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
# Begin Group "Command Line"

# PROP Default_Filter "r"
# Begin Source File

SOURCE=..\..\kernel\src\icommandlineargs.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Gm-
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\icommandlineargs.h
# End Source File
# End Group
# Begin Group "ClientFileMgr"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\client\src\client_filemgr.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\client\src\client_filemgr.h
# End Source File
# End Group
# Begin Group "Game Interfaces"

# PROP Default_Filter ""
# Begin Group "ILTModel"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\client\src\client_iltmodel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\iltmodel.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\modellt_impl.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\server\src\server_iltmodel.cpp
# End Source File
# End Group
# Begin Group "ILTCommon"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\client\src\client_iltcommon.cpp
# End Source File
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
# Begin Group "ILTCursor"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\sdk\inc\iltcursor.h
# End Source File
# Begin Source File

SOURCE=..\..\kernel\src\sys\win\lt_cursor_impl.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# End Group
# Begin Group "ILTPhysics"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\client\src\client_iltphysics.cpp
# End Source File
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
# Begin Group "ILTTransform"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\sdk\inc\ilttransform.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\src\transformlt_impl.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

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
# Begin Source File

SOURCE=..\..\sound\src\soundmgr.cpp

!IF  "$(CFG)" == "Exe_LithTech - Win32 Release"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Final"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Release Public Dev"

!ELSEIF  "$(CFG)" == "Exe_LithTech - Win32 Eval"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\sound\src\soundmgr.h
# End Source File
# End Group
# Begin Group "ILTVideoMgr"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\client\src\client_iltvideomgr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\iltvideomgr.h
# End Source File
# End Group
# End Group
# Begin Group "Resource"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\res\client.rc2
# End Source File
# Begin Source File

SOURCE=.\idr_main.ico
# End Source File
# Begin Source File

SOURCE=.\lithtech.rc
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\shared\src\interface_linkage.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# End Target
# End Project
