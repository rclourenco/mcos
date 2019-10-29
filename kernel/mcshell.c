#include "mcosterm.h"
#include "mcshell.h"
#include "mcos.h"
#include "mcoslib.h"
#include "vfs.h"

char input[256];
char comando[13];
char argumento[128];
char prompt1[]="[MCOS]$ ";
char prompt2[]="[MCOS A:]$ ";

void Debug();


void shell()
{
	set_keys();
	set_terminal();
	getcmd();
}

void getcmd()
{
	char exit=0;
	while(!exit)
	{
		showprompt();
		Get_Str(input);
		parse();
		if(!Strcmp(comando,"!exit"))
			exit=1;
		else if(!Strcmp(comando,"!cls"))
			Cls();
		else if(!Strcmp(comando,"!chdrv"))
			ChDrive();
		else if(!Strcmp(comando,"!mount"))
			MountDrive();
		else if(!Strcmp(comando,"!unmount"))
			UnMountDrive();
		else if(!Strcmp(comando,"!info"))
			Info(0);
		else if(!Strcmp(comando,"!xinfo"))
			Info(1);
		else if(!Strcmp(comando,"!help"))
			Help();
		else if(!Strcmp(comando,"!ls"))
			Ls();
		else if(!Strcmp(comando,"!ed"))
			Ed();
		else if(!Strcmp(comando,"!type"))
			Type();
		else if(!Strcmp(comando,"!lmem"))
			Mem();
		else if(!Strcmp(comando,"!amem"))
			AMem();
		else if(!Strcmp(comando,"!fmem"))
			FMem();
		else if(!Strcmp(comando,"!dump"))
			DumpFile();
		else if(!Strcmp(comando,"!sync"))
			Sync();
		else if(!Strcmp(comando,"!debug"))
			Debug();
		else if(comando[0]) Executar();
	}
}

void set_keys()
{
	send_str("\x1B\x30p!help\r");
	send_str("\x1B\x31p!info\r");
	send_str("\x1B\x32p!cls\r");
	send_str("\x1B\x33p!exit\r");
	send_str("\x1B\x34p!ls\r");
	send_str("\x1B\x35p!mount\r");
	send_str("\x1B\x36p!unmount\r");
}


void set_terminal()
{
	Term_Output(27);write_str("3h");
	Term_Output(27);write_str("0m");
	write_str("MicroComputer Operating System\n");
	write_str(" Ver 1.01\n\n");
	if(CurDrv>=0)
	{
		write_str("Work on Drive ");
		Term_Output('A'+CurDrv);
		write_str(":\n\n");
	}
}

void Cls()
{
	write_str("\x1BJ");
}
void ChDrive()
{
	unsigned char c=argumento[0];
	char d=-1;
	switch(c)
	{
		case 'A':
		case 'a':d=0;break;
		case 'B':
		case 'b':d=1;break;
	}
	if(d==-1)
	{
		write_str("\nInvalid Argument...\n");
		return;
	}
	if(Montada(d))
		CurDrv=d;
	else
		write_str("\nDrive is not mounted...\n");
}

void MountDrive()
{
	unsigned char c=argumento[0];
	char d=-1;
	switch(c)
	{
		case 'A':
		case 'a':d=0;break;
		case 'B':
		case 'b':d=1;break;
	}

	if(d==-1)
	{
		write_str("\nInvalid drive\n\n");
		return;
	}

	if(Montada(d))
	{
		write_str("\nDrive is already mounted\n\n");
		return;
	}

	if(!MontarDrive(d)) {
		write_str("\nError...Cannot mount drive\n\n");
		return;
	}
	
	write_str("\nDrive successful mounted\n\n");
}

void UnMountDrive()
{
	unsigned char c=argumento[0];
	char d=-1;
	switch(c)
	{
		case 'A':
		case 'a':d=0;break;
		case 'B':
		case 'b':d=1;break;
	}

	if(d==-1)
	{
		write_str("\nInvalid drive\n\n");
		return;
	}

	if(CurDrv==d) {
		if(d==0 && Montada(1))
			CurDrv=1;
		else if(d==1 && Montada(0)) 
			CurDrv=0;
		else
			CurDrv=-1;
	}
	if(Montada(d)) {
		DesMontarDrive(d);
	}
}

