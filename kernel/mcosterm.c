#include "mcoslib.h"
#include "vfs.h"
#include "mcosterm.h"

#define TERM_KEYN 22
#define TERM_KEYB 15

unsigned char Term_KeyBuffer[TERM_KEYN][TERM_KEYB+1];
unsigned char Term_KeyA;
char Term_Buffer[256];
char Term_BackBuffer[256];
int Term_Bp;
char Term_b;
char Term_Tabs;
unsigned char Term_Attr;
char Term_Esc;
char Term_EscPi;
unsigned char Term_EscP[2];
unsigned char Term_EscS;
char Term_EscST;

void interrupt far (*OldKeyInt)();


struct {
	char x,y;
}OldCursor;


void Term_Flush()
{
	Term_b=0;
}


void Term_EdWr(unsigned char ol)
{
	unsigned char a;
	for(a=0;a<ol;a++)
		Term_Output(32);
	Term_EdCB(a);
	for(a=0;a<Term_Bp;a++)
		Term_Output(Term_Buffer[a]);
	Term_EdCB(a);
}


void Term_EdCF(unsigned char c)
{
	unsigned char x;
	for(x=0;x<c;x++)
		Term_CursorF();

}

void Term_EdCB(unsigned char c)
{
	while(c)
	{
		Term_Output(8);
		c--;
	}
}

void Get_Str(char far *s)
{
	char c;
	while(((c=Term_Input())!=10)&&(c!=26))
	{
		*s=c;
		s++;
	}
	*s=0;
}

void send_str(char far *s)
{
	while(*s)
	{
		Term_Output(*s);
		s++;
	}
	Term_Output(0);
}

void write_str(char far *s)
{
	while(*s)
	{
		Term_Output(*s);
		s++;
	}
}

void far *GetIntVect(unsigned char i)
{
	void far *ia;
	ia=MK_FP(peek(0,i*4+2),peek(0,i*4));
	return ia;
}

void SetIntVect(unsigned char i,void far *p)
{
	asm cli;
	poke(0,i*4+2,FP_SEG(p));
	poke(0,i*4,FP_OFF(p));
	asm sti;
}

void Term_Init()
{
  asm cli;
	OldKeyInt=GetIntVect(0x9);
	SetIntVect(0x9,NewKeyInt);
	asm sti;
	Term_b=0;
	Term_Tabs=8;
	Term_Cursor(0);
	Term_BackBuffer[0]=0;
	Term_Attr=7;
	Term_Esc=0;
	Term_KeyA=0;
	OldCursor.x=0;
	OldCursor.y=0;
}

void Term_Done()
{
  asm cli;
	SetIntVect(0x9,OldKeyInt);
	asm sti;
}

void Term_Edit()
{
	unsigned char sub,i,ol;
	unsigned char cursor,cursort;
	unsigned c;
	unsigned char c2;
	char done=0;
	Term_Buffer[0]=0;
	cursor=0;
	sub=0;
	Term_Bp=Strlenx(Term_Buffer);
	do
	{
		ol=Term_Bp;
		cursort=cursor;
		if(sub)
			Term_Cursor(2);
		else
			Term_Cursor(1);
		c=GetKey();
		c2=(unsigned char)c;
		Term_Cursor(0);
		switch(c)
		{
			case 0x47E0:
			case 0x4700:cursor=0;break;
			case 0x4FE0:
			case 0x4F00:cursor=Term_Bp;break;
			case 0x4BE0:
			case 0x4B00:if(cursor) cursor--;break;
			case 0x4DE0:
			case 0x4D00:if(cursor<Term_Bp) cursor++;break;
			case 0x5200:
			case 0x52E0:sub=!sub;break;
			case 0x5300:
			case 0x53E0:if(cursor<Term_Bp) dels(Term_Buffer,cursor,1);break;
			default:switch(c2)
						{
							case 22:for(i=0;i<Strlenx(Term_BackBuffer);i++)
										{
											if((sub)&&(cursor<Strlenx(Term_Buffer)))
												Term_Buffer[cursor++]=Term_BackBuffer[i];
											else if(Strlenx(Term_Buffer)<254)
												insc(Term_Buffer,Term_BackBuffer[i],cursor++);
							}
							break;
							case 27:
								Term_Buffer[0]=0;
								cursor=0;
							break;
							case 8:
								if(cursor>0)
								{
									dels(Term_Buffer,--cursor,1);
								}
							break;
							case 26:
								cursor=Term_Bp;
								done=2;
							break;
							case 13:
								cursor=Term_Bp;
								done=1;
							break;
							default:
								if(c2>=32)
								{
									if((sub)&&(cursor<Term_Bp))
										Term_Buffer[cursor++]=c2;
									else if(Term_Bp<254)
										insc(Term_Buffer,c2,cursor++);
								}
						}
		}
		Term_Bp=Strlenx(Term_Buffer);
		if(cursor>Term_Bp) cursor=Term_Bp;
		Term_EdCB(cursort);
		Term_EdWr(ol);
		Term_EdCF(cursor);
	}while(!done);
	Strcpyx(Term_BackBuffer,Term_Buffer);
	if(done==1)
		Term_Buffer[Term_Bp]=10;
	else
		Term_Buffer[Term_Bp]=26;
	Term_Output(10);
	Term_Bp=0;
	Term_b=1;
}

