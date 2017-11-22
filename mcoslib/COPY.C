#include "mcosapi.h"

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
    else {
      printf("Error creating: %s\n", dest);
    }
    mcos_close(handle); 
  }
  else {
    printf("Error opening: %s\n", path);
  }
}

main(int argc, char **argv)
{
  int i=0;
  if(argc==3) {
    copyfile(argv[1],argv[2]);
  }
  else {
    printf("Copies a file\nSyntax: %s <source> <destination>\n", argv[0]);
  }
  return 0;
}
