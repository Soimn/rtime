@echo off
setlocal

if "%Platform%" neq "x64" (
    echo ERROR: Platform is not "x64" - please run this from the MSVC x64 native tools command prompt.
    exit /b 1
)

cd %~dp0

if "%1" neq "" (
	echo Illegal number of arguments^, expected^: build
	goto end
)

cl /nologo /Od /Zo /Z7 /W3 rtime.c /link /subsystem:console /out:rtime.exe /pdb:rtime.pdb /incremental:no /opt:icf /opt:ref shlwapi.lib shell32.lib

:end
endlocal
