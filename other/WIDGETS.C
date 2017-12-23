#include <stdio.h>
#include <math.h>
#include <graphics.h>
#include <stdlib.h>
#include "widgets.h"

void uiOverlayButtonInit(TOverlayButtonUI *buttons, int xi, int yi, int h, int w,int sp);
void uiOverlayButtonDrawAll(TOverlayButtonUI *buttons);
char *uiOverlayButtonEvents(TOverlayButtonUI *buttons, TEvent *ev);

#define LEVELS 256

float leveldistance[LEVELS];

TButtonUI uiButtons[]={
    {90, 0,0,0,0,"Quote One",           "load"},
    {90,0,0,0,0,"Quote Two", "op1"},
    {90,0,0,0,0,"Quote Three",        "op2"},
    {90,0,0,0,0,"Quote Four",        "op3"},
    {90,0,0,0,0,"Quote Five",    "op4"},
    {80,0,0,0,0, "Sair",            "exit"},
    {0}
};

TButtonUI uiButtonsScreenPos[]={
/*    {w:30,   caption:">",   action: "dir1"},
    {w:30,  caption: "<",   action: "dir2"},
    {w:30,  caption: "/\\", action: "dir3"},
    {w:30,  caption: "\\/", action: "dir4"},
    {w:30, caption: "H+",   action: "hplus"},
    {w:30, caption: "H-",   action: "hminus"},
    {w:30, caption: "V+",   action: "vplus"},
    {w:30, caption: "V-",   action: "vminus"},}*/
    {0}
};



TOverlayButtonUI uiOverlayButtons[]={
/*    {caption: "Abrir",             action: "load"},
    {caption: "Exportar Imagem",   action: "saveimg"},
    {caption: "Exportar Texto",    action: "savetxt"},
    {caption: "É deterministico?", action: "op1"},
    {caption: "Forma Canonica",    action: "op2"},
    {caption: "Calcular Paralelo", action: "op3"},
    {caption: "Propriedades",      action: "op4"},
    {caption: "Info",              action: "info"},
    {caption: "Ajuda",             action: "help"},
    {caption: "About",             action: "about"},
    {caption: "Sair",              action: "exit"},
    {caption: NULL,       action: NULL},
    */
    {0,0,0,0,0,NULL,NULL}
};

float
    ovx1=100,
    ovy1=100,
    ovx2=300,
    ovy2=300;

#define OVW  200
#define OVH  257

char *overlaycolor="slategray";
char *overlaycolorlight="gray";
char *overlaycolordark="darkslategray";
char *overlaycolortext="white";
char *overlaycolorhigh="green";
char *overlaycolorhighborder="white";

int xmin=0;
int xmax=639;
int ymin=0;
int ymax=479;


void srectangle(int x1, int y1, int x2, int y2)
{
	int d=3;

	arc(x1+d,y1+d,90, 180,d);
	arc(x2-d,y1+d, 0,  90,d);
	arc(x1+d,y2-d,180,270,d);
	arc(x2-d,y2-d,270,  0,d);

	line(x1+d,y1,x2-d,y1);
	line(x1+d,y2,x2-d,y2);
	line(x1,y1+d,x1,y2-d);
	line(x2,y1+d,x2,y2-2);

}

int uiOfferFocus(TObjectUI *p, int dir)
{
	TObjectUI
		*cur=NULL,
		*nearx=NULL,
		*chd=p->chdfirst;
	long int fp=-1;
	long int d=0x7FFFFFFF;
	const long int m=100000L;
	if(!chd)
		return -1;

	while(chd) {
		if(chd->flags&WFOCUS) {
			cur=chd;
			fp=chd->x1+chd->y1*m;

		}
		if(d<chd->x1+chd->y1*m) {
			d=chd->x1+chd->y1*m;
		}
		chd=chd->next;
	}


	chd=p->chdfirst;
	if(dir==0) {
		if(fp>-1) {
			while(chd) {
				long int nd=chd->x1+chd->y1*m;
				if( nd>fp && nd-fp < d ) {
					d=nd-fp;
					nearx=chd;
				}
				chd=chd->next;
			}
		}
		else {
			d=0x7FFFFFFF;
			while(chd) {
				long int nd=chd->y1*m;
				nd+=chd->x1;
				if( nd < d ) {

					d=nd;
					nearx=chd;
				}
				chd=chd->next;
			}

		}
	} else {
		if(fp>-1) {
			while(chd) {
				long int nd=chd->x1+chd->y1*m;
				if( nd<fp && fp-nd < d ) {
					d=fp-nd;
					nearx=chd;
				}
				chd=chd->next;
			}
		}
		else {
			d=0;
			while(chd) {
				long int nd=chd->y1*m;
				nd+=chd->x1;
				if( nd > d ) {

					d=nd;
					nearx=chd;
				}
				chd=chd->next;
			}

		}
	}
	if(cur) {
		cur->flags &= ~WFOCUS;
		cur->flags |= WREDRAW;
		p->flags |= WCREDRAW;
	}

	if( nearx ) {
		//gotoxy(10,10);
		//printf("Nearx %10p %5d|%5d|%5ld",nearx, nearx->x1, nearx->y1, d);
		nearx->flags |= WFOCUS;
		nearx->flags |= WREDRAW;
		p->flags |= WCREDRAW;
		return 1;
	}
	return 0;
}

