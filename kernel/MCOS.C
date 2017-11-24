#include "mcos.h"
#include "mcosterm.h"
#include "mcoslib.h"


#pragma inline


void interrupt far Dispatcher(unsigned bp, unsigned di,unsigned si,
										unsigned ds, unsigned es,unsigned dx,
										unsigned cx, unsigned bx,unsigned ax);

void interrupt far CtrlBreak(unsigned bp, unsigned di,unsigned si,
										unsigned ds, unsigned es,unsigned dx,
										unsigned cx, unsigned bx,unsigned ax);

void (far *program)();
void interrupt (*oldint80)();
void interrupt (*oldint1B)();
char CurDrv=-1;
char bootdrive=0;
unsigned CurProcess=0;

void System_Init()
{
  CurProcess=1;
 // kprintf("Memory Init.............................................");
	MEM_Init2();
//	kprintf("[Done]\r\n");

  //kprintf("File System Init........................................");
	INITFS();
	//kprintf("[Done]\r\n");

	if(ERRO)
	{
		kprintf("Falha a inicializar sistema de ficheiros...\r\n");
		//kprintf(GetMsgErro(ERRO));
//		kprintf("\r\n");
		_exit(0);
	}
	Term_Init();
	MontarDrive(bootdrive);
	if(Montada(bootdrive))
		CurDrv=bootdrive;
	else
		CurDrv=-1;

	/*Mount the another floppy unit*/
	if(bootdrive==0)
		MontarDrive(1);
	else
		MontarDrive(0);

	asm cli;
	oldint80=GetIntVect(0x80);
	SetIntVect(0x80,Dispatcher);

//	oldint1B=GetIntVect(0x1B);
//	SetIntVect(0x1B,CtrlBreak);
	asm sti;
}

void System_Done()
{
  asm cli;
	SetIntVect(0x80,oldint80);
//	SetIntVect(0x1B,oldint1B);
  asm sti;

	//kprintf("Syscall interrupt Shutdown.........[DONE]\r\n");
	Term_Done();
	//kprintf("Term Shutdown......................[DONE]\r\n");
	CLOSEFS();
	//kprintf("File System Shutdown...............[DONE]\r\n");
	MEM_Done();
	//kprintf("Memory System Shutdown.............[DONE]\r\n");
}

void interrupt far CtrlBreak(unsigned bp, unsigned di,unsigned si,
										unsigned ds, unsigned es,unsigned dx,
										unsigned cx, unsigned bx,unsigned ax)
{
  unsigned char d;
  if(CurProcess > 0x70) {
    ModoVideo(3);
    kprintf( "Terminate called\r\n" );
    kprintf( "SS:SP %x:%x\r\n", _SS, _SP);
    kprintf( "DS ES %x:%x\r\n", _DS, _ES);
    asm in al, 0x21;
    asm mov al,d;
    kprintf( "MASK %x\r\n",d);
    asm mov al, 0x20;
    asm out 0x20,al;
    //outportb (0x20, 0x20)
    Exit_Process();
  }
  else{
    kprintf( "Nothing to kill\r\n" );
    asm in al, 0x21;
    asm mov al,d;
    kprintf( "MASK %x\r\n",d);
  }
}

void kill_proc()
{
  if(CurProcess > 0x70) {
    ModoVideo(3);
    kprintf( "Terminate called\r\n" );
    kprintf( "SS:SP %x:%x\r\n", _SS, _SP);
    kprintf( "DS ES %x:%x\r\n", _DS, _ES);
    Exit_Process();
  }
}

void Exit_Process()
{
	asm mov ax,CurProcess;    /*Altera o Segmento Extra*/
	asm mov es,ax;            /*para o segmento do programa*/

	asm cli;              /*Restaura os registos de pilha*/
	asm mov bx,0x10;
	asm mov ax,es:[bx];
	asm mov ss,ax;
	asm mov bx,0x12;
	asm mov ax,es:[bx];
	asm mov sp,ax;
	asm sti;

	asm pop si;
	asm pop di;
	asm pop ds;
	asm pop es;
	asm pop bp;
	asm popf;

}