char Term_Input()
{
	char c;
	if(!Term_b)
		Term_Edit();
	c=Term_Buffer[Term_Bp++];
	if((c==10)||(c==26))
		Term_b=0;
	return c;
}


void Term_Output(char c)
{
	int i;
	if(Term_Esc)
	{
		Term_EscCod(c);
		return;
	}
	switch(c)
	{
		case 12:Term_Cls();break;
		case 8:Term_CursorB();
				break;
		case 9:for(i=0;i<Term_Tabs;i++)
						Char_Output(32);
				break;
		case 13:Char_TTOutput(13);break;
		case 10:Char_TTOutput(10); Char_TTOutput(13);break;
		case 7:Char_TTOutput(c);break;
		case 27:Term_EscInit();break;
		case 17:Term_CursorL();break;
		case 16:Term_CursorR();break;
		case 21:Term_CursorU();break;
		case 25:Term_CursorD();break;
		default: Char_Output(c);
	}
}

void Char_Output(char c)
{
	char p=Term_GetActivePage();
	asm{
		mov ah,0x09;
		mov al,c;
		mov bh,p;
		mov bl,Term_Attr;
		mov cx,1;
		int 0x10;
	}
  Term_CursorF();
}

void Char_TTOutput(char c)
{
	char p=Term_GetActivePage();
	asm{
		mov ah,0x0e;
		mov al,c;
		mov bh,p;
		mov bl,1;
		int 0x10;
	}
}

void Term_Cursor(char o)
{
	char p1,p2;
	switch(o)
	{
		case 0:p1=0x20;p2=1;break;
		case 1:p1=0x08;p2=0x1F;break;
		case 2:p1=0x00;p2=0x1F;break;
//		case 1:p1=0x1E;p2=0x1F;break;
//		case 2:p1=0x8;p2=0x1F;break;
		default:p1=0x0;p2=0x1F;
	}
	asm{
		mov ah,0x01;
		mov cl,p2;
		mov ch,p1;
		int 0x10; 
	}
}

void Term_SetCursorPos(char l,char c)
{
	char p=Term_GetActivePage();
	asm{
		mov ah,0x02;
		mov bh,p;
		mov dh,l;
		mov dl,c;
		int 0x10;
	}
}
void Term_GetCursorPos(char far *l,char far *c)
{
	char nc,nl,p;
	p=Term_GetActivePage();
	asm{
		mov ah,0x03;
		mov bh,p;
		int 0x10;
		mov nl,dh;
		mov nc,dl;
	}

	*l=nl;
	*c=nc;
}

void Term_GetVideoAttr(char far *modo,char far *ncols,char far *pact)
{
	char nm,nnc,nna;
	asm{
		mov ah,0x0F;
		int 0x10;
		mov nm,al;
		mov nnc,ah;
		mov nna,bh;
	}
	*modo=nm;
	*ncols=nnc;
	*pact=nna;
}

char Term_GetActivePage()
{
	char m,c,a;
	Term_GetVideoAttr(&m,&c,&a);
	a=a;
	return a;
}

char Term_GetCols()
{
	char c;

	asm push es;
	asm mov ax,0x40;
	asm mov es,ax;
	asm mov bx,0x4A;
	asm mov al,es:[bx];
	asm pop es;
	asm mov c,al;

	return c;
}

void Term_Cls()
{
	Term_SetCursorPos(0,0);
	asm{
		mov ah,0x06;
		mov al,0x0;
		mov bh,Term_Attr;
		mov cx,0x0;
		mov dx,0xFEFE;
		int 0x10;
	}
}

