
@echo off
setlocal


rem Parameters:
rem    "/r" - Rebuild

call _build_project final %1

endlocal
