#include "fsbase.h"
#include "mcosmem.h"
#include "fserror.h"

TDRIVE Drive[MAXDRIVES];
TFCB far * far *BlocoControlo;

BOOTRECORD far *BootRecord;
WORD ERRO;

void ApagaErro()
{
	ERRO=0;
}

void fsbaseCloseFs()
{
	LibertaMemoria(BootRecord);
	LibertaMemoria(BlocoControlo);
}

WORD fsbaseInitFs()
{
	BYTE contador;
	ERRO=FALSE;
	for (contador=0;contador<MAXDRIVES;contador++)
			Drive[contador].Montada=FALSE;
	BootRecord=(BOOTRECORD far *)AlocaMemoria(sizeof(BOOTRECORD));
	if (FP_SEG(BootRecord)==0)
	{
		ERRO=EMEM;
		return 0;
	}
	BlocoControlo=(TFCB far * far *)AlocaMemoria(sizeof(TFCB far *)*FILES);

	if(FP_SEG(BlocoControlo)==0)
	{
		ERRO=EMEM;
		return 0;
	}
	for(contador=0;contador<FILES;contador++)
		BlocoControlo[contador]=MK_FP(0,0);
	return 1;
}

IFS *fsbaseGetDriver(BYTE drive)
{
	ERRO=FALSE;
	if (drive>=MAXDRIVES || Drive[drive].Montada==FALSE) {
		ERRO=EINVDRV;
		return NULL;
	}
	return Drive[drive].ifsdriver;
}

IFS *fsbaseGetDriverForFcb(WORD fcbn)
{
	BYTE drive;
	ERRO=FALSE;
	if((FP_SEG(BlocoControlo[fcbn])==0)||(fcbn>=FILES))
	{
		ERRO=EBADF;
		return NULL;
	}

	drive=BlocoControlo[fcbn]->Drive;

	if (drive>=MAXDRIVES || Drive[drive].Montada==FALSE) {
		ERRO=EINVDRV;
		return NULL;
	}
	return Drive[drive].ifsdriver;
}


WORD Ler_Packed_Data()
{
	TDATA data;
	WORD pdata;
	GetDate(&data);
	pdata=(data.dia<<11);
	pdata=(WORD)(pdata+(data.mes<<7));
	pdata=(WORD)(pdata+(data.ano-1980));
	return pdata;
}

TDATA UnPacked_Data(WORD pdata)
{       TDATA data;
	data.dia=(pdata>>11)&0x1F;
	data.mes=(pdata>>7)&0x0F;
	data.ano=(pdata&0x7F)+1980;
	return data;
}

WORD Ler_Packed_Hora()
{
	THORA hora;
	WORD phora;
	GetHora(&hora);
	phora=hora.hora<<11;
	phora=phora+(hora.minutos<<5);
	phora=phora+(hora.segundos>>1);
	return phora;
}

THORA UnPacked_Hora(WORD phora)
{
	THORA hora;
	hora.hora=(phora>>11)&0x1F;
	hora.minutos=(phora>>5)&0x3F;
	hora.segundos=(phora&0x1F)*2;
	hora.centesimos=0;
	return hora;
}

void far *AlocaMemoria(WORD Tamanho)
{
	WORD Segmento;
	WORD blocos16bytes;
	blocos16bytes=Tamanho/16;
	if (Tamanho%16) blocos16bytes++;
	if(!Aloca_Segmento(blocos16bytes,CurProcess,&Segmento))
		Segmento=0;
	return MK_FP(Segmento,0);
}

void LibertaMemoria(void far *ptr)
{
	WORD Segmento;
	WORD Offset;
	Segmento=FP_SEG(ptr);
	Offset=FP_OFF(ptr);
	if (Offset) return;
	if (!Segmento) return;
	Liberta_Segmento(Segmento);
}

WORD GetFreeBloco()
{
	WORD cont;
	for(cont=0;cont<FILES;cont++)
	{
		if(FP_SEG(BlocoControlo[cont])==0)
			return cont;
	}
	return 0xFFFF;
}

