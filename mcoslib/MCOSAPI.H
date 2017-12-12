#ifndef _MCOSAPI_H

#define MK_FP( seg,ofs )( (void _seg * )( seg ) +( void near * )( ofs ))
#define FP_SEG( fp )( (unsigned )( void _seg * )( void far * )( fp ))
#define FP_OFF( fp )( (unsigned )( fp ))

#define peek( a,b )( *( (int  far* )MK_FP( (a ),( b )) ))
#define peekb( a,b )( *( (char far* )MK_FP( (a ),( b )) ))
#define poke( a,b,c )( *( (int  far* )MK_FP( (a ),( b )) ) =( int )( c ))
#define pokeb( a,b,c )( *( (char far* )MK_FP( (a ),( b )) ) =( char )( c ))


#define _MCOSAPI_H

#define READMODE  (0x0)
#define WRITEMODE (0x7)

void mcos_print(char *str);
void mcos_clearerr();
int mcos_error();
int mcos_open(char *pathname, int mode);
void mcos_close(int handle);
int mcos_eof(int handle);
int mcos_fputc(int handle, char ch);
int mcos_fgetc(int handle);
unsigned mcos_read(int handle, unsigned char far *buffer, unsigned len);
void mcos_putchar(char c);
char mcos_getchar();
unsigned mcos_getkey();
unsigned long mcos_fsize(int handle);

unsigned gethseconds();
int getch();
int kbhit();
void nosound();
void sound(unsigned freq);

void printf(char *str,...);
void far *farmalloc(unsigned s);

void far *GetIntVect(unsigned char i);
void SetIntVect(unsigned char i,void far *p);

void _fstrncpy(char far *dest,char far *org, unsigned b);

#endif
