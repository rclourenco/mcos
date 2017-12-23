#include <stdio.h>
#include <conio.h>

#define MAXCOLS 255
#define MAXLINES 1000

typedef struct {
  unsigned len;
  unsigned char contents[MAXCOLS];
} TLine;

typedef struct {
  unsigned cap;
  unsigned nlines;
  TLine **lines;
} TText;

typedef struct {
  unsigned cx,cy;
  unsigned sx,sy;
  TText text;
} TControl;

int text_init(TText *t, unsigned cap);
int text_newline(TText *t, unsigned pos);

int text_ctrl_init(TControl *tc, unsigned cap);
int text_ctrl_newline(TControl *tc);
int text_ctrl_up(TControl *tc);
int text_ctrl_down(TControl *tc);

int text_ctrl_left(TControl *tc);
int text_ctrl_right(TControl *tc);

int text_ctrl_draw(TControl *tc);

int text_ctrl_edit(TControl *tc, unsigned char ch);
int text_ctrl_delete(TControl *tc, unsigned char ch);

int text_ctrl_drawline(TControl *tc, unsigned pos);


TText text;
TControl textctrl;



unsigned getkey()
{
  unsigned k = getch();
  if(!k) {
    k = getch();
    k |= 0x0100;
  }
  return k;
}


int main()
{
  unsigned run = 1;
  if(text_ctrl_init(&textctrl,100)) {
    printf("Ok\n");
  }
  getch();
  text_ctrl_draw(&textctrl);
  while(run) {
    unsigned t = getkey();
    switch(t) {
      case 27: run=0; break;
      case 13:
	text_ctrl_newline(&textctrl);
	text_ctrl_draw(&textctrl);
      break;
      case 0x148: text_ctrl_up(&textctrl); break;
      case 0x150: text_ctrl_down(&textctrl); break;
      case 0x14B: text_ctrl_left(&textctrl); break;
      case 0x14D: text_ctrl_right(&textctrl); break;
      case 0x008: text_ctrl_delete(&textctrl, 0); break;
      //case 0x153: text_ctrl_toggle(&textctrl); break;
      default: if( t >= 32 && t < 256 ) {
		text_ctrl_edit(&textctrl, t);
		text_ctrl_drawline(&textctrl, textctrl.cy);
	}
	else {
		printf("Key %04X\n", t);
	}
    }
  }
  clrscr();
  text_ctrl_draw(&textctrl);
  getch();
  return 0;
}

int text_init(TText *t, unsigned cap)
{
  t->lines = (TLine **)malloc( sizeof(TLine *)*cap );
  if(t->lines) {
    unsigned i;
    for(i=0;i<cap;i++)
	t->lines[i]=NULL;
    t->nlines = 0;
    t->cap = cap;
    return cap;
  }
  return 0;
}

int text_ctrl_init(TControl *tc, unsigned cap)
{
	tc->cx=0;
	tc->cy=0;
	tc->sx=0;
	tc->sy=0;
	return text_init(&tc->text,cap);
}

int text_newline(TText *t, unsigned pos)
{
	TLine *newl;
	if( t->nlines >= t->cap )
		return 0;
	newl = (TLine *)malloc(sizeof(TLine));
	if(!newl)
		return 0;
	newl->len=0;
	if(pos<t->nlines) {
		memmove( &t->lines[pos+1], &t->lines[pos], (t->nlines-pos)*sizeof(TLine *) );
	}
	t->lines[pos]=newl;
	t->nlines++;
	return 1;
}

int text_ctrl_newline(TControl *tc)
{
	if(text_newline(&tc->text, tc->cy)) {
		tc->cx=0;
		return text_ctrl_down(tc);
	}
	return 0;
}

int text_ctrl_up(TControl *tc)
{
	if(tc->cy>0) {
		tc->cy--;
		gotoxy(tc->cx+1,tc->cy+1);
	}
	return 1;
}

int text_ctrl_left(TControl *tc)
{
	if(tc->cx>0) {
		tc->cx--;
		gotoxy(tc->cx+1,tc->cy+1);
	}
	return 1;
}

int text_ctrl_right(TControl *tc)
{
	TLine *l = tc->text.lines[tc->cy];
	if(!l) {
		tc->cx=0;
	} else if(tc->cx < l->len ) {
		tc->cx++;
	}
	gotoxy(tc->cx+1, tc->cy+1);
	return 1;
}


int text_ctrl_down(TControl *tc)
{
	if(tc->cy < tc->text.nlines) {
		tc->cy++;
		gotoxy(tc->cx+1,tc->cy+1);
	}
	return 1;
}

int text_ctrl_draw(TControl *tc)
{
	unsigned cy=0;
	unsigned min = tc->text.nlines <25 ? tc->text.nlines : 25;
	clrscr();
	for(cy=0;cy<min;cy++)
	{
		gotoxy(1,cy+1);
		text_ctrl_drawline(tc,cy);
	}
	for(;cy<25;cy++)
	{
		gotoxy(1,cy+1);
		printf("~");
	}
	gotoxy(tc->cx+1, tc->cy+1);
	return 0;
}

int text_ctrl_edit(TControl *tc, unsigned char ch)
{
	TLine *l;
	if(tc->cy>=tc->text.nlines)
	{
		if(!text_newline(&tc->text, tc->cy)) {
			return 0;
		}
	}
	l = tc->text.lines[tc->cy];
	//printf("Loc: %0X\n", l);
	if(l->len<MAXCOLS) {
		if(tc->cx==l->len) {
			l->contents[l->len]=ch;
			l->len++;
			tc->cx++;
		} else if (tc->cx>l->len) {
			memset(&(l->contents[l->len]),' ',tc->cx - l->len);
			l->contents[tc->cx]=ch;
			l->len=tc->cx+1;
			tc->cx++;
		} else {
			memmove(
				&(l->contents[tc->cx+1]),
				&(l->contents[tc->cx]),
				l->len-tc->cx
				);
			l->contents[tc->cx]=ch;
			l->len++;
			tc->cx++;
		}
	}
	return 1;
}

int text_ctrl_delete(TControl *tc, unsigned char ch)
{
	TLine *l;
	l = tc->text.lines[tc->cy];
	if(!l) {
		tc->cx=0;
	} else if (tc->cx > 0 && l->len>tc->cx) {
		memmove(
			&l->contents[tc->cx-1],
			&l->contents[tc->cx],
			l->len-tc->cx
		);
		l->len--;
		tc->cx--;
	} else if (l->len>0 && l->len==tc->cx) {
		l->len--;
		tc->cx--;
	} else {
		tc->cx=0;
	}
	text_ctrl_drawline(tc, tc->cy);
	return 1;
}

int text_ctrl_drawline(TControl *tc, unsigned pos)
{
	TLine *l;
	unsigned cx=0;
	unsigned min;
	if( pos >= tc->text.nlines )
		return 0;
	l=tc->text.lines[pos];
	if(!l)
		return 0;
	//printf("Len %u", l->len);
	//return 0;
	min = l->len < 79 ? l->len : 79;
	_wscroll=0;
	_setcursortype(_NOCURSOR);
	gotoxy(1,pos+1);
	for(cx=0;cx<min;cx++) {
		putch(l->contents[cx]);
	}
	for(;cx<79;cx++)
	{
		putch(' ');
	}
	gotoxy(tc->cx+1,pos+1);
	_setcursortype(_NORMALCURSOR);
	return 1;
}
