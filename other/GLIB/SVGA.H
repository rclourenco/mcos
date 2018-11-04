#ifndef _SVGA_H
 #define _SVGA_H


#define COPY_MODE  0
#define XOR_MODE   1
#define OR_MODE    2
#define AND_MODE   3
#define NOT_MODE   4


typedef unsigned int WORD;
typedef unsigned long DWORD;
typedef unsigned char BYTE;

typedef struct {
	BYTE  VbeSignature[4];
	WORD  VbeVersion;
	DWORD OemStringPtr;
	DWORD Capabilites;
	DWORD VideoModePtr;
	WORD  TotalMemory;
	WORD  OemSoftwareRev;
	DWORD OemVendorNamePtr;
	DWORD OemProductNamePtr;
	DWORD OemProductRevPtr;
	WORD  Vbe_AF_Version;
	BYTE  Reserved[220];
	BYTE  OemData[256];
}TVBE_INFO;

typedef struct {
	WORD w,h;
	BYTE i[1];
}GIMAGE;

void vgetimage(int left, int top, int right, int bottom, GIMAGE far *bitmap);
void vputimage(int left, int op, GIMAGE far *bitmap);

void drawbitmap(int left, int top, WORD bitmap[], int h, BYTE color);

typedef struct {
	WORD  Attributes;
	BYTE  WindowA_Attributes;
	BYTE  WindowB_Attributes;
	WORD  WindowGranK;
	WORD  WindowSizeK;
	WORD  WindowA_Start;
	WORD  WindowB_Start;
	DWORD WindowPosFunc;
	WORD  ScanlineBytes;
	WORD  Width;
	WORD  Height;
	BYTE  CharWidth;
	BYTE  CharHeight;
	BYTE  NumberOfPlanes;
	BYTE  BitsPerPixel;
	BYTE  NumberOfBanks;
	BYTE  MemoryModelType;
	BYTE  BankSize;
	BYTE  NumberOfImagePages;
	BYTE  Reserved1;
	BYTE  RedMaskSize;
	BYTE  RedMaskPosition;
	BYTE  GreenMaskSize;
	BYTE  GreenMaskPosition;
	BYTE  BlueMaskSize;
	BYTE  BlueMaskPosition;
	BYTE  ReservedMaskSize;
	BYTE  ReservedMaskPosition;
	BYTE  DirectScreenModeInfo;
	DWORD PhysicalAddress;
	DWORD OffscreenMemoryStart;
	WORD  OffscreenMemorySize;
	BYTE  Padding[0xCE];
}TVBEMODE_INFO;

void BiosPutPixel(WORD x,WORD y,BYTE c);
BYTE BiosGetPixel(WORD x,WORD y);
char GetVbeInfo(TVBE_INFO far *ptr);
char SetVbeMode(WORD mode);
int GetVbeModeInfo(WORD vmode, TVBEMODE_INFO far *ptr);
void VESAPutByte(unsigned long ofs, unsigned char c);
void VESAPutXor(unsigned long ofs, unsigned char c);
int VESASetOffset(int ofs);
void VESAHLine(unsigned long ofs1, unsigned long ofs2, unsigned char c);
void VESAPutpixel(int x, int y, int c);
void VESACleardevice();
void VESAClosegraph();

void gline(int x1, int y1, int x2, int y2);
void grectangle(int x1, int y1, int x2, int y2);
void gbar(int x1, int y1, int x2, int y2);
void garc(int x, int y, int stangle, int endangle, int radius);

void glineto(int x, int y);
void writest(int x, int y, char *str);

void gsetwritemode(int m);

void VESASetbkcolor(int color);
void VESASetcolor(int color);
void VESASetfillpattern(char *upattern, int color);
void VESASetfillstyle(int pattern, int color);
void VESASetlinestyle(int linestyle, unsigned upattern, int thickness);
void VESASettextstyle(int font, int direction, int charsize);
int VESATextheight(char *str);
int VESATextwidth(char *str);

int ggetmaxx();
int ggetmaxy();



extern WORD VESAWidth;
extern WORD VESAHeight;

extern BYTE Color;
extern BYTE TextColor;
extern BYTE TextBackGround;
extern BYTE FillColor;
extern BYTE TextSize;

#endif
