#include <stdio.h>
#include <conio.h>
#include <graphics.h>
#include <dos.h>
#include "widgets.h"
#include "graphsys.h"


void test()
{
//    gx->setcolor(2);
//    gx->setfillstyle(SOLID_FILL,2);
//    gx->bar(10,10,300,30);
	displayBackground();
}

char *dptr[8] = {
	"(empty)",
	"(empty)",
	"(empty)",
	"(empty)",
	"(empty)",
	"(empty)",
	"(empty)",
	"(empty)"};

char text[3000];


const char *lines =
"\
int gxConfirmBox(TObjectUI *parent, const char *title, const char *text, const char *label1, const char *label2)\n\
{\n\
  char *action;\n\
  TObjectUI *dlo=gxCreateDialogUI(NULL,-1,-1,220,80,title,text);\n\
  gxCreateButtonUI(dlo,dlo->w-200,dlo->h-30,70,20,label1,\"dl_op1\");\n\
  gxCreateButtonUI(dlo,dlo->w-80,dlo->h-30,70,20,label2,\"dl_op2\");\n\
  action=gxRunDialog(dlo, parent);\n\
  if(!strcmp(action,\"dl_op1\")) {\n\
    return 0;\n\
  }\n\
  else if(!strcmp(action,\"dl_op2\")) {\n\
    return 1;\n\
  }\n\
  return -1;\n\
}\n\
";


const char *lines2 = "RCL Productions 2017";

int container_proc(TObjectUI *o, TEvent *ev)
{
	if(ev->action) {
		if(!strcmp(ev->action,"load")) {
			gxMessageBox(o,"Quote 1...", dptr[0]);
		} else if(!strcmp(ev->action,"op1")) {
			gxMessageBox(o,"Quote 2...", dptr[1]);
		} else if(!strcmp(ev->action,"op2")) {
			gxMessageBox(o,"Quote 3...", dptr[2]);
		} else if(!strcmp(ev->action,"op3")) {
			gxMessageBox(o,"Quote 4...", dptr[3]);
		} else if(!strcmp(ev->action,"op4")) {
			gxMessageBox(o,"Quote 5...", dptr[4]);
		} else if(!strcmp(ev->action,"exit")) {
			int action=gxConfirmBox( o,
				"Queres Bazar?...",
				"Terminar o programa?",
				"Sim",
				"Nao"
				);
			if(action==0) {
				gxMessageBox(o,"Fuck Off...", "Escolheu Sim... :(");
				gxMessageBox(o,"It's The Final Countdown...", "3!");
				gxMessageBox(o,"It's The Final Countdown...", "2!");
				gxMessageBox(o,"It's The Final Countdown...", "1!");
			} else {
				ev->action=NULL;
				ev->from=NULL;
			}

		}
		else if(!strcmp(ev->action,"hscroll")) {
			float value = gxScrollGetValue(ev->from);
		}
		else if(!strcmp(ev->action,"vscroll")) {
			float value = gxScrollGetValue(ev->from);
		}
	}
}

TObjectUI *cvtButtons()
{
	TObjectUI *container=gxCreateObjectUI(NULL,0,0,gx->getmaxx(),gx->getmaxy(),NULL,desktop_draw,container_proc,0,0);
	if(container) {
		TButtonUI *buttons=uiButtons;
		int id=1;
		while(buttons->w) {
			gxCreateButtonUI(
				container,
				buttons->bx1,
				buttons->by1,
				buttons->bx2-buttons->bx1,
				buttons->by2-buttons->by1,
				buttons->caption,
				buttons->action
				);
			buttons++;
		}
		return container;
	}
	return NULL;
}

void dumpObjects(TObjectUI *container)
{
	if(container) {
		TObjectUI *chd=container->chdfirst;
		printf("<%06d> >>>>>>>>>>>\n", container->id);

		while(chd){
			dumpObjects(chd);
			chd=chd->next;
		}
		printf("<<<<<<<<<<<<<<<<<<<<\n");
	}
}

void load_disclaimers(const char *filename)
{
	int i=0,j=0;
	FILE *fp=fopen(filename,"rt");
	if(!fp) {
	  fprintf(stderr,"Cannot load %s: %s", filename, strerror(errno));
	  exit(2);
	}
	dptr[j]=&text[i];
	while(!feof(fp) && i<2999) {
		int ch=fgetc(fp);
		if(ch=='@') {
			text[i++]='\0';
			if(j<7)
				dptr[++j]=&text[i];
		}else {
			text[i++]=ch;
		}

	}
	text[i]='\0';
	fclose(fp);
}

int ctab[10]={1,8,2,8,1,
	      1,8,2,8,15};

int main()
{
   TObjectUI *container;
   TObjectUI *hsb;
   TObjectUI *vsb;

   int i=0;
   int mih=3;
   int mch=(10-1)*mih;

   int miv=8;
   int mcv=(10-1)*miv;

   load_disclaimers("contents.txt");
//   initgraphics("BGI_AUTO", "c:\\devtools\\tc\\bgi");

   if(!initgraphics("SVGA_800x600x256", NULL)) {
	return -2;
   }

   //getch();


   for(i=0;i<10;i++) {
	int o1, o2;

	gx->setcolor(ctab[i]);
	gx->settextstyle(DEFAULT_FONT, HORIZ_DIR,3);
	o1 = (gx->getmaxx()-gx->textwidth("Welcome to DeathStar"))/2-20;

	printlines(o1+i*mih,100+i*miv, "Welcome to DeathStar\n");
	gx->settextstyle(DEFAULT_FONT, HORIZ_DIR,2);
	o2 = (gx->getmaxx()-gx->textwidth("RCL Productions 2017 (c)"))/2;
	printlines(o2+i*mih,200+i*miv,"RCL Productions 2017 (c)\n");

	gx->settextstyle(DEFAULT_FONT, HORIZ_DIR,3);
	printlines(o1+mch*2-i*mih,100+i*miv,"Welcome to DeathStar\n");
	gx->settextstyle(DEFAULT_FONT, HORIZ_DIR,2);
	printlines(o2+mch*2-i*mih,200+i*miv,"RCL Productions 2017 (c)\n");

	delay(50);
   }


   gx->settextstyle(DEFAULT_FONT,HORIZ_DIR,0);
   sleep(1);
   uiButtonInitRow(uiButtons, (gx->getmaxx()-620)/2,40,20,10);
   container=cvtButtons();
   vsb=gxCreateScrollUI(container, gx->getmaxx()-50,100,20,100+gx->getmaxy()/2,0,0,100,50);
   hsb=gxCreateScrollUI(container,100,200+gx->getmaxy()/2,gx->getmaxx()-gx->getmaxx()/4,20,1,0,5,2);

   gxCreateTextUI(container,100,100,gx->getmaxx()-200,gx->getmaxy()-210,lines);

   gxRun(container);
   gx->cleardevice();
   printlines((gx->getmaxx()-540)/2,(gx->getmaxy()-180)/2,dptr[6]);
   sleep(2);
   gx->closegraph();
   printf("Bye!\n");
   return 0;
}
