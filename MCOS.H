#ifndef _MCOS_H

#define _MCOS_H

#include "mcoslib.h"


#define SIG_MC (0x4D43)
#define SIG_OS (0x4F53)
#define VERSION (0x0001)

#define PDT_OFS 0x40
#define PDT_N   16

typedef struct {
	BYTE idx;
	BYTE flags;
}TProcessDescriptor;

BYTE TestValidPrg(WORD psp);
BYTE TestValidPsp(WORD psp);
void SetPSP(WORD psp,char far *nome,char far *cmd);
void UnLoad(WORD PID);
WORD Load(BYTE drv,char far *nome);
void Run(BYTE drv,char far *nome,char far *cmd);
void Callprg();
void Dump(WORD pid);
void Exit_Process();
void System_Init();
void System_Done();

extern char CurDrv;
extern char bootdrive;
extern unsigned CurProcess;

#endif