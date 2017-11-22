#include "mcosapi.h"

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
	else {
	  printf("Error opening %s\n", path);
	}
}


main(int argc, char **argv)
{
  int i=0;
  if(argc>1) {
    typefile(argv[1]);
  }
  else {
    printf("Displays a text file\nSyntax: %s <file>\n", argv[0]);
  }

}
