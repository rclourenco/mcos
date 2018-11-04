#include <stdio.h>
#include <dos.h>
#include <graphics.h>
#include <conio.h>
#include "svga.h"
#include "mouse.h"

unsigned char set_mouse_handler(void far *fp)
{
	_AX = 0xc207;
	_ES = FP_SEG(fp);
	_BX = FP_OFF(fp);
	geninterrupt(0x15);
	return _AH;
}

unsigned char enable_mouse()
{
	_AX = 0xc200;
	_BH = 0x1;
	geninterrupt(0x15);
	return _AH;
}

unsigned char disable_mouse()
{
	_AX = 0xc200;
	_BH = 0x1;
	geninterrupt(0x15);
	return _AH;
}

void reset_mouse()
{
	_AX = 0xc201;
	geninterrupt(0x15);
//	printf("%x %x %x\n", _AH, _BH, _BL );
}

#define DV 2

volatile int mousex = 320;
volatile int mousey = 240;

volatile char arrow[400];
volatile char cursor[400];
volatile char hasimage = 0;

volatile int bgx=0,bgy=0,obgx=0,obgy=0;

volatile WORD mouse_status;
volatile WORD mouse_hide=1;

char mouse_background[16*16+sizeof(GIMAGE)];

static WORD pointer_a[16]={
		0xC000,    // xxx
		0xE000,    // x#x
		0xF000,    // x##x
		0xF800,    // x###x
		0xFC00,    // x####x
		0xFE00,    // x#####x
		0xFF00,    // x######x
		0xFF80,    // x#######x
		0xFFC0,    // x########x
		0xFFE0,    // x#########x
		0xFFF0,    // x######xxxx
		0xFF00,    //  ###x##
		0xF780,    //  ##   ##
		0xE780,    //  #    ##
		0x43C0,    //        ##
		0x0180};   //


static WORD pointer[16]={
		0x0000,    // xxx
		0x4000,    // x#x
		0x6000,    // x##x
		0x7000,    // x###x
		0x7800,    // x####x
		0x7C00,    // x#####x
		0x7E00,    // x######x
		0x7F00,    // x#######x
		0x7F80,    // x########x
		0x7FC0,    // x#########x
		0x7E00,    // x######xxxx
		0x7600,    //  ###x##
		0x6300,    //  ##   ##
		0x4300,    //  #    ##
		0x0180,    //        ##
		0x0000};   //

void far handler(unsigned a, unsigned b, unsigned c, unsigned d)
{
	char dx=c;
	char dy=b;

	mouse_status = d;

	if( (d&0xA0) == 0xA0)
		dy=0x80;
	else if(d&0x80) dy=0x7F;

	if( (d&0x50) == 0x50)
		dx=0x80;
	else if(d&0x40)
		dx=0x7F;

	mousex+=dx;
	mousey-=dy;

	if(mousex<0)
		mousex=0;
	if(mousex>VESAWidth*DV-1)
		mousex=VESAWidth*DV-1;
	if(mousey<0)
		mousey=0;
	if(mousey>VESAHeight*DV-1)
		mousey=VESAHeight*DV-1;

	obgx=bgx;
	obgy=bgy;

	bgx=mousex/DV;
	bgy=mousey/DV;
	if(bgx<0)
		bgx=0;
	if(bgy<0)
		bgy=0;

	if(mouse_hide)
		return;

	if(hasimage)
	{
		vputimage(obgx, obgy, (GIMAGE *)mouse_background);
	}

	hasimage=1;
	vgetimage(bgx, bgy, bgx+15, bgy+15, (GIMAGE *)mouse_background);

	drawbitmap(bgx, bgy, pointer_a, 16, 0);
	drawbitmap(bgx,bgy,pointer, 16, 15);
}

int MouseOpen()
{
	int a;
	bgx=VESAWidth/2;
	bgy=VESAHeight/2;
	obgx=bgx;
	obgy=bgy;
	mousex=bgx*DV;
	mousey=bgy*DV;

	a = set_mouse_handler(handler);
	reset_mouse();
	enable_mouse();
	return a;
}

int MouseClose()
{
	set_mouse_handler(MK_FP(0,0));
	disable_mouse();

	return 1;
}

int MouseX() {
	return bgx;
}

int MouseY() {
	return bgy;
}

int MouseStatus() {
	 return mouse_status&0xFF;
}

void MouseGet(int *x, int *y, int *b)
{
	*x=bgx;
	*y=bgy;
	*b=mouse_status&0x3;
}

void MouseShow()
{
	if(mouse_hide>1)
	{
		return;
	}
	asm cli;
	hasimage=1;
	vgetimage(bgx, bgy, bgx+15, bgy+15, (GIMAGE *)mouse_background);

	drawbitmap(bgx, bgy, pointer_a, 16, 0);
	drawbitmap(bgx,bgy,pointer, 16, 15);
	mouse_hide--;
	asm sti;
}

void MouseHide()
{
	asm cli;
	if(hasimage)
	{
		vputimage(bgx, bgy, (GIMAGE *)mouse_background);
		hasimage=0;
	}
	mouse_hide++;
	asm sti;
}