void uiButtonInitRow(TButtonUI *buttons, int xi, int yi, int h, int sp)
{
    int curx=xi;
    int tsize=1;
    while(buttons->w) {
	if(buttons->caption) {
		tsize=textwidth(buttons->caption);
		if(tsize+20>buttons->w) {
			buttons->w=tsize+20;
		}
	}
	buttons->state=0;
	buttons->over=0;
	buttons->bx1 = curx;
	buttons->bx2 = curx+buttons->w;
	buttons->by1=yi;
	buttons->by2=yi+h;
	buttons->tx=(buttons->w-tsize)/2+curx;
	curx+=buttons->w+sp;
	buttons++;
    }
}

void uiButtonDraw2(TObjectUI *o, int x, int y)
{
    TButtonUI *b=o->data;
    if(b->state!=1) {
	setfillstyle(SOLID_FILL,2);
	bar(x+o->x1+1,y+o->y1+1,x+o->x2-1,y+o->y2-1);
	setcolor(10);
	srectangle(x+o->x1,y+o->y1,x+o->x2,y+o->y2);
	if(b->over)
	    setcolor(4);
	else
	    setcolor(15);
	outtextxy(x+o->x1+b->tx,y+o->y1+b->ty, b->caption);
    }
    else {
	setfillstyle(SOLID_FILL,10);
	bar(x+o->x1+1,y+o->y1+1,x+o->x2-1,y+o->y2-1);
	setcolor(2);
	srectangle(x+o->x1,y+o->y1,x+o->x2,y+o->y2);

	setcolor(4);
	outtextxy(x+o->x1+b->tx,y+o->y1+b->ty, b->caption);
   }

   if(o->flags&WFOCUS) {
	setcolor(4);
	srectangle(x+o->x1+2,y+o->y1+2,x+o->x2-2,y+o->y2-2);
	srectangle(x+o->x1+3,y+o->y1+3,x+o->x2-3,y+o->y2-3);
   }

}

void uiButtonDraw(TButtonUI *button)
{
    if(button->state!=1) {
	setfillstyle(SOLID_FILL,2);
	bar(button->bx1,button->by1,button->bx2,button->by2);
	setcolor(10);
	srectangle(button->bx1-1,button->by1-1,button->bx2+1,button->by2+1);
	if(button->over)
	    setcolor(4);
	else
	    setcolor(15);
	outtextxy(button->tx, button->by1+button->ty, button->caption);
    }
    else {
	setfillstyle(SOLID_FILL,10);
	bar(button->bx1,button->by1,button->bx2,button->by2);
	setcolor(2);
	srectangle(button->bx1-1,button->by1-1,button->bx2+1,button->by2+1);

	setcolor(4);
	outtextxy(button->tx, button->by1+button->ty, button->caption);
   }

}

void scroll_draw(TObjectUI *o, int x, int y)
{
	TScrollBarUI *sb=(TScrollBarUI *)o->data;
	int
		dragx1=o->x1+sb->dragx+x,
		dragx2=o->x1+sb->dragx+sb->dragw+x,
		dragy1=o->y1+sb->dragy+y,
		dragy2=o->y1+sb->dragy+sb->dragh+y;


	if( o->flags & WFOCUS ) {
		setfillstyle(CLOSE_DOT_FILL,4);
	}
	else {
		setfillstyle(CLOSE_DOT_FILL,8);
	}
	bar(x+o->x1+1,y+o->y1,x+o->x2-1,y+o->y2-1);
	setcolor(7);
	srectangle(x+o->x1,y+o->y1,x+o->x2,y+o->y2);

	if(sb->dragstate==0) {
		setcolor(10);
		setfillstyle(SOLID_FILL,2);

	}
	else {
		setcolor(2);
		setfillstyle(SOLID_FILL,10);
	}
	bar(dragx1+1,dragy1+1,dragx2-1,dragy2-1);
	srectangle(dragx1,dragy1,dragx2,dragy2);


}


void uiButtonDrawRow(TButtonUI *buttons)
{
    while(buttons->w) {
	uiButtonDraw(buttons);
	buttons++;
    }
}


