# Microsoft Developer Studio Project File - Name="PreProcessor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=PreProcessor - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "preprocessor60.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "preprocessor60.mak" CFG="PreProcessor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PreProcessor - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "PreProcessor - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PreProcessor - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "PreProcessor___Win32_Debug"
# PROP BASE Intermediate_Dir "PreProcessor___Win32_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\built\debug\tools\Preprocessor"
# PROP Intermediate_Dir "..\..\built\debug\tools\Preprocessor"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\Runtime\world\src" /I "..\..\Runtime\model\src" /I "..\..\Runtime\kernel\src\sys\win" /I "..\..\lithshared\stl" /I "..\..\lithshared\incs" /I "..\..\sdk\inc" /I "..\..\sdk\inc\compat" /I "..\..\Runtime\shared\src" /I "." /I ".\win" /I "..\Shared\src" /I "..\..\World" /I "..\..\shared\src" /I "..\dedit\draw" /D "_DEBUG" /D "PRE_FLOAT" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "BDEFS_MFC" /D "STDLITH_MFC" /D "PREPROCESSOR_BUILD" /D "NO_PRAGMA_LIBS" /D "LT15_COMPAT" /Yu"bdefs.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\libs\STLPORT-4.0\STLPORT" /I ".\\" /I "..\..\sdk\inc" /I "..\..\sdk\inc\physics" /I "..\shared\engine\\" /I "..\shared\world\\" /I "..\shared\model\\" /I "win" /I "..\..\..\libs\stdlith\sys\win" /I "..\..\..\libs\stdlith" /I "..\..\..\libs\regmgr" /I "..\..\..\libs\lith" /I "..\dedit\draw" /I ".\packer_pc\\" /I ".\packer_ps2\\" /I "..\dedit\lightmap" /I "..\..\libs\ltamgr" /D "_DEBUG" /D "WIN32" /D "PRE_FLOAT" /D "_WINDOWS" /D "_MBCS" /D "BDEFS_MFC" /D "STDLITH_MFC" /D "PREPROCESSOR_BUILD" /D "NO_PRAGMA_LIBS" /D "LT15_COMPAT" /D "_AFXDLL" /FR /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 winmm.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd.lib" /out:".\Debug\Processor.exe"
# ADD LINK32 winmm.lib /nologo /subsystem:windows /profile /debug /machine:I386 /nodefaultlib:"libcd.lib" /out:"..\..\built\debug\tools\Preprocessor\Processor.exe"
# Begin Custom Build
TargetPath=\Working\Engine\built\debug\tools\Preprocessor\Processor.exe
InputPath=\Working\Engine\built\debug\tools\Preprocessor\Processor.exe
SOURCE="$(InputPath)"

"..\..\..\development\tools\Processor.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\development\tools 
	copy $(TargetPath) ..\..\..\development\tools 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "PreProcessor - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "PreProcessor___Win32_Release"
# PROP BASE Intermediate_Dir "PreProcessor___Win32_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\built\release\tools\Preprocessor"
# PROP Intermediate_Dir "..\..\built\release\tools\Preprocessor"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Zi /O1 /Ob2 /I "." /I "..\..\..\..\engine\sdk\inc" /I "..\..\..\..\engine\sdk\inc\physics" /I "..\..\lib\inc" /I "..\shared\engine\\" /I "..\shared\world\\" /I "..\shared\model\\" /I "win" /I "..\..\lib\stdlith\sys\win" /I "..\..\lib\stdlith" /I "..\lib\regmgr" /I "..\..\lib\lith" /I "..\dedit\draw" /I ".\packer_pc\\" /I ".\packer_ps2\\" /D "NDEBUG" /D "PRE_FLOAT" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "BDEFS_MFC" /D "STDLITH_MFC" /D "PREPROCESSOR_BUILD" /D "NO_PRAGMA_LIBS" /D "LT15_COMPAT" /FR /Yu"bdefs.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /Ob2 /I "..\..\..\libs\STLPORT-4.0\STLPORT" /I ".\\" /I "..\..\sdk\inc" /I "..\..\sdk\inc\physics" /I "..\shared\engine\\" /I "..\shared\world\\" /I "..\shared\model\\" /I "win" /I "..\..\..\libs\stdlith\sys\win" /I "..\..\..\libs\stdlith" /I "..\..\..\libs\regmgr" /I "..\..\..\libs\lith" /I "..\dedit\draw" /I ".\packer_pc\\" /I ".\packer_ps2\\" /I "..\dedit\lightmap" /I "..\..\libs\ltamgr" /D "NDEBUG" /D "PRE_FLOAT" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "BDEFS_MFC" /D "STDLITH_MFC" /D "PREPROCESSOR_BUILD" /D "NO_PRAGMA_LIBS" /D "LT15_COMPAT" /D "_AFXDLL" /FR /Yu"bdefs.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 winmm.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc.lib" /out:"..\..\bin\release_debuginfo\Processor.exe"
# ADD LINK32 winmm.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"msvcrtd.lib" /out:"..\..\built\release\tools\Preprocessor\Processor.exe"
# SUBTRACT LINK32 /debug
# Begin Custom Build
TargetPath=\Working\Engine\built\release\tools\Preprocessor\Processor.exe
InputPath=\Working\Engine\built\release\tools\Preprocessor\Processor.exe
SOURCE="$(InputPath)"

