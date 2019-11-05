#include "fserror.h"
#include "fsext2.h"
#include "mcosmem.h"

int fsext2iGetInode(BYTE drive, DWORD inode, FSEXT2_INODE far *p);
WORD fsext2OpenByInode(BYTE drive, DWORD inode, BYTE modo);
int fsext2LoadCluster(WORD bloco, DWORD *o, DWORD *r);
BYTE fsext2iReadBlock(BYTE drive, DWORD block, BYTE far *buffer);


FSEXT2_DATA far * fsext2DriveData(BYTE drive)
{
	return (FSEXT2_DATA far *)Drive[drive].fs_data;
}

WORD fsext2MontarDrive(BYTE drive)
{	
	BYTE far *tmp;
	WORD sector;
	WORD cont;
	FSEXT2_DATA far *dd;

	ERRO=FALSE;

	tmp = (BYTE far *)AlocaMemoria(1024);
	if (!tmp) {
		ERRO=EMEM;
		goto _abort_mount;
	}

	if(!(LerSector(drive,2,(char far *)tmp)))
	{
		LibertaMemoria(tmp);
		ERRO=EINOUT;
		return 0;
	}

	kprintf("Super Block read!\r\n");
	Drive[drive].ifsdriver = fsext2GetDriver();
	Drive[drive].fs_data = (FSEXT2_DATA far *)AlocaMemoria(sizeof(FSEXT2_DATA));
		
	dd = fsext2DriveData(drive);
	if (!FP_SEG(dd)) {
		LibertaMemoria(tmp);
		ERRO=EMEM;
		return 0;
	}

	/* Set all pointer to NULL before any allocations */
	dd->work_block = NULL;

	dd->s = (FSEXT2_SUPER far *) tmp;
	if (dd->s->magic != FSEXT2_SUPER_MAGIC) {
		ERRO = EINVFS;
		goto _abort_mount;
	}

	kprintf("Signature: %x\r\n", dd->s->magic);
	cont = dd->s->log_block_size;
	dd->bsb = 1024 << cont;
	dd->bss = dd->bsb / 512;

	kprintf("Block Size: %u Bytes %u Sectores\r\n", dd->bsb, dd->bss);
	kprintf("First Data Block: %x\r\n", (WORD)dd->s->first_data_block);

	cont = dd->s->first_data_block;
	sector = (cont+1)*dd->bss;
	kprintf("Block Group Table Sector: %u\r\n", sector);

	tmp = tmp + 512;
	
	if(!(LerSector(drive,sector,(char far *)tmp)))
	{
		ERRO=EINOUT;
		goto _abort_mount;
	}

	dd->g = (FSEXT2_GROUP far *) tmp;

	kprintf("Block Bitmap: %u\r\n", (WORD)dd->g->block_bitmap);	
	kprintf("Inode Bitmap: %u\r\n", (WORD)dd->g->inode_bitmap);	
	kprintf("Inode Table:  %u\r\n", (WORD)dd->g->inode_table);	

	kprintf("Number of Blocks: %u\r\n", (WORD)dd->s->blocks_count);
        kprintf("Number of Inodes: %u\r\n", (WORD)dd->s->inodes_count);	
	
	dd->work_block = (BYTE far *)AlocaMemoria(dd->bsb);

	dd->work_block_number = 0L;
	dd->dirty = 0;

	if (!dd->work_block) {
		ERRO=EMEM;
		goto _abort_mount;
	}

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
	LibertaMemoria(dd->work_block);
	LibertaMemoria(dd);
	return 0;
}

WORD fsext2DesMontarDrive(BYTE drive)
{
	FSEXT2_DATA far *dd;

	if (!Drive[drive].Montada)
		return 1;

	if (drive < MAXDRIVES)
		Drive[drive].Montada=FALSE;
	
	dd = fsext2DriveData(drive);
	if (!dd)
		return 1;

	LibertaMemoria(dd->s);
	LibertaMemoria(dd->work_block);
	LibertaMemoria(dd);
	
	return 1;
}

