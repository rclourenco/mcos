#ifndef WIDGETS_H
    #define WIDGETS_H

typedef struct{
	struct {
		int x,y;
		int button;
	}mouse;
	int key;
	int type;
	const char *action;
	struct TObjectUI *from;
}TEvent;

typedef void (*TDrawProc)(struct TObjectUI *o, int x, int y);
typedef int  (*TEventProc)(struct TObjectUI *o, TEvent *ev);

typedef struct TObjectUI{
    int type;
    int id;
    int w,h;
    int
	x1,
	y1,
	x2,
	y2;
   struct TObjectUI *parent;
   struct TObjectUI *next;
   struct TObjectUI *chdfirst;
   struct TObjectUI *chdlast;
   void *data;
   TDrawProc draw_proc;
   TEventProc event_proc;
   int flags;
}TObjectUI;

typedef struct {
    int w;
    int
	bx1,
	by1,
	bx2,
	by2;
    const char *caption;
    const char *action;
    int state;
    int over;
    int tx,ty;
}TButtonUI;

typedef struct {
    const char *title;
    const char *text;
    int tx, ty;
    int tix,tiy;
    int dragstate;
    int drmx,drx;
    int drmy,dry;
    int lset;
}TDialogUI;

#define MAXTEXT (80*25)


typedef struct {
    char buffer[MAXTEXT+1];
    int len;    /* tamanho total do texto */
    int tc,tl;  /* total de colunas e linhas do texto  */
    int nc,nl;  /* numero de colunas e linhas visiveis */
    int sc,sl;  /* coluna e linha iniciais */
    int ofs;    /* inicio do texto visivel */
    int csr;    /* cursor */
    int cx,cy;
}TTextUI;



typedef struct {
    int
	bx1,
	by1,
	bx2,
	by2;
    char *caption;
    char *action;
    int state;
    int over;
}TOverlayButtonUI;


typedef struct {
    int dir;

    TObjectUI *bt[3];

    int inc;

    int min,
	max,
	pos;

    int dragx,
	dragy,
	dragw,
	dragh;

    int
	dragmin,
	dragmax;

    int dragmx,
	dragmy;

    int dragstate,
	dragover;

    float value;
}TScrollBarUI;

#define WREDRAW  1
#define WCREDRAW 2
#define WFOCUS   4
#define WBGDIRTY 8

#define WIDGET_EVENT_MOUSE_AXES 1
#define WIDGET_EVENT_MOUSE_BUTTON_UP   2
#define WIDGET_EVENT_MOUSE_BUTTON_DOWN 4
#define WIDGET_EVENT_KEYBOARD 8

#define DIR_UP      0
#define DIR_DOWN    1
#define DIR_LEFT    2
#define DIR_RIGHT   3

#define HSCROLL 10
#define VSCROLL 11

#define DIR_INC     10

#define HPLUS       0
#define HMINUS      1
#define VPLUS       2
#define VMINUS      3

#define WDESK_ID    1000
#define WWINDOW_ID  1001
#define WDIALOG_ID  1002
#define WBUTTON_ID  1010
#define WSCROLL_ID  1020
#define WTEXT_ID    1030

void uiButtonInitRow(TButtonUI *buttons, int xi, int yi, int h, int sp);
void displayBackground();

extern TButtonUI uiButtons[];

TObjectUI *gxCreateObjectUI(TObjectUI *parent, int x1, int y1, int w, int h, void *data, TDrawProc dproc, TEventProc eproc,int type, int id);
void gxDrawObjectUI(TObjectUI *o, int x, int y, int w, int h);

TObjectUI *gxCreateButtonUI(TObjectUI *parent, int x1, int y1, int w, int h, const char *caption, const char *action);
TObjectUI *gxCreateScrollUI(TObjectUI *parent, int x1, int y1, int w, int h, int dir, int min, int max, int pos);

void uiButtonDraw2(TObjectUI *o, int x, int y);


TObjectUI *gxDispatchEvents(TObjectUI *o, TEvent *ev);


int uiOfferFocus(TObjectUI *p, int dir);
char *gxRunDialog(TObjectUI *root, TObjectUI *caller);
void gxRun(TObjectUI *root);

void desktop_draw(TObjectUI *o, int x, int y);


TObjectUI *gxCreateTextUI(
	TObjectUI *parent,
	int x1, int y1, int w, int h,
	const char *text);


#endif
