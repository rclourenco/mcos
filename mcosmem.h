#ifndef _MCOSMEM_H

#define _MCOSMEM_H

#include "mcoslib.h"
#include "mcos.h"

#define LAST 'L'
#define INTER 'I'


#define MAXSEG 30


typedef struct {
	char Tipo;
	unsigned Tam;
	unsigned ProcessoID;
	char Label[11];
}MCB;

int MEM_Init();
void MEM_Init2();
void MEM_Done();

void MEM_Set(unsigned start, unsigned size);
unsigned MEM_seg();

void MEM_Copy(unsigned sego,unsigned segd,unsigned size);
int MEM_Search(int f,MCB far *mcb,unsigned far *seg);             //user adapted
void Set_Bloco(unsigned seg,MCB far *mcb);
int Aloca_Segmento(unsigned size,unsigned procid,unsigned far *seg); //user
int Realoca_Segmento(unsigned size,unsigned far *seg);       //user
void Liberta_Segmento(unsigned seg);                         //user
void MEM_Optimize();
void Get_Bloco(unsigned seg,MCB far *mcb);
void LibertaMemPID(unsigned pid);
unsigned ProcurarMem(unsigned size);
char far *GetProcessLabel(unsigned pid);

extern unsigned MEM_SearchNext;
extern unsigned MEM_Start;
extern unsigned MEM_Size;
extern unsigned MEM_End;

extern char far *Label_MF;
extern char far *Default_Label;


#endif