"..\..\..\development\tools\Processor.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo copy $(TargetPath) ..\..\..\development\tools 
	copy $(TargetPath) ..\..\..\development\tools 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "PreProcessor - Win32 Debug"
# Name "PreProcessor - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Group "PCPacker Source"

# PROP Default_Filter ""
# Begin Group "NvTriStrip"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Packer_PC\NvTriStrip\NvTriStrip.cpp

!IF  "$(CFG)" == "PreProcessor - Win32 Debug"

!ELSEIF  "$(CFG)" == "PreProcessor - Win32 Release"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Packer_PC\NvTriStrip\NvTriStrip.h
# End Source File
# Begin Source File

SOURCE=.\Packer_PC\NvTriStrip\NvTriStripObjects.cpp

!IF  "$(CFG)" == "PreProcessor - Win32 Debug"

!ELSEIF  "$(CFG)" == "PreProcessor - Win32 Release"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Packer_PC\NvTriStrip\NvTriStripObjects.h
# End Source File
# Begin Source File

SOURCE=.\Packer_PC\NvTriStrip\VertexCache.cpp

!IF  "$(CFG)" == "PreProcessor - Win32 Debug"

!ELSEIF  "$(CFG)" == "PreProcessor - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Packer_PC\NvTriStrip\VertexCache.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Packer_PC\PCLMPacker.cpp
# End Source File
# Begin Source File

SOURCE=.\Packer_PC\PCRenderTree.cpp
# End Source File
# Begin Source File

SOURCE=.\Packer_PC\PCRenderTreeNode.cpp
# End Source File
# Begin Source File

SOURCE=.\Packer_PC\PCRenderTri.cpp
# End Source File
# Begin Source File

SOURCE=.\Packer_PC\PCRenderVert.cpp
# End Source File
# Begin Source File

SOURCE=.\Packer_PC\PCRenderWorld.cpp
# End Source File
# Begin Source File

SOURCE=.\Packer_PC\PCWorldPacker.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ApplyAmbientOverride.cpp
# End Source File
# Begin Source File

SOURCE=.\ApplyRenderGroups.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\world\BasePoly.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\engine\bdefs.cpp
# ADD CPP /Yc"bdefs.h"
# End Source File
# Begin Source File

SOURCE=..\shared\world\bezierpatch.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\shared\engine\bindmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\BrushToWorld.cpp
# End Source File
# Begin Source File

SOURCE=.\BspGen.cpp
# End Source File
# Begin Source File

SOURCE=.\BspHullMaker.cpp
# End Source File
# Begin Source File

SOURCE=.\CenterWorldAroundOrigin.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\engine\classbind.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\engine\conparse.cpp
# End Source File
# Begin Source File

SOURCE=.\ConvertKeyData.cpp
# End Source File
# Begin Source File

SOURCE=.\ConvertScatter.cpp
# End Source File
# Begin Source File

SOURCE=.\create_world_tree.cpp
# End Source File
# Begin Source File

SOURCE=.\CreateDecals.cpp
# End Source File
# Begin Source File

SOURCE=.\createphysicsbsp.cpp
# End Source File
# Begin Source File

SOURCE=.\CreatePolyEdges.cpp
# End Source File
# Begin Source File

SOURCE=.\createvisbsp.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\engine\dtxmgr.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\world\EditBrush.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\world\EditObjects.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\world\EditPatch.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\world\EditPlane.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\world\EditPoly.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\world\EditRegion.cpp
# End Source File
# Begin Source File

SOURCE=.\FillInGroupObjects.cpp
# End Source File
# Begin Source File

SOURCE=.\FindWorldModel.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\engine\genericprop_setup.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\engine\genltstream.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\engine\geomroutines.cpp
# End Source File
# Begin Source File

SOURCE=.\gettextureflags.cpp
# End Source File
# Begin Source File

SOURCE=.\gettextureinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\HullPoly.cpp
# End Source File
# Begin Source File

