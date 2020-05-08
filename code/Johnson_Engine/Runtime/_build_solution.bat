@echo off

setlocal

rem Parameters:
rem    solutionconfig (debug, release, final)
rem    "/r" - Rebuild

set BuildType=/build

if "%2" == "/R" set BuildType=/rebuild
if "%2" == "/r" set BuildType=/rebuild

@echo on
devenv Engine.sln %BuildType% %1 >> build.log
@echo off
if errorlevel 1 echo -    Found errors

endlocal