void Callprg()
{

	asm mov bx,OFFSET(ci)+1;
	asm mov word ptr dx,CurProcess;
	asm mov word ptr CS:[bx+2],dx;
	asm mov word ptr CS:[bx],0x100;


	asm pushf;
	asm push bp;
	asm push es;
	asm push ds;
	asm push di;
	asm push si;

	asm mov ax,CurProcess;    /*Altera o Segmento de Dados e o Extra*/
	asm mov es,ax;            /*para o segmento do programa*/
	asm mov ds,ax;

	asm cli;               /*Guarda os registos de pilha actuais*/
	asm mov ax,ss;
	asm mov bx,0x10;
	asm mov [bx],ax;
	asm mov ax,sp;
	asm mov bx,0x12;
	asm mov [bx],ax;

	asm mov ax,ds;         /*Configura os registos de pilha para o programa*/
	asm mov ss,ax;
	asm mov ax,0xFFF0;
	asm mov sp,ax;

	asm sti;

	asm ci: db 0x9A,0x00,0x00,0x00,0x00;

	asm re:

	asm cli;              /*Restaura os registos de pilha*/
	asm mov bx,0x10;
	asm mov ax,[bx];
	asm mov ss,ax;
	asm mov bx,0x12;
	asm mov ax,[bx];
	asm mov sp,ax;
	asm sti;

	asm pop si;
	asm pop di;
	asm pop ds;
	asm pop es;
	asm pop bp;
	asm popf;
}



void SetPSP(WORD psp,char far *nome,char far *cmd)
{
	unsigned char far *p=(unsigned char far *)MK_FP(psp,0x0);
	unsigned int far *opsp=(unsigned int far *)MK_FP(psp,0xE);
	TProcessDescriptor far *pdt=(TProcessDescriptor far *)MK_FP(psp,PDT_OFS);
	unsigned i;
	p[0]=0xB8;p[1]=0x00;p[2]=0x00; /*mov ax,0*/
       //	p[0]=0xCB;p[1]=0x90;p[2]=0x90; /*retf;nop;nop*/
	p[3]=0x90;                     /*nop*/
	p[4]=0x90;                     /*nop*/
	p[5]=0xCD;p[6]=0x80;           /*int 80*/
	p[7]=0xC3;                     /*ret*/
	p[10]='M';p[11]='C';p[12]='O';p[13]='S';   /*DB 'MCOS'*/
	*opsp=CurProcess;
	p[0x20]=CurDrv;
	/* TODO: add env pointer */
	for(i=0;i<PDT_N;i++)
	{
		pdt[i].idx=0xFF;
		pdt[i].flags=0;
	}
	
	/* TODO: descriptor process->kernel map */
	p=(unsigned char far *)MK_FP(psp,0x70);
	i=0;
	while(nome[i])
	{
		p[i]=nome[i];
		i++;
	}
	p[i]=0;
	
	p=(unsigned char far *)MK_FP(psp,0x80);
	i=0;
	while(cmd[i] && i<127)
	{
		p[i]=cmd[i];
		i++;
	}
	p[i]=0;

}

WORD Load(BYTE drv,char far *nome)
{
	WORD seg,handle;
	BYTE ch;
	WORD offset;
	WORD f;
	ApagaErro();
	if(!Aloca_Segmento(4096,0,&seg))
		return 0;
	handle=AbrirFicheiro(drv,nome,LEITURA);
	if(!ERRO)
	if(Ficheiro_Size(handle)>0xD000)
	{
			ERRO=EFAULT;
	}
	else
	{
		offset=0x100;
		while(!Fim_Ficheiro(handle))
		{
			ch=LerCaracter(handle);
			if(ERRO) break;
			pokeb(seg,offset,ch);
			offset++;
		}
	}
	f=ERRO;
	if(ERRO)
	{
		Liberta_Segmento(seg);
	}
	FecharFicheiro(handle);
	if(f)
		return 0;
	else
		return seg;
}

