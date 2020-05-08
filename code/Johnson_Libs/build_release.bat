
@echo off
setlocal


rem Parameters:
rem    "/r" - Rebuild

call _build_project release %1

endlocal
