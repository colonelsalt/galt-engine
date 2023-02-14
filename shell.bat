@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
set path=C:\dev\homespun\misc;%path%
doskey npp="C:\Program Files\Notepad++\notepad++.exe" $*