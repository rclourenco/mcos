#ifndef _MCOSAPI_H

#define MK_FP( seg,ofs )( (void _seg * )( seg ) +( void near * )( ofs ))
#define FP_SEG( fp )( (unsigned )( void _seg * )( void far * )( fp ))
#define FP_OFF( fp )( (unsigned )( fp ))

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
void mcos_putchar(char c);
char mcos_getchar();
void printf(char *str,...);

#endif