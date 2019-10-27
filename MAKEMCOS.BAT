@echo off
m:
cd m:\kernel
make
copy mcosmain.bin ..\mcos.sys
copy mcosmain.bin c:\MCOS.SYS
cd ..