const char *uiButtonEventRow(TButtonUI *buttons, TEvent *ev)
{
    const char *action = NULL;
    while(buttons->w) {
	if(ev->type & WIDGET_EVENT_MOUSE_BUTTON_DOWN) {
	    if(ev->mouse.button==1) {
		    if( ev->mouse.x>=buttons->bx1 && ev->mouse.x<=buttons->bx2 &&
			ev->mouse.y>=buttons->by1 && ev->mouse.y<=buttons->by2) {
			    if(buttons->state==0) {
				buttons->state=1;
				uiButtonDraw(buttons);
			    }
		    }
	    }
	}

	if(ev->type & WIDGET_EVENT_MOUSE_BUTTON_UP) {
	    if(ev->mouse.button==1) {
		if( ev->mouse.x>=buttons->bx1 && ev->mouse.x<=buttons->bx2 &&
		    ev->mouse.y>=buttons->by1 && ev->mouse.y<=buttons->by2) {
			if(buttons->state==1) {
			    buttons->state=0;
			    uiButtonDraw(buttons);
			    action=buttons->action;
			}
		}
		else{
		    if(buttons->state) {
			uiButtonDraw(buttons);
			buttons->state=0;
		    }
		}
	    }
	}


	if(ev->type & WIDGET_EVENT_MOUSE_AXES) {
	    if( ev->mouse.x>=buttons->bx1 && ev->mouse.x<=buttons->bx2 &&
		ev->mouse.y>=buttons->by1 && ev->mouse.y<=buttons->by2) {

		    if(buttons->state==2) {
			buttons->over=1;
			buttons->state=1;
			uiButtonDraw(buttons);
		    }
		    else if(buttons->over==0){
			buttons->over=1;
			uiButtonDraw(buttons);
		    }

	    }
	    else{
		if(buttons->state==1) {
		    buttons->over=0;
		    buttons->state=2;
		    uiButtonDraw(buttons);
		}
		else if(buttons->over==1){
		    buttons->over=0;
		    uiButtonDraw(buttons);
		}

	    }

	}

	if(action)
	    break;
	buttons++;
    }
    return action;
}

int button_proc(TObjectUI *o, TEvent *ev)
{
	TButtonUI *b=o->data;

	if(ev->type & WIDGET_EVENT_MOUSE_BUTTON_DOWN) {
	    if(ev->mouse.button==1) {
		    if( ev->mouse.x>=o->x1 && ev->mouse.x<=o->x2 &&
			ev->mouse.y>=o->y1 && ev->mouse.y<=o->y2) {
			    if(b->state==0) {
				b->state=1;
				o->flags |= WREDRAW;
			    }
		    }
	    }
	}

	if(ev->type & WIDGET_EVENT_MOUSE_BUTTON_UP) {
	    if(ev->mouse.button==1) {
		if( ev->mouse.x>=o->x1 && ev->mouse.x<=o->x2 &&
		    ev->mouse.y>=o->y1 && ev->mouse.y<=o->y2) {
			if(b->state==1) {
			    b->state=0;
			    o->flags |= WREDRAW;
			    ev->action=b->action;
			    ev->from=o;
			}
			else if(b->over==0){
				b->over=1;
				o->flags |= WREDRAW;
			}

		}
		else{
		    if(b->state) {
			o->flags |= WREDRAW;
			b->state=0;
		    }
		}
	    }
	}


	if(ev->type & WIDGET_EVENT_MOUSE_AXES) {
	    if( ev->mouse.x>=o->x1 && ev->mouse.x<=o->x2 &&
		ev->mouse.y>=o->y1 && ev->mouse.y<=o->y2) {

		    if(b->state==2) {
			b->over=1;
			b->state=1;
			o->flags |= WREDRAW;
		    }
		    else if(b->over==0){
			if(ev->mouse.button==0) {
				b->over=1;
				o->flags |= WREDRAW;
			}
		    }

	    }
	    else{
		if(b->state==1) {
		    b->over=0;
		    b->state=2;
		    o->flags |= WREDRAW;
		}
		else if(b->over==1){
		    b->over=0;
		    o->flags |= WREDRAW;
		}

	    }

	}

	if(o->flags&WFOCUS && ev->type&WIDGET_EVENT_KEYBOARD)
	{
		if(ev->key==0xD) {
			ev->action=b->action;
			ev->from=o;
			ev->key=0;
		}
	}
 return 0;
}

