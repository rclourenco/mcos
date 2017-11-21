#include "mcos.h"
#include "mcoslib.h"
#include "mcshell.h"

#ifndef __TINY__
	#error O Modelo deve ser Tiny
#endif

void System_Init();
void System_Done();
void SetupMemory();

extern void _ping(void);

extern unsigned _StartTime1, _StartTime2;

void exit(int c)
{
  kprintf("Rebooting...\r\n");
  _exit();
}

void pong()
{
  kprintf("Pong %X %X\r\n", _StartTime1, _StartTime2);
  GetKey();
}

void dump_mem(unsigned seg, unsigned offset, unsigned len)
{
  unsigned far *fp = (unsigned far *)MK_FP(seg, offset);
  unsigned i;
  for(i=0;i<len;i++)
  {
    writehex(fp[i]);
    writestr(" ");
  }
  writestr("\r\n");
}


int main()
{
	SetupMemory();
	System_Init();

	shell();

	System_Done();

	return 0;
}


void SetupMemory()
{
	MEM_Set(0x2000,0x4000);
}


