# Microsoft Developer Studio Project File - Name="LIB_D3DRender" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=LIB_D3DRender - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LIB_D3DRender.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LIB_D3DRender.mak" CFG="LIB_D3DRender - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LIB_D3DRender - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "LIB_D3DRender - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "LIB_D3DRender - Win32 Final" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/SA/Johnson_Code/Johnson_Engine/Runtime/render_a/src/sys/d3d", NLXBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LIB_D3DRender - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\..\..\built\Release\Libs\Lib_D3DRender"
# PROP Intermediate_Dir "..\..\..\..\..\built\Release\Libs\Lib_D3DRender"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /Ob2 /I "..\..\..\..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\..\..\..\client\src" /I "." /I "..\..\..\..\kernel\mem\src" /I "..\..\..\..\shared\src" /I "..\..\..\..\kernel\src" /I "..\..\..\..\..\sdk\inc" /I "..\..\..\..\..\..\libs\stdlith" /I "..\..\..\..\shared\src\sys\win" /I "..\..\..\..\world\src" /I "..\..\..\..\model\src" /I "..\..\..\..\model\src\sys\d3d" /I "..\..\..\..\kernel\src\sys\win" /I "..\..\..\..\kernel\io\src" /I "..\..\..\..\lithtemplate" /I "..\..\..\..\..\sdk\inc\physics" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__D3DREND" /D "__D3D" /D "MODEL_SUPPORT_ABC" /D "NO_PROFILE" /Fr /Yu"precompile.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\..\..\built\Release\Libs\LIB_D3DRender.lib"

!ELSEIF  "$(CFG)" == "LIB_D3DRender - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\..\..\..\Johnson_Built\Johnson_Engine\Debug\Libs\Lib_D3DRender"
# PROP Intermediate_Dir "..\..\..\..\..\..\Johnson_Built\Johnson_Engine\Debug\Libs\Lib_D3DRender"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /ZI /Od /I "..\..\..\..\..\..\Johnson_Libs\STLPORT-4.0\STLPORT" /I "..\..\..\..\client\src" /I "." /I "..\..\..\..\kernel\mem\src" /I "..\..\..\..\shared\src" /I "..\..\..\..\kernel\src" /I "..\..\..\..\..\sdk\inc" /I "..\..\..\..\..\..\Johnson_Libs\stdlith" /I "..\..\..\..\shared\src\sys\win" /I "..\..\..\..\world\src" /I "..\..\..\..\model\src" /I "..\..\..\..\model\src\sys\d3d" /I "..\..\..\..\kernel\src\sys\win" /I "..\..\..\..\kernel\io\src" /I "..\..\..\..\lithtemplate" /I "..\..\..\..\..\sdk\inc\physics" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "__D3DREND" /D "__D3D" /D "MODEL_SUPPORT_ABC" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\..\..\..\Lib\Debug\LIB_D3DRender.lib"

!ELSEIF  "$(CFG)" == "LIB_D3DRender - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Final"
# PROP BASE Intermediate_Dir "Final"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\..\..\built\Final\Libs\Lib_D3DRender"
# PROP Intermediate_Dir "..\..\..\..\..\built\Final\Libs\Lib_D3DRender"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "..\..\..\..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\..\..\..\client\src" /I "." /I "..\..\..\..\kernel\mem\src" /I "..\..\..\..\shared\src" /I "..\..\..\..\kernel\src" /I "..\..\..\..\..\sdk\inc" /I "..\..\..\..\..\..\libs\stdlith" /I "..\..\..\..\shared\src\sys\win" /I "..\..\..\..\world\src" /I "..\..\..\..\model\src" /I "..\..\..\..\model\src\sys\d3d" /I "..\..\..\..\kernel\src\sys\win" /I "..\..\..\..\kernel\io\src" /I "..\..\..\..\lithtemplate" /I "..\..\..\..\..\sdk\inc\physics" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__D3DREND" /D "__D3D" /D "MODEL_SUPPORT_ABC" /Fr /Yu"precompile.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /Ob2 /I "..\..\..\..\..\..\libs\STLPORT-4.0\STLPORT" /I "..\..\..\..\client\src" /I "." /I "..\..\..\..\kernel\mem\src" /I "..\..\..\..\shared\src" /I "..\..\..\..\kernel\src" /I "..\..\..\..\..\sdk\inc" /I "..\..\..\..\..\..\libs\stdlith" /I "..\..\..\..\shared\src\sys\win" /I "..\..\..\..\world\src" /I "..\..\..\..\model\src" /I "..\..\..\..\model\src\sys\d3d" /I "..\..\..\..\kernel\src\sys\win" /I "..\..\..\..\kernel\io\src" /I "..\..\..\..\lithtemplate" /I "..\..\..\..\..\sdk\inc\physics" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__D3DREND" /D "__D3D" /D "MODEL_SUPPORT_ABC" /D "_FINAL" /D "NO_PROFILE" /Fr /Yu"precompile.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\..\..\built\Final\Libs\LIB_D3DRender.lib"