void Info(int l)
{
	write_str("\nMicroComputer Operating System\n");
	write_str(" MCOS 1.01\n\n");
	if (l>0) {
		char out[30];
		unsigned long stime = (_StartTime1 << 16L) | _StartTime2;
		write_str("StartTime: ");
		_ultoa(stime, out ,3, 12);
		write_str(out);
	        write_str("\n Boot Unit: ");
		_ultoa(_BootUnit, out, 3, 5);
	       	write_str(out);
		write_str("\n");
	}
}

char *token(char **str_in)
{
	char *c;
	if (*str_in==NULL)
		return NULL;

	while((**str_in)==' ')
		(*str_in)++;

	if ((**str_in)=='\0')
		return NULL;

	c = *str_in;

	while ((**str_in)!=' ' && (**str_in)!='\0')
		(*str_in)++;
	if (**str_in==' ') {
		**str_in='\0';
		(*str_in)++;
	}
	return c;
}

void DumpMem(WORD seg, WORD ofs, DWORD max);

void debug_mem(int argc, char **argv)
{
	char *addr;
	WORD seg = 0;
	WORD ofs = 0;
	char out[30];

	if (argc<2) {
		write_str("Sintaxe: ");
		write_str(comando);
		write_str(" ");
		write_str(argv[0]);
		write_str(" ");
		write_str("<segment_hex>:<offset_hex>\n");
		return;
	}

	addr = argv[1];

	while(*addr != '\0' && *addr!=':') {
		if (seg>0xFFF) {
			write_str("Out of range segment\n");
			return;
		}
		if (*addr>='0' && *addr<='9')
			seg = seg*16+(*addr)-'0';
		else if(*addr>='a' && *addr<='f')
			seg = seg*16+((*addr)-'a'+10);
		else if(*addr>='A' && *addr<='F')
			seg = seg*16+((*addr)-'A'+10);
		else {
			write_str("Invalid character at segment\n");
			return;
		}
		addr++;
	}

	if (*addr=='\0') {
		write_str("Missing offset\n");
		return;
	}

	addr++; //skip :
	while(*addr!='\0') {
		if (ofs>0xFFF) {
			write_str("Out of range segment\n");
			return;
		}
		if (*addr>='0' && *addr<='9')
			ofs = ofs*16+(*addr)-'0';
		else if(*addr>='a' && *addr<='f')
			ofs = ofs*16+((*addr)-'a'+10);
		else if(*addr>='A' && *addr<='F')
			ofs = ofs*16+((*addr)-'F'+10);
		else {
			write_str("Invalid character at offset\n");
			return;
		}
		addr++;
	}

	write_str("Segment: ");	_ultoa(seg,out,3,8); write_str(out); write_str("\n");
	write_str("Offset:  "); _ultoa(ofs,out,3,8); write_str(out); write_str("\n");

	DumpMem(seg, ofs, 0x10000L);
}

WORD atod(char *num)
{
	WORD n=0;
	while(*num!='\0') {
		if (n>10000)
			break;
		if( *num>='0' && *num<='9') {
			n=n*10+(*num-'0');
		}
		else {
			break;
		}
		num++;
	}
	return n;
}

void debug_disk(int argc, char **argv)
{
	unsigned seg;

	WORD drive, c, h, s;
	if (argc<5) {
		write_str("Sintaxe: ");
		write_str(comando);
		write_str(" ");
		write_str(argv[0]);
		write_str(" ");
		write_str("<bios_drive> <c> <h> <s>\n");
		return;
	}

	write_str("Go...\n");

	drive=atod(argv[1]);
	c=atod(argv[2]);
	h=atod(argv[3]);
	s=atod(argv[4]);

	write_str("Go...\n");
	Aloca_Segmento(0x40,8,&seg);
        if (!seg)
		return;

	// HCS
	if(!LerSectorFisico(drive, h,c,s, (char far *)MK_FP(seg, 0))) {
		write_str("Drive error\n");
		return;
	}
	DumpMem(seg, 0, 0x200);
	Liberta_Segmento(seg);
}

// assuming little endian
struct {
	BYTE size;
	BYTE res;
	WORD read_number;
	WORD read_offset;
	WORD read_seg;
	WORD sector_w1;
	WORD sector_w2;
	WORD sector_w3;
	WORD sector_w4;
} dap = {
	16, // size
	0,  // res
	1,  // read_number
	0,  // seg
	0,  // off
	0,  // sw1
	0,  // sw2
	0,  // sw3
	0   // sw4
};

