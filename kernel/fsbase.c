#include "fsbase.h"
#include "mcosmem.h"
#include "fserror.h"

TDRIVE Drive[MAXDRIVES];
TFCB far * far *BlocoControlo;

BOOTRECORD far *BootRecord;
WORD ERRO;

BYTE GetDriveParams(BYTE drive, DRIVE_PARAM *dp);

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

IFS far *fsbaseGetDriver(BYTE drive)
{
	ERRO=FALSE;
	if (drive>=MAXDRIVES || Drive[drive].Montada==FALSE) {
		ERRO=EINVDRV;
		return NULL;
	}
	return Drive[drive].ifsdriver;
}

IFS far *fsbaseGetDriverForFcb(WORD fcbn)
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

BYTE fsbaseMount(BYTE drive)
{
	int i;
	BYTE r;
	r = GetDriveParams(drive, &(Drive[drive].p));
	if (!r) {
		kprintf("Drive params FAIL!\r\n");

		ERRO=EINVDRV;
		return 0;
	}

	kprintf("Drive params OK!\r\n");
	kprintf("SectorsTrack: %u\r\n", Drive[drive].p.SectorsPista);
	kprintf("Heads: %u\r\n", Drive[drive].p.Heads);
	kprintf("Drive params OK!\r\n");

	//WORD bootid;
	if(!(LerSectorFisico(drive,0,0,1,(char far *)BootRecord)))
	{
		ERRO=EINOUT;
		return 0;
	}

	kprintf("Bootrecord OK!\r\n");

	for(i=0;i<6;i++) {
		kprintf("%c",  BootRecord->Fsid[i]);
		Drive[drive].FsID[i] =  BootRecord->Fsid[i];
	}

	kprintf("\r\n");
	Drive[drive].TamSector = 512;

	return 1;
}

BYTE GetDriveParams(BYTE drive, DRIVE_PARAM *dp)
{
	BYTE status;
	BYTE cmos_drive_type;
	BYTE cylinders;
	BYTE track_sectors;
	BYTE nsides;
	BYTE ndrives;
	WORD rcylinders;
	BYTE r = 0;
	WORD pseg;
	WORD pofs;
	asm {
		push es;
		push di;
		mov ah, 0x8;
		mov dl, byte ptr drive;
		int 0x13;
		mov status, ah;
		jc error;
	
		mov ax, es;
		mov pseg, ax;
		mov pofs, di;

		mov cmos_drive_type, bl;
		mov cylinders, ch;
		mov track_sectors, cl;
		mov nsides, dh;
		mov ndrives, dl;
		mov r, 1;
	}
error:
	asm {
		pop di;
		pop es;
	}

	rcylinders = ((track_sectors & 0xC0)<<2) | cylinders;
	track_sectors &= 0x3F;

	dp->SectorsPista = track_sectors;
	dp->Heads = nsides+1;
	dp->Tracks = rcylinders+1;
	dp->BytesSector = 512;
	dp->TotalSectors = dp->SectorsPista*dp->Heads*dp->Tracks;

	return r;
}

BYTE LerSector(BYTE drive,WORD sector,void far *buffer)
{
	WORD head,track,sect,r;
	DRIVE_PARAM *p;
	/*
	if((Drive[drive].Montada==FALSE)||(drive>=MAXDRIVES))
		return 0;
	*/
	p = &(Drive[drive].p);

	if(sector>=p->TotalSectors) return 0;

	r=sector/p->SectorsPista;
	sect=sector%p->SectorsPista+1;
	head=r%p->Heads;
	track=r/p->Heads;
	return (LerSectorFisico(drive,head,track,sect,buffer));
}

BYTE EscreverSector(BYTE drive,WORD sector,void far *buffer)
{
	WORD head,track,sect,r;
	DRIVE_PARAM *p;

	if((Drive[drive].Montada==FALSE)||(drive>=MAXDRIVES))
		return 0;

	p = &(Drive[drive].p);

	if(sector>=p->TotalSectors) return 0;

	r=sector/p->SectorsPista;
	sect=sector%p->SectorsPista+1;
	head=r%p->Heads;
	track=r/p->Heads;
	return (EscreverSectorFisico(drive,head,track,sect,buffer));
}

BYTE LerSectorFisico(BYTE drive, WORD head, WORD track, WORD sector, char far *buffer)
{
	WORD retries=0;
	BYTE OK=0;
	BYTE c,h,s;

	int segb=FP_SEG(buffer),ofsb=FP_OFF(buffer);
	char r;
	if (drive>=MAXDRIVES)
		return 0;

	if (head>255 || track>1023 || sector > 63)
		return 0;
	
	s = sector | ((track&0x300)>>2);
	c = track & 0xFF;
	h = head;

	while((retries<NRETRIES)&&(!OK))
	{
		asm{
			mov ah,0x2;
			mov al,0x1;
			mov ch,c;
			mov cl,s;
			mov dh,h;
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
		retries++;
	}
	return OK;
}

BYTE EscreverSectorFisico(BYTE drive,WORD head,WORD track,WORD sector,char far *buffer)
{
	WORD retries=0;
	int segb=FP_SEG(buffer),ofsb=FP_OFF(buffer);
	char r;
	BYTE c,h,s;
	BYTE OK=0;
	if(drive>=MAXDRIVES)
		return 0;
	if (head>255 || track>1023 || sector > 63)
		return 0;
	
	s = sector | ((track&0x300)>>2);
	c = track & 0xFF;
	h = head;

	while((retries<NRETRIES)&&(!OK))
	{
		asm{
			mov ah,0x3;
			mov al,0x1;
			mov ch,c;
			mov cl,s;
			mov dh,h;
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
		retries++;
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
	kprintf("FSIZE %u of %u\r\n", (WORD)BlocoControlo[bloco]->Tamanho, bloco);
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

