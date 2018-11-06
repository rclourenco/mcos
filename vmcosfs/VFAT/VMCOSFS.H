#ifndef _VMCOSFS_H
 #define _VMCOSFS_H

#include <stdlib.h>
#include <dos.h>
#include <stdio.h>

#define NUMERROS 11
//sem erros
#define EZERO 0
//acesso n∆o permitido
#define EACCES 1
//erro de alocaá∆o de memoria
#define EMEM 2
//numero de ficheiro inv†lido
#define EBADF 3
//Erro de entradas e sa°das
#define EINOUT 4
//Erro de existencia do ficheiro
#define EEXIST 5
//Erro interno ou desconhecido
#define EFAULT 6
//Erro de drive inv†lida
#define EINVDRV 7
//Demasiados ficheiros abertos
#define EMFILE 8
//O ficheiro n∆o existe
#define ENOFILE 9
//Nome de ficheiro inv†lido
#define EINVFILE 10

#define Strlen(x) strlen(x)
#define Strcmp(x,y) strcmp((x),(y))
#define AlocaMemoria(s) malloc(s)
#define LibertaMemoria(x) free(x)

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
#define DIRS  30

typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;

typedef struct{
	WORD ano;
	BYTE dia;
	BYTE mes;
}TDATA;


typedef struct{
	BYTE minutos;
	BYTE hora;
	BYTE centesimos;
	BYTE segundos;
}THORA;

typedef struct {
	BYTE _jmpcode[3]; //reservado
	BYTE Fsid[8];   //indentificador do sistema de ficheiros
	WORD Bps;        //Bytes por Sector
	BYTE Spc;        //Sectores por Cluster
	WORD Rs;        //Sectores Reservados
	BYTE Fn;
	WORD Rde;       //Numero de entradas na directoria raiz
	WORD Lsn;       //Numero de Sectores Logicos
	BYTE Md;        //tipo de media
	WORD Spf;       //Sectores na fat
	WORD Spp;       //Sectores por pista
	WORD Hn;        //Numero de Cabe‚Ä°as
	DWORD Ssn;       //Sectores ocupados pelo sistema
	DWORD LargeLsn;
	BYTE PhysicalNumber;
	BYTE CurHead;
	BYTE Signature;
	DWORD Serial;
	BYTE  Label[11];
	BYTE  SystemId[8];
	BYTE _bootstrap[448]; //reservado
	BYTE mark[2];
} BOOTRECORD;


typedef struct {
	BYTE Nome[11];
	BYTE Attr;
	BYTE _reserved1[2];
	WORD Data;
	WORD Hora;
	WORD AccessDate;
	WORD _reserved2;
	WORD LastWriteTime;
	WORD LastWriteDate;
	WORD FCluster;
	DWORD Tamanho;
}TDIR;

typedef struct {
	BYTE Drive;
	DWORD Fpos;
	WORD Cluster;
	WORD FCluster;
	DWORD Tamanho;
	TDIR *Entrada;
	BYTE Modo;
	BYTE *Buffer;
	WORD Procid;
}TFCB;

#define DCB_FREE 0xffff
#define DCB_ROOT 0xfffe

typedef struct {
	BYTE drive;
	WORD handle;
	WORD rec;
} TDIRCB;

#define FAT12  0x1
#define FAT16  0x2
#define FAT16B 0x3
#define FAT32  0x4

typedef struct {
	BYTE Montada;
	WORD TamSector;
	BYTE SectorCluster;
	BYTE SectorPista;
	BYTE Heads;
	WORD FATSector;
	WORD DirSector;
	WORD DadosSector;
	WORD NEntradas;
	WORD NClusters;
	WORD NSectors;
	WORD *FAT;
	TDIR *Entrada;
	WORD _fatsize;
	WORD _dirsize;
	WORD _UCA;  //ultimo cluster alocado
	WORD _UE;  //ultima entrada
	BYTE Type;

	FILE *ifp;
	char *imagefile;
}TDRIVE;