void Term_ClearLine()
{
	unsigned char ol,oc;
	unsigned char p=Term_GetActivePage();
	unsigned n=Term_GetCols();
	Term_GetCursorPos(&ol,&oc);
	Term_SetCursorPos(ol,0);
	asm{
		mov ah,0x09;
		mov al,0;
		mov bh,p;
		mov bl,Term_Attr;
		mov cx,n;
		int 0x10;
	}
	Term_SetCursorPos(ol,0);
}

void Term_EscCod(char c)
{
	int n;
	if(Term_EscS)
		Term_EscSP(c);
	else
	switch(c)
	{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':n=Term_EscP[Term_EscPi]*10+c-48;
				if((n>=0)&&(n<256))
					Term_EscP[Term_EscPi]=n;
				break;
		case ',':if(Term_EscPi<1)
						Term_EscPi=1;
					else
						Term_Esc=0;
					break;
		case 'f':
		case 'H':Term_SetCursorPos(Term_EscP[0],Term_EscP[1]);
					Term_Esc=0;
					break;
		case 'J':Term_Cls();Term_Esc=0;break;
		case 'K':Term_ClearLine();Term_Esc=0;break;
		case 'W':Char_TTOutput(Term_EscP[0]);Term_Esc=0;break;
		case 'w':Char_Output(Term_EscP[0]);Term_Esc=0;break;
		case 'g':Char_Output(7);Term_Esc=0;break;
		case 'p':if(Term_EscP[0]<TERM_KEYN)
					{
						Term_EscST=0;
						Term_EscS=1;
						Term_SetKeyInit(Term_EscP[0]);
					}
					else
					Term_Esc=0;
					break;
		case 'm':Term_SetAttr(Term_EscP[0]);Term_Esc=0;break;
		case 'c':Term_Cursor(Term_EscP[0]);Term_Esc=0;break;
		case 'h':Term_SetModo(Term_EscP[0]);Term_Esc=0;break;
		case 'a':Term_Attr=Term_EscP[0];Term_Esc=0;break;
		case 's':Term_GetCursorPos(&OldCursor.x,&OldCursor.y);Term_Esc=0;break;
		case 'u':Term_SetCursorPos(OldCursor.x,OldCursor.y);Term_Esc=0;break;
		case 'E':
		case 'F':
		case 'B':
		case 'C':
		case 'D':
		case 'A':for(n=0;n<Term_EscP[0];n++)
					{
						switch(c)
						{
							case 'A':Term_CursorU();break;
							case 'B':Term_CursorD();break;
							case 'C':Term_CursorR();break;
							case 'D':Term_CursorL();break;
							case 'E':Term_CursorF();break;
							case 'F':Term_CursorB();break;
						}
					}
					Term_Esc=0;break;
		default:Term_Esc=0;
	}
}

void Term_EscInit()
{
	Term_Esc=1;
	Term_EscPi=0;
	Term_EscP[0]=0;
	Term_EscP[1]=0;
	Term_EscS=0;
}

void Term_EscSP(char c)
{
	if(!c)
	{
		Term_EscS=0;
		Term_Esc=0;
	}
	else
	switch(Term_EscST)
	{
		case 0:Term_SetKeySend(c);break;
	}
}

void Term_SetKeySend(char c)
{
	unsigned char p;
	p=Term_KeyBuffer[Term_KeyA][0];
	if(p<TERM_KEYB)
	{
		p++;
		Term_KeyBuffer[Term_KeyA][p]=c;
		Term_KeyBuffer[Term_KeyA][0]=p;
	}
}

void Term_SetKeyInit(char k)
{
	if(k<TERM_KEYN)
	{
		Term_KeyA=k;
		Term_KeyBuffer[Term_KeyA][0]=0;
	}
}

