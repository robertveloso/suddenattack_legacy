
@echo off
setlocal


rem Parameters:
rem    "/r" - Rebuild

call _build_project debug %1

endlocal
