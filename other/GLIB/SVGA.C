#include <stdio.h>
#include <dos.h>
#include <string.h>
#include <math.h>
//#include <rato2.c>
#include <stdlib.h>
//Testes ao sistema VESA (SUPER VGA)
#include "svga.h"

static unsigned long VESALastOffset=0L;
WORD VESAWidth=640;
WORD VESAHeight=480;
BYTE Color=15;
BYTE FillColor=0;
BYTE TextColor=15;
BYTE TextBackGround=0;
BYTE TextSize=8;

WORD last_x = 0;
WORD last_y = 0;
int LineWidth=1;

static TVBEMODE_INFO ModeInfo;

void dcircle(int x0, int y0, int r);

void (*drawpixel)(unsigned long offset, unsigned char color) = VESAPutByte;

void gsetwritemode(int m)
{
	if(m==XOR_MODE) {
		drawpixel=VESAPutXor;
		return;
	}

	drawpixel=VESAPutByte;
}

unsigned char far *getcpt()
{
	unsigned s;
	unsigned o;

	asm {
		push es;
		push bp;

		mov ax, 0x1130;
		mov bx, 0x0300;
		int 0x10;
		mov ax, es;
		mov bx, bp;
		pop bp;
		pop es;

		mov s, ax;
		mov o, bx;
	}


	return ((unsigned char far *) MK_FP(s,o));
}


char GetVbeInfo(TVBE_INFO far *ptr)
{
	struct REGPACK r;
	r.r_ax=0x4F00;
	r.r_es=FP_SEG(ptr);
	r.r_di=FP_OFF(ptr);
	intr(0x10,&r);
	if((r.r_ax==0x004F)) {
		return 1;
	}

	return 0;
}

int GetVbeModeInfo(WORD vmode, TVBEMODE_INFO far *ptr)
{
	struct REGPACK r;

	r.r_ax=0x4F01;
	r.r_cx=vmode;
	r.r_es=FP_SEG(ptr);
	r.r_di=FP_OFF(ptr);
	intr(0x10,&r);
	if((r.r_ax==0x004F))
		return 1;
	else
		return 0;
}


char SetVbeMode(WORD mode)
{
	struct REGPACK r;
	r.r_ax=0x4F02;
	r.r_bx=mode;
	intr(0x10,&r);
	if((r.r_ax==0x004F)) {
		GetVbeModeInfo(mode, &ModeInfo);
		VESAWidth  = ModeInfo.Width;
		VESAHeight = ModeInfo.Height;
		VESASetOffset(0);
		return 1;
	}

	return 0;
}

void BiosPutPixel(WORD x,WORD y,BYTE c)
{
	asm{
		mov ah,0x0C;
		mov bh,0;
		mov al,c;
		mov cx,x;
		mov dx,y;
		int 0x10;
	}
}

int VESASetOffset(int ofs)
{
	struct REGPACK r;
	r.r_ax=0x4F05;
	r.r_bx=0x0000;
	r.r_dx=ofs;
	intr(0x10,&r);
	if((r.r_ax==0x004F)) {
		VESALastOffset = ofs;
		return 1;
	}
	return 0;
}

void VESAPutByte(unsigned long ofs, unsigned char c)
{
	register unsigned x = ofs; // & 0xFFFFL;
	register unsigned b = ofs >> 16L;

	if (b!=VESALastOffset)  {
		if(!VESASetOffset(b)) {
			return;
		}
	}
	((char far *)0xA0000000)[x]=c;
}

void VESAPutXor(unsigned long ofs, unsigned char c)
{
	register unsigned x = ofs; // & 0xFFFFL;
	register unsigned b = ofs >> 16L;

	if (b!=VESALastOffset)  {
		if(!VESASetOffset(b)) {
			return;
		}
	}
	((char far *)0xA0000000)[x]^=c;
}

void VESACleardevice()
{
	gbar(0,0,VESAWidth-1, VESAHeight-1);
	last_x=0;
	last_y=0;
}

void VESAClosegraph()
{

}