!ENDIF 

# Begin Target

# Name "LIB_D3DRender - Win32 Release"
# Name "LIB_D3DRender - Win32 Debug"
# Name "LIB_D3DRender - Win32 Final"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "shadows"

# PROP Default_Filter ""
# Begin Group "Shadow Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\shadows\d3dshadowtexture.h
# End Source File
# Begin Source File

SOURCE=.\iaggregateshader.h
# End Source File
# Begin Source File

SOURCE=.\modelshadowshader.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\shadows\d3dshadowtexture.cpp
# End Source File
# Begin Source File

SOURCE=.\modelshadowshader.cpp
# End Source File
# End Group
# Begin Group "Texture Script Source"

# PROP Default_Filter "*.cpp"
# Begin Source File

SOURCE=.\texturescriptdefs.h
# End Source File
# Begin Source File

SOURCE=.\texturescriptevaluator.h
# End Source File
# Begin Source File

SOURCE=.\texturescriptinstance.cpp
# End Source File
# Begin Source File

SOURCE=.\texturescriptinstance.h
# End Source File
# Begin Source File

SOURCE=.\texturescriptinterpreter.cpp

!IF  "$(CFG)" == "LIB_D3DRender - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "LIB_D3DRender - Win32 Debug"

!ELSEIF  "$(CFG)" == "LIB_D3DRender - Win32 Final"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\texturescriptinterpreter.h
# End Source File
# Begin Source File

SOURCE=.\texturescriptmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\texturescriptmgr.h
# End Source File
# Begin Source File

SOURCE=.\texturescriptvarmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\texturescriptvarmgr.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\3d_ops.cpp
# End Source File
# Begin Source File

SOURCE=.\AABB.cpp
# End Source File
# Begin Source File

SOURCE=.\common_draw.cpp
# End Source File
# Begin Source File

SOURCE=.\common_init.cpp
# End Source File
# Begin Source File

SOURCE=.\common_stuff.cpp
# End Source File
# Begin Source File

SOURCE=.\d3d_device.cpp
# End Source File
# Begin Source File

SOURCE=.\d3d_device_wrapper.cpp
# End Source File
# Begin Source File

SOURCE=.\d3d_draw.cpp
# End Source File
# Begin Source File

SOURCE=.\d3d_drawsky.cpp
# End Source File
# Begin Source File

SOURCE=.\d3d_init.cpp
# End Source File
# Begin Source File

SOURCE=.\d3d_optimizedsurface.cpp
# End Source File
# Begin Source File

SOURCE=.\d3d_renderblock.cpp
# End Source File
# Begin Source File

SOURCE=.\d3d_rendershader_dynamiclight.cpp
# End Source File
# Begin Source File

SOURCE=.\d3d_rendershader_glow.cpp
# End Source File
# Begin Source File

SOURCE=.\d3d_rendershader_gouraud.cpp

!IF  "$(CFG)" == "LIB_D3DRender - Win32 Release"

# ADD CPP /FAs

!ELSEIF  "$(CFG)" == "LIB_D3DRender - Win32 Debug"

!ELSEIF  "$(CFG)" == "LIB_D3DRender - Win32 Final"

