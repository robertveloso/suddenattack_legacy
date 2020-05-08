@echo off

setlocal

rem Parameters:
rem    projectconfig (debug, release, final, all)
rem    "/r" - Rebuild

set BuildType=/build

if "%2" == "/R" set BuildType=/rebuild
if "%2" == "/r" set BuildType=/rebuild

@echo on
msdev Libs.dsw /MAKE "Libs - %1" %BuildType% >> build.log
@echo off
if errorlevel 1 echo -    Found errors

endlocal