BYTE TestValidPrg(WORD psp)
{
	unsigned char far *p=(unsigned char far *)MK_FP(psp,0x100);
	if((p[3]=='M')&&(p[4]=='C'))
		return 1;
	else
		return 0;
}

BYTE TestValidPsp(WORD psp)
{
	unsigned char far *p=(unsigned char far *)MK_FP(psp,0x0);
	if((p[10]=='M')&&(p[11]=='C')&&(p[12]=='O')&&(p[13]=='S'))
		return 1;
	else
		return 0;
}

void Run(BYTE drv,char far *nome,char far *cmd)
{
	WORD pid;
	pid=Load(drv,nome);
	if(pid<2)
	{
		return;
	}
	if(!TestValidPrg(pid))
	{
		Liberta_Segmento(pid);
		return;
	}
	program=MK_FP(pid,0x100);
	SetPSP(pid,nome,cmd);
	CurProcess=pid;
	Callprg();
	CurProcess=peek(pid,0xE);
	FecharFicheirosProcesso(pid);
	Liberta_Segmento(pid);
	LibertaMemPID(pid);
}


void Dump(WORD pid)
{
	unsigned char far *p=(unsigned char far *)MK_FP(pid,0x0);
	BYTE v,i;
	for(i=0;i<20;i++)
	{
		v=p[0x100+i];
		kprintf("%04X -> %02X оп %c ",0x100+i,v,v>31?v:'.');
	}
}

int getFreeDescriptor(WORD psp)
{
	TProcessDescriptor far *pdt=(TProcessDescriptor far *)MK_FP(psp,PDT_OFS);
	unsigned i;
	for(i=0;i<PDT_N;i++)
	{
		if( pdt[i].idx==0xFF ) {
			return i;
		}
	}
	return -1;
}

void setDescriptor(WORD psp, int i, BYTE idx, BYTE flags)
{
	TProcessDescriptor far *pdt=(TProcessDescriptor far *)MK_FP(psp,PDT_OFS);
	if(i<0 || i>=PDT_N)
		return;
	pdt[i].idx=idx;
	pdt[i].flags=flags;
}

int getDescriptorIdx(WORD psp, int i, BYTE far *flags)
{
	TProcessDescriptor far *pdt=(TProcessDescriptor far *)MK_FP(psp,PDT_OFS);
	if(i<0 || i>=PDT_N)
		return -1;
	if(flags)
		*flags = pdt[i].flags;
	return pdt[i].idx;
}


WORD Open(BYTE far *nome,BYTE modo)
{
	int n=0;
	char drive=-1;
	if(CurProcess<0x0070)
		return 0xFFFF;
	
	modo &= MODMASK;

	while(nome[n] && nome[n]!=':') n++;
	if(nome[n]) {
		if(n==1) {
			switch(nome[0])
			{
				case 'a':
				case 'A': drive=0; break;
				case 'b':
				case 'B': drive=1; break;
				case '#': drive=-2; break;
			}
			n++;
		}
	}
	else {
		n=0;
		drive=CurDrv;
	}

	if(drive>=0) {
		int handle = getFreeDescriptor(CurProcess);
		if(handle!=-1) {
			WORD idx=AbrirFicheiro(drive,nome+n,modo);
			if(idx<256) {
				setDescriptor(CurProcess, handle, idx, REGULAR|modo);
				return handle;
			}
		}
	}
	else if(drive==-2) {
		kprintf("[KERNEL] Open Device [%S]\r\n", nome+n );
		/*TODO open device*/
		return 0xFFFF;
	}
	return 0xFFFF;
}