# ADD BASE CPP /FAs
# ADD CPP /FAs

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\d3d_rendershader_gouraud_fullbright.cpp
# End Source File
# Begin Source File

SOURCE=.\d3d_rendershader_lightmap.cpp
# End Source File
# Begin Source File

SOURCE=.\d3d_renderstatemgr.cpp
# End Source File
# Begin Source File

SOURCE=.\d3d_renderstyle.cpp
# End Source File
# Begin Source File

SOURCE=.\d3d_renderstyleinterface.cpp
# End Source File
# Begin Source File

SOURCE=.\d3d_renderworld.cpp
# End Source File
# Begin Source File

SOURCE=.\d3d_renderworld_occluder.cpp
# End Source File
# Begin Source File

SOURCE=.\d3d_shell.cpp
# End Source File
# Begin Source File

SOURCE=.\d3d_surface.cpp
# End Source File
# Begin Source File

SOURCE=.\d3d_texture.cpp
# End Source File
# Begin Source File

SOURCE=.\d3d_utils.cpp
# End Source File
# Begin Source File

SOURCE=.\d3d_viewparams.cpp
# End Source File
# Begin Source File

SOURCE=.\d3dmeshrendobj_rigid.cpp
# End Source File
# Begin Source File

SOURCE=.\d3dmeshrendobj_skel.cpp
# End Source File
# Begin Source File

SOURCE=.\d3dmeshrendobj_vertanim.cpp
# End Source File
# Begin Source File

SOURCE=.\d3drender.rc
# End Source File
# Begin Source File

SOURCE=.\devicelightlist.cpp
# End Source File
# Begin Source File

SOURCE=.\dirtyrect.cpp
# End Source File
# Begin Source File

SOURCE=.\draw_canvas.cpp
# End Source File
# Begin Source File

SOURCE=.\drawlight.cpp
# End Source File
# Begin Source File

SOURCE=.\drawlinesystem.cpp
# End Source File
# Begin Source File

SOURCE=.\drawmodel.cpp
# End Source File
# Begin Source File

SOURCE=.\drawobjects.cpp
# End Source File
# Begin Source File

SOURCE=.\drawparticles.cpp
# End Source File
# Begin Source File

SOURCE=.\drawparticles_A.cpp
# End Source File
# Begin Source File

SOURCE=.\drawpolygrid.cpp
# End Source File
# Begin Source File

SOURCE=.\drawsky.cpp
# End Source File
# Begin Source File

SOURCE=.\drawsprite.cpp
# End Source File
# Begin Source File

SOURCE=.\drawvolumeeffects.cpp
# End Source File
# Begin Source File

SOURCE=.\drawworldmodel.cpp
# End Source File
# Begin Source File

SOURCE=.\ltpixelshadermgr.cpp
# End Source File
# Begin Source File

SOURCE=.\LTShaderDeviceStateImp.cpp
# End Source File
# Begin Source File

SOURCE=.\ltvertexshadermgr.cpp
# End Source File
# Begin Source File

SOURCE=.\memstats_world.cpp
# End Source File
# Begin Source File

SOURCE=.\modeldebug.cpp
# End Source File
# Begin Source File

SOURCE=.\objectgroupmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\precompile.cpp
# ADD CPP /Yc"precompile.h"
# End Source File
# Begin Source File

SOURCE=.\relevantlightlist.cpp
# End Source File
# Begin Source File

SOURCE=.\rendererconsolevars.cpp
# End Source File
# Begin Source File

SOURCE=.\rendererframestats.cpp
# End Source File
# Begin Source File

SOURCE=.\renderlight.cpp
# End Source File
# Begin Source File

SOURCE=.\rendermodelinfolist.cpp
# End Source File
# Begin Source File

SOURCE=.\rendermodelpiecelist.cpp
# End Source File
# Begin Source File

SOURCE=.\rendershadowlist.cpp
# End Source File
# Begin Source File

SOURCE=.\renderstylelookuptables.cpp
# End Source File
# Begin Source File

SOURCE=.\renderstylemap.cpp
# End Source File
# Begin Source File

SOURCE=.\screenglowmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\setupmodel.cpp
# End Source File
# Begin Source File

