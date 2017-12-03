#include "graphlib.c"
#include "mcosapi.h"
#include "midilib.c"

#define abs(X) ((X)<0?-(X):(X))

#define xrandom mrandom

#ifdef USE_RATO
	#include "rato2.c"
#endif
#include "mcosapi.h"

#define WAIT 0
#define PLAY 1
#define ANI 2
#define GO 3
#define PAUSA 4


#define ANF 9

#define PFXI 5
#define PFYI 0

#define PAC 0
#define GHOST 1

#define PFXD 31
#define PFYD 17

#define STOP 0
#define UP 1
#define DOWN 2
#define RIGHT 3
#define LEFT 4
#define ANY 5

#define H 0
#define V 1

#define BX 15
#define BY 11

#define MkColor(R,G,B) ((((R)&0x7)<<5) | (((G)&0x7)<<2) | (((B)&0x3)))

typedef unsigned int WORD;
typedef unsigned char BYTE;

typedef struct{
	int x,y;
	int ox,oy;
	char dir;
	char t;
	char st;
	int v;
}TPl;

struct{
	TPl m;
	int vc;
	char ld;
}Ghost[4];



struct{
	int level;
	char flagl;
	BYTE far *PFg;
	int PF_dots;
	char OVER;
	unsigned score;
	int pacs;
}Player[2];

BYTE far *LPFg=0L;

TPicture far *PFp[21];
TPicture far *Gp[6];

TPicture far *Bp;

struct{
	TPl m;
	TPicture far *pic[12];
}Pac;


struct{
	int st;
	int count;
	unsigned count2;
	char bonus;
	int ast;
	int plr;
}Game;

volatile BYTE StickA[7];
volatile BYTE StickB[7];
char far *VirScr;
char far *MusSeg;
unsigned MusicLen=0;
char LoadError=0;
void LoadAll();
void UnloadAll();
void Play_game();
void UpDateScreen();
void GameInit();
void AvaliaJoyStick();
void interrupt (*oldkeyb)(void);
void interrupt newkeyb(void);
void SetupMenu();
void Fundo();
void Sobre();
void Ajuda();
char Menu();

void Load_PF();
void Show_PF();
void Show_Pac();
void Show_Ghost(int g);
void Move(TPl *m);
void PacControl();
void GhostControl(int g);
void GhostColide(int g);
void GameControl();
int TestDir(char dir,TPl *m);
void SetDir(char dir,TPl *m);
void Vsync();
void DotColide();
void Kill();
void PlReset();
void MoveGhost(int g);
void GhostSetDH(int g);
void GhostSetDV(int g);
void Colides();
void BaseColide();
void ShowScore();
void ShowBonus();
void ControlBonus();
void BonusColide();
void ShowPopUp();
char ControlPopUp(int op);
char NextPlayer();
void SndC();
void SetSnd(int freq,int count, int bip);
void InitLevel(int l);
void NextLevel();
void SetSomOnOff();
void CpyPFg(BYTE far *dest,BYTE far *orig);
char TestGO();
#ifdef USE_RATO
	char RatoM(char dir);
#endif
char WaitToStart();


typedef struct {
	char id[11];
	unsigned char byteorder;
	unsigned char size[2];
} TResourceHeader;

typedef struct {
	char id[11];
	unsigned size;
	void far *ptr;
} TResourceMap;

int vcm=1000;
int GXI=15;
int GYI=7;
int NG=4;
int stl=1;
int frames=70;

int som=0;
int musica=0;
int pl=0;

int nivel[10][4]={1000,2,53,0,    //N0
						900,4,53,0,    //N1
						800,4,53,53,    //N2
						800,4,53,43,    //N3
						600,4,53,33,    //N4
						600,4,53,23,    //N5
						500,4,53,17,    //N6
						400,4,53,13,    //N7
						350,4,53,9,    //N8
						350,4,53,5};//N9
struct{
	int count;
	int freq;
	int Bip;
}Snd;

char VDir[4]={UP,DOWN,LEFT,RIGHT};

char SDir(char dir);

int PopUp;

int PF_dotsL;

struct{
	char m;
	int x,y,b;
}SM;


unsigned mrandom(unsigned mrandom)
{
	static x;
	static init;
	if(!init) {
		x=0x5124;
		init=1;
	}

	x ^= x << 7;
	x ^= x >> 11;
	x ^= x << 3;

	return x%mrandom;
}


#define MAXRESOURCES 50
TResourceMap rmap[MAXRESOURCES];
Trgb far *palette=0L;

int loadResourceFile(const char *filename, TResourceMap *rmap, int max);
TResourceMap *getResource(const char *id, TResourceMap *rmap, int max);

void setpali(char i, char r, char g, char b)
{
		asm {
			mov dx, 0x3c8;
			mov al, i; out dx, al;
			mov dx, 0x3c9;
			mov al, r; out dx, al;
			mov al, g; out dx, al;
			mov al, b; out dx, al;
		}
}

void setpal(Trgb far *pal)
{
	int i;
	for(i=0;i<256;i++)
	{
		setpali(i, pal[i].R/(256/64), pal[i].G/(256/64), pal[i].B/(256/64));
	}
}


int loadResourceFile(const char *filename, TResourceMap *rmap, int max)
{
	int handle;
	int n=0;
	unsigned size=0;
	char buffer[12];
	TResourceHeader rh;
	int ok=1;
	if( (handle=mcos_open(filename,READMODE)) == -1 ) {
		printf("Cannot open resource file %s\n", filename);
		return 0;
	}

	while(ok)
	{
		if(!mcos_read(handle,&rh,sizeof(rh)))
			break;
		size=rh.size[0]+rh.size[1]*256;
		strcpy(rmap[n].id, rh.id);
		rmap[n].size=size;
		rmap[n].ptr=0L;
		//printf("Resource id %s, size %u\n", rh.id, size);
		
		if(size) {
			rmap[n].ptr = farmalloc(size);
			if(!rmap[n].ptr) {
				ok=0;
				printf("Out of memory %u\r\n",size);
				break;
			}
			if(!mcos_read(handle,rmap[n].ptr,size))
				break;
		}
		n++;
		
		if(rh.id[0]=='R' && rh.id[1]=='E' && rh.id[2]=='$') {
			break;
		}
		
		if(n>=max)
			break;
	}
	
	mcos_close(handle);
	return n;
}


