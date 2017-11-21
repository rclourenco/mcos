c:
cd c:\home\mcoslib
tasm mcosc0 /t/mx;
tcc -mt -c test.c
tcc -mt -c mcosapi.c
tlink mcosc0 mcosapi test /s/c/m,mcostest
c:\many\mcos\exe2bin mcostest
del mcostest.exe
call c:\home\vmcosfs\floppy.bat
c:\home\vmcosfs\vmcosfs.exe cp mcostest.bin /a/testapi
