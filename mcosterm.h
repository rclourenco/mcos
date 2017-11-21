#ifndef _MCOSTERM_H

#include "mcoslib.h"
#include "fsystem.h"

#define TERM_KEYN 22
#define TERM_KEYB 15

extern unsigned char Term_KeyBuffer[TERM_KEYN][TERM_KEYB+1];
extern unsigned char Term_KeyA;
extern char Term_Buffer[256];
extern char Term_BackBuffer[256];
extern int Term_Bp;
extern char Term_b;
extern char Term_Tabs;
extern unsigned char Term_Attr;
extern char Term_Esc;
extern char Term_EscPi;
extern unsigned char Term_EscP[2];
extern unsigned char Term_EscS;
extern char Term_EscST;

void Term_EscSP(char c);
void Term_EscInit();
void Term_EscCod(char c);
void Term_Cursor(char o);
void Term_Output(char c);      //user
void Char_Output(char c);      //user
void Char_TTOutput(char c);    //user
void Term_Edit();
void Term_Init();
void Term_Done();
void Term_SetCursorPos(char l,char c);
void Term_GetCursorPos(char far *l,char far *c);
void Term_GetVideoAttr(char far *modo,char far *ncols,char far *pact);
char Term_GetActivePage();
char Term_GetCols();
void Term_Cls();
void Term_ClearLine();
char Term_Input();                             //user
void Term_SetKeySend(char c);
void Term_SetKeyInit(char k);
void Term_SetAttr(unsigned char a);
void Term_SetModo(unsigned char m);
void Term_CursorR();
void Term_CursorL();
void Term_CursorU();
void Term_CursorD();
void Term_CursorF();
void Term_CursorB();
void ModoVideo(unsigned char m);
void far *GetIntVect(unsigned char i);
void SetIntVect(unsigned char i,void far *p);
void interrupt far NewKeyInt(void);
void Get_Str(char far *s);                           //user
void Term_EdCB(unsigned char c);
void Term_EdCF(unsigned char c);
void Term_EdWr(unsigned char ol);
void Term_Flush();               //user
void send_str(char far *s);                          //user
void write_str(char far *s);                         //user

#endif