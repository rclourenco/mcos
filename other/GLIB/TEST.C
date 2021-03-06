#include <math.h>
#include <stdio.h>
#include "svga.h"
#include "mouse.h"

TVBE_INFO VbeInfo;

TVBEMODE_INFO ModeInfo;



unsigned vmode = 0x103;

char tmp[1000];

WORD pointer[16]={
		0x0000,    // xxx
		0x4000,    // x#x
		0x6000,    // x##x
		0x7000,    // x###x
		0x7800,    // x####x
		0x7C00,    // x#####x
		0x7E00,    // x######x
		0x7F00,    // x#######x
		0x7F80,    // x########x
		0x7FC0,    // x#########x
		0x7E00,    // x######xxxx
		0x7600,    //  ###x##
		0x6300,    //  ##   ##
		0x4300,    //  #    ##
		0x0180,    //        ##
		0x0000};   //

main()
{
	int a,b;
	int oms=-1, omx=-1, omy=-1;
	int done=0;
	WORD far *p;
	clrscr();
	if(!GetVbeInfo(&VbeInfo)) exit(0);

	if(GetVbeModeInfo(vmode, &ModeInfo)) {
		 //VESAWidth = ModeInfo.Width;

		 printf("Mode Attr: %X\n", ModeInfo.Attributes);
		 printf("Resolution: %d x %d\n", ModeInfo.Width, ModeInfo.Height);
		 printf("Window Gran: %d KB\n", ModeInfo.WindowGranK);
		 printf("Window Size: %d KB\n", ModeInfo.WindowSizeK);
		 printf("Window A Start: %X\n", ModeInfo.WindowA_Start);
		 printf("Window B Start: %X\n", ModeInfo.WindowB_Start);
		 printf("Window Pos Func: %Fp\n", ModeInfo.WindowPosFunc);
		 printf("Bytes Per Scanline: %d\n", ModeInfo.ScanlineBytes);
		 printf("Char Resolution: %d x %d\n", ModeInfo.CharWidth, ModeInfo.CharHeight);
		 printf("Number of Planes: %d\n", ModeInfo.NumberOfPlanes);
		 printf("Bits per Pixel: %d\n", ModeInfo.BitsPerPixel);
		 printf("Number of banks: %d\n", ModeInfo.NumberOfBanks);
		 printf("Memory Model Type: %d\n", ModeInfo.MemoryModelType);
		 printf("Size of Bank: %d KB\n", ModeInfo.BankSize);
		 printf("Number of Image Pages: %d\n", ModeInfo.NumberOfImagePages);
		 printf("Linear Video Buffer Physical Address: %Fp\n", ModeInfo.PhysicalAddress);
		 printf("Offscreen Memory Start: %Fp\n", ModeInfo.OffscreenMemoryStart);
		 printf("_____________________________________________\n");
		 getch();
	}
	for(a=0;a<4;a++)
		putch(VbeInfo.VbeSignature[a]);
	printf("\nVers�o %d.%d encontrada.",VbeInfo.VbeVersion>>8,VbeInfo.VbeVersion<<8);
	printf("\nTotal Video Memory: %dK.",VbeInfo.TotalMemory*64);
	printf("\nOemString: %Fs\n",VbeInfo.OemStringPtr);
	p=(WORD far *)VbeInfo.VideoModePtr;
	printf("Video mode ptr: %Fp\n", VbeInfo.VideoModePtr);

	printf("Size of mode info: %d\n", sizeof(TVBEMODE_INFO));
	printf("\nModos VESA suportados: \n");

	while(p[a]!=0xFFFF)
	{
		printf("%X\t",p[a]);
		a++;
	}
	printf("\nOemSoftwareRev: %d.%d",VbeInfo.OemSoftwareRev>>8,VbeInfo.OemSoftwareRev<<8);

	//printf("WIDTH %d\n", VESAWidth);
	getch();
	clrscr();

	if(SetVbeMode(vmode))
	{
		long int dim = ModeInfo.Width;
		//puts("TESTE_OK");
		getch();
		Color=14;
		grectangle(10, 10, ModeInfo.Width-11, ModeInfo.Height-11);
		FillColor=200;
		gbar(11,11,ModeInfo.Width-12, ModeInfo.Height-12);
		Color=15;
/*		for(a=0;a<ModeInfo.Height;a++) {
			for(b=0;b<dim;b++)
			{
				float x = (a%80 ^ b%80)+(sin(b/1)+1)*20;
				float y = (b%80 | a%80)+(cos(a/1)+1)*20;
				VESAPutByte(a*dim+b, ((int)(x/10) + (int)(y/10))%50+20);
				//VESAPutByte(a*dim+b, b%2);
			}
		}
*/
		getch();
		for(a=80;a<304;a++) {
			VESAHLine(a*dim+80, a*dim+704, a);
		}
		getch();

		vgetimage(200, 100, 219, 129, (GIMAGE *)tmp);

		gline(100, 100, 400, 400);
		gsetwritemode(XOR_PUT);
		gline(400, 400, 100, 599);

		TextColor=40;
		TextBackGround=255;
		gsetwritemode(COPY_PUT);
		getch();
		b=0;
		for(a=0;a<=16;a++) {
			TextSize=a;
			TextColor=30+a;
			//b+=TextSize;
			writest(10, 10+b, "The quick brown fox jumps over the lazy dog!");
			b+=TextSize+2;
		}
		//gsetwritemode(COPY_PUT);

		for(a=0;a<600;a+=30) {
		/*	for(b=0;b<800;b+=40)
				vputimage(b, a, (GIMAGE *)tmp);
		*/
		}
		//for(b=0;b<640;b++)
		//for(a=0;a<200;a++)
		   //	pokeb(0xa000,b+a*640,b);
			//BiosPutPixel(a,b,a+b%8);
  //		ShowRato();
		MouseOpen();
		MouseShow();
		while(!done) {
			int s;
			int click_l;
			int click_r;
			int mx, my;
			delay(1);
			//gotoxy(1,1);
			s=MouseStatus();
			mx=MouseX();
			my=MouseY();

			if(oms==-1)
				oms=s;
			if(omx==-1)
				omx=mx;
			if(omy==-1)
				omy=my;

			click_l = (s&1) && !(oms&1);
			//printf("%3d - %3d - %02X\n", MouseX(), MouseY(), s);
			if(kbhit()) {
				switch (getch()) {
				case 27: done=1; break;
				case 32: MouseHide();
					vputimage(MouseX(), MouseY(), (GIMAGE *)tmp);
					MouseShow();
					break;
				}
			}

			if((s&1) && (omx != mx || omy != my)) {
				MouseHide();
				glineto(mx, my);
				//vputimage(MouseX(), MouseY(), (GIMAGE *)tmp);
				MouseShow();
			}
			oms=s;
			omx=mx;
			omy=my;
		}
		MouseHide();
		getch();
		MouseClose();
    //		HideRato();
		SetVbeMode(0x3);

		printf("Image %d x %d\n", ((GIMAGE *)tmp)->w, ((GIMAGE *)tmp)->h);
	}
	else
		puts("TESTE_FAILED");
	getch();
	return 0;

}