SOURCE=.\setuptouchinglights.cpp
# End Source File
# Begin Source File

SOURCE=.\tagnodes.cpp
# End Source File
# Begin Source File

SOURCE=.\VertexBufferController.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\3d_ops.h
# End Source File
# Begin Source File

SOURCE=.\AABB.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\model\src\animtracker.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\shared\src\bdefs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\client\src\client_formatmgr.h
# End Source File
# Begin Source File

SOURCE=.\clipline.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\shared\src\colorops.h
# End Source File
# Begin Source File

SOURCE=.\common_draw.h
# End Source File
# Begin Source File

SOURCE=.\common_init.h
# End Source File
# Begin Source File

SOURCE=.\common_stuff.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\shared\src\conparse.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\kernel\src\sys\win\counter.h
# End Source File
# Begin Source File

SOURCE=.\d3d_convar.h
# End Source File
# Begin Source File

SOURCE=.\d3d_defs.h
# End Source File
# Begin Source File

SOURCE=.\d3d_device.h
# End Source File
# Begin Source File

SOURCE=.\d3d_device_wrapper.h
# End Source File
# Begin Source File

SOURCE=.\d3d_draw.h
# End Source File
# Begin Source File

SOURCE=.\d3d_init.h
# End Source File
# Begin Source File

SOURCE=.\d3d_renderblock.h
# End Source File
# Begin Source File

SOURCE=.\d3d_rendershader.h
# End Source File
# Begin Source File

SOURCE=.\d3d_rendershader_base.h
# End Source File
# Begin Source File

SOURCE=.\d3d_rendershader_default.h
# End Source File
# Begin Source File

SOURCE=.\d3d_rendershader_dynamiclight.h
# End Source File
# Begin Source File

SOURCE=.\d3d_rendershader_glow.h
# End Source File
# Begin Source File

SOURCE=.\d3d_rendershader_gouraud.h
# End Source File
# Begin Source File

SOURCE=.\d3d_rendershader_gouraud_fullbright.h
# End Source File
# Begin Source File

SOURCE=.\d3d_rendershader_lightmap.h
# End Source File
# Begin Source File

SOURCE=.\d3d_renderstatemgr.h
# End Source File
# Begin Source File

SOURCE=.\d3d_renderstyle.h
# End Source File
# Begin Source File

SOURCE=.\d3d_renderstyleinterface.h
# End Source File
# Begin Source File

SOURCE=.\d3d_renderworld.h
# End Source File
# Begin Source File

SOURCE=.\d3d_renderworld_occluder.h
# End Source File
# Begin Source File

SOURCE=.\d3d_shell.h
# End Source File
# Begin Source File

SOURCE=.\d3d_surface.h
# End Source File
# Begin Source File

SOURCE=.\d3d_texture.h
# End Source File
# Begin Source File

SOURCE=.\d3d_utils.h
# End Source File
# Begin Source File

SOURCE=.\d3d_viewparams.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\shared\src\sys\win\d3dddstructs.h
# End Source File
# Begin Source File

SOURCE=.\d3dmeshrendobj_rigid.h
# End Source File
# Begin Source File

SOURCE=.\d3dmeshrendobj_skel.h
# End Source File
# Begin Source File

SOURCE=.\d3dmeshrendobj_vertanim.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\world\src\de_mainworld.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\kernel\mem\src\de_memory.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\world\src\de_objects.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\world\src\de_sprite.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\world\src\de_world.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\shared\src\DebugGeometry.h
# End Source File
# Begin Source File

SOURCE=.\devicelightlist.h
# End Source File
# Begin Source File

SOURCE=.\dirtyrect.h
# End Source File
# Begin Source File

SOURCE=.\draw_canvas.h
# End Source File
# Begin Source File

SOURCE=.\drawlight.h
# End Source File
# Begin Source File

SOURCE=.\drawobjects.h
# End Source File
# Begin Source File

SOURCE=.\drawsky.h
# End Source File
# Begin Source File

SOURCE=.\drawworldmodel.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\kernel\src\dsys.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\kernel\src\sys\win\dsys_interface.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\shared\src\dtxmgr.h
# End Source File
# Begin Source File

