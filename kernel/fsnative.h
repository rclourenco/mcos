#ifndef _FSNATIVE_H
#define _FSNATIVE_H


#include "mcoslib.h"
#include "fserror.h"
#include "mcosmem.h"
#include "fsbase.h"


typedef struct {
	BYTE Nome[12];
	BYTE Attr;
	WORD Data;
	WORD Hora;
	DWORD Tamanho;
	WORD FCluster;
	BYTE _reserved[9];
}TDIR;

typedef struct {
	BYTE SectorCluster;
	WORD FATSector;
	WORD DirSector;
	WORD DadosSector;
	WORD NEntradas;
	WORD NClusters;
	WORD far *FAT;
	TDIR far *Entrada;
	WORD _fatsize;
	WORD _dirsize;
	WORD _UCA;  //ultimo cluster alocado
	WORD _UE;  //ultima entrada
} FSN_DATA;

WORD fsnMontarDrive(BYTE drive);                                 //user
WORD fsnDesMontarDrive(BYTE drive);                              //user
WORD fsnSyncDrive(BYTE drive);                                      //user
void fsnCriarFicheiro(BYTE drive,BYTE far *nome,BYTE attr);        //user
BYTE fsnDirProcura(BYTE drive,TDIR_RECORD far *rec,BYTE first);    //user
void fsnChmod(BYTE drive,BYTE far *nome,BYTE attr);                //user
void fsnRenomear(BYTE drive,BYTE far *nome,BYTE far *novo_nome);   //user
void fsnEliminar(BYTE drive,BYTE far *nome);                       //user
void fsnFecharFicheiro(WORD bloco);                                //user
void fsnFlushFicheiro(WORD bloco);                                 //user
BYTE fsnGetAttr(BYTE drive,BYTE far *nome);                        //user
WORD fsnAbrirFicheiro(BYTE drive,BYTE far *nome,BYTE modo);        //user
WORD fsnEscreverCaracter(WORD bloco,BYTE caracter);                //user
WORD fsnLerCaracter(WORD bloco);                                   //user
void fsnEscreverCluster(BYTE drive,WORD cluster,BYTE far *buffer);
void fsnLerCluster(BYTE drive,WORD cluster,BYTE far *buffer);
void fsnPosicionarFicheiro(WORD bloco,long deslocamento,BYTE modo);   //user
void fsnTruncarFicheiro(WORD bloco);                                  //user
WORD fsnFreeClusters(BYTE drive);
DWORD fsnFreeSpace(BYTE drive);                                       //user
DWORD fsnDiskSpace(BYTE drive);                                       //user
BYTE fsnMontada(BYTE drive);                                          //user
WORD fsnLerFicheiro(WORD bloco, char far *ptr, WORD len);             //user;
WORD fsnEscreverFicheiro(WORD bloco, char far *ptr, WORD len);        //user;
IFS *fsnGetDriver();

#endif