BYTE LerSector(BYTE drive,WORD sector,void far *buffer)
{
	int head,track,sect;
	if((Drive[drive].Montada==FALSE)||(drive>=MAXDRIVES))
		return 0;
	if(sector>=Drive[drive].NSectors) return 0;
	sect=sector%Drive[drive].SectorPista+1;
	head=(sector/Drive[drive].SectorPista)%Drive[drive].Heads;
	track=(sector/Drive[drive].SectorPista)/Drive[drive].Heads;
	return (LerSectorFisico(drive,head,track,sect,buffer));
}

BYTE EscreverSector(BYTE drive,WORD sector,void far *buffer)
{
	int head,track,sect;
	if((Drive[drive].Montada==FALSE)||(drive>=MAXDRIVES))
		return 0;
	if(sector>=Drive[drive].NSectors) return 0;
	sect=sector%Drive[drive].SectorPista+1;
	head=(sector/Drive[drive].SectorPista)%Drive[drive].Heads;
	track=(sector/Drive[drive].SectorPista)/Drive[drive].Heads;
	return (EscreverSectorFisico(drive,head,track,sect,buffer));
}

// TODO fix ch, cl values, check int 13h docs
BYTE LerSectorFisico(char drive,char head,char track,char sector,char far *buffer)
{
	WORD entries=0;
	BYTE OK=0;
	int segb=FP_SEG(buffer),ofsb=FP_OFF(buffer);
	char r;
	if(drive>=MAXDRIVES)
		return 0;

	while((entries<NENTRIES)&&(!OK))
	{
		asm{
			mov ah,0x2;
			mov al,0x1;
			mov ch,track;
			mov cl,sector;
			mov dh,head;
			mov dl,drive;
			mov es,segb;
			mov bx,ofsb;
			int 0x13;
			mov r,0;
			jnc done;
			mov r,1;
			}
		done:
		if(!r)
			OK=1;
		entries++;
	}
	return OK;
}

// TODO fix ch, cl values, check int 13h docs
BYTE EscreverSectorFisico(char drive,char head,char track,char sector,char far *buffer)
{
	WORD entries=0;
	int segb=FP_SEG(buffer),ofsb=FP_OFF(buffer);
	char r;
	BYTE OK=0;
	if(drive>=MAXDRIVES)
		return 0;
	while((entries<NENTRIES)&&(!OK))
	{
		asm{
			mov ah,0x3;
			mov al,0x1;
			mov ch,track;
			mov cl,sector;
			mov dh,head;
			mov dl,drive;
			mov es,segb;
			mov bx,ofsb;
			int 0x13;
			mov r,0;
			jnc done;
			mov r,1;
		}
		done:
		if(!r)
			OK=1;
		entries++;
	}
	return OK;
}

BYTE fsbaseFim_Ficheiro(WORD bloco)
{
	ERRO=FALSE;
	if((FP_SEG(BlocoControlo[bloco])==0)||(bloco>=FILES))
	{
		ERRO=EBADF;
		return 1;
	}
	if(BlocoControlo[bloco]->Fpos>=BlocoControlo[bloco]->Tamanho)
		return 1;
	else
		return 0;
}

DWORD fsbaseFicheiro_Pos(WORD bloco)
{
	ERRO=FALSE;
	if((FP_SEG(BlocoControlo[bloco])==0)||(bloco>=FILES))
	{
		ERRO=EBADF;
		return 0L;
	}
	return BlocoControlo[bloco]->Fpos;
}

DWORD fsbaseFicheiro_Size(WORD bloco)
{
	ERRO=FALSE;
	if((FP_SEG(BlocoControlo[bloco])==0)||(bloco>=FILES))
	{
		ERRO=EBADF;
		return 0L;
	}
	return BlocoControlo[bloco]->Tamanho;
}

BYTE fsbaseMontada(BYTE drive)
{
	if(drive>=MAXDRIVES)
	{
		ERRO=EINVDRV;
		return 0;
	}
	return Drive[drive].Montada;
}

