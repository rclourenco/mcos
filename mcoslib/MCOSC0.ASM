	NAME    c0
	PAGE    60,132
;[]------------------------------------------------------------[]
;|      C0.ASM -- Start Up Code                                 |
;|                                                              |
;|      Turbo-C Run Time Library        version 2.0             |
;|                                                              |
;|      Copyright (c) 1988 by Borland International Inc.        |
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
ExtProc@        mcos_makeargs,   __CDECL__
ExtProc@        exit,   __CDECL__

ExtSym@ _heaplen, WORD,  __CDECL__
ExtSym@ _stklen,  WORD,  __CDECL__

	SUBTTL  Start Up Code
	PAGE
;/*                                                     */
;/*-----------------------------------------------------*/
;/*                                                     */
;/*     Start Up Code                                   */
;/*     -------------                                   */
;/*                                                     */
;/*-----------------------------------------------------*/
;/*                                                     */
PSPHigh         equ     00002h
PSPEnv          equ     0002ch
PSPCmd          equ     00080h

	MINSTACK        equ     128     ; minimal stack size in words
;
;       At the start, DS and ES both point to the segment prefix.
;       SS points to the stack segment except in TINY model where
;       SS is equal to CS
;
_TEXT   SEGMENT

	ORG     100h
STARTX          PROC    NEAR
	jmp start_prog
	db 'MC'
;       Save general information, such as :
;               DGROUP segment address
;               DOS version number
;               Program Segment Prefix address
;               Environment address
;               Top of far heap
;       Reset uninitialized data area
start_prog:
		mov     dx, cs          ; DX = GROUP Segment address
		mov     cs:DGROUP@@, dx
		xor     ax, ax
		mov     es, cs:DGROUP@@
		mov     di, offset DGROUP: bdata@
		mov     cx, offset DGROUP: edata@
		sub     cx, di
		rep     stosb


;       Prepare main arguments

;       ExitCode = main(argc,argv,envp);
		call 	mcos_makeargs@

		push  	word ptr environ@
		push 	word ptr _argv@
		push  	_argc@
		call    main@
		pop 	cx
		pop 	cx
		pop 	cx
;       Flush and close streams and files

		call    _exit@

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


ExitToMCOS       label   near
		xor ax,ax
		int 80h
EndProc@        _exit, __CDECL__


public  N_SCOPY@

N_SCOPY@        proc near
                push    bp
                mov     bp,sp
                push    si
                push    di
                push    ds
                lds     si,dword ptr 4[bp]
                les     di,dword ptr 8[bp]
                cld
                shr     cx, 1
                rep     movsw
                adc     cx, cx
                rep     movsb
                pop     ds
                pop     di
                pop     si
                pop     bp
                ret     8
N_SCOPY@        endp

        public  N_LXMUL@

N_LXMUL@        PROC    NEAR
                push    si
                xchg    si, ax          ; save lo1
                xchg    ax, dx
                test    ax, ax          ; skip mul if hi1==0
                jz      nohi1
                mul     bx              ; hi1 * lo2

nohi1:          ; if we jumped here, ax==0 so the following swap works
                jcxz    nohi2           ; skip mul if hi2==0
                xchg    cx, ax          ; result <-> hi2
                mul     si              ; lo1 * hi2
                add     ax, cx          ; ax = hi1*lo2 + hi2*lo1
nohi2:
                xchg    ax, si
                mul     bx              ; lo1 * lo2
                add     dx, si          ; hi order result += partials
                pop     si
                ret
N_LXMUL@        ENDP


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


STARTX          ENDP

	SUBTTL  Miscellaneous
	PAGE

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


CopyRight       db      4 dup(0)
		db      'MCOS Software - Copyright (c) 2017 Renato Lourenzo.',0
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
PubSym@         _StartTime,     <dw    0,0>,            __CDECL__


;       Memory management variables

PubSym@         __heapbase,     <dw   DGROUP:edata@>,   __CDECL__
PubSym@         __brklvl,       <dw   DGROUP:edata@>,   __CDECL__
PubSym@         __heaptop,      <dw   DGROUP:edata@>,   __CDECL__

_DATA   ENDS


IFNDEF __HUGE__
_BSS    SEGMENT
bdata@  label   byte
_BSS    ENDS

_BSSEND SEGMENT
edata@  label   byte
_BSSEND ENDS
ENDIF

	END     STARTX
