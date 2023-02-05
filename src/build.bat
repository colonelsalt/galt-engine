@echo off

set IncludePath="../include"
set LibPath="../lib"
set Libs=gdi32.lib User32.lib Shell32.lib opengl32.lib glfw3.lib msvcrtd.lib
set Defines=/DGALT_INTERNAL

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build
del *.pdb
cl /MTd /std:c++17 /nologo /Gm- /GR- /EHsc /Od /Oi /W3 /FC /Zi %Defines% /I%IncludePath% ..\src\Galt.cpp /LD /link /SUBSYSTEM:WINDOWS /PDB:galt_%random%.pdb /LIBPATH:%LibPath% %Libs%
cl /MTd /std:c++17 /nologo /Gm- /GR- /EHsc /Od /Oi /W3 /FC /Zi %Defines% /I%IncludePath% ..\src\Win_Galt.cpp /link /SUBSYSTEM:WINDOWS /LIBPATH:%LibPath% %Libs%
popd