	NAME    c0
	PAGE    60,132
;[]------------------------------------------------------------[]
;|      STARTBIN.ASM -- Start Up Code                           |
;|                                                              |
;|      MicroComputer Operator System				            |
;|                                                              |
;|      Copyright (c) 2017 by Renato Lourenzo.                  |
;|      All Rights Reserved.                                    |
;[]------------------------------------------------------------[]

	INCLUDE MACROS.ASI

_Strict87_      equ     false           ; emulation skips peculiar details

;       Segment and Group declarations

_TEXT   SEGMENT BYTE PUBLIC 'CODE'
_TEXT   ENDS
_DATA   SEGMENT PARA PUBLIC 'DATA'
_DATA   ENDS
_BSS    SEGMENT WORD PUBLIC 'BSS'
_BSS    ENDS
_BSSEND SEGMENT BYTE PUBLIC 'STACK'
_BSSEND ENDS


DGROUP  GROUP   _TEXT, _DATA, _BSS, _BSSEND

	ASSUME  CS:_TEXT, DS:DGROUP

;       External References

ExtProc@        main,   __CDECL__
ExtProc@        exit,   __CDECL__
ExtProc@        pong,   __CDECL__
ExtProc@        putchar,   __CDECL__

ExtSym@ _heaplen, WORD,  __CDECL__
ExtSym@ _stklen,  WORD,  __CDECL__

	SUBTTL  Start Up Code
	PAGE
	MINSTACK        equ     128     ; minimal stack size in words
;
;       At the start, DS and ES both point to the segment prefix.
;       SS points to the stack segment except in TINY model where
;       SS is equal to CS
;
_TEXT   SEGMENT

	ORG     0h
STARTX          PROC    NEAR
;       Save general information, such as :
;               DGROUP segment address
;               DOS version number
;               Program Segment Prefix address
;               Environment address
;               Top of far heap
		mov     ax, cs          ; DX = GROUP Segment address
		mov     ds, ax
		mov 	byte ptr _BootUnit@, dl
		mov 	byte ptr _BootPartition@, dh
		mov dx, ax
		mov ax, 0FFF0h
		cli
		mov     sp, ax
		mov     ss, dx
		sti
		mov     cs:DGROUP@@, dx
		mov     _8087@, -1

;       Reset uninitialized data area

		xor     ax, ax
		mov     es, cs:DGROUP@@
		mov     di, offset DGROUP: bdata@
		mov     cx, offset DGROUP: edata@
		sub     cx, di
		rep     stosb

;       Prepare main arguments

    mov dx, ds
    mov es, dx

		mov     ah, 0
		int     1ah                     ; get current BIOS time in ticks
		mov     word ptr _StartTime1@,dx ; save it for clock() fn
		mov     word ptr _StartTime2@,cx


;       ExitCode = main(argc,argv,envp);
		call    main@

		push ax

;       Flush and close streams and files

		call    exit@
		pop     ax
    
;---------------------------------------------------------------------------
;       _exit()
;
;       Restore interrupt vectors taken during startup.  signal() functions
;       could have grabbed vectors 0, 4, 5 or 6.
;
;       Check for NULL pointer errors.
;
;       Exit to DOS.
;
;NOTE : _exit() doesn't close any files or run exit functions.  This is a
;       minimal 'cleanup & quit' program exit.
;---------------------------------------------------------------------------
PubProc@        _exit,  __CDECL__
		mov     ds, cs:DGROUP@@

    db 9Ah
    db 00h
    db 00h
    db 0FFh
    db 0FFh             ; Reboot
EndProc@        _exit, __CDECL__

PubProc@        _ping,  __CDECL__
    call       pong@
    ret
EndProc@        _ping, __CDECL__


public  N_SCOPY@