void VESAHLine(unsigned long ofs1, unsigned long ofs2, unsigned char c)
{
	register unsigned x1 = ofs1; // & 0xFFFFL;
	register unsigned b1 = ofs1 >> 16L;

	register unsigned x2 = ofs2; // & 0xFFFFL;
	register unsigned b2 = ofs2 >> 16L;


	if (b1!=VESALastOffset)  {
		if(!VESASetOffset(b1)) {
			return;
		}
	}

	if (b1==b2) {
		_fmemset(MK_FP(0xA000,x1), c, x2-x1+1);
		return;
	}

	_fmemset(MK_FP(0xA000,x1), c, 0x10000-x1);

	if(!VESASetOffset(b2)) {
		return;
	}

	_fmemset(MK_FP(0xA000,0), c,  x2+1);
}

void vgetimage(int left, int top, int right, int bottom, GIMAGE far *bitmap)
{
	WORD width = right - left + 1;
	char far *ptr = (char far *)bitmap->i;

	bitmap->w=width;
	bitmap->h=bottom - top + 1;

	for( ;top<=bottom; top++)
	{
		WORD d;
		DWORD ofs1 = (DWORD)top*VESAWidth+left;
		DWORD ofs2 = (DWORD)top*VESAWidth+right;

		register unsigned x1 = ofs1; // & 0xFFFFL;
		register unsigned b1 = ofs1 >> 16L;

		register unsigned x2 = ofs2; // & 0xFFFFL;
		register unsigned b2 = ofs2 >> 16L;


		if (b1!=VESALastOffset)  {
			if(!VESASetOffset(b1)) {
				return;
			}
		}

		if (b1==b2) {
			_fmemcpy(ptr, MK_FP(0XA000, x1), width);
			ptr+=width;
			continue;
		}

		d=0x10000-x1;
		_fmemcpy(ptr, MK_FP(0xA000,x1), d);

		if(!VESASetOffset(b2)) {
			return;
		}

		_fmemcpy(ptr+d, MK_FP(0xA000,0), width-d);

		ptr+=width;

	}
}

void vputimage(int left, int top, GIMAGE far *bitmap)
{
	WORD width = bitmap->w;
	char far *ptr = (char far *)bitmap->i;
	WORD right  = left + width - 1;
	WORD bottom = top + bitmap->h;

	for( ;top<bottom; top++)
	{
		WORD d;
		DWORD ofs1 = (DWORD)top*VESAWidth+left;
		DWORD ofs2 = (DWORD)top*VESAWidth+right;

		register unsigned x1 = ofs1; // & 0xFFFFL;
		register unsigned b1 = ofs1 >> 16L;

		register unsigned x2 = ofs2; // & 0xFFFFL;
		register unsigned b2 = ofs2 >> 16L;

	 //	printf("OFFSET %lX\n", ofs1);

		if (b1!=VESALastOffset)  {
			if(!VESASetOffset(b1)) {
				return;
			}
		}

		if (b1==b2) {
			_fmemcpy(MK_FP(0XA000, x1), ptr, width);
			ptr+=width;
			continue;
		}

		d=0x10000-x1;
		_fmemcpy(MK_FP(0xA000,x1), ptr, d);

		if(!VESASetOffset(b2)) {
			return;
		}

		_fmemcpy(MK_FP(0xA000,0), ptr+d, width-d);

		ptr+=width;

	}

}

void drawbitmap(int left, int top, WORD bitmap[], int h, BYTE color)
{
	int i,j;
	for(i=0;i<h && top < VESAHeight; i++) {
		WORD a=bitmap[i];
		for(j=0;j<16;j++)
		{
			if(a&0x8000 && j+left<VESAWidth)
				VESAPutByte((unsigned long)top*VESAWidth+j+left, color);
			a<<=1;
		}
		top++;
	}
}

#define DrawPixel(x,y,c) {drawpixel((unsigned long)(y)*VESAWidth+(x), (c)); last_x = (x); last_y=(y);}


