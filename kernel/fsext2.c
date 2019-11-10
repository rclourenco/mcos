#include "fserror.h"
#include "fsext2.h"
#include "mcosmem.h"

int fsext2iGetInode(BYTE drive, DWORD inode, FSEXT2_INODE far *p);
WORD fsext2OpenByInode(BYTE drive, DWORD inode, BYTE modo);
int fsext2LoadCluster(WORD bloco, DWORD far *o, DWORD far *r);
BYTE fsext2iReadBlock(BYTE drive, DWORD block, BYTE far *buffer);
BYTE fsext2iWriteBlock(BYTE drive, DWORD block, BYTE far *buffer);
DWORD fsext2iFindFile(BYTE drive, char far *name);
DWORD fsext2iClusterToBlock(BYTE drive, FSEXT2_OPEN_INODE far *on, DWORD cluster);
DWORD fsext2iMknode(BYTE drive, WORD mode);

FSEXT2_DATA far * fsext2DriveData(BYTE drive)
{
	return (FSEXT2_DATA far *)Drive[drive].fs_data;
}

DWORD AllocInode(BYTE drive)
{
	DWORD nf = 0;
	DWORD i;
	FSEXT2_DATA far *dd = fsext2DriveData(drive);
	
	if (!dd)
		return 0;

	nf = dd->s->inodes_count / 8L + (dd->s->inodes_count%8L ? 1L : 0L);
//	kprintf("NF: %lu\r\n", nf);
	for(i=0;i<nf;i++) {
		kprintf("%X ", dd->inode_bitmap[i]);
		if (dd->inode_bitmap[i]<0xFF) {
			break;
		}
	}
//	kprintf("Free At %lu\r\n", i);
	if (i==nf)
		return 0;

	nf = i;

//	kprintf("> %X\r\n", dd->inode_bitmap[nf] );

	for(i=0;i<8;i++) {
//		kprintf("%lu: %X\r\n", i, ((dd->inode_bitmap[nf]>>i) & 1));
		if (((dd->inode_bitmap[nf]>>i) & 1)==0) {
			dd->inode_bitmap[nf] |= (1<<i);
			dd->s->free_inodes_count--;
			dd->g->free_inodes--;
			return nf * 8L + i + 1;
		}
	}
	return 0;
}