N_SCOPY@        proc near
                push    bp
                mov     bp,sp
                push    si
                push    di
                push    ds
                push    es
                lds     si,dword ptr 4[bp]
                les     di,dword ptr 8[bp]
                cld
                shr     cx, 1
                rep     movsw
                adc     cx, cx
                rep     movsb
                pop     es
                pop     ds
                pop     di
                pop     si
                pop     bp
                ret     8
N_SCOPY@        endp

        public  LDIV@
        public  F_LDIV@
        public  N_LDIV@
        public  LUDIV@
        public  F_LUDIV@
        public  N_LUDIV@
        public  LMOD@
        public  F_LMOD@
        public  N_LMOD@
        public  LUMOD@
        public  F_LUMOD@
        public  N_LUMOD@

N_LDIV@:
        pop     cx                      ;fix up far return
        push    cs
        push    cx
LDIV@:
F_LDIV@:
        xor     cx,cx                   ; signed divide
        jmp     short common

N_LUDIV@:
        pop     cx                      ;fix up far return
        push    cs
        push    cx
LUDIV@:
F_LUDIV@:
        mov     cx,1                    ; unsigned divide
        jmp     short common

N_LMOD@:
        pop     cx                      ;fix up far return
        push    cs
        push    cx
LMOD@:
F_LMOD@:
        mov     cx,2                    ; signed remainder
        jmp     short   common

N_LUMOD@:
        pop     cx                      ;fix up far return
        push    cs
        push    cx
LUMOD@:
F_LUMOD@:
        mov     cx,3                    ; unsigned remainder

;
;       di now contains a two bit control value.  The low order
;       bit (test mask of 1) is on if the operation is unsigned,
;       signed otherwise.  The next bit (test mask of 2) is on if
;       the operation returns the remainder, quotient otherwise.
;
common:
        push    bp
        push    si
        push    di
        mov     bp,sp                   ; set up frame
        mov     di,cx
;
;       dividend is pushed last, therefore the first in the args
;       divisor next.
;
        mov     ax,10[bp]               ; get the first low word
        mov     dx,12[bp]               ; get the first high word
        mov     bx,14[bp]               ; get the second low word
        mov     cx,16[bp]               ; get the second high word

        or      cx,cx
        jnz     @@slow@ldiv             ; both high words are zero

        or      dx,dx
        jz      @@quick@ldiv

        or      bx,bx
        jz      @@quick@ldiv            ; if cx:bx == 0 force a zero divide
                                        ; we don't expect this to actually
                                        ; work

@@slow@ldiv:

        test    di,1                    ; signed divide?
        jnz     @@positive              ; no: skip
;
;               Signed division should be done.  Convert negative
;               values to positive and do an unsigned division.
;               Store the sign value in the next higher bit of
;               di (test mask of 4).  Thus when we are done, testing
;               that bit will determine the sign of the result.
;
        or      dx,dx                   ; test sign of dividend
        jns     @@onepos
        neg     dx
        neg     ax
        sbb     dx,0                    ; negate dividend
        or      di,0Ch
@@onepos:
        or      cx,cx                   ; test sign of divisor
        jns     @@positive
        neg     cx
        neg     bx
        sbb     cx,0                    ; negate divisor
        xor     di,4
@@positive:
        mov     bp,cx
        mov     cx,32                   ; shift counter
        push    di                      ; save the flags
;
;       Now the stack looks something like this:
;
;               16[bp]: divisor (high word)
;               14[bp]: divisor (low word)
;               12[bp]: dividend (high word)
;               10[bp]: dividend (low word)
;                8[bp]: return CS
;                6[bp]: return IP
;                4[bp]: previous BP
;                2[bp]: previous SI
;                 [bp]: previous DI
;               -2[bp]: control bits
;                       01 - Unsigned divide
;                       02 - Remainder wanted
;                       04 - Negative quotient
;                       08 - Negative remainder
;
        xor     di,di                   ; fake a 64 bit dividend
        xor     si,si                   ;
