
@echo off
setlocal


rem Parameters:
rem    "/r" - Rebuild

call _build_project eval %1

endlocal
