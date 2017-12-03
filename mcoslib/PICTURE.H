#ifndef _PICTURE_H
	#define _PICTURE_H
	
typedef struct{
	unsigned w,h;
	char data[1];
}TPicture;

typedef struct{
	unsigned char B,G,R,U;
}Trgb;

#endif
