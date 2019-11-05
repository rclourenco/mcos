#include "fserror.h"
#include "fsnative.h"
#include "mcosmem.h"
#include "mcoslib.h"

WORD fsniGetFat(BYTE drive,WORD ParentCluster);                      // internal
void fsniSetFat(BYTE drive,WORD ParentCluster,WORD Cluster);         // internal
WORD fsniAlocaCluster(BYTE drive);
void fsniLibertaClusters(BYTE drive,WORD cluster);
WORD fsniProcurarEntrada(BYTE far *nome,BYTE drive);
BYTE fsniNomeFicheiroCmp(BYTE far *fnome,BYTE far *nome);
void fsniSetNomeEntrada(BYTE far *fnome,BYTE far *nome);
void fsniGetNomeEntrada(BYTE far *fnome,BYTE far *nome);
WORD fsniGetEntrada(BYTE drive,BYTE first);
BYTE fsniVerificarAberto(TDIR far *entrada);

FSN_DATA far *fsnDriveData(BYTE drive)
{
	return (FSN_DATA far *)Drive[drive].fs_data;
}

void fsnEliminar(BYTE drive,BYTE far *nome)
{
	WORD entrada;
	BYTE aberto;
	FSN_DATA far *dd;
	ERRO=FALSE;
	if((Drive[drive].Montada==FALSE)||(drive>=MAXDRIVES))
	{
		ERRO=EINVDRV;
		return;
	}
	if(Strlen(nome)==0)
	{
		ERRO=EINVFILE;
		return;
	}

	dd = fsnDriveData(drive);
	entrada=fsniProcurarEntrada(nome,drive);
	if(entrada>dd->NEntradas)
	{
		ERRO=ENOFILE;
		return;
	}
	if(dd->Entrada[entrada].Attr==So_leitura)
	{
		ERRO=EACCES;
		return;
	}
	aberto=fsniVerificarAberto(&(dd->Entrada[entrada]));
	if(aberto)
	{
		ERRO=EACCES;
		return;
	}
	fsniLibertaClusters(drive,dd->Entrada[entrada].FCluster);
	fsniSetNomeEntrada(dd->Entrada[entrada].Nome,"");
}

void fsnRenomear(BYTE drive,BYTE far *nome,BYTE far *novo_nome)
{
	WORD entrada,entrada2;
	WORD len;
	FSN_DATA far *dd;

	ERRO=FALSE;
	if((Drive[drive].Montada==FALSE)||(drive>=MAXDRIVES))
	{
		ERRO=EINVDRV;
		return;
	}
	if(Strlen(nome)==0)
	{
		ERRO=EINVFILE;
		return;
	}

	dd=fsnDriveData(drive);
	entrada=fsniProcurarEntrada(nome,drive);
	if(entrada>dd->NEntradas)
	{
		ERRO=ENOFILE;
		return;
	}
	len=Strlen(novo_nome);
	if((len<1)||(len>12))
	{
		ERRO=EINVFILE;
		return;
	}
	entrada2=fsniProcurarEntrada(novo_nome,drive);
	if((entrada2<dd->NEntradas)&&(entrada2!=entrada))
	{
		ERRO=EEXIST;
		return;
	}
	if(dd->Entrada[entrada].Attr==So_leitura)
	{
		ERRO=EACCES;
		return;
	}
	fsniSetNomeEntrada(dd->Entrada[entrada].Nome,novo_nome);
}

void fsnChmod(BYTE drive,BYTE far *nome,BYTE attr)
{
	WORD entrada;
	FSN_DATA far *dd;

	ERRO=FALSE;
	if((Drive[drive].Montada==FALSE)||(drive>=MAXDRIVES))
	{
		ERRO=EINVDRV;
		return;
	}
	if(Strlen(nome)==0)
	{
		ERRO=EINVFILE;
		return;
	}

	dd = fsnDriveData(drive);
	entrada=fsniProcurarEntrada(nome,drive);
	if(entrada>dd->NEntradas)
	{
		ERRO=ENOFILE;
		return;
	}
	dd->Entrada[entrada].Attr=attr;
}

BYTE fsnGetAttr(BYTE drive,BYTE far *nome)
{
	WORD entrada;
	FSN_DATA far *dd;

	ERRO=FALSE;
	if((Drive[drive].Montada==FALSE)||(drive>=MAXDRIVES))
	{
		ERRO=EINVDRV;
		return 0xFF;
	}
	if(Strlen(nome)==0)
	{
		ERRO=EINVFILE;
		return 0xFF;
	}
	entrada=fsniProcurarEntrada(nome,drive);

	dd = fsnDriveData(drive);
	if(entrada>dd->NEntradas)
	{
		ERRO=ENOFILE;
		return 0xFF;
	}
	return dd->Entrada[entrada].Attr;
}

