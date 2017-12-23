#ifndef _FSYSTEM_H
#define _FSYSTEM_H


#include "mcoslib.h"
#include "FSERROR.h"
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
	BYTE Nome[12];
	BYTE Attr;
	WORD Data;
	WORD Hora;
	DWORD Tamanho;
	WORD FCluster;
	BYTE _reserved[9];
}TDIR;

typedef struct {
	BYTE Drive;
	DWORD Fpos;
	WORD Cluster;
	WORD FCluster;
	DWORD Tamanho;
	TDIR far *Entrada;
	BYTE Modo;
	BYTE far *Buffer;
	WORD Procid;
}TFCB;

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
	WORD far *FAT;
	TDIR far *Entrada;
	WORD _fatsize;
	WORD _dirsize;
	WORD _UCA;  //ultimo cluster alocado
	WORD _UE;  //ultima entrada
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
void far *AlocaMemoria(WORD Tamanho);
void LibertaMemoria(void far *ptr);
WORD INITFS(); //inicializa o systema de ficheiros
void CLOSEFS();//termina o systema de ficheiros
WORD MontarDrive(BYTE drive);                                 //user
WORD DesMontarDrive(BYTE drive);                              //user
BYTE LerSector(BYTE drive,WORD sector,void far *buffer);      //user
BYTE EscreverSector(BYTE drive,WORD sector,void far *buffer); //user
WORD GetFat(BYTE drive,WORD ParentCluster);
void SetFat(BYTE drive,WORD ParentCluster,WORD Cluster);
WORD SyncDrive(BYTE drive);                                    //user
WORD AlocaCluster(BYTE drive);
WORD ProcurarEntrada(BYTE far *nome,BYTE drive);
BYTE NomeFicheiroCmp(BYTE far *fnome,BYTE far *nome);
void SetNomeEntrada(BYTE far *fnome,BYTE far *nome);
void GetNomeEntrada(BYTE far *fnome,BYTE far *nome);
void LibertaClusters(BYTE drive,WORD cluster);
void CriarFicheiro(BYTE drive,BYTE far *nome,BYTE attr);        //user
WORD GetEntrada(BYTE drive,BYTE first);
BYTE DirProcura(BYTE drive,TDIR_RECORD far *rec,BYTE first);    //user
void Chmod(BYTE drive,BYTE far *nome,BYTE attr);                //user
void Renomear(BYTE drive,BYTE far *nome,BYTE far *novo_nome);   //user
void Eliminar(BYTE drive,BYTE far *nome);                       //user
void FecharFicheiro(WORD bloco);                                //user
void FlushFicheiro(WORD bloco);                                 //user
BYTE GetAttr(BYTE drive,BYTE far *nome);                        //user
WORD AbrirFicheiro(BYTE drive,BYTE far *nome,BYTE modo);        //user
BYTE VerificarAberto(TDIR far *entrada);
void FecharFicheirosDrive(BYTE drive);
void FecharFicheirosProcesso(WORD procid);
WORD EscreverCaracter(WORD bloco,BYTE caracter);                //user
WORD LerCaracter(WORD bloco);                                   //user
void EscreverCluster(BYTE drive,WORD cluster,BYTE far *buffer);
void LerCluster(BYTE drive,WORD cluster,BYTE far *buffer);
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

WORD LerFicheiro(WORD bloco, char far *ptr, WORD len); //user;
WORD EscreverFicheiro(WORD bloco, char far *ptr, WORD len); //user;

extern TDRIVE Drive[MAXDRIVES];
extern TFCB far * far *BlocoControlo;
extern BOOTRECORD far *BootRecord;
extern WORD ERRO;
void ApagaErro();                                                  //user



#endif
