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
	printf("%x %x %x\n", _AH, _BH, _BL );
}

volatile int mousex = 320;
volatile int mousey = 240;

volatile char arrow[400];
volatile char cursor[400];
volatile char hasimage = 0;

volatile int bgx,bgy,bgw,bgh;


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
	#define DV 10
	if(hasimage)
	{
		drawbitmap(bgx,bgy,pointer,16);
	}
	printf("HERE\n");
	mousex+=dx;
	mousey-=dy;

	if(mousex<0)
		mousex=0;
	if(mousex>8000)
		mousex=8000;
	if(mousey<0)
		mousey=0;
	if(mousey>6000)
		mousey=6000;

	bgx=mousex/DV;
	bgy=mousey/DV;
	if(bgx<0)
		bgx=0;
	if(bgy<0)
		bgy=0;
 /*	bgw=bgx+15;
	bgh=bgy+15;
	if(bgw>639)
		bgw=639;
	if(bgh>479)
		bgh=479;
*/
	hasimage=1;

	drawbitmap(bgx,bgy,pointer,16);
}

int MouseOpen()
{
	int a = set_mouse_handler(handler);
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

/*
int main()
{
	unsigned char a;
	// request auto detection
	int gdriver = DETECT, gmode, errorcode;

	// initialize graphics mode
	initgraph(&gdriver, &gmode, "/tc/bgi");

	// read result of initialization
	errorcode = graphresult();

	if (errorcode != grOk)  // an error occurred //
	{
		printf("Graphics error: %s\n", grapherrormsg(errorcode));
		printf("Press any key to halt:");
		getch();
		exit(1);             // return with error code //
	}
	setcolor(14);
	line(0,0,10,10);
	line(0,0,0,12);
	line(0,12,4,10);
	line(10,10,6,10);
	line(4,10,7,14);
	line(6,10,9,14);
	line(7,14,9,14);
	setfillstyle(SOLID_FILL,15);
	floodfill(1,3,14);
	getimage(0,0,14,14,cursor);
	setfillstyle(SOLID_FILL,1);
	bar(0,0,639,749);
	a = set_mouse_handler(handler);
	printf("R: %X\n",a);
	reset_mouse();
	enable_mouse();
	getchar();
	disable_mouse();
	set_mouse_handler(MK_FP(0,0));
	return 0;
}
*/