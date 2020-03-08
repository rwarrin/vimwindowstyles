@echo off

REM SET CompilerFlags=/nologo /Z7 /Od /Oi /Gm- /FC /LD
SET CompilerFlags=/nologo /O2 /Oi /fp:fast /GR- /Gm- /LD
SET LinkerFlags=/SUBSYSTEM:WINDOWS,5.02 /incremental:no /opt:ref user32.lib gdi32.lib

IF NOT EXIST build mkdir build
pushd build

cl.exe %CompilerFlags% ..\VIMFullscreen\code\main.c /link %LinkerFlags%
call "..\VIMFullscreen\code\copy.bat"

popd