TResourceMap *getResource(const char *id, TResourceMap *rmap, int max)
{
	unsigned i;
	for(i=0;i<max;i++)
	{
		if(!strcmp(rmap[i].id, id))
			return &rmap[i];
	}
	return 0;
}

void DrawNextPic(TPicture far *ptr)
{
	unsigned w,h;
	static curx;
	static cury;
	static maxh;

	if(!ptr)
		return;
	w=ptr->w;
	h=ptr->h;
	if(curx+w>319)
	{
		curx=0;
		cury+=maxh;
		maxh=0;
	}
	if(h>maxh)
		maxh=h;
	DrawPic(curx,cury,ptr);
	curx+=w;
}


void drawpics()
{
	TResourceMap *rs;
	char buffer[12];
	int i;

	if(!(rs=getResource("Paleta",rmap,MAXRESOURCES))) {
		printf("Resource not found Paleta\n");
		LoadError=1;
		return;
	}
	
	if(rs->size!=256*sizeof(Trgb)) {
		printf("Wrong palette data size expected %u got %u\n",256*sizeof(Trgb), rs->size);
		return;
	}

	palette=rs->ptr;	
	if(palette)
		setpal(palette);

	
	if(!(rs=getResource("BonusPic00",rmap,MAXRESOURCES))) {
		printf("Resource not found BonusPic00\n");
		//LoadError=1;
		return;
	} else 
	if(rs->ptr) {
		DrawNextPic(rs->ptr);
	}
	
	for(i=0;i<12;i++)
	{
		strcpy(buffer,"PacPic00");
		buffer[6]=(i/10)%10+'0';
		buffer[7]=i%10+'0';
		if(!(rs=getResource(buffer,rmap,MAXRESOURCES))) {
			printf("Resource not found %s\n", buffer);
			LoadError=1;
			return;
		}
		DrawNextPic(rs->ptr);
	}

	for(i=0;i<6;i++)
	{
		strcpy(buffer,"GhostPic00");
		buffer[8]=(i/10)%10+'0';
		buffer[9]=i%10+'0';

		if(!(rs=getResource(buffer,rmap,MAXRESOURCES))) {
			printf("Resource not found %s\n", buffer);
			LoadError=1;
			return;
		}
		DrawNextPic(rs->ptr);
	}
	
	for(i=0;i<21;i++)
	{
		strcpy(buffer,"WallPic00");
		buffer[7]=(i/10)%10+'0';
		buffer[8]=i%10+'0';

		if(!(rs=getResource(buffer,rmap,MAXRESOURCES))) {
			printf("Resource not found %s\n", buffer);
			LoadError=1;
			return;
		}
		DrawNextPic(rs->ptr);
	}

}

int main()
{
	BYTE op;
	SM.m=0;
	#ifdef USE_RATO
	if(CheckRato())
	{
		SM.m=1;
		SetRato(100,100);
		GetRato(&SM.x,&SM.y,&SM.b);
	}
	#endif
	for(op=0;op<4;op++)
		Ghost[op].m.t=GHOST;
	som=1;
	Pac.m.t=PAC;
	PopUp=0;
	LoadAll();
	Midi_Init();
	if(!SBMIDI)
		musica=0;
	else
		musica=2;
	modo13h();
	setpal(palette);
	UserScreen(VirScr);

	do{
		op=Menu();
		switch(op)
		{
			case 0:Play_game();break;
			case 1:SetupMenu();break;
			case 2:Ajuda();break;
			case 3:Sobre();break;
		}
	}while(op!=4);
	
	modo3h();
	UnloadAll();
	return 0;
}

void newkeyb2(void);

void Play_game()
{

	char done=0;
	int set;
	int frcnt=0;
	int div=1;
	unsigned hsecs;
	unsigned oldclk;
	void far * far *intrtab=0x0;

	setpal(palette);
	GameInit();
	
	asm cli;
	oldkeyb=intrtab[0x9];
	intrtab[0x9]=MK_FP(_CS,newkeyb);
	asm sti;
	
	oldclk=gethseconds();
	printf("HSecs: %u\n", oldclk);
	getch();
/*	oldkeyb=GetIntVect(0x9);
	SetIntVect(0x9,newkeyb);
*/
	if(musica==2) Midi_Play(MusSeg);
	UpDateScreen();

	while(!done)
	{
		set=10;
		if((musica==2)&&(Midi_Status()==0)) Midi_Play(MusSeg);
		Game.count++;


		switch(Game.st)
		{
			case PLAY:
				Game.count2++;
				GameControl();
				Colides();
				if(Player[Game.plr].PF_dots==0)
					NextLevel();
				AvaliaJoyStick();
				break;
			case ANI:
				Kill();
				break;

		}

		if(kbhit())
		{
			switch(getch())
			{
				case 27:switch(Game.st)
							{
								case PAUSA:set=ControlPopUp(3);break;
								case WAIT:
								case PLAY:Game.st=PAUSA;PopUp=0;break;
								default:set=3;break;
							}
							break;
				case 0:switch(getch())
						{
							case 59:if(Game.st!=PAUSA) set=1;break;
							case 60:set=2;break;
							case 61:som=!som;break;
							case 80:if(Game.st==PAUSA)
											set=ControlPopUp(0);
									  break;
							case 72:if(Game.st==PAUSA)
											set=ControlPopUp(1);
									  break;
						}
						break;
				case 13:if(Game.st==PAUSA)
								set=ControlPopUp(2);
							break;

			}

		}
		if(!done)
			switch(set)
			{
				case 1:GameInit();break;
				case 2:if(pl==0)
							pl=1;
						 else
						 {
							pl=0;
							if(stl<9)
								stl++;
							else
								stl=0;
						 }
						 GameInit();
						break;
				case 4:done=1;break;
				case 5:Game.st=WAIT;break;
				default:if((WaitToStart())||(set==3))
							{
								switch(Game.st)
								{
									case WAIT:Game.st=PLAY;break;
									case GO:GameInit();break;
								}
							}
			}
		SndC();
		
		UpDateScreen();
		frcnt++;

		hsecs=gethseconds();
		if(hsecs-oldclk>100)
		{
			frames=frcnt;
				
			frcnt=0;
			oldclk=hsecs;
		}
	}
	nosound();
	if(musica==2) Midi_Stop(MusSeg);
	asm cli;
	intrtab[0x9]=oldkeyb;
	asm sti;
/*	SetIntVect(0x9,oldkeyb); */
}


