#ifndef _GRAPHSYS_H

#define _GRAPHSYS_H

typedef struct {
	void (*mouse_show)(void);
	void (*mouse_hide)(void);
	void (*mouse_get)(int *x, int *y, int *b);

	void (*line)(int x1, int y1, int x2, int y2);
	void (*rectangle)(int x1, int y1, int x2, int y2);
	void (*putpixel)(int x, int y, int c);
	void (*arc)(int x, int y, int stangle, int endangle, int radius);
	void (*bar)(int left, int top, int right, int bottom);
//	void (*bar3d)(int left, int top, int right, int bottom, int depth, int topflag);
//	void (*circle)(int x, int y, int radius);
	void (*cleardevice)(void);
//	void (*clearviewport)(void);
	void (*closegraph)(void);

//	void (*drawpoly)(int numpoints, int *polypoints);
//	void (*ellipse)(int x, int y, int stangle, int endangle, int xradius, int yradius);
//	void (*fillellipse)(int x, int y, int xradius, int yradius);
//	void (*fillpoly)(int numpoints, int far *polypoints);
//	void (*floodfill)(int x, int y, int border);
/*	getarccoords
	getaspectratio
	getbkcolor
	getcolor
	getdefaultpalette
	getdrivername
	getfillpattern
	getfillsettings
	getgraphmode
	getimage
	getlinesettings
	getmaxcolor
	getmaxmode */
	int (*getmaxx)(void);
	int (*getmaxy)(void);
 /*	getmodename
	getmoderange
	getpalette
	getpalettesize
	getpixel
	gettextsettings
	getviewsettings
	getx
	gety
	graphdefaults
	grapherrormsg
	_graphfreemem
	_graphgetmem
	graphresult
	imagesize
	initgraph
	installuserdriver
	installuserfont
	line
	linerel
	lineto
	moverel
	moveto
	outtext */
	void (*outtextxy)(int x, int y, char *textstring);
/*	pieslice
	putimage
	putpixel
	rectangle
	registerbgidriver
	registerfarbgidriver
	registerbgifont
	registerfarbgifont
	restorecrtmode
	sector
	setactivepage
	setallpalette
	setaspectratio */
	void (*setbkcolor)(int color);
	void (*setcolor)(int color);
	void (*setfillpattern)(char *upattern, int color);
	void (*setfillstyle)(int pattern, int color);
/*	setgraphbufsize
	setgraphmode   */
	void (*setlinestyle)(int linestyle, unsigned upattern, int thickness);
/*	setpalette
	setrgbpalette
	settextjustify */
	void (*settextstyle)(int font, int direction, int charsize);
/*	setusercharsize
	setviewport
	setvisualpage   */
	void (*setwritemode)(int mode);
	int (*textheight)(char *str);
	int (*textwidth)(char *str);
}GDriver;

extern GDriver *gx;

int initgraphics(char *mode, char *opt);

#endif