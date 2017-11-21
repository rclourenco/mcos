#include "mcoslib.h"
#include "mcosmem.h"

unsigned MEM_SearchNext;
unsigned MEM_Start;
unsigned MEM_Size;
unsigned MEM_End;

char far *Label_MF="<FREE>";
char far *Default_Label="<MCOS>";

/*########################################################################*/
void MEM_Copy(unsigned sego,unsigned segd,unsigned size)
{
	unsigned c1,c2;
	char far *o;
	char far *d;
	for(c1=0;c1<size;c1++)
	{
		o=(char far *)MK_FP(sego,0);
		d=(char far *)MK_FP(segd,0);
		for(c2=0;c2<16;c2++)
			d[c2]=o[c2];
		segd++;
		sego++;
	}
}

int Realoca_Segmento(unsigned size,unsigned far *seg)
{
	MCB mcb1,mcb2;
	unsigned seg1,seg2,size2;
	if((!*seg)||(!size))
		return 0;
	seg1=*seg-1;
	Get_Bloco(seg1,&mcb1);
	if(size==mcb1.Tam)
		return 1;
	if(size<mcb1.Tam)
	{
		seg2=seg1+size+1;
		mcb2.Tam=mcb1.Tam-size-1;
		mcb2.ProcessoID=0;
		mcb2.Tipo=mcb1.Tipo;
		Strcpyx(mcb2.Label,Label_MF);
		mcb1.Tipo=INTER;
		mcb1.Tam=size;
		Set_Bloco(seg1,&mcb1);
		Set_Bloco(seg2,&mcb2);
		MEM_Optimize();
		return 1;
	}
	seg2=seg1+mcb1.Tam+1;
	Get_Bloco(seg2,&mcb2);
	size2=mcb2.Tam+mcb1.Tam+1;
	if((mcb2.ProcessoID==0)&&(size2>=size))
	{
		mcb1.Tam=size;
		if(size2>size)
		{
			seg2=seg1+size+1;
			mcb2.ProcessoID=0;
			Strcpyx(mcb2.Label,Label_MF);
			size2=size2-size-1;
			mcb2.Tam=size2;
			Set_Bloco(seg2,&mcb2);
		}
		else
			mcb1.Tipo=mcb2.Tipo;
		Set_Bloco(seg1,&mcb1);
		return 1;
	}
	if(!Aloca_Segmento(size,mcb1.ProcessoID,&seg2))
		return 0;
	MEM_Copy(*seg,seg2,mcb1.Tam);
	Liberta_Segmento(*seg);
	*seg=seg2;
	return 1;
}

int MEM_Init()
{
//	unsigned segm;
//	unsigned size;
//	size=40960;
//	do
//	{
//		segm=0;
//		allocmem(size,&segm);
//		if(!segm)
//			size--;
//	}while((size>=4096)&&(!segm));
//	if(segm)
//	{
//		MEM_Start=segm;
//		MEM_End=MEM_Start+size;
//		MEM_Size=size;
//		return 1;
//	}
//	else
//	{
//		MEM_Start=0;
//		MEM_End=0;
//		MEM_Size=0;
//		return 0;
//	}
}

void MEM_Set(unsigned start, unsigned size)
{
	MEM_Start=start;
	MEM_End=MEM_Start+size;
	MEM_Size=size;
}

unsigned MEM_seg()
{
	return MEM_Start;
}

void MEM_Done()
{
	if(MEM_Start)
	{
		/*freemem(MEM_Start);*/
	}
}

void MEM_Init2()
{
	MCB fmcb;
	fmcb.Tipo=LAST;
	fmcb.Tam=MEM_Size-1;
	fmcb.ProcessoID=0;
	Strcpyx(fmcb.Label,Label_MF);
	Set_Bloco(MEM_Start,&fmcb);
	MEM_SearchNext=0xFFFF;
}

void farmemcp(char far *dest, char far *org, unsigned n)
{
  while(n--) {
    *dest++=*org++;
  }
}

void Set_Bloco(unsigned seg,MCB far *mcb)
{
	MCB far *m;
	m=MK_FP(seg,0);
	farmemcp((char far *)m,(char far *)mcb,sizeof(MCB));
}

void Get_Bloco(unsigned seg,MCB far *mcb)
{
	MCB far *m;
	m=MK_FP(seg,0);
	farmemcp((char far *)mcb,(char far *)m,sizeof(MCB));
}

