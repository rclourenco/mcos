@echo off
m:
cd m:\kernel
make
copy mcosmain.bin ..\mcos.sys
cd ..
m:\bldutils\mcimg
pause
exit

