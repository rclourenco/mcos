#include <stdio.h>
#include <dos.h>
#include <string.h>
#include <math.h>
//#include <rato2.c>
#include <stdlib.h>
//Testes ao sistema VESA (SUPER VGA)
#include "svga.h"

static unsigned long VESALastOffset=0L;
static WORD VESAWidth=640;

static TVBEMODE_INFO ModeInfo;

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
	printf("Ptr: %Fp\n", ptr);

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
		VESAWidth = ModeInfo.Width;
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
       //	printf("Bottom %d Right %d\n", bottom, right);
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
	   //		_fmemset(MK_FP(0xA000,x1), 0, width);
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

void drawbitmap(int left, int top, WORD bitmap[], int h)
{
	int i,j;
	for(i=0;i<h;i++) {
		WORD a=bitmap[i];
		for(j=0;j<16;j++)
		{
			if(a&0x8000 && j+left<VESAWidth)
				VESAPutXor(top*VESAWidth+j+left, 255);
			a<<=1;
		}
		top++;
	}
}