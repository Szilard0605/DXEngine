@echo off
pushd %~dp0\..\
call ThirdParty\premake\premake5.exe vs2019
popd
PAUSE
