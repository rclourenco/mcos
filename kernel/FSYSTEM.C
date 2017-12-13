#include "FSERROR.h"
#include "fsystem.h"
#include "mcosmem.h"

TDRIVE Drive[MAXDRIVES];
TFCB far * far *BlocoControlo;
BOOTRECORD far *BootRecord;
WORD ERRO;


void ApagaErro()
{
	ERRO=0;
}

BYTE Montada(BYTE drive)
{
	if(drive>=MAXDRIVES)
	{
		ERRO=EINVDRV;
		return 0;
	}
	return Drive[drive].Montada;
}

void Eliminar(BYTE drive,BYTE far *nome)
{
	WORD entrada;
	BYTE aberto;
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
	entrada=ProcurarEntrada(nome,drive);
	if(entrada>Drive[drive].NEntradas)
	{
		ERRO=ENOFILE;
		return;
	}
	if(Drive[drive].Entrada[entrada].Attr==So_leitura)
	{
		ERRO=EACCES;
		return;
	}
	aberto=VerificarAberto(&(Drive[drive].Entrada[entrada]));
	if(aberto)
	{
		ERRO=EACCES;
		return;
	}
	LibertaClusters(drive,Drive[drive].Entrada[entrada].FCluster);
	SetNomeEntrada(Drive[drive].Entrada[entrada].Nome,"");
}


BYTE Fim_Ficheiro(WORD bloco)
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

DWORD Ficheiro_Pos(WORD bloco)
{
	ERRO=FALSE;
	if((FP_SEG(BlocoControlo[bloco])==0)||(bloco>=FILES))
	{
		ERRO=EBADF;
		return 0L;
	}
	return BlocoControlo[bloco]->Fpos;
}

DWORD Ficheiro_Size(WORD bloco)
{
	ERRO=FALSE;
	if((FP_SEG(BlocoControlo[bloco])==0)||(bloco>=FILES))
	{
		ERRO=EBADF;
		return 0L;
	}
	return BlocoControlo[bloco]->Tamanho;
}


void Renomear(BYTE drive,BYTE far *nome,BYTE far *novo_nome)
{
	WORD entrada,entrada2;
	WORD len;
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
	entrada=ProcurarEntrada(nome,drive);
	if(entrada>Drive[drive].NEntradas)
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
	entrada2=ProcurarEntrada(novo_nome,drive);
	if((entrada2<Drive[drive].NEntradas)&&(entrada2!=entrada))
	{
		ERRO=EEXIST;
		return;
	}
	if(Drive[drive].Entrada[entrada].Attr==So_leitura)
	{
		ERRO=EACCES;
		return;
	}
	SetNomeEntrada(Drive[drive].Entrada[entrada].Nome,novo_nome);
}

void Chmod(BYTE drive,BYTE far *nome,BYTE attr)
{
	WORD entrada;
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
	entrada=ProcurarEntrada(nome,drive);
	if(entrada>Drive[drive].NEntradas)
	{
		ERRO=ENOFILE;
		return;
	}
	Drive[drive].Entrada[entrada].Attr=attr;
}

BYTE GetAttr(BYTE drive,BYTE far *nome)
{
	WORD entrada;
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
	entrada=ProcurarEntrada(nome,drive);
	if(entrada>Drive[drive].NEntradas)
	{
		ERRO=ENOFILE;
		return 0xFF;
	}
	return Drive[drive].Entrada[entrada].Attr;
}

WORD GetEntrada(BYTE drive,BYTE first)
{
	WORD cont;
	ERRO=FALSE;
	if((Drive[drive].Montada==FALSE)||(drive>=MAXDRIVES))
	{
		ERRO=EINVDRV;
		return 0xFFFF;
	}
	if(first)
		Drive[drive]._UE=0;
	if(Drive[drive]._UE>=Drive[drive].NEntradas)
		Drive[drive]._UE=0;
	for(cont=Drive[drive]._UE;cont<Drive[drive].NEntradas;cont++)
	{
		if(Drive[drive].Entrada[cont].Nome[0])
		{
			Drive[drive]._UE=cont+1;
			return cont;
		}
	}
	Drive[drive]._UE=cont;
	return 0xFFFF;
}

