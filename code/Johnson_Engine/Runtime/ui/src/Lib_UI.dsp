# Microsoft Developer Studio Project File - Name="Lib_UI" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Lib_UI - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Lib_UI.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Lib_UI.mak" CFG="Lib_UI - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Lib_UI - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Lib_UI - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Lib_UI - Win32 Final" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/SA/Johnson_Code/Johnson_Engine/Runtime/ui/src", NCYBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Lib_UI - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\built\Release\Libs\Lib_UI"
# PROP Intermediate_Dir "..\..\..\built\Release\Libs\Lib_UI"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /Ob2 /I "..\..\..\..\libs\stdlith" /I "..\..\..\..\libs\lith" /I "..\..\..\libs\rezmgr" /I "..\..\controlfilemgr" /I "..\..\crtcompat" /I "..\..\lithtemplate" /I "." /I "..\..\client\src" /I "..\..\kernel\mem\src" /I "..\..\kernel\io\src" /I "..\..\kernel\src" /I "..\..\kernel\src\sys\win" /I "..\..\model\src" /I "..\..\sound\src" /I "..\..\server\src" /I "..\..\shared\src\sys\win" /I "..\..\shared\src" /I "..\..\world\src" /I "..\..\..\sdk\inc" /I "..\..\..\sdk\inc\compat" /I "..\..\kernel\net\src" /I "..\..\render_b\src" /I "..\..\mpm\src" /I "..\..\comm\src" /I "..\..\ui\src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "__D3D" /D "__D3DREND" /D "NO_PROFILE" /D "NOT_USING_FULL" /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\built\Release\Libs\Lib_UI.lib"

!ELSEIF  "$(CFG)" == "Lib_UI - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\..\Johnson_Built\Johnson_Engine\Debug\Libs\Lib_UI"
# PROP Intermediate_Dir "..\..\..\..\Johnson_Built\Johnson_Engine\Debug\Libs\Lib_UI"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\Johnson_Libs\stdlith" /I "..\..\..\..\Johnson_Libs\lith" /I "..\..\..\libs\rezmgr" /I "..\..\controlfilemgr" /I "..\..\crtcompat" /I "..\..\lithtemplate" /I "." /I "..\..\client\src" /I "..\..\kernel\mem\src" /I "..\..\kernel\io\src" /I "..\..\kernel\src" /I "..\..\kernel\src\sys\win" /I "..\..\model\src" /I "..\..\sound\src" /I "..\..\server\src" /I "..\..\shared\src\sys\win" /I "..\..\shared\src" /I "..\..\world\src" /I "..\..\..\sdk\inc" /I "..\..\..\sdk\inc\compat" /I "..\..\kernel\net\src" /I "..\..\render_b\src" /I "..\..\mpm\src" /I "..\..\comm\src" /I "..\..\ui\src" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__D3D" /D "__D3DREND" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\..\Lib\Debug\Lib_UI.lib"

!ELSEIF  "$(CFG)" == "Lib_UI - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Final"
# PROP BASE Intermediate_Dir "Final"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\built\Final\Libs\Lib_UI"
# PROP Intermediate_Dir "..\..\..\built\Final\Libs\Lib_UI"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\libs\stdlith" /I "..\..\..\libs\lith" /I "..\..\..\libs\rezmgr" /I "..\..\controlfilemgr" /I "..\..\crtcompat" /I "..\..\lithtemplate" /I "." /I "..\..\client\src" /I "..\..\kernel\mem\src" /I "..\..\kernel\io\src" /I "..\..\kernel\src" /I "..\..\kernel\src\sys\win" /I "..\..\model\src" /I "..\..\sound\src" /I "..\..\server\src" /I "..\..\shared\src\sys\win" /I "..\..\shared\src" /I "..\..\world\src" /I "..\..\..\sdk\inc" /I "..\..\..\sdk\inc\compat" /I "..\..\kernel\net\src" /I "..\..\render_b\src" /I "..\..\mpm\src" /I "..\..\comm\src" /I "..\..\ui\src" /I "..\..\ui\src\freetype-2.0.1\src" /I "..\..\ui\src\freetype-2.0.1\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "__D3D" /D "__D3DREND" /FR /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /Ob2 /I "..\..\..\..\libs\stdlith" /I "..\..\..\..\libs\lith" /I "..\..\..\libs\rezmgr" /I "..\..\controlfilemgr" /I "..\..\crtcompat" /I "..\..\lithtemplate" /I "." /I "..\..\client\src" /I "..\..\kernel\mem\src" /I "..\..\kernel\io\src" /I "..\..\kernel\src" /I "..\..\kernel\src\sys\win" /I "..\..\model\src" /I "..\..\sound\src" /I "..\..\server\src" /I "..\..\shared\src\sys\win" /I "..\..\shared\src" /I "..\..\world\src" /I "..\..\..\sdk\inc" /I "..\..\..\sdk\inc\compat" /I "..\..\kernel\net\src" /I "..\..\render_b\src" /I "..\..\mpm\src" /I "..\..\comm\src" /I "..\..\ui\src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "__D3D" /D "__D3DREND" /D "_FINAL" /D "NO_PROFILE" /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\built\Final\Libs\Lib_UI.lib"

!ENDIF 

# Begin Target

# Name "Lib_UI - Win32 Release"
# Name "Lib_UI - Win32 Debug"
# Name "Lib_UI - Win32 Final"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\cuibase.cpp
# End Source File
# Begin Source File

SOURCE=.\cuibase_impl.cpp
# End Source File
# Begin Source File

SOURCE=.\cuibitmapfont.cpp
# End Source File
# Begin Source File

SOURCE=.\cuibutton.cpp
# End Source File
# Begin Source File

SOURCE=.\cuibutton_impl.cpp
# End Source File
# Begin Source File

