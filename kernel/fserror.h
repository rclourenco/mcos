#ifndef _FSERROR_H
 #define _FSERROR_H
 
#define NUMERROS 13
//sem erros
#define EZERO 0
//acesso nao permitido
#define EACCES 1
//erro de alocacao de memoria
#define EMEM 2
//numero de ficheiro invalido
#define EBADF 3
//Erro de entradas e saidas
#define EINOUT 4
//Erro de existencia do ficheiro
#define EEXIST 5
//Erro interno ou desconhecido
#define EFAULT 6
//Erro de drive invalida
#define EINVDRV 7
//Demasiados ficheiros abertos
#define EMFILE 8
//O ficheiro nao existe
#define ENOFILE 9
//Nome de ficheiro invalido
#define EINVFILE 10
//Invalid filesystem
#define EINVFS   11
//Function unsuported
#define EUNSUP   12

extern char far *MsgErro[NUMERROS];
char far *GetMsgErro(unsigned int erro);

#endif
