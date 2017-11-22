#INCLUDES=.
CC=tcc
CFLAGS=-mt

SRCS=mcoslib.c fserror.c mcosmem.c fsystem.c mcosterm.c mcos.c mcshell.c mcosmain.c

OBJS=$(SRCS:.c=.obj)

all: mcosmain.bin clean

mcosmain.bin: startbin.obj $(OBJS)
	@echo $(OBJS)
	tlink startbin $(OBJS) /s/c/m,mcosmain
	exe2bin mcosmain

startbin.obj: startbin.asm
    tasm /t/mx startbin,startbin

.c.obj:
	$(CC) $(CFLAGS) $(INCLUDES) -o$@ -c $<
	
clean:
	del *.obj
	del mcosmain.exe
	
