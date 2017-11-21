#ifndef _MCSHELL_H

#define _MCSHELL_H

#include "mcos.h"

void Cls();
void ChDrive();
void MountDrive();
void UnMountDrive();
void Info();
void Help();
void Executar();
void getcmd();
void set_keys();
void set_terminal();
void parse();
void shell();
void showprompt();
void Ls();
void Ed();
void Type();
void strp(char far *dest,char far *or,unsigned char t);
void Mem();
void AMem();
void FMem();
void DumpFile();
void escreve_erro();
void Sync();

extern char prompt1[];
extern char prompt2[];

#endif