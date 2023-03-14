set ROOTDIR=%~dp0

:: This removes the trailing \ from the path
set ROOTDIR=%ROOTDIR:~0,-1%

set PROJECT=Hera
set PROJECT_DIR=%ROOTDIR%\%PROJECT%
set UPROJECT_PATH=%PROJECT_DIR%\%PROJECT%.uproject

set UE_DIR=C:\UnrealEngine\UE_5.1.0
set UEEDITOR_EXE=%UE_DIR%\Engine\Binaries\Win64\UnrealEditor.exe
set BUILD_BAT=%UE_DIR%\Engine\Build\BatchFiles\Build.bat