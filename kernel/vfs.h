#ifndef _VFS_H
	#define _VFS_H

#include "mcoslib.h"
#include "fsbase.h"
#include "fsystem.h"

void CriarFicheiro(BYTE drive,BYTE far *nome,BYTE attr);
void Chmod(BYTE drive,BYTE far *nome,BYTE attr);
void Renomear(BYTE drive,BYTE far *nome,BYTE far *novo_nome);
void Eliminar(BYTE drive,BYTE far *nome);
void FecharFicheiro(WORD bloco);
void FlushFicheiro(WORD bloco);
BYTE GetAttr(BYTE drive,BYTE far *nome); 
WORD AbrirFicheiro(BYTE drive,BYTE far *nome,BYTE modo);
WORD EscreverCaracter(WORD bloco,BYTE caracter);
WORD LerCaracter(WORD bloco);
void EscreverCluster(BYTE drive,WORD cluster,BYTE far *buffer);
void LerCluster(BYTE drive,WORD cluster,BYTE far *buffer);
void PosicionarFicheiro(WORD bloco,long deslocamento,BYTE modo);
void TruncarFicheiro(WORD bloco);
WORD FreeClusters(BYTE drive);
DWORD FreeSpace(BYTE drive);
DWORD DiskSpace(BYTE drive);
BYTE Montada(BYTE drive);
BYTE Fim_Ficheiro(WORD bloco);
DWORD Ficheiro_Pos(WORD bloco);
DWORD Ficheiro_Size(WORD bloco);

WORD LerFicheiro(WORD bloco, char far *ptr, WORD len);
WORD EscreverFicheiro(WORD bloco, char far *ptr, WORD len);

void FecharFicheirosDrive(BYTE drive);
void FecharFicheirosProcesso(WORD procid);

BYTE Fim_Ficheiro(WORD bloco);                                      //user
DWORD Ficheiro_Pos(WORD bloco);                                     //user
DWORD Ficheiro_Size(WORD bloco);                                    //user

WORD DesMontarDrive(BYTE drive);
void CLOSEFS();
WORD INITFS();
#endif

