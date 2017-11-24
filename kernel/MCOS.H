#ifndef _MCOS_H

#define _MCOS_H

#include "mcoslib.h"

#define REGULAR 0x00
#define DEVICE  0x80

#define DEVMASK 0xC0
#define MODMASK 0x3F

#define SIG_MC (0x4D43)
#define SIG_OS (0x4F53)
#define VERSION (0x0001)

#define PDT_OFS 0x40
#define PDT_N   16

typedef void far *(far *DeviceInitProc) (int d); /* returns devdta */
typedef int (far *DeviceOpenProc) (void far *devdta,  char far *name, BYTE modo);
typedef int (far *DeviceCloseProc)(void far *devdta,  void far *data);
typedef WORD (far *DeviceReadProc) (void far *devdta, void far *data);
typedef WORD (far *DeviceWriteProc)(void far *devdta, void far *data, BYTE c);
typedef int  (far *DeviceKillProc)(void far *devdta);

typedef struct tdevice {
	struct tdevice far *next;
	char name[13];
	DeviceInitProc init;
	DeviceOpenProc open;
	DeviceCloseProc close;
	DeviceReadProc read;
	DeviceWriteProc write;
	DeviceKillProc kill;
	void far *devdta;
}TDEVICE;

typedef struct {
	WORD DevId;
	TDEVICE far *devdef;
	void far    *data;
	BYTE Modo;
	WORD Procid;
}TDeviceControl;

typedef struct {
	BYTE idx;
	BYTE flags;
}TProcessDescriptor;


int DeviceRegister(
	char far *name, 
	DeviceInitProc init,
	DeviceOpenProc open,
	DeviceCloseProc close,
	DeviceReadProc read,
	DeviceWriteProc write,
	DeviceKillProc kill
);

BYTE TestValidPrg(WORD psp);
BYTE TestValidPsp(WORD psp);
void SetPSP(WORD psp,char far *nome,char far *cmd);
void UnLoad(WORD PID);
WORD Load(BYTE drv,char far *nome);
void Run(BYTE drv,char far *nome,char far *cmd);
void Callprg();
void Dump(WORD pid);
void Exit_Process();
void kill_proc();
void System_Init();
void System_Done();

extern char CurDrv;
extern char bootdrive;
extern unsigned CurProcess;

#endif