WORD fsniGetEntrada(BYTE drive,BYTE first)
{
	WORD cont;
	FSN_DATA far *dd;

	ERRO=FALSE;
	if((Drive[drive].Montada==FALSE)||(drive>=MAXDRIVES))
	{
		ERRO=EINVDRV;
		return 0xFFFF;
	}

	dd = fsnDriveData(drive);

	if(first)
		dd->_UE=0;
	if(dd->_UE>=dd->NEntradas)
		dd->_UE=0;
	for(cont=dd->_UE;cont<dd->NEntradas;cont++)
	{
		if(dd->Entrada[cont].Nome[0])
		{
			dd->_UE=cont+1;
			return cont;
		}
	}
	dd->_UE=cont;
	return 0xFFFF;
}

void fsnCriarFicheiro(BYTE drive,BYTE far *nome,BYTE attr)
{
	WORD entrada;
	FSN_DATA far *dd;
	ERRO=FALSE;
	if((Drive[drive].Montada==FALSE)||(drive>=MAXDRIVES))
	{
		ERRO=EINVDRV;
		return;
	}
	if(Strlen(nome)==0)
	{
		ERRO=EINVFILE;
		return;
	}
	entrada=fsniProcurarEntrada(nome,drive);

	dd = fsnDriveData(drive);
	if(entrada<dd->NEntradas)
	{
		if(dd->Entrada[entrada].Attr&So_leitura)
		{
			ERRO=EACCES;
			return;
		}
		fsniLibertaClusters(drive,dd->Entrada[entrada].FCluster);
	}
	else
	{
		entrada=fsniProcurarEntrada("",drive);
		if(entrada>=dd->NEntradas)
		{
			ERRO=EFAULT;
			return;
		}
		fsniSetNomeEntrada(dd->Entrada[entrada].Nome,nome);
	}
	dd->Entrada[entrada].FCluster=0;
	dd->Entrada[entrada].Tamanho=0;
	dd->Entrada[entrada].Attr=attr;
	dd->Entrada[entrada].Data=Ler_Packed_Data();
	dd->Entrada[entrada].Hora=Ler_Packed_Hora();
}

void fsniLibertaClusters(BYTE drive,WORD cluster)
{
	WORD pcluster;
	FSN_DATA far *dd;
	if((Drive[drive].Montada==FALSE)||(drive>=MAXDRIVES))
		return;
	dd = fsnDriveData(drive);

	if(cluster>dd->NClusters)
		return;
	while((cluster>0)&&(cluster<__EOF))
	{
		pcluster=cluster;
		cluster=fsniGetFat(0,cluster);
		fsniSetFat(drive,pcluster,0);
	}
}

WORD fsnMontarDrive(BYTE drive)
{
	WORD dirsect;
	WORD _fatsize,_dirsize;
	BYTE far *bootid;
	BYTE far *endereco;
	WORD sector;
	WORD cont;
	FSN_DATA far *dd;

	ERRO=FALSE;

	bootid=(BYTE far *)&(Drive[drive].FsID);
	if((bootid[0]!='M')||(bootid[1]!='C')||(bootid[2]!='F')||
		(bootid[3]!='S')||(bootid[4]!='0')||(bootid[5]!='0'))
	{
		ERRO=EINVFS;
		return 0;
	}
	Drive[drive].ifsdriver = fsnGetDriver();
	Drive[drive].fs_data = (FSN_DATA far *)AlocaMemoria(sizeof(FSN_DATA));
	
	dd = fsnDriveData(drive);
	if (!FP_SEG(dd)) {
		ERRO=EMEM;
		return 0;
	}

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


	return 1;
_abort_mount:
	Drive[drive].Montada=FALSE;
	LibertaMemoria(dd->FAT);
	LibertaMemoria(dd->Entrada);
	return 0;
}

WORD fsnDesMontarDrive(BYTE drive)
{
	FSN_DATA far *dd;

	if(!(drive<MAXDRIVES))
	{
		ERRO=EINVDRV;
		return 0;
	}
	if(Drive[drive].Montada==FALSE)
	{
		ERRO=EINVDRV;
		return 0;
	}
	
	fsnSyncDrive(drive);
	Drive[drive].Montada=FALSE;

	dd = fsnDriveData(drive);
	
	LibertaMemoria(dd->FAT);
	LibertaMemoria(dd->Entrada);
	LibertaMemoria(dd);

	return 0;
}



WORD fsniGetFat(BYTE drive,WORD ParentCluster)
{
	WORD index;
	FSN_DATA far *dd;

	if((Drive[drive].Montada==FALSE)||(drive>=MAXDRIVES))
		return 0xFFFF;

	dd = fsnDriveData(drive);

	if((ParentCluster<1)||(ParentCluster>dd->NClusters))
	{
		return 0xFFFF;
	}
	index=ParentCluster-1;
	return dd->FAT[index];
}

