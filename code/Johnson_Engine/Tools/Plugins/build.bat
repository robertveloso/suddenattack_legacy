@echo off

if exist build.log del build.log

echo  -- build tools common plugins

call ..\..\..\BuildProject.bat MaxModelExport\MaxModelExport.dsp "MaxModelExport - ALL" %1
call ..\..\..\BuildProject.bat MaxWorldImport\MaxWorldImport.dsp "MaxWorldImport - ALL" %1
call ..\..\..\BuildProject.bat MaxWorldExport\MaxWorldExport.dsp "MaxWorldExport - ALL" %1
call ..\..\..\BuildProject.bat MayaModelExport\MayaModelExport.dsp "MayaModelExport - ALL" %1
call ..\..\..\BuildProject.bat MayaWorldExport\MayaWorldExport.dsp "MayaWorldExport - ALL" %1
call ..\..\..\BuildProject.bat MayaDTXLoader\MayaDTXLoader.dsp "MayaDTXLoader - ALL" %1