WORD fsext2MontarDrive(BYTE drive)
{	
	BYTE far *tmp;
	WORD sector;
	WORD cont;
	DWORD i;
	DWORD numl;
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
	dd->work_block   = NULL;
	dd->block_bitmap = NULL;
	dd->inode_bitmap = NULL;

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

	Drive[drive].Montada=TRUE;

	kprintf("Block Bitmap Size: %lu\r\n", (DWORD)(dd->g->inode_bitmap - dd->g->block_bitmap));
	kprintf("Inode Bitmap Size: %lu\r\n", (DWORD)(dd->g->inode_table  - dd->g->inode_bitmap));

	// Load Block Bitmap
	numl = dd->g->inode_bitmap - dd->g->block_bitmap;
	dd->block_bitmap = (BYTE far *)AlocaMemoria( numl * dd->bsb);
	if (!dd->block_bitmap) {
		ERRO=EMEM;
		goto _abort_mount;
	}

	for (i=0;i<numl;i++) {
		char far *ptr = (char far *)dd->block_bitmap;
		if(!fsext2iReadBlock(drive, dd->g->block_bitmap+i, ptr)) {
			ERRO=EINOUT;
			goto _abort_mount;
		}
		ptr += dd->bsb;
	}

	// Load Inode Bitmap
	numl = dd->g->inode_table  - dd->g->inode_bitmap;
	dd->inode_bitmap = (BYTE far *)AlocaMemoria( numl * dd->bsb );
	if (!dd->inode_bitmap) {
		ERRO=EMEM;
		goto _abort_mount;
	}

	for (i=0;i<numl;i++) {
		char far *ptr = (char far *)dd->inode_bitmap;
		if(!fsext2iReadBlock(drive, dd->g->inode_bitmap+i, ptr)) {
			ERRO=EINOUT;
			goto _abort_mount;
		}
		ptr += dd->bsb;
	}


	return 1;
	
_abort_mount:
	Drive[drive].Montada=FALSE;
	LibertaMemoria(dd->block_bitmap);
	LibertaMemoria(dd->inode_bitmap);
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

	LibertaMemoria(dd->block_bitmap);
	LibertaMemoria(dd->inode_bitmap);
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

void cvtUnixTime(DWORD t, TDATA far *data, THORA far *hora)
{
	DWORD a,b,c,d,e,f;

	hora->centesimos = 0;

	hora->segundos = t % 60;
	t /= 60;
	hora->minutos = t % 60;
	t /= 60;
	hora->hora = t % 24;
	t /= 24;

	a = (t*4+102032) / 146097+15;
	b = t+2442113+a - (a/4);
	c = (20*b-2442) / 7305;
	d = b - 365*c - (c/4);
	e = d * 1000 / 30601;
	f = d - e * 30 - e * 601 / 1000;

	if (e <= 13) {
		c -= 4716;
		e -= 1;
	}
	else {
		c -= 4715;
		e -= 13;
	}

	data->ano = c;
	data->mes = e;
	data->dia = f;

}

struct _dirll {
	DWORD inode;
	WORD rec_len;
	BYTE name_len;
	BYTE file_type;
} dirll;

BYTE dirfname[256];
FSEXT2_INODE stat_inode;
WORD cwd=0xFFFF;

BYTE fsext2DirProcura(BYTE drive, char far *folder, TDIR_RECORD far *rec,BYTE first)
{
	WORD ch;
	FSEXT2_OPEN_INODE far *on;

	if (first) {
		if (folder && folder[0]) {
			if (cwd!=0xFFFF)
				fsext2FecharFicheiro(cwd);
			cwd = fsext2AbrirFicheiro(drive, folder, 0);
		} else if(cwd==0xFFFF) {
			cwd=fsext2OpenByInode(drive,2, 0);
		} else {
			fsext2PosicionarFicheiro(cwd, 0, SF_INICIO);
		}
	}

	if (cwd==0xFFFF)
		return 0;

	on = (FSEXT2_OPEN_INODE far *)BlocoControlo[cwd]->fsd;
	if ((on->inode.mode & 0xF000) != 0x4000) {
		return 0;
	}

	if( fsext2LerFicheiro(cwd, (char far *)&dirll, sizeof(dirll))==sizeof(dirll) ) {
		WORD rest;
		if(fsext2LerFicheiro(cwd, (char far *)dirfname, dirll.name_len)!=dirll.name_len) {
			return 0;
		}

		dirfname[dirll.name_len]='\0';
		//kprintf("%u %s\r\n", (WORD)dirll.inode, dirfname);
		rest = dirll.rec_len - dirll.name_len - sizeof(dirll);

		_fmemcpy(rec->nome, dirfname, 13);

		rec->Tamanho = stat_inode.size;
		rec->Attr = 0;

		rec->Hora.minutos = 0;
		rec->Hora.hora = 0;
		rec->Hora.centesimos = 0;
		rec->Hora.segundos = 0;

		rec->Data.ano = 0;
		rec->Data.mes = 0;
		rec->Data.dia = 0;

		if(fsext2iGetInode(drive, dirll.inode, &stat_inode)) {
			//kprintf("%u %s %u Bytes %X\r\n", (WORD)dirll.inode, dirfname, (WORD) stat_inode.size, (WORD)stat_inode.mode);
			rec->Tamanho = stat_inode.size;
			rec->Attr = 0;
			cvtUnixTime(stat_inode.mtime, &rec->Data, &rec->Hora);
		}

		fsext2PosicionarFicheiro(cwd, rest, SF_CURRENTE);
		return 1;
	}
//	kprintf("\r\n");

	//fsext2FecharFicheiro(b);

	(void)rec;
	(void)first;
	ERRO=EUNSUP;
	return 0;
}

DWORD _fsext2iFindFile(BYTE drive, DWORD inode, char far *name)
{
	WORD b = fsext2OpenByInode(drive, inode, 0);

	DWORD i_n = 0;

	kprintf("Finding %S at %X\r\n", name, b);
	if (b==0xFFFF)
		return 0;
	
	while( fsext2LerFicheiro(b, (char far *)&dirll, sizeof(dirll))==sizeof(dirll) ) {
		WORD rest;
		int i=0;
		if(fsext2LerFicheiro(b, (char far *)dirfname, dirll.name_len)!=dirll.name_len) {
			break;
		}

		dirfname[dirll.name_len]='\0';
		while(name[i]==dirfname[i]) {
			if (name[i]=='\0') {
				break;
			}
			i++;
		}
		if (i>0 && name[i]=='\0') {
		       i_n = dirll.inode;
		       break;
	      	}	       

		rest = dirll.rec_len - dirll.name_len - sizeof(dirll);

		if (rest > dirll.rec_len)
			break;
		fsext2PosicionarFicheiro(b, rest, SF_CURRENTE);
	}

	fsext2FecharFicheiro(b);

	return i_n;

}

char far *trindex(char far *text, char c)
{
	char far *tmp = text;
	while(*tmp)
		tmp++;
	while(*tmp != c && tmp != text)
		tmp--;

	return *tmp == c ? tmp : NULL;
}


char far *tindex(char far *text, char c)
{
	while(*text && *text!=c)
		text++;
	return *text == c ? text : NULL;
}

DWORD fsext2iFindFile(BYTE drive, char far *name)
{
	DWORD start_inode = 2;
	char far *e;
	if (name[0]!='/') {
		
	}
	else {
		if (name[1]=='\0')
			return start_inode;
		name++;
	}

	do {
		e = tindex(name, '/');
		if (e) e[0] = '\0';

		start_inode = _fsext2iFindFile(drive, start_inode, name);
		if (!start_inode)
			return start_inode;

		kprintf("=> %S %u\r\n", name, (WORD)start_inode);
		if (e) {
			e[0] = '/';
			name = e + 1;
		}
	} while(e);

	return start_inode;
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
	FSEXT2_OPEN_INODE far *on;
	ERRO=FALSE;
	if((FP_SEG(BlocoControlo[bloco])==0)||(bloco>=FILES))
	{
		ERRO=EBADF;
		return;
	}
	
	if (BlocoControlo[bloco]->Modo&ESCREVER)
	{
		//TDIR far *b_ent;
		fsext2FlushFicheiro(bloco);

		/*
		b_ent = fsnBlocoEntrada(BlocoControlo[bloco]);

		b_ent->Tamanho=BlocoControlo[bloco]->Tamanho;
		b_ent->FCluster=BlocoControlo[bloco]->FCluster;
		b_ent->Data=Ler_Packed_Data();
		b_ent->Hora=Ler_Packed_Hora();
		*/
	}

	on = (FSEXT2_OPEN_INODE far *)BlocoControlo[bloco]->fsd;
	if (on && on->cn_block) {
		LibertaMemoria(on->cn_block);
	}
	// TODO free fsd buffers
	LibertaMemoria(BlocoControlo[bloco]->fsd);
	LibertaMemoria(BlocoControlo[bloco]->Buffer);
	LibertaMemoria(BlocoControlo[bloco]);
	BlocoControlo[bloco]=(TFCB far *)MK_FP(0,0);
}

void fsext2FlushFicheiro(WORD bloco)
{
	FSEXT2_DATA far *dd;
	FSEXT2_OPEN_INODE far *on;
	BYTE drive;
	ERRO=FALSE;
	if((FP_SEG(BlocoControlo[bloco])==0)||(bloco>=FILES))
	{
		ERRO=EBADF;
		return;
	}

	drive = BlocoControlo[bloco]->Drive;
	dd = fsext2DriveData(drive);
	if (!dd)
		return;

	on = (FSEXT2_OPEN_INODE far *)BlocoControlo[bloco]->fsd;
	
	if (BlocoControlo[bloco]->dirty && BlocoControlo[bloco]->Cluster) {
		DWORD dirty_block = fsext2iClusterToBlock(drive, on, BlocoControlo[bloco]->Cluster);
		if (dirty_block>0 && dirty_block < 0xFFFFFFFFL) {
			if (!fsext2iWriteBlock(drive, dirty_block, BlocoControlo[bloco]->Buffer)) {
				ERRO=EINOUT;
				return;
			}
		}
		BlocoControlo[bloco]->dirty = 0;
	}

	// Write Inode and pointer blocks
	// Sync
}

BYTE fsext2GetAttr(BYTE drive,BYTE far *nome)
{
	(void)drive;
	(void)nome;
	ERRO=EUNSUP;
	return 0;
}

int fsext2iLink(BYTE drive, DWORD i_n, BYTE far *path)
{
	WORD b;
	char far *p;
	char far *nome;
	WORD nl, pad, padb;
	BYTE ok=0;
	DWORD posb;

	p = trindex(path, '/');
	if (p)	
		*p = '\0';

	nome = p ? p+1: path;

	nl=Strlenx(nome);
	if (nl==0)
		return 0;

	pad = 4 - nl%4;

	kprintf(">> Name Len %u Pad %u Store at: %S name %S\r\n", nl, pad, p ? path : "/", nome);

	b=fsext2AbrirFicheiro(drive, p ? path : "/", ESCREVER|EXPANDIR);
	kprintf("Opened at %u\r\n", b);
	if (b>=FILES||ERRO)
		return 0;


	posb = fsbaseFicheiro_Pos(b);
	while( fsext2LerFicheiro(b, (char far *)&dirll, sizeof(dirll))==sizeof(dirll) ) {
		WORD rest;

	//	int i=0;
		if(fsext2LerFicheiro(b, (char far *)dirfname, dirll.name_len)!=dirll.name_len) {
			break;
		}

		/*
		dirfname[dirll.name_len]='\0';
		while(name[i]==dirfname[i]) {
			if (name[i]=='\0') {
				break;
			}
			i++;
		}
		if (i>0 && name[i]=='\0') {
		       i_n = dirll.inode;
		       break;
	      	}
 		*/		

		rest = dirll.rec_len - dirll.name_len - sizeof(dirll);
		padb = 4 - dirll.name_len % 4;
		if (rest > nl+sizeof(dirll) + pad+padb) {
			ok = 1;
		}
		kprintf("Rest %u Inode %lu Loc %lu, Rec Len %u\r\n", rest, dirll.inode, posb, dirll.rec_len);
		if (ok)
			break;

		if (rest > dirll.rec_len)
			break;

		fsext2PosicionarFicheiro(b, rest, SF_CURRENTE);
		posb = fsbaseFicheiro_Pos(b);
	}

	if (ok) {
		WORD offset = dirll.name_len + padb;
		WORD fix_rec_len = sizeof(dirll) + dirll.name_len + padb;
		WORD new_rec_len = dirll.rec_len - fix_rec_len;
		fsext2PosicionarFicheiro(b, posb, SF_INICIO);
		dirll.rec_len = fix_rec_len;
		if (fsext2EscreverFicheiro(b, (char far *)&dirll, sizeof(dirll))!=sizeof(dirll)) {
			ERRO = EFAULT;
			return 0;
		}
		fsext2PosicionarFicheiro(b, offset, SF_CURRENTE);
		dirll.rec_len = new_rec_len;
		dirll.inode = i_n;
		dirll.name_len = nl;
		if (fsext2EscreverFicheiro(b, (char far *)&dirll, sizeof(dirll))!=sizeof(dirll)) {
			ERRO = EFAULT;
			return 0;
		}

		if (fsext2EscreverFicheiro(b, nome, nl)!=nl) {
			ERRO = EFAULT;
			return 0;
		}
	}

	posb = fsbaseFicheiro_Pos(b);
	kprintf("After Changes: %lu\r\n", posb);

	fsext2FecharFicheiro(b);
	return ok;
}

WORD fsext2AbrirFicheiro(BYTE drive,BYTE far *nome,BYTE modo)
{
	BYTE cf = 0;
	DWORD i_n;
	WORD handle;

      	ERRO=FALSE; 

	i_n = fsext2iFindFile(drive, nome);
	cf |= (i_n) ? 1 : 0;
	cf |= (modo&CRIAR_F) ? 2 : 0;
	cf |= (modo&ESCREVER) ? 4 : 0;

	switch (cf) {
	case 0: 
	case 4:
		ERRO=ENOFILE; return 0xFFFF;
	case 1: return fsext2OpenByInode(drive, i_n, 0);
	case 5: return fsext2OpenByInode(drive, i_n, modo);
	case 6:
	case 2:
		ERRO=FALSE;
		i_n = fsext2iMknode(drive, 0); 
		if (!i_n) {
			ERRO=EFAULT;
			return 0xFFFF;
		}	
		fsext2iLink(drive, i_n, nome);
		kprintf("Inode %lu\r\n", i_n);
		handle = fsext2OpenByInode(drive, i_n, modo);
		kprintf("Handle %lu\r\n", handle);
		return handle;
	case 7:
	case 3: // truncate(drive, i_n);
		return fsext2OpenByInode(drive, i_n, modo);

	}

	ERRO=EUNSUP;
	return 0xFFFF;
}

DWORD fsext2iClusterToBlock(BYTE drive, FSEXT2_OPEN_INODE far *on, DWORD cluster)
{
	FSEXT2_DATA far *dd;
	DWORD block = 0xFFFFFFFFL;

	dd = fsext2DriveData(drive);
	if (!dd)
		return block; // invalid

	if (cluster < 13) {
		block = on->inode.block[cluster-1];
	} else if (cluster < 269 ) {
		if (!on->inode.block[12])
			return block; // invalid

		if (on->cn_block==NULL) {
			on->cn_block = (DWORD far *)AlocaMemoria(dd->bsb);
			if (on->cn_block==NULL)
				return block; // invalid
		}

		if (on->cn_block_number != on->inode.block[12]) {
			//kprintf("Block %X\r\n", (WORD)on->inode.block[12]);
			if(!fsext2iReadBlock(drive, on->inode.block[12], (char far *)on->cn_block)) {
				return block; // invalid
			}
			on->cn_block_number = on->inode.block[12];
			//kprintf("File Block 13 %X\r\n", (WORD)on->cn_block[0]);
		}

		block = on->cn_block[cluster-13];
	}
	
	return block;
}

int fsext2LoadCluster(WORD bloco, DWORD far *o, DWORD far *r)
{
	FSEXT2_DATA far *dd;
	WORD drive=0;
	DWORD fpos;
	DWORD cluster;
	DWORD block;
	FSEXT2_OPEN_INODE far *on; 

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

	on = (FSEXT2_OPEN_INODE far *)BlocoControlo[bloco]->fsd;

	if (BlocoControlo[bloco]->dirty && BlocoControlo[bloco]->Cluster) {
		DWORD dirty_block = fsext2iClusterToBlock(drive, on, BlocoControlo[bloco]->Cluster);
		if (dirty_block>0 && dirty_block < 0xFFFFFFFFL) {
			if (!fsext2iWriteBlock(drive, dirty_block, BlocoControlo[bloco]->Buffer)) {
				return 0;
			}
		}
		BlocoControlo[bloco]->dirty = 0;
	}
	
	block = fsext2iClusterToBlock(drive, on, cluster);
	if (block==0xFFFFFFFFL)
		return 0;

	if (!block) {
		/*
		if (write) {
			block = alloc_block();
		}
		*/

		// sparse file, set buffer to zeroes
		_fmemset(BlocoControlo[bloco]->Buffer, 0, dd->bsb);
		BlocoControlo[bloco]->Cluster=cluster;
		return 1;
	}

	if(!fsext2iReadBlock(drive,block, BlocoControlo[bloco]->Buffer)) {
		return 0;
	}
	BlocoControlo[bloco]->Cluster=cluster;
	
	return 1;
}

WORD fsext2EscreverCaracter(WORD bloco,BYTE caracter)
{
	DWORD r=0;
	DWORD o=0;

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

	if(!fsext2LoadCluster(bloco, &o, &r)) {
		ERRO=EFAULT;
		return 0;	
	}

	BlocoControlo[bloco]->Fpos++;

	BlocoControlo[bloco]->dirty=1;
	BlocoControlo[bloco]->Buffer[o]=caracter;
	
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
		return __EOF;

	if(!fsext2LoadCluster(bloco, &o, &r)) {
		ERRO=EFAULT;
		return 0xFFFF;	
	}

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

BYTE fsext2iWriteBlock(BYTE drive, DWORD block, BYTE far *buffer)
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
		if(!EscreverSector(drive, sector+i, buffer)) {
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

	return 1;	
}

DWORD fsext2iMknode(BYTE drive, WORD mode)
{
	DWORD inode; 
	FSEXT2_DATA far *dd;
	DWORD block;
	WORD pos;
	FSEXT2_INODE far *n;

	dd = fsext2DriveData(drive);
	if (!dd)
		return 0;

	inode = AllocInode(drive); 
	if (inode==0L)
	       return 0;
	inode--;

	block = ((WORD)inode/8)+dd->g->inode_table;
	pos = inode&0x7;

	n = (FSEXT2_INODE far *)dd->work_block;
	n+=pos;

	if(!fsext2iReadWorkBlock(drive, block)) {
		return 0;
	}

	dd->dirty = 1;
	if (mode == 0) {
		mode = 0x8000 | 0666;
	}
	n->mode = mode;
	return inode + 1;
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
	BlocoControlo[bloco]->dirty=0;
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
	DWORD n_pos=0;

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

	BlocoControlo[bloco]->Fpos = n_pos;
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
	DWORD r=0;
	DWORD o=0;
	WORD cnt = 0;
	WORD ch;

	ERRO=FALSE;
	if((FP_SEG(BlocoControlo[bloco])==0)||(bloco>=FILES))
	{
		ERRO=EBADF;
		return 0;
	}

	if (BlocoControlo[bloco]->Fpos >= BlocoControlo[bloco]->Tamanho)
		return 0;

	if (BlocoControlo[bloco]->Fpos + len > BlocoControlo[bloco]->Tamanho)
		len = BlocoControlo[bloco]->Tamanho - BlocoControlo[bloco]->Fpos;

	while(len) {
		if(!fsext2LoadCluster(bloco, &o, &r)) {
	       		ERRO=EFAULT;
			return cnt;
		}

		if (r==0)
			return 0;
		if (r > len) {
			r=len;
		}
		_fmemcpy(ptr, BlocoControlo[bloco]->Buffer+o, r);
		len -= r;
		ptr += r;
		cnt += r;
		BlocoControlo[bloco]->Fpos += r;
	}

	return cnt;
}

WORD fsext2EscreverFicheiro(WORD bloco, char far *ptr, WORD len)
{
	DWORD r=0;
	DWORD o=0;
	WORD cnt=0;

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

        if(BlocoControlo[bloco]->Fpos+len>BlocoControlo[bloco]->Tamanho)
        {
                if(!(BlocoControlo[bloco]->Modo&EXPANDIR))
                {
			if (BlocoControlo[bloco]->Tamanho > BlocoControlo[bloco]->Fpos)
				len = BlocoControlo[bloco]->Tamanho - BlocoControlo[bloco]->Fpos;
			else
                        	return 0;
                }
        }

	while(len) {
		if(!fsext2LoadCluster(bloco, &o, &r)) {
	       		ERRO=EFAULT;
			return cnt;
		}

		if (r==0)
			return 0;
		if (r > len) {
			r=len;
		}
		_fmemcpy(BlocoControlo[bloco]->Buffer+o, ptr, r);
		len -= r;
		ptr += r;
		cnt += r;
		BlocoControlo[bloco]->dirty = 1;
		BlocoControlo[bloco]->Fpos += r;
	}

	return cnt;
}


static IFS fs_ext2 = {
	fsext2CriarFicheiro,
	fsext2Chmod,
	fsext2Renomear, // Next
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

IFS far *fsext2GetDriver()
{
	return &fs_ext2;
}