void fsniSetFat(BYTE drive,WORD ParentCluster,WORD Cluster)
{
	WORD index;
	FSN_DATA far *dd;

	if((Drive[drive].Montada==FALSE)||(drive>=MAXDRIVES))
	{
		ERRO=EINVDRV;
		return ;
	}

	dd = fsnDriveData(drive);

	if((ParentCluster<1)||(ParentCluster>dd->NClusters))
	{
		ERRO=EFAULT;
		return ;
	}
	index=ParentCluster-1;
	dd->FAT[index]=Cluster;
}

WORD fsnSyncDrive(BYTE drive)
{
	BYTE far *endereco;
	WORD sector;
	WORD cont;
	FSN_DATA far *dd;

	ERRO=FALSE;
	if(!(drive<MAXDRIVES))
	{
		ERRO=EINVDRV;
		return 0;
	}
	if(!Drive[drive].Montada)
	{
		ERRO=EINVDRV;
		return 0;
	}

	dd = fsnDriveData(drive);

	endereco=(BYTE far *)dd->FAT;
	sector=dd->FATSector;
	for(cont=0;cont<dd->_fatsize;cont++)
	{
		if(!(EscreverSector(drive,sector,endereco)))
		{
			ERRO=EINOUT;
			return 0;
		}
		sector++;
		cont+=Drive[drive].TamSector;
		endereco+=Drive[drive].TamSector;
	}
	endereco=(BYTE far *)dd->Entrada;
	sector=dd->DirSector;
	for(cont=0;cont<dd->_dirsize;cont++)
	{
		if(!(EscreverSector(drive,sector,endereco)))
		{
			ERRO=EINOUT;
			return 0;
		}
		sector++;
		cont+=Drive[drive].TamSector;
		endereco+=Drive[drive].TamSector;
	}
	return 0;
}

WORD fsniAlocaCluster(BYTE drive)
{
	WORD cont;
	FSN_DATA far *dd;

	if((Drive[drive].Montada==FALSE)||(drive>=MAXDRIVES))
	{
		ERRO=EINVDRV;
		return 0;
	}

	dd = fsnDriveData(drive);

	if(dd->_UCA>=dd->NClusters)
		dd->_UCA=0;
	for(cont=dd->_UCA+1;cont<=dd->NClusters;cont++)
	{
		if (fsniGetFat(drive,cont)==0)
		{
			fsniSetFat(drive,cont,__EOF);
			dd->_UCA=cont;
			return cont;
		}
	}
	for(cont=1;cont<=dd->_UCA;cont++)
	{
		if (fsniGetFat(drive,cont)==0)
		{
			fsniSetFat(drive,cont,__EOF);
			dd->_UCA=cont;
			return cont;
		}
	}
	return 0;
}

WORD fsniProcurarEntrada(BYTE far *nome,BYTE drive)
{
	WORD cont;
	FSN_DATA far *dd;

	if((Drive[drive].Montada==FALSE)||(drive>=MAXDRIVES))
	{
		ERRO=EINVDRV;
		return 0xFFFF;
	}

	dd = fsnDriveData(drive);

	for(cont=0;cont<dd->NEntradas;cont++)
	{
		if(fsniNomeFicheiroCmp(dd->Entrada[cont].Nome,nome))
			return cont;
	}
	return 0xFFFF;
}

BYTE fsniNomeFicheiroCmp(BYTE far *fnome,BYTE far *nome)
{
	BYTE nome2[20];
	fsniGetNomeEntrada(fnome,nome2);
	if(Strcmp(nome,nome2))
		return 0;
	else
		return 1;
}

void fsniSetNomeEntrada(BYTE far *fnome,BYTE far *nome)
{
	WORD cont;
	WORD len;
	len=Strlen(nome);
	if(len>12)
		return;
	for (cont=0;cont<len;cont++)
	{
		fnome[cont]=nome[cont];
	}
	if(cont<12)
		fnome[cont]=0;
}

void fsniGetNomeEntrada(BYTE far *fnome,BYTE far *nome)
{
	WORD cont;
	for (cont=0;cont<12;cont++)
	{
		nome[cont]=fnome[cont];
	}
	nome[cont]=0;
}

