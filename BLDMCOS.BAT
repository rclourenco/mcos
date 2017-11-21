@echo off
c:
cd c:\many\mcos
make -B
copy mcosmain.bin ..\mcos.sys
cd ..
mcimg
cd mcos