void CriarFicheiro(BYTE drive,BYTE far *nome,BYTE attr)
{
	WORD entrada;
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
	entrada=ProcurarEntrada(nome,drive);
	if(entrada<Drive[drive].NEntradas)
	{
		if(Drive[drive].Entrada[entrada].Attr&So_leitura)
		{
			ERRO=EACCES;
			return;
		}
		LibertaClusters(drive,Drive[drive].Entrada[entrada].FCluster);
	}
	else
	{
		entrada=ProcurarEntrada("",drive);
		if(entrada>=Drive[drive].NEntradas)
		{
			ERRO=EFAULT;
			return;
		}
		SetNomeEntrada(Drive[drive].Entrada[entrada].Nome,nome);
	}
	Drive[drive].Entrada[entrada].FCluster=0;
	Drive[drive].Entrada[entrada].Tamanho=0;
	Drive[drive].Entrada[entrada].Attr=attr;
	Drive[drive].Entrada[entrada].Data=Ler_Packed_Data();
	Drive[drive].Entrada[entrada].Hora=Ler_Packed_Hora();
}

void LibertaClusters(BYTE drive,WORD cluster)
{
	WORD pcluster;
	if((Drive[drive].Montada==FALSE)||(drive>=MAXDRIVES))
		return;
	if(cluster>Drive[drive].NClusters)
		return;
	while((cluster>0)&&(cluster<__EOF))
	{
		pcluster=cluster;
		cluster=GetFat(0,cluster);
		SetFat(drive,pcluster,0);
	}
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

WORD INITFS()
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

void CLOSEFS()
{
	BYTE cont;
	for(cont=0;cont<MAXDRIVES;cont++)
		if(Drive[cont].Montada)
			DesMontarDrive(cont);
	LibertaMemoria(BootRecord);
	LibertaMemoria(BlocoControlo);
}

WORD MontarDrive(BYTE drive)
{
	WORD dirsect;
	WORD _fatsize,_dirsize;
	BYTE far *bootid;
	BYTE far *endereco;
	WORD sector;
	WORD cont;
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
	if(!(LerSectorFisico(drive,0,0,1,(char far *)BootRecord)))
	{
		ERRO=EINOUT;
		return 0;
	}
	bootid=BootRecord->Fsid;
	if((bootid[0]!='M')||(bootid[1]!='C')||(bootid[2]!='F')||
		(bootid[3]!='S')||(bootid[4]!='0')||(bootid[5]!='0'))
	{
		ERRO=EINVDRV;
		return 0;
	}
	Drive[drive].TamSector=BootRecord->Bps;
	Drive[drive].SectorCluster=BootRecord->Spc;
	Drive[drive].SectorPista=BootRecord->Spp;
	Drive[drive].Heads=BootRecord->Hn;
	Drive[drive].FATSector=BootRecord->Rs+BootRecord->Ssn;
	Drive[drive].DirSector=Drive[drive].FATSector+BootRecord->Spf;
	dirsect=((BootRecord->Rde)*32)/Drive[drive].TamSector;
	if(((BootRecord->Rde)*32)%Drive[drive].TamSector) dirsect++;
	Drive[drive].DadosSector=Drive[drive].DirSector+dirsect;
	Drive[drive].NEntradas=BootRecord->Rde;
	Drive[drive].NClusters=(BootRecord->Lsn-Drive[drive].DadosSector)/Drive[drive].SectorCluster;
	Drive[drive].NSectors=BootRecord->Lsn;
	_fatsize=(BootRecord->Spf)*Drive[drive].TamSector;
	_dirsize=dirsect*Drive[drive].TamSector;
	Drive[drive].FAT=(WORD far *)AlocaMemoria(_fatsize);
	Drive[drive].Entrada=(TDIR far *)AlocaMemoria(_dirsize);
	Drive[drive]._UCA=0;
	Drive[drive]._UE=0;
	Drive[drive].Montada=TRUE;
	if((!FP_SEG(Drive[drive].FAT))||(!FP_SEG(Drive[drive].Entrada)))
	{
		ERRO=EMEM;
		return 0;
	}
	endereco=(BYTE far *)Drive[drive].FAT;
	sector=Drive[drive].FATSector;
	for(cont=0;cont<_fatsize;cont++)
	{
		if(!(LerSector(drive,sector,endereco)))
		{
			Drive[drive].Montada=FALSE;
			ERRO=EINOUT;
			return 0;
		}
		sector++;
		cont+=Drive[drive].TamSector;
		endereco+=Drive[drive].TamSector;
	}
	endereco=(BYTE far *)Drive[drive].Entrada;
	sector=Drive[drive].DirSector;
	for(cont=0;cont<_dirsize;cont++)
	{
		if(!(LerSector(drive,sector,endereco)))
		{
			Drive[drive].Montada=FALSE;
			ERRO=EINOUT;
			return 0;
		}
		sector++;
		cont+=Drive[drive].TamSector;
		endereco+=Drive[drive].TamSector;
	}
	Drive[drive]._fatsize=_fatsize;
	Drive[drive]._dirsize=_dirsize;
	return 1;
}

WORD DesMontarDrive(BYTE drive)
{
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
	FecharFicheirosDrive(drive);
	SyncDrive(drive);
	Drive[drive].Montada=FALSE;
	LibertaMemoria(Drive[drive].FAT);
	LibertaMemoria(Drive[drive].Entrada);
	return 0;
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

WORD GetFat(BYTE drive,WORD ParentCluster)
{
	WORD index;
	if((Drive[drive].Montada==FALSE)||(drive>=MAXDRIVES))
		return 0xFFFF;
	if((ParentCluster<1)||(ParentCluster>Drive[drive].NClusters))
	{
		return 0xFFFF;
	}
	index=ParentCluster-1;
	return Drive[drive].FAT[index];
}

void SetFat(BYTE drive,WORD ParentCluster,WORD Cluster)
{
	WORD index;
	if((Drive[drive].Montada==FALSE)||(drive>=MAXDRIVES))
	{
		ERRO=EINVDRV;
		return ;
	}
	if((ParentCluster<1)||(ParentCluster>Drive[drive].NClusters))
	{
		ERRO=EFAULT;
		return ;
	}
	index=ParentCluster-1;
	Drive[drive].FAT[index]=Cluster;
}

WORD SyncDrive(BYTE drive)
{
	BYTE far *endereco;
	WORD sector;
	WORD cont;
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
	endereco=(BYTE far *)Drive[drive].FAT;
	sector=Drive[drive].FATSector;
	for(cont=0;cont<Drive[drive]._fatsize;cont++)
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
	endereco=(BYTE far *)Drive[drive].Entrada;
	sector=Drive[drive].DirSector;
	for(cont=0;cont<Drive[drive]._dirsize;cont++)
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

WORD AlocaCluster(BYTE drive)
{
	WORD cont;
	if((Drive[drive].Montada==FALSE)||(drive>=MAXDRIVES))
	{
		ERRO=EINVDRV;
		return 0;
	}
	if(Drive[drive]._UCA>=Drive[drive].NClusters)
		Drive[drive]._UCA=0;
	for(cont=Drive[drive]._UCA+1;cont<=Drive[drive].NClusters;cont++)
	{
		if (GetFat(drive,cont)==0)
		{
			SetFat(drive,cont,__EOF);
			Drive[drive]._UCA=cont;
			return cont;
		}
	}
	for(cont=1;cont<=Drive[drive]._UCA;cont++)
	{
		if (GetFat(drive,cont)==0)
		{
			SetFat(drive,cont,__EOF);
			Drive[drive]._UCA=cont;
			return cont;
		}
	}
	return 0;
}

WORD ProcurarEntrada(BYTE far *nome,BYTE drive)
{
	WORD cont;
	if((Drive[drive].Montada==FALSE)||(drive>=MAXDRIVES))
	{
		ERRO=EINVDRV;
		return 0xFFFF;
	}
	for(cont=0;cont<Drive[drive].NEntradas;cont++)
	{
		if(NomeFicheiroCmp(Drive[drive].Entrada[cont].Nome,nome))
			return cont;
	}
	return 0xFFFF;
}

BYTE NomeFicheiroCmp(BYTE far *fnome,BYTE far *nome)
{
	BYTE nome2[20];
	GetNomeEntrada(fnome,nome2);
	if(Strcmp(nome,nome2))
		return 0;
	else
		return 1;
}

void SetNomeEntrada(BYTE far *fnome,BYTE far *nome)
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

void GetNomeEntrada(BYTE far *fnome,BYTE far *nome)
{
	WORD cont;
	for (cont=0;cont<12;cont++)
	{
		nome[cont]=fnome[cont];
	}
	nome[cont]=0;
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

WORD AbrirFicheiro(BYTE drive,BYTE far *nome,BYTE modo)
{
	WORD entrada;
	WORD bloco;
	TFCB far *enderecobloco;
	BYTE far *enderecobuffer;
	WORD buffersize;
	BYTE aberto=FALSE;
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
	entrada=ProcurarEntrada(nome,drive);
	if(entrada<Drive[drive].NEntradas)
		aberto=VerificarAberto(&(Drive[drive].Entrada[entrada]));
	if(((modo&CRIAR_F)||(modo&ESCREVER))&&(aberto))
	{
		ERRO=EACCES;
		return 0xFFFF;
	}
	if(modo&CRIAR_F)
	{
		CriarFicheiro(drive,nome,Sem_atributos);
		if(ERRO)
			return 0xFFFF;
	}
	entrada=ProcurarEntrada(nome,drive);
	if(entrada>=Drive[drive].NEntradas)
	{
		ERRO=ENOFILE;
		return 0xFFFF;
	}
	if((modo&ESCREVER)&&(Drive[drive].Entrada[entrada].Attr&So_leitura))
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
	buffersize=Drive[drive].TamSector*Drive[drive].SectorCluster;
	enderecobuffer=(BYTE far *)AlocaMemoria(buffersize);
	if(FP_SEG(enderecobuffer)==0)
	{
		LibertaMemoria(enderecobloco);
		ERRO=EMEM;
		return 0xFFFF;
	}
	BlocoControlo[bloco]=enderecobloco;
	BlocoControlo[bloco]->Entrada=&(Drive[drive].Entrada[entrada]);
	BlocoControlo[bloco]->Tamanho=BlocoControlo[bloco]->Entrada->Tamanho;
	BlocoControlo[bloco]->Modo=modo;
	BlocoControlo[bloco]->Fpos=0;
	BlocoControlo[bloco]->Drive=drive;
	BlocoControlo[bloco]->Buffer=enderecobuffer;
	BlocoControlo[bloco]->FCluster=BlocoControlo[bloco]->Entrada->FCluster;
	BlocoControlo[bloco]->Cluster=0;
	BlocoControlo[bloco]->Procid=CurProcess;
	return bloco;
}

BYTE VerificarAberto(TDIR far *entrada)
{
	WORD cont;
	for(cont=0;cont<FILES;cont++)
	{
		if(FP_SEG(BlocoControlo[cont]))
		{
			if(BlocoControlo[cont]->Entrada==entrada)
				 return TRUE;
		}
	}
	return FALSE;
}

void FecharFicheiro(WORD bloco)
{
	ERRO=FALSE;
	if((FP_SEG(BlocoControlo[bloco])==0)||(bloco>=FILES))
	{
		ERRO=EBADF;
		return;
	}
	if(BlocoControlo[bloco]->Modo&ESCREVER)
	{
		FlushFicheiro(bloco);
		BlocoControlo[bloco]->Entrada->Tamanho=BlocoControlo[bloco]->Tamanho;
		BlocoControlo[bloco]->Entrada->FCluster=BlocoControlo[bloco]->FCluster;
		BlocoControlo[bloco]->Entrada->Data=Ler_Packed_Data();
		BlocoControlo[bloco]->Entrada->Hora=Ler_Packed_Hora();
	}
	LibertaMemoria(BlocoControlo[bloco]->Buffer);
	LibertaMemoria(BlocoControlo[bloco]);
	BlocoControlo[bloco]=(TFCB far *)MK_FP(0,0);
}

void FlushFicheiro(WORD bloco)
{
	WORD deslocamento;
	WORD tamcluster;
	BYTE drive;
	if((FP_SEG(BlocoControlo[bloco])==0)||(bloco>=FILES))
	{
		ERRO=EBADF;
		return;
	}
	drive=BlocoControlo[bloco]->Drive;
	tamcluster=Drive[drive].TamSector*Drive[drive].SectorCluster;
	deslocamento=BlocoControlo[bloco]->Fpos%tamcluster;
	if(deslocamento)
	{
		EscreverCluster(drive,BlocoControlo[bloco]->Cluster,BlocoControlo[bloco]->Buffer);
		if(ERRO)
		{
			return;
		}
	}
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

WORD EscreverCaracter(WORD bloco,BYTE caracter)
{
	WORD deslocamento;
	WORD tamcluster;
	WORD cluster_anterior;
	BYTE drive;
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
	tamcluster=Drive[drive].TamSector*Drive[drive].SectorCluster;
	deslocamento=BlocoControlo[bloco]->Fpos%tamcluster;
	if(deslocamento==0)
	{
		cluster_anterior=BlocoControlo[bloco]->Cluster;
		if(BlocoControlo[bloco]->FCluster==0)
		{
			BlocoControlo[bloco]->FCluster=AlocaCluster(drive);
			BlocoControlo[bloco]->Cluster=BlocoControlo[bloco]->FCluster;
		}
		else
		{
			if(BlocoControlo[bloco]->Cluster==0)
				  BlocoControlo[bloco]->Cluster=BlocoControlo[bloco]->FCluster;
			else
				  BlocoControlo[bloco]->Cluster=GetFat(drive,BlocoControlo[bloco]->Cluster);
			if(BlocoControlo[bloco]->Cluster==0)
			{
				BlocoControlo[bloco]->Cluster=cluster_anterior;
				ERRO=EFAULT;
				return 0;
			}
			if(BlocoControlo[bloco]->Cluster>=__EOF)
			{
				BlocoControlo[bloco]->Cluster=AlocaCluster(drive);
				SetFat(drive,cluster_anterior,BlocoControlo[bloco]->Cluster);
			}
			else
			{
				LerCluster(drive,BlocoControlo[bloco]->Cluster,BlocoControlo[bloco]->Buffer);
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
		EscreverCluster(drive,BlocoControlo[bloco]->Cluster,BlocoControlo[bloco]->Buffer);
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


void EscreverCluster(BYTE drive,WORD cluster,BYTE far *buffer)
{
	WORD cont;
	WORD sector;
	ERRO=FALSE;
	if((Drive[drive].Montada==FALSE)||(drive>=MAXDRIVES))
	{
		ERRO=EINVDRV;
		return;
	}
	if((cluster<1)||(cluster>Drive[drive].NClusters))
	{
		ERRO=EFAULT;
		return;
	}
	sector=Drive[drive].DadosSector+(cluster-1)*Drive[drive].SectorCluster;
	for(cont=0;cont<Drive[drive].SectorCluster;cont++)
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

void LerCluster(BYTE drive,WORD cluster,BYTE far *buffer)
{
	WORD cont;
	WORD sector;
	ERRO=FALSE;
	if((Drive[drive].Montada==FALSE)||(drive>=MAXDRIVES))
	{
		ERRO=EINVDRV;
		return;
	}
	if((cluster<1)||(cluster>Drive[drive].NClusters))
	{
		ERRO=EFAULT;
		return;
	}
	sector=Drive[drive].DadosSector+(cluster-1)*Drive[drive].SectorCluster;
	for(cont=0;cont<Drive[drive].SectorCluster;cont++)
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

WORD LerCaracter(WORD bloco)
{
	WORD deslocamento;
	WORD cluster_anterior;
	WORD tamcluster;
	BYTE caracter;
	BYTE drive;
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
	tamcluster=Drive[drive].TamSector*Drive[drive].SectorCluster;
	deslocamento=BlocoControlo[bloco]->Fpos%tamcluster;
	if(deslocamento==0)
	{
		cluster_anterior=BlocoControlo[bloco]->Cluster;
		if(BlocoControlo[bloco]->Cluster==0)
			BlocoControlo[bloco]->Cluster=BlocoControlo[bloco]->FCluster;
		else
			BlocoControlo[bloco]->Cluster=GetFat(drive,BlocoControlo[bloco]->Cluster);
		if(!((BlocoControlo[bloco]->Cluster>0)&&(BlocoControlo[bloco]->Cluster<__EOF)))
		{
			BlocoControlo[bloco]->Cluster=cluster_anterior;
			ERRO=EFAULT;
			return 0xFFFF;
		}
		LerCluster(drive,BlocoControlo[bloco]->Cluster,BlocoControlo[bloco]->Buffer);
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


WORD LerFicheiro(WORD bloco, char far *ptr, WORD len)
{
	WORD deslocamento;
	WORD cluster_anterior;
	WORD tamcluster;
	BYTE caracter;
	BYTE drive;
	WORD count=0;
	WORD ksize=0;
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
	tamcluster=Drive[drive].TamSector*Drive[drive].SectorCluster;
	while(len) {
	  deslocamento=BlocoControlo[bloco]->Fpos%tamcluster;
	  if(deslocamento==0)
	  {
		  cluster_anterior=BlocoControlo[bloco]->Cluster;
		  if(BlocoControlo[bloco]->Cluster==0)
			  BlocoControlo[bloco]->Cluster=BlocoControlo[bloco]->FCluster;
		  else
			  BlocoControlo[bloco]->Cluster=GetFat(drive,BlocoControlo[bloco]->Cluster);
		  if(!((BlocoControlo[bloco]->Cluster>0)&&(BlocoControlo[bloco]->Cluster<__EOF)))
		  {
			  BlocoControlo[bloco]->Cluster=cluster_anterior;
			  ERRO=EFAULT;
			  return 0;
		  }
		  LerCluster(drive,BlocoControlo[bloco]->Cluster,BlocoControlo[bloco]->Buffer);
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


void PosicionarFicheiro(WORD bloco,long deslocamento,BYTE modo)
{
	DWORD n_pos;
	WORD n_cluster;
	WORD nciclos;
	WORD cont;
	BYTE drive;
	WORD tamcluster;
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
	tamcluster=Drive[drive].TamSector*Drive[drive].SectorCluster;
	nciclos=(n_pos-1)/tamcluster;
	n_cluster=BlocoControlo[bloco]->FCluster;
	if(n_cluster>Drive[drive].NClusters)
	{
		ERRO=EFAULT;
		return;
	}
	for(cont=0;cont<nciclos;cont++)
	{
		n_cluster=GetFat(drive,n_cluster);
		if((n_cluster<1)||(n_cluster>Drive[drive].NClusters))
		{
			ERRO=EFAULT;
			return;
		}
	}
	if(n_pos%tamcluster)
	{
		LerCluster(drive,n_cluster,BlocoControlo[bloco]->Buffer);
		if(ERRO)
			return;
	}
	BlocoControlo[bloco]->Cluster=n_cluster;
	BlocoControlo[bloco]->Fpos=n_pos;
}

void TruncarFicheiro(WORD bloco)
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
		LibertaClusters(drive,BlocoControlo[bloco]->FCluster);
		BlocoControlo[bloco]->FCluster=0;
	}
	else
	{
		LibertaClusters(drive,GetFat(drive,BlocoControlo[bloco]->Cluster));
		SetFat(drive,BlocoControlo[bloco]->Cluster,__EOF);
	}
}

WORD FreeClusters(BYTE drive)
{
	WORD cont;
	WORD free=0;
	if(drive>=MAXDRIVES)
		return 0;
	if(Drive[drive].Montada==FALSE)
		return 0;
	for(cont=1;cont<=Drive[drive].NClusters;cont++)
	{
		if(GetFat(drive,cont)==0)
			free++;
	}
	return free;
}

DWORD FreeSpace(BYTE drive)
{
	DWORD frees;
	WORD tamcluster;
	if(drive>=MAXDRIVES)
		return 0;
	if(Drive[drive].Montada==FALSE)
		return 0;
	tamcluster=Drive[drive].TamSector*Drive[drive].SectorCluster;
	frees=longmul2((DWORD)FreeClusters(drive),tamcluster);
	return frees;
}

DWORD DiskSpace(BYTE drive)
{
	WORD tamcluster;
	if(drive>=MAXDRIVES)
		return 0;
	if(Drive[drive].Montada==FALSE)
		return 0;
	tamcluster=Drive[drive].TamSector*Drive[drive].SectorCluster;
	return longmul2(Drive[drive].NClusters,tamcluster);
}

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

BYTE DirProcura(BYTE drive,TDIR_RECORD far *rec,BYTE first)
{
	WORD entrada;
	ERRO=FALSE;
	if((Drive[drive].Montada==FALSE)||(drive>=MAXDRIVES))
	{
		ERRO=EINVDRV;
		return 0;
	}
	entrada=GetEntrada(drive,first);
	if (entrada<Drive[drive].NEntradas)
	{
		GetNomeEntrada(Drive[drive].Entrada[entrada].Nome,rec->nome);
		rec->Data=UnPacked_Data(Drive[drive].Entrada[entrada].Data);
		rec->Hora=UnPacked_Hora(Drive[drive].Entrada[entrada].Hora);
		rec->Attr=Drive[drive].Entrada[entrada].Attr;
		rec->Tamanho=Drive[drive].Entrada[entrada].Tamanho;
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
