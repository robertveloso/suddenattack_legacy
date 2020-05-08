@echo off

setlocal

rem Parameters:
rem    "/r" - Rebuild

echo - Building All

del build.log

call build_release7 %1
call build_debug7 %1
call build_final7 %1

endlocal