void debug_disk3(int argc, char **argv)
{
	unsigned seg;
	unsigned dap_ofs = FP_OFF(&dap);
	unsigned retv;

	WORD drive, lba;
	if (argc<3) {
		write_str("Sintaxe: ");
		write_str(comando);
		write_str(" ");
		write_str(argv[0]);
		write_str(" ");
		write_str("<bios_drive> <lba_sector>\n");
		return;
	}

	drive=atod(argv[1]);
	lba=atod(argv[2]);

	term_printf("Drive: %d %d %d %d\n", drive, sizeof(dap), dap_ofs, &dap);
	dap.sector_w1 = lba;

	write_str("Go...\n");
	Aloca_Segmento(0x40,8,&seg);

	if (!seg)
		return;

	dap.read_seg = seg;
	dap.read_offset = 0;
	
	asm {
		mov ah, 0x42;
		mov dl, byte ptr drive;
		push si;
		mov si, word ptr dap_ofs;
		int 0x13;
		pop si;
		mov byte ptr retv, ah;
		jnc ok;
	}
	retv |= 0x100;
ok:


	if ((retv&0x100)==0)
		DumpMem(seg, 0, 0x200);
	else
		term_printf("Status: %d\n", retv&0xFF);

	Liberta_Segmento(seg);

	
}

void debug_disk2(int argc, char **argv)
{
	unsigned seg;
	BYTE status;
	BYTE cmos_drive_type;
	BYTE cylinders;
	BYTE track_sectors;
	BYTE nsides;
	BYTE ndrives;
	WORD rcylinders;
	WORD drive;
	if (argc<2) {
		write_str("Sintaxe: ");
		write_str(comando);
		write_str(" ");
		write_str(argv[0]);
		write_str(" ");
		write_str("<bios_drive>\n");
		return;
	}

	write_str("Go...\n");

	drive=atod(argv[1]);

	asm {
		mov ah, 0x8;
		mov dl, byte ptr drive;
		int 0x13;
		mov status, ah;
		mov cmos_drive_type, bl;
		mov cylinders, ch;
		mov track_sectors, cl;
		mov nsides, dh;
		mov ndrives, dl;
	}

	term_printf("Drive %d Status %x CmosDriveType %x\n", drive, status, cmos_drive_type);
	rcylinders = ((track_sectors & 0xC0)<<2) | cylinders;
	track_sectors &= 0x3F;
	term_printf("C: %u H: %u S: %u \n", rcylinders+1, nsides+1, track_sectors);
	term_printf("NDrives: %d\n", ndrives);
}


void Debug()
{
	char *str_p = argumento;
	char *argv[10];
	int argc=0;
	int i;
	char *cur;
	while ( argc < 10 && (cur=token(&str_p))!=NULL ) {
		argv[argc++]=cur;
	}

	for (i=0;i<argc;i++) {
		write_str("Arg: ");
		write_str(argv[i]);
		write_str("\n");	
	}

	if (argc<1)
		return;
	if (!Strcmp(argv[0], "mem")) {
		debug_mem(argc, argv);
	}
	else if(!Strcmp(argv[0], "disk")) {
		debug_disk(argc, argv);
	}
	else if(!Strcmp(argv[0], "disk2")) {
		debug_disk2(argc, argv);
	}
	else if(!Strcmp(argv[0], "disk3")) {
		debug_disk3(argc, argv);
	}
}

void Help()
{
	write_str("\nTBD...\n\n");
}

void Executar()
{
  char drv = CurDrv;
  if(comando[1]==':') {
		switch(comando[0])
		{
			case 'a':
			case 'A': drv=0; break;
			case 'b':
			case 'B': drv=1; break;
		}
	Run(drv,comando+2,argumento);
  }
  else {
	Run(drv,comando,argumento);
  }
}

void parse()
{
	int a,b;
	a=0;
	while((input[a]!=32)&&(a<12)&&(input[a]))
	{
		comando[a]=input[a];
		a++;
	}
	if((input[a]!=32)&&(input[a]!=0))
	{
		comando[0]=0;
		argumento[0]=0;
	}
	else
		comando[a]=0;
	if(input[a]==32) a++;
	b=0;
	while((input[a])&&(b<126))
	{
		argumento[b]=input[a];
		a++;
		b++;
	}
	argumento[b]=0;
}

