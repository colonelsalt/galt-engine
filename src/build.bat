@echo off

set IncludePath="../include"
set LibPath="../lib"
set Libs=gdi32.lib User32.lib Shell32.lib opengl32.lib glfw3.lib msvcrtd.lib
set Defines=/DGALT_INTERNAL

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build
cl /MTd /std:c++17 /nologo /Gm- /GR- /EHsc /Od /Oi /W3 /FC /Zi %Defines% /I%IncludePath% ..\src\Win_Galt.cpp ..\lib\glad.c /link /SUBSYSTEM:WINDOWS -incremental:no -opt:ref /LIBPATH:%LibPath% %Libs%
popd