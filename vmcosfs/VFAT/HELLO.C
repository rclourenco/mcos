#include <stdio.h>

#define MAX 100

typedef unsigned char BYTE;
typedef unsigned short WORD;

BYTE map[MAX]={0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10};

WORD getfat12(WORD i)
{
	WORD j=(i*3)>>1;
	if(i&1) {
		return (map[j]>>4) | (map[j+1]<<4);
	}
	return map[j]&0xFF | ((map[j+1]&0x0F)<<8);
}

void setfat12(WORD i, WORD v)
{
 WORD j=(i*3)>>1;
 if(i&1){
  map[j]=(map[j]&0x0f) | ((v&0xf)<<4);
  map[j+1]=v>>4;
  return;
 }
 map[j]=v&0xFF;
 map[j+1]=(map[j+1]&0xf0) | (v>>8)&0xf;
}

void showfat()
{
  int c;
  for(c=0;c<5;c++)
  {
    printf("> %04X\n", getfat12(c));
  }
  printf("-------------\n");
}


int main()
{
  printf("====================\n");
  showfat();
  setfat12(0,0xdef);
  setfat12(1,0xabc);
  setfat12(2,0x789);
  setfat12(3,0x456);
  setfat12(4,0x123);
  showfat();
  getchar();
  return 0;
}