void Sync()
{
	if(Montada(0))
		SyncDrive(0);
	if(Montada(1))
		SyncDrive(1);
	write_str("Drives Synced!\n");
}

void showprompt()
{
	if(CurDrv>=0)
	{
		prompt2[6]=CurDrv+'A';
		write_str(prompt2);
	}
	else
		write_str(prompt1);
}

void Ls()
{
	TDIR_RECORD d;
	int linhas,numl;
	int nfich;
	char v,i;
	char out[20];
	unsigned char c=argumento[0];
	char drive=CurDrv;
	switch(c)
	{
		case 'A':
		case 'a':drive=0;break;
		case 'B':
		case 'b':drive=1;break;
	}
	ApagaErro();
	if(drive>1)
	{
		write_str("\nInvalid disck drive...\n\n");
		return;
	}
	if(!Montada(drive))
	{
		write_str("\nThe unit is not mounted...\n\n");
		return;
	}
	write_str("\nListing unit ");
	Term_Output('A'+drive);
	write_str(":\n");
	linhas=25;
	v=DirProcura(drive,&d,1);
	numl=0;
	nfich=0;
	while(v)
	{
		numl++;
		nfich++;
		strp(out,d.nome,12);
		write_str(out);write_str(" ");
		_ultoa(d.Tamanho,out,3,8);
		write_str(out);write_str(" Bytes ");
		_ultoa(d.Data.dia,out,2,2);
		write_str(out);write_str("-");
		_ultoa(d.Data.mes,out,2,2);
		write_str(out);write_str("-");
		_ultoa(d.Data.ano,out,2,4);
		write_str(out);write_str(" ");
		_ultoa(d.Hora.hora,out,2,2);
		write_str(out);write_str(":");
		_ultoa(d.Hora.minutos,out,2,2);
		write_str(out);write_str(" ");
		if(d.Attr&So_leitura)
			write_str("READ_ONLY ");
		write_str("\n");
		if(numl>=linhas-4)
		{
			write_str("\nPress any key to continue... or ESC to break...\n\n");
			if(GetKey()==0x011B) break;
			numl=0;
		}
		v=DirProcura(drive,&d,0);
	}
	_ultoa(nfich,out,3,8);
	write_str("\nNumber of files: ");write_str(out);write_str("\n");
	_ultoa(FreeSpace(drive),out,3,8);
	write_str("Free Space:        ");write_str(out);write_str("\n");
	_ultoa(DiskSpace(drive),out,3,8);
	write_str("Total Space:       ");write_str(out);write_str("\n");

	if(ERRO)
		escreve_erro();
}

void strp(char far *dest,char far *or,unsigned char t)
{
	unsigned char i;
	for(i=0;i<t;i++)
	{
		if(i<Strlen(or))
			dest[i]=or[i];
		else
			dest[i]=32;
	}
	dest[i]=0;
}

void escreve_erro()
{
		write_str("ERRO: ");
		write_str(GetMsgErro(ERRO));
		write_str(".\n");
}

void Ed()
{
	WORD handle;
	char ch;
	ApagaErro();
	handle=AbrirFicheiro(CurDrv,argumento,ESCREVER|CRIAR_F|EXPANDIR);
	if((handle>=FILES)||(ERRO))
	{
		write_str("Erro ao criar ficheiro de destino...\n");
		escreve_erro();
		return;
	}
	ch=Term_Input();
	while((ch!=26)&&(!ERRO))
	{
		EscreverCaracter(handle,ch);
		ch=Term_Input();
	}
	FecharFicheiro(handle);
	if(ERRO)
		escreve_erro();
}

void Type()
{
	WORD handle;
	WORD ch;
	WORD lines;
	ApagaErro();
	handle=AbrirFicheiro(CurDrv,argumento,LEITURA);
	if((handle>=FILES)||(ERRO))
	{
		write_str("Erro a abrir ficheiro de origem...\n");
		escreve_erro();
		return;
	}
	lines=0;
	while(!Fim_Ficheiro(handle))
	{
		ch=LerCaracter(handle);
		if(ERRO) break;
		Term_Output(ch);
		if(ch=='\n') {
			lines++;
			if(lines==24) {
				int s=0;
				Term_SetAttr(7);
				write_str("Press a key to continue...");
				Term_SetAttr(7);
				if(GetKey()==0x011B) s=1;
				Term_ClearLine();
				lines=0;
				if(s) break;
			}
		}
	}
	Term_Output(10);
	if(ERRO)
		escreve_erro();
	FecharFicheiro(handle);
	if(ERRO)
		escreve_erro();
}