void Ajuda()
{
	/*
	BYTE tecla;
	int handle,bytes;
	unsigned p100;
	unsigned pos;
	unsigned max;
	BYTE caracter;
	handle=_open("misc/PacMan.hlp",O_RDONLY|O_BINARY);
	textmode(3);
	SetOverScan(5);
	if(handle==-1)
	{
		puts("Erro ao abrir ficheiro de ajuda.");
		getch();
	}
	else
	{
		do
		{
			textcolor(7);
			textbackground(1);
			clrscr();
			textcolor(2);
			textbackground(4);
			gotoxy(1,1);
			clreol();
			cprintf(" P a c   M a n");
			gotoxy(75,1);
			cprintf("Ajuda\r\n");
			textcolor(7);
			textbackground(1);
			do
			{
				bytes=_read(handle,&caracter,1);
				if(bytes)putch(caracter);
			}while((bytes>0)&&(wherey()!=25));
			textcolor(2);
			textbackground(4);
			gotoxy(1,25);
			clreol();
			max=filelength(handle);
			pos=tell(handle);
			asm {
				push ax;
				push cx;
				push dx;
				mov cx,100;
				xor dx,dx;
				mov ax,pos;
				mul cx;
				div max;
				mov p100, ax;
				pop dx;
				pop cx;
				pop ax;
			}
			gotoxy(75,25);
			cprintf("%03u%%",p100);
			gotoxy(1,25);
			if(bytes)
			{
				cputs("-- Mais --");
				tecla=getch();
				if(tecla==0) tecla=getch();
				if(tecla==27) break;
			}
		}while(bytes>0);
		_close(handle);
		if(tecla!=27)
		while(getch()!=27);
	}
	modo13h();
	setpal(palette);
	*/
}


void Sobre()
{
	Fundo();
	TextColor=MkColor(0,0,2);
	writest(50,70,"VERSAO: ");
	writest(50,100,"AUTOR: ");
	writest(50,130,"DATA: ");
	writest(190,70,"1.0");
	writest(150,100,"RENATO LOURENCO");
	writest(160,130,"FEVEREIRO 2002");
	TextColor=MkColor(4,0,0);
	writest(51,71,"VERSAO: ");
	writest(51,101,"AUTOR: ");
	writest(51,131,"DATA: ");
	writest(191,71,"1.0");
	writest(151,101,"RENATO LOURENCO");
	writest(161,131,"FEVEREIRO 2002");
	ScrnCpyUD();
	getch();
}


void Fundo()
{
	int c;
	fillscreen(0);
	for(c=0;c<300;c++)
		DrawPixel(xrandom(320),xrandom(180)+20,xrandom(255));
	Color=MkColor(2,2,2);
	DrawRect(0,0,319,199,LINE);
	Color=MkColor(0,0,3);
	FillColor=MkColor(5,0,0);
	DrawRect(1,1,318,20,LIFI);
	TextColor=MkColor(0,0,0);
	writest(100,7,"P A C   M A N");
	TextColor=MkColor(0,7,0);
	writest(101,8,"P A C   M A N");
}

char Menu()
{
	BYTE done=0,op=0;
	BYTE tecla;
	Fundo();
	FillColor=MkColor(4,0,0);
	Color=MkColor(6,0,0);
	DrawRect(100,60,220,80,LIFI);
	TextColor=MkColor(0,0,1);
	writest(143,66,"MENU");
	TextColor=MkColor(0,0,3);
	writest(144,67,"MENU");
	while(!done)
	{
		FillColor=MkColor(0,0,0);
		Color=MkColor(7,0,0);
		DrawRect(100,82,220,160,LIFI);
		TextColor=MkColor(0,7,0);
		writest(140,90,"JOGAR");
		writest(140,104,"SETUP");
		writest(140,118,"AJUDA");
		writest(140,132,"SOBRE");
		writest(140,146,"SAIR");
		Color=MkColor(1,6,1);
		DrawRect(105,88+14*op,215,98+14*op,LINE);
		ScrnCpyUD();
		tecla=getch();
		switch(tecla)
		{
			case 13:done=1;break;
			case 0:switch(getch())
					 {
						case 72:if(op==0)
										op=4;
									else
										op--;
									break;
						case 80:if(op==4)
										op=0;
									else
										op++;
									break;
					 }
					 break;
		}
	}
	return op;
}


void SetupMenu()
{
	BYTE done=0,op=0,sd,niv,p,mus;
	BYTE st[4],tecla;
	st[3]=0;
	mus=musica;
	sd=som;
	niv=stl;
	p=pl;
	Fundo();
	FillColor=MkColor(2,2,1);
	Color=MkColor(3,3,2);
	DrawRect(100,56,220,76,LIFI);
	TextColor=MkColor(0,0,1);
	writest(139,63,"SETUP");
	TextColor=MkColor(1,1,3);
	writest(140,64,"SETUP");
	while(!done)
	{
		FillColor=MkColor(0,0,0);
		Color=MkColor(5,5,2);
		DrawRect(100,78,220,162,LIFI);
		TextColor=MkColor(0,0,2);
		writest(110,82,"NIVEL");
		writest(110,96,"PLAYERS");
		writest(110,110,"MUSICA");
		writest(110,124,"SOM");
		TextColor=MkColor(0,4,0);
		writest(110,138,"ACEITAR");
		TextColor=MkColor(4,0,0);
		writest(110,152,"CANCELAR");
		st[2]=0;
		st[1]=niv%10+48;
		st[0]=niv/10+48;
		TextColor=MkColor(0,5,0);
		writest(190,82,st);
		st[0]=' ';
		if(p)
			st[1]='2';
		else
			st[1]='1';

		st[2]=0;
		writest(190,96,st);
		switch(mus)
		{
			case 0:TextColor=MkColor(2,2,1);st[0]='N';st[1]='/';st[2]='A';break;
			case 1:TextColor=MkColor(5,0,0);st[0]='O';st[1]='F';st[2]='F';break;
			case 2:TextColor=MkColor(0,5,0);st[0]='O';st[1]='N';st[2]=' ';break;
		}
		writest(190,110,st);
		if(sd)
		{
			TextColor=MkColor(0,5,0);
			st[0]='O';st[1]='N';st[2]=' ';
		}
		else
		{
			TextColor=MkColor(5,0,0);
			st[0]='O';st[1]='F';st[2]='F';
		}
		writest(190,124,st);
		Color=MkColor(5,5,0);
		DrawRect(105,80+14*op,215,90+14*op,LINE);
		ScrnCpyUD();
		tecla=getch();
		switch(tecla)
		{
			case 13:switch(op)
						{
							case 0:if(niv<9)
										niv++;
									else
										niv=0;
									break;
							case 2:switch(mus)
									{
										case 1:mus=2;break;
										case 2:mus=1;break;
									}
									break;
							case 1:p=!p;break;
							case 3:sd=!sd;break;
							case 4:stl=niv;
									pl=p;
									musica=mus;
									som=sd;
							case 5:done=1;break;
						}
						break;
			case 0:switch(getch())
					 {
						case 72:if(op==0)
										op=5;
									else
										op--;
									break;
						case 80:if(op==5)
										op=0;
									else
										op++;
									break;
					 }
		}
	}
}