void Term_SetAttr(unsigned char a)
{
	switch(a)
	{
		case 1:Term_Attr=Term_Attr|0x08;break;
		case 22:
		case 2:Term_Attr=Term_Attr&0xF7;break;
		case 5:Term_Attr=Term_Attr|0x80;break;
		case 25:Term_Attr=Term_Attr&0x7F;break;
		case 27:
		case 7:Term_Attr=(Term_Attr&0x88)|((Term_Attr&0x70)>>4)+((Term_Attr&0x07)<<4);
				break;
		case 30:Term_Attr=(Term_Attr&0xF8);break;
		case 31:Term_Attr=(Term_Attr&0xF8)|0x4;break;
		case 32:Term_Attr=(Term_Attr&0xF8)|0x2;break;
		case 33:Term_Attr=(Term_Attr&0xF8)|0x6;break;
		case 34:Term_Attr=(Term_Attr&0xF8)|0x1;break;
		case 35:Term_Attr=(Term_Attr&0xF8)|0x5;break;
		case 36:Term_Attr=(Term_Attr&0xF8)|0x3;break;
		case 39:
		case 37:Term_Attr=(Term_Attr&0xF8)|0x7;break;
		case 49:
		case 40:Term_Attr=(Term_Attr&0x8F);break;
		case 41:Term_Attr=(Term_Attr&0x8F)|0x40;break;
		case 42:Term_Attr=(Term_Attr&0x8F)|0x20;break;
		case 43:Term_Attr=(Term_Attr&0x8F)|0x60;break;
		case 44:Term_Attr=(Term_Attr&0x8F)|0x10;break;
		case 45:Term_Attr=(Term_Attr&0x8F)|0x50;break;
		case 46:Term_Attr=(Term_Attr&0x8F)|0x30;break;
		case 47:Term_Attr=(Term_Attr&0x8F)|0x70;break;
		default: Term_Attr=2;
	}
}

void ModoVideo(unsigned char m)
{
	asm{
		mov ah,0x0;
		mov al,m;
		int 0x10;
	}
}

void Term_SetModo(unsigned char m)
{
	switch(m)
	{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
		case 18:
		case 19:ModoVideo(m);break;
	}
}

void Term_CursorR()
{
	char ol,oc;
	Term_GetCursorPos(&ol,&oc);
	if(oc<Term_GetCols()-1)
		Term_SetCursorPos(ol,oc+1);
}

void Term_CursorL()
{
	Char_TTOutput(8);
}

void Term_CursorU()
{
	char ol,oc;
	Term_GetCursorPos(&ol,&oc);
	if(ol>0)
		Term_SetCursorPos(ol-1,oc);
}

void Term_CursorD()
{
	Char_TTOutput(10);
}

void Term_CursorF()
{
	char ol,oc;
	Term_GetCursorPos(&ol,&oc);
	//Term_SetCursorPos(ol,oc+1);

	if(oc<Term_GetCols()-1)
		Term_SetCursorPos(ol,oc+1);
	else
	{
		Char_TTOutput(10);
		Char_TTOutput(13);
	}
}

void Term_CursorB()
{
	char ol,oc;
	Term_GetCursorPos(&ol,&oc);
	if(oc>0)
		Term_SetCursorPos(ol,oc-1);
	else
		if(ol>0)
			Term_SetCursorPos(ol-1,Term_GetCols()-1);
}