int scroll_proc(TObjectUI *o, TEvent *ev)
{
	TScrollBarUI *sb=(TScrollBarUI *)o->data;
	int
		dx1=o->x1+sb->dragx,
		dx2=o->x1+sb->dragx+sb->dragw,
		dy1=o->y1+sb->dragy,
		dy2=o->y1+sb->dragy+sb->dragh;


	if(ev->type & WIDGET_EVENT_MOUSE_BUTTON_DOWN) {
	    if(ev->mouse.button==1) {
		    if( ev->mouse.x>=dx1 && ev->mouse.x<=dx2 &&
			ev->mouse.y>=dy1 && ev->mouse.y<=dy2) {
			    if(sb->dragstate==0) {
				sb->dragstate=1;
				sb->dragmx=ev->mouse.x-sb->dragx;
				sb->dragmy=ev->mouse.y-sb->dragy;
				o->flags |= WREDRAW;
			    }
		    }
	    }
	}

	if(ev->type & WIDGET_EVENT_MOUSE_BUTTON_UP) {
	    if(ev->mouse.button==1) {
		if( ev->mouse.x>=dx1 && ev->mouse.x<=dx2 &&
		    ev->mouse.y>=dy1 && ev->mouse.y<=dy2) {
			if(sb->dragstate==1) {
			    sb->dragstate=0;
			    o->flags |= WREDRAW;
			}
		}
		else{
		    if(sb->dragstate) {
			o->flags |= WREDRAW;
			sb->dragstate=0;
		    }
		}
	    }
	}



	if(ev->type & WIDGET_EVENT_MOUSE_AXES) {
	    if( sb->dragstate) {
		    if(sb->dir==0) {
			sb->dragy=ev->mouse.y - sb->dragmy;
			if(sb->dragy<sb->dragmin)
				sb->dragy=sb->dragmin;
			if(sb->dragy>sb->dragmax)
				sb->dragy=sb->dragmax;
		    }
		    else {
			sb->dragx=ev->mouse.x - sb->dragmx;
			if(sb->dragx<sb->dragmin)
				sb->dragx=sb->dragmin;
			if(sb->dragx>sb->dragmax)
				sb->dragx=sb->dragmax;
		    }
		    o->flags |= WREDRAW;
	    }
	}



	if(ev->action)
	{
		if(ev->from==sb->bt[0]) {
			if(sb->dir==0) {
				sb->dragy-=10;
				if(sb->dragy<sb->dragmin)
					sb->dragy=sb->dragmin;
			} else {
				sb->dragx-=10;
				if(sb->dragx<sb->dragmin)
					sb->dragx=sb->dragmin;
			}
			o->flags |= WREDRAW;
		}
		if(ev->from==sb->bt[1]) {
			if(sb->dir==0) {
				sb->dragy+=10;
				if(sb->dragy>sb->dragmax)
					sb->dragy=sb->dragmax;
			} else {
				sb->dragx+=10;
				if(sb->dragx>sb->dragmax)
					sb->dragx=sb->dragmax;
			}
			o->flags |= WREDRAW;
		}
	}

	ev->action=NULL;
	ev->from=NULL;

	if(o->flags&WFOCUS && ev->type&WIDGET_EVENT_KEYBOARD)
	{
		switch(ev->key)
		{
			case 0x148:
				if(sb->dir==0) {
					sb->dragy-=10;
					if(sb->dragy<sb->dragmin)
						sb->dragy=sb->dragmin;
				}
			break;
			case 0x150:
				if(sb->dir==0) {
					sb->dragy+=10;
					if(sb->dragy>sb->dragmax)
						sb->dragy=sb->dragmax;
				}
			break;
			case 0x14B:
				if(sb->dir==1) {
					sb->dragx-=10;
					if(sb->dragx<sb->dragmin)
						sb->dragx=sb->dragmin;
				}
			break;
			case 0x14D:
				if(sb->dir==1) {
					sb->dragx+=10;
					if(sb->dragx>sb->dragmax)
						sb->dragx=sb->dragmax;
				}
			break;

		}
		o->flags |= WREDRAW;
	}


	if(o->flags & WREDRAW) {
		float newv=0;
		newv = (sb->dir==0)
			? (float)(sb->dragy-sb->dragmin)
			: (float)(sb->dragx-sb->dragmin);
		newv /= (sb->dragmax-sb->dragmin);

		if(sb->value!=newv) {
			sb->value=newv;
			ev->action = (sb->dir==0) ? "vscroll" : "hscroll";
			ev->from = o;
		}
	}

	return 0;
}



void displayBackground()
{
    cleardevice();
    //setcolor(2);
    //rectangle(xmin,ymin,xmax,ymax);
   /* al_draw_line(1,25,program.winwidth-1,25, al_color_name("green"),2);
    al_draw_line(1,55,program.winwidth-1,55, al_color_name("green"),2);
    al_draw_textf(program.font, al_color_name("white"), program.winwidth/2, 5,ALLEGRO_ALIGN_CENTRE, "%s", "Automatonic (R)");
    */
//    uiButtonDrawRow(uiButtons);
//    uiButtonDrawRow(uiButtonsScreenPos);
//    uiDrawScrollBar(&hsb);
//    uiDrawScrollBar(&vsb);
}


