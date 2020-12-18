@echo off

set CommonCompilerFlags=-MT -nologo -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4100 -wd4189 -wd4505 -wd4101 -DGAME_INTERNAL=1 -DGAME_SLOW=1 -FC -Z7 -Fmwin32_platform_layer.map
set CommonLinkerFlags=-opt:ref user32.lib gdi32.lib

if not defined DevEnvDir (
	call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
)

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build
set starttime=%time%

REM 32-bit build
REM cl %CommonCompilerFlags% ..\code\win32_game.cpp /link -subsystem:windows,5.1 %CommonLinkerFlags%

REM 64-bit build
cl %CommonCompilerFlags% ..\src\win32_platform_layer.cpp /link %CommonLinkerFlags%

set endtime=%time%
set /a elapsed=endtime-starttime
echo Compile start = %starttime%
echo Compile end   = %endtime%
popd