#define DUMPCOLS 16
void DumpFile()
{
	WORD handle;
	WORD ch;
	WORD lines;
	WORD cols;
	WORD offset,old_offset;
	BYTE h1,h2,h3,h4;
	BYTE lb[DUMPCOLS+1];
	int s=0;
	ApagaErro();
	handle=AbrirFicheiro(CurDrv,argumento,LEITURA);
	if((handle>=FILES)||(ERRO))
	{
		write_str("Erro a abrir ficheiro de origem...\n");
		escreve_erro();
		return;
	}
	lines=0;
	cols=0;
	offset=0;
	do {
		WORD k;
		lines=0;
		cols=0;
		offset=Ficheiro_Pos(handle);
		old_offset=offset;
		while(!s && !Fim_Ficheiro(handle))
		{
			if(offset%DUMPCOLS==0) {
				write_str("   ");
				h1 = (offset>>12)&0xF;
				h2 = (offset>>8)&0xF;
				h3 = (offset>>4)&0xF;
				h4 = (offset>>0)&0xF;
				Term_Output(hextab[h1]);
				Term_Output(hextab[h2]);
				Term_Output(hextab[h3]);
				Term_Output(hextab[h4]);
				write_str("  ");
			}
			ch=LerCaracter(handle);
			if(ch<32 || ch>126 ){
				lb[cols]='.';
			} else {
				lb[cols]=ch; 
			}
			if(ERRO) break;
			h1 = (ch>>4)&0xF;
			h2 = ch&0xF;
			Term_Output(hextab[h1]);
			Term_Output(hextab[h2]);
			Term_Output(' ');
			cols++;
			if(cols==DUMPCOLS/2) {
				write_str("- ");
			}
			else if(cols==DUMPCOLS) {
				lb[cols]=0;
				write_str("  ");
				write_str(lb);
				Term_Output(10);
				lines++;
				if(lines==24) {
					Term_SetAttr(7);
					write_str("Press a key to continue...");
					Term_SetAttr(7);
					k=GetKey();
					Term_ClearLine();
					lines=0;
					switch(k)
					{
						case 0x011B: s=1; break;
						case 0x4800: 
							if(Ficheiro_Pos(handle)>DUMPCOLS*48) {
								PosicionarFicheiro(handle, -DUMPCOLS*48, SF_CURRENTE);
							} else {
								PosicionarFicheiro(handle, 0, SF_INICIO);
							}
							offset=Ficheiro_Pos(handle)-1;
							break;
					}
					if(s) break;
				}
				cols=0;
			}
			offset++;
		}

		if(cols) {
			int i;
			lb[cols]=0;
			for(i=0;i<DUMPCOLS-cols;i++)
				write_str("   ");
			if(cols<DUMPCOLS/2) {
				write_str("    ");
			}
			else {
				write_str("  ");
			}
			write_str(lb);
			Term_Output(10);
		}
		Term_SetAttr(7);
		write_str("   END    ");
		Term_SetAttr(7);
		k=GetKey();
		Term_ClearLine();
		do {
			switch(k)
			{
				case 0x011B: s=1; break;
				case 0x4800:
					offset=Ficheiro_Pos(handle);
		
					if(offset%(DUMPCOLS*24)) {
						offset=-DUMPCOLS*23-offset%(DUMPCOLS*24);
					}
					else {
						offset=-DUMPCOLS*24;
					}

					if(Ficheiro_Pos(handle)>offset) {
						PosicionarFicheiro(handle, -offset, SF_CURRENTE);
					} else {
						PosicionarFicheiro(handle, 0, SF_INICIO);
					}
				break;
			}
			if(s) break;
			k=GetKey();
			offset=Ficheiro_Pos(handle);
		}while(offset==old_offset);

	} while(!s);
	if(ERRO)
		escreve_erro();
	FecharFicheiro(handle);
	if(ERRO)
		escreve_erro();
}