TObjectUI *gxCreateObjectUI(TObjectUI *parent, int x1, int y1, int w, int h, void *data, TDrawProc dproc, TEventProc eproc, int type, int id)
{
	TObjectUI *o=(TObjectUI *)malloc(sizeof(TObjectUI));
	if(!o) return NULL;
	if(parent){
		if(parent->chdlast) {
			parent->chdlast->next=o;
			parent->chdlast=o;
		} else {
			parent->chdfirst=o;
			parent->chdlast=o;
		}
	}
	o->parent=parent;
	o->next=NULL;
	o->chdfirst=NULL;
	o->chdlast=NULL;
	o->data=data;
	o->x1=x1;
	o->y1=y1;
	o->x2=x1+w;
	o->y2=y1+h;
	o->w=w;
	o->h=h;
	o->type=type;
	o->id=id;
	o->draw_proc=dproc;
	o->event_proc=eproc;
	o->flags=0;
	return o;
}

void uiClipEvents(TObjectUI *o, TEvent *ev)
{
	if(ev->type & 0x7) {
		ev->mouse.x -= o->x1;
		ev->mouse.y -= o->y1;
	}
}

TObjectUI *gxDispatchEvents(TObjectUI *o, TEvent *ev)
{
	if(o->chdfirst) {
		TObjectUI *chd=o->chdfirst;
		TEvent evc=*ev;

		uiClipEvents(o,&evc);
		while(chd) {
			gxDispatchEvents(chd,&evc);
			if( (chd->flags&WREDRAW) || (chd->flags&WCREDRAW)) {
				o->flags |= WCREDRAW;
			}
			if(evc.action) {
				ev->action = evc.action;
				ev->from   = evc.from;
			}
			chd=chd->next;
		}
	}
	if(o->event_proc) {
		if( o->event_proc(o, ev) ) {
			return o;
		}
	}

	return NULL;
}

void gxDrawObjectUI(TObjectUI *o, int x, int y, int w, int h)
{
	TObjectUI *chd=o->chdfirst;

	int x1=o->x1+x;
	int y1=o->y1+y;

	o->flags &= ~(WREDRAW | WCREDRAW);

	if( o->draw_proc )
		o->draw_proc(o,x,y);

	while(chd) {
		gxDrawObjectUI(chd,x1,y1,o->w,o->h);
		chd=chd->next;
	}
}

void gxRedrawObjectUI(TObjectUI *o, int x, int y, int w, int h)
{
	int x1=o->x1+x;
	int y1=o->y1+y;
	if(o->flags & WREDRAW) {
		gxDrawObjectUI(o,x,y,w,h);
		o->flags &= ~(WREDRAW | WCREDRAW);
	}

	if(o->flags & WCREDRAW) {
		TObjectUI *chd=o->chdfirst;
		while(chd) {
			gxRedrawObjectUI(chd,x1,y1,o->w,o->h);
			chd=chd->next;
		}
		o->flags &= ~WCREDRAW;
	}

}

TObjectUI *gxCreateButtonUI(TObjectUI *parent, int x1, int y1, int w, int h, const char *caption, const char *action)
{
	TObjectUI *o;
	TButtonUI *b=(TButtonUI *)malloc(sizeof(TButtonUI));
	if(!b)
		return NULL;
	b->caption=caption;
	b->action=action;

	if(b->caption) {
		int tsx=textwidth(b->caption);
		int tsy=textheight(b->caption);
		if(tsx>w)
			w=tsx;
		b->tx=(w-tsx)/2;

		if(tsy>h)
			h=tsy;
		b->ty=(h-tsy)/2;
	}
	else {
		b->tx=0;
		b->ty=0;
	}
	b->bx1=0;
	b->bx2=w;
	b->by1=0;
	b->by2=h;
	b->state=0;
	b->over=0;
	b->w=w;
	o=gxCreateObjectUI(parent,x1,y1,w,h,b,uiButtonDraw2,button_proc,1, (int)b);
	if(!o)
		free(b);
	return o;
}


TObjectUI *gxCreateScrollUI(TObjectUI *parent, int x1, int y1, int w, int h, int dir, int min, int max, int pos)
{
	TObjectUI *o;
	TScrollBarUI *sb=(TScrollBarUI *)malloc(sizeof(TScrollBarUI));
	float dragm;
	if(!sb)
		return NULL;

	if(pos<min)
		pos=min;
	if(pos>max)
		pos=max;
	sb->max=max;
	sb->min=min;
	sb->pos=pos;
	sb->dragstate=0;
	sb->dragover=0;
	sb->dir=dir;
	dragm = (float)pos/(max-min);
	sb->value=dragm;

	o=gxCreateObjectUI(parent,x1,y1,w,h,sb,scroll_draw,scroll_proc,WSCROLL_ID, (int)sb);
	if(!o)
		free(sb);

	if(dir==0){
		sb->bt[0]=gxCreateButtonUI(o,0,0,w,w,"\x1E","less");
		sb->bt[1]=gxCreateButtonUI(o,0,h-w,w,w,"\x1F","more");

		sb->dragy=(h-w)*dragm+4;
		sb->dragmin=w+2;
		sb->dragmax=h-w*2;
		sb->dragx=1;
		sb->dragw=w-2;
		sb->dragh=w-2;
	}
	else{
		sb->bt[0]=gxCreateButtonUI(o,0,0,h,h,"\x11","less");
		sb->bt[1]=gxCreateButtonUI(o,w-h,0,h,h,"\x10","more");

		sb->dragx=(w-h)*dragm+4;
		sb->dragmin=h+2;
		sb->dragmax=w-h*2;
		sb->dragy=1;
		sb->dragw=h-2;
		sb->dragh=h-2;
	}
	return o;
}

