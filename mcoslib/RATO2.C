#include <dos.h>
//prototipos
char CheckRato();
void ShowRato();
void HideRato();
void SetRato(int x,int y);
void GetRato(int *x,int *y,int *b);
//rotina suplementar verifica se o rato esta dentro de uma certa area e
//se o botao esta premido
char quadro(int x1,int y1,int x2,int y2);


//implementacao
char CheckRato()
{
	struct REGPACK regs;
	regs.r_ax=0;
	intr(0x33,&regs);
	return regs.r_ax;
}

void ShowRato()
{
	struct REGPACK regs;
	regs.r_ax=1;
	intr(0x33,&regs);
}

void HideRato()
{
	struct REGPACK regs;
	regs.r_ax=2;
	intr(0x33,&regs);
}

void SetRato(int x,int y)
{
	struct REGPACK regs;
	regs.r_ax=4;
	regs.r_cx=x;
	regs.r_dx=y;
	intr(0x33,&regs);
}

void GetRato(int *x,int *y,int *b)
{
	struct REGPACK regs;
	regs.r_ax=3;
	intr(0x33,&regs);
	*x=regs.r_cx;
	*y=regs.r_dx;
	*b=regs.r_bx;
}

char quadro(int x1,int y1,int x2,int y2)
{
	int x,y,b;
	GetRato(&x,&y,&b);
	if((x>=x1)&&(x<=x2)&&(y>=y1)&&(y<=y2)&&(b==1))
		return 1;
	else
		return 0;
}