@@xloop:
        shl     ax,1                    ; shift dividend left one bit
        rcl     dx,1
        rcl     si,1
        rcl     di,1
        cmp     di,bp                   ; dividend larger?
        jb      @@nosub
        ja      @@subtract
        cmp     si,bx                   ; maybe
        jb      @@nosub
@@subtract:
        sub     si,bx
        sbb     di,bp                   ; subtract the divisor
        inc     ax                      ; build quotient
@@nosub:
        loop    @@xloop
;
;       When done with the loop the four register value look like:
;
;       |     di     |     si     |     dx     |     ax     |
;       |        remainder        |         quotient        |
;
        pop     bx                      ; get control bits
        test    bx,2                    ; remainder?
        jz      @@usequo
        mov     ax,si
        mov     dx,di                   ; use remainder
        shr     bx,1                    ; shift in the remainder sign bit
@@usequo:
        test    bx,4                    ; needs negative
        jz      @@finish
        neg     dx
        neg     ax
        sbb     dx,0                    ; negate
@@finish:
        pop     di
        pop     si
        pop     bp
        retf    8

@@quick@ldiv:
        div     bx                      ; unsigned divide
                                        ; DX = remainder AX = quotient
        test    di,2                    ; want remainder?
        jz      @@quick@quo
        xchg    ax,dx

@@quick@quo:

        xor     dx,dx
        jmp     short @@finish

INCLUDE CSUBS.ASI

STARTX          ENDP

	SUBTTL  Miscellaneous
;	PAGE

;       The DGROUP@ variable is used to reload DS with DGROUP

PubSym@ DGROUP@, <dw    ?>, __PASCAL__
_TEXT   ENDS

	SUBTTL  Start Up Data Area
	PAGE
;[]------------------------------------------------------------[]
;|      Start Up Data Area                                      |
;|                                                              |
;|      WARNING         Do not move any variables in the data   |
;|                      segment unless you're absolutely sure   |
;|                      that it does not matter.                |
;|                                                              |
;[]------------------------------------------------------------[]

_DATA   SEGMENT

;       The CopyRight string must NOT be moved or changed without
;       changing the null pointer check logic

CopyRight       db      4 dup(0)
		db      'MCOS - Copyright (c) 2017 Renato Lourenzo.',0
lgth_CopyRight  equ     $ - CopyRight


;
;                       Miscellaneous variables
;       
PubSym@         _argc,          <dw     0>,             __CDECL__
dPtrPub@        _argv,          0,                      __CDECL__
dPtrPub@        environ,        0,                      __CDECL__
PubSym@         _envLng,        <dw     0>,             __CDECL__
PubSym@         _envseg,        <dw    0>,              __CDECL__
PubSym@         _envSize,       <dw    0>,              __CDECL__
PubSym@         _psp,           <dw    0>,              __CDECL__
PubSym@         _version,       <label word>,           __CDECL__
PubSym@         _osmajor,       <db    0>,              __CDECL__
PubSym@         _osminor,       <db    0>,              __CDECL__
PubSym@         errno,          <dw    0>,              __CDECL__
PubSym@         _8087,          <dw    0>,              __CDECL__
PubSym@         _StartTime1,    <dw    0>,            __CDECL__
PubSym@         _StartTime2,    <dw    0>,            __CDECL__
PubSym@         _BootUnit,      <db    0>,            __CDECL__
PubSym@         _BootPartition  <db    0>,            __CDECL__

;       Memory management variables

PubSym@         __heapbase,     <dw   DGROUP:edata@>,   __CDECL__
PubSym@         __brklvl,       <dw   DGROUP:edata@>,   __CDECL__
;PubSym@         __heaptop,      <dw   DGROUP:edata@>,   __CDECL__

_DATA   ENDS


_BSS    SEGMENT
bdata@  label   byte
_BSS    ENDS

_BSSEND SEGMENT
edata@  label   byte
_BSSEND ENDS


	END     STARTX