void desktop_draw(TObjectUI *o, int x, int y)
{
	int x1=o->x1+x;
	int y1=o->y1+y;
	int x2=o->x2+x;
	int y2=o->y2+y;

	setfillstyle(SOLID_FILL,1);
	bar(x1,y1,x2,y2);
}

void copylines(char *dest, const char **src, int max)
{
	while(**src && **src!='\n' && max>0) {
		*dest=**src;
		dest++;
		max--;
		(*src)++;
	}
	*dest='\0';
	while(**src=='\n')
		(*src)++;
}

void printlines(int x, int y, const char *lines)
{
	char buffer[100];
	while(*lines)
	{
		copylines(buffer, &lines,68);
		outtextxy(x,y,buffer);
		y+=textheight("H")+2;
	}
}

void calctextwh(const char *lines, int *w, int *h)
{
	char buffer[100];
	*w=0;
	*h=0;
	while(*lines)
	{
		int tw;
		copylines(buffer, &lines,68);
		tw=textwidth(buffer);
		*h+=textheight("H")+2;
		if(tw>*w)
			*w=tw;
	}

}



void dialog_draw(TObjectUI *o, int x, int y)
{
	TDialogUI *dl=(TDialogUI *)o->data;
	int x1=o->x1+x;
	int y1=o->y1+y;
	int x2=o->x2+x;
	int y2=o->y2+y;

	setcolor(9);
	setlinestyle(SOLID_LINE,1,THICK_WIDTH);
	line(x1+1,y1+1,x2-2,y1+1);
	line(x1+1,y1+1,x1+1,y2-1);

	setcolor(0);
	line(x1+2,y2-1,x2-1,y2-1);
	line(x2-1,y1+1,x2-1,y2-1);

	setlinestyle(SOLID_LINE,1,1);
	setfillstyle(SOLID_FILL,2);
	bar(x1+3,y1+3,x2-3,y1+21);
	setfillstyle(SOLID_FILL,1);
	bar(x1+3,y1+22,x2-3,y2-3);

	setcolor(15);
	outtextxy(dl->tix+x1, dl->tiy+y1, dl->title);
	setcolor(15);
	printlines(dl->tx+x1, dl->ty+y1, dl->text);
}

int dialog_proc(TObjectUI *o, TEvent *ev)
{
	TDialogUI *dl=(TDialogUI *)o->data;
	int
		dx1=o->x1,
		dx2=o->x2,
		dy1=o->y1,
		dy2=o->y1+20;

	if(o->flags&WFOCUS && ev->type&WIDGET_EVENT_KEYBOARD)
	{
		if(ev->key==27) {
			ev->action="close";
			ev->from=o;
		}
	}

	if(ev->type & WIDGET_EVENT_MOUSE_BUTTON_DOWN) {
	    if(ev->mouse.button==1) {
		    if( ev->mouse.x>=dx1 && ev->mouse.x<=dx2 &&
			ev->mouse.y>=dy1 && ev->mouse.y<=dy2) {
			    if(dl->dragstate==0) {
				dl->dragstate=1;
				dl->drmx=dx1-ev->mouse.x;
				dl->drmy=dy1-ev->mouse.y;
				dl->drx=dx1;
				dl->dry=dy1;
				dl->lset=0;
			    }
		    }
	    }
	}
}

TObjectUI *gxCreateDialogUI(TObjectUI *parent, int x1, int y1, int w, int h, const char *title, const char *text)
{
	int tw,th;
	TObjectUI *o;
	TDialogUI *dl=(TDialogUI *)malloc(sizeof(TDialogUI));
	if(!dl) {
		return NULL;
	}
	dl->title=title;
	dl->text=text;
	calctextwh(dl->text,&tw,&th);
	if(w<tw+80)
		w=tw+80;
	if(h<th+70)
		h=th+70;

	if(w<textwidth(title)+40) {
		w=textwidth(title)+40;
	}


	dl->tix=8;
	dl->tiy=3+(20-textheight(title))/2;
	dl->tx=20;
	dl->ty=30;
	dl->drx=0;
	dl->dry=0;
	dl->drmx=0;
	dl->drmy=0;
	dl->dragstate=0;

	if(x1==-1)
		x1=(640-w)/2;
	if(y1==-1)
		y1=(480-h)/2;

	o=gxCreateObjectUI(parent,x1,y1,w,h,dl,dialog_draw,dialog_proc, WDIALOG_ID, (int)dl);
	if(!o)
		free(dl);
	return o;
}

