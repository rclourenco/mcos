/*
	Biblioteca programada por Renato Louren‡o.
*/
//biblioteca para o modo grafico 13h

#include <dos.h>
#include <math.h>
#include <mem.h>

//defini‡äes de desenho
#define LIFI 0
#define LINE 1
#define FILL 2
#define COPY_D 0
#define XOR_D 1
#define AND_D 2
#define OR_D 3

#define PLOT(X,Y,C) screen[(X)+(Y)*320]=C
#define GETPLOT(X,Y) screen[(X)+(Y)*320]
#define PI M_PI

#define DEFAULT 0
#define USER 1

void pchar(int x,int y, unsigned char c);
void writest(int x,int y,unsigned char *st);
void fillscreen(char a);
void swap(int *a,int *b);
unsigned char DRAW=COPY_D;
unsigned char Color=15;
unsigned char FillColor=15;
unsigned char TextColor=7;
unsigned char TextBackGround=255;
unsigned char far *dscreen=(unsigned char far *)MK_FP(0xA000,0x0);
unsigned char far *screen=(unsigned char far *)MK_FP(0xA000,0x0);

//instru‡äes graficas de interface com a bios
typedef unsigned char far *CPT;
void modo13h();//modo grafico
void modo3h();//modo texto
unsigned char far *getcpt();//retorna a posicao do mapa de caracteres
void SetOverScan(char c);
void SetDacRegs(unsigned a,unsigned char r,unsigned char g,unsigned char b);
void GetDacRegs(unsigned a,unsigned char *r,unsigned char *g,unsigned char *b);


//instru‡äes de desenho
void DrawPixel(int x,int y,char c);
void DrawRect(int x1,int y1,int x2,int y2,char op);
void DrawCircle(int x,int y,int r,char op);
void DrawLine(int x1,int y1,int x2,int y2);

//instru‡äes para manipula‡Æo de bitmaps
unsigned PicSize(int x1,int y1,int x2,int y2);
void GetPic(int x1,int y1,int x2,int y2,unsigned char far *pic);
void DrawPic(int x1,int y1,unsigned char far *pic);
void DrawPicB(int x1,int y1,unsigned char far *pic);
void DrawPicF(int x1,int y1,unsigned char far *pic);
int XPRes(unsigned char far *pic);
int YPRes(unsigned char far *pic);

//instru‡äes para ecran virtual
char ScrnT=DEFAULT;
void DefaultScreen();
void UserScreen(unsigned char far *s);
char ScrnCpyUD();
char ScrnCpyDU();

//implementa‡Æo

void modo13h()
{
	asm mov ax,0x13;
	asm int 0x10;
}

void modo3h()
{
	asm mov ax,0x03;
	asm int 0x10;
}

unsigned char far *getcpt()
{
	unsigned seg, ofs;
	asm {
		push es;
		push bp;
		mov ax, 0x1130;
		mov bx, 0x0300;
		int 0x10;
		mov seg, es;
		mov ofs, bp;
		pop bp;
		pop es;
	}
	return ((unsigned char far *) MK_FP(seg,ofs));
}

void pchar(int x,int y, unsigned char c)
{
	int cx,cy;
	unsigned char v;
	unsigned char far *cart;
	cart=getcpt();
	for(cy=0;cy<8;cy++)
	{
		v=cart[8*c+cy];
		for(cx=0;cx<8;cx++)
		{
			if((v&128)==128)
				PLOT(x+cx,y+cy,TextColor);
			else
			if(TextBackGround!=255)
				PLOT(x+cx,y+cy,TextBackGround);
			v=v<<1;
		}
	}
}

void writest(int x,int y,unsigned char *st)
{
	while(*st)
	{
		pchar(x,y,*st);
		st++;
		x+=8;
	}
}

void fillscreen(char a)
{
	_fmemset(screen,a,64000);
}

//instru‡äes de desenho
void DrawRect(int x1,int y1,int x2,int y2,char op)
{
	int x,y;
	int cor;
	if(x1>x2) swap(&x1,&x2);
	if(y1>y2) swap(&y1,&y2);
	if(op!=1)
	{
		for(x=x1+1;x<x2;x++)
		for(y=y1+1;y<y2;y++)
			DrawPixel(x,y,FillColor);
	}
	if(op==2)
		cor=FillColor;
	else
		cor=Color;
	for(x=x1;x<=x2;x++)
	{
		DrawPixel(x,y1,cor);
		DrawPixel(x,y2,cor);
	}
	for(y=y1+1;y<y2;y++)
	{
		DrawPixel(x1,y,cor);
		DrawPixel(x2,y,cor);
	}
}

void DrawCircle(int x,int y,int r,char op)
{
	int c1,c2;
	int r2=r-1;
	double alfa;
	double inca;
	int px,py;
	char cor;
	if ((r==0)||(r>180))
	{
		DrawPixel(x,y,Color);
		return;
	}
	inca=PI/(PI*r);
	if(op!=1)
	{
		for(c1=x-r2;c1<=x+r2;c1++)
		for(c2=y-r2;c2<=y+r2;c2++)
		{
			if((long double)((x-c1)*(x-c1))+((y-c2)*(y-c2))<(long double)(r*r)-r)
				DrawPixel(c1,c2,FillColor);
		}
	}
	if(op==2)
		cor=FillColor;
	else
		cor=Color;
	for(alfa=0;alfa<PI/2;alfa+=inca)
	{
		px=cos(alfa)*r;
		py=sin(alfa)*r;
		DrawPixel(x+px,y+py,cor);
		DrawPixel(x-px,y-py,cor);
		DrawPixel(x-px,y+py,cor);
		DrawPixel(x+px,y-py,cor);
	}
}