void Close(handle)
{
	BYTE flags = 0;
	int idx = getDescriptorIdx(CurProcess, handle, &flags);
	if(idx<0) {
		return;
	}

	if( (flags & DEVMASK) == REGULAR )
		FecharFicheiro(idx);
}

void Flush(WORD handle)
{
	BYTE flags = 0;
	int idx = getDescriptorIdx(CurProcess, handle, &flags);
	if(idx<0) {
		return;
	}

	if( (flags & DEVMASK) == REGULAR )
		FlushFicheiro(idx);
}

WORD WriteChar(WORD handle,BYTE caracter)
{
	BYTE flags = 0;
	int idx = getDescriptorIdx(CurProcess, handle, &flags);

	if(idx<0) {
		return 0;
	}

	if( (flags & DEVMASK) == REGULAR )
		return EscreverCaracter(idx,caracter);
	return 0;
}

WORD ReadChar(WORD handle)
{
	BYTE flags = 0;
	int idx = getDescriptorIdx(CurProcess, handle, &flags);
	if(idx<0) {
		return 0;
	}

	if( (flags & DEVMASK) == REGULAR )
		return LerCaracter(idx);
	return 0;
}

void Seek(WORD handle, long deslocamento, BYTE modo)
{
	BYTE flags = 0;
	int idx = getDescriptorIdx(CurProcess, handle, &flags);
	if(idx<0) {
		return;
	}

	if( (flags & DEVMASK) == REGULAR )
		PosicionarFicheiro(idx,deslocamento,modo);
}

void Truncate(WORD handle)
{
	BYTE flags = 0;
	int idx = getDescriptorIdx(CurProcess, handle, &flags);
	if(idx<0) {
		return;
	}

	if( (flags & DEVMASK) == REGULAR )
		TruncarFicheiro(idx);
}

DWORD Position(WORD handle)
{
	BYTE flags = 0;
	int idx = getDescriptorIdx(CurProcess, handle, &flags);

	if(idx<0) {
		return 0L;
	}

	if( (flags & DEVMASK) == REGULAR )
		return Ficheiro_Pos(idx);
	return 0L;
}

DWORD Size(WORD handle)
{
	BYTE flags = 0;
	int idx = getDescriptorIdx(CurProcess, handle, &flags);

	if(idx<0) {
		return 0L;
	}

	if( (flags & DEVMASK) == REGULAR )
		return Ficheiro_Size(idx);
	return 0L;
}

BYTE EndOfFile(WORD handle)
{
	BYTE flags = 0;
	int idx = getDescriptorIdx(CurProcess, handle, &flags);
	if(idx<0) {
		return 0;
	}

	if( (flags & DEVMASK) == REGULAR )
		return Fim_Ficheiro(idx);
}

