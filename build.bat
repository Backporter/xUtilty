@echo off

set addionaloptions2=-Wno-unknown-pragmas -Wno-unused-private-field -Wno-c++1z-extensions -Wno-c++17-extensions -Wno-null-conversion -Wno-dll-attribute-on-redeclaration -Wno-overloaded-virtual -Wno-inconsistent-missing-override
set addionaloptions=-Wno-c++1z-extensions -Wno-nonportable-include-path -D__OPENORBIS__=1 -DPLATFORM_ORBIS=1 -o2 %addionaloptions2%


set libraries=-lc -lkernel -lc++

set intdir=ORBIS_Debug
set targetname=OrbisUtil
set outputstatic=OrbisUtil.a

@mkdir %intdir%

for /r %%f in (*.cpp) do (
    rem -Wc++1z-extensions
    clang++.exe --target=x86_64-pc-freebsd12-elf -fPIC -funwind-tables %addionaloptions% -I"%OO_PS4_TOOLCHAIN%\\include" -I"%OO_PS4_TOOLCHAIN%\\include\\c++\\v1" -c -o "%intdir%\%%~nf.o" "%%f"
)

rem cls

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