void SetSomOnOff()
{
	switch(musica)
	{
		case 1:
				musica=2;
				Midi_Play(MusSeg);
				break;
		case 2:
				musica=1;
				Midi_Stop(MusSeg);
				break;
	}
}

#ifdef USE_RATO
char RatoM(char dir)
{
	int x,y,b;
	int r=0;
	if(!SM.m) return 0;
	GetRato(&x,&y,&b);
	switch(dir)
	{
		case ANY:if((abs(x-SM.x)>5)||(abs(y-SM.y)>5)) r=1;break;
		case LEFT:if(SM.x-x>5) r=1;break;
		case RIGHT:if(x-SM.x>5) r=1;break;
		case UP:if(SM.y-y>5) r=1;break;
		case DOWN:if(y-SM.y>5) r=1;break;

	}
	if(r)
	{
		SetRato(100,100);
		GetRato(&SM.x,&SM.y,&SM.b);
	}
	return r;
}
#endif

char WaitToStart()
{
	#ifdef USE_RATO
	if(RatoM(ANY))
		return 1;
	#endif
  if(pl)
  {
	switch(Game.plr)
	{
		case 0:if(StickA[0]||StickA[1]) return 1;break;
		case 1:if(StickB[0]||StickB[1]) return 1;break;
	}
  }
  else
  {
	if(StickB[0]||StickB[1]||StickA[0]||StickA[1]) return 1;
  }
  return 0;
}

void CpyPFg(BYTE far *dest,BYTE far *orig)
{
	int i;
	int j;
	for(i=0;i<PFYD;i++)
		for(j=0;j<PFXD;j++)
		{
			dest[i*PFXD+j]=orig[i*PFXD+j];
		}
}




int mainx()
{
	modo13h();
	fillscreen(0);
    printf("Hello World!\r\n");
	FillColor=40;
	Color=14;
	TextColor=14;
	DrawRect(10,10,40,40,LIFI);
	writest(100,50,"Hello World!");
	mcos_getkey();
	modo3h();
	loadResourceFile("pacman.res",rmap,MAXRESOURCES);
	printf("Press a key\r\n");
	mcos_getkey();
	modo13h();
	drawpics();
	mcos_getkey();
	modo3h();
    return 0;
}

void Show_Ghost(int g)
{
	int w;
	int x,y;
	w=vcm/4;
	x=Ghost[g].m.x*10+PFXI+Ghost[g].m.ox;
	y=Ghost[g].m.y*10+PFYI+Ghost[g].m.oy;
	switch(Ghost[g].m.st)
	{
		case 0:if(Game.count%2==0)
							DrawPic(x,y,Gp[g]);
					break;
		case 1:if(Ghost[g].vc>w)
						DrawPic(x,y,Gp[4]);
				else
				{
					if(Game.count%10<5)
							DrawPic(x,y,Gp[4]);
					else
							DrawPic(x,y,Gp[g]);
					break;
				}
				break;
		case 2:DrawPic(x,y,Gp[5]);
	}
}

void Show_Pac()
{
	int p;
	switch(Game.ast)
	{
			case 0:switch(Pac.m.st)
					 {
							case 0:p=0;break;
							case 1:p=1;break;
							case 2:p=2;break;
							case 3:p=3;break;
							default:p=0;
					 }
					 if(Game.plr)
						p+=8;
					 break;
			case 1:
			case 2:p=4;break;
			case 3:
			case 4:p=5;break;
			case 5:
			case 6:p=6;break;
			case 7:
			case 8:
			default:p=7;
	}

	DrawPic(Pac.m.x*10+PFXI+Pac.m.ox,Pac.m.y*10+PFYI+Pac.m.oy,Pac.pic[p]);
}

void DotColide()
{
	int c;
	BYTE far *PFg=Player[Game.plr].PFg;
	if(PFg[Pac.m.y*PFXD+Pac.m.x]=='x')
	{
		Player[Game.plr].PF_dots--;
		Player[Game.plr].score++;
		PFg[Pac.m.y*PFXD+Pac.m.x]='z';
		SetSnd(50,1,1);
	}
	if(PFg[Pac.m.y*PFXD+Pac.m.x]=='y')
	{
		Player[Game.plr].score+=5;
		Player[Game.plr].PF_dots--;
		SetSnd(100,5,1);
		for(c=0;c<NG;c++)
		{
			if(Ghost[c].m.st!=2)
			{
				Ghost[c].m.st=1;
				Ghost[c].vc=vcm;
			}
		}
		PFg[Pac.m.y*PFXD+Pac.m.x]='z';
	}

}

void GhostColide(int g)
{
	int gx,gy,px,py;
	px=Pac.m.x*10+Pac.m.ox;
	py=Pac.m.y*10+Pac.m.oy;
	gx=Ghost[g].m.x*10+Ghost[g].m.ox;
	gy=Ghost[g].m.y*10+Ghost[g].m.oy;
	if((abs(px-gx)<5)&&(abs(py-gy)<5))
	switch(Ghost[g].m.st)
	{
		case 0:Game.st=ANI;/*Player[Game.plr].pacs--;*/break;
		case 1:Ghost[g].m.st=2;Player[Game.plr].score+=10;SetSnd(500,10,1);break;
	}
}

