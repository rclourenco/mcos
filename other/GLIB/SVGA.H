#ifndef _SVGA_H
 #define _SVGA_H

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

void drawbitmap(int left, int top, WORD bitmap[], int h);

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
int VESASetOffset(int ofs);
void VESAHLine(unsigned long ofs1, unsigned long ofs2, unsigned char c);


#endif
