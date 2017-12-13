#include "mcosapi.h"

#pragma inline

char *prgname=(char *)0x70;
char *args=(char *)0x80;
char *curdrv=(char *)0x20;

extern int _argc;
extern char **_argv;

char *argtab[64];
char sargs[128];

unsigned key_last=0;
unsigned key_pending=0;

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


void exit()
{
	asm {
		mov ah,0h;
		int 80h;
	}
}

int getch()
{
	if(key_pending) {
		key_pending = 0;
		return key_last >> 8;
	}
	key_last = mcos_getkey();
	if((key_last&0xFF)==0) {
		key_pending = 1;
	}
	
	return key_last&0xFF;
}

int kbhit()
{
	int k=1;
	if(key_pending)
		return 1;
	asm {
		mov ah,01h;
		int 16h;
		jnz got;
		mov k, 0;
	}
	got:
	return k;
}



void mcos_makeargs()
{
	int i;
	int k=0;
	int j=0;
	argtab[j++]=prgname;
	for(i=0;args[i];i++)
		sargs[i]=args[i];
	sargs[i]=0;
	for(i=0;sargs[i];i++)
	{
		if(sargs[i]==' ' && k==1) {
			k=0;
			sargs[i]=0;
		}
		else if(sargs[i]!=' ' && k==0) {
			argtab[j++]=&sargs[i];
			k=1;
		}
	}
	sargs[i]=0;
	_argc=j;
	_argv=argtab;
}


void mcos_print(char *str)
{
	asm{
		push es
		push ds
		pop es
		mov ah,8;
		mov dx,str;
		int 80h;
		pop es;
	}
}

void mcos_clearerr()
{
	asm{
		mov ah,49;
		int 80h;
	}
}

int mcos_error()
{
	int e;
	asm {
		mov ah,48;
		int 80h;
		mov e,ax;
	}
	return e;
}

int mcos_open(char *pathname, int mode)
{
	int r;
	asm {
		push es;
		push ds;
		pop es;
		mov ah,80;
		mov bl,mode;
		mov dx,pathname;
		int 80h;
		mov r,ax;
		pop es;
	}
	return r;
}

void mcos_close(int handle)
{
	asm {
		mov ah,83;
		mov bx,handle;
		int 80h;
	}
}

int mcos_eof(int handle)
{
	int eof;
	asm {
		mov ah,87;
		mov bx,handle;
		int 80h;
		mov eof,al;
	}
	return eof;
}

unsigned long mcos_fsize(int handle)
{
	unsigned l,h;
	asm {
		mov ah,36;
		mov bx,handle;
		int 80h;
		mov h,cx;
		mov l,dx;
	}
	return (h<<16)+l;
}


int mcos_fputc(int handle, char c)
{
	int eof;
	asm {
		mov ah,81;
		mov al,c;
		mov bx,handle;
		int 80h;
		mov eof,ax;
	}
	return eof;
}

int mcos_fgetc(int handle)
{
	int c;
	asm {
		mov ah,82;
		mov bx,handle;
		int 80h;
		mov c,ax;
	}
	return c;
}

void mcos_putchar(char c)
{
	asm {
		mov ah,1;
		mov al,c;
		int 80h;
	}
}

char mcos_getchar()
{
	char r;
	asm {
		mov ah,4;
		int 80h;
		mov r,al;
	}
	return r;
}


unsigned mcos_getkey()
{
	unsigned r;
	asm {
		mov ah,65;
		int 80h;
		mov r,ax;
	}
	return r;
}

int mcos_alloc(unsigned size, unsigned *segm)
{
	unsigned r;
	asm {
		push es;
		push ds;
		pop es;
		mov ah,15;
		mov bx,size;
		mov dx,segm;
		int 80h;
		pop es;
		mov r,ax;
	}
	return r;
}

unsigned gethseconds()
{
	unsigned ticks;
	asm {
		mov ah, 00h;
		int 1Ah;
		mov ticks, dx;
	}
	ticks%=10000;
	ticks*=5;
	return ticks;
}


unsigned mcos_read(int handle, unsigned char far *buffer, unsigned len)
{
	unsigned r;
	asm {
		push es;
		mov bx,handle;
		mov cx,len;
		les dx,buffer;
		mov ah,89;
		int 80h;
		mov r,ax;
		pop es;
	}
	return r;

}

void far *farmalloc(unsigned s)
{
	unsigned seg;
	unsigned n = s/16 + (s%16 ? 1 : 0);
	if(mcos_alloc(n,&seg)) {
		return MK_FP(seg,0);
	}
	return MK_FP(0,0);
}

void farfree(void far *ptr)
{
}

void sound(unsigned freq)
{
	if(freq<19)
	{
		nosound();
		return;
	}
	asm {
		mov cx, freq;
		mov dx, 0x12;
		mov ax, 0x34DC;
		div cx;
		xchg ax, cx;

		mov dx, 0x43;
		mov al, 0xB6;
		out dx, al;
		
		mov dx, 0x42;
		mov al, cl;
		out dx, al;
		
		mov al, ch;
		out dx, al;
		
		mov dx, 0x61;
		in al, dx;
		
		or al, 0x3
		out dx, al;
	}
		
}
 
void nosound()
{
	asm {
		mov dx, 0x61;
		in al, dx;
		and al, 0xFC;
		out dx, al;
	}
}


/*
 SPECIAL
*/

char hextab[20] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','X','X','X','X'};

void writehex(unsigned d)
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
  mcos_print(t);
}


void writenum(unsigned d)
{
  char t[6];
  t[0] = (d/10000)%10+'0';
  t[1] = (d/1000)%10+'0';
  t[2] = (d/100)%10+'0';
  t[3] = (d/10)%10+'0';
  t[4] = d%10+'0';
  t[5] = '\0';

  mcos_print(t);
}


void printf(char *str,...)
{
  int st=0;
  int *ptr = (int *)&str;
  ptr++;
  while(*str) {
    switch(st) {
      case 0: if( *str=='%' ) {
                st=1;
              }
              else {
                mcos_putchar(*str);
              }
      break;
      case 1:
        switch(*str) {
          case '%': mcos_putchar('%'); break;
          case 'x':
          case 'X': writehex(*ptr); ptr++;
            break;
          case 'd':
          case 'i':
          case 'u': writenum(*ptr); ptr++;
            break;
          case 's': mcos_print((char *)*ptr); ptr++; break;
          case 'c': mcos_putchar(*ptr); ptr++; break;
         }
        st=0;
      break;
    }
    str++;
  }
}


unsigned strlen(char *s)
{
	unsigned i=0;
	while(s[i])
		i++;
	return i;
}

int strcmp(char *s1,char *s2)
{
	unsigned a,b;
	a=strlen(s1);
	b=strlen(s2);
	if(a!=b)
		return 1;
	a=0;
	while(s1[a])
	{
		if(s1[a]!=s2[a])
			return 1;
		a++;
	}
	return 0;
}

int strncmp(char *s1,char *s2, unsigned b)
{
	unsigned a;
	a=0;
	while(s1[a] && a<b)
	{
		if(s1[a]!=s2[a])
			return 1;
		a++;
	}
	return 0;
}

void strcpy(char *dest,char *or)
{
	unsigned x;
	x=0;
	do
	{
		dest[x]=or[x];
	}while(or[x++]);
}


void strncpy(char *dest,char *org, unsigned b)
{
	unsigned x;
	x=0;
	while(org[x] && x<b)
	{
		dest[x]=org[x];
		x++;
	};
}


void _fstrncpy(char far *dest,char far *org, unsigned b)
{
	unsigned x;
	x=0;
	while(org[x] && x<b)
	{
		dest[x]=org[x];
		x++;
	};
}

