#include "fserror.h"
#include "fsext2.h"
#include "mcosmem.h"

FSEXT2_DATA far * fsext2DriveData(BYTE drive)
{
	return (FSEXT2_DATA far *)Drive[drive].fs_data;
}

WORD fsext2MontarDrive(BYTE drive)
{
	
	//WORD dirsect;
	//WORD _fatsize,_dirsize;
	//BYTE far *bootid;
	//BYTE far *endereco;
	BYTE far *tmp;
	WORD sector;
	WORD cont;
	FSEXT2_DATA far *dd;

	ERRO=FALSE;
	if(!(drive<MAXDRIVES))
	{
		ERRO=EINVDRV;
		return 0;
	}
	if(Drive[drive].Montada)
	{
		ERRO=EINVDRV;
		return 0;
	}
	/*
	if(!(LerSectorFisico(drive,0,0,1,(char far *)BootRecord)))
	{
		ERRO=EINOUT;
		return 0;
	}
	bootid=BootRecord->Fsid;
	if((bootid[0]!='M')||(bootid[1]!='C')||(bootid[2]!='F')||
		(bootid[3]!='S')||(bootid[4]!='0')||(bootid[5]!='0'))
	{
		ERRO=EINVFS;
		return 0;
	}
	*/
	tmp = (BYTE far *)AlocaMemoria(1024);
	if (!tmp) {
		ERRO=EMEM;
		goto _abort_mount;
	}
	if(!(LerSectorFisico(drive,0,0,3,(char far *)tmp)))
	{
		LibertaMemoria(tmp);
		ERRO=EINOUT;
		return 0;
	}

	Drive[drive].ifsdriver = fsnGetDriver();
	Drive[drive].fs_data = (FSEXT2_DATA far *)AlocaMemoria(sizeof(FSEXT2_DATA));
	
	dd = fsext2DriveData(drive);
	if (!FP_SEG(dd)) {
		LibertaMemoria(tmp);
		ERRO=EMEM;
		return 0;
	}
	dd->s = (FSEXT2_SUPER far *) tmp;
	if (dd->s->magic != FSEXT2_SUPER_MAGIC) {
		ERRO = EINVFS;
		goto _abort_mount;
	}

	kprintf("Signature: %x\r\n", dd->s->magic);

	/*
	Drive[drive].TamSector=BootRecord->Bps;
	Drive[drive].SectorPista=BootRecord->Spp;
	Drive[drive].Heads=BootRecord->Hn;
	Drive[drive].NSectors=BootRecord->Lsn;

	dd->SectorCluster=BootRecord->Spc;
	dirsect=((BootRecord->Rde)*32)/Drive[drive].TamSector;
	if(((BootRecord->Rde)*32)%Drive[drive].TamSector) dirsect++;

	dd->FATSector=BootRecord->Rs+BootRecord->Ssn;
	dd->DirSector=dd->FATSector+BootRecord->Spf;

	dd->DadosSector=dd->DirSector+dirsect;
	dd->NEntradas=BootRecord->Rde;
	dd->NClusters=(BootRecord->Lsn-dd->DadosSector)/dd->SectorCluster;
	_fatsize=(BootRecord->Spf)*Drive[drive].TamSector;
	_dirsize=dirsect*Drive[drive].TamSector;

	dd->FAT=(WORD far *)AlocaMemoria(_fatsize);
	dd->Entrada=(TDIR far *)AlocaMemoria(_dirsize);
	dd->_UCA=0;
	dd->_UE=0;
	if((!FP_SEG(dd->FAT))||(!FP_SEG(dd->Entrada)))
	{
		ERRO=EMEM;
		goto _abort_mount;
	}

	Drive[drive].Montada=TRUE; // needed for LerSector

	endereco=(BYTE far *)dd->FAT;
	sector=dd->FATSector;
	for(cont=0;cont<_fatsize;cont++)
	{
		if(!(LerSector(drive,sector,endereco)))
		{
			ERRO=EINOUT;
			goto _abort_mount;
		}
		sector++;
		cont+=Drive[drive].TamSector;
		endereco+=Drive[drive].TamSector;
	}
	endereco=(BYTE far *)dd->Entrada;
	sector=dd->DirSector;
	for(cont=0;cont<_dirsize;cont++)
	{
		if(!(LerSector(drive,sector,endereco)))
		{
			ERRO=EINOUT;
			goto _abort_mount;
		}
		sector++;
		cont+=Drive[drive].TamSector;
		endereco+=Drive[drive].TamSector;
	}
	dd->_fatsize=_fatsize;
	dd->_dirsize=_dirsize;

        */
	Drive[drive].Montada=TRUE;
	return 1;
	
_abort_mount:
	Drive[drive].Montada=FALSE;
	LibertaMemoria(dd->s);
	LibertaMemoria(dd);
	return 0;

//	ERRO=EUNSUP;
//	return 0;
}