void interrupt far NewKeyInt(void)
{
	unsigned baseptr=_BP;
	unsigned char a,i,t;
	unsigned b,w,x;
	unsigned headptr;
	unsigned tailptr;

	b=peek(0x0040,0x001C);
	asm cli;
	x=peek(0x0040,0x0071);
	x=x&0x7F;
	poke(0x0040,0x0071,x);
	asm sti;
	OldKeyInt();
	x=peek(0x0040,0x0071);
	asm cli;
	w=peek(0x0040,b);
	t=0;
	switch(w)
	{
		case 0x3B00:i=0;t=Term_KeyBuffer[i][0];break;//F1
		case 0x3C00:i=1;t=Term_KeyBuffer[i][0];break;//F2
		case 0x3D00:i=2;t=Term_KeyBuffer[i][0];break;//F3
		case 0x3E00:i=3;t=Term_KeyBuffer[i][0];break;//F4
		case 0x3F00:i=4;t=Term_KeyBuffer[i][0];break;//F5
		case 0x4000:i=5;t=Term_KeyBuffer[i][0];break;//F6
		case 0x4100:i=6;t=Term_KeyBuffer[i][0];break;//F7
		case 0x4200:i=7;t=Term_KeyBuffer[i][0];break;//F8
		case 0x4300:i=8;t=Term_KeyBuffer[i][0];break;//F9
		case 0x4400:i=9;t=Term_KeyBuffer[i][0];break;//F10
		case 0x8500:i=10;t=Term_KeyBuffer[i][0];break;//F11
		case 0x8600:i=11;t=Term_KeyBuffer[i][0];break;//F12
		case 0x48E0:i=12;t=Term_KeyBuffer[i][0];break;//Up
		case 0x50E0:i=13;t=Term_KeyBuffer[i][0];break;//Down
		case 0x4BE0:i=14;t=Term_KeyBuffer[i][0];break;//Left
		case 0x4DE0:i=15;t=Term_KeyBuffer[i][0];break;//Right
		case 0x52E0:i=16;t=Term_KeyBuffer[i][0];break;//Ins
		case 0x53E0:i=17;t=Term_KeyBuffer[i][0];break;//Del
		case 0x47E0:i=18;t=Term_KeyBuffer[i][0];break;//Home
		case 0x4FE0:i=19;t=Term_KeyBuffer[i][0];break;//End
		case 0x49E0:i=20;t=Term_KeyBuffer[i][0];break;//PgUp
		case 0x51E0:i=21;t=Term_KeyBuffer[i][0];break;//PgDown
	}
	for(a=0;a<t;a++)
	{
		poke(0x0040,b,Term_KeyBuffer[i][a+1]);
		if((b+=2)>=peek(0x0040,0x0082))
			b=peek(0x0040,0x0080);
		poke(0x0040,0x001C,b);
	}
	
	headptr = *(unsigned far *)0x0040001A;
	tailptr = *(unsigned far *)0x0040001C;
	if(headptr != tailptr) {
		unsigned key = *(unsigned far *) (0x00400000 + headptr);
		static unsigned state = 0;
		if(key==0x1E00 && state==0) {
			state=1;
		}
		else if(state==1) {
			if(key==0x1100) {
				kprintf("Key %X\r\n", key);
			}
			state=0;
		}
	}

	asm sti;
	if(x&0x80) {
/*	  unsigned stackseg = _SS;
	  unsigned i;
	  for(i=0;i<20;i+=2)
	  {
		  kprintf("[SS:BP+%u] [%x:%x] => %x\r\n", i,stackseg,baseptr+i, peek(stackseg, baseptr+i));
	  }
*/
	  kill_proc();
	}
	
}


//char hextab[20] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','X','X','X','X'};

void write_hex(unsigned d)
{
  char t[5];
  t[0] = ((d & 0xF000)>>12);
  t[1] = ((d & 0x0F00)>>8);
  t[2] = ((d & 0x00F0)>>4);
  t[3] = (d & 0x000F);
  t[4] = '\0';

  t[0]=hextab[t[0]];
  t[1]=hextab[t[1]];
  t[2]=hextab[t[2]];
  t[3]=hextab[t[3]];
  write_str(t);
}


void write_num(unsigned d)
{
  char t[6];
  t[0] = (d/10000)%10+'0';
  t[1] = (d/1000)%10+'0';
  t[2] = (d/100)%10+'0';
  t[3] = (d/10)%10+'0';
  t[4] = d%10+'0';
  t[5] = '\0';

  write_str(t);
}


void write_longnum(unsigned long d)
{
  char t[11];
  t[0] = (d/1000000000L)%10+'0';
  t[1] = (d/100000000L)%10+'0';
  t[2] = (d/10000000L)%10+'0';
  t[3] = (d/1000000L)%10+'0';
  t[4] = (d/100000L)%10+'0';
  t[5] = (d/10000L)%10+'0';
  t[6] = (d/1000L)%10+'0';
  t[7] = (d/100L)%10+'0';
  t[8] = (d/10L)%10+'0';
  t[9] = d%10+'0';
  
  t[10] = '\0';

  write_str(t);
}

void term_printf(char *str,...)
{
  int st=0;
  int far *ptr = (int far *)&str;
  ptr++;
  while(*str) {
    switch(st) {
      case 0: if( *str=='%' ) {
                st=1;
              }
              else {
             	Term_Output(*str);
              }
      break;
      case 1:
        switch(*str) {
          case '%': Term_Output('%'); break;
          case 'x':
          case 'X': write_hex(*ptr); ptr++;
            break;
          case 'd':
          case 'i':
          case 'u': write_num(*ptr); ptr++;
            break;
          case 'c': Term_Output(*ptr); ptr++;
			break;
		  case 's': write_str((char *)*ptr); ptr++; break;
		  case 'S':
			write_str((char far *)MK_FP(*(ptr+1),*ptr)); ptr++; ptr++;
        }
        st=0;
      break;
    }
    str++;
  }
}


