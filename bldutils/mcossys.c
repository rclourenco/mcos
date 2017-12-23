#include <stdio.h>
#include <stdlib.h>


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

int extdiskwrite(long int sect,void *buffer);
unsigned insertmcos();
void formatfs();

FILE *diskfp;
long int tsects = 2880;

void setdisk()
{
  long int i=0;
  for(i=0;i<tsects;i++) {
    memset(buffer,'A'+(char)(i%26),512);
    extdiskwrite(i,buffer);
  }
}


int main(int argc, char **argv)
{
	int c;
	unsigned c2;
	FILE *fp;
	if( argc!=3 ) {
	  printf("Invalid arguments...\n");
	}
	diskfp=fopen(".IMG", "rb");
	if(!diskfp) {
	  printf("Cannot open disk image");
	  exit(2);
	}
  setdisk();
//  printf("DONE.\n");
//  exit(0);
	fp=fopen("MCBOOT.BIN","rb");
	if(!fp)
	{
		printf("Error: File 'MCBOOT.BIN' not found...");
		return 1;
	}
	c=fread(bootrecord,1,512,fp);
	fclose(fp);
	if((bootid[0]!='M')||(bootid[1]!='C')||(bootid[2]!='F')||
		(bootid[3]!='S')||(bootid[4]!='0')||(bootid[5]!='0'))
			c=0;
	if(c!=512)
	{
		printf("Error: MCBOOT.BIN is not valid...\n");
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
		printf("System not stored...\n");
	}
	*sysnsec=c2;
	fatsect=syssect+(*sysnsec);
	rootsec=fatsect+(*sectfat);
	rootsize=((*rootdir)*32)/512;
	formatfs();

	extdiskwrite(0,bootrecord);
	printf("Sector da FAT: %i\n",fatsect);
	printf("Sector da Directoria Raiz: %i\n",rootsec);
	printf("Sector dos Dados: %i\n",rootsec+rootsize);

  fflush(diskfp);
  fclose(diskfp);
}

int extdiskwrite(long int sect, void *buffer)
{
  fseek(diskfp,sect*512,SEEK_SET);
  return fwrite(buffer,512,1,diskfp);
}

int extdiskread(long int sect, void *buffer)
{
  fseek(diskfp,sect*512,SEEK_SET);
  return fread(buffer,512,1,diskfp);
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
