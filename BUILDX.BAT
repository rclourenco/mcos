@echo off
c:
cd c:\many\mcos
make
copy mcosmain.bin ..\mcos.sys
cd ..
mcimg
cd mcos
pause
exit
