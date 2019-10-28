#ifndef _FSBASE_H
	#define _FSBASE_H

#include "mcoslib.h"
#include "mcosmem.h"

#define MAXDRIVES 2

#define Sem_atributos 0
#define So_leitura 1
#define LEITURA 0x00
#define CRIAR_F 0x02
#define ESCREVER 0x01
#define EXPANDIR 0x04
#define SF_INICIO 0
#define SF_CURRENTE 1
#define SF_FIM 2

#define __EOF 0xFFF8

#define FALSE 0
#define TRUE 1

#define NENTRIES 3

#define FILES 30

typedef struct{
	BYTE nome[13];
	BYTE Attr;
	TDATA Data;
	THORA Hora;
	DWORD Tamanho;
}TDIR_RECORD;

typedef struct {
	void (*vCriarFicheiro)(BYTE drive,BYTE far *nome,BYTE attr);        //user
	void (*vChmod)(BYTE drive,BYTE far *nome,BYTE attr);                //user
	void (*vRenomear)(BYTE drive,BYTE far *nome,BYTE far *novo_nome);   //user
	void (*vEliminar)(BYTE drive,BYTE far *nome);                       //user
	void (*vFecharFicheiro)(WORD bloco);                                //user
	void (*vFlushFicheiro)(WORD bloco);                                 //user
	BYTE (*vGetAttr)(BYTE drive,BYTE far *nome);                        //user
	WORD (*vAbrirFicheiro)(BYTE drive,BYTE far *nome,BYTE modo);        //user
	WORD (*vEscreverCaracter)(WORD bloco,BYTE caracter);                //user
	WORD (*vLerCaracter)(WORD bloco);                                   //user
	void (*vEscreverCluster)(BYTE drive,WORD cluster,BYTE far *buffer);
	void (*vLerCluster)(BYTE drive,WORD cluster,BYTE far *buffer);
	void (*vPosicionarFicheiro)(WORD bloco,long deslocamento,BYTE modo);  //user
	void (*vTruncarFicheiro)(WORD bloco);                                 //user
	WORD (*vFreeClusters)(BYTE drive);
	DWORD (*vFreeSpace)(BYTE drive);                                       //user
	DWORD (*vDiskSpace)(BYTE drive);                                       //user
	WORD (*vLerFicheiro)(WORD bloco, char far *ptr, WORD len); //user;
	WORD (*vEscreverFicheiro)(WORD bloco, char far *ptr, WORD len); //user;

	WORD (*vDesMontarDrive)(BYTE drive);
	WORD (*vSyncDrive)(BYTE drive);
	BYTE (*vDirProcura)(BYTE drive, TDIR_RECORD far *rec, BYTE first);
} IFS;

IFS *fsbaseGetDriver(BYTE drive);
IFS *fsbaseGetDriverForFcb(WORD fcbn);

typedef struct {
	BYTE _jmpcode[3]; //reservado
	BYTE Fsid[6];   //indentificador do sistema de ficheiros
	WORD Bps;        //Bytes por Sector
	BYTE Spc;        //Sectores por Cluster
	WORD Rs;	//Sectores Reservados
	WORD Rde;	//Numero de entradas na directoria raiz
	WORD Lsn;	//Numero de Sectores Logicos
	BYTE Md;	//tipo de media
	WORD Spf;	//Sectores na fat
	WORD Spp;	//Sectores por pista
	WORD Hn;	//Numero de Cabe‡as
	WORD Ssn;	//Sectores ocupados pelo sistema
	BYTE _drv;	//reservado
	BYTE _bootstrap[484]; //reservado
}BOOTRECORD;

typedef struct {
	BYTE Drive;
	DWORD Fpos;
	WORD Cluster;
	WORD FCluster;
	DWORD Tamanho;
	DWORD Inode;
	BYTE Modo;
	BYTE far *Buffer;
	WORD Procid;
}TFCB;

typedef struct {
	BYTE Montada;
	WORD TamSector;
	BYTE SectorPista;
	BYTE Heads;
	WORD NSectors;
	IFS *ifsdriver;
	void far *fs_data;
}TDRIVE;

extern TDRIVE Drive[MAXDRIVES];
extern TFCB far * far *BlocoControlo;
extern BOOTRECORD far *BootRecord;
extern WORD ERRO;
void ApagaErro();                                                  //user

WORD Ler_Packed_Data();
WORD Ler_Packed_Hora();
TDATA UnPacked_Data(WORD pdata);
THORA UnPacked_Hora(WORD phora);
void far *AlocaMemoria(WORD Tamanho);
void LibertaMemoria(void far *ptr);
void fsbaseCloseFs();
WORD GetFreeBloco();
BYTE LerSector(BYTE drive,WORD sector,void far *buffer);      //user
BYTE EscreverSector(BYTE drive,WORD sector,void far *buffer); //user
BYTE LerSectorFisico(char drive,char head,char track,char sector,char far *buffer);
BYTE EscreverSectorFisico(char drive,char head,char track,char sector,char far *buffer);

void fsbaseCloseFs();
WORD fsbaseInitFs();
BYTE fsbaseFim_Ficheiro(WORD bloco);
DWORD fsbaseFicheiro_Pos(WORD bloco);
DWORD fsbaseFicheiro_Size(WORD bloco);
BYTE fsbaseMontada(BYTE drive);
#endif