WORD fsnAbrirFicheiro(BYTE drive,BYTE far *nome,BYTE modo)
{
	WORD entrada;
	WORD bloco;
	TFCB far *enderecobloco;
	BYTE far *enderecobuffer;
	WORD buffersize;
	BYTE aberto=FALSE;
	FSN_DATA far *dd;
	TDIR far *b_ent;

	ERRO=FALSE;
	if(Strlen(nome)==0)
	{
		ERRO=EINVFILE;
		return 0xFFFF;
	}
	if((Drive[drive].Montada==FALSE)||(drive>=MAXDRIVES))
	{
		ERRO=EINVDRV;
		return 0xFFFF;
	}

	dd = fsnDriveData(drive);

	entrada=fsniProcurarEntrada(nome,drive);
	if(entrada<dd->NEntradas)
		aberto=fsniVerificarAberto(&(dd->Entrada[entrada]));
	if(((modo&CRIAR_F)||(modo&ESCREVER))&&(aberto))
	{
		ERRO=EACCES;
		return 0xFFFF;
	}
	if(modo&CRIAR_F)
	{
		fsnCriarFicheiro(drive,nome,Sem_atributos);
		if(ERRO)
			return 0xFFFF;
	}
	entrada=fsniProcurarEntrada(nome,drive);
	if(entrada>=dd->NEntradas)
	{
		ERRO=ENOFILE;
		return 0xFFFF;
	}
	if((modo&ESCREVER)&&(dd->Entrada[entrada].Attr&So_leitura))
	{
		ERRO=EACCES;
		return 0xFFFF;
	}
	bloco=GetFreeBloco();
	if(bloco>=FILES)
	{
		ERRO=EMFILE;
		return 0xFFFF;
	}
	enderecobloco=(TFCB far *)AlocaMemoria(sizeof(TFCB));
	if(FP_SEG(enderecobloco)==0)
	{
		ERRO=EMEM;
		return 0xFFFF;
	}
	buffersize=Drive[drive].TamSector*dd->SectorCluster;
	enderecobuffer=(BYTE far *)AlocaMemoria(buffersize);
	if(FP_SEG(enderecobuffer)==0)
	{
		LibertaMemoria(enderecobloco);
		ERRO=EMEM;
		return 0xFFFF;
	}

	b_ent = &(dd->Entrada[entrada]);

	BlocoControlo[bloco]=enderecobloco;
	BlocoControlo[bloco]->Inode = entrada+1;
	BlocoControlo[bloco]->Tamanho=b_ent->Tamanho;
	BlocoControlo[bloco]->Modo=modo;
	BlocoControlo[bloco]->Fpos=0;
	BlocoControlo[bloco]->Drive=drive;
	BlocoControlo[bloco]->Buffer=enderecobuffer;
	BlocoControlo[bloco]->FCluster=b_ent->FCluster;
	BlocoControlo[bloco]->Cluster=0;
	BlocoControlo[bloco]->Procid=CurProcess;
	return bloco;
}

TDIR far *fsnBlocoEntrada(TFCB far *fcb) {
	BYTE drive;
	FSN_DATA far *dd;

	if (!FP_SEG(fcb))
		return MK_FP(0,0);

	dd=fsnDriveData(fcb->Drive);
	if (fcb->Inode>0L)
		return &dd->Entrada[fcb->Inode-1];
	return MK_FP(0,0);	
}

BYTE fsniVerificarAberto(TDIR far *entrada)
{
	WORD cont;
	for(cont=0;cont<FILES;cont++)
	{
		if(FP_SEG(BlocoControlo[cont]))
		{
			TDIR far *b_ent = fsnBlocoEntrada(BlocoControlo[cont]);
			if(FP_SEG(b_ent) && b_ent==entrada)
				 return TRUE;
		}
	}
	return FALSE;
}

void fsnFecharFicheiro(WORD bloco)
{
	ERRO=FALSE;
	if((FP_SEG(BlocoControlo[bloco])==0)||(bloco>=FILES))
	{
		ERRO=EBADF;
		return;
	}
	if(BlocoControlo[bloco]->Modo&ESCREVER)
	{
		TDIR far *b_ent;
		fsnFlushFicheiro(bloco);

		b_ent = fsnBlocoEntrada(BlocoControlo[bloco]);

		b_ent->Tamanho=BlocoControlo[bloco]->Tamanho;
		b_ent->FCluster=BlocoControlo[bloco]->FCluster;
		b_ent->Data=Ler_Packed_Data();
		b_ent->Hora=Ler_Packed_Hora();
	}
	LibertaMemoria(BlocoControlo[bloco]->Buffer);
	LibertaMemoria(BlocoControlo[bloco]);
	BlocoControlo[bloco]=(TFCB far *)MK_FP(0,0);
}

void fsnFlushFicheiro(WORD bloco)
{
	WORD deslocamento;
	WORD tamcluster;
	BYTE drive;
	FSN_DATA far *dd;

	if((FP_SEG(BlocoControlo[bloco])==0)||(bloco>=FILES))
	{
		ERRO=EBADF;
		return;
	}
	drive=BlocoControlo[bloco]->Drive;
	dd=fsnDriveData(drive);
	tamcluster=Drive[drive].TamSector*dd->SectorCluster;
	deslocamento=BlocoControlo[bloco]->Fpos%tamcluster;
	if(deslocamento)
	{
		fsnEscreverCluster(drive,BlocoControlo[bloco]->Cluster,BlocoControlo[bloco]->Buffer);
		if(ERRO)
		{
			return;
		}
	}
}

