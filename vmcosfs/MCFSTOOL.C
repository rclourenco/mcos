#include <stdio.h>
#include "vmcosfs.h"

#define escreve_erro() if(ERRO) fprintf(stderr,"%s\n",GetMsgErro(ERRO));


void MountDrive(char c)
{
	char d=-1;
	switch(c)
	{
		case 'A':
		case 'a':d=0;break;
		case 'B':
		case 'b':d=1;break;
	}

	if(d==-1)
	{
		printf("Invalid drive\n");
		return;
	}

	if(Montada(d))
	{
		printf("Drive is already mounted\n");
		return;
	}

	if(!MontarDrive(d)) {
		printf("Error...Cannot mount drive\n");
		return;
	}

	printf("Drive successful mounted\n");
}

void Ls(char c)
{
	TDIR_RECORD d;
	int linhas,numl;
	int nfich;
	char v,i;
	char out[20];
	char drive=-1;
	switch(c)
	{
		case 'A':
		case 'a':drive=0;break;
		case 'B':
		case 'b':drive=1;break;
	}
	ApagaErro();
	if(drive>1 || drive < 0)
	{
		printf("Dispositivo Inv lido...\n\n");
		return;
	}
	if(!Montada(drive))
	{
		printf("A Drive nÆo se encontra montada...\n\n");
		return;
	}
	printf("Listagem da Drive %c:\n",'A'+drive);
	linhas=25;
	v=DirProcura(drive,&d,1);
	numl=0;
	nfich=0;
	while(v)
	{
		numl++;
		nfich++;

		strncpy(out,d.nome,12);
		out[12]=0;

		printf(
			"%-12s %8lu Bytes %04u-%02u-%02u %02u:%02u %s\n",
			out, d.Tamanho, d.Data.ano, d.Data.mes, d.Data.dia,
			d.Hora.hora, d.Hora.minutos,
			d.Attr&So_leitura ? "READ_ONLY" : ""
		);
		if(numl>=linhas-4)
		{
			printf("Prima qualquer Tecla para Continuar ou ESC para Terminar...\n\n");
			if(getchar()==27) break;
			numl=0;
		}
		v=DirProcura(drive,&d,0);
	}
	printf("\nNumber of entries:   %8u\n", nfich);
	printf("Free Disk Space:     %8lu\n", FreeSpace(drive));
	printf("Total Disk Space:    %8lu\n", DiskSpace(drive));

	if(ERRO)
		fprintf(stderr,"%s\n",GetMsgErro(ERRO));
}

void copymd(char drv, char *mfile, char *dpath)
{
	char drive=0;
	WORD handle;
	WORD ch;
	WORD lines;
	FILE *fp;
	switch(drv)
	{
		case 'A':
		case 'a':drive=0;break;
		case 'B':
		case 'b':drive=1;break;
	}
	MountDrive(drv);
	ApagaErro();
	fp=fopen(dpath,"wb");
	if(!fp) {
		fprintf(stderr,"Erro a criar ficheiro de destino %s\n", dpath);
		return;
	}
	handle=AbrirFicheiro(drive,mfile,LEITURA);
	printf("Drive %d %s\n", drive, mfile);
	if((handle>=FILES)||(ERRO))
	{
		fprintf(stderr,"Erro a abrir ficheiro de origem...\n");
		escreve_erro();
		fclose(fp);
		return;
	}
	lines=0;
	while(!Fim_Ficheiro(handle))
	{
		ch=LerCaracter(handle);
		if(ERRO) break;
		fputc(ch,fp);
	}
	FecharFicheiro(handle);
	fclose(fp);
}

/*
void copymm(char drv, char *mfile, char *dpath)
{
	char drive=0;
	WORD handle;
	WORD ch;
	WORD lines;
	FILE *fp;
	switch(drv)
	{
		case 'A':
		case 'a':drive=0;break;
		case 'B':
		case 'b':drive=1;break;
	}
	MountDrive(drv);
	ApagaErro();
	fp=fopen(dpath,"")
	handle=AbrirFicheiro(drive,mfile,LEITURA);
	printf("Drive %d %s\n", drive, mfile);
	if((handle>=FILES)||(ERRO))
	{
		fprintf(stderr,"Erro a abrir ficheiro de origem...\n");
		//escreve_erro();
		return;
	}
	lines=0;
	while(!Fim_Ficheiro(handle))
	{
		ch=LerCaracter(handle);
		if(ERRO) break;
		putchar(ch);
	}
	FecharFicheiro(handle);
}
*/


void copydm(char *dpath,char drv, char *mfile)
{
	char drive=0;
	WORD handle;
	WORD ch;
	WORD lines;
	FILE *fp;
	switch(drv)
	{
		case 'A':
		case 'a':drive=0;break;
		case 'B':
		case 'b':drive=1;break;
	}
	MountDrive(drv);
	ApagaErro();
	fp=fopen(dpath,"rb");
	handle=AbrirFicheiro(drive,mfile,CRIAR_F|ESCREVER|EXPANDIR);
	printf("Drive %d %s\n", drive, mfile);
	if((handle>=FILES)||(ERRO))
	{
		fprintf(stderr,"Erro a abrir ficheiro de origem...\n");
		escreve_erro();
		return;
	}
	lines=0;
	while(!feof(fp))
	{
		ch=fgetc(fp);
		EscreverCaracter(handle,ch);
		if(ERRO) break;
	}
	FecharFicheiro(handle);
	fclose(fp);
}


void copy(char *from, char *to)
{
	char from_drv=0;
	char to_drv=0;
	if(from[0]=='/') {
		from_drv=-1;
		if(from[2]=='/') {
			switch(from[1])
			{
				case 'a':
				case 'A':
				case 'b':
				case 'B':  from_drv=from[1];
				break;
			}
		}

	}
	if(to[0]=='/') {
		to_drv=-1;
		if(to[2]=='/') {
			switch(to[1])
			{
				case 'a':
				case 'A':
				case 'b':
				case 'B':  to_drv=to[1];
				break;
			}
		}
	}
	if(from_drv==-1 || to_drv==-1){
		fprintf(stderr, "Invalid syntax\n");
		return;
	}
	if(from_drv && to_drv) {
	      //	copymm(
	      //		from_drv,&from[3],
	      //		to_drv,  &to[3]
	      //	);
	}
	else if(from_drv) {
		copymd( from_drv,&from[3],to );
	}
	else if(to_drv) {
		copydm(from,to_drv,&to[3]);
	}
}


main(int argc, char **argv)
{
	if(!INITFS())
	{
		fprintf(stderr, "Cannot init mcos filesystem\n");
		return -2;
	}

	if(argc>1) {
		if(!strcmp(argv[1],"ls")) {
			char drive='a';
			if(argc>2) {
				switch(argv[2][0])
				{
					case 'a':
					case 'A':
					case 'b':
					case 'B': drive = argv[2][0];
					default: drive=0;
				}
			}
			if(drive) {
				MountDrive(drive);
				Ls(drive);
			}
			else {
				fprintf(stderr, "Invalid drive...\n");
			}

		}
		else if(!strcmp(argv[1],"cp")) {
			if(argc==3) {
				//copy(argv[2],".");
			}
			else if(argc==4) {
				copy(argv[2],argv[3]);
			}
			else {
				fprintf(stderr, "missing arguments...\n");
			}
		}
	}
	CLOSEFS();
	return 0;
}

