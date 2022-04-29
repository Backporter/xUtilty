@echo off

set libraries=-lc -lkernel -lc++

set intdir=ORBIS_Debug
set targetname=OrbisUtil
set outputstatic=OrbisUtil.a

@mkdir %intdir%

for /r %%f in (*.cpp) do (
    clang++.exe --target=x86_64-pc-freebsd12-elf -fPIC -std=c++17 -funwind-tables -D__OPENORBIS__=1 -o0 -I"%OO_PS4_TOOLCHAIN%\\include" -I"%OO_PS4_TOOLCHAIN%\\include\\c++\\v1" -c -o "%intdir%\%%~nf.o" "%%f"
)

cls

SETLOCAL EnableDelayedExpansion

set obj_files=
for %%f in (%intdir%\*.o) do set obj_files=!obj_files! .\%%f

SETLOCAL DISABLEDELAYEDEXPANSION

rem pack
llvm-ar -rc %outputstatic% %obj_files%

rem clean and move

pause

del /s /q ORBIS_Debug\*.*
move OrbisUtil.a ORBIS_Debug\libOrbisUtil.a


pause