int MEM_Search(int f,MCB far *mcb,unsigned far *seg)
{
	if(f)
		MEM_SearchNext=MEM_Start;
	if((unsigned)MEM_SearchNext>=0xA000 )
	{
		return 2;
	}
	*seg=MEM_SearchNext;
	Get_Bloco(MEM_SearchNext,mcb);
	switch(mcb->Tipo)
	{
		case INTER:MEM_SearchNext+=mcb->Tam+1;return 0;
		case LAST:MEM_SearchNext=0xFFFF;return 1;
		default: return 2;
	}
}

int Aloca_Segmento(unsigned size,unsigned procid,unsigned far *seg)
{
	MCB cmcb,nmcb;
	unsigned nseg,lseg,lsize;
	if(!size)
		return 0;
	nseg=ProcurarMem(size);
	if(!nseg)
	{
		return 0;
	}
	Get_Bloco(nseg,&cmcb);
	if(size<cmcb.Tam)
	{
		lseg=nseg+size+1;
		lsize=cmcb.Tam-size-1;
		nmcb.Tam=lsize;
		nmcb.ProcessoID=0;
		if(cmcb.Tipo==INTER)
			nmcb.Tipo=INTER;
		else
		{
			nmcb.Tipo=LAST;
			cmcb.Tipo=INTER;
		}
		Strcpyx(nmcb.Label,Label_MF);
		Set_Bloco(lseg,&nmcb);
	}
	cmcb.Tam=size;
	*seg=nseg+1;
	if(!procid)
		cmcb.ProcessoID=*seg;
	else
		cmcb.ProcessoID=procid;
	Strcpy(cmcb.Label,GetProcessLabel(procid));
	Set_Bloco(nseg,&cmcb);

	return 1;
}

unsigned ProcurarMem(unsigned size)
{
	unsigned r,seg,nseg;
	unsigned d;
	MCB mbloco;
	r=MEM_Search(1,(MCB far *)&mbloco,(unsigned far *)&seg);
	nseg=0;
	d=0xFFFF;
	while(r<2)
	{
		if((!mbloco.ProcessoID)&&(mbloco.Tam>=size))
		{
			if(d>mbloco.Tam-size)
			{
				d=mbloco.Tam-size;
				nseg=seg;
			}
		}
		r=MEM_Search(0,(MCB far *)&mbloco,(unsigned far *)&seg);
	}
	return nseg;
}

char far *GetProcessLabel(unsigned pid)
{
	return Default_Label;
}

void Liberta_Segmento(unsigned seg)
{
	unsigned nseg;
	MCB cmcb,nmcb;
	if(!seg)
		return;
	seg=seg-1;
	Get_Bloco(seg,&cmcb);
	if((cmcb.Tipo!=LAST)&&(cmcb.Tipo!=INTER))
		return;
	cmcb.ProcessoID=0;
	Strcpyx(cmcb.Label,Label_MF);
	if(cmcb.Tipo!=LAST)
	{
		nseg=seg+cmcb.Tam+1;
		Get_Bloco(nseg,&nmcb);
		if(nmcb.ProcessoID==0)
		{
			cmcb.Tam+=nmcb.Tam+1;
			cmcb.Tipo=nmcb.Tipo;
		}
	}
	Set_Bloco(seg,&cmcb);
	MEM_Optimize();
}

void MEM_Optimize()
{
	unsigned seg1,seg2;
	MCB mcb1,mcb2;
	seg1=MEM_Start;
	Get_Bloco(seg1,&mcb1);
	seg2=seg1;
	farmemcp((char far *)&mcb2,(char far *)&mcb1,sizeof(MCB));
	if(mcb1.Tipo==LAST)
		return;
	do
	{
		seg2=seg2+mcb2.Tam+1;
		Get_Bloco(seg2,&mcb2);
		if((mcb1.ProcessoID==0)&&(mcb2.ProcessoID==0))
		{
			mcb1.Tam+=mcb2.Tam+1;
			mcb1.Tipo=mcb2.Tipo;
			Set_Bloco(seg1,&mcb1);
		}
		else
		{
			seg1=seg2;
			farmemcp((char far *)&mcb1,(char far *)&mcb2,sizeof(MCB));
		}
	}while(mcb2.Tipo!=LAST);

}

void LibertaMemPID(unsigned pid)
{
	char f=0;
	int r;
	MCB m;
	unsigned s;
	if(pid<2)
		return;
	r=MEM_Search(1,&m,&s);
	if(r==2) f=1;
	while(!f)
	{
		if(r>0) f=1;
		if(m.ProcessoID==pid)
		{
			Liberta_Segmento(s+1);
			r=MEM_Search(1,&m,&s);
		}
		else
			r=MEM_Search(0,&m,&s);
	 }
}