void BaseColide(int g)
{
	int gx,gy,px,py;
	px=GXI*10;
	py=GYI*10;
	gx=Ghost[g].m.x*10+Ghost[g].m.ox;
	gy=Ghost[g].m.y*10+Ghost[g].m.oy;
	if((abs(px-gx)<5)&&(abs(py-gy)<5))
	if(Ghost[g].m.st==2)
	{
		Ghost[g].m.st=0;
	}
}

int TestDir(char dir,TPl *m)
{
	int x=m->x,y=m->y;
	BYTE far *PFg=Player[Game.plr].PFg;
	if(dir==STOP)
		return 0;
	if((x<0)||(x>=PFXD)||(y<0)||(y>=PFYD))
		return 0;
	switch(dir)
	{
		case UP:if(y>0) y--;break;
		case DOWN:if(y<PFYD-1) y++;break;
		case LEFT:if(x>0) x--;break;
		case RIGHT:if(x<PFXD-1) x++;break;
	}
	switch(PFg[y*PFXD+x])
	{
		case 'x':
		case 'y':
		case 'z':return 1;break;
		case 'r':if(m->t==GHOST)
						return 1;
					else
						return 0;
				  break;
		default:return 0;
	}
	return 0;
}

void Move(TPl *m)
{
		int d1=1,d2=1;
		if(m->v)
		{
			if(Game.count%m->v==0)
				d2=d1=2;
		}
		if(m->ox%2)
			d1=1;
		if(m->oy%2)
			d2=1;
		switch(m->dir)
		{
			case LEFT:m->ox-=d1;break;
			case RIGHT:m->ox+=d1;break;
			case UP:m->oy-=d2;break;
			case DOWN:m->oy+=d2;break;
		}
		if(m->ox<-4)
		{
				m->x--;
				m->ox+=10;
		}
		if(m->oy<-4)
		{
				m->y--;
				m->oy+=10;
		}
		if(m->oy>5)
		{
				m->y++;
				m->oy-=10;
		}
		if(m->ox>5)
		{
				m->x++;
				m->ox-=10;
		}
		if(m->y<0)
			m->y=16;
		if(m->y>=PFYD)
			m->y=0;
		switch(m->dir)
		{
			case LEFT:
			case RIGHT:if(m->ox==0)
								if(!TestDir(m->dir,m)) m->dir=STOP;
							break;
			case UP:
			case DOWN:if(m->oy==0)
								if(!TestDir(m->dir,m)) m->dir=STOP;
							break;
		}
}

void GameInit()
{

	Player[0].score=0;
	Player[1].score=0;
	Player[0].OVER=0;
	Player[1].OVER=0;
	Player[0].level=-1;
	Player[1].level=-1;
	Player[0].pacs=3;
	Player[1].pacs=4;
	Player[0].flagl=0;
	Player[1].flagl=0;
	Game.count2=0;
	Game.bonus=0;
	Game.plr=0;
	InitLevel(stl);
}


void InitLevel(int l)
{
	CpyPFg(Player[Game.plr].PFg,LPFg);
	Player[Game.plr].PF_dots=PF_dotsL;
	SetSnd(0,0,0);
	if(l<0) l=0;
	if(l>9) l=9;
	Player[Game.plr].level=l;
	PlReset();
}

void NextLevel()
{
	if(Player[Game.plr].flagl)
	{
		if(Player[Game.plr].pacs<3)
			Player[Game.plr].pacs++;
		Player[Game.plr].flagl=0;
	}
	else
		Player[Game.plr].flagl=1;
	InitLevel(Player[Game.plr].level+1);
}

void SndC()
{
	if(Snd.count)
	{

		if(Snd.count-1<=0)
			nosound();
		else if(som)
			switch(Snd.Bip)
			{
				case 1:sound(Snd.freq*Snd.count);break;
				case 2:sound(Snd.freq/Snd.count);break;
				default:sound(Snd.freq);break;
			}
		Snd.count--;
	}
	if(Snd.count<0)
		Snd.count=0;


}
void SetSnd(int freq,int count, int bip)
{
	if(count<Snd.count)
		return;
	Snd.freq=abs(freq);
	Snd.count=abs(count)+1;
	Snd.Bip=bip;
}


void PlReset()
{
	int c;
	Game.st=WAIT;
	Game.ast=0;
	Game.count=0;
	Pac.m.st=3;
	Pac.m.dir=STOP;
	Pac.m.x=15;
	Pac.m.y=13;
	Pac.m.v=nivel[Player[Game.plr].level][2];
	vcm=nivel[Player[Game.plr].level][0];
	NG=nivel[Player[Game.plr].level][1];
	for(c=0;c<NG;c++)
	{
		Ghost[c].m.st=0;
		Ghost[c].m.x=15;
		Ghost[c].m.y=7;
		Ghost[c].m.ox=0;
		Ghost[c].m.oy=0;
		Ghost[c].m.dir=STOP;
		Ghost[c].m.v=nivel[Player[Game.plr].level][3];
		Ghost[c].ld=H;
	}
	Pac.m.ox=0;
	Pac.m.oy=0;
}

void SetDir(char dir,TPl *m)
{
	switch(dir)
	{
		case LEFT:
		case RIGHT:if(m->oy==0)
							if(TestDir(dir,m)) m->dir=dir;
						break;
		case UP:
		case DOWN:if(m->ox==0)
							if(TestDir(dir,m)) m->dir=dir;
						break;
	}
	if(m->t==PAC)
		switch(m->dir)
		{
			case LEFT:m->st=m->st|0x2;break;
			case RIGHT:m->st=m->st&(~(0x2));break;
		}
}


void ShowBonus()
{
	if(Game.bonus)
	{
		DrawPic(BX*10+PFXI,BY*10+PFYI,Bp);
	}
}

void ControlBonus()
{
	if(Game.count2%2000==1500)
	{
		Game.bonus=1;
	}
	if((Game.bonus)&&(Game.count2%2000<1500))
		Game.bonus=0;
}

