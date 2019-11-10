#include "mcoslib.h"

void GetDate(TDATA far *data)
{
	unsigned char a,m,d,s;
	j:
	asm  mov ah,0x04;
	asm  int 0x1A;
	asm  mov s,ch;
	asm  mov a,cl;
	asm  mov m,dh;
	asm  mov d,dl;
	asm  jc j;
	data->ano=bcd2bin(a)+bcd2bin(s)*100;
	data->mes=bcd2bin(m);
	data->dia=bcd2bin(d);

}

void GetHora(THORA far *hora)
{
	unsigned char h,m,s;
	j:
	asm  mov ah,0x02;
	asm  int 0x1A;
	asm  mov h,ch;
	asm  mov m,cl;
	asm  mov s,dh;
	asm  jc j;
	hora->hora=bcd2bin(h);
	hora->minutos=bcd2bin(m);
	hora->segundos=bcd2bin(s);
	hora->centesimos=0;
}

unsigned char bcd2bin(unsigned char bcd)
{
	unsigned char a,b;
	a=bcd&0x0F;
	b=(bcd&0xF0)>>4;
	return a+b*10;
}

unsigned char bin2bcd(unsigned char bin)
{
	unsigned char a,b;
	a=(bin%10)&0x0F;
	b=bin/10;
	b=(b&0x0F)<<4;
	return a|b;
}

unsigned long power(unsigned long n,unsigned p)
{
	unsigned long l;
	unsigned i;
	l=1;
	for(i=0;i<p;i++)
	{
		l=longmul2(l,n);
	}
	return l;
}

void _ultoa(unsigned long value, char far *string,char m,unsigned t)
{
	unsigned d,i;

	string[0]='\0';
	while(value>=10)
	{
		insc(string,'0'+value%10,0);
		value=value/10;
	}

	insc(string,'0'+value%10,0);

	d=Strlenx(string);

	if(t<d) t=d;
	switch(m)
	{
		case 1: for(i=d;i<t;i++) insc(string,' ',Strlenx(string));
			break;
		case 2: for(i=d;i<t;i++) insc(string,'0',0);
			break;
		case 3: for(i=d;i<t;i++) insc(string,' ',0);
			break;

	}

}



unsigned GetKey()
{
	unsigned k;
	asm{
		mov ah,0;
		int 0x16;
		mov k,ax;
	}
	return k;
}

unsigned Strlen(char far *s)
{
	unsigned i=0;
	while(s[i])
		i++;
	return i;
}

int Strcmp(char far *s1,char far *s2)
{
	unsigned a,b;
	a=Strlen(s1);
	b=Strlen(s2);
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

int Strncmp(char far *s1,char far *s2, unsigned b)
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


void Strcpyx(char far *dest,char far *or)
{
	unsigned x;
	x=0;
	do
	{
		dest[x]=or[x];
	}while(or[x++]);
}

void Strcpy(char far *dest,char far *or)
{
	unsigned x;
	x=0;
	do
	{
		dest[x]=or[x];
	}while(or[x++]);
}

unsigned Strlenx(char far *s)
{
	unsigned i=0;
	while(s[i])
		i++;
	return i;
}

int Strcmpx(char far *s1,char far *s2)
{
	unsigned a,b;
	a=Strlenx(s1);
	b=Strlenx(s2);
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


// implementacao das rotinas
void dels(char far *st,unsigned p,unsigned t)
{
	unsigned c;
	if(p+t>Strlenx(st))
		return;
	c=p;
	while(st[c+t])
	{
		st[c]=st[c+t];
		c++;
	}
	st[c]=0;
}

void ins(char far *st,char far *is,unsigned p)
{
	unsigned c,c2,t=Strlenx(is);
	unsigned t2=Strlenx(st);
	if(p>t2) return;
	c2=t2-p;
	for(c=0;c<=c2;c++)
	{
		st[t2-c+t]=st[t2-c];
	}
	c=0;
	while(is[c])
	{
		st[p+c]=is[c];
		c++;
	}
}

void insc(char far *st,char ic,unsigned p)
{
	unsigned c,c2;
	unsigned t2=Strlenx(st);
	if(p>t2) return;
	c2=t2-p;
	for(c=0;c<=c2;c++)
	{
		st[t2-c+1]=st[t2-c];
	}
	c=0;
	st[p]=ic;
}



/**********************************************************************************************/

void kprintf(char *str,...)
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
                putchar(*str);
              }
      break;
      case 1:
        switch(*str) {
          case '%': putchar('%'); break;
          case 'x':
          case 'X': writehex(*ptr); ptr++;
            break;
          case 'd':
          case 'i':
          case 'u': writenum(*ptr); ptr++;
            break;
          case 'c': putchar(*ptr); ptr++;
			break;
	  case 's': writestr((char *)*ptr); ptr++; break;
	  case 'S':
		writestr((char far *)MK_FP(*(ptr+1),*ptr)); ptr++; ptr++;
		break;
	  case 'l': st=2; break;
        }
	if (st==1) st=0;
      break;
      case 2:
      	switch(*str) {
	case 'x':
	case 'X': writehex(*(ptr+1)); writehex(*ptr);
		ptr++; ptr++;
		break;
	case 'd':
	case 'i':
	case 'u':writelongnum(*ptr);
		ptr++; ptr++;
		break;
	}
	st=0;
      break;
    }
    str++;
  }
}

void writestr(char far *str)
{
  while(*str)
    putchar(*str++);
}

void putchar(char l)
{
  asm {
     mov ah,0eh;
     mov al,l;
     mov bh,0;
     mov bl,7;
     int 10h;
  };
}

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
  writestr(t);
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

  writestr(t);
}


void writelongnum(unsigned long d)
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

  writestr(t);
}


unsigned long longmul(unsigned long a, unsigned long b)
{
  unsigned long rh1,rh2,rh3;
  unsigned h1,h2,h3;

  unsigned lo1 = a & 0xFFFF;
  unsigned lo2 = b & 0xFFFF;

  unsigned hi1 = (a >> 16) & 0xFFFF;
  unsigned hi2 = (a >> 16) & 0xFFFF;

  rh1  = hi1  * lo2;
  rh1 &= 0xFFFF;
  h1 = rh1;

  rh2  = hi2  * lo1;
  rh2 &= 0xFFFF;
  h2 = rh2;


  rh3  = lo1 * lo2;
  rh3 &= 0xFFFF0000;

  h3 = (rh3>>16) + h2 + h1;

  return (h3<<16) + (lo1*lo2) & 0xFFFF;
}

unsigned long longmul2(unsigned long a, unsigned long b)
{
  unsigned long ac=0;
  while(b) {
    if(b&1)
      ac+=a;
    a=a<<1;
    b=b>>1;
  }
  return ac;
}


/*
TODO
if D = 0 then error(DivisionByZeroException) end
Q := 0                  -- Initialize quotient and remainder to zero
R := 0                     
for i := n − 1 .. 0 do  -- Where n is number of bits in N
  R := R << 1           -- Left-shift R by 1 bit
  R(0) := N(i)          -- Set the least-significant bit of R equal to bit i of the numerator
  if R ≥ D then
    R := R − D
    Q(i) := 1
  end
end
*/

/************************************* DEBUG *******************************/
void writehex2(unsigned d, unsigned s, char pad)
{
  char t[5];
  int i;
  char *st = t;
  t[0] = ((d & 0xF000)>>12);
  t[1] = ((d & 0x0F00)>>8);
  t[2] = ((d & 0x00F0)>>4);
  t[3] = (d & 0x000F);
  t[4] = '\0';

  t[0]=hextab[t[0]];
  t[1]=hextab[t[1]];
  t[2]=hextab[t[2]];
  t[3]=hextab[t[3]];

  for(i=0;i<4;i++) {
    if(t[i]!='0')
      break;
    if(i<4-s)
      st++;
    t[i]=pad;
  }
  writestr(st);
}


void dump_mem2(unsigned seg, unsigned offset, unsigned len)
{
  unsigned char far *fp = (unsigned far *)MK_FP(seg, offset);
  unsigned i;
  for(i=0;i<len;i++)
  {
    writehex2(fp[i],2,'0');
    writestr(" ");
  }
  writestr("\r\n");
}

void _fmemcpy(void far *a, void far *b, unsigned s)
{
	asm {
		push ds;
		les di, a;
		lds si, b;
		mov cx, s;
		cld;
		rep movsb;
		pop ds;
	}
}

void _fmemset(void far *a, int c, unsigned s)
{
	asm {
		mov al, byte ptr c;
		les di, a;
		mov cx, s;
		cld;
		rep stosb;
	}
}