void interrupt Dispatcher(unsigned bp, unsigned di,unsigned si,
				unsigned ds, unsigned es,unsigned dx,
				unsigned cx, unsigned bx,unsigned ax)
{
	void far *p;
	long l1;
	unsigned far *lh,far *ll;
	unsigned char ah=ax>>8;
	unsigned char al=ax;
	unsigned char bh=bx>>8;
	unsigned char bl=bx;
	char out[25];
	unsigned sds;
	char outo;
	int i,i2;
	asm mov ax,ds;
	asm mov sds,ax;

	ll=(unsigned far *)&l1;
	lh=ll+1;
	*ll=dx;
	*lh=cx;


	out[0]='0';
	out[1]='1';
	out[2]='0';
	out[3]=0;

	i=0;
	i2=0;
	switch(ah)
	{

		case 0:Exit_Process();break;

		//funcoes de terminal
		case 1:Term_Output(al);break;
		case 2:Char_Output(al);break;
		case 3:Char_TTOutput(al);break;
		case 4:ax=Term_Input();break;
		case 5:Term_Flush();break;
		case 6:Get_Str((char far *)MK_FP(es,dx));break;
		case 7:send_str((char far *)MK_FP(es,dx));break;
		case 8:write_str((char far *)MK_FP(es,dx));break;
		case 9:
		       _ultoa(l1,out,bh,bl);
		       write_str(out);
		       break;
		case 10:
				l1=123456789L;
				dx=l1;
				cx=l1>>16;
				break;
		case 11:
			bx=SIG_MC;
			cx=SIG_OS;
			ax=VERSION;
		break;
		//funcoes de memoria
		case 15:ax=Aloca_Segmento(bx,CurProcess,(unsigned far *)MK_FP(es,dx));break;
		case 16:Liberta_Segmento(bx);break;
		case 17:ax=Realoca_Segmento(bx,(unsigned far *)MK_FP(es,dx));break;
		case 18:al=MEM_Search(al,(MCB far *)MK_FP(es,dx),(unsigned far *)MK_FP(bx,cx));break;

		//funcoes de systema de ficheiros
		case 25:ax=MontarDrive(al);break;
		case 26:ax=DesMontarDrive(al);break;
		case 27:ax=SyncDrive(al);break;
		case 28:ax=Montada(al);break;
		case 29:ax=DirProcura(al,(TDIR_RECORD far *)MK_FP(es,dx),bl);break;
		
		case 30:ax=AbrirFicheiro(al,(BYTE far *)MK_FP(es,dx),bl);break; // TODO deal with devices
		case 31:ax=EscreverCaracter(bx,al);break; 
		case 32:ax=LerCaracter(bx);break;
		case 33:FecharFicheiro(bx);break;
		case 34:PosicionarFicheiro(bx,l1,al);break;
		case 35:l1=Ficheiro_Pos(bx);
					dx=l1;
					cx=l1>>16;
			break;
		case 36:l1=Ficheiro_Size(bx);
					dx=l1;
					cx=l1>>16;
			break;
		case 37:ax=Fim_Ficheiro(bx);
		        break;
		case 38:TruncarFicheiro(bx);break;
		
		case 39:CriarFicheiro(al,(BYTE far *)MK_FP(es,dx),bl);break;
		case 40:Renomear(al,(BYTE far *)MK_FP(es,dx),(BYTE far *)MK_FP(bx,cx));break;
		case 41:Eliminar(al,(BYTE far *)MK_FP(es,dx));break;
		case 42:ax=GetAttr(al,(BYTE far *)MK_FP(es,dx));
		        break;
		case 43:Chmod(al,(BYTE far *)MK_FP(es,dx),bl);break;
		case 44:l1=FreeSpace(al);
				dx=l1;
				cx=l1>>16;
			break;
		case 45:l1=DiskSpace(al);
				dx=l1;
				cx=l1>>16;
			break;
		case 46:ax=LerSector(al,bx,MK_FP(es,dx));
		        break;
		case 47:ax=EscreverSector(al,bx,MK_FP(es,dx));break;
		case 48:ax=ERRO;break;
		case 49:ApagaErro();break;
		case 50:p=(void far *)GetMsgErro(bx);
				dx=FP_OFF(p);
				es=FP_SEG(p);
			break;

		case 60:GetDate((TDATA far*)MK_FP(es,dx));break;
		case 61:GetHora((THORA far *)MK_FP(es,dx));break;
		case 65:ax=GetKey(); break;

		case 70:ax=CurProcess;break;

		case 80:ax=Open((BYTE far *)MK_FP(es,dx),bl);break; // TODO deal with devices
		case 81:ax=WriteChar(bx,al);break; 
		case 82:ax=ReadChar(bx);break;
		case 83:Close(bx);break;
		case 84:Seek(bx,l1,al);break;
		case 85:l1=Position(bx);
					dx=l1;
					cx=l1>>16;
			break;
		case 86:l1=Size(bx);
					dx=l1;
					cx=l1>>16;
			break;
		case 87:ax=EndOfFile(bx);break;
		case 88:Truncate(bx);break;

		
		default:ax=0xFFFF;break;
	}
}