WORD fsext2SyncDrive(BYTE drive)
{
	FSEXT2_DATA far *dd;

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
	WORD ch;
	WORD b=fsext2OpenByInode(drive,2, 0);
	kprintf("Open File: %u\r\n", b);

	if (b==0xFFFF)
		return 0;
	
	while( (ch=fsext2LerCaracter(b))<0x100) {
		kprintf("%c", ch>31 && ch<128 ? ch : '.');
	}
	kprintf("\r\n");
	fsext2FecharFicheiro(b);
	

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
	ERRO=FALSE;
	if((FP_SEG(BlocoControlo[bloco])==0)||(bloco>=FILES))
	{
		ERRO=EBADF;
		return;
	}
	/*
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
	*/

	// TODO free fsd buffers
	LibertaMemoria(BlocoControlo[bloco]->fsd);
	LibertaMemoria(BlocoControlo[bloco]->Buffer);
	LibertaMemoria(BlocoControlo[bloco]);
	BlocoControlo[bloco]=(TFCB far *)MK_FP(0,0);
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

int fsext2LoadCluster(WORD bloco, DWORD *o, DWORD *r)
{
	FSEXT2_DATA far *dd;
	WORD drive=0;
	DWORD fpos;
	DWORD cluster;

	ERRO=FALSE;
	if((FP_SEG(BlocoControlo[bloco])==0)||(bloco>=FILES))
	{
		ERRO=EBADF;
		return 0;
	}

	drive = BlocoControlo[bloco]->Drive;

	dd = fsext2DriveData(drive);
	if (!dd)
		return 0;

	fpos = BlocoControlo[bloco]->Fpos;
	cluster = fpos / dd->bsb + 1;
	*o = fpos % dd->bsb;
	*r = dd->bsb - fpos % dd->bsb;

	if (cluster == BlocoControlo[bloco]->Cluster)
		return 1;

	if (cluster > 12)
		return 0;

	{
		FSEXT2_OPEN_INODE far *on = (FSEXT2_OPEN_INODE far *)BlocoControlo[bloco]->fsd;
		DWORD block = on->inode.block[cluster-1];
		if (!block)
			return 0;

		if(!fsext2iReadBlock(drive,block, BlocoControlo[bloco]->Buffer)) {
				return 0;
		}
		BlocoControlo[bloco]->Cluster=cluster;
	}
	
	return 1;
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
	DWORD r=0;
	DWORD o=0;
	WORD ch;

	ERRO=FALSE;
	if((FP_SEG(BlocoControlo[bloco])==0)||(bloco>=FILES))
	{
		ERRO=EBADF;
		return 0xFFFF;
	}


	if (BlocoControlo[bloco]->Fpos >= BlocoControlo[bloco]->Tamanho)
		return 0xFFFF;

	if(!fsext2LoadCluster(bloco, &o, &r))
	       return 0xFFFF;	


	BlocoControlo[bloco]->Fpos++;

	ch = BlocoControlo[bloco]->Buffer[o];
	
	return ch;
}

void fsext2EscreverCluster(BYTE drive,WORD cluster,BYTE far *buffer)
{
	FSEXT2_DATA far *dd;

	(void)drive;
	(void)cluster;
	(void)buffer;
	ERRO=EUNSUP;
}

BYTE fsext2iReadBlock(BYTE drive, DWORD block, BYTE far *buffer)
{
	DWORD sector;
	FSEXT2_DATA far *dd;
	int i;
	ERRO=FALSE;

	dd = fsext2DriveData(drive);
	if (!dd) {
		ERRO=EINVDRV;
		return 0;
	}

	sector = longmul2(block, dd->bss);

	for (i=0; i<dd->bss; i++) {
		if(!LerSector(drive, sector+i, buffer)) {
			ERRO=EINOUT;
			return 0;
		}
		buffer = buffer + 512;
	}

	return 1;

}

void fsext2LerCluster(BYTE drive,WORD cluster,BYTE far *buffer)
{
	fsext2iReadBlock(drive, cluster, buffer);
}

BYTE fsext2iReadWorkBlock(BYTE drive, DWORD block)
{
	FSEXT2_DATA far *dd;	
	dd = fsext2DriveData(drive);

	if (!dd)
		return 0;

	if (dd->work_block_number==block)
		return 1;

	if (dd->dirty) {
		// write current
	}

	
	if(!fsext2iReadBlock(drive, block, dd->work_block)) {
		return 0;
	}

	dd->work_block_number=block;

	return 1;

}

int fsext2iGetInode(BYTE drive, DWORD inode, FSEXT2_INODE far *p)
{
	FSEXT2_DATA far *dd;
	DWORD block;
	WORD pos;
	FSEXT2_INODE far *n;

	if (inode==0L)
	       return 0;
	inode--;

	dd = fsext2DriveData(drive);

	block = ((WORD)inode/8)+dd->g->inode_table;
	pos = inode&0x7;

	n = (FSEXT2_INODE far *)dd->work_block;
	n+=pos;
	if(!fsext2iReadWorkBlock(drive, block)) {
		return 0;
	}

	_fmemcpy(p, n, sizeof(FSEXT2_INODE));
	kprintf("Mode %X\r\n", n->mode);
	kprintf("Size %u\r\n", (WORD)n->size);

	return 1;	
}

WORD fsext2OpenByInode(BYTE drive, DWORD inode, BYTE modo)
{
	FSEXT2_OPEN_INODE far *on=NULL;
	WORD bloco=0xFFFF;
	TFCB far *enderecobloco=NULL;
	BYTE far *enderecobuffer=NULL;
	FSEXT2_DATA far *dd=NULL;

	dd = fsext2DriveData(drive);
	if (!dd) {
		ERRO=EINVDRV;
		return 0xFFFF;
	}

	bloco=GetFreeBloco();
	if(bloco>=FILES)
	{
		ERRO=EMFILE;
		return 0xFFFF;
	}

	on = (FSEXT2_OPEN_INODE far *)AlocaMemoria(sizeof(FSEXT2_OPEN_INODE));
	if (!on) {
		ERRO=EMEM;
		return 0xFFFF;
	}

	if(!fsext2iGetInode(drive, inode, &(on->inode))) {
		ERRO=ENOFILE;
		goto _abort_open_inode;
	}
	on->inode_number = inode;
	on->cn_block_number=0;
	on->cn_block=NULL;


	enderecobloco=(TFCB far *)AlocaMemoria(sizeof(TFCB));
	if(FP_SEG(enderecobloco)==0)
	{
		ERRO=EMEM;
		goto _abort_open_inode;
	}

	enderecobuffer=(BYTE far *)AlocaMemoria(dd->bsb);
	if(FP_SEG(enderecobuffer)==0)
	{
		ERRO=EMEM;
		goto _abort_open_inode;
	}


	BlocoControlo[bloco]=enderecobloco;
	BlocoControlo[bloco]->fsd=on;
	BlocoControlo[bloco]->Inode = inode;
	BlocoControlo[bloco]->Tamanho=on->inode.size;
	BlocoControlo[bloco]->Modo=modo;
	BlocoControlo[bloco]->Fpos=0;
	BlocoControlo[bloco]->Drive=drive;
	BlocoControlo[bloco]->Buffer=enderecobuffer;
	BlocoControlo[bloco]->FCluster=on->inode.block[0];
	BlocoControlo[bloco]->Cluster=0;
	BlocoControlo[bloco]->Procid=CurProcess;
	return bloco;

_abort_open_inode:
	LibertaMemoria(enderecobuffer);
	LibertaMemoria(enderecobloco);
	LibertaMemoria(on);

	return 0xFFFF;
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
	FSEXT2_DATA far *dd = fsext2DriveData(drive);
	if (!dd)
		return 0;
	return dd->g->free_blocks;
}

DWORD fsext2FreeSpace(BYTE drive)
{
	FSEXT2_DATA far *dd = fsext2DriveData(drive);
	if (!dd)
		return 0;
	return longmul2(dd->g->free_blocks, dd->bsb);

}

DWORD fsext2DiskSpace(BYTE drive)
{
	FSEXT2_DATA far *dd = fsext2DriveData(drive);
	if (!dd)
		return 0;
	return longmul2(dd->s->blocks_count, dd->bsb);
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