int gxMessageBox(TObjectUI *parent, const char *title, const char *text)
{
	TObjectUI *dlo=gxCreateDialogUI(NULL,-1,-1,0,0,title,text);
	gxCreateButtonUI(dlo,dlo->w-80,dlo->h-30,70,20,"OK","close");
	gxRunDialog(dlo, parent);
}

int gxConfirmBox(TObjectUI *parent, const char *title, const char *text, const char *label1, const char *label2)
{
	char *action;
	TObjectUI *dlo=gxCreateDialogUI(NULL,-1,-1,220,80,title,text);

	gxCreateButtonUI(dlo,dlo->w-200,dlo->h-30,70,20,label1,"dl_op1");
	gxCreateButtonUI(dlo,dlo->w-80,dlo->h-30,70,20,label2,"dl_op2");
	action=gxRunDialog(dlo, parent);
	if(!strcmp(action,"dl_op1")) {
		return 0;
	}
	else if(!strcmp(action,"dl_op2")) {
		return 1;
	}
	return -1;
}

void gxProcessWindowsEvents(TObjectUI *o, TEvent *ev)
{
	TDialogUI *dl=(TDialogUI *)o->data;
	int clear=0;
	int set=0,sx,sy;
	int x1,y1,x2,y2;
	int move=0;

	if(o->type!=WDIALOG_ID)
		return;
	if(!dl->dragstate)
		return;


	set=1;
	sx=dl->drx;
	sy=dl->dry;


	if(ev->type & WIDGET_EVENT_MOUSE_BUTTON_UP) {
	    if(ev->mouse.button==1) {
		clear=1;
		dl->dragstate=0;
		if(dl->drx!=o->x1 || dl->drx!=o->y1) {
			ev->action="move";
			o->x1=dl->drx;
			o->y1=dl->dry;
			o->x2=dl->drx+o->w;
			o->y2=dl->dry+o->h;
			o->flags|=WREDRAW;
			o->flags|=WBGDIRTY;
		}
	    }
	}


	if(ev->type & WIDGET_EVENT_MOUSE_AXES) {
	    if( dl->dragstate) {
		//clear=1;
		sx=ev->mouse.x+dl->drmx;
		sy=ev->mouse.y+dl->drmy;
		if(sx!=dl->drx || sy!=dl->dry) {
			set=1;
		}
	    }
	}

	if(dl->lset) {
		x1=dl->drx;
		x2=dl->drx+o->w;
		y1=dl->dry;
		y2=dl->dry+o->h;
		mouse_hide();
		setwritemode(XOR_PUT);
		setcolor(14);
		setlinestyle(DOTTED_LINE,1,THICK_WIDTH);
		rectangle(x1,y1,x2,y2);
		setwritemode(COPY_PUT);
		setlinestyle(SOLID_LINE,1,1);
		setwritemode(COPY_PUT);
		mouse_show();
		dl->lset=0;
	}
	if(set) {
		dl->drx=sx;
		dl->dry=sy;
		x1=dl->drx;
		x2=dl->drx+o->w;
		y1=dl->dry;
		y2=dl->dry+o->h;
		mouse_hide();
		setwritemode(XOR_PUT);
		setcolor(14);
		setlinestyle(DOTTED_LINE,1,THICK_WIDTH);
		rectangle(x1,y1,x2,y2);
		setwritemode(COPY_PUT);
		setlinestyle(SOLID_LINE,1,1);
		mouse_show();
		dl->lset=1;
	}
	ev->type=0;
}