void BonusColide()
{
	if(!Game.bonus)
		return;
	if((Pac.m.x==BX)&&(Pac.m.y==BY))
	{
		Player[Game.plr].score+=20;
		Game.bonus=0;
		SetSnd(500,5,1);
	}
}


void ShowPopUp()
{
	char st[4];
	st[3]=0;
	if(Game.st!=PAUSA)
		return;
	FillColor=MkColor(2,2,1);
	Color=MkColor(0,5,0);
	DrawRect(115,55,205,109,LIFI);
	TextColor=MkColor(0,0,3);
	writest(125,63,"CONTINUAR");
	writest(125,74,"MUSICA");
	writest(125,85,"SOM");
	writest(125,96,"TERMINAR");
	switch(musica)
	{
			case 0:TextColor=MkColor(5,5,2);st[0]='N';st[1]='/';st[2]='A';break;
			case 1:TextColor=MkColor(5,0,0);st[0]='O';st[1]='F';st[2]='F';break;
			case 2:TextColor=MkColor(0,5,0);st[0]=' ';st[1]='O';st[2]='N ';break;
	}
	writest(173,74,st);
	if(som)
	{
		TextColor=MkColor(0,5,0);st[0]=' ';st[1]='O';st[2]='N ';
	}
	else
	{
		TextColor=MkColor(5,0,0);st[0]='O';st[1]='F';st[2]='F';
	}
	writest(170,85,st);
	Color=MkColor(0,6,0);
	DrawRect(118,61+11*PopUp,202,71+11*PopUp,LINE);
}

char ControlPopUp(int op)
{
	char r=0;
	if(Game.st!=PAUSA) return 0;
	switch(op)
	{
		case 1:if(PopUp>0) PopUp--; else PopUp=3; break;
		case 0:if(PopUp<3) PopUp++; else PopUp=0; break;
		case 2:switch(PopUp)
				{
					case 1:SetSomOnOff();break;
					case 2:som=!som;break;
					case 0:r=5;break;
					case 3:r=4;break;
				}
				break;
		case 3:PopUp=3;r=5;break;
	}
	return r;
}


void Colides()
{
	int c;
	for(c=0;c<NG;c++)
	{
		GhostColide(c);
		if(Game.st!=PLAY)
			break;
	}
	for(c=0;c<NG;c++)
		BaseColide(c);
	BonusColide();
	if(Game.st==PLAY)
		DotColide();
}

void GhostControl(int g)
{
	if(Ghost[g].m.st==1)
	{
		Ghost[g].vc--;
		if(Ghost[g].vc<=0)
			Ghost[g].m.st=0;
	}
	MoveGhost(g);
	Move(&Ghost[g].m);
}

void PacControl()
{
	if(Game.count%10==0) Pac.m.st=Pac.m.st^0x1;
	Move(&Pac.m);
}

void GameControl()
{
	int c;
	PacControl();
	ControlBonus();
	for(c=0;c<NG;c++)
		GhostControl(c);

}

void Kill()
{
	if(Game.count%5) return;
	if(Game.ast<=ANF)
	{
		if(som) sound(500-40*Game.ast);
		Game.ast++;
	}
	else
	{
		nosound();
		if(Player[Game.plr].pacs<1)
			Player[Game.plr].OVER=1;

		if(TestGO())
			Game.st=GO;
		else
		{
			if(Player[NextPlayer()].OVER==0)
				Game.plr=NextPlayer();
			Player[Game.plr].pacs--;
			if(Player[Game.plr].level==-1)
				InitLevel(stl);
			else
				PlReset();
		}

	}
}

char NextPlayer()
{
	char next=Game.plr;

	if(pl)
	switch(Game.plr)
	{
		case 0:next=1;break;
		case 1:next=0;break;
	}
	return next;
}

char TestGO()
{
	if(pl)
	{
		if((Player[0].OVER)&&(Player[1].OVER))
			return 1;
		else
			return 0;
	}
	else
	{
		if(Player[Game.plr].OVER)
			return 1;
		else
			return 0;
	}
}

void AvaliaJoyStick()
{
	if((!pl)||(Game.plr==1))
	{
		if(StickB[0]) SetDir(LEFT,&Pac.m);
		if(StickB[1]) SetDir(RIGHT,&Pac.m);
		if(StickB[2]) SetDir(UP,&Pac.m);
		if(StickB[3]) SetDir(DOWN,&Pac.m);
	}

	if((!pl)||(Game.plr==0))
	{
		if(StickA[0]) SetDir(LEFT,&Pac.m);
		if(StickA[1]) SetDir(RIGHT,&Pac.m);
		if(StickA[2]) SetDir(UP,&Pac.m);
		if(StickA[3]) SetDir(DOWN,&Pac.m);
	}

	#ifdef USE_RATO
	if(RatoM(LEFT)) SetDir(LEFT,&Pac.m);
	if(RatoM(RIGHT)) SetDir(RIGHT,&Pac.m);
	if(RatoM(UP)) SetDir(UP,&Pac.m);
	if(RatoM(DOWN)) SetDir(DOWN,&Pac.m);
	#endif
}

void GhostSetDH(int g)
{
	switch(Ghost[g].m.st)
	{
		case 0:
				if(Pac.m.x>Ghost[g].m.x)
				{
					SetDir(SDir(RIGHT),&Ghost[g].m);
				}
				if(Pac.m.x<Ghost[g].m.x)
				{
					SetDir(SDir(LEFT),&Ghost[g].m);
				}
			break;
		case 1:
				if(Pac.m.x<=Ghost[g].m.x)
				{
					SetDir(SDir(RIGHT),&Ghost[g].m);
				}
				if(Pac.m.x>=Ghost[g].m.x)
				{
					SetDir(SDir(LEFT),&Ghost[g].m);
				}
			break;
		case 2:
				if(GXI>Ghost[g].m.x)
				{
					SetDir(RIGHT,&Ghost[g].m);
				}
				if(GXI<Ghost[g].m.x)
				{
					SetDir(LEFT,&Ghost[g].m);
				}
			break;
	}
}

