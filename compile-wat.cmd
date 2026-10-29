@echo off
rem compile-wat.cmd -- Build Osuwari Multi for ArcaOS
rem Requires: OpenWatcom 2.0, OS/2 Toolkit 4.5

set LOG=compile-wat.log
echo Build started > %LOG%

rem Auto-detect OpenWatcom
if exist c:\watcom2\binp\wcc386.exe set WATCOM=c:\watcom2
if exist c:\watcom\binp\wcc386.exe  set WATCOM=c:\watcom
if "%WATCOM%"=="" (
    echo ERROR: OpenWatcom not found at c:\watcom or c:\watcom2 >> %LOG%
    echo BUILD FAILED
    goto end
)
set PATH=%WATCOM%\binp;%WATCOM%\binw;%PATH%
set INCLUDE=%WATCOM%\h;%WATCOM%\h\os2;c:\os2tk45\h
set LIB=%WATCOM%\lib386;%WATCOM%\lib386\os2;c:\os2tk45\lib

rem Default OS2TK
if "%OS2TK%"=="" set OS2TK=c:\os2tk45

if not exist bin md bin
if not exist bin\lang md bin\lang

echo Using WATCOM=%WATCOM% >> %LOG%
echo Using OS2TK=%OS2TK% >> %LOG%

echo Cleaning previous build... >> %LOG%
wmake -h -f makefile.wat clean >> %LOG% 2>&1

echo Building... >> %LOG%
wmake -h -f makefile.wat all >> %LOG% 2>&1

echo Copying language files... >> %LOG%
copy lang\*.txt bin\lang\ >> %LOG% 2>&1

if exist bin\osmulti2.exe (
    echo BUILD OK
    echo BUILD OK >> %LOG%
) else (
    echo BUILD FAILED
    echo BUILD FAILED >> %LOG%
)

:end