char *gxRunDialog(TObjectUI *root, TObjectUI *caller)
{
   int x,y,b;
   int ox,oy,ob;
   TEvent ev;
   char *action=NULL;
   if(caller) {
	mouse_hide();
	gxRedrawObjectUI(caller,0,0,639,479);
	mouse_show();
   }
//   setbkcolor(0);
//   test();
   mouse_hide();
   gxDrawObjectUI(root,0,0,639,479);
   mouse_show();
   ox=0;
   oy=0;
   ob=0;
   while(1){
    mouse_get(&x,&y,&b);
    ev.type=0;
    ev.key=0;

    if(kbhit()){
	ev.key=getch();
	if(!ev.key) {
		ev.key=getch();
		ev.key |= 0x100;
	}
	ev.type |= WIDGET_EVENT_KEYBOARD;
	//break;
    }

    ev.action=0;
    ev.from=NULL;
    ev.mouse.x=x;
    ev.mouse.y=y;
    ev.mouse.button=b;
    if( x!=ox || y!=oy ) {
	ox=x;
	oy=y;
	ev.type=0x1;
	ev.key=0;
	ev.mouse.x=x;
	ev.mouse.y=y;
	ev.mouse.button=b;
    }
    if(b!=ob) {
      if((b&1) && !(ob&1)) {
	ev.type |= WIDGET_EVENT_MOUSE_BUTTON_DOWN;
	ev.mouse.button=1;
      } else if(!(b&1) && (ob&1)) {
	ev.type |= WIDGET_EVENT_MOUSE_BUTTON_UP;
	ev.mouse.button=1;
      }
      ob=b;
    }


    if(ev.type && root->type==WDIALOG_ID) {
	gxProcessWindowsEvents(root,&ev);
    }

    if(ev.type) {
	gxDispatchEvents(root,&ev);
	if(root->flags) {
	}
	action=ev.action;
	if(action) {
		//if(!strcmp(action,"exit"))
		//	break;
		//else if(!strcmp(action,"close"))
		//break;
		//if(ev.flags&WCLOSE)
			break;
	}
    }

    if(ev.type&WIDGET_EVENT_KEYBOARD)
    {
	if(ev.key==0x9) {
		if( uiOfferFocus(root,0)==0 )
			uiOfferFocus(root,0);
		ev.key=0;
	}
	else if(ev.key==0x10F) {
		if( uiOfferFocus(root,1)==0 )
			uiOfferFocus(root,1);
		ev.key=0;
	}
    }

    if(root->flags & (WREDRAW|WCREDRAW)) {
	mouse_hide();
	if(root->flags&WBGDIRTY) {
		gxDrawObjectUI(caller,0,0,639,479);
		root->flags &= ~WBGDIRTY;
	}
	gxRedrawObjectUI(root,0,0,639,479);
	mouse_show();
    }
    delay(1);
   }

   if(caller) {
	mouse_hide();
	gxDrawObjectUI(caller,0,0,639,479);
	mouse_show();
   }
   return action;
}

void gxRun(TObjectUI *root)
{
   int x,y,b;
   int ox,oy,ob;
   TEvent ev;
   char *action;

   setbkcolor(0);
   test();
   gxDrawObjectUI(root,0,0,639,479);
   mouse_show();
   ox=0;
   oy=0;
   ob=0;
   while(1){
    mouse_get(&x,&y,&b);
    ev.type=0;
    ev.key=0;

    if(kbhit()){
	ev.key=getch();
	if(!ev.key) {
		ev.key=getch();
		ev.key |= 0x100;
	}
	ev.type |= WIDGET_EVENT_KEYBOARD;
	//break;
    }

    ev.action=0;
    ev.from=NULL;
    ev.mouse.x=x;
    ev.mouse.y=y;
    ev.mouse.button=b;
    if( x!=ox || y!=oy ) {
	ox=x;
	oy=y;
	ev.type=0x1;
	ev.key=0;
	ev.mouse.x=x;
	ev.mouse.y=y;
	ev.mouse.button=b;
    }
    if(b!=ob) {
      if((b&1) && !(ob&1)) {
	ev.type |= WIDGET_EVENT_MOUSE_BUTTON_DOWN;
	ev.mouse.button=1;
      } else if(!(b&1) && (ob&1)) {
	ev.type |= WIDGET_EVENT_MOUSE_BUTTON_UP;
	ev.mouse.button=1;
      }
      ob=b;
    }

    if(ev.type) {
	gxDispatchEvents(root,&ev);
	if(root->flags) {
	}
	action=ev.action;
	if(action) {
		static float hvalue,vvalue;
		if(!strcmp(action,"exit"))
			break;
		else if(!strcmp(action,"hscroll")) {
			hvalue=((TScrollBarUI *)ev.from->data)->value;
		}
		else if(!strcmp(action,"vscroll")) {
			vvalue=((TScrollBarUI *)ev.from->data)->value;
		}

	}
    }

    if(ev.type&WIDGET_EVENT_KEYBOARD)
    {
	if(ev.key==0x9) {
		if( uiOfferFocus(root,0)==0 )
			uiOfferFocus(root,0);
		ev.key=0;
	}
	else if(ev.key==0x10F) {
		if( uiOfferFocus(root,1)==0 )
			uiOfferFocus(root,1);
		ev.key=0;
	}
    }

    if(root->flags & (WREDRAW|WCREDRAW)) {
	mouse_hide();
	gxRedrawObjectUI(root,0,0,639,479);
	mouse_show();
    }
    delay(1);
   }
   mouse_hide();

}

float gxScrollGetValue(TObjectUI *o)
{

}


void gxObjectToRedraw(TObjectUI *o)
{
	o->flags|=WREDRAW;
	while( (o=o->parent) ) {
		o->flags|=WCREDRAW;
	}
}