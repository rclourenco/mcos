m:
cd m:\mcoslib
tasm mcosc0 /t/mx;
tcc -mt -c test.c
tcc -mt -c mcosapi.c
tlink mcosc0 mcosapi test /s/c/m,mcostest
exe2bin mcostest
del mcostest.exe
call m:\vmcosfs\floppy.bat
m:\vmcosfs\vmcosfs.exe cp mcostest.bin /a/testapi