typedef struct{
	BYTE nome[13];
	BYTE Attr;
	TDATA Data;
	THORA Hora;
	DWORD Tamanho;
}TDIR_RECORD;


WORD Ler_Packed_Data();
WORD Ler_Packed_Hora();
TDATA UnPacked_Data(WORD pdata);
THORA UnPacked_Hora(WORD phora);
WORD INITFS(); //inicializa o systema de ficheiros
void CLOSEFS();//termina o systema de ficheiros
WORD MontarDrive(BYTE drive);                                 //user
WORD DesMontarDrive(BYTE drive);                              //user
BYTE LerSector(BYTE drive,WORD sector,void *buffer);      //user
BYTE EscreverSector(BYTE drive,WORD sector,void *buffer); //user
WORD GetFat(BYTE drive,WORD ParentCluster);
void SetFat(BYTE drive,WORD ParentCluster,WORD Cluster);
WORD SyncDrive(BYTE drive);                                    //user
WORD AlocaCluster(BYTE drive);
WORD ProcurarEntrada(BYTE *nome,BYTE drive);
BYTE NomeFicheiroCmp(BYTE *fnome,BYTE *nome);
void SetNomeEntrada(BYTE *fnome,BYTE *nome);
void GetNomeEntrada(BYTE *fnome,BYTE *nome);
void LibertaClusters(BYTE drive,WORD cluster);
void CriarFicheiro(BYTE drive,BYTE *nome,BYTE attr);        //user
WORD GetEntrada(BYTE drive,BYTE first);
BYTE DirProcura(BYTE drive,TDIR_RECORD *rec,BYTE first);    //user
void Chmod(BYTE drive,BYTE *nome,BYTE attr);                //user
void Renomear(BYTE drive,BYTE *nome,BYTE *novo_nome);   //user
void Eliminar(BYTE drive,BYTE *nome);                       //user
void FecharFicheiro(WORD bloco);                                //user
void FlushFicheiro(WORD bloco);                                 //user
BYTE GetAttr(BYTE drive,BYTE *nome);                        //user
WORD AbrirFicheiro(BYTE drive,BYTE *nome,BYTE modo);        //user
BYTE VerificarAberto(TDIR *entrada);
void FecharFicheirosDrive(BYTE drive);
void FecharFicheirosProcesso(WORD procid);
WORD EscreverCaracter(WORD bloco,BYTE caracter);                //user
WORD LerCaracter(WORD bloco);                                   //user
void EscreverCluster(BYTE drive,WORD cluster,BYTE *buffer);
void LerCluster(BYTE drive,WORD cluster,BYTE *buffer);
void PosicionarFicheiro(WORD bloco,long deslocamento,BYTE modo);  //user
void TruncarFicheiro(WORD bloco);                                 //user
WORD FreeClusters(BYTE drive);
DWORD FreeSpace(BYTE drive);                                       //user
DWORD DiskSpace(BYTE drive);                                       //user
BYTE LerSectorFisico(char drive,char head,char track,char sector,char far *buffer);
BYTE EscreverSectorFisico(char drive,char head,char track,char sector,char far *buffer);
BYTE Montada(BYTE drive);                                           //user
BYTE Fim_Ficheiro(WORD bloco);                                      //user
DWORD Ficheiro_Pos(WORD bloco);                                     //user
DWORD Ficheiro_Size(WORD bloco);                                    //user
WORD GetFreeBloco();
void ApagaErro();                                                  //user
char *GetMsgErro(unsigned int erro);    //user

WORD LerFicheiro(WORD bloco, char far *ptr, WORD len);
WORD EscreverFicheiro(WORD bloco, char far *ptr, WORD len);

extern char *MsgErro[NUMERROS];

extern TFCB **BlocoControlo;
extern BOOTRECORD *BootRecord;
extern WORD ERRO;



#endif