SOURCE=.\cuicheck.cpp
# End Source File
# Begin Source File

SOURCE=.\cuicheck_impl.cpp
# End Source File
# Begin Source File

SOURCE=.\sys\d3d\cuidebug.cpp
# End Source File
# Begin Source File

SOURCE=.\cuidropdownlist.cpp
# End Source File
# Begin Source File

SOURCE=.\cuidropdownlist_impl.cpp
# End Source File
# Begin Source File

SOURCE=.\cuifont_impl.cpp
# End Source File
# Begin Source File

SOURCE=.\cuifontmanager.cpp
# End Source File
# Begin Source File

SOURCE=.\cuiformattedpolystring.cpp
# End Source File
# Begin Source File

SOURCE=.\cuiformattedpolystring_impl.cpp
# End Source File
# Begin Source File

SOURCE=.\cuiinterval.cpp
# End Source File
# Begin Source File

SOURCE=.\cuiinterval_impl.cpp
# End Source File
# Begin Source File

SOURCE=.\cuilinklist.cpp
# End Source File
# Begin Source File

SOURCE=.\cuilist.cpp
# End Source File
# Begin Source File

SOURCE=.\cuilist_impl.cpp
# End Source File
# Begin Source File

SOURCE=.\cuimessagequeue.cpp
# End Source File
# Begin Source File

SOURCE=.\cuioption.cpp
# End Source File
# Begin Source File

SOURCE=.\cuioption_impl.cpp
# End Source File
# Begin Source File

SOURCE=.\cuipolystring.cpp
# End Source File
# Begin Source File

SOURCE=.\cuipolystring_impl.cpp
# End Source File
# Begin Source File

SOURCE=.\cuipolytex.cpp
# End Source File
# Begin Source File

SOURCE=.\cuiprogress.cpp
# End Source File
# Begin Source File

SOURCE=.\cuiprogress_impl.cpp
# End Source File
# Begin Source File

SOURCE=.\cuiredblack.cpp
# End Source File
# Begin Source File

SOURCE=.\cuirenderstate.cpp
# End Source File
# Begin Source File

SOURCE=.\cuislider.cpp
# End Source File
# Begin Source File

SOURCE=.\cuislider_impl.cpp
# End Source File
# Begin Source File

SOURCE=.\cuistaticimage.cpp
# End Source File
# Begin Source File

SOURCE=.\cuistaticimage_impl.cpp
# End Source File
# Begin Source File

SOURCE=.\cuistatictext.cpp
# End Source File
# Begin Source File

SOURCE=.\cuistatictext_impl.cpp
# End Source File
# Begin Source File

SOURCE=.\cuivectorfont.cpp
# End Source File
# Begin Source File

SOURCE=.\cuiwidget.cpp
# End Source File
# Begin Source File

SOURCE=.\cuiwidget_impl.cpp
# End Source File
# Begin Source File

SOURCE=.\cuiwidgetmanager.cpp
# End Source File
# Begin Source File

SOURCE=.\cuiwindow.cpp
# End Source File
# Begin Source File

SOURCE=.\cuiwindow_impl.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\sdk\inc\cui.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\cuibase.h
# End Source File
# Begin Source File

SOURCE=.\cuibase_impl.h
# End Source File
# Begin Source File

SOURCE=.\cuibitmapfont.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\cuibutton.h
# End Source File
# Begin Source File

SOURCE=.\cuibutton_impl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\cuicheck.h
# End Source File
# Begin Source File

SOURCE=.\cuicheck_impl.h
# End Source File
# Begin Source File

SOURCE=.\cuidebug.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\cuidropdownlist.h
# End Source File
# Begin Source File

SOURCE=.\cuidropdownlist_impl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\cuifont.h
# End Source File
# Begin Source File

SOURCE=.\cuifont_impl.h
# End Source File
# Begin Source File

SOURCE=.\cuifontmanager.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\cuiformattedpolystring.h
# End Source File
# Begin Source File

SOURCE=.\cuiformattedpolystring_impl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\cuiinterval.h
# End Source File
# Begin Source File

SOURCE=.\cuiinterval_impl.h
# End Source File
# Begin Source File

SOURCE=.\cuilinklist.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\cuilist.h
# End Source File
# Begin Source File

SOURCE=.\cuilist_impl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\cuimessage.h
# End Source File
# Begin Source File

SOURCE=.\cuimessagequeue.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\cuioption.h
# End Source File
# Begin Source File

SOURCE=.\cuioption_impl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\cuipolystring.h
# End Source File
# Begin Source File

SOURCE=.\cuipolystring_impl.h
# End Source File
# Begin Source File

SOURCE=.\cuipolytex.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\cuiprogress.h
# End Source File
# Begin Source File

SOURCE=.\cuiprogress_impl.h
# End Source File
# Begin Source File

SOURCE=.\cuiredblack.h
# End Source File
# Begin Source File

SOURCE=.\cuirenderstate.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\cuislider.h
# End Source File
# Begin Source File

SOURCE=.\cuislider_impl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\cuistaticimage.h
# End Source File
# Begin Source File

SOURCE=.\cuistaticimage_impl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\cuistatictext.h
# End Source File
# Begin Source File

SOURCE=.\cuistatictext_impl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\cuitypes.h
# End Source File
# Begin Source File

SOURCE=.\cuivectorfont.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\cuiwidget.h
# End Source File
# Begin Source File

SOURCE=.\cuiwidget_impl.h
# End Source File
# Begin Source File

SOURCE=.\cuiwidgetmanager.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\cuiwindow.h
# End Source File
# Begin Source File

SOURCE=.\cuiwindow_impl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\iltfontmanager.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\iltwidgetmanager.h
# End Source File
# End Group
# End Target
# End Project