void VESAPutpixel(int x, int y, int c)
{
	DrawPixel(x,y,c);
}

void grectangle(int x1, int y1, int x2, int y2)
{
	gline(x1,y1,x2,y1);
	gline(x1,y1+1,x1,y2-1);
	gline(x2,y1+1,x2,y2-1);
	gline(x1,y2,x2,y2);
}

void gbar(int x1, int y1, int x2, int y2)
{
	int y;
	for(y=y1;y<=y2;y++)
	{
		VESAHLine((unsigned long)y*VESAWidth+x1, (unsigned long)y*VESAWidth+x2, FillColor);
	}
}

void gcbar(int x1, int y1, int x2, int y2, BYTE color)
{
	int y;
	for(y=y1;y<=y2;y++)
	{
		VESAHLine((unsigned long)y*VESAWidth+x1, (unsigned long)y*VESAWidth+x2, color);
	}
}


void dline(int x1, int y1, int x2, int y2)
{
	int dx,dy,d,y,x;
	int ax,ay;
	int sx,sy;

	dx = x2-x1;
	dy = y2-y1;

	sx = (x2<x1?-1:1);
	sy = (y2<y1?-1:1);
	ax = 2*(dx<0?-dx:dx);
	ay = 2*(dy<0?-dy:dy);

	x=x1;
	y=y1;
	if( ax >= ay ) {
		d=ay-ax/2;
		while(1)
		{
			DrawPixel(x,y,Color);
			if(x==x2) break;
			if(d>=0) {
				y=y+sy;
				d=d-ax;
			}
			d=d+ay;
			x=x+sx;
		}
	}
	else {
		d=ax-ay/2;
		while(1)
		{
			DrawPixel(x,y,Color);
			if(y==y2) break;
			if(d>=0) {
				x=x+sx;
				d=d-ay;
			}
			d=d+ax;
			y=y+sy;
		}
	}

}

void thick_line(int x1, int y1, int x2, int y2, int w)
{
	int dx,dy,d,y,x;
	int ax,ay;
	int sx,sy;
	int ks=-w/2;
	int kf=(w/2)+w%2;

	dx = x2-x1;
	dy = y2-y1;

	sx = (x2<x1?-1:1);
	sy = (y2<y1?-1:1);
	ax = 2*(dx<0?-dx:dx);
	ay = 2*(dy<0?-dy:dy);

	x=x1;
	y=y1;
	if( ax >= ay ) {
		d=ay-ax/2;
		for(;;)
		{
			int k;
			for(k=ks;k<kf;k++)
				DrawPixel(x,y+k,Color);
			if(x==x2) break;
			if(d>=0) {
				y=y+sy;
				d=d-ax;
			}
			d=d+ay;
			x=x+sx;
		}
	}
	else {
		d=ax-ay/2;
		for(;;)
		{
			int k;
			for(k=ks;k<kf;k++)
				DrawPixel(x+k,y,Color);
			if(y==y2) break;
			if(d>=0) {
				x=x+sx;
				d=d-ay;
			}
			d=d+ax;
			y=y+sy;
		}
	}


}


void gline(int x1, int y1, int x2, int y2)
{
	if(LineWidth<=1)
		dline(x1,y1,x2,y2);
	else
		thick_line(x1,y1,x2,y2,LineWidth);
}

void dcircle(int x0, int y0, int r)
{
	int x = r - 1;
	int y = 0;
	int dx = 1;
	int dy = 1;
	int err = dx - (r<<1);

	while(x>=y)
	{
		DrawPixel(x0+x,y0+y,Color);
		DrawPixel(x0+x,y0-y,Color);
		DrawPixel(x0-x,y0+y,Color);
		DrawPixel(x0-x,y0-y,Color);

		DrawPixel(x0+y,y0+x,Color);
		DrawPixel(x0+y,y0-x,Color);
		DrawPixel(x0-y,y0+x,Color);
		DrawPixel(x0-y,y0-x,Color);

		if(err<=0)
		{
			y++;
			err+=dy;
			dy+=2;
		}
		if(err>0)
		{
			x--;
			dx+=2;
			err += dx  - (r<<1);
		}
	}
}