void GhostSetDV(int g)
{
	switch(Ghost[g].m.st)
	{
		case 0:
				if(Pac.m.y>Ghost[g].m.y)
				{
					SetDir(SDir(DOWN),&Ghost[g].m);
				}
				if(Pac.m.y<Ghost[g].m.y)
				{
					SetDir(SDir(UP),&Ghost[g].m);
				}
			break;
		case 1:
				if(Pac.m.y<=Ghost[g].m.y)
				{
					SetDir(SDir(DOWN),&Ghost[g].m);
				}
				if(Pac.m.y>=Ghost[g].m.y)
				{
					SetDir(SDir(UP),&Ghost[g].m);
				}
			break;
		case 2:
				if(GYI>Ghost[g].m.y)
				{
					SetDir(DOWN,&Ghost[g].m);
				}
				if(GYI<Ghost[g].m.y)
				{
					SetDir(UP,&Ghost[g].m);
				}
			break;
	}
}


void MoveGhost(int g)
{
	char flag=0;
	if(Ghost[g].m.dir==STOP)
		flag=1;
	if(Game.count%200==0)
		flag=1;
	if((xrandom(10)<5)||(Ghost[g].m.st==2))
	{
		if((Ghost[g].ld==H)&&(Ghost[g].m.ox==0))
			flag=1;
		if((Ghost[g].ld==V)&&(Ghost[g].m.oy==0))
			flag=1;
	}
	if(!flag)
			return;
	if(Ghost[g].ld==H)
	{
		GhostSetDV(g);
	}
	else
	{
		GhostSetDH(g);
	}

	if(Ghost[g].m.dir==STOP) SetDir(UP,&Ghost[g].m);
	if(Ghost[g].m.dir==STOP) SetDir(LEFT,&Ghost[g].m);
	if(Ghost[g].m.dir==STOP) SetDir(RIGHT,&Ghost[g].m);
	if(Ghost[g].m.dir==STOP) SetDir(DOWN,&Ghost[g].m);
	switch(Ghost[g].m.dir)
	{
		case LEFT:
		case RIGHT:Ghost[g].ld=H;break;
		case UP:
		case DOWN:Ghost[g].ld=V;break;
	}
}

char SDir(char dir)
{
	if(xrandom(20)<15)
		return dir;
	else
		return(VDir[xrandom(4)]);
}


void writenumber(char *buffer, unsigned d, unsigned r)
{
  char t[6];
  unsigned rm=0;
  t[0] = (d/10000)%10+'0';
  t[1] = (d/1000)%10+'0';
  t[2] = (d/100)%10+'0';
  t[3] = (d/10)%10+'0';
  t[4] = d%10+'0';
  t[5] = '\0';

  if(r>5)
	  r=5;
  if(r<1)
	  r=1;
	while(t[rm]=='0')
	  rm++;

  if(r>5-rm)
	  rm=5-r;
  if(rm>5)
	  rm=5;
  strcpy(buffer,&t[rm]);
}


void ShowScore()
{
	char st[80];
	char cor[2]={MkColor(0,6,0),MkColor(0,6,3)};
	int i,c,off=0;
	TextColor=MkColor(2,2,1);
	writest(12,180,"Pac Man");
	TextColor=MkColor(6,6,0);
	writest(13,181,"Pac Man");
	if(pl)
		off=-5;
	for(i=0;i<=pl;i++)
	{
		off+=i*13;
		if(i==Game.plr)
		{
			Color=MkColor(0,6,0);
			DrawRect(107,177+off,314,190+off,LINE);
			Color=MkColor(5,0,0);
			DrawRect(106,176+off,313,189+off,LINE);
		}
		writenumber(st,Player[i].score,5);
		//sprintf(st,"%05d",Player[i].score);
		
		TextColor=MkColor(2,2,1);
		writest(112,180+off,st);
		TextColor=cor[i];
		writest(113,181+off,st);
		writenumber(st,Player[i].level!=-1?Player[i].level:stl,2);
		//sprintf(st,"%02d",Player[i].level!=-1?Player[i].level:stl);
		TextColor=MkColor(2,2,1);
		writest(212,180+off,st);
		TextColor=cor[i];
		writest(213,181+off,st);
		for(c=0;c<Player[i].pacs;c++)
		{
			DrawPic(300-c*15,179+off,Pac.pic[0+8*i]);
		}
	}
	switch(Game.st)
	{
		case GO:
			Color=MkColor(0,6,0);
			FillColor=MkColor(3,3,2);
			DrawRect(120,75,199,94,LIFI);
			TextColor=MkColor(0,0,0);
			writest(124,80,"Game Over");
			TextColor=MkColor(0,6,0);
			writest(125,81,"Game Over");
		break;
		case PAUSA:
			ShowPopUp();
		break;
	}
	//sprintf(st,"%d fps",frames);
	writenumber(st,frames,1);
	TextColor=MkColor(7,7,3);
	writest(1,1,st);
}

void UpDateScreen()
{
		int c;
		fillscreen(0);
		Show_PF();
		ShowBonus();
		Show_Pac();
		for(c=0;c<NG;c++)
			Show_Ghost(c);
		ShowScore();
		Vsync();
		ScrnCpyUD();
}

void Show_PF()
{
	int x,y;
	int p;
	BYTE far *PFg=Player[Game.plr].PFg;
	for(y=0;y<PFYD;y++)
	for(x=0;x<PFXD;x++)
	{
		switch(PFg[y*PFXD+x])
		{
			case 'h':p=0;break;
			case 'v':p=5;break;
			case 'a':p=1;break;
			case 'b':p=2;break;
			case 'c':p=3;break;
			case 'd':p=4;break;
			case 'e':p=9;break;
			case 'f':p=8;break;
			case 'g':p=6;break;
			case 'i':p=7;break;
			case 'j':p=15;break;
			case 'k':p=14;break;
			case 'l':p=12;break;
			case 'm':p=13;break;
			case 'n':p=16;break;
			case 'p':p=10;break;
			case 'q':p=17;break;
			case 'r':p=11;break;
			case 'x':p=19;break;
			case 'y':p=20;break;
			default:p=18;
		}
		DrawPicF(x*10+PFXI,y*10+PFYI,PFp[p]);
	}

}

void Vsync()
{
	asm               mov dx,0x3DA
		 uno:
	asm               in  al,dx
	asm               and al,0x8
	asm               jnz uno
		 dos:
	asm               in  al,dx
	asm               and al,0x8
	asm               jz  dos

}