WORD fsnEscreverCaracter(WORD bloco,BYTE caracter)
{
	WORD deslocamento;
	WORD tamcluster;
	WORD cluster_anterior;
	BYTE drive;
	FSN_DATA far *dd;

	ERRO=FALSE;
	if((FP_SEG(BlocoControlo[bloco])==0)||(bloco>=FILES))
	{
		ERRO=EBADF;
		return 0;
	}
	if(!(BlocoControlo[bloco]->Modo&ESCREVER))
	{
		ERRO=EACCES;
		return 0;
	}
	if(!(BlocoControlo[bloco]->Fpos<BlocoControlo[bloco]->Tamanho))
	{
		if(!(BlocoControlo[bloco]->Modo&EXPANDIR))
		{
			return __EOF;
		}
	}
	drive=BlocoControlo[bloco]->Drive;
	dd=fsnDriveData(drive);
	tamcluster=Drive[drive].TamSector*dd->SectorCluster;
	deslocamento=BlocoControlo[bloco]->Fpos%tamcluster;
	if(deslocamento==0)
	{
		cluster_anterior=BlocoControlo[bloco]->Cluster;
		if(BlocoControlo[bloco]->FCluster==0)
		{
			BlocoControlo[bloco]->FCluster=fsniAlocaCluster(drive);
			BlocoControlo[bloco]->Cluster=BlocoControlo[bloco]->FCluster;
		}
		else
		{
			if(BlocoControlo[bloco]->Cluster==0)
				  BlocoControlo[bloco]->Cluster=BlocoControlo[bloco]->FCluster;
			else
				  BlocoControlo[bloco]->Cluster=fsniGetFat(drive,BlocoControlo[bloco]->Cluster);
			if(BlocoControlo[bloco]->Cluster==0)
			{
				BlocoControlo[bloco]->Cluster=cluster_anterior;
				ERRO=EFAULT;
				return 0;
			}
			if(BlocoControlo[bloco]->Cluster>=__EOF)
			{
				BlocoControlo[bloco]->Cluster=fsniAlocaCluster(drive);
				fsniSetFat(drive,cluster_anterior,BlocoControlo[bloco]->Cluster);
			}
			else
			{
				fsnLerCluster(drive,BlocoControlo[bloco]->Cluster,BlocoControlo[bloco]->Buffer);
				if(ERRO)
				{
					BlocoControlo[bloco]->Cluster=cluster_anterior;
					return 0;
				}
			}
		}
		if(BlocoControlo[bloco]->Cluster==0)
		{
			ERRO=EFAULT;
			return 0;
		}
	}
	BlocoControlo[bloco]->Buffer[deslocamento]=caracter;
	BlocoControlo[bloco]->Fpos++;
	deslocamento=BlocoControlo[bloco]->Fpos%tamcluster;
	if(deslocamento==0)
	{
		fsnEscreverCluster(drive,BlocoControlo[bloco]->Cluster,BlocoControlo[bloco]->Buffer);
		if(ERRO)
		{
			BlocoControlo[bloco]->Fpos--;
			return 0;
		}
	}
	if(BlocoControlo[bloco]->Fpos>BlocoControlo[bloco]->Tamanho)
		BlocoControlo[bloco]->Tamanho=BlocoControlo[bloco]->Fpos;
	return 0;

}