void garc(int x, int y, int stangle, int endangle, int radius)
{
	int c1,c2;
	int r2=radius-1;
	double alfa=stangle*M_PI/180.0;
	double final=endangle*M_PI/180.0;
	double inca;
	int px,py;
	char cor;

	/*
	if (alfa>final)
		alfa+=M_PI*2.0;
		*/
	if ((radius==0)||(radius>180))
	{
		DrawPixel(x,y,Color);
		return;
	}
	inca=M_PI/(M_PI*radius);

/*	if(op!=1)
	{
		for(c1=x-r2;c1<=x+r2;c1++)
		for(c2=y-r2;c2<=y+r2;c2++)
		{
			if((long double)((x-c1)*(x-c1))+((y-c2)*(y-c2))<(long double)(r*r)-r)
				DrawPixel(c1,c2,FillColor);
		}
	}
*/
	cor=Color;
	for(;alfa<=final;alfa+=inca)
	{
		px=cos(alfa)*radius;
		py=sin(alfa)*radius;
		DrawPixel(x+px,y-py,cor);
/*		DrawPixel(x-px,y-py,cor);
		DrawPixel(x-px,y+py,cor);
		DrawPixel(x+px,y-py,cor); */
	}
}



unsigned char far *cart=0L;

void pchar2(int x,int y, unsigned char c)
{
	int cx,cy;
	unsigned char v;
	if(!cart)
		cart=getcpt();
	for(cy=0;cy<8;cy++)
	{
		v=cart[8*c+cy];
		for(cx=0;cx<8;cx++)
		{
			float rcx=(cx)*TextSize/8.0+x;
			float rcy=(cy)*TextSize/8.0+y;
			float ncx=(cx+1)*TextSize/8.0+x;
			float ncy=(cy+1)*TextSize/8.0+y;

			if((v&128)==128) {
				gcbar(rcx,rcy, ncx, ncy, TextColor);
			}
			else
			if(TextBackGround!=255) {
				gcbar(rcx,rcy,ncx, ncy, TextBackGround);
			}
			v=v<<1;
		}
	}
}

void pchar(int x,int y, unsigned char c)
{
	int cx,cy;
	unsigned char v;
	if(!cart)
		cart=getcpt();
	for(cy=0;cy<8;cy++)
	{
		v=cart[8*c+cy];
		for(cx=0;cx<8;cx++)
		{

			if((v&128)==128) {
				DrawPixel(x+cx,y+cy,TextColor);
			}
			else
			if(TextBackGround!=255) {
				DrawPixel(x+cx,y+cy,TextBackGround);
			}
			v=v<<1;
		}
	}
}


void writest(int x, int y, char *st)
{
	while(*st)
	{
		if(TextSize==8)
			pchar(x,y,*st);
		else
			pchar2(x,y,*st);
		st++;
		x+=TextSize;
	}
}

void glineto(int x, int y)
{
	gline(last_x, last_y, x, y);
}

void VESASetbkcolor(int color)
{
	FillColor=color;
}

void VESASetcolor(int color)
{
	TextColor=color;
	Color=color;
}

void VESASetfillpattern(char *upattern, int color)
{
	FillColor=color;
}

void VESASetfillstyle(int pattern, int color)
{
	FillColor=color;
}

void VESASetlinestyle(int linestyle, unsigned upattern, int thickness)
{
	LineWidth=thickness;
}

void VESASettextstyle(int font, int direction, int charsize)
{
	if(charsize==0)
		TextSize=8;
	else
		TextSize=charsize*8;
}

int VESATextheight(char *str)
{
	return TextSize;
}

int VESATextwidth(char *str)
{
	int size=0;
	while(*str++)
		size+=TextSize;
	return size;
}

int ggetmaxx()
{
	return VESAWidth-1;
}

int ggetmaxy()
{
	return VESAHeight-1;
}