unsigned char inportb(unsigned p)
{
	unsigned char v;
	asm {
		mov dx, p;
		in al, dx;
		mov v, al;
	}
	return v;
}

void interrupt newkeyb(void)
{
  unsigned char tecla;
  tecla=inportb(0x60);

  if (tecla<0x80)
	  switch (tecla)
	  {
	case 30:   StickA[0]=1; break;
	case 32:   StickA[1]=1; break;
	case 17:   StickA[2]=1; break;
	case 31:   StickA[3]=1; break;
	case 20:   StickA[4]=1; break;
	case 22:   StickA[5]=1; break;
	case 21:   StickA[6]=1; break;
	case 75:   StickB[0]=1; break;
	case 77:   StickB[1]=1; break;
	case 72:   StickB[2]=1; break;
	case 80:   StickB[3]=1; break;
	case 82:   StickB[4]=1; break;
	case 28:   StickB[5]=1; break;
	case 83:   StickB[6]=1; break;
	  }
  else
	  switch (tecla-0x80)
	  {
	case 30:   StickA[0]=0; break;
	case 32:   StickA[1]=0; break;
	case 17:   StickA[2]=0; break;
	case 31:   StickA[3]=0; break;
	case 20:   StickA[4]=0; break;
	case 22:   StickA[5]=0; break;
	case 21:   StickA[6]=0; break;
	case 75:   StickB[0]=0; break;
	case 77:   StickB[1]=0; break;
	case 72:   StickB[2]=0; break;
	case 80:   StickB[3]=0; break;
	case 82:   StickB[4]=0; break;
	case 28:   StickB[5]=0; break;
	case 83:   StickB[6]=0; break;
	}

  oldkeyb();
}



void LoadAllNew();

void LoadAll()
{
	VirScr=0;
	if(!(Player[0].PFg=farmalloc((PFYD*PFXD))))
		LoadError=1;

	if(!(Player[1].PFg=farmalloc((PFYD*PFXD))))
		LoadError=1;
	
	
	if(!(VirScr=farmalloc(0xFFFF)))
	{
		printf("Memoria insuficiente!\n");
		LoadError=1;
	}
	
	LoadAllNew();
	
	if(LoadError)
	{
		UnloadAll();
		exit(1);
	}

}

void LoadAllNew()
{
	TResourceMap *rs;
	int count;
	int i=0;
	char buffer[11];
	count=loadResourceFile("pacman.res",rmap,MAXRESOURCES);
	if(!count) {
		LoadError=1;
		return;
	}

	if(!getResource("RS$PacMan",rmap,MAXRESOURCES)) {
		printf("XIT\n");
		LoadError=1;
		return;
	}
	
	if(!getResource("RE$PacMan",rmap,MAXRESOURCES)) {
		LoadError=1;
		return;
	}

	if(!(rs=getResource("MusicMidi",rmap,MAXRESOURCES))) {
		printf("Resource not found MusicMidi\n");
		LoadError=1;
		return;
	}
	MusSeg=rs->ptr;
	MusicLen=rs->size;
	
	if(!(rs=getResource("PlayField",rmap,MAXRESOURCES))) {
		printf("Resource not found PlayField\n");
		LoadError=1;
		return;
	}
	LPFg=rs->ptr;
	if(PFXD*PFYD!=rs->size)
	{
		printf("Wrong playfield data size expected %u got %u\n", PFXD*PFYD, rs->size);
		LoadError=1;
		return;
	}

	if(!(rs=getResource("BonusPic00",rmap,MAXRESOURCES))) {
		printf("Resource not found BonusPic00\n");
		LoadError=1;
		return;
	}
	Bp=rs->ptr;
	
	for(i=0;i<12;i++)
	{
		strcpy(buffer,"PacPic00");
		buffer[6]=(i/10)%10 + '0';
		buffer[7]=i%10 + '0';
		if(!(rs=getResource(buffer,rmap,MAXRESOURCES))) {
			printf("Resource not found %s\n", buffer);
			LoadError=1;
			return;
		}
		Pac.pic[i]=rs->ptr;
	}

	for(i=0;i<6;i++)
	{
		strcpy(buffer,"GhostPic00");
		buffer[8]=(i/10)%10 + '0';
		buffer[9]=i%10 + '0';
		if(!(rs=getResource(buffer,rmap,MAXRESOURCES))) {
			printf("Resource not found %s\n", buffer);
			LoadError=1;
			return;
		}
		Gp[i]=rs->ptr;
	}
	
	for(i=0;i<21;i++)
	{
		strcpy(buffer,"WallPic00");
		buffer[7]=(i/10)%10 + '0';
		buffer[8]=i%10 + '0';

		if(!(rs=getResource(buffer,rmap,MAXRESOURCES))) {
			printf("Resource not found %s\n", buffer);
			LoadError=1;
			return;
		}
		PFp[i]=rs->ptr;
	}

	if(!(rs=getResource("Paleta",rmap,MAXRESOURCES))) {
		printf("Resource not found Paleta\n");
		LoadError=1;
		return;
	}
	
	palette=rs->ptr;
	if(rs->size!=256*sizeof(Trgb)) {
		printf("Wrong palette data size expected %u got %u\n",256*sizeof(Trgb), rs->size);
		LoadError=1;
		return;
	}
	
	//LoadPic(&Pac.pic[0],"pics/gpic0.bmp");
	
	printf("Resources Loaded %d\n", count);
	{
		int x,y;
		PF_dotsL=0;
		for(y=0;y<PFYD;y++)
		{
			for(x=0;x<PFXD;x++)
			{
				unsigned char caracter = LPFg[y*PFXD+x];
				if((caracter=='x')||(caracter=='y')) PF_dotsL++;
			}
		}
	}
}

void UnloadAll()
{
	int i;
	if(Player[0].PFg)
		farfree(Player[0].PFg);
	if(Player[1].PFg)
		farfree(Player[1].PFg);
	if(VirScr) farfree(VirScr);
	
	for(i=0;i<MAXRESOURCES;i++)
	{
		if(rmap[i].ptr)
			farfree(rmap[i].ptr);
	}
	//return NULL;

}
