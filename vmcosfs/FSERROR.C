#define NUMERROS 11
//sem erros
#define EZERO 0
//acesso n∆o permitido
#define EACCES 1
//erro de alocaá∆o de memoria
#define EMEM 2
//numero de ficheiro inv†lido
#define EBADF 3
//Erro de entradas e sa°das
#define EINOUT 4
//Erro de existencia do ficheiro
#define EEXIST 5
//Erro interno ou desconhecido
#define EFAULT 6
//Erro de drive inv†lida
#define EINVDRV 7
//Demasiados ficheiros abertos
#define EMFILE 8
//O ficheiro n∆o existe
#define ENOFILE 9
//Nome de ficheiro inv†lido
#define EINVFILE 10


char far *MsgErro[NUMERROS]={"N∆o ocurreu erro",
									"Acesso n∆o permitido",
									"Erro de alocaá∆o de mem¢ria",
									"Numero de ficheiro inv†lido",
									"Erro de Input/Output",
									"O ficheiro j† existe",
									"Erro interno ou desconhecido",
									"Drive inv†lida",
									"Demasiados ficheiros abertos",
									"O ficheiro n∆o existe",
									"Nome de ficheiro inv†lido"};
char far *GetMsgErro(unsigned int erro)    //user
{
	if(erro<NUMERROS)
		return MsgErro[erro];
	else
		return MsgErro[EFAULT];
}
