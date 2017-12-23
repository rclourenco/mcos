#include <stdlib.h>
#include <graphics.h>
#include "widgets.h"

static void counttextcl(const char *text, int *cols, int *lines)
{
	int cc=0;
	*cols=0;
	*lines=0;
	while(*text)
	{
		putchar(*text);
		if(*text=='\n') {
			if( cc > *cols )
				*cols=cc;
			cc=0;
			*lines=*lines+1;
		}
		else {
			cc++;
		}
		text++;
	}
	if(cc>*cols)
		*cols=cc;
	if(cc)
		(*lines)++;
	printf("%d %d\n", *cols, *lines);
	getch();

}

void wtext_draw(TObjectUI *o, int x, int y)
{

    int x1=o->x1+x,
	y1=o->y1+y,
	x2=o->x2+x,
	y2=o->y2+y;
    int i=0,
	c=0,
	l=0;
    int tx,ty;
    TTextUI *t=(TTextUI *)o->data;
    char otb[2] = {0,0};
    setfillstyle(SOLID_FILL,0);
    setcolor(2);
    rectangle(x1,y1,x2,y2);

    bar(x1+1,y1+1,x2-1,y2-1);

    if(o->flags&WFOCUS) {
	setcolor(4);
	rectangle(x1+1,y1+1,x2-1,y2-1);
	setcolor(2);
    }

    for(i=0;t->buffer[i];i++) {
	if(t->csr==i) {
		t->cx=c;
		t->cy=l;
	}

	if( t->buffer[i]=='\n' ) {
		c=0;
		l++;
		continue;
	}
	else {
		ty = y1+(l-t->sl)*textheight("²")+5;
		tx = x1+(c-t->sc)*textwidth("Í")+5;

		if( c >= t->sc && c< t->nc+t->sc && l >= t->sl &&  l < t->nl+t->sl )
		{
			otb[0]=t->buffer[i];
			outtextxy(tx,ty,otb);
		}
		c++;
	}

	if( t->cx >= t->sc && t->cx< t->nc+t->sc && t->cy >= t->sl &&  t->cy < t->nl+t->sl ) {
		tx = x1+(t->cy-t->sl)*textheight("²")+5;
		ty = y1+(t->cx-t->sc)*textwidth("Í")+5;
		rectangle(tx, ty, tx+textwidth("Í"), ty+textwidth("Í"));

	}
    }
}


int wtext_proc(TObjectUI *o, TEvent *ev)
{
	TTextUI *t=(TTextUI *)o->data;
	if(o->flags&WFOCUS && ev->type&WIDGET_EVENT_KEYBOARD)
	{
		int i;
		switch(ev->key)
		{
			case 0x014B:
				if(t->sc>0) {
					t->sc--;
					o->flags |= WREDRAW;
				}
			break;
			case 0x014D:
				if(t->sc<t->tc-1) {
					t->sc++;
					o->flags |= WREDRAW;
				}
			break;
			case 0x0148:
				if(t->sl>0) {
					t->sl--;
					o->flags |= WREDRAW;
				}
			break;
			case 0x0150:
				if(t->sl<t->tl-1) {
					t->sl++;
					o->flags |= WREDRAW;
				}
			break;
			default:
				gotoxy(14,14);
				printf("%04X    %d ",ev->key, t->tl);
			break;
		}
	}

}


TObjectUI *gxCreateTextUI(
	TObjectUI *parent,
	int x1, int y1, int w, int h,
	const char *text)
{
	int tc,tl;
	TObjectUI *o;
	TTextUI *t=(TTextUI *)malloc(sizeof(TTextUI));
	if(!t)
		return NULL;
	t->sc=0;
	t->sl=0;
	strncpy(t->buffer,text,MAXTEXT);
	t->buffer[MAXTEXT]='\0';
	printf("%s\n", t->buffer);
	counttextcl(t->buffer,&t->tc,&t->tl);
	t->nc=(w-10)/textwidth("Í");
	t->nl=(h-10)/textheight("²");
	t->csr=0;
	t->cx=0;
	t->cy=0;
	//if(t->nc<tc) t->nc=tc;
	//if(t->nl<tl) t->nl=tl;

	o=gxCreateObjectUI(parent,x1,y1,w,h,t,wtext_draw,wtext_proc, WTEXT_ID, (int)t);
	if(!o)
		free(t);
	return o;

}

int gxTextGetCols(TObjectUI *o)
{
	if(o->type!=WTEXT_ID)
		return 0;
	return ((TTextUI *)o->data)->tc;
}

int gxTextGetLines(TObjectUI *o)
{
	if(o->type!=WTEXT_ID)
		return 0;
	return ((TTextUI *)o->data)->tl;

}


int gxTextGetStartColumn(TObjectUI *o)
{
	if(o->type!=WTEXT_ID)
		return 0;
	return ((TTextUI *)o->data)->sc;
}

int gxTextGetStartLine(TObjectUI *o)
{
	if(o->type!=WTEXT_ID)
		return 0;
	return ((TTextUI *)o->data)->sl;

}

void gxTextSetStartColumn(TObjectUI *o, int sc)
{
	if(o->type!=WTEXT_ID)
		return;
	if( sc>=0 && sc < ((TTextUI *)o->data)->tc ) {
		((TTextUI *)o->data)->sc = sc;
		gxObjectToRedraw(o);
	}
}

void gxTextSetStartLine(TObjectUI *o, int sl)
{
	if(o->type!=WTEXT_ID)
		return;
	if( sl>=0 && sl < ((TTextUI *)o->data)->tl ) {
		((TTextUI *)o->data)->sl = sl;
		gxObjectToRedraw(o);
	}
}