WORD fsnEscreverFicheiro(WORD bloco, char far *ptr, WORD len)
{
	WORD deslocamento;
	WORD tamcluster;
	WORD cluster_anterior;
	BYTE drive;
	WORD count=0;
	WORD ksize=0;
	FSN_DATA far *dd;

	ERRO=FALSE;
	if((BlocoControlo[bloco]==NULL)||(bloco>=FILES))
	{
		ERRO=EBADF;
		return 0;
	}
	if(!(BlocoControlo[bloco]->Modo&ESCREVER))
	{
		ERRO=EACCES;
		return 0;
	}
	if(BlocoControlo[bloco]->Fpos+len>BlocoControlo[bloco]->Tamanho)
	{
		if(!(BlocoControlo[bloco]->Modo&EXPANDIR))
		{
		  /*TODO reduce length*/
			return 0;
		}
	}
	drive=BlocoControlo[bloco]->Drive;
	dd=fsnDriveData(drive);
	tamcluster=Drive[drive].TamSector*dd->SectorCluster;
	while(len) {
	  deslocamento=BlocoControlo[bloco]->Fpos%tamcluster;
	  if(deslocamento==0)
	  {
		  cluster_anterior=BlocoControlo[bloco]->Cluster;
		  if(BlocoControlo[bloco]->FCluster==0)
		  {
			  BlocoControlo[bloco]->FCluster=fsniAlocaCluster(drive);
			  BlocoControlo[bloco]->Cluster=BlocoControlo[bloco]->FCluster;
		  }
		  else
		  {
			  if(BlocoControlo[bloco]->Cluster==0)
				    BlocoControlo[bloco]->Cluster=BlocoControlo[bloco]->FCluster;
			  else
				    BlocoControlo[bloco]->Cluster=fsniGetFat(drive,BlocoControlo[bloco]->Cluster);
			  if(BlocoControlo[bloco]->Cluster==0)
			  {
				  BlocoControlo[bloco]->Cluster=cluster_anterior;
				  ERRO=EFAULT;
				  return count;
			  }
			  if(BlocoControlo[bloco]->Cluster>=__EOF)
			  {
				  BlocoControlo[bloco]->Cluster=fsniAlocaCluster(drive);
				  fsniSetFat(drive,cluster_anterior,BlocoControlo[bloco]->Cluster);
			  }
			  else
			  {
				  fsnLerCluster(drive,BlocoControlo[bloco]->Cluster,BlocoControlo[bloco]->Buffer);
				  if(ERRO)
				  {
					  BlocoControlo[bloco]->Cluster=cluster_anterior;
					  return count;
				  }
			  }
		  }
		  if(BlocoControlo[bloco]->Cluster==0)
		  {
			  ERRO=EFAULT;
			  return count;
		  }
	  }
	  ksize = tamcluster-deslocamento;

	  if(ksize==0)
	    break;

	  if(ksize>len)
	    ksize=len;

	  _fmemcpy(BlocoControlo[bloco]->Buffer+deslocamento,ptr+count,ksize);

	  BlocoControlo[bloco]->Fpos+=ksize;
	  count+=ksize;
	  len-=ksize;

	  deslocamento=BlocoControlo[bloco]->Fpos%tamcluster;
	  if(deslocamento==0)
	  {
		  fsnEscreverCluster(drive,BlocoControlo[bloco]->Cluster,BlocoControlo[bloco]->Buffer);
		  if(ERRO)
		  {
			  BlocoControlo[bloco]->Fpos-=ksize;
			  count-=ksize;
			  return count;
		  }
	  }
	  if(BlocoControlo[bloco]->Fpos>BlocoControlo[bloco]->Tamanho)
		  BlocoControlo[bloco]->Tamanho=BlocoControlo[bloco]->Fpos;
	}
	return count;

}

void fsnEscreverCluster(BYTE drive,WORD cluster,BYTE far *buffer)
{
	WORD cont;
	WORD sector;
	FSN_DATA far *dd;

	ERRO=FALSE;
	if((Drive[drive].Montada==FALSE)||(drive>=MAXDRIVES))
	{
		ERRO=EINVDRV;
		return;
	}
	dd = fsnDriveData(drive);
	if((cluster<1)||(cluster>dd->NClusters))
	{
		ERRO=EFAULT;
		return;
	}
	sector=dd->DadosSector+(cluster-1)*dd->SectorCluster;
	for(cont=0;cont<dd->SectorCluster;cont++)
	{
	  if(!(EscreverSector(drive,sector,buffer)))
		{
			ERRO=EINOUT;
			return ;
		}
	  buffer=buffer+Drive[drive].TamSector;
	  sector++;
	}
}

void fsnLerCluster(BYTE drive,WORD cluster,BYTE far *buffer)
{
	WORD cont;
	WORD sector;
	FSN_DATA far *dd;

	ERRO=FALSE;

	if((Drive[drive].Montada==FALSE)||(drive>=MAXDRIVES))
	{
		ERRO=EINVDRV;
		return;
	}

	dd = fsnDriveData(drive);

	if((cluster<1)||(cluster>dd->NClusters))
	{
		ERRO=EFAULT;
		return;
	}
	sector=dd->DadosSector+(cluster-1)*dd->SectorCluster;
	for(cont=0;cont<dd->SectorCluster;cont++)
	{
	  if(!(LerSector(drive,sector,buffer)))
		{
			ERRO=EINOUT;
			return ;
		}
	  buffer=buffer+Drive[drive].TamSector;
	  sector++;
	}
}

WORD fsnLerCaracter(WORD bloco)
{
	WORD deslocamento;
	WORD cluster_anterior;
	WORD tamcluster;
	BYTE caracter;
	BYTE drive;
	FSN_DATA far *dd;

	ERRO=FALSE;
	if((FP_SEG(BlocoControlo[bloco])==0)||(bloco>=FILES))
	{
		ERRO=EBADF;
		return 0xFFFF;
	}
	if(!(BlocoControlo[bloco]->Fpos<BlocoControlo[bloco]->Tamanho))
	{
		return __EOF;
	}
	drive=BlocoControlo[bloco]->Drive;
	dd=fsnDriveData(drive);

	tamcluster=Drive[drive].TamSector*dd->SectorCluster;
	deslocamento=BlocoControlo[bloco]->Fpos%tamcluster;
	if(deslocamento==0)
	{
		cluster_anterior=BlocoControlo[bloco]->Cluster;
		if(BlocoControlo[bloco]->Cluster==0)
			BlocoControlo[bloco]->Cluster=BlocoControlo[bloco]->FCluster;
		else
			BlocoControlo[bloco]->Cluster=fsniGetFat(drive,BlocoControlo[bloco]->Cluster);
		if(!((BlocoControlo[bloco]->Cluster>0)&&(BlocoControlo[bloco]->Cluster<__EOF)))
		{
			BlocoControlo[bloco]->Cluster=cluster_anterior;
			ERRO=EFAULT;
			return 0xFFFF;
		}
		fsnLerCluster(drive,BlocoControlo[bloco]->Cluster,BlocoControlo[bloco]->Buffer);
		if(ERRO)
		{
			BlocoControlo[bloco]->Cluster=cluster_anterior;
			return 0xFFFF;
		}
	}
	caracter=BlocoControlo[bloco]->Buffer[deslocamento];
	BlocoControlo[bloco]->Fpos++;
	return caracter;

}


