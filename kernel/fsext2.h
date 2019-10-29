#ifndef _FSEXT2_H
#define _FSEXT2_H


#include "mcoslib.h"
#include "fserror.h"
#include "mcosmem.h"
#include "fsbase.h"

#define FSEXT2_SUPER_MAGIC 0xEF53 

typedef struct {
	BYTE Nome[12];
	BYTE Attr;
	WORD Data;
	WORD Hora;
	DWORD Tamanho;
	WORD FCluster;
	BYTE _reserved[9];
}TINODE;

typedef struct {
	DWORD inodes_count;
	DWORD blocks_count;
	DWORD r_blocks_count;
	DWORD free_blocks_count;
	DWORD free_inodes_count;
	DWORD first_date_block;
	DWORD log_block_size;
	DWORD log_frag_size;
	DWORD blocks_per_group;
	DWORD frags_per_group;
	DWORD inodes_per_group;
	DWORD mtime;
	DWORD wtime;
	WORD mntcount;
	WORD max_mnt_count;
	WORD magic;
	WORD state;
	WORD errors;
	WORD minor_rev_level;
	DWORD lastcheck;
	DWORD checkinterval;
	DWORD create_os;
	DWORD rev_level;
	WORD def_resuid;
	WORD def_resgid;
}FSEXT2_SUPER;

typedef struct {
	FSEXT2_SUPER far *s;
	WORD bsb; // block size bytes
	BYTE bss; // block size 512 units
	DWORD work_block_number; //which block is cached
	BYTE far *work_block; //current loaded block
	BYTE dirty;           //work block is dirty
	

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
} FSEXT2_DATA;


WORD fsext2MontarDrive(BYTE drive);                                 //user
WORD fsext2DesMontarDrive(BYTE drive);                              //user
WORD fsext2SyncDrive(BYTE drive);                                      //user
void fsext2CriarFicheiro(BYTE drive,BYTE far *nome,BYTE attr);        //user
BYTE fsext2DirProcura(BYTE drive,TDIR_RECORD far *rec,BYTE first);    //user
void fsext2Chmod(BYTE drive,BYTE far *nome,BYTE attr);                //user
void fsext2Renomear(BYTE drive,BYTE far *nome,BYTE far *novo_nome);   //user
void fsext2Eliminar(BYTE drive,BYTE far *nome);                       //user
void fsext2FecharFicheiro(WORD bloco);                                //user
void fsext2FlushFicheiro(WORD bloco);                                 //user
BYTE fsext2GetAttr(BYTE drive,BYTE far *nome);                        //user
WORD fsext2AbrirFicheiro(BYTE drive,BYTE far *nome,BYTE modo);        //user
WORD fsext2EscreverCaracter(WORD bloco,BYTE caracter);                //user
WORD fsext2LerCaracter(WORD bloco);                                   //user
void fsext2EscreverCluster(BYTE drive,WORD cluster,BYTE far *buffer);
void fsext2LerCluster(BYTE drive,WORD cluster,BYTE far *buffer);
void fsext2PosicionarFicheiro(WORD bloco,long deslocamento,BYTE modo);   //user
void fsext2TruncarFicheiro(WORD bloco);                                  //user
WORD fsext2FreeClusters(BYTE drive);
DWORD fsext2FreeSpace(BYTE drive);                                       //user
DWORD fsext2DiskSpace(BYTE drive);                                       //user
BYTE fsext2Montada(BYTE drive);                                          //user
WORD fsext2LerFicheiro(WORD bloco, char far *ptr, WORD len);             //user;
WORD fsext2EscreverFicheiro(WORD bloco, char far *ptr, WORD len);        //user;
IFS *fsext2GetDriver();

#endif
