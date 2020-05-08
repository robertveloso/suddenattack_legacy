@echo off

setlocal

rem Parameters:
rem    "/r" - Rebuild

set BuildType=/build

if "%1" == "/R" set BuildType=/rebuild
if "%1" == "/r" set BuildType=/rebuild


echo - Building Release

call _build_solution release %1


endlocal
