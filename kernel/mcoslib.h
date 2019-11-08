#ifndef _MCOSLIB_H

#define _MCOSLIB_H

#define TRUE 1
#define FALSE 0
#define SYSTEMID 1
#define WORD unsigned int
#define DWORD unsigned long
#define BYTE unsigned char
#define peek( a,b )( *( (int  far* )MK_FP( (a ),( b )) ))
#define peekb( a,b )( *( (char far* )MK_FP( (a ),( b )) ))
#define poke( a,b,c )( *( (int  far* )MK_FP( (a ),( b )) ) =( int )( c ))
#define pokeb( a,b,c )( *( (char far* )MK_FP( (a ),( b )) ) =( char )( c ))
#define MK_FP( seg,ofs )( (void _seg * )( seg ) +( void near * )( ofs ))
#define FP_SEG( fp )( (unsigned )( void _seg * )( void far * )( fp ))
#define FP_OFF( fp )( (unsigned )( fp ))


typedef struct{
	WORD ano;
	BYTE dia;
	BYTE mes;
}TDATA;


typedef struct{
	BYTE minutos;
	BYTE hora;
	BYTE centesimos;
	BYTE segundos;
}THORA;


void dels(char far *st,unsigned p,unsigned t);
void ins(char far *st,char far *is, unsigned p);
void insc(char far *st,char ic,unsigned p);
unsigned Strlenx(char far *s);
int Strcmpx(char far *s1,char far *s2);
void Strcpyx(char far *dest,char far *or);
void Strcpy(char far *dest,char far *or);
unsigned Strlen(char far *s);
int Strcmp(char far *s1,char far *s2);
unsigned GetKey();
void _ultoa(unsigned long value, char far *string,char m,unsigned t);
unsigned long power(unsigned long n,unsigned p);
unsigned char bcd2bin(unsigned char bcd);
unsigned char bin2bcd(unsigned char bin);
void GetDate(TDATA far *data);    //user
void GetHora(THORA far *hora);    //user

/*kernel printf*/
void putchar(char ch);
void writestr(char far *str);
void writehex(unsigned d);
void writenum(unsigned d);
void writelongnum(unsigned long);
void kprintf(char *str, ...);

void _fmemcpy(void far *a, void far *b, unsigned s);
void _fmemset(void far *a, int c, unsigned s);

unsigned long longmul2(unsigned long a, unsigned long b);

extern char hextab[];

#endif
