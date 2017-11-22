echo Building %1
m:
cd m:\mcoslib
if not exist %1.c goto end

tasm mcosc0 /t/mx;
if errorlevel 1 goto end

tcc -mt -c %1.c
if errorlevel 1 goto end

tcc -mt -c mcosapi.c
if errorlevel 1 goto end

tlink mcosc0 mcosapi %1 /s/c/m,prg%1
if errorlevel 1 goto end

exe2bin prg%1
if errorlevel 1 goto end

del prg%1.exe
call m:\vmcosfs\floppy.bat

m:\vmcosfs\vmcosfs.exe cp prg%1.bin /a/prg%1

:end
