#include "fserror.h"

char far *MsgErro[NUMERROS]={"No error",
									"Forbidden Access",
									"Memory allocation error",
									"Invalid descriptor",
									"Input/Output Error",
									"File already exists",
									"Internal Error",
									"Invalid Drive Unit",
									"Too many open files",
									"File not found",
									"Invalid filename"};
									
char far *GetMsgErro(unsigned int erro)    //user
{
	if(erro<NUMERROS)
		return MsgErro[erro];
	else
		return MsgErro[EFAULT];
}