void DumpMem(WORD seg, WORD ofs, DWORD max)
{
	WORD ch;
	WORD lines;
	WORD cols;
	WORD offset,old_offset;
	DWORD fpos=ofs;
	BYTE h1,h2,h3,h4;
	BYTE lb[DUMPCOLS+1];
	int s=0;
	lines=0;
	cols=0;
	offset=0;
	do {
		WORD k;
		lines=0;
		cols=0;
		offset=fpos;
		old_offset=offset;
		while(!s && fpos<max)
		{
			if(offset%DUMPCOLS==0) {
				write_str(" ");
				h1 = (seg>>12)&0xF;
				h2 = (seg>>8)&0xF;
				h3 = (seg>>4)&0xF;
				h4 = (seg>>0)&0xF;
				Term_Output(hextab[h1]);
				Term_Output(hextab[h2]);
				Term_Output(hextab[h3]);
				Term_Output(hextab[h4]);
				write_str(":");
				h1 = (offset>>12)&0xF;
				h2 = (offset>>8)&0xF;
				h3 = (offset>>4)&0xF;
				h4 = (offset>>0)&0xF;
				Term_Output(hextab[h1]);
				Term_Output(hextab[h2]);
				Term_Output(hextab[h3]);
				Term_Output(hextab[h4]);
				write_str(" ");
			}
			ch=peekb(seg, fpos);
			fpos++;	
			if(ch<32 || ch>126 ){
				lb[cols]='.';
			} else {
				lb[cols]=ch; 
			}

			h1 = (ch>>4)&0xF;
			h2 = ch&0xF;
			Term_Output(hextab[h1]);
			Term_Output(hextab[h2]);
			Term_Output(' ');
			cols++;
			if(cols==DUMPCOLS/2) {
				write_str("- ");
			}
			else if(cols==DUMPCOLS) {
				lb[cols]=0;
				write_str("  ");
				write_str(lb);
				Term_Output(10);
				lines++;
				if(lines==24) {
					Term_SetAttr(7);
					write_str("Press a key to continue...");
					Term_SetAttr(7);
					k=GetKey();
					Term_ClearLine();
					lines=0;
					switch(k)
					{
						case 0x011B: s=1; break;
						case 0x4800: 
							if(offset>DUMPCOLS*48) {
								fpos-=DUMPCOLS*48;
							} else {
								fpos=0;
							}
							offset=fpos-1;
							break;
					}
					if(s) break;
				}
				cols=0;
			}
			offset++;
		}

		if(cols) {
			int i;
			lb[cols]=0;
			for(i=0;i<DUMPCOLS-cols;i++)
				write_str("   ");
			if(cols<DUMPCOLS/2) {
				write_str("    ");
			}
			else {
				write_str("  ");
			}
			write_str(lb);
			Term_Output(10);
		}
		Term_SetAttr(7);
		write_str("   END    ");
		Term_SetAttr(7);
		k=GetKey();
		Term_ClearLine();
		do {
			switch(k)
			{
				case 0x011B: s=1; break;
				case 0x4800:
					offset=fpos;
		
					if(offset%(DUMPCOLS*24)) {
						offset=-DUMPCOLS*23-offset%(DUMPCOLS*24);
					}
					else {
						offset=-DUMPCOLS*24;
					}

					if(fpos>offset) {
						fpos-=offset;
					} else {
						fpos=0;
					}
				break;
			}
			if(s) break;
			k=GetKey();
			offset=fpos;
		}while(offset==old_offset);

	} while(!s);

}

void Mem()
{
	char out[30];
	char f=0;
	int r;
	MCB m;
	unsigned s;
	write_str("Memory:\n");
	r=MEM_Search(1,&m,&s);
	if(r==2) f=1;
	while(!f)
	{
		if(r>0) f=1;
		_ultoa(s,out,3,8);
		write_str(out);
		write_str(" ");
		Term_Output(m.Tipo);
		write_str(" ");
		_ultoa(m.Tam,out,3,8);
		write_str(out);
		write_str(" ");
		_ultoa(m.ProcessoID,out,3,8);
		write_str(out);
		write_str("\n");
		r=MEM_Search(0,&m,&s);
	 }
	 write_str("\nEND...\n\n");
}

void AMem()
{
	unsigned seg;
	Aloca_Segmento(100,8,&seg);

}

void FMem()
{
	LibertaMemPID(8);
}