SOURCE=.\FixedPoint.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\world\src\fullintersectline.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\shared\src\gamemath.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\shared\src\geometry.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\shared\src\geomroutines.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\sdk\inc\iclientshell.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\sdk\inc\iltbenchmark.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\sdk\inc\iltclient.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\sdk\inc\iltcommon.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\sdk\inc\iltcsbase.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\sdk\inc\iltdirectmusic.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\sdk\inc\iltdrawprim.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\sdk\inc\iltinfo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\sdk\inc\iltmath.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\sdk\inc\iltrenderstyles.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\sdk\inc\iltsoundmgr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\sdk\inc\iltspritecontrol.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\sdk\inc\iltstream.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\sdk\inc\ilttexinterface.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\kernel\src\sys\win\interface_helpers.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\world\src\intersect_line.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\world\src\light_table.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\shared\src\lightmap_compress.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\sdk\inc\lithtech.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\sdk\inc\ltanimtracker.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\sdk\inc\ltassert.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\model\src\ltb.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\sdk\inc\ltbasedefs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\sdk\inc\ltbasetypes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\sdk\inc\ltcodes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\kernel\src\sys\win\lthread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\sdk\inc\ltinteger.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\sdk\inc\ltlink.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\sdk\inc\ltmatrix.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\sdk\inc\ltmodule.h
# End Source File
# Begin Source File

SOURCE=.\ltpixelshadermgr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\sdk\inc\ltplane.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\sdk\inc\ltpvalue.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\sdk\inc\ltquatbase.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\sdk\inc\ltrect.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\sdk\inc\ltrenderstyle.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\sdk\inc\ltrotation.h
# End Source File
# Begin Source File

SOURCE=.\LTShaderDeviceStateImp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\sdk\inc\ltvector.h
# End Source File
# Begin Source File

SOURCE=.\ltvertexshadermgr.h
# End Source File
# Begin Source File

SOURCE=.\memstats_world.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\model\src\model.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\model\src\model_ops.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\model\src\modelallocations.h
# End Source File
# Begin Source File

SOURCE=.\modeldebug.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\shared\src\nexus.h
# End Source File
# Begin Source File

SOURCE=.\objectgroupmgr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\shared\src\objectmgr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\shared\src\pixelformat.h
# End Source File
# Begin Source File

SOURCE=.\polyclip.h
# End Source File
# Begin Source File

SOURCE=.\precompile.h
# End Source File
# Begin Source File

SOURCE=.\relevantlightlist.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\kernel\src\sys\win\render.h
# End Source File
# Begin Source File

SOURCE=.\rendererconsolevars.h
# End Source File
# Begin Source File

SOURCE=.\rendererframestats.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\shared\src\renderinfostruct.h
# End Source File
# Begin Source File

SOURCE=.\renderlight.h
# End Source File
# Begin Source File

SOURCE=.\rendermodelinfolist.h
# End Source File
# Begin Source File

SOURCE=.\rendermodelpiecelist.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\shared\src\RenderObject.h
# End Source File
# Begin Source File

SOURCE=.\rendershadowlist.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\shared\src\sys\win\renderstruct.h
# End Source File
# Begin Source File

SOURCE=.\renderstylelookuptables.h
# End Source File
# Begin Source File

SOURCE=.\renderstylemap.h
# End Source File
# Begin Source File

SOURCE=.\screenglowmgr.h
# End Source File
# Begin Source File

SOURCE=.\setupmodel.h
# End Source File
# Begin Source File

SOURCE=.\setuptouchinglights.h
# End Source File
# Begin Source File

SOURCE=.\SimD_Utilities.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\kernel\src\syscounter.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\shared\src\sysddstructs.h
# End Source File
# Begin Source File

SOURCE=.\tagnodes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\model\src\transformmaker.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\shared\src\version_info.h
# End Source File
# Begin Source File

SOURCE=.\VertexBufferController.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\world\src\visit_pvs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\world\src\world_tree.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\world\src\worldtreehelper.h
# End Source File
# End Group
# End Target
# End Project