WORD fsext2DesMontarDrive(BYTE drive)
{
	(void)drive;
	return 0;
}

WORD fsext2SyncDrive(BYTE drive)
{
	(void)drive;
	return 0;
}

void fsext2CriarFicheiro(BYTE drive,BYTE far *nome,BYTE attr)
{
	(void)drive;
	(void)nome;
	(void)attr;
	ERRO=EUNSUP;
}

BYTE fsext2DirProcura(BYTE drive,TDIR_RECORD far *rec,BYTE first)
{
	(void)drive;
	(void)rec;
	(void)first;
	ERRO=EUNSUP;
	return 0;
}

void fsext2Chmod(BYTE drive,BYTE far *nome,BYTE attr)
{
	(void)drive;
	(void)nome;
	(void)attr;
	ERRO=EUNSUP;
}

void fsext2Renomear(BYTE drive,BYTE far *nome,BYTE far *novo_nome)
{
	(void)drive;
	(void)nome;
	(void)novo_nome;
	ERRO=EUNSUP;
}

void fsext2Eliminar(BYTE drive,BYTE far *nome)
{
	(void)drive;
	(void)nome;
	ERRO=EUNSUP;
}

void fsext2FecharFicheiro(WORD bloco)
{
	(void)bloco;
	ERRO=EUNSUP;
}

void fsext2FlushFicheiro(WORD bloco)
{
	(void)bloco;
	ERRO=EUNSUP;
}

BYTE fsext2GetAttr(BYTE drive,BYTE far *nome)
{
	(void)drive;
	(void)nome;
	ERRO=EUNSUP;
	return 0;
}

WORD fsext2AbrirFicheiro(BYTE drive,BYTE far *nome,BYTE modo)
{
	(void)drive;
	(void)nome;
	(void)modo;
	ERRO=EUNSUP;
	return ERRO;
}

WORD fsext2EscreverCaracter(WORD bloco,BYTE caracter)
{
	(void)bloco;
	(void)caracter;
	ERRO=EUNSUP;
	return 0;
}

WORD fsext2LerCaracter(WORD bloco)
{
	(void)bloco;
	ERRO=EUNSUP;
	return 0xFFFF;
}

void fsext2EscreverCluster(BYTE drive,WORD cluster,BYTE far *buffer)
{
	(void)drive;
	(void)cluster;
	(void)buffer;
	ERRO=EUNSUP;
}

void fsext2LerCluster(BYTE drive,WORD cluster,BYTE far *buffer)
{
	(void)drive;
	(void)cluster;
	(void)buffer;
	ERRO=EUNSUP;
}

void fsext2PosicionarFicheiro(WORD bloco,long deslocamento,BYTE modo)
{
	(void)bloco;
	(void)deslocamento;
	(void)modo;
	ERRO=EUNSUP;
}

void fsext2TruncarFicheiro(WORD bloco)
{
	(void)bloco;
	ERRO=EUNSUP;
}

WORD fsext2FreeClusters(BYTE drive)
{
	(void)drive;
	ERRO=EUNSUP;
	return 0;
}

DWORD fsext2FreeSpace(BYTE drive)
{
	(void)drive;
	ERRO=EUNSUP;
	return 0;

}

DWORD fsext2DiskSpace(BYTE drive)
{
	(void)drive;
	ERRO=EUNSUP;
	return 0;

}

BYTE fsext2Montada(BYTE drive)
{
	(void)drive;
	return FALSE;
}

WORD fsext2LerFicheiro(WORD bloco, char far *ptr, WORD len)
{
	(void)bloco;
	(void)ptr;
	(void)len;
	ERRO=EUNSUP;
	return 0xFFFF;
}

WORD fsext2EscreverFicheiro(WORD bloco, char far *ptr, WORD len)
{
	(void)bloco;
	(void)ptr;
	(void)len;
	ERRO=EUNSUP;
	return 0;
}


static IFS fs_ext2 = {
	fsext2CriarFicheiro,
	fsext2Chmod,
	fsext2Renomear,
	fsext2Eliminar,
	fsext2FecharFicheiro,
	fsext2FlushFicheiro,
	fsext2GetAttr,
	fsext2AbrirFicheiro,
	fsext2EscreverCaracter,
	fsext2LerCaracter,
	fsext2EscreverCluster,
	fsext2LerCluster,
	fsext2PosicionarFicheiro,
	fsext2TruncarFicheiro,
	fsext2FreeClusters,
	fsext2FreeSpace,
	fsext2DiskSpace,
	fsext2LerFicheiro,
	fsext2EscreverFicheiro,
	fsext2DesMontarDrive,
	fsext2SyncDrive,
	fsext2DirProcura,
};

IFS *fsext2GetDriver()
{
	return &fs_ext2;
}
