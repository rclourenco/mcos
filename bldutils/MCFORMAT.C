#include <bios.h>
#include <stdio.h>
#include <conio.h>


char buffer[512];
char bootrecord[512];
char 				*bootid=bootrecord+0x3;
unsigned int 	*bytesec=bootrecord+0x9;
unsigned char  *sectclu=bootrecord+0xB;
unsigned int   *ressect=bootrecord+0xC;
unsigned int   *rootdir=bootrecord+0xE;
unsigned int 	*nsector=bootrecord+0x10;
unsigned char  *mediaid=bootrecord+0x12;
unsigned int   *sectfat=bootrecord+0x13;
unsigned int   *secttrk=bootrecord+0x15;
unsigned int   *numhead=bootrecord+0x17;
unsigned int   *sysnsec=bootrecord+0x19;

unsigned int syssect;
unsigned int fatsect;
unsigned int rootsec;
unsigned int rootsize;
int unit;

int extdiskwrite(int sect,void *buffer);
unsigned insertmcos();
void formatfs();

int main()
{
	int c;
	unsigned c2;
	FILE *fp;
	printf("Qual a unidade a formatar (0 -> A: | 1 -> B: | Outro -> Sair...):\n");
	scanf("%i",&c);
	switch(c)
	{
		case 0:unit=0;break;
		case 1:unit=1;break;
		default:return 0;

	}
	biosdisk(2,unit,0,0,1,1,bootrecord);//testedisk
	fp=fopen("MCBOOT.BIN","rb");
	if(!fp)
	{
		printf("Erro: Ficheiro ®MCBOOT.BIN¯ nÆo encontrado...");
		return 1;
	}
	c=fread(bootrecord,1,512,fp);
	fclose(fp);
	if((bootid[0]!='M')||(bootid[1]!='C')||(bootid[2]!='F')||
		(bootid[3]!='S')||(bootid[4]!='0')||(bootid[5]!='0'))
			c=0;
	if(c!=512)
	{
		printf("Erro: MCBOOT.BIN inv lido...\n");
		return 1;
	}
	*bytesec=512;
	*sectclu=1;
	*ressect=1;
	*rootdir=224;
	*nsector=2880;
	*mediaid=0xF0;
	*sectfat=9;
	*secttrk=18;
	*numhead=2;
	syssect=*ressect;
	c2=insertmcos();
	if(!c2)
	{
		printf("Sistema nÆo guardado...\n");
	}
	*sysnsec=c2;
	fatsect=syssect+(*sysnsec);
	rootsec=fatsect+(*sectfat);
	rootsize=((*rootdir)*32)/512;
	formatfs();
	biosdisk(3,unit,0,0,1,1,bootrecord);
	printf("Sector da FAT: %i\n",fatsect);
	printf("Sector da Directoria Raiz: %i\n",rootsec);
	printf("Sector dos Dados: %i\n",rootsec+rootsize);

}

int extdiskwrite(int sect,void *buffer)
{
	int head,track,sector;
	if(sect>=(*nsector)) return 0;
	sector=sect%(*secttrk)+1;
	head=(sect/(*secttrk))%(*numhead);
	track=sect/(*secttrk)*(*numhead);
	return biosdisk(3,unit,head,track,sector,1,buffer);
}

void formatfs()
{
	unsigned c;
	unsigned start,end;
	for(c=0;c<512;c++)
		buffer[c]=0;
	start=fatsect;
	end=rootsec+rootsize;
	for(c=start;c<end;c++)
		extdiskwrite(c,buffer);
}

unsigned insertmcos()
{
	FILE *fp;
	char a;
	unsigned nsectwrit;
	printf("Guardar sistema(S/N): ");
	a=getche();
	putchar('\n');
	if((a!='s')&&(a!='S'))
	{
		return 0;
	}
	fp=fopen("MCOS.SYS","rb");
	if(!fp)
	{
		printf("Erro ao abrir ficheiro de sistema...\n");
		return 0;
	}
	nsectwrit=0;
	while(!feof(fp))
	{
		fread(buffer,1,512,fp);
		extdiskwrite(syssect+nsectwrit,buffer);
		nsectwrit++;
	}
	fclose(fp);
	return nsectwrit;
}