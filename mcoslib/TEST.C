#include "mcosapi.h"

char far * far *blines;

void typefile(char *path)
{
	int handle;
	if( (handle=mcos_open(path,READMODE)) != -1 ) {
		int ch;
		int lines=0,s=0;
		while( (ch=mcos_fgetc(handle))>=0 ) {
			if(ch==10) {
				lines++;
				s=1;
			}
			mcos_putchar(ch);
			if(lines%24==0 && s==1) {
				s=0;
				printf("%c7mPress a key to continue...%c7m",27,27);
				mcos_getkey();
				printf("%cK",27);
			}
		}
		printf("\n");
		printf("%c7m    <END OF FILE>    \n%c7m",27,27);
		
		mcos_close(handle);
	}
}

void far *fmalloc(unsigned s)
{
	unsigned seg;
	unsigned n = s/16 + (s%16 ? 1 : 0);
	printf("N Paragraphs: %u\n", n);
	if(mcos_alloc(n,&seg)) {
		return MK_FP(seg,0);
	}
	return MK_FP(0,0);
}

void typefile2(char *path)
{
	int handle;
	if( (handle=mcos_open(path,READMODE)) != -1 ) {
		int ch;
		int lines=0,s=0;
		printf("%c7m %u %c7m ",27,lines,27);
		while( (ch=mcos_fgetc(handle))>=0 ) {
			if(ch==13)
				continue;
			if(ch==10) {
				lines++;
				s=1;
			}
			mcos_putchar(ch);
			if(lines%24==0 && s==1) {
				s=0;
				printf("%c7mPress a key to continue...%c7m",27,27);
				mcos_getkey();
				printf("%cK",27);
				printf("%c7m %u %c7m ",27,lines,27);
			}
			else if(s==1) {
				s=0;
				printf("%c7m %u %c7m ",27,lines,27);
			}
		}
		printf("\n");
		printf("%c7m    <END OF FILE>    \n%c7m",27,27);
		
		mcos_close(handle);
	}
}


void copyfile(char *path, char *dest)
{
	int handle;
	int ho;
	if( (handle=mcos_open(path,READMODE)) != -1 ) {
		if( (ho=mcos_open(dest,WRITEMODE))!= -1 ) {
			int ch;
			while( (ch=mcos_fgetc(handle))>=0 ) {
				mcos_fputc(ho,ch);
			}
			mcos_close(ho);
		}
		mcos_close(handle);
	}
}


unsigned initialize_serial(int serial)
{
	unsigned r;
	asm {
		mov ah,00h;
		mov al,0C7h;
		mov dx,serial;
		int 14h;
		mov r,ax;
	}
	return r;
}

unsigned send_serial(int serial, unsigned char val)
{
	unsigned r;
	asm {
		mov ah,01h;
		mov al,val;
		mov dx,serial;
		int 14h;
		mov r,ax;
	}
	return r;
}


void sendfile(char *path, int serial)
{
	int handle;
	unsigned r;
	printf("Sending file to serial port %u\n", serial);
	r=initialize_serial(serial);
	printf("Initialize return %X\n", r);
	if( (handle=mcos_open(path,READMODE)) != -1 ) {
		int ch;
		while( (ch=mcos_fgetc(handle))>=0 ) {
			send_serial(serial,ch);
		}
		mcos_close(handle);
	}
}


main(int argc, char **argv)
{
	int i=0;
	if(argc>1) {
		switch(argv[1][0])
		{
			case 'c':
				if(argc>3)
					copyfile(argv[2],argv[3]);
				break;
			case 'd':
				if(argc>2)
					typefile(argv[2]);
				break;
			case 'e':
				if(argc>2)
					typefile2(argv[2]);
				break;
			case '0':
			case '1':
				if(argc>2)
					sendfile(argv[2], argv[1][0]-'0');
				break;
			break;
		}
	}
}