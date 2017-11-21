#include "mcshell.c"
#include <dos.h>

#ifndef __TINY__
	#error O Modelo deve ser Tiny
#endif

void System_Init();
void System_Done();
void SetupMemory();

void main()
{
	kprintf("MCOS Loader...........\n");
	kprintf("Version 1.0...\n\n");
	kprintf("Press a Key to Continue...\n");
  GetKey();
	SetupMemory(); /* GetMemoryFromDos */
	System_Init();
	shell();
	System_Done();
}


void SetupMemory()
{
	unsigned segm;
	unsigned size;
	size=40960;
	do
	{
		segm=0;
		allocmem(size,&segm);
		if(!segm)
			size--;
	}while((size>=4096)&&(!segm));
	if(!segm)
	{
		kprintf("Falha a inicializar memoria...\n");
		exit(-2);
	}
	MEM_Set(segm,size);
}


