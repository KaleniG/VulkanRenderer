@echo off
pushd ..\
call Vendor\Premake\premake5.exe vs2022
popd

IF %ERRORLEVEL% NEQ 0 ( PAUSE )