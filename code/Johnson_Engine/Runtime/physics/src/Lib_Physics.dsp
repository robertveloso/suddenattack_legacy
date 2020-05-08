# Microsoft Developer Studio Project File - Name="Lib_Physics" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Lib_Physics - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Lib_Physics.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Lib_Physics.mak" CFG="Lib_Physics - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Lib_Physics - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Lib_Physics - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=xicl6.exe
RSC=rc.exe

!IF  "$(CFG)" == "Lib_Physics - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\..\proj\Release\Runtime\Lib_Physics"
# PROP Intermediate_Dir "..\..\..\..\proj\Release\Runtime\Lib_Physics"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\..\..\Libs\STLPORT-4.0\STLPORT" /I "..\..\..\sdk\inc" /I "..\..\..\sdk\inc\physics" /I "..\..\..\sdk\inc\state_mgr" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\..\proj\Release\Runtime\Lib_Physics\physics.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Copying Physics.lib to SDK\lib
PostBuild_Cmds=mkdir     ..\..\..\sdk\lib\   	copy     ..\..\..\..\proj\Release\Runtime\Lib_Physics\physics.lib      ..\..\..\sdk\lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Lib_Physics - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\..\proj\Debug\Runtime\Lib_Physics"
# PROP Intermediate_Dir "..\..\..\..\proj\Debug\Runtime\Lib_Physics"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\..\Libs\STLPORT-4.0\STLPORT" /I "..\..\..\sdk\inc" /I "..\..\..\sdk\inc\physics" /I "..\..\..\sdk\inc\state_mgr" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\..\proj\Debug\Runtime\Lib_Physics\physics.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Copying Physics.lib to SDK\lib
PostBuild_Cmds=mkdir   ..\..\..\sdk\lib\Debug 	copy   ..\..\..\..\proj\Debug\Runtime\Lib_Physics\physics.lib    ..\..\..\sdk\lib\Debug
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Lib_Physics - Win32 Release"
# Name "Lib_Physics - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\aabb.cpp
# ADD CPP /Za
# End Source File
# Begin Source File

SOURCE=.\aabb_tree.cpp
# ADD CPP /Za
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\build_aabb.cpp
# ADD CPP /Za
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\collision_data.cpp
# ADD CPP /Za
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\collision_notify.cpp
# ADD CPP /Za
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\collision_object.cpp
# ADD CPP /Za
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\collision_stream.cpp
# End Source File
# Begin Source File

SOURCE=.\coordinate_frame.cpp
# ADD CPP /Za
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\cylinder.cpp
# ADD CPP /Za
# End Source File
# Begin Source File

SOURCE=.\gjk.cpp
# ADD CPP /Za
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\math_phys.cpp
# ADD CPP /Za
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\move_object.cpp
# ADD CPP /Za
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\obb.cpp
# ADD CPP /Za
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\quaternion.cpp
# ADD CPP /Za
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\rigid_body.cpp
# ADD CPP /Za
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\sphere.cpp
# ADD CPP /Za
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\triangle.cpp
# ADD CPP /Za
# SUBTRACT CPP /YX
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\sdk\inc\physics\aabb.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\physics\aabb_tree.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\physics\build_aabb.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\physics\collision_data.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\physics\collision_mgr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\physics\collision_notify.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\physics\collision_object.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\physics\collision_stream.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\physics\coordinate_frame.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\physics\cylinder.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\physics\gjk.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\libltphysics.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\physics\math_phys.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\physics\matrix.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\physics\move_object.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\physics\obb.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\physics\player_movement.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\physics\quaternion.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\physics\rigid_body.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\physics\sphere.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\physics\triangle.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sdk\inc\physics\vector.h
# End Source File
# End Group
# End Target
# End Project
