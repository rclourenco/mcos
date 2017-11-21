#ifndef _MCOS_STD_H
#define _MCOS_STD_H

#pragma inline

void mcos_exit()
asm mov ax,0;
asm int 0x80;


int mcos_open(const char *path, int modo)
es=seg path
dx=offset path
al=path 2
bl=modo

return ax


#endif