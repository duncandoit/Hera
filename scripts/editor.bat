@echo off

call %~dp0\vars.bat

start "" "%UEEDITOR_EXE%" "%UPROJECT_PATH%" %*