WORD fsnLerFicheiro(WORD bloco, char far *ptr, WORD len)
{
	WORD deslocamento;
	WORD cluster_anterior;
	WORD tamcluster;
	BYTE caracter;
	BYTE drive;
	WORD count=0;
	WORD ksize=0;
	FSN_DATA far *dd;

	ERRO=FALSE;
	if((FP_SEG(BlocoControlo[bloco])==0)||(bloco>=FILES))
	{
		ERRO=EBADF;
		return 0;
	}
	if(!(BlocoControlo[bloco]->Fpos<BlocoControlo[bloco]->Tamanho))
	{
		return 0;
	}
	drive=BlocoControlo[bloco]->Drive;
	dd=fsnDriveData(drive);
	tamcluster=Drive[drive].TamSector*dd->SectorCluster;
	while(len) {
	  deslocamento=BlocoControlo[bloco]->Fpos%tamcluster;
	  if(deslocamento==0)
	  {
		  cluster_anterior=BlocoControlo[bloco]->Cluster;
		  if(BlocoControlo[bloco]->Cluster==0)
			  BlocoControlo[bloco]->Cluster=BlocoControlo[bloco]->FCluster;
		  else
			  BlocoControlo[bloco]->Cluster=fsniGetFat(drive,BlocoControlo[bloco]->Cluster);
		  if(!((BlocoControlo[bloco]->Cluster>0)&&(BlocoControlo[bloco]->Cluster<__EOF)))
		  {
			  BlocoControlo[bloco]->Cluster=cluster_anterior;
			  ERRO=EFAULT;
			  return 0;
		  }
		  fsnLerCluster(drive,BlocoControlo[bloco]->Cluster,BlocoControlo[bloco]->Buffer);
		  if(ERRO)
		  {
			  BlocoControlo[bloco]->Cluster=cluster_anterior;
			  return 0;
		  }
		  
	  }
	  ksize = tamcluster-deslocamento;
	  if(ksize > BlocoControlo[bloco]->Tamanho-BlocoControlo[bloco]->Fpos )
	    ksize=BlocoControlo[bloco]->Tamanho-BlocoControlo[bloco]->Fpos;

	  if(ksize==0)
	    break;

	  if(ksize>len)
	    ksize=len;

	  _fmemcpy(ptr+count,BlocoControlo[bloco]->Buffer+deslocamento,ksize);

	  BlocoControlo[bloco]->Fpos+=ksize;
	  count+=ksize;
	  len-=ksize;
	  
	  if(!(BlocoControlo[bloco]->Fpos<BlocoControlo[bloco]->Tamanho))
	    break;
	}
	return count;
}


void fsnPosicionarFicheiro(WORD bloco,long deslocamento,BYTE modo)
{
	DWORD n_pos;
	WORD n_cluster;
	WORD nciclos;
	WORD cont;
	BYTE drive;
	WORD tamcluster;
	FSN_DATA far *dd;

	ERRO=FALSE;
	if((FP_SEG(BlocoControlo[bloco])==0)||(bloco>=FILES))
	{
		ERRO=EBADF;
		return;
	}
	switch(modo)
	{
		case SF_INICIO:
			n_pos=deslocamento;break;
		case SF_CURRENTE:
			n_pos=BlocoControlo[bloco]->Fpos+deslocamento;break;
		case SF_FIM:
			n_pos=BlocoControlo[bloco]->Tamanho+deslocamento;break;
		default:
			ERRO=EFAULT;
			return;
	}
	if(n_pos>BlocoControlo[bloco]->Tamanho)
	{
		ERRO=EFAULT;
		return;
	}
	if(n_pos==0)
	{
		BlocoControlo[bloco]->Cluster=0;
		BlocoControlo[bloco]->Fpos=0;
		return;
	}
	drive=BlocoControlo[bloco]->Drive;
	dd=fsnDriveData(drive);
	tamcluster=Drive[drive].TamSector*dd->SectorCluster;
	nciclos=(n_pos-1)/tamcluster;
	n_cluster=BlocoControlo[bloco]->FCluster;
	if(n_cluster>dd->NClusters)
	{
		ERRO=EFAULT;
		return;
	}
	for(cont=0;cont<nciclos;cont++)
	{
		n_cluster=fsniGetFat(drive,n_cluster);
		if((n_cluster<1)||(n_cluster>dd->NClusters))
		{
			ERRO=EFAULT;
			return;
		}
	}
	if(n_pos%tamcluster)
	{
		fsnLerCluster(drive,n_cluster,BlocoControlo[bloco]->Buffer);
		if(ERRO)
			return;
	}
	BlocoControlo[bloco]->Cluster=n_cluster;
	BlocoControlo[bloco]->Fpos=n_pos;
}

