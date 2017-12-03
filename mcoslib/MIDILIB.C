
unsigned char SBMIDI=0;  //Ok.

void Midi_Init();       //Ok.
int Midi_GetSynth();         //Ok.
int Midi_Play(void far *p); //Ok.
void Midi_Stop(void far *p); //Ok.
int Midi_Status();           //Ok.
void Midi_Pause();           //Ok.
void Midi_Resume();          //Ok.


void Midi_Init()
{
/*	char far  *p;
	int i,c;
	char st[8];
	for(i=0x80;i<=0xBF;i++)
	{
		p=(char far *)MK_FP(FP_SEG(getvect(i)),0x0000);
		for(c=0;c<7;c++)
		{
			st[c]=p[c];
		}
		st[7]=0;
		if(!strcmp("SBMIDI",st)) SBMIDI=i;
	}
	*/
}

int Midi_GetSynth()
{
/*
	struct REGPACK regs;
	if (SBMIDI==0) return 3;
	regs.r_bx=10;
	intr(SBMIDI,&regs);
	return regs.r_ax;
*/
	return 3;
}

int Midi_Play(void far *p)
{
	/*
	struct REGPACK regs;
	if (SBMIDI==0) return 0;
	regs.r_bx=4;
	regs.r_dx=FP_SEG(p);
	regs.r_ax=FP_OFF(p);
	intr(SBMIDI,&regs);
	regs.r_bx=5;
	intr(SBMIDI,&regs);
	if(regs.r_ax)
		return 0;
	else
		return 1;
	*/
	return 0;
}

void Midi_Stop(void far *p)
{
	/*
	struct REGPACK regs;
	if (SBMIDI==0) return;
	regs.r_bx=4;
	regs.r_dx=FP_SEG(p);
	regs.r_ax=FP_OFF(p);
	intr(SBMIDI,&regs);
	*/
}

int Midi_Status()
{
	/*
	struct REGPACK regs;
	if (SBMIDI==0) return 0;
	regs.r_bx=11;
	intr(SBMIDI,&regs);
	return regs.r_ax;
	*/
	return 0;
}

void Midi_Pause()
{
	/*
	struct REGPACK regs;
	if (SBMIDI==0) return;
	regs.r_bx=7;
	intr(SBMIDI,&regs);
	*/
}

void Midi_Resume()
{
	/*
	struct REGPACK regs;
	if (SBMIDI==0) return;
	regs.r_bx=8;
	intr(SBMIDI,&regs);
	*/
}