SOURCE=.\LeakFinders.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\engine\light_table.cpp
# End Source File
# Begin Source File

SOURCE=.\LightFragmentMaker.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\engine\lightmap_compress.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\engine\lightmap_planes.cpp
# End Source File
# Begin Source File

SOURCE=.\LightMapMaker.cpp
# End Source File
# Begin Source File

SOURCE=.\LMPolyTree.cpp
# End Source File
# Begin Source File

SOURCE=..\..\sdk\inc\LTEulerAngles.cpp

!IF  "$(CFG)" == "PreProcessor - Win32 Debug"

!ELSEIF  "$(CFG)" == "PreProcessor - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\sdk\inc\ltquatbase.cpp

!IF  "$(CFG)" == "PreProcessor - Win32 Debug"

!ELSEIF  "$(CFG)" == "PreProcessor - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\DEdit\draw\Navigator.cpp
# End Source File
# Begin Source File

SOURCE=.\Node.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\world\node_ops.cpp
# End Source File
# Begin Source File

SOURCE=.\Noise.cpp
# End Source File
# Begin Source File

SOURCE=.\PackerFactory.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\shared\engine\parse_world_info.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\engine\pixelformat.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\world\PrefabMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\world\PrefabRef.cpp
# End Source File
# Begin Source File

SOURCE=.\pregeometry.cpp
# End Source File
# Begin Source File

SOURCE=.\PreLeaf.cpp
# End Source File
# Begin Source File

SOURCE=.\PreLightMap.cpp
# End Source File
# Begin Source File

SOURCE=.\PrePlane.cpp
# End Source File
# Begin Source File

SOURCE=.\PrePoly.cpp
# End Source File
# Begin Source File

SOURCE=.\PrePolyFragments.cpp
# End Source File
# Begin Source File

SOURCE=.\PreProcessor.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\PreProcessorDlg.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\PreProcessorThread.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\PreSurface.cpp
# End Source File
# Begin Source File

SOURCE=.\PreWorld.cpp
# ADD BASE CPP /D "BDEFS_WINDOWS_H"
# SUBTRACT BASE CPP /D "BDEFS_MFC" /D "STDLITH_MFC" /YX /Yc /Yu
# ADD CPP /D "BDEFS_WINDOWS_H"
# SUBTRACT CPP /D "BDEFS_MFC" /D "STDLITH_MFC" /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Processing.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessingOptionsDlg.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\shared\world\PropList.cpp
# End Source File
# Begin Source File

SOURCE=.\replacetextures.cpp
# End Source File
# Begin Source File

SOURCE=.\RPDManager.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\shared\world\SpriteFile.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\engine\streamsim.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\world\TexturedPlane.cpp
# End Source File
# Begin Source File

SOURCE=.\Win\Threads.cpp
# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Unseen.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\world\UVtoOPQ.cpp
# End Source File
# Begin Source File

SOURCE=.\VisListBuilder.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\engine\world_tree.cpp
# End Source File
# Begin Source File

SOURCE=..\shared\world\WorldNode.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Group "PCPacker Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Packer_PC\PCFileIO.h
# End Source File
# Begin Source File

SOURCE=.\Packer_PC\PCLMPacker.h
# End Source File
# Begin Source File

SOURCE=.\Packer_PC\PCRenderShaders.h
# End Source File
# Begin Source File

SOURCE=.\Packer_PC\PCRenderTools.h
# End Source File
# Begin Source File

SOURCE=.\Packer_PC\PCRenderTree.h
# End Source File
# Begin Source File

SOURCE=.\Packer_PC\PCRenderTreeNode.h
# End Source File
# Begin Source File

SOURCE=.\Packer_PC\PCRenderTri.h
# End Source File
# Begin Source File

SOURCE=.\Packer_PC\PCRenderVert.h
# End Source File
# Begin Source File

SOURCE=.\Packer_PC\PCRenderWorld.h
# End Source File
# Begin Source File

SOURCE=.\Packer_PC\PCWorldPacker.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ApplyRenderGroups.h
# End Source File
# Begin Source File

SOURCE=..\shared\world\bezierpatch.h
# End Source File
# Begin Source File

SOURCE=.\BspGen.h
# End Source File
# Begin Source File

SOURCE=.\BspHullMaker.h
# End Source File
# Begin Source File

SOURCE=.\BTWPoly.h
# End Source File
# Begin Source File

SOURCE=.\CenterWorldAroundOrigin.h
# End Source File
# Begin Source File

SOURCE=.\ConvertKeyData.h
# End Source File
# Begin Source File

SOURCE=.\ConvertScatter.h
# End Source File
# Begin Source File

SOURCE=.\CreatePhysicsAABB.h
# End Source File
# Begin Source File

SOURCE=.\CreatePolyEdges.h
# End Source File
# Begin Source File

SOURCE=..\shared\world\EditPatch.h
# End Source File
# Begin Source File

SOURCE=.\FileMarker.h
# End Source File
# Begin Source File

SOURCE=.\FillInGroupObjects.h
# End Source File
# Begin Source File

SOURCE=.\FindWorldModel.h
# End Source File
# Begin Source File

SOURCE=.\HullPoly.h
# End Source File
# Begin Source File

SOURCE=.\IWorldPacker.h
# End Source File
# Begin Source File

SOURCE=.\LightFragmentMaker.h
# End Source File
# Begin Source File

SOURCE=.\LightMapMaker.h
# End Source File
# Begin Source File

SOURCE=.\LMAABB.h
# End Source File
# Begin Source File

SOURCE=.\LMPolyTree.h
# End Source File
# Begin Source File

SOURCE=.\Node.h
# End Source File
# Begin Source File

SOURCE=.\Noise.h
# End Source File
# Begin Source File

SOURCE=.\PackerFactory.h
# End Source File
# Begin Source File

SOURCE=.\PreBasePoly.h
# End Source File
# Begin Source File

SOURCE=..\shared\world\PrefabMgr.h
# End Source File
# Begin Source File

SOURCE=..\shared\world\PrefabRef.h
# End Source File
# Begin Source File

SOURCE=.\PreGeometry.h
# End Source File
# Begin Source File

SOURCE=.\PreLeaf.h
# End Source File
# Begin Source File

SOURCE=.\PreLightMap.h
# End Source File
# Begin Source File

SOURCE=.\PrePlane.h
# End Source File
# Begin Source File

SOURCE=.\PrePoly.h
# End Source File
# Begin Source File

SOURCE=.\PrePolyFragments.h
# End Source File
# Begin Source File

SOURCE=.\PreProcessor.h
# End Source File
# Begin Source File

SOURCE=.\PreprocessorBase.h
# End Source File
# Begin Source File

SOURCE=.\PreProcessorDlg.h
# End Source File
# Begin Source File

SOURCE=.\PreProcessorThread.h
# End Source File
# Begin Source File

SOURCE=.\PreWorld.h
# End Source File
# Begin Source File

SOURCE=.\Processing.h
# End Source File
# Begin Source File

SOURCE=.\ProcessingOptionsDlg.h
# End Source File
# Begin Source File

SOURCE=.\PropUtils.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\RPDManager.h
# End Source File
# Begin Source File

SOURCE=.\SetupPolyAlpha.h
# End Source File
# Begin Source File

SOURCE=.\SplitPoly.h
# End Source File
# Begin Source File

SOURCE=..\shared\world\SpriteFile.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# Begin Source File

SOURCE=..\shared\world\TexturedPlane.h
# End Source File
# Begin Source File

SOURCE=.\win\Threads.h
# End Source File
# Begin Source File

SOURCE=..\shared\world\UVtoOPQ.h
# End Source File
# Begin Source File

SOURCE=.\VisListBuilder.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\Res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\PreProcessor.ico
# End Source File
# Begin Source File

SOURCE=.\PreProcessor.rc
# End Source File
# Begin Source File

SOURCE=.\res\PreProcessor.rc2
# End Source File
# End Group
# Begin Group "libs_release"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\built\Release\libs\LTAMgr\ltamgr.lib

!IF  "$(CFG)" == "PreProcessor - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "PreProcessor - Win32 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\libs\built\Release\RegMgr\regmgr.lib

!IF  "$(CFG)" == "PreProcessor - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "PreProcessor - Win32 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Libs\built\Debug\Lib_StdLith\Lib_StdLith.lib

!IF  "$(CFG)" == "PreProcessor - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "PreProcessor - Win32 Release"

!ENDIF 

# End Source File
# End Group
# Begin Group "libs_debug"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\built\Debug\libs\LTAMgr\ltamgr.lib

!IF  "$(CFG)" == "PreProcessor - Win32 Debug"

!ELSEIF  "$(CFG)" == "PreProcessor - Win32 Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\libs\built\Debug\RegMgr\regmgr.lib

!IF  "$(CFG)" == "PreProcessor - Win32 Debug"

!ELSEIF  "$(CFG)" == "PreProcessor - Win32 Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Libs\built\Release\Lib_StdLith\Lib_StdLith.lib

!IF  "$(CFG)" == "PreProcessor - Win32 Debug"

!ELSEIF  "$(CFG)" == "PreProcessor - Win32 Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