void fsnTruncarFicheiro(WORD bloco)
{
	BYTE drive;
	ERRO=FALSE;
	if((FP_SEG(BlocoControlo[bloco])==0)||(bloco>=FILES))
	{
		ERRO=EBADF;
		return;
	}
	if(!(BlocoControlo[bloco]->Modo&ESCREVER))
	{
		ERRO=EACCES;
		return;
	}
	drive=BlocoControlo[bloco]->Drive;
	BlocoControlo[bloco]->Tamanho=BlocoControlo[bloco]->Fpos;
	if(BlocoControlo[bloco]->Tamanho==0)
	{
		fsniLibertaClusters(drive,BlocoControlo[bloco]->FCluster);
		BlocoControlo[bloco]->FCluster=0;
	}
	else
	{
		fsniLibertaClusters(drive,fsniGetFat(drive,BlocoControlo[bloco]->Cluster));
		fsniSetFat(drive,BlocoControlo[bloco]->Cluster,__EOF);
	}
}

WORD fsnFreeClusters(BYTE drive)
{
	WORD cont;
	WORD free=0;
	FSN_DATA far *dd;

	if(drive>=MAXDRIVES)
		return 0;
	if(Drive[drive].Montada==FALSE)
		return 0;

	dd = fsnDriveData(drive);

	for(cont=1;cont<=dd->NClusters;cont++)
	{
		if(fsniGetFat(drive,cont)==0)
			free++;
	}
	return free;
}

DWORD fsnFreeSpace(BYTE drive)
{
	DWORD frees;
	WORD tamcluster;
	FSN_DATA far *dd;

	if(drive>=MAXDRIVES)
		return 0;
	if(Drive[drive].Montada==FALSE)
		return 0;
	dd=fsnDriveData(drive);
	tamcluster=Drive[drive].TamSector*dd->SectorCluster;
	frees=longmul2((DWORD)fsnFreeClusters(drive),tamcluster);
	return frees;
}

DWORD fsnDiskSpace(BYTE drive)
{
	WORD tamcluster;
	FSN_DATA far *dd;

	if(drive>=MAXDRIVES)
		return 0;
	if(Drive[drive].Montada==FALSE)
		return 0;

	dd = fsnDriveData(drive);

	tamcluster=Drive[drive].TamSector*dd->SectorCluster;
	return longmul2(dd->NClusters,tamcluster);
}

BYTE fsnDirProcura(BYTE drive,TDIR_RECORD far *rec,BYTE first)
{
	WORD entrada;
	FSN_DATA far *dd;
	ERRO=FALSE;
	if((Drive[drive].Montada==FALSE)||(drive>=MAXDRIVES))
	{
		ERRO=EINVDRV;
		return 0;
	}

	dd = fsnDriveData(drive);

	entrada=fsniGetEntrada(drive,first);
	if (entrada<dd->NEntradas)
	{
		fsniGetNomeEntrada(dd->Entrada[entrada].Nome,rec->nome);
		rec->Data=UnPacked_Data(dd->Entrada[entrada].Data);
		rec->Hora=UnPacked_Hora(dd->Entrada[entrada].Hora);
		rec->Attr=dd->Entrada[entrada].Attr;
		rec->Tamanho=dd->Entrada[entrada].Tamanho;
		return 1;
	}
	else
	{
		rec->Attr=0;
		rec->Data=UnPacked_Data(0);
		rec->Hora=UnPacked_Hora(0);
		rec->Tamanho=0L;
		rec->nome[0]=0;
		return 0;
	}
}

static IFS fs_native = {
	fsnCriarFicheiro,
	fsnChmod,
	fsnRenomear,
	fsnEliminar,
	fsnFecharFicheiro,
	fsnFlushFicheiro,
	fsnGetAttr,
	fsnAbrirFicheiro,
	fsnEscreverCaracter,
	fsnLerCaracter,
	fsnEscreverCluster,
	fsnLerCluster,
	fsnPosicionarFicheiro,
	fsnTruncarFicheiro,
	fsnFreeClusters,
	fsnFreeSpace,
	fsnDiskSpace,
	fsnLerFicheiro,
	fsnEscreverFicheiro,
	fsnDesMontarDrive,
	fsnSyncDrive,
	fsnDirProcura,
};

IFS *fsnGetDriver()
{
	return &fs_native;
}
