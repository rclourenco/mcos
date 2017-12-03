	INCLUDE MACROS.ASI

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

ExtSym@ DGROUP@, WORD,  __PASCAL__

ExtProc@  isr9main,   __CDECL__
ExtSym@  oldkeyb,dword  __CDECL__



	SUBTTL  Interrupt Services
	PAGE
_TEXT   SEGMENT

public _newkeyb2

	assume	cs:_TEXT
_newkeyb2	proc	far
	push	ax
	push	bx
	push	cx
	push	dx
	push	es
	push	ds
	push	si
	push	di
	push	bp
	mov	bp, cs:DGROUP@@	
	mov	ds,bp
	mov	bp,sp
	
	pushf
	call dword ptr _oldkeyb
	
	mov	sp,bp
	pop	bp
	pop	di
	pop	si
	pop	ds
	pop	es
	pop	dx
	pop	cx
	pop	bx
	pop	ax
	iret	
_newkeyb2	endp

_TEXT   ENDS

end