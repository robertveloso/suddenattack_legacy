@echo off
setlocal


rem Parameters:
rem    "/r" - Rebuild

call _build_project all %1

endlocal
