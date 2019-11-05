#include "vfs.h"

typedef WORD (*FSMounter)(BYTE drive);

FSMounter fsmounters[] = {
	fsnMontarDrive,
	fsext2MontarDrive,
	NULL
};

#define DRIVERFOR(unit, ret) IFS *ifsd=fsbaseGetDriver(unit); if (!ifsd) return ret;
#define DRIVERFORFCB(b, ret) IFS *ifsd=fsbaseGetDriverForFcb(b); if (!ifsd) return ret;

void CriarFicheiro(BYTE drive,BYTE far *nome,BYTE attr)
{
	DRIVERFOR(drive,)
	ifsd->vCriarFicheiro(drive, nome, attr);
}

void Chmod(BYTE drive,BYTE far *nome,BYTE attr)
{
	DRIVERFOR(drive,)
	ifsd->vChmod(drive, nome, attr);
}

void Renomear(BYTE drive,BYTE far *nome,BYTE far *novo_nome)
{
	DRIVERFOR(drive,)
	ifsd->vRenomear(drive, nome, novo_nome);
}

void Eliminar(BYTE drive,BYTE far *nome)
{
	DRIVERFOR(drive,)
	ifsd->vEliminar(drive, nome);
}

void FecharFicheiro(WORD bloco)
{
	DRIVERFORFCB(bloco,)
	ifsd->vFecharFicheiro(bloco);
}

void FlushFicheiro(WORD bloco)
{
	DRIVERFORFCB(bloco,)
	ifsd->vFlushFicheiro(bloco);
}

BYTE GetAttr(BYTE drive,BYTE far *nome)
{
	DRIVERFOR(drive,0xFF)
	return ifsd->vGetAttr(drive, nome);
}	

WORD AbrirFicheiro(BYTE drive,BYTE far *nome,BYTE modo)
{
	DRIVERFOR(drive, 0xFFFF)
	return ifsd->vAbrirFicheiro(drive, nome, modo);
}

WORD EscreverCaracter(WORD bloco,BYTE caracter)
{
	DRIVERFORFCB(bloco, 0)
	return ifsd->vEscreverCaracter(bloco, caracter);
}

WORD LerCaracter(WORD bloco)
{
	DRIVERFORFCB(bloco, 0xFFFF)
	return ifsd->vLerCaracter(bloco);
}

void EscreverCluster(BYTE drive,WORD cluster,BYTE far *buffer)
{
	DRIVERFOR(drive,)
	ifsd->vEscreverCluster(drive, cluster, buffer);
}

void LerCluster(BYTE drive,WORD cluster,BYTE far *buffer)
{
	DRIVERFOR(drive,)
	ifsd->vLerCluster(drive, cluster, buffer);
}

void PosicionarFicheiro(WORD bloco,long deslocamento,BYTE modo)
{
	DRIVERFORFCB(bloco,)
	ifsd->vPosicionarFicheiro(bloco, deslocamento, modo);
}

void TruncarFicheiro(WORD bloco)
{
	DRIVERFORFCB(bloco, )
	ifsd->vTruncarFicheiro(bloco);
}

WORD FreeClusters(BYTE drive)
{
	DRIVERFOR(drive, 0)
	return ifsd->vFreeClusters(drive);
}

DWORD FreeSpace(BYTE drive)
{
	DRIVERFOR(drive, 0)
	return ifsd->vFreeSpace(drive);
}

DWORD DiskSpace(BYTE drive)
{
	DRIVERFOR(drive, 0)
	return ifsd->vDiskSpace(drive);
}

BYTE Montada(BYTE drive)
{
	return fsbaseMontada(drive);
}

BYTE Fim_Ficheiro(WORD bloco)
{
	return fsbaseFim_Ficheiro(bloco);
}

DWORD Ficheiro_Pos(WORD bloco)
{
	return fsbaseFicheiro_Pos(bloco);
}

DWORD Ficheiro_Size(WORD bloco)
{
	return fsbaseFicheiro_Size(bloco);
}

WORD LerFicheiro(WORD bloco, char far *ptr, WORD len)
{
	DRIVERFORFCB(bloco, 0)
	return ifsd->vLerFicheiro(bloco, ptr, len);
}

WORD EscreverFicheiro(WORD bloco, char far *ptr, WORD len)
{
	DRIVERFORFCB(bloco, 0)
	return ifsd->vEscreverFicheiro(bloco, ptr, len);
}

void FecharFicheirosDrive(BYTE drive)
{
	WORD cont;
	if((Drive[drive].Montada==FALSE)||(drive>=MAXDRIVES))
	{
		ERRO=EINVDRV;
		return;
	}
	for(cont=0;cont<FILES;cont++)
	{
		if(BlocoControlo[cont]->Drive==drive)
			FecharFicheiro(cont);
	}
}

void FecharFicheirosProcesso(WORD procid)
{
	WORD cont;
	for(cont=0;cont<FILES;cont++)
	{
		if(BlocoControlo[cont]->Procid==procid)
			FecharFicheiro(cont);
	}
}

WORD MontarDrive(BYTE drive)
{
	int i=0;

	if (drive > MAXDRIVES)
		return 0;

	if (Drive[drive].Montada)
		return 1;

	if (!fsbaseMount(drive))
		return 0;

	while(fsmounters[i]) {
		WORD r = fsmounters[i](drive);
		if (r)
			return r;
		if (ERRO!=EINVFS) // other error, so skip
			return 0;
		i++;
	}
	return 0;
}

WORD DesMontarDrive(BYTE drive)
{
	DRIVERFOR(drive, 0)

	FecharFicheirosDrive(drive);

	return ifsd->vDesMontarDrive(drive);
}

WORD INITFS()
{
	return fsbaseInitFs();
}

void CLOSEFS()
{
	BYTE cont;
	for(cont=0;cont<MAXDRIVES;cont++)
		if(Drive[cont].Montada)
			DesMontarDrive(cont);
	fsbaseCloseFs();
}

WORD SyncDrive(BYTE drive)
{
	DRIVERFOR(drive, 0)

	return ifsd->vSyncDrive(drive);
}

BYTE DirProcura(BYTE drive, TDIR_RECORD far *rec, BYTE first)
{
	DRIVERFOR(drive, 0)

	return ifsd->vDirProcura(drive, rec, first);
}

