#include "graphsys.h"
#include <graphics.h>
#include <dos.h>

#include "mouse.h"
#include "svga.h"

void dmouse_show()
{
	struct REGPACK reg;
	reg.r_ax=0x1;
	intr(0x33,&reg);
}

void dmouse_hide()
{
	struct REGPACK reg;
	reg.r_ax=0x2;
	intr(0x33,&reg);

}

void dmouse_get(int *x, int *y, int *b)
{
	struct REGPACK reg;
	reg.r_ax=0x3;
	intr(0x33,&reg);
	*x=reg.r_cx;
	*y=reg.r_dx;
	*b=reg.r_bx;
}

void bgi_line(int x1, int y1, int x2, int y2)
{
	line(x1, y1, x2, y2);
}

void bgi_rectangle(int x1, int y1, int x2, int y2)
{
	rectangle(x1, y1, x2, y2);
}

void bgi_putpixel(int x, int y, int c)
{
	putpixel(x, y, c);
}

void bgi_arc(int x, int y, int stangle, int endangle, int radius)
{
	arc(x, y, stangle, endangle, radius);
}

void bgi_bar(int left, int top, int right, int bottom)
{
	bar(left, top, right, bottom);
}

void bgi_bar3d(int left, int top, int right, int bottom, int depth, int topflag)
{
	bar3d(left, top, right, bottom, depth, topflag);
}

void bgi_cleardevice(void)
{
	cleardevice();
}

void bgi_closegraph(void)
{
	closegraph();
}

int bgi_getmaxx(void)
{
	return getmaxx();
}

int bgi_getmaxy(void)
{
	return getmaxy();
}


void bgi_outtextxy(int x, int y, char *textstring)
{
	outtextxy(x,y,textstring);
}

void bgi_setbkcolor(int color)
{
	setbkcolor(color);
}

void bgi_setcolor(int color)
{
	setcolor(color);
}

void bgi_setfillpattern(char *upattern, int color)
{
	setfillpattern(upattern, color);
}

void bgi_setfillstyle(int pattern, int color)
{
	setfillstyle(pattern, color);
}

void bgi_setlinestyle(int linestyle, unsigned upattern, int thickness)
{
	setlinestyle(linestyle, upattern, thickness);
}

void bgi_settextstyle(int font, int direction, int charsize)
{
	settextstyle(font, direction, charsize);
}

void bgi_setwritemode(int mode)
{
	setwritemode(mode);
}

int bgi_textheight(char *str)
{
	return textheight(str);
}

int bgi_textwidth(char *str)
{
	return textwidth(str);
}

GDriver bgi = {
		dmouse_show,
		dmouse_hide,
		dmouse_get,

		bgi_line,
		bgi_rectangle,
		bgi_putpixel,
		bgi_arc,
		bgi_bar,
		bgi_cleardevice,
		bgi_closegraph,
		bgi_getmaxx,
		bgi_getmaxy,
		bgi_outtextxy,
		bgi_setbkcolor,
		bgi_setcolor,
		bgi_setfillpattern,
		bgi_setfillstyle,
		bgi_setlinestyle,
		bgi_settextstyle,
		bgi_setwritemode,
		bgi_textheight,
		bgi_textwidth
};

void vesa_closegraph()
{
		MouseClose();
    //		HideRato();
		SetVbeMode(0x3);

}

GDriver  vesa_driver = {
		MouseShow,
		MouseHide,
		MouseGet,
		gline,
		grectangle,
		VESAPutpixel,
		garc,
		gbar,
		VESACleardevice,
		vesa_closegraph,
		ggetmaxx,
		ggetmaxy,
		writest,
		VESASetbkcolor,
		VESASetcolor,
		VESASetfillpattern,
		VESASetfillstyle,
		VESASetlinestyle,
		VESASettextstyle,
		gsetwritemode,
		VESATextheight,
		VESATextwidth
};


GDriver *gx=&bgi;


int initgraphics(char *mode, char *opt)
{
	if(!strcmp(mode, "BGI_AUTO")) {
		int graphdriver = DETECT, graphmode;
		initgraph(&graphdriver, &graphmode, opt);
		if (graphdriver>=0) {
			gx=&bgi;
			return 1;
		}
	} else if(!strcmp(mode, "SVGA_800x600x256")) {
		if(SetVbeMode(0x103)) {
			gx=&vesa_driver;
			TextBackGround=255;
			MouseOpen();
			return 1;
		}

	}

	return 0;
}