void DrawPixel(int x,int y,char c)
{
	if ((x>=0)&&(y>=0)&&(x<320)&&(y<200))
	{
		switch (DRAW)
		{
			case XOR_D:PLOT(x,y,c^GETPLOT(x,y));break;
			case AND_D:PLOT(x,y,c&GETPLOT(x,y));break;
			case OR_D:PLOT(x,y,c|GETPLOT(x,y));break;
			default:PLOT(x,y,c);
		}

	}

}

void DrawLine(int x1,int y1,int x2,int y2)
{
	long vx,vy;
	int x,y;
	double a,nv;
	vx=x2-x1;
	vy=y2-y1;
	nv=(double)sqrt((vx*vx)+(vy*vy));
	if(nv!=0)
	for(a=0;a<=1;a+=(1/nv))
	{
		x=x1+a*vx+.5;
		y=y1+a*vy+.5;
		DrawPixel(x,y,Color);
	}
	DrawPixel(x2,y2,Color);
}

unsigned PicSize(int x1,int y1,int x2,int y2)
{
	int x,y;
	if (x1>x2) swap(&x1,&x2);
	if (y1>y2) swap(&y1,&y2);
	x=x2-x1+1;
	y=y2-y1+1;
	if((x>320)&&(y>200))
		return 0;
	return x*y+10;
}

void swap(int *a,int *b)
{
	int temp;
	temp=*a;
	*a=*b;
	*b=temp;
}

void GetPic(int x1,int y1,int x2,int y2,unsigned char far *pic)
{
	int x,y;
	int cy,cx;
	int far*ip;
	if (x1>x2) swap(&x1,&x2);
	if (y1>y2) swap(&y1,&y2);
	x=x2-x1+1;
	y=y2-y1+1;
	ip=(int far*)pic;
	ip[0]=x;
	ip[1]=y;
	pic+=4;
	for(cy=y1;cy<=y2;cy++)
	for(cx=x1;cx<=x2;cx++)
	{
		*pic=GETPLOT(cx,cy);
		pic++;
	}
}

int XPRes(unsigned char far *pic)
{
	int far*ip=(int far*)pic;
	return ip[0];
}

int YPRes(unsigned char far *pic)
{
	int far*ip=(int far*)pic;
	return ip[1];
}

void DrawPicF(int x1,int y1,unsigned char far *pic)
{
	int far *ip=(int far *)pic;
	int x2=ip[0];
	int y2=ip[1];
	int py,y;
	pic+=4;

	y=0;

	asm mov cx,y2;
	l:
	asm mov y,cx;
	asm push cx;
	asm mov bx,cx;
	asm mov ax,y2
	asm sub ax,bx;
	asm add ax,y1;
	asm mov py,ax;
	py=py*320;
	_fmemcpy(MK_FP(screen,x1+py),pic,x2);
	pic+=x2;
	asm pop cx;
	asm loop l;

}


void DrawPic(int x1,int y1,unsigned char far *pic)
{
	int far *ip=(int far *)pic;
	int x2=x1+ip[0]-1;
	int y2=y1+ip[1]-1;
	int x,y;
	pic+=4;
	for(y=y1;y<=y2;y++)
	for(x=x1;x<=x2;x++)
	{
		if (*pic!=0xFF) PLOT(x,y,*pic);
		pic++;
	}

}

void DrawPicB(int x1,int y1,unsigned char far *pic)
{
	int far *ip=(int far *)pic;
	int x2=x1+ip[0]-1;
	int y2=y1+ip[1]-1;
	int x,y;
	pic+=4;
	for(y=y1;y<=y2;y++)
	for(x=x1;x<=x2;x++)
	{
		if (*pic!=0xFF) DrawPixel(x,y,*pic);
		pic++;
	}
}

void DefaultScreen()
{
	ScrnT=DEFAULT;
	screen=dscreen;
}

void UserScreen(unsigned char far *s)
{
	ScrnT=USER;
	screen=s;
}

char ScrnCpyUD()
{
	if (ScrnT!=USER) return 0;
	_fmemcpy(dscreen,screen,64000);
	return 1;
}

char ScrnCpyDU()
{
	if (ScrnT!=USER) return 0;
	_fmemcpy(screen,dscreen,64000);
	return 1;
}

void SetOverScan(char c)
{
	struct REGPACK regs;
	regs.r_ax=0x1001;
	regs.r_bx=c<<8;
	intr(0x10,&regs);
}

void SetDacRegs(unsigned a,unsigned char r,unsigned char g,unsigned char b)
{
	struct REGPACK regs;
	regs.r_ax=0x1010;
	regs.r_bx=a;
	regs.r_cx=(g<<8)+b;
	regs.r_dx=r<<8;
	intr(0x10,&regs);
}

void GetDacRegs(unsigned a,unsigned char *r,unsigned char *g,unsigned char *b)
{
	struct REGPACK regs;
	regs.r_ax=0x1015;
	regs.r_bx=a;
	intr(0x10,&regs);
	*r=regs.r_dx>>8;
	*g=regs.r_cx>>8;
	*b=(regs.r_cx<<8)>>8;
}