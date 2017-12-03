@echo off
echo Building %1
m:
cd m:\mcoslib
if not exist %1.c goto endf

tasm mcosc0 /t/mx;
if errorlevel 1 goto endf

tasm mcosintr /t/mx;
if errorlevel 1 goto endf


pause
tcc -mt -c %1.c
if errorlevel 1 goto endf
pause
tcc -mt -c mcosapi.c
if errorlevel 1 goto endf
pause
tlink mcosc0 mcosintr mcosapi %1 /s/c/m,prg%1
if errorlevel 1 goto endf
pause
exe2bin prg%1
if errorlevel 1 goto endf

del prg%1.exe
call m:\vmcosfs\floppy.bat

m:\vmcosfs\vmcosfs.exe cp prg%1.bin /a/prg%1

:endf
 echo "Error...."
