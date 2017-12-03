	ifndef	??version
?debug	macro
	endm
$comm	macro	name,dist,size,count
	comm	dist name:BYTE:count*size
	endm
	else
$comm	macro	name,dist,size,count
	comm	dist name[size]:BYTE:count
	endm
	endif
	?debug	S "draw.c"
	?debug	C E9000D834B06647261772E63
	?debug	C E94BAB824B0A67726170686C69622E63
	?debug	C E9C1B2824B096D636F736170692E68
	?debug	C E96594824B09706963747572652E68
_TEXT	segment byte public 'CODE'
_TEXT	ends
DGROUP	group	_DATA,_BSS
	assume	cs:_TEXT,ds:DGROUP
_DATA	segment word public 'DATA'
d@	label	byte
d@w	label	word
_DATA	ends
_BSS	segment word public 'BSS'
b@	label	byte
b@w	label	word
_BSS	ends
_DATA	segment word public 'DATA'
_DRAW	label	byte
	db	0
_Color	label	byte
	db	15
_FillColor	label	byte
	db	15
_TextColor	label	byte
	db	7
_TextBackGround	label	byte
	db	255
_dscreen	label	dword
	db	0
	db	0
	db	0
	db	160
_screen	label	dword
	db	0
	db	0
	db	0
	db	160
_ScrnT	label	byte
	db	0
_cart	label	dword
	db	0
	db	0
	db	0
	db	0
_DATA	ends
_TEXT	segment byte public 'CODE'
   ;	
   ;	void _fmemset(void far *a, int c, unsigned s)
   ;	
	assume	cs:_TEXT
__fmemset	proc	near
	push	bp
	mov	bp,sp
	push	di
   ;	
   ;	{
   ;		asm {
   ;			mov ax, word ptr [a+2];
   ;	
	mov	 ax, word ptr [[bp+4]+2]
   ;	
   ;			mov es, ax;
   ;	
	mov	 es, ax
   ;	
   ;			mov di, word ptr [a];
   ;	
	mov	 di, word ptr [[bp+4]]
   ;	
   ;			mov cx, s;
   ;	
	mov	 cx, [bp+10]
   ;	
   ;			mov ax, c;
   ;	
	mov	 ax, [bp+8]
   ;	
   ;			cld;
   ;	
	cld	
   ;	
   ;			rep stosb;
   ;	
	rep stosb	
   ;	
   ;		}
   ;	}
   ;	
	pop	di
	pop	bp
	ret	
__fmemset	endp
   ;	
   ;	void _fmemcpy(void far *a, void far *b, unsigned s)
   ;	
	assume	cs:_TEXT
__fmemcpy	proc	near
	push	bp
	mov	bp,sp
	push	si
	push	di
   ;	
   ;	{
   ;		asm {
   ;			push ds;
   ;	
	push	 ds
   ;	
   ;			les di, a;
   ;	
	les	 di, [bp+4]
   ;	
   ;			lds si, b;
   ;	
	lds	 si, [bp+8]
   ;	
   ;			mov cx, s;
   ;	
	mov	 cx, [bp+12]
   ;	
   ;			cld;
   ;	
	cld	
   ;	
   ;			rep movsb;
   ;	
	rep movsb	
   ;	
   ;			pop ds;
   ;	
	pop	 ds
   ;	
   ;		}
   ;	}
   ;	
	pop	di
	pop	si
	pop	bp
	ret	
__fmemcpy	endp
   ;	
   ;	void modo13h()
   ;	
	assume	cs:_TEXT
_modo13h	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;		asm {
   ;			mov ax, 0x13;
   ;	
	mov	 ax, 013H
   ;	
   ;			int 0x10;
   ;	
	int	 010H
   ;	
   ;		}
   ;	}
   ;	
	pop	bp
	ret	
_modo13h	endp
   ;	
   ;	void modo3h()
   ;	
	assume	cs:_TEXT
_modo3h	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;		asm {
   ;			mov ax, 0x3;
   ;	
	mov	 ax, 03H
   ;	
   ;			int 0x10;
   ;	
	int	 010H
   ;	
   ;		}
   ;	}
   ;	
	pop	bp
	ret	
_modo3h	endp
   ;	
   ;	unsigned char far *getcpt()
   ;	
	assume	cs:_TEXT
_getcpt	proc	near
	push	bp
	mov	bp,sp
	sub	sp,4
   ;	
   ;	{
   ;		unsigned s;
   ;		unsigned o;
   ;	
   ;		asm {
   ;			push es;
   ;	
	push	 es
   ;	
   ;			push bp;
   ;	
	push	 bp
   ;	
   ;			
   ;			mov ax, 0x1130;
   ;	
	mov	 ax, 01130H
   ;	
   ;			mov bx, 0x0300;
   ;	
	mov	 bx, 00300H
   ;	
   ;			int 0x10;
   ;	
	int	 010H
   ;	
   ;			mov ax, es;
   ;	
	mov	 ax, es
   ;	
   ;			mov bx, bp;
   ;	
	mov	 bx, bp
   ;	
   ;			pop bp;
   ;	
	pop	 bp
   ;	
   ;			pop es;
   ;	
	pop	 es
   ;	
   ;	
   ;			mov s, ax;
   ;	
	mov	 [bp-2], ax
   ;	
   ;			mov o, bx;
   ;	
	mov	 [bp-4], bx
   ;	
   ;		}
   ;		
   ;		
   ;		return ((unsigned char far *) MK_FP(s,o));
   ;	
	mov	dx,word ptr [bp-2]
	mov	ax,word ptr [bp-4]
	jmp	short @5@366
@5@366:
   ;	
   ;	}
   ;	
	mov	sp,bp
	pop	bp
	ret	
_getcpt	endp
   ;	
   ;	void pchar(int x,int y, unsigned char c)
   ;	
	assume	cs:_TEXT
_pchar	proc	near
	push	bp
	mov	bp,sp
	sub	sp,2
	push	si
	push	di
   ;	
   ;	{
   ;		int cx,cy;
   ;		unsigned char v;
   ;		if(!cart)
   ;	
	mov	ax,word ptr DGROUP:_cart
	or	ax,word ptr DGROUP:_cart+2
	jne	short @6@86
   ;	
   ;			cart=getcpt();
   ;	
	call	near ptr _getcpt
	mov	word ptr DGROUP:_cart+2,dx
	mov	word ptr DGROUP:_cart,ax
@6@86:
   ;	
   ;		for(cy=0;cy<8;cy++)
   ;	
	xor	si,si
	jmp	@6@366
@6@114:
   ;	
   ;		{
   ;			v=cart[8*c+cy];
   ;	
	mov	al,byte ptr [bp+8]
	mov	ah,0
	shl	ax,1
	shl	ax,1
	shl	ax,1
	add	ax,si
	mov	es,word ptr DGROUP:_cart+2
	add	ax,word ptr DGROUP:_cart
	mov	bx,ax
	mov	al,byte ptr es:[bx]
	mov	byte ptr [bp-1],al
   ;	
   ;			for(cx=0;cx<8;cx++)
   ;	
	xor	di,di
	jmp	short @6@310
@6@142:
   ;	
   ;			{
   ;				if((v&128)==128)
   ;	
	mov	al,byte ptr [bp-1]
	mov	ah,0
	and	ax,128
	cmp	ax,128
	jne	short @6@198
   ;	
   ;					PLOT(x+cx,y+cy,TextColor);
   ;	
	mov	ax,word ptr [bp+6]
	add	ax,si
	mov	dx,320
	imul	dx
	mov	bx,word ptr [bp+4]
	add	bx,di
	add	bx,ax
	mov	es,word ptr DGROUP:_screen+2
	add	bx,word ptr DGROUP:_screen
	mov	al,byte ptr DGROUP:_TextColor
	mov	byte ptr es:[bx],al
	jmp	short @6@254
@6@198:
   ;	
   ;				else
   ;				if(TextBackGround!=255)
   ;	
	cmp	byte ptr DGROUP:_TextBackGround,255
	je	short @6@254
   ;	
   ;					PLOT(x+cx,y+cy,TextBackGround);
   ;	
	mov	ax,word ptr [bp+6]
	add	ax,si
	mov	dx,320
	imul	dx
	mov	bx,word ptr [bp+4]
	add	bx,di
	add	bx,ax
	mov	es,word ptr DGROUP:_screen+2
	add	bx,word ptr DGROUP:_screen
	mov	al,byte ptr DGROUP:_TextBackGround
	mov	byte ptr es:[bx],al
@6@254:
   ;	
   ;				v=v<<1;
   ;	
	mov	al,byte ptr [bp-1]
	shl	al,1
	mov	byte ptr [bp-1],al
	inc	di
@6@310:
	cmp	di,8
	jl	short @6@142
	inc	si
@6@366:
	cmp	si,8
	jge	@@0
	jmp	@6@114
@@0:
   ;	
   ;			}
   ;		}
   ;	}
   ;	
	pop	di
	pop	si
	mov	sp,bp
	pop	bp
	ret	
_pchar	endp
   ;	
   ;	void writest(int x,int y,unsigned char *st)
   ;	
	assume	cs:_TEXT
_writest	proc	near
	push	bp
	mov	bp,sp
	push	si
	push	di
	mov	di,word ptr [bp+4]
	mov	si,word ptr [bp+8]
	jmp	short @7@86
@7@58:
   ;	
   ;	{
   ;		while(*st)
   ;		{
   ;			pchar(x,y,*st);
   ;	
	mov	al,byte ptr [si]
	push	ax
	push	word ptr [bp+6]
	push	di
	call	near ptr _pchar
	add	sp,6
   ;	
   ;			st++;
   ;	
	inc	si
   ;	
   ;			x+=8;
   ;	
	add	di,8
@7@86:
	cmp	byte ptr [si],0
	jne	short @7@58
   ;	
   ;		}
   ;	}
   ;	
	pop	di
	pop	si
	pop	bp
	ret	
_writest	endp
   ;	
   ;	void fillscreen(char a)
   ;	
	assume	cs:_TEXT
_fillscreen	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;		_fmemset(screen,a,64000);
   ;	
	mov	ax,64000
	push	ax
	mov	al,byte ptr [bp+4]
	cbw	
	push	ax
	push	word ptr DGROUP:_screen+2
	push	word ptr DGROUP:_screen
	call	near ptr __fmemset
	add	sp,8
   ;	
   ;	}
   ;	
	pop	bp
	ret	
_fillscreen	endp
   ;	
   ;	void DrawRect(int x1,int y1,int x2,int y2,char op)
   ;	
	assume	cs:_TEXT
_DrawRect	proc	near
	push	bp
	mov	bp,sp
	sub	sp,2
	push	si
	push	di
   ;	
   ;	{
   ;		int x,y;
   ;		int cor;
   ;		if(x1>x2) swap(&x1,&x2);
   ;	
	mov	ax,word ptr [bp+4]
	cmp	ax,word ptr [bp+8]
	jle	short @9@86
	lea	ax,word ptr [bp+8]
	push	ax
	lea	ax,word ptr [bp+4]
	push	ax
	call	near ptr _swap
	pop	cx
	pop	cx
@9@86:
   ;	
   ;		if(y1>y2) swap(&y1,&y2);
   ;	
	mov	ax,word ptr [bp+6]
	cmp	ax,word ptr [bp+10]
	jle	short @9@142
	lea	ax,word ptr [bp+10]
	push	ax
	lea	ax,word ptr [bp+6]
	push	ax
	call	near ptr _swap
	pop	cx
	pop	cx
@9@142:
   ;	
   ;		if(op!=1)
   ;	
	cmp	byte ptr [bp+12],1
	je	short @9@366
   ;	
   ;		{
   ;			for(x=x1+1;x<x2;x++)
   ;	
	mov	ax,word ptr [bp+4]
	inc	ax
	mov	si,ax
	jmp	short @9@338
@9@198:
   ;	
   ;			for(y=y1+1;y<y2;y++)
   ;	
	mov	ax,word ptr [bp+6]
	inc	ax
	mov	di,ax
	jmp	short @9@282
@9@226:
   ;	
   ;				DrawPixel(x,y,FillColor);
   ;	
	mov	al,byte ptr DGROUP:_FillColor
	push	ax
	push	di
	push	si
	call	near ptr _DrawPixel
	add	sp,6
	inc	di
@9@282:
	cmp	di,word ptr [bp+10]
	jl	short @9@226
	inc	si
@9@338:
	cmp	si,word ptr [bp+8]
	jl	short @9@198
@9@366:
   ;	
   ;		}
   ;		if(op==2)
   ;	
	cmp	byte ptr [bp+12],2
	jne	short @9@422
   ;	
   ;			cor=FillColor;
   ;	
	mov	al,byte ptr DGROUP:_FillColor
	jmp	short @9@450
@9@422:
   ;	
   ;		else
   ;			cor=Color;
   ;	
	mov	al,byte ptr DGROUP:_Color
@9@450:
	mov	ah,0
	mov	word ptr [bp-2],ax
   ;	
   ;		for(x=x1;x<=x2;x++)
   ;	
	mov	si,word ptr [bp+4]
	jmp	short @9@562
@9@506:
   ;	
   ;		{
   ;			DrawPixel(x,y1,cor);
   ;	
	mov	al,byte ptr [bp-2]
	push	ax
	push	word ptr [bp+6]
	push	si
	call	near ptr _DrawPixel
	add	sp,6
   ;	
   ;			DrawPixel(x,y2,cor);
   ;	
	mov	al,byte ptr [bp-2]
	push	ax
	push	word ptr [bp+10]
	push	si
	call	near ptr _DrawPixel
	add	sp,6
	inc	si
@9@562:
	cmp	si,word ptr [bp+8]
	jle	short @9@506
   ;	
   ;		}
   ;		for(y=y1+1;y<y2;y++)
   ;	
	mov	ax,word ptr [bp+6]
	inc	ax
	mov	di,ax
	jmp	short @9@674
@9@618:
   ;	
   ;		{
   ;			DrawPixel(x1,y,cor);
   ;	
	mov	al,byte ptr [bp-2]
	push	ax
	push	di
	push	word ptr [bp+4]
	call	near ptr _DrawPixel
	add	sp,6
   ;	
   ;			DrawPixel(x2,y,cor);
   ;	
	mov	al,byte ptr [bp-2]
	push	ax
	push	di
	push	word ptr [bp+8]
	call	near ptr _DrawPixel
	add	sp,6
	inc	di
@9@674:
	cmp	di,word ptr [bp+10]
	jl	short @9@618
   ;	
   ;		}
   ;	}
   ;	
	pop	di
	pop	si
	mov	sp,bp
	pop	bp
	ret	
_DrawRect	endp
   ;	
   ;	void DrawPixel(int x,int y,char c)
   ;	
	assume	cs:_TEXT
_DrawPixel	proc	near
	push	bp
	mov	bp,sp
	push	si
	push	di
	mov	si,word ptr [bp+4]
	mov	di,word ptr [bp+6]
	mov	cl,byte ptr [bp+8]
   ;	
   ;	{
   ;		if ((x>=0)&&(y>=0)&&(x<320)&&(y<200))
   ;	
	or	si,si
	jge	@@1
	jmp	@10@422
@@1:
	or	di,di
	jge	@@2
	jmp	@10@422
@@2:
	cmp	si,320
	jl	@@3
	jmp	@10@422
@@3:
	cmp	di,200
	jl	@@4
	jmp	@10@422
@@4:
   ;	
   ;		{
   ;			switch (DRAW)
   ;	
	mov	al,byte ptr DGROUP:_DRAW
	mov	ah,0
	cmp	ax,1
	je	short @10@282
	cmp	ax,2
	je	short @10@338
	cmp	ax,3
	je	short @10@366
	jmp	short @10@394
@10@282:
   ;	
   ;			{
   ;				case XOR_D:PLOT(x,y,c^GETPLOT(x,y));break;
   ;	
	mov	ax,di
	mov	dx,320
	imul	dx
	mov	bx,si
	add	bx,ax
	mov	es,word ptr DGROUP:_screen+2
	add	bx,word ptr DGROUP:_screen
	mov	al,cl
	xor	al,byte ptr es:[bx]
@10@310:
	push	ax
	mov	ax,di
	mov	dx,320
	imul	dx
	mov	bx,si
	add	bx,ax
	mov	es,word ptr DGROUP:_screen+2
	add	bx,word ptr DGROUP:_screen
	pop	ax
	mov	byte ptr es:[bx],al
	jmp	short @10@422
@10@338:
   ;	
   ;				case AND_D:PLOT(x,y,c&GETPLOT(x,y));break;
   ;	
	mov	ax,di
	mov	dx,320
	imul	dx
	mov	bx,si
	add	bx,ax
	mov	es,word ptr DGROUP:_screen+2
	add	bx,word ptr DGROUP:_screen
	mov	al,cl
	and	al,byte ptr es:[bx]
	jmp	short @10@310
@10@366:
   ;	
   ;				case OR_D:PLOT(x,y,c|GETPLOT(x,y));break;
   ;	
	mov	ax,di
	mov	dx,320
	imul	dx
	mov	bx,si
	add	bx,ax
	mov	es,word ptr DGROUP:_screen+2
	add	bx,word ptr DGROUP:_screen
	mov	al,cl
	or	al,byte ptr es:[bx]
	jmp	short @10@310
@10@394:
   ;	
   ;				default:PLOT(x,y,c);
   ;	
	mov	ax,di
	mov	dx,320
	imul	dx
	mov	bx,si
	add	bx,ax
	mov	es,word ptr DGROUP:_screen+2
	add	bx,word ptr DGROUP:_screen
	mov	al,cl
	mov	byte ptr es:[bx],al
@10@422:
   ;	
   ;			}
   ;	
   ;		}
   ;	
   ;	}
   ;	
	pop	di
	pop	si
	pop	bp
	ret	
_DrawPixel	endp
   ;	
   ;	void dline(int x1, int y1, int x2, int y2)
   ;	
	assume	cs:_TEXT
_dline	proc	near
	push	bp
	mov	bp,sp
	sub	sp,12
	push	si
	push	di
   ;	
   ;	{
   ;		int dx,dy,d,y,x;
   ;		int ax,ay;
   ;		int sx,sy;
   ;	
   ;		dx = x2-x1;
   ;	
	mov	ax,word ptr [bp+8]
	sub	ax,word ptr [bp+4]
	mov	word ptr [bp-2],ax
   ;	
   ;		dy = y2-y1;
   ;	
	mov	ax,word ptr [bp+10]
	sub	ax,word ptr [bp+6]
	mov	word ptr [bp-4],ax
   ;	
   ;	
   ;		sx = (x2<x1?-1:1);
   ;	
	mov	ax,word ptr [bp+8]
	cmp	ax,word ptr [bp+4]
	jge	short @11@86
	mov	ax,65535
	jmp	short @11@114
@11@86:
	mov	ax,1
@11@114:
	mov	word ptr [bp-10],ax
   ;	
   ;		sy = (y2<y1?-1:1);
   ;	
	mov	ax,word ptr [bp+10]
	cmp	ax,word ptr [bp+6]
	jge	short @11@170
	mov	ax,65535
	jmp	short @11@198
@11@170:
	mov	ax,1
@11@198:
	mov	word ptr [bp-12],ax
   ;	
   ;		ax = 2*(dx<0?-dx:dx);
   ;	
	cmp	word ptr [bp-2],0
	jge	short @11@254
	mov	ax,word ptr [bp-2]
	neg	ax
	jmp	short @11@282
@11@254:
	mov	ax,word ptr [bp-2]
@11@282:
	shl	ax,1
	mov	word ptr [bp-6],ax
   ;	
   ;		ay = 2*(dy<0?-dy:dy);
   ;	
	cmp	word ptr [bp-4],0
	jge	short @11@338
	mov	ax,word ptr [bp-4]
	neg	ax
	jmp	short @11@366
@11@338:
	mov	ax,word ptr [bp-4]
@11@366:
	shl	ax,1
	mov	word ptr [bp-8],ax
   ;	
   ;	
   ;		x=x1;
   ;	
	mov	ax,word ptr [bp+4]
	mov	cx,ax
   ;	
   ;		y=y1;
   ;	
	mov	di,word ptr [bp+6]
   ;	
   ;		if( ax >= ay ) {
   ;	
	mov	ax,word ptr [bp-6]
	cmp	ax,word ptr [bp-8]
	jl	short @11@618
   ;	
   ;			d=ay-ax/2;
   ;	
	mov	ax,word ptr [bp-6]
	mov	bx,2
	cwd	
	idiv	bx
	mov	dx,word ptr [bp-8]
	sub	dx,ax
	mov	si,dx
@11@422:
   ;	
   ;			while(1)
   ;			{
   ;				PLOT(x,y,Color);
   ;	
	mov	ax,di
	mov	dx,320
	imul	dx
	mov	bx,cx
	add	bx,ax
	mov	es,word ptr DGROUP:_screen+2
	add	bx,word ptr DGROUP:_screen
	mov	al,byte ptr DGROUP:_Color
	mov	byte ptr es:[bx],al
   ;	
   ;				if(x==x2) break;
   ;	
	mov	ax,cx
	cmp	ax,word ptr [bp+8]
	jne	short @11@478
	jmp	short @11@590
@11@478:
   ;	
   ;				if(d>=0) {
   ;	
	or	si,si
	jl	short @11@534
   ;	
   ;					y=y+sy;
   ;	
	mov	ax,di
	add	ax,word ptr [bp-12]
	mov	di,ax
   ;	
   ;					d=d-ax;
   ;	
	mov	ax,si
	sub	ax,word ptr [bp-6]
	mov	si,ax
@11@534:
   ;	
   ;				}
   ;				d=d+ay;
   ;	
	mov	ax,si
	add	ax,word ptr [bp-8]
	mov	si,ax
   ;	
   ;				x=x+sx;
   ;	
	mov	ax,cx
	add	ax,word ptr [bp-10]
	mov	cx,ax
	jmp	short @11@422
@11@590:
   ;	
   ;			}
   ;		}
   ;	
	jmp	short @11@814
@11@618:
   ;	
   ;		else {
   ;			d=ax-ay/2;
   ;	
	mov	ax,word ptr [bp-8]
	mov	bx,2
	cwd	
	idiv	bx
	mov	dx,word ptr [bp-6]
	sub	dx,ax
	mov	si,dx
@11@646:
   ;	
   ;			while(1)
   ;			{
   ;				PLOT(x,y,Color);
   ;	
	mov	ax,di
	mov	dx,320
	imul	dx
	mov	bx,cx
	add	bx,ax
	mov	es,word ptr DGROUP:_screen+2
	add	bx,word ptr DGROUP:_screen
	mov	al,byte ptr DGROUP:_Color
	mov	byte ptr es:[bx],al
   ;	
   ;				if(y==y2) break;
   ;	
	cmp	di,word ptr [bp+10]
	jne	short @11@702
	jmp	short @11@814
@11@702:
   ;	
   ;				if(d>=0) {
   ;	
	or	si,si
	jl	short @11@758
   ;	
   ;					x=x+sx;
   ;	
	mov	ax,cx
	add	ax,word ptr [bp-10]
	mov	cx,ax
   ;	
   ;					d=d-ay;
   ;	
	mov	ax,si
	sub	ax,word ptr [bp-8]
	mov	si,ax
@11@758:
   ;	
   ;				}
   ;				d=d+ax;
   ;	
	mov	ax,si
	add	ax,word ptr [bp-6]
	mov	si,ax
   ;	
   ;				y=y+sy;
   ;	
	mov	ax,di
	add	ax,word ptr [bp-12]
	mov	di,ax
	jmp	short @11@646
@11@814:
   ;	
   ;			}
   ;		}
   ;	
   ;	}
   ;	
	pop	di
	pop	si
	mov	sp,bp
	pop	bp
	ret	
_dline	endp
   ;	
   ;	void dcircle(int x0, int y0, int r)
   ;	
	assume	cs:_TEXT
_dcircle	proc	near
	push	bp
	mov	bp,sp
	sub	sp,6
	push	si
	push	di
   ;	
   ;	{
   ;		int x = r - 1;
   ;	
	mov	ax,word ptr [bp+8]
	dec	ax
	mov	si,ax
   ;	
   ;		int y = 0;
   ;	
	xor	di,di
   ;	
   ;		int dx = 1;
   ;	
	mov	word ptr [bp-2],1
   ;	
   ;		int dy = 1;
   ;	
	mov	word ptr [bp-4],1
   ;	
   ;		int err = dx - (r<<1);
   ;	
	mov	ax,word ptr [bp+8]
	shl	ax,1
	mov	dx,word ptr [bp-2]
	sub	dx,ax
	mov	word ptr [bp-6],dx
	jmp	@12@170
@12@58:
   ;	
   ;	
   ;		while(x>=y)
   ;		{
   ;			DrawPixel(x0+x,y0+y,Color);
   ;	
	mov	al,byte ptr DGROUP:_Color
	push	ax
	mov	ax,word ptr [bp+6]
	add	ax,di
	push	ax
	mov	ax,word ptr [bp+4]
	add	ax,si
	push	ax
	call	near ptr _DrawPixel
	add	sp,6
   ;	
   ;			DrawPixel(x0+x,y0-y,Color);
   ;	
	mov	al,byte ptr DGROUP:_Color
	push	ax
	mov	ax,word ptr [bp+6]
	sub	ax,di
	push	ax
	mov	ax,word ptr [bp+4]
	add	ax,si
	push	ax
	call	near ptr _DrawPixel
	add	sp,6
   ;	
   ;			DrawPixel(x0-x,y0+y,Color);
   ;	
	mov	al,byte ptr DGROUP:_Color
	push	ax
	mov	ax,word ptr [bp+6]
	add	ax,di
	push	ax
	mov	ax,word ptr [bp+4]
	sub	ax,si
	push	ax
	call	near ptr _DrawPixel
	add	sp,6
   ;	
   ;			DrawPixel(x0-x,y0-y,Color);
   ;	
	mov	al,byte ptr DGROUP:_Color
	push	ax
	mov	ax,word ptr [bp+6]
	sub	ax,di
	push	ax
	mov	ax,word ptr [bp+4]
	sub	ax,si
	push	ax
	call	near ptr _DrawPixel
	add	sp,6
   ;	
   ;	
   ;			DrawPixel(x0+y,y0+x,Color);
   ;	
	mov	al,byte ptr DGROUP:_Color
	push	ax
	mov	ax,word ptr [bp+6]
	add	ax,si
	push	ax
	mov	ax,word ptr [bp+4]
	add	ax,di
	push	ax
	call	near ptr _DrawPixel
	add	sp,6
   ;	
   ;			DrawPixel(x0+y,y0-x,Color);
   ;	
	mov	al,byte ptr DGROUP:_Color
	push	ax
	mov	ax,word ptr [bp+6]
	sub	ax,si
	push	ax
	mov	ax,word ptr [bp+4]
	add	ax,di
	push	ax
	call	near ptr _DrawPixel
	add	sp,6
   ;	
   ;			DrawPixel(x0-y,y0+x,Color);
   ;	
	mov	al,byte ptr DGROUP:_Color
	push	ax
	mov	ax,word ptr [bp+6]
	add	ax,si
	push	ax
	mov	ax,word ptr [bp+4]
	sub	ax,di
	push	ax
	call	near ptr _DrawPixel
	add	sp,6
   ;	
   ;			DrawPixel(x0-y,y0-x,Color);
   ;	
	mov	al,byte ptr DGROUP:_Color
	push	ax
	mov	ax,word ptr [bp+6]
	sub	ax,si
	push	ax
	mov	ax,word ptr [bp+4]
	sub	ax,di
	push	ax
	call	near ptr _DrawPixel
	add	sp,6
   ;	
   ;	
   ;			if(err<=0)
   ;	
	cmp	word ptr [bp-6],0
	jg	short @12@114
   ;	
   ;			{
   ;				y++;
   ;	
	inc	di
   ;	
   ;				err+=dy;
   ;	
	mov	ax,word ptr [bp-4]
	add	word ptr [bp-6],ax
   ;	
   ;				dy+=2;
   ;	
	add	word ptr [bp-4],2
@12@114:
   ;	
   ;			}
   ;			if(err>0)
   ;	
	cmp	word ptr [bp-6],0
	jle	short @12@170
   ;	
   ;			{
   ;				x--;
   ;	
	dec	si
   ;	
   ;				dx+=2;
   ;	
	add	word ptr [bp-2],2
   ;	
   ;				err += dx  - (r<<1);
   ;	
	mov	ax,word ptr [bp+8]
	shl	ax,1
	mov	dx,word ptr [bp-2]
	sub	dx,ax
	add	word ptr [bp-6],dx
@12@170:
	cmp	si,di
	jl	@@5
	jmp	@12@58
@@5:
   ;	
   ;			}
   ;		}
   ;	}
   ;	
	pop	di
	pop	si
	mov	sp,bp
	pop	bp
	ret	
_dcircle	endp
   ;	
   ;	unsigned PicSize(int x1,int y1,int x2,int y2)
   ;	
	assume	cs:_TEXT
_PicSize	proc	near
	push	bp
	mov	bp,sp
	push	si
	push	di
   ;	
   ;	{
   ;		int x,y;
   ;		if (x1>x2) swap(&x1,&x2);
   ;	
	mov	ax,word ptr [bp+4]
	cmp	ax,word ptr [bp+8]
	jle	short @13@86
	lea	ax,word ptr [bp+8]
	push	ax
	lea	ax,word ptr [bp+4]
	push	ax
	call	near ptr _swap
	pop	cx
	pop	cx
@13@86:
   ;	
   ;		if (y1>y2) swap(&y1,&y2);
   ;	
	mov	ax,word ptr [bp+6]
	cmp	ax,word ptr [bp+10]
	jle	short @13@142
	lea	ax,word ptr [bp+10]
	push	ax
	lea	ax,word ptr [bp+6]
	push	ax
	call	near ptr _swap
	pop	cx
	pop	cx
@13@142:
   ;	
   ;		x=x2-x1+1;
   ;	
	mov	ax,word ptr [bp+8]
	sub	ax,word ptr [bp+4]
	inc	ax
	mov	si,ax
   ;	
   ;		y=y2-y1+1;
   ;	
	mov	ax,word ptr [bp+10]
	sub	ax,word ptr [bp+6]
	inc	ax
	mov	di,ax
   ;	
   ;		if((x>320)&&(y>200))
   ;	
	cmp	si,320
	jle	short @13@254
	cmp	di,200
	jle	short @13@254
   ;	
   ;			return 0;
   ;	
	xor	ax,ax
@13@226:
	jmp	short @13@282
@13@254:
   ;	
   ;		return x*y+10;
   ;	
	mov	ax,si
	imul	di
	add	ax,10
	jmp	short @13@226
@13@282:
   ;	
   ;	}
   ;	
	pop	di
	pop	si
	pop	bp
	ret	
_PicSize	endp
   ;	
   ;	void swap(int *a,int *b)
   ;	
	assume	cs:_TEXT
_swap	proc	near
	push	bp
	mov	bp,sp
	sub	sp,2
	push	si
	push	di
	mov	si,word ptr [bp+4]
	mov	di,word ptr [bp+6]
   ;	
   ;	{
   ;		int temp;
   ;		temp=*a;
   ;	
	mov	ax,word ptr [si]
	mov	word ptr [bp-2],ax
   ;	
   ;		*a=*b;
   ;	
	mov	ax,word ptr [di]
	mov	word ptr [si],ax
   ;	
   ;		*b=temp;
   ;	
	mov	ax,word ptr [bp-2]
	mov	word ptr [di],ax
   ;	
   ;	}
   ;	
	pop	di
	pop	si
	mov	sp,bp
	pop	bp
	ret	
_swap	endp
   ;	
   ;	void GetPic(int x1,int y1,int x2,int y2,TPicture far *pic)
   ;	
	assume	cs:_TEXT
_GetPic	proc	near
	push	bp
	mov	bp,sp
	sub	sp,8
	push	si
	push	di
   ;	
   ;	{
   ;		int x,y;
   ;		int cy,cx;
   ;		char far *d;
   ;		if (x1>x2) swap(&x1,&x2);
   ;	
	mov	ax,word ptr [bp+4]
	cmp	ax,word ptr [bp+8]
	jle	short @15@86
	lea	ax,word ptr [bp+8]
	push	ax
	lea	ax,word ptr [bp+4]
	push	ax
	call	near ptr _swap
	pop	cx
	pop	cx
@15@86:
   ;	
   ;		if (y1>y2) swap(&y1,&y2);
   ;	
	mov	ax,word ptr [bp+6]
	cmp	ax,word ptr [bp+10]
	jle	short @15@142
	lea	ax,word ptr [bp+10]
	push	ax
	lea	ax,word ptr [bp+6]
	push	ax
	call	near ptr _swap
	pop	cx
	pop	cx
@15@142:
   ;	
   ;		x=x2-x1+1;
   ;	
	mov	ax,word ptr [bp+8]
	sub	ax,word ptr [bp+4]
	inc	ax
	mov	word ptr [bp-2],ax
   ;	
   ;		y=y2-y1+1;
   ;	
	mov	ax,word ptr [bp+10]
	sub	ax,word ptr [bp+6]
	inc	ax
	mov	word ptr [bp-4],ax
   ;	
   ;	
   ;		pic->w=x;
   ;	
	les	bx,dword ptr [bp+12]
	mov	ax,word ptr [bp-2]
	mov	word ptr es:[bx],ax
   ;	
   ;		pic->h=y;
   ;	
	les	bx,dword ptr [bp+12]
	mov	ax,word ptr [bp-4]
	mov	word ptr es:[bx+2],ax
   ;	
   ;	
   ;		d=pic->data;
   ;	
	mov	ax,word ptr [bp+14]
	mov	dx,word ptr [bp+12]
	add	dx,4
	mov	word ptr [bp-6],ax
	mov	word ptr [bp-8],dx
   ;	
   ;		for(cy=y1;cy<=y2;cy++)
   ;	
	mov	si,word ptr [bp+6]
	jmp	short @15@310
@15@170:
   ;	
   ;		for(cx=x1;cx<=x2;cx++)
   ;	
	mov	di,word ptr [bp+4]
	jmp	short @15@254
@15@198:
   ;	
   ;		{
   ;			*d=GETPLOT(cx,cy);
   ;	
	mov	ax,si
	mov	dx,320
	imul	dx
	mov	bx,di
	add	bx,ax
	mov	es,word ptr DGROUP:_screen+2
	add	bx,word ptr DGROUP:_screen
	mov	al,byte ptr es:[bx]
	les	bx,dword ptr [bp-8]
	mov	byte ptr es:[bx],al
   ;	
   ;			d++;
   ;	
	inc	word ptr [bp-8]
	inc	di
@15@254:
	cmp	di,word ptr [bp+8]
	jle	short @15@198
	inc	si
@15@310:
	cmp	si,word ptr [bp+10]
	jle	short @15@170
   ;	
   ;		}
   ;	}
   ;	
	pop	di
	pop	si
	mov	sp,bp
	pop	bp
	ret	
_GetPic	endp
   ;	
   ;	int XPRes(TPicture far *pic)
   ;	
	assume	cs:_TEXT
_XPRes	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;		return pic->w;
   ;	
	les	bx,dword ptr [bp+4]
	mov	ax,word ptr es:[bx]
	jmp	short @16@58
@16@58:
   ;	
   ;	}
   ;	
	pop	bp
	ret	
_XPRes	endp
   ;	
   ;	int YPRes(TPicture far *pic)
   ;	
	assume	cs:_TEXT
_YPRes	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;		return pic->h;
   ;	
	les	bx,dword ptr [bp+4]
	mov	ax,word ptr es:[bx+2]
	jmp	short @17@58
@17@58:
   ;	
   ;	}
   ;	
	pop	bp
	ret	
_YPRes	endp
   ;	
   ;	void DrawPicF(int x1,int y1,TPicture far *pic)
   ;	
	assume	cs:_TEXT
_DrawPicF	proc	near
	push	bp
	mov	bp,sp
	sub	sp,10
	push	si
	push	di
	mov	di,word ptr [bp+6]
   ;	
   ;	{
   ;		char far *data=pic->data;
   ;	
	mov	ax,word ptr [bp+10]
	mov	dx,word ptr [bp+8]
	add	dx,4
	mov	word ptr [bp-2],ax
	mov	word ptr [bp-4],dx
   ;	
   ;		int x2=pic->w;
   ;	
	les	bx,dword ptr [bp+8]
	mov	ax,word ptr es:[bx]
	mov	word ptr [bp-6],ax
   ;	
   ;		int y2=pic->h;
   ;	
	les	bx,dword ptr [bp+8]
	mov	ax,word ptr es:[bx+2]
	mov	word ptr [bp-8],ax
   ;	
   ;		int py,y;
   ;	
   ;		py=y1;
   ;	
	mov	word ptr [bp-10],di
   ;	
   ;		for(y=0;y<y2;y++)
   ;	
	xor	si,si
	jmp	short @18@114
@18@58:
   ;	
   ;		{
   ;			py=(y1+y)*320;
   ;	
	mov	ax,di
	add	ax,si
	mov	dx,320
	imul	dx
	mov	word ptr [bp-10],ax
   ;	
   ;			_fmemcpy(&screen[x1+py],data,x2);
   ;	
	push	word ptr [bp-6]
	push	word ptr [bp-2]
	push	word ptr [bp-4]
	mov	ax,word ptr [bp+4]
	add	ax,word ptr [bp-10]
	add	ax,word ptr DGROUP:_screen
	push	word ptr DGROUP:_screen+2
	push	ax
	call	near ptr __fmemcpy
	add	sp,10
   ;	
   ;			data+=x2;
   ;	
	mov	ax,word ptr [bp-6]
	add	word ptr [bp-4],ax
	inc	si
@18@114:
	cmp	si,word ptr [bp-8]
	jl	short @18@58
   ;	
   ;		}
   ;	}
   ;	
	pop	di
	pop	si
	mov	sp,bp
	pop	bp
	ret	
_DrawPicF	endp
   ;	
   ;	void DrawPic(int x1,int y1,TPicture far *pic)
   ;	
	assume	cs:_TEXT
_DrawPic	proc	near
	push	bp
	mov	bp,sp
	sub	sp,8
	push	si
	push	di
	mov	cx,word ptr [bp+4]
   ;	
   ;	{
   ;		char far *data=pic->data;
   ;	
	mov	ax,word ptr [bp+10]
	mov	dx,word ptr [bp+8]
	add	dx,4
	mov	word ptr [bp-2],ax
	mov	word ptr [bp-4],dx
   ;	
   ;		int x2=x1+pic->w-1;
   ;	
	les	bx,dword ptr [bp+8]
	mov	ax,cx
	add	ax,word ptr es:[bx]
	dec	ax
	mov	word ptr [bp-6],ax
   ;	
   ;		int y2=y1+pic->h-1;
   ;	
	les	bx,dword ptr [bp+8]
	mov	ax,word ptr [bp+6]
	add	ax,word ptr es:[bx+2]
	dec	ax
	mov	word ptr [bp-8],ax
   ;	
   ;		int x,y;
   ;		
   ;		for(y=y1;y<=y2;y++)
   ;	
	mov	di,word ptr [bp+6]
	jmp	short @19@254
@19@58:
   ;	
   ;		for(x=x1;x<=x2;x++)
   ;	
	mov	si,cx
	jmp	short @19@198
@19@86:
   ;	
   ;		{
   ;			if (*data!=0) PLOT(x,y,*data);
   ;	
	les	bx,dword ptr [bp-4]
	cmp	byte ptr es:[bx],0
	je	short @19@142
	mov	ax,di
	mov	dx,320
	imul	dx
	mov	bx,si
	add	bx,ax
	mov	es,word ptr DGROUP:_screen+2
	add	bx,word ptr DGROUP:_screen
	push	es
	mov	es,word ptr [bp-2]
	push	bx
	mov	bx,word ptr [bp-4]
	mov	al,byte ptr es:[bx]
	pop	bx
	pop	es
	mov	byte ptr es:[bx],al
@19@142:
   ;	
   ;			data++;
   ;	
	inc	word ptr [bp-4]
	inc	si
@19@198:
	cmp	si,word ptr [bp-6]
	jle	short @19@86
	inc	di
@19@254:
	cmp	di,word ptr [bp-8]
	jle	short @19@58
   ;	
   ;		}
   ;	
   ;	}
   ;	
	pop	di
	pop	si
	mov	sp,bp
	pop	bp
	ret	
_DrawPic	endp
   ;	
   ;	void DrawPicB(int x1,int y1,TPicture far *pic)
   ;	
	assume	cs:_TEXT
_DrawPicB	proc	near
	push	bp
	mov	bp,sp
	sub	sp,8
	push	si
	push	di
   ;	
   ;	{
   ;		char far *data=pic->data;
   ;	
	mov	ax,word ptr [bp+10]
	mov	dx,word ptr [bp+8]
	add	dx,4
	mov	word ptr [bp-2],ax
	mov	word ptr [bp-4],dx
   ;	
   ;		int x2=x1+pic->w-1;
   ;	
	les	bx,dword ptr [bp+8]
	mov	ax,word ptr [bp+4]
	add	ax,word ptr es:[bx]
	dec	ax
	mov	word ptr [bp-6],ax
   ;	
   ;		int y2=y1+pic->h-1;
   ;	
	les	bx,dword ptr [bp+8]
	mov	ax,word ptr [bp+6]
	add	ax,word ptr es:[bx+2]
	dec	ax
	mov	word ptr [bp-8],ax
   ;	
   ;		int x,y;
   ;	
   ;		for(y=y1;y<=y2;y++)
   ;	
	mov	di,word ptr [bp+6]
	jmp	short @20@254
@20@58:
   ;	
   ;		for(x=x1;x<=x2;x++)
   ;	
	mov	si,word ptr [bp+4]
	jmp	short @20@198
@20@86:
   ;	
   ;		{
   ;			if (*data!=-1) DrawPixel(x,y,*data);
   ;	
	les	bx,dword ptr [bp-4]
	cmp	byte ptr es:[bx],255
	je	short @20@142
	les	bx,dword ptr [bp-4]
	mov	al,byte ptr es:[bx]
	push	ax
	push	di
	push	si
	call	near ptr _DrawPixel
	add	sp,6
@20@142:
   ;	
   ;			data++;
   ;	
	inc	word ptr [bp-4]
	inc	si
@20@198:
	cmp	si,word ptr [bp-6]
	jle	short @20@86
	inc	di
@20@254:
	cmp	di,word ptr [bp-8]
	jle	short @20@58
   ;	
   ;		}
   ;	}
   ;	
	pop	di
	pop	si
	mov	sp,bp
	pop	bp
	ret	
_DrawPicB	endp
   ;	
   ;	void DefaultScreen()
   ;	
	assume	cs:_TEXT
_DefaultScreen	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;		ScrnT=DEFAULT;
   ;	
	mov	byte ptr DGROUP:_ScrnT,0
   ;	
   ;		screen=dscreen;
   ;	
	mov	ax,word ptr DGROUP:_dscreen+2
	mov	dx,word ptr DGROUP:_dscreen
	mov	word ptr DGROUP:_screen+2,ax
	mov	word ptr DGROUP:_screen,dx
   ;	
   ;	}
   ;	
	pop	bp
	ret	
_DefaultScreen	endp
   ;	
   ;	void UserScreen(unsigned char far *s)
   ;	
	assume	cs:_TEXT
_UserScreen	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;		ScrnT=USER;
   ;	
	mov	byte ptr DGROUP:_ScrnT,1
   ;	
   ;		screen=s;
   ;	
	mov	ax,word ptr [bp+6]
	mov	dx,word ptr [bp+4]
	mov	word ptr DGROUP:_screen+2,ax
	mov	word ptr DGROUP:_screen,dx
   ;	
   ;	}
   ;	
	pop	bp
	ret	
_UserScreen	endp
   ;	
   ;	char ScrnCpyUD()
   ;	
	assume	cs:_TEXT
_ScrnCpyUD	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;		if (ScrnT!=USER) return 0;
   ;	
	cmp	byte ptr DGROUP:_ScrnT,1
	je	short @23@114
	mov	al,0
@23@86:
	jmp	short @23@142
@23@114:
   ;	
   ;		_fmemcpy(dscreen,screen,64000);
   ;	
	mov	ax,64000
	push	ax
	push	word ptr DGROUP:_screen+2
	push	word ptr DGROUP:_screen
	push	word ptr DGROUP:_dscreen+2
	push	word ptr DGROUP:_dscreen
	call	near ptr __fmemcpy
	add	sp,10
   ;	
   ;		return 1;
   ;	
	mov	al,1
	jmp	short @23@86
@23@142:
   ;	
   ;	}
   ;	
	pop	bp
	ret	
_ScrnCpyUD	endp
   ;	
   ;	char ScrnCpyDU()
   ;	
	assume	cs:_TEXT
_ScrnCpyDU	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;		if (ScrnT!=USER) return 0;
   ;	
	cmp	byte ptr DGROUP:_ScrnT,1
	je	short @24@114
	mov	al,0
@24@86:
	jmp	short @24@142
@24@114:
   ;	
   ;		_fmemcpy(screen,dscreen,64000);
   ;	
	mov	ax,64000
	push	ax
	push	word ptr DGROUP:_dscreen+2
	push	word ptr DGROUP:_dscreen
	push	word ptr DGROUP:_screen+2
	push	word ptr DGROUP:_screen
	call	near ptr __fmemcpy
	add	sp,10
   ;	
   ;		return 1;
   ;	
	mov	al,1
	jmp	short @24@86
@24@142:
   ;	
   ;	}
   ;	
	pop	bp
	ret	
_ScrnCpyDU	endp
   ;	
   ;	void SetOverScan(char c)
   ;	
	assume	cs:_TEXT
_SetOverScan	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;		asm {
   ;			mov ax, 0x1001;
   ;	
	mov	 ax, 01001H
   ;	
   ;			mov bh, c;
   ;	
	mov	 bh, [bp+4]
   ;	
   ;			xor bl, bl;
   ;	
	xor	 bl, bl
   ;	
   ;			int 0x10;
   ;	
	int	 010H
   ;	
   ;		}
   ;	}
   ;	
	pop	bp
	ret	
_SetOverScan	endp
	?debug	C E9C1B2824B096D636F736170692E68
	?debug	C E9B6AB824B096D6964696C69622E63
_TEXT	ends
_DATA	segment word public 'DATA'
_SBMIDI	label	byte
	db	0
_DATA	ends
_TEXT	segment byte public 'CODE'
   ;	
   ;	void Midi_Init()
   ;	
	assume	cs:_TEXT
_Midi_Init	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;	/*	char far  *p;
   ;		int i,c;
   ;		char st[8];
   ;		for(i=0x80;i<=0xBF;i++)
   ;		{
   ;			p=(char far *)MK_FP(FP_SEG(getvect(i)),0x0000);
   ;			for(c=0;c<7;c++)
   ;			{
   ;				st[c]=p[c];
   ;			}
   ;			st[7]=0;
   ;			if(!strcmp("SBMIDI",st)) SBMIDI=i;
   ;		}
   ;		*/
   ;	}
   ;	
	pop	bp
	ret	
_Midi_Init	endp
   ;	
   ;	int Midi_GetSynth()
   ;	
	assume	cs:_TEXT
_Midi_GetSynth	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;	/*
   ;		struct REGPACK regs;
   ;		if (SBMIDI==0) return 3;
   ;		regs.r_bx=10;
   ;		intr(SBMIDI,&regs);
   ;		return regs.r_ax;
   ;	*/
   ;		return 3;
   ;	
	mov	ax,3
	jmp	short @27@58
@27@58:
   ;	
   ;	}
   ;	
	pop	bp
	ret	
_Midi_GetSynth	endp
   ;	
   ;	int Midi_Play(void far *p)
   ;	
	assume	cs:_TEXT
_Midi_Play	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;		/*
   ;		struct REGPACK regs;
   ;		if (SBMIDI==0) return 0;
   ;		regs.r_bx=4;
   ;		regs.r_dx=FP_SEG(p);
   ;		regs.r_ax=FP_OFF(p);
   ;		intr(SBMIDI,&regs);
   ;		regs.r_bx=5;
   ;		intr(SBMIDI,&regs);
   ;		if(regs.r_ax)
   ;			return 0;
   ;		else
   ;			return 1;
   ;		*/
   ;		return 0;
   ;	
	xor	ax,ax
	jmp	short @28@58
@28@58:
   ;	
   ;	}
   ;	
	pop	bp
	ret	
_Midi_Play	endp
   ;	
   ;	void Midi_Stop(void far *p)
   ;	
	assume	cs:_TEXT
_Midi_Stop	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;		/*
   ;		struct REGPACK regs;
   ;		if (SBMIDI==0) return;
   ;		regs.r_bx=4;
   ;		regs.r_dx=FP_SEG(p);
   ;		regs.r_ax=FP_OFF(p);
   ;		intr(SBMIDI,&regs);
   ;		*/
   ;	}
   ;	
	pop	bp
	ret	
_Midi_Stop	endp
   ;	
   ;	int Midi_Status()
   ;	
	assume	cs:_TEXT
_Midi_Status	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;		/*
   ;		struct REGPACK regs;
   ;		if (SBMIDI==0) return 0;
   ;		regs.r_bx=11;
   ;		intr(SBMIDI,&regs);
   ;		return regs.r_ax;
   ;		*/
   ;		return 0;
   ;	
	xor	ax,ax
	jmp	short @30@58
@30@58:
   ;	
   ;	}
   ;	
	pop	bp
	ret	
_Midi_Status	endp
   ;	
   ;	void Midi_Pause()
   ;	
	assume	cs:_TEXT
_Midi_Pause	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;		/*
   ;		struct REGPACK regs;
   ;		if (SBMIDI==0) return;
   ;		regs.r_bx=7;
   ;		intr(SBMIDI,&regs);
   ;		*/
   ;	}
   ;	
	pop	bp
	ret	
_Midi_Pause	endp
   ;	
   ;	void Midi_Resume()
   ;	
	assume	cs:_TEXT
_Midi_Resume	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;		/*
   ;		struct REGPACK regs;
   ;		if (SBMIDI==0) return;
   ;		regs.r_bx=8;
   ;		intr(SBMIDI,&regs);
   ;		*/
   ;	}
   ;	
	pop	bp
	ret	
_Midi_Resume	endp
	?debug	C E9C1B2824B096D636F736170692E68
_TEXT	ends
_DATA	segment word public 'DATA'
_LPFg	label	dword
	db	0
	db	0
	db	0
	db	0
_MusicLen	label	word
	db	0
	db	0
_LoadError	label	byte
	db	0
_vcm	label	word
	db	232
	db	3
_GXI	label	word
	db	15
	db	0
_GYI	label	word
	db	7
	db	0
_NG	label	word
	db	4
	db	0
_stl	label	word
	db	1
	db	0
_frames	label	word
	db	70
	db	0
_som	label	word
	db	0
	db	0
_musica	label	word
	db	0
	db	0
_pl	label	word
	db	0
	db	0
_nivel	label	word
	db	232
	db	3
	db	2
	db	0
	db	53
	db	0
	db	0
	db	0
	db	132
	db	3
	db	4
	db	0
	db	53
	db	0
	db	0
	db	0
	db	32
	db	3
	db	4
	db	0
	db	53
	db	0
	db	53
	db	0
	db	32
	db	3
	db	4
	db	0
	db	53
	db	0
	db	43
	db	0
	db	88
	db	2
	db	4
	db	0
	db	53
	db	0
	db	33
	db	0
	db	88
	db	2
	db	4
	db	0
	db	53
	db	0
	db	23
	db	0
	db	244
	db	1
	db	4
	db	0
	db	53
	db	0
	db	17
	db	0
	db	144
	db	1
	db	4
	db	0
	db	53
	db	0
	db	13
	db	0
	db	94
	db	1
	db	4
	db	0
	db	53
	db	0
	db	9
	db	0
	db	94
	db	1
	db	4
	db	0
	db	53
	db	0
	db	5
	db	0
_VDir	label	byte
	db	1
	db	2
	db	4
	db	3
_DATA	ends
_BSS	segment word public 'BSS'
	db	2 dup (?)
	db	2 dup (?)
_BSS	ends
_TEXT	segment byte public 'CODE'
   ;	
   ;	unsigned mrandom(unsigned mrandom)
   ;	
	assume	cs:_TEXT
_mrandom	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;		static x;
   ;		static init;
   ;		if(!init) {
   ;	
	cmp	word ptr DGROUP:b@w+2,0
	jne	short @33@86
   ;	
   ;			x=0x5124;
   ;	
	mov	word ptr DGROUP:b@w+0,20772
   ;	
   ;			init=1;
   ;	
	mov	word ptr DGROUP:b@w+2,1
@33@86:
   ;	
   ;		}
   ;	
   ;		x ^= x << 7;
   ;	
	mov	ax,word ptr DGROUP:b@w+0
	mov	cl,7
	shl	ax,cl
	xor	word ptr DGROUP:b@w+0,ax
   ;	
   ;		x ^= x >> 11;
   ;	
	mov	ax,word ptr DGROUP:b@w+0
	mov	cl,11
	sar	ax,cl
	xor	word ptr DGROUP:b@w+0,ax
   ;	
   ;		x ^= x << 3;
   ;	
	mov	ax,word ptr DGROUP:b@w+0
	shl	ax,1
	shl	ax,1
	shl	ax,1
	xor	word ptr DGROUP:b@w+0,ax
   ;	
   ;	
   ;		return x%mrandom;
   ;	
	mov	ax,word ptr DGROUP:b@w+0
	xor	dx,dx
	div	word ptr [bp+4]
	mov	ax,dx
	jmp	short @33@114
@33@114:
   ;	
   ;	}
   ;	
	pop	bp
	ret	
_mrandom	endp
_TEXT	ends
_DATA	segment word public 'DATA'
_palette	label	dword
	db	0
	db	0
	db	0
	db	0
_DATA	ends
_TEXT	segment byte public 'CODE'
   ;	
   ;	void setpali(char i, char r, char g, char b)
   ;	
	assume	cs:_TEXT
_setpali	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;			asm {
   ;				mov dx, 0x3c8;
   ;	
	mov	 dx, 03c8H
   ;	
   ;				mov al, i; out dx, al;
   ;	
	mov	 al, [bp+4]
 	out	 dx, al
   ;	
   ;				mov dx, 0x3c9;
   ;	
	mov	 dx, 03c9H
   ;	
   ;				mov al, r; out dx, al;
   ;	
	mov	 al, [bp+6]
 	out	 dx, al
   ;	
   ;				mov al, g; out dx, al;
   ;	
	mov	 al, [bp+8]
 	out	 dx, al
   ;	
   ;				mov al, b; out dx, al;
   ;	
	mov	 al, [bp+10]
 	out	 dx, al
   ;	
   ;			}
   ;	}
   ;	
	pop	bp
	ret	
_setpali	endp
   ;	
   ;	void setpal(Trgb far *pal)
   ;	
	assume	cs:_TEXT
_setpal	proc	near
	push	bp
	mov	bp,sp
	sub	sp,2
   ;	
   ;	{
   ;		int i;
   ;		for(i=0;i<256;i++)
   ;	
	mov	word ptr [bp-2],0
	jmp	short @35@114
@35@58:
   ;	
   ;		{
   ;			setpali(i, pal[i].R/(256/64), pal[i].G/(256/64), pal[i].B/(256/64));
   ;	
	mov	ax,word ptr [bp-2]
	shl	ax,1
	shl	ax,1
	les	bx,dword ptr [bp+4]
	add	bx,ax
	mov	al,byte ptr es:[bx]
	mov	ah,0
	mov	bx,4
	cwd	
	idiv	bx
	push	ax
	mov	ax,word ptr [bp-2]
	shl	ax,1
	shl	ax,1
	les	bx,dword ptr [bp+4]
	add	bx,ax
	mov	al,byte ptr es:[bx+1]
	mov	ah,0
	mov	bx,4
	cwd	
	idiv	bx
	push	ax
	mov	ax,word ptr [bp-2]
	shl	ax,1
	shl	ax,1
	les	bx,dword ptr [bp+4]
	add	bx,ax
	mov	al,byte ptr es:[bx+2]
	mov	ah,0
	mov	bx,4
	cwd	
	idiv	bx
	push	ax
	mov	al,byte ptr [bp-2]
	push	ax
	call	near ptr _setpali
	add	sp,8
	inc	word ptr [bp-2]
@35@114:
	cmp	word ptr [bp-2],256
	jl	short @35@58
   ;	
   ;		}
   ;	}
   ;	
	mov	sp,bp
	pop	bp
	ret	
_setpal	endp
   ;	
   ;	int loadResourceFile(const char *filename, TResourceMap *rmap, int max)
   ;	
	assume	cs:_TEXT
_loadResourceFile	proc	near
	push	bp
	mov	bp,sp
	sub	sp,20
	push	si
	push	di
	mov	di,word ptr [bp+6]
   ;	
   ;	{
   ;		int handle;
   ;		int n=0;
   ;	
	xor	si,si
   ;	
   ;		unsigned size=0;
   ;	
	mov	word ptr [bp-4],0
   ;	
   ;		char buffer[12];
   ;		TResourceHeader rh;
   ;		int ok=1;
   ;	
	mov	word ptr [bp-6],1
   ;	
   ;		if( (handle=mcos_open(filename,READMODE)) == -1 ) {
   ;	
	xor	ax,ax
	push	ax
	push	word ptr [bp+4]
	call	near ptr _mcos_open
	pop	cx
	pop	cx
	mov	word ptr [bp-2],ax
	cmp	ax,65535
	jne	short @36@114
   ;	
   ;			printf("Cannot open resource file %s\n", filename);
   ;	
	push	word ptr [bp+4]
	mov	ax,offset DGROUP:s@
	push	ax
	call	near ptr _printf
	pop	cx
	pop	cx
   ;	
   ;			return 0;
   ;	
	xor	ax,ax
@36@86:
	jmp	@36@562
@36@114:
	jmp	@36@506
@36@142:
   ;	
   ;		}
   ;	
   ;		while(ok)
   ;		{
   ;			if(!mcos_read(handle,&rh,sizeof(rh)))
   ;	
	mov	ax,14
	push	ax
	push	ss
	lea	ax,word ptr [bp-20]
	push	ax
	push	word ptr [bp-2]
	call	near ptr _mcos_read
	add	sp,8
	or	ax,ax
	jne	short @36@198
   ;	
   ;				break;
   ;	
	jmp	@36@534
@36@198:
   ;	
   ;			size=rh.size[0]+rh.size[1]*256;
   ;	
	mov	al,byte ptr [bp-8]
	mov	ah,0
	mov	dl,byte ptr [bp-7]
	mov	dh,0
	mov	cl,8
	shl	dx,cl
	add	ax,dx
	mov	word ptr [bp-4],ax
   ;	
   ;			strcpy(rmap[n].id, rh.id);
   ;	
	lea	ax,word ptr [bp-20]
	push	ax
	mov	ax,si
	mov	dx,17
	imul	dx
	mov	dx,di
	add	dx,ax
	push	dx
	call	near ptr _strcpy
	pop	cx
	pop	cx
   ;	
   ;			rmap[n].size=size;
   ;	
	mov	ax,si
	mov	dx,17
	imul	dx
	mov	dx,word ptr [bp-4]
	mov	bx,ax
	mov	word ptr [bx+di+11],dx
   ;	
   ;			rmap[n].ptr=0L;
   ;	
	mov	ax,si
	mov	dx,17
	imul	dx
	mov	bx,ax
	mov	word ptr [bx+di+15],0
	mov	word ptr [bx+di+13],0
   ;	
   ;			//printf("Resource id %s, size %u\n", rh.id, size);
   ;			
   ;			if(size) {
   ;	
	cmp	word ptr [bp-4],0
	je	short @36@338
   ;	
   ;				rmap[n].ptr = farmalloc(size);
   ;	
	push	word ptr [bp-4]
	call	near ptr _farmalloc
	pop	cx
	push	ax
	mov	ax,si
	mov	bx,17
	push	dx
	imul	bx
	mov	bx,ax
	pop	ax
	mov	word ptr [bx+di+15],ax
	pop	ax
	mov	word ptr [bx+di+13],ax
   ;	
   ;				if(!rmap[n].ptr) {
   ;	
	mov	ax,si
	mov	dx,17
	imul	dx
	mov	bx,ax
	mov	ax,word ptr [bx+di+13]
	or	ax,word ptr [bx+di+15]
	jne	short @36@282
   ;	
   ;					ok=0;
   ;	
	mov	word ptr [bp-6],0
   ;	
   ;					printf("Out of memory %u\r\n",size);
   ;	
	push	word ptr [bp-4]
	mov	ax,offset DGROUP:s@+30
	push	ax
	call	near ptr _printf
	pop	cx
	pop	cx
   ;	
   ;					break;
   ;	
	jmp	short @36@534
@36@282:
   ;	
   ;				}
   ;				if(!mcos_read(handle,rmap[n].ptr,size))
   ;	
	push	word ptr [bp-4]
	mov	ax,si
	mov	dx,17
	imul	dx
	mov	bx,ax
	push	word ptr [bx+di+15]
	push	word ptr [bx+di+13]
	push	word ptr [bp-2]
	call	near ptr _mcos_read
	add	sp,8
	or	ax,ax
	jne	short @36@338
   ;	
   ;					break;
   ;	
	jmp	short @36@534
@36@338:
   ;	
   ;			}
   ;			n++;
   ;	
	inc	si
   ;	
   ;			
   ;			if(rh.id[0]=='R' && rh.id[1]=='E' && rh.id[2]=='$') {
   ;	
	cmp	byte ptr [bp-20],82
	jne	short @36@450
	cmp	byte ptr [bp-19],69
	jne	short @36@450
	cmp	byte ptr [bp-18],36
	jne	short @36@450
   ;	
   ;				break;
   ;	
	jmp	short @36@534
@36@450:
   ;	
   ;			}
   ;			
   ;			if(n>=max)
   ;	
	cmp	si,word ptr [bp+8]
	jl	short @36@506
   ;	
   ;				break;
   ;	
	jmp	short @36@534
@36@506:
	cmp	word ptr [bp-6],0
	je	@@6
	jmp	@36@142
@@6:
@36@534:
   ;	
   ;		}
   ;		
   ;		mcos_close(handle);
   ;	
	push	word ptr [bp-2]
	call	near ptr _mcos_close
	pop	cx
   ;	
   ;		return n;
   ;	
	mov	ax,si
	jmp	@36@86
@36@562:
   ;	
   ;	}
   ;	
	pop	di
	pop	si
	mov	sp,bp
	pop	bp
	ret	
_loadResourceFile	endp
   ;	
   ;	TResourceMap *getResource(const char *id, TResourceMap *rmap, int max)
   ;	
	assume	cs:_TEXT
_getResource	proc	near
	push	bp
	mov	bp,sp
	push	si
	push	di
	mov	di,word ptr [bp+6]
   ;	
   ;	{
   ;		unsigned i;
   ;		for(i=0;i<max;i++)
   ;	
	xor	si,si
	jmp	short @37@170
@37@58:
   ;	
   ;		{
   ;			if(!strcmp(rmap[i].id, id))
   ;	
	push	word ptr [bp+4]
	mov	ax,si
	mov	dx,17
	imul	dx
	mov	dx,di
	add	dx,ax
	push	dx
	call	near ptr _strcmp
	pop	cx
	pop	cx
	or	ax,ax
	jne	short @37@142
   ;	
   ;				return &rmap[i];
   ;	
	mov	ax,si
	mov	dx,17
	imul	dx
	push	ax
	mov	ax,di
	pop	dx
	add	ax,dx
@37@114:
	jmp	short @37@226
@37@142:
	inc	si
@37@170:
	cmp	si,word ptr [bp+8]
	jb	short @37@58
   ;	
   ;		}
   ;		return 0;
   ;	
	xor	ax,ax
	jmp	short @37@114
@37@226:
   ;	
   ;	}
   ;	
	pop	di
	pop	si
	pop	bp
	ret	
_getResource	endp
_TEXT	ends
_BSS	segment word public 'BSS'
	db	2 dup (?)
	db	2 dup (?)
	db	2 dup (?)
_BSS	ends
_TEXT	segment byte public 'CODE'
   ;	
   ;	void DrawNextPic(TPicture far *ptr)
   ;	
	assume	cs:_TEXT
_DrawNextPic	proc	near
	push	bp
	mov	bp,sp
	push	si
	push	di
   ;	
   ;	{
   ;		unsigned w,h;
   ;		static curx;
   ;		static cury;
   ;		static maxh;
   ;	
   ;		if(!ptr)
   ;	
	mov	ax,word ptr [bp+4]
	or	ax,word ptr [bp+6]
	jne	short @38@86
   ;	
   ;			return;
   ;	
	jmp	short @38@226
@38@86:
   ;	
   ;		w=ptr->w;
   ;	
	les	bx,dword ptr [bp+4]
	mov	si,word ptr es:[bx]
   ;	
   ;		h=ptr->h;
   ;	
	les	bx,dword ptr [bp+4]
	mov	di,word ptr es:[bx+2]
   ;	
   ;		if(curx+w>319)
   ;	
	mov	ax,word ptr DGROUP:b@w+4
	add	ax,si
	cmp	ax,319
	jbe	short @38@142
   ;	
   ;		{
   ;			curx=0;
   ;	
	mov	word ptr DGROUP:b@w+4,0
   ;	
   ;			cury+=maxh;
   ;	
	mov	ax,word ptr DGROUP:b@w+8
	add	word ptr DGROUP:b@w+6,ax
   ;	
   ;			maxh=0;
   ;	
	mov	word ptr DGROUP:b@w+8,0
@38@142:
   ;	
   ;		}
   ;		if(h>maxh)
   ;	
	cmp	di,word ptr DGROUP:b@w+8
	jbe	short @38@198
   ;	
   ;			maxh=h;
   ;	
	mov	word ptr DGROUP:b@w+8,di
@38@198:
   ;	
   ;		DrawPic(curx,cury,ptr);
   ;	
	push	word ptr [bp+6]
	push	word ptr [bp+4]
	push	word ptr DGROUP:b@w+6
	push	word ptr DGROUP:b@w+4
	call	near ptr _DrawPic
	add	sp,8
   ;	
   ;		curx+=w;
   ;	
	mov	ax,word ptr DGROUP:b@w+4
	add	ax,si
	mov	word ptr DGROUP:b@w+4,ax
@38@226:
   ;	
   ;	}
   ;	
	pop	di
	pop	si
	pop	bp
	ret	
_DrawNextPic	endp
   ;	
   ;	void drawpics()
   ;	
	assume	cs:_TEXT
_drawpics	proc	near
	push	bp
	mov	bp,sp
	sub	sp,12
	push	si
	push	di
   ;	
   ;	{
   ;		TResourceMap *rs;
   ;		char buffer[12];
   ;		int i;
   ;	
   ;		if(!(rs=getResource("Paleta",rmap,MAXRESOURCES))) {
   ;	
	mov	ax,50
	push	ax
	mov	ax,offset DGROUP:_rmap
	push	ax
	mov	ax,offset DGROUP:s@+49
	push	ax
	call	near ptr _getResource
	add	sp,6
	mov	si,ax
	or	ax,ax
	jne	short @39@114
   ;	
   ;			printf("Resource not found Paleta\n");
   ;	
	mov	ax,offset DGROUP:s@+56
	push	ax
	call	near ptr _printf
	pop	cx
@39@86:
   ;	
   ;			LoadError=1;
   ;	
	mov	byte ptr DGROUP:_LoadError,1
   ;	
   ;			return;
   ;	
	jmp	@39@898
@39@114:
   ;	
   ;		}
   ;		
   ;		if(rs->size!=256*sizeof(Trgb)) {
   ;	
	cmp	word ptr [si+11],1024
	je	short @39@170
   ;	
   ;			printf("Wrong palette data size expected %u got %u\n",256*sizeof(Trgb), rs->size);
   ;	
	push	word ptr [si+11]
	mov	ax,1024
	push	ax
	mov	ax,offset DGROUP:s@+83
	push	ax
	call	near ptr _printf
	add	sp,6
   ;	
   ;			return;
   ;	
	jmp	@39@898
@39@170:
   ;	
   ;		}
   ;	
   ;		palette=rs->ptr;	
   ;	
	mov	ax,word ptr [si+15]
	mov	dx,word ptr [si+13]
	mov	word ptr DGROUP:_palette+2,ax
	mov	word ptr DGROUP:_palette,dx
   ;	
   ;		if(palette)
   ;	
	mov	ax,word ptr DGROUP:_palette
	or	ax,word ptr DGROUP:_palette+2
	je	short @39@226
   ;	
   ;			setpal(palette);
   ;	
	push	word ptr DGROUP:_palette+2
	push	word ptr DGROUP:_palette
	call	near ptr _setpal
	pop	cx
	pop	cx
@39@226:
   ;	
   ;	
   ;		
   ;		if(!(rs=getResource("BonusPic00",rmap,MAXRESOURCES))) {
   ;	
	mov	ax,50
	push	ax
	mov	ax,offset DGROUP:_rmap
	push	ax
	mov	ax,offset DGROUP:s@+127
	push	ax
	call	near ptr _getResource
	add	sp,6
	mov	si,ax
	or	ax,ax
	jne	short @39@310
   ;	
   ;			printf("Resource not found BonusPic00\n");
   ;	
	mov	ax,offset DGROUP:s@+138
	push	ax
	call	near ptr _printf
	pop	cx
   ;	
   ;			//LoadError=1;
   ;			return;
   ;	
	jmp	@39@898
   ;	
   ;		} else 
   ;	
	jmp	short @39@366
@39@310:
   ;	
   ;		if(rs->ptr) {
   ;	
	mov	ax,word ptr [si+13]
	or	ax,word ptr [si+15]
	je	short @39@366
   ;	
   ;			DrawNextPic(rs->ptr);
   ;	
	push	word ptr [si+15]
	push	word ptr [si+13]
	call	near ptr _DrawNextPic
	pop	cx
	pop	cx
@39@366:
   ;	
   ;		}
   ;		
   ;		for(i=0;i<12;i++)
   ;	
	xor	di,di
	jmp	short @39@506
@39@394:
   ;	
   ;		{
   ;			strcpy(buffer,"PacPic00");
   ;	
	mov	ax,offset DGROUP:s@+169
	push	ax
	lea	ax,word ptr [bp-12]
	push	ax
	call	near ptr _strcpy
	pop	cx
	pop	cx
   ;	
   ;			buffer[6]=(i/10)%10+'0';
   ;	
	mov	ax,di
	mov	bx,10
	cwd	
	idiv	bx
	mov	bx,10
	cwd	
	idiv	bx
	add	dl,48
	mov	byte ptr [bp-6],dl
   ;	
   ;			buffer[7]=i%10+'0';
   ;	
	mov	ax,di
	mov	bx,10
	cwd	
	idiv	bx
	add	dl,48
	mov	byte ptr [bp-5],dl
   ;	
   ;			if(!(rs=getResource(buffer,rmap,MAXRESOURCES))) {
   ;	
	mov	ax,50
	push	ax
	mov	ax,offset DGROUP:_rmap
	push	ax
	lea	ax,word ptr [bp-12]
	push	ax
	call	near ptr _getResource
	add	sp,6
	mov	si,ax
	or	ax,ax
	jne	short @39@450
   ;	
   ;				printf("Resource not found %s\n", buffer);
   ;	
	lea	ax,word ptr [bp-12]
	push	ax
	mov	ax,offset DGROUP:s@+178
	jmp	@39@786
@39@450:
   ;	
   ;				LoadError=1;
   ;				return;
   ;			}
   ;			DrawNextPic(rs->ptr);
   ;	
	push	word ptr [si+15]
	push	word ptr [si+13]
	call	near ptr _DrawNextPic
	pop	cx
	pop	cx
	inc	di
@39@506:
	cmp	di,12
	jl	short @39@394
   ;	
   ;		}
   ;	
   ;		for(i=0;i<6;i++)
   ;	
	xor	di,di
	jmp	short @39@674
@39@562:
   ;	
   ;		{
   ;			strcpy(buffer,"GhostPic00");
   ;	
	mov	ax,offset DGROUP:s@+201
	push	ax
	lea	ax,word ptr [bp-12]
	push	ax
	call	near ptr _strcpy
	pop	cx
	pop	cx
   ;	
   ;			buffer[8]=(i/10)%10+'0';
   ;	
	mov	ax,di
	mov	bx,10
	cwd	
	idiv	bx
	mov	bx,10
	cwd	
	idiv	bx
	add	dl,48
	mov	byte ptr [bp-4],dl
   ;	
   ;			buffer[9]=i%10+'0';
   ;	
	mov	ax,di
	mov	bx,10
	cwd	
	idiv	bx
	add	dl,48
	mov	byte ptr [bp-3],dl
   ;	
   ;	
   ;			if(!(rs=getResource(buffer,rmap,MAXRESOURCES))) {
   ;	
	mov	ax,50
	push	ax
	mov	ax,offset DGROUP:_rmap
	push	ax
	lea	ax,word ptr [bp-12]
	push	ax
	call	near ptr _getResource
	add	sp,6
	mov	si,ax
	or	ax,ax
	jne	short @39@618
   ;	
   ;				printf("Resource not found %s\n", buffer);
   ;	
	lea	ax,word ptr [bp-12]
	push	ax
	mov	ax,offset DGROUP:s@+212
	jmp	short @39@786
@39@618:
   ;	
   ;				LoadError=1;
   ;				return;
   ;			}
   ;			DrawNextPic(rs->ptr);
   ;	
	push	word ptr [si+15]
	push	word ptr [si+13]
	call	near ptr _DrawNextPic
	pop	cx
	pop	cx
	inc	di
@39@674:
	cmp	di,6
	jl	short @39@562
   ;	
   ;		}
   ;		
   ;		for(i=0;i<21;i++)
   ;	
	xor	di,di
	jmp	short @39@870
@39@730:
   ;	
   ;		{
   ;			strcpy(buffer,"WallPic00");
   ;	
	mov	ax,offset DGROUP:s@+235
	push	ax
	lea	ax,word ptr [bp-12]
	push	ax
	call	near ptr _strcpy
	pop	cx
	pop	cx
   ;	
   ;			buffer[7]=(i/10)%10+'0';
   ;	
	mov	ax,di
	mov	bx,10
	cwd	
	idiv	bx
	mov	bx,10
	cwd	
	idiv	bx
	add	dl,48
	mov	byte ptr [bp-5],dl
   ;	
   ;			buffer[8]=i%10+'0';
   ;	
	mov	ax,di
	mov	bx,10
	cwd	
	idiv	bx
	add	dl,48
	mov	byte ptr [bp-4],dl
   ;	
   ;	
   ;			if(!(rs=getResource(buffer,rmap,MAXRESOURCES))) {
   ;	
	mov	ax,50
	push	ax
	mov	ax,offset DGROUP:_rmap
	push	ax
	lea	ax,word ptr [bp-12]
	push	ax
	call	near ptr _getResource
	add	sp,6
	mov	si,ax
	or	ax,ax
	jne	short @39@814
   ;	
   ;				printf("Resource not found %s\n", buffer);
   ;	
	lea	ax,word ptr [bp-12]
	push	ax
	mov	ax,offset DGROUP:s@+245
@39@786:
	push	ax
	call	near ptr _printf
	pop	cx
	pop	cx
	jmp	@39@86
@39@814:
   ;	
   ;				LoadError=1;
   ;				return;
   ;			}
   ;			DrawNextPic(rs->ptr);
   ;	
	push	word ptr [si+15]
	push	word ptr [si+13]
	call	near ptr _DrawNextPic
	pop	cx
	pop	cx
	inc	di
@39@870:
	cmp	di,21
	jl	short @39@730
@39@898:
   ;	
   ;		}
   ;	
   ;	}
   ;	
	pop	di
	pop	si
	mov	sp,bp
	pop	bp
	ret	
_drawpics	endp
   ;	
   ;	int main()
   ;	
	assume	cs:_TEXT
_main	proc	near
	push	bp
	mov	bp,sp
	sub	sp,2
   ;	
   ;	{
   ;		BYTE op;
   ;		SM.m=0;
   ;	
	mov	byte ptr DGROUP:_SM,0
   ;	
   ;		#ifdef USE_RATO
   ;		if(CheckRato())
   ;		{
   ;			SM.m=1;
   ;			SetRato(100,100);
   ;			GetRato(&SM.x,&SM.y,&SM.b);
   ;		}
   ;		#endif
   ;		for(op=0;op<4;op++)
   ;	
	mov	byte ptr [bp-1],0
	jmp	short @40@114
@40@58:
   ;	
   ;			Ghost[op].m.t=GHOST;
   ;	
	mov	al,byte ptr [bp-1]
	mov	ah,0
	mov	cl,4
	shl	ax,cl
	mov	bx,ax
	mov	byte ptr DGROUP:_Ghost[bx+9],1
	inc	byte ptr [bp-1]
@40@114:
	cmp	byte ptr [bp-1],4
	jb	short @40@58
   ;	
   ;		som=1;
   ;	
	mov	word ptr DGROUP:_som,1
   ;	
   ;		Pac.m.t=PAC;
   ;	
	mov	byte ptr DGROUP:_Pac+9,0
   ;	
   ;		PopUp=0;
   ;	
	mov	word ptr DGROUP:_PopUp,0
   ;	
   ;		LoadAll();
   ;	
	call	near ptr _LoadAll
   ;	
   ;		Midi_Init();
   ;	
	call	near ptr _Midi_Init
   ;	
   ;		if(!SBMIDI)
   ;	
	mov	al,byte ptr DGROUP:_SBMIDI
	mov	ah,0
	or	ax,ax
	jne	short @40@198
   ;	
   ;			musica=0;
   ;	
	mov	word ptr DGROUP:_musica,0
	jmp	short @40@226
@40@198:
   ;	
   ;		else
   ;			musica=2;
   ;	
	mov	word ptr DGROUP:_musica,2
@40@226:
   ;	
   ;		modo13h();
   ;	
	call	near ptr _modo13h
   ;	
   ;		setpal(palette);
   ;	
	push	word ptr DGROUP:_palette+2
	push	word ptr DGROUP:_palette
	call	near ptr _setpal
	pop	cx
	pop	cx
   ;	
   ;		UserScreen(VirScr);
   ;	
	push	word ptr DGROUP:_VirScr+2
	push	word ptr DGROUP:_VirScr
	call	near ptr _UserScreen
	pop	cx
	pop	cx
@40@254:
   ;	
   ;	
   ;		do{
   ;			op=Menu();
   ;	
	call	near ptr _Menu
	mov	byte ptr [bp-1],al
   ;	
   ;			switch(op)
   ;	
	mov	al,byte ptr [bp-1]
	mov	ah,0
	mov	bx,ax
	cmp	bx,3
	ja	short @40@478
	shl	bx,1
	jmp	word ptr cs:@40@C594[bx]
@40@366:
   ;	
   ;			{
   ;				case 0:Play_game();break;
   ;	
	call	near ptr _Play_game
	jmp	short @40@478
@40@394:
   ;	
   ;				case 1:SetupMenu();break;
   ;	
	call	near ptr _SetupMenu
	jmp	short @40@478
@40@422:
   ;	
   ;				case 2:Ajuda();break;
   ;	
	call	near ptr _Ajuda
	jmp	short @40@478
@40@450:
   ;	
   ;				case 3:Sobre();break;
   ;	
	call	near ptr _Sobre
	jmp	short @40@478
@40@478:
   ;	
   ;			}
   ;		}while(op!=4);
   ;	
	cmp	byte ptr [bp-1],4
	jne	short @40@254
   ;	
   ;		
   ;		modo3h();
   ;	
	call	near ptr _modo3h
   ;	
   ;		UnloadAll();
   ;	
	call	near ptr _UnloadAll
   ;	
   ;		return 0;
   ;	
	xor	ax,ax
	jmp	short @40@534
@40@534:
   ;	
   ;	}
   ;	
	mov	sp,bp
	pop	bp
	ret	
_main	endp
@40@C594	label	word
	dw	@40@366
	dw	@40@394
	dw	@40@422
	dw	@40@450
   ;	
   ;	void Play_game()
   ;	
	assume	cs:_TEXT
_Play_game	proc	near
	push	bp
	mov	bp,sp
	sub	sp,12
	push	si
	push	di
   ;	
   ;	{
   ;	
   ;		char done=0;
   ;	
	mov	byte ptr [bp-1],0
   ;	
   ;		int set;
   ;		int frcnt=0;
   ;	
	xor	di,di
   ;	
   ;	
   ;		unsigned hsecs;
   ;		unsigned oldclk;
   ;		void far * far *intrtab=0x0;
   ;	
	mov	word ptr [bp-8],0
	mov	word ptr [bp-10],0
   ;	
   ;	
   ;		setpal(palette);
   ;	
	push	word ptr DGROUP:_palette+2
	push	word ptr DGROUP:_palette
	call	near ptr _setpal
	pop	cx
	pop	cx
   ;	
   ;		GameInit();
   ;	
	call	near ptr _GameInit
   ;	
   ;		
   ;		oldkeyb=intrtab[0x9];
   ;	
	les	bx,dword ptr [bp-10]
	mov	ax,word ptr es:[bx+38]
	mov	dx,word ptr es:[bx+36]
	mov	word ptr DGROUP:_oldkeyb+2,ax
	mov	word ptr DGROUP:_oldkeyb,dx
   ;	
   ;		intrtab[0x9]=MK_FP(_CS,newkeyb);
   ;	
	les	bx,dword ptr [bp-10]
	mov	word ptr es:[bx+38],cs
	mov	word ptr es:[bx+36],offset _newkeyb
   ;	
   ;		
   ;		printf("%X %X\n", FP_SEG(oldkeyb), FP_OFF(oldkeyb));
   ;	
	push	word ptr DGROUP:_oldkeyb
	push	word ptr DGROUP:_oldkeyb+2
	mov	ax,offset DGROUP:s@+268
	push	ax
	call	near ptr _printf
	add	sp,6
   ;	
   ;		printf("%X %X\n", FP_SEG(newkeyb2), FP_OFF(newkeyb2));
   ;	
	mov	ax,offset _newkeyb2
	push	ax
	push	cs
	mov	ax,offset DGROUP:s@+275
	push	ax
	call	near ptr _printf
	add	sp,6
   ;	
   ;		getch();
   ;	
	call	near ptr _getch
   ;	
   ;	/*	oldkeyb=GetIntVect(0x9);
   ;		SetIntVect(0x9,newkeyb);
   ;	*/
   ;		if(musica==2) Midi_Play(MusSeg);
   ;	
	cmp	word ptr DGROUP:_musica,2
	jne	short @41@86
	push	word ptr DGROUP:_MusSeg+2
	push	word ptr DGROUP:_MusSeg
	call	near ptr _Midi_Play
	pop	cx
	pop	cx
@41@86:
   ;	
   ;		UpDateScreen();
   ;	
	call	near ptr _UpDateScreen
   ;	
   ;	
   ;		oldclk=gethseconds();
   ;	
	call	near ptr _gethseconds
	mov	word ptr [bp-6],ax
	jmp	@41@2102
@41@114:
   ;	
   ;	
   ;		while(!done)
   ;		{
   ;			set=10;
   ;	
	mov	si,10
   ;	
   ;			if((musica==2)&&(Midi_Status()==0)) Midi_Play(MusSeg);
   ;	
	cmp	word ptr DGROUP:_musica,2
	jne	short @41@198
	call	near ptr _Midi_Status
	or	ax,ax
	jne	short @41@198
	push	word ptr DGROUP:_MusSeg+2
	push	word ptr DGROUP:_MusSeg
	call	near ptr _Midi_Play
	pop	cx
	pop	cx
@41@198:
   ;	
   ;			Game.count++;
   ;	
	inc	word ptr DGROUP:_Game+2
   ;	
   ;	
   ;	
   ;			switch(Game.st)
   ;	
	mov	ax,word ptr DGROUP:_Game
	cmp	ax,1
	je	short @41@338
	cmp	ax,2
	je	short @41@422
	jmp	short @41@450
@41@338:
   ;	
   ;			{
   ;				case PLAY:
   ;					Game.count2++;
   ;	
	inc	word ptr DGROUP:_Game+4
   ;	
   ;					GameControl();
   ;	
	call	near ptr _GameControl
   ;	
   ;					Colides();
   ;	
	call	near ptr _Colides
   ;	
   ;					if(Player[Game.plr].PF_dots==0)
   ;	
	mov	ax,word ptr DGROUP:_Game+9
	mov	dx,14
	imul	dx
	mov	bx,ax
	cmp	word ptr DGROUP:_Player[bx+7],0
	jne	short @41@394
   ;	
   ;						NextLevel();
   ;	
	call	near ptr _NextLevel
@41@394:
   ;	
   ;					AvaliaJoyStick();
   ;	
	call	near ptr _AvaliaJoyStick
   ;	
   ;					break;
   ;	
	jmp	short @41@450
@41@422:
   ;	
   ;				case ANI:
   ;					Kill();
   ;	
	call	near ptr _Kill
   ;	
   ;					break;
   ;	
	jmp	short @41@450
@41@450:
   ;	
   ;	
   ;			}
   ;	
   ;			if(kbhit())
   ;	
	call	near ptr _kbhit
	or	ax,ax
	jne	@@7
	jmp	@41@1458
@@7:
   ;	
   ;			{
   ;				switch(getch())
   ;	
	call	near ptr _getch
	or	ax,ax
	je	short @41@870
	cmp	ax,13
	jne	@@8
	jmp	@41@1374
@@8:
	cmp	ax,27
	je	short @41@618
	jmp	@41@1458
@41@618:
   ;	
   ;				{
   ;					case 27:switch(Game.st)
   ;	
	mov	ax,word ptr DGROUP:_Game
	or	ax,ax
	je	short @41@786
	cmp	ax,1
	je	short @41@786
	cmp	ax,4
	je	short @41@758
	jmp	short @41@814
@41@758:
   ;	
   ;								{
   ;									case PAUSA:set=ControlPopUp(3);break;
   ;	
	mov	ax,3
	push	ax
	call	near ptr _ControlPopUp
	pop	cx
	cbw	
	mov	si,ax
	jmp	short @41@842
@41@786:
   ;	
   ;									case WAIT:
   ;									case PLAY:Game.st=PAUSA;PopUp=0;break;
   ;	
	mov	word ptr DGROUP:_Game,4
	mov	word ptr DGROUP:_PopUp,0
	jmp	short @41@842
@41@814:
   ;	
   ;									default:set=3;break;
   ;	
	mov	si,3
	jmp	short @41@842
@41@842:
   ;	
   ;								}
   ;								break;
   ;	
	jmp	short @41@1458
@41@870:
   ;	
   ;					case 0:switch(getch())
   ;	
	call	near ptr _getch
	mov	word ptr [bp-12],ax
	mov	cx,5
	mov	bx,offset @41@C1954
@41@926:
	mov	ax,word ptr cs:[bx]
	cmp	ax,word ptr [bp-12]
	je	short @41@1010
	add	bx,2
	loop	short @41@926
	jmp	short @41@1346
@41@1010:
	jmp	word ptr cs:[bx+10]
@41@1038:
   ;	
   ;							{
   ;								case 59:if(Game.st!=PAUSA) set=1;break;
   ;	
	cmp	word ptr DGROUP:_Game,4
	je	short @41@1094
	mov	si,1
@41@1094:
	jmp	short @41@1346
@41@1122:
   ;	
   ;								case 60:set=2;break;
   ;	
	mov	si,2
	jmp	short @41@1346
@41@1150:
   ;	
   ;								case 61:som=!som;break;
   ;	
	mov	ax,word ptr DGROUP:_som
	neg	ax
	sbb	ax,ax
	inc	ax
	mov	word ptr DGROUP:_som,ax
	jmp	short @41@1346
@41@1178:
   ;	
   ;								case 80:if(Game.st==PAUSA)
   ;	
	cmp	word ptr DGROUP:_Game,4
	jne	short @41@1234
   ;	
   ;												set=ControlPopUp(0);
   ;	
	xor	ax,ax
	push	ax
	call	near ptr _ControlPopUp
	pop	cx
	cbw	
	mov	si,ax
@41@1234:
   ;	
   ;										  break;
   ;	
	jmp	short @41@1346
@41@1262:
   ;	
   ;								case 72:if(Game.st==PAUSA)
   ;	
	cmp	word ptr DGROUP:_Game,4
	jne	short @41@1318
   ;	
   ;												set=ControlPopUp(1);
   ;	
	mov	ax,1
	push	ax
	call	near ptr _ControlPopUp
	pop	cx
	cbw	
	mov	si,ax
@41@1318:
   ;	
   ;										  break;
   ;	
	jmp	short @41@1346
@41@1346:
   ;	
   ;							}
   ;							break;
   ;	
	jmp	short @41@1458
@41@1374:
   ;	
   ;					case 13:if(Game.st==PAUSA)
   ;	
	cmp	word ptr DGROUP:_Game,4
	jne	short @41@1430
   ;	
   ;									set=ControlPopUp(2);
   ;	
	mov	ax,2
	push	ax
	call	near ptr _ControlPopUp
	pop	cx
	cbw	
	mov	si,ax
@41@1430:
   ;	
   ;								break;
   ;	
	jmp	short @41@1458
@41@1458:
   ;	
   ;	
   ;				}
   ;	
   ;			}
   ;			if(!done)
   ;	
	mov	al,byte ptr [bp-1]
	cbw	
	or	ax,ax
	jne	short @41@2046
   ;	
   ;				switch(set)
   ;	
	mov	bx,si
	dec	bx
	cmp	bx,4
	ja	short @41@1822
	shl	bx,1
	jmp	word ptr cs:@41@C2098[bx]
@41@1570:
   ;	
   ;				{
   ;					case 1:GameInit();break;
   ;	
	call	near ptr _GameInit
	jmp	short @41@2046
@41@1598:
   ;	
   ;					case 2:if(pl==0)
   ;	
	cmp	word ptr DGROUP:_pl,0
	jne	short @41@1654
   ;	
   ;								pl=1;
   ;	
	mov	word ptr DGROUP:_pl,1
	jmp	short @41@1738
@41@1654:
   ;	
   ;							 else
   ;							 {
   ;								pl=0;
   ;	
	mov	word ptr DGROUP:_pl,0
   ;	
   ;								if(stl<9)
   ;	
	cmp	word ptr DGROUP:_stl,9
	jge	short @41@1710
   ;	
   ;									stl++;
   ;	
	inc	word ptr DGROUP:_stl
	jmp	short @41@1738
@41@1710:
   ;	
   ;								else
   ;									stl=0;
   ;	
	mov	word ptr DGROUP:_stl,0
@41@1738:
	jmp	short @41@1570
@41@1766:
   ;	
   ;							 }
   ;							 GameInit();
   ;							break;
   ;					case 4:done=1;break;
   ;	
	mov	byte ptr [bp-1],1
	jmp	short @41@2046
@41@1794:
   ;	
   ;					case 5:Game.st=WAIT;break;
   ;	
	mov	word ptr DGROUP:_Game,0
	jmp	short @41@2046
@41@1822:
   ;	
   ;					default:if((WaitToStart())||(set==3))
   ;	
	call	near ptr _WaitToStart
	or	al,al
	jne	short @41@1878
	cmp	si,3
	jne	short @41@2046
@41@1878:
   ;	
   ;								{
   ;									switch(Game.st)
   ;	
	mov	ax,word ptr DGROUP:_Game
	or	ax,ax
	je	short @41@1990
	cmp	ax,3
	je	short @41@2018
	jmp	short @41@2046
@41@1990:
   ;	
   ;									{
   ;										case WAIT:Game.st=PLAY;break;
   ;	
	mov	word ptr DGROUP:_Game,1
	jmp	short @41@2046
@41@2018:
   ;	
   ;										case GO:GameInit();break;
   ;	
	call	near ptr _GameInit
	jmp	short @41@2046
@41@2046:
   ;	
   ;									}
   ;								}
   ;				}
   ;			SndC();
   ;	
	call	near ptr _SndC
   ;	
   ;			UpDateScreen();
   ;	
	call	near ptr _UpDateScreen
   ;	
   ;			frcnt++;
   ;	
	inc	di
   ;	
   ;	
   ;			hsecs=gethseconds();
   ;	
	call	near ptr _gethseconds
	mov	word ptr [bp-4],ax
   ;	
   ;			if(hsecs-oldclk>100)
   ;	
	mov	ax,word ptr [bp-4]
	sub	ax,word ptr [bp-6]
	cmp	ax,100
	jbe	short @41@2102
   ;	
   ;			{
   ;				frames=frcnt;
   ;	
	mov	word ptr DGROUP:_frames,di
   ;	
   ;				frcnt=0;
   ;	
	xor	di,di
   ;	
   ;				oldclk=hsecs;
   ;	
	mov	ax,word ptr [bp-4]
	mov	word ptr [bp-6],ax
@41@2102:
	mov	al,byte ptr [bp-1]
	cbw	
	or	ax,ax
	jne	@@9
	jmp	@41@114
@@9:
   ;	
   ;			}
   ;		}
   ;		nosound();
   ;	
	call	near ptr _nosound
   ;	
   ;		if(musica==2) Midi_Stop(MusSeg);
   ;	
	cmp	word ptr DGROUP:_musica,2
	jne	short @41@2186
	push	word ptr DGROUP:_MusSeg+2
	push	word ptr DGROUP:_MusSeg
	call	near ptr _Midi_Stop
	pop	cx
	pop	cx
@41@2186:
   ;	
   ;		intrtab[0x9]=oldkeyb;
   ;	
	les	bx,dword ptr [bp-10]
	mov	ax,word ptr DGROUP:_oldkeyb+2
	mov	dx,word ptr DGROUP:_oldkeyb
	mov	word ptr es:[bx+38],ax
	mov	word ptr es:[bx+36],dx
   ;	
   ;	/*	SetIntVect(0x9,oldkeyb); */
   ;	}
   ;	
	pop	di
	pop	si
	mov	sp,bp
	pop	bp
	ret	
_Play_game	endp
@41@C2098	label	word
	dw	@41@1570
	dw	@41@1598
	dw	@41@1822
	dw	@41@1766
	dw	@41@1794
@41@C1954	label	word
	db	59
	db	0
	db	60
	db	0
	db	61
	db	0
	db	72
	db	0
	db	80
	db	0
	dw	@41@1038
	dw	@41@1122
	dw	@41@1150
	dw	@41@1262
	dw	@41@1178
   ;	
   ;	void Ajuda()
   ;	
	assume	cs:_TEXT
_Ajuda	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;		/*
   ;		BYTE tecla;
   ;		int handle,bytes;
   ;		unsigned p100;
   ;		unsigned pos;
   ;		unsigned max;
   ;		BYTE caracter;
   ;		handle=_open("misc/PacMan.hlp",O_RDONLY|O_BINARY);
   ;		textmode(3);
   ;		SetOverScan(5);
   ;		if(handle==-1)
   ;		{
   ;			puts("Erro ao abrir ficheiro de ajuda.");
   ;			getch();
   ;		}
   ;		else
   ;		{
   ;			do
   ;			{
   ;				textcolor(7);
   ;				textbackground(1);
   ;				clrscr();
   ;				textcolor(2);
   ;				textbackground(4);
   ;				gotoxy(1,1);
   ;				clreol();
   ;				cprintf(" P a c   M a n");
   ;				gotoxy(75,1);
   ;				cprintf("Ajuda\r\n");
   ;				textcolor(7);
   ;				textbackground(1);
   ;				do
   ;				{
   ;					bytes=_read(handle,&caracter,1);
   ;					if(bytes)putch(caracter);
   ;				}while((bytes>0)&&(wherey()!=25));
   ;				textcolor(2);
   ;				textbackground(4);
   ;				gotoxy(1,25);
   ;				clreol();
   ;				max=filelength(handle);
   ;				pos=tell(handle);
   ;				asm {
   ;					push ax;
   ;					push cx;
   ;					push dx;
   ;					mov cx,100;
   ;					xor dx,dx;
   ;					mov ax,pos;
   ;					mul cx;
   ;					div max;
   ;					mov p100, ax;
   ;					pop dx;
   ;					pop cx;
   ;					pop ax;
   ;				}
   ;				gotoxy(75,25);
   ;				cprintf("%03u%%",p100);
   ;				gotoxy(1,25);
   ;				if(bytes)
   ;				{
   ;					cputs("-- Mais --");
   ;					tecla=getch();
   ;					if(tecla==0) tecla=getch();
   ;					if(tecla==27) break;
   ;				}
   ;			}while(bytes>0);
   ;			_close(handle);
   ;			if(tecla!=27)
   ;			while(getch()!=27);
   ;		}
   ;		modo13h();
   ;		setpal(palette);
   ;		*/
   ;	}
   ;	
	pop	bp
	ret	
_Ajuda	endp
   ;	
   ;	void Sobre()
   ;	
	assume	cs:_TEXT
_Sobre	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;		Fundo();
   ;	
	call	near ptr _Fundo
   ;	
   ;		TextColor=MkColor(0,0,2);
   ;	
	mov	byte ptr DGROUP:_TextColor,2
   ;	
   ;		writest(50,70,"VERSAO: ");
   ;	
	mov	ax,offset DGROUP:s@+282
	push	ax
	mov	ax,70
	push	ax
	mov	ax,50
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;		writest(50,100,"AUTOR: ");
   ;	
	mov	ax,offset DGROUP:s@+291
	push	ax
	mov	ax,100
	push	ax
	mov	ax,50
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;		writest(50,130,"DATA: ");
   ;	
	mov	ax,offset DGROUP:s@+299
	push	ax
	mov	ax,130
	push	ax
	mov	ax,50
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;		writest(190,70,"1.0");
   ;	
	mov	ax,offset DGROUP:s@+306
	push	ax
	mov	ax,70
	push	ax
	mov	ax,190
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;		writest(150,100,"RENATO LOURENCO");
   ;	
	mov	ax,offset DGROUP:s@+310
	push	ax
	mov	ax,100
	push	ax
	mov	ax,150
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;		writest(160,130,"FEVEREIRO 2002");
   ;	
	mov	ax,offset DGROUP:s@+326
	push	ax
	mov	ax,130
	push	ax
	mov	ax,160
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;		TextColor=MkColor(4,0,0);
   ;	
	mov	byte ptr DGROUP:_TextColor,128
   ;	
   ;		writest(51,71,"VERSAO: ");
   ;	
	mov	ax,offset DGROUP:s@+341
	push	ax
	mov	ax,71
	push	ax
	mov	ax,51
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;		writest(51,101,"AUTOR: ");
   ;	
	mov	ax,offset DGROUP:s@+350
	push	ax
	mov	ax,101
	push	ax
	mov	ax,51
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;		writest(51,131,"DATA: ");
   ;	
	mov	ax,offset DGROUP:s@+358
	push	ax
	mov	ax,131
	push	ax
	mov	ax,51
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;		writest(191,71,"1.0");
   ;	
	mov	ax,offset DGROUP:s@+365
	push	ax
	mov	ax,71
	push	ax
	mov	ax,191
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;		writest(151,101,"RENATO LOURENCO");
   ;	
	mov	ax,offset DGROUP:s@+369
	push	ax
	mov	ax,101
	push	ax
	mov	ax,151
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;		writest(161,131,"FEVEREIRO 2002");
   ;	
	mov	ax,offset DGROUP:s@+385
	push	ax
	mov	ax,131
	push	ax
	mov	ax,161
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;		ScrnCpyUD();
   ;	
	call	near ptr _ScrnCpyUD
   ;	
   ;		getch();
   ;	
	call	near ptr _getch
   ;	
   ;	}
   ;	
	pop	bp
	ret	
_Sobre	endp
   ;	
   ;	void Fundo()
   ;	
	assume	cs:_TEXT
_Fundo	proc	near
	push	bp
	mov	bp,sp
	push	si
   ;	
   ;	{
   ;		int c;
   ;		fillscreen(0);
   ;	
	mov	al,0
	push	ax
	call	near ptr _fillscreen
	pop	cx
   ;	
   ;		for(c=0;c<300;c++)
   ;	
	xor	si,si
	jmp	short @44@114
@44@58:
   ;	
   ;			DrawPixel(xrandom(320),xrandom(180)+20,xrandom(255));
   ;	
	mov	ax,255
	push	ax
	call	near ptr _mrandom
	pop	cx
	push	ax
	mov	ax,180
	push	ax
	call	near ptr _mrandom
	pop	cx
	add	ax,20
	push	ax
	mov	ax,320
	push	ax
	call	near ptr _mrandom
	pop	cx
	push	ax
	call	near ptr _DrawPixel
	add	sp,6
	inc	si
@44@114:
	cmp	si,300
	jl	short @44@58
   ;	
   ;		Color=MkColor(2,2,2);
   ;	
	mov	byte ptr DGROUP:_Color,74
   ;	
   ;		DrawRect(0,0,319,199,LINE);
   ;	
	mov	al,1
	push	ax
	mov	ax,199
	push	ax
	mov	ax,319
	push	ax
	xor	ax,ax
	push	ax
	xor	ax,ax
	push	ax
	call	near ptr _DrawRect
	add	sp,10
   ;	
   ;		Color=MkColor(0,0,3);
   ;	
	mov	byte ptr DGROUP:_Color,3
   ;	
   ;		FillColor=MkColor(5,0,0);
   ;	
	mov	byte ptr DGROUP:_FillColor,160
   ;	
   ;		DrawRect(1,1,318,20,LIFI);
   ;	
	mov	al,0
	push	ax
	mov	ax,20
	push	ax
	mov	ax,318
	push	ax
	mov	ax,1
	push	ax
	mov	ax,1
	push	ax
	call	near ptr _DrawRect
	add	sp,10
   ;	
   ;		TextColor=MkColor(0,0,0);
   ;	
	mov	byte ptr DGROUP:_TextColor,0
   ;	
   ;		writest(100,7,"P A C   M A N");
   ;	
	mov	ax,offset DGROUP:s@+400
	push	ax
	mov	ax,7
	push	ax
	mov	ax,100
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;		TextColor=MkColor(0,7,0);
   ;	
	mov	byte ptr DGROUP:_TextColor,28
   ;	
   ;		writest(101,8,"P A C   M A N");
   ;	
	mov	ax,offset DGROUP:s@+414
	push	ax
	mov	ax,8
	push	ax
	mov	ax,101
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;	}
   ;	
	pop	si
	pop	bp
	ret	
_Fundo	endp
   ;	
   ;	char Menu()
   ;	
	assume	cs:_TEXT
_Menu	proc	near
	push	bp
	mov	bp,sp
	sub	sp,4
   ;	
   ;	{
   ;		BYTE done=0,op=0;
   ;	
	mov	byte ptr [bp-1],0
	mov	byte ptr [bp-2],0
   ;	
   ;		BYTE tecla;
   ;		Fundo();
   ;	
	call	near ptr _Fundo
   ;	
   ;		FillColor=MkColor(4,0,0);
   ;	
	mov	byte ptr DGROUP:_FillColor,128
   ;	
   ;		Color=MkColor(6,0,0);
   ;	
	mov	byte ptr DGROUP:_Color,192
   ;	
   ;		DrawRect(100,60,220,80,LIFI);
   ;	
	mov	al,0
	push	ax
	mov	ax,80
	push	ax
	mov	ax,220
	push	ax
	mov	ax,60
	push	ax
	mov	ax,100
	push	ax
	call	near ptr _DrawRect
	add	sp,10
   ;	
   ;		TextColor=MkColor(0,0,1);
   ;	
	mov	byte ptr DGROUP:_TextColor,1
   ;	
   ;		writest(143,66,"MENU");
   ;	
	mov	ax,offset DGROUP:s@+428
	push	ax
	mov	ax,66
	push	ax
	mov	ax,143
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;		TextColor=MkColor(0,0,3);
   ;	
	mov	byte ptr DGROUP:_TextColor,3
   ;	
   ;		writest(144,67,"MENU");
   ;	
	mov	ax,offset DGROUP:s@+433
	push	ax
	mov	ax,67
	push	ax
	mov	ax,144
	push	ax
	call	near ptr _writest
	add	sp,6
	jmp	@45@590
@45@58:
   ;	
   ;		while(!done)
   ;		{
   ;			FillColor=MkColor(0,0,0);
   ;	
	mov	byte ptr DGROUP:_FillColor,0
   ;	
   ;			Color=MkColor(7,0,0);
   ;	
	mov	byte ptr DGROUP:_Color,224
   ;	
   ;			DrawRect(100,82,220,160,LIFI);
   ;	
	mov	al,0
	push	ax
	mov	ax,160
	push	ax
	mov	ax,220
	push	ax
	mov	ax,82
	push	ax
	mov	ax,100
	push	ax
	call	near ptr _DrawRect
	add	sp,10
   ;	
   ;			TextColor=MkColor(0,7,0);
   ;	
	mov	byte ptr DGROUP:_TextColor,28
   ;	
   ;			writest(140,90,"JOGAR");
   ;	
	mov	ax,offset DGROUP:s@+438
	push	ax
	mov	ax,90
	push	ax
	mov	ax,140
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;			writest(140,104,"SETUP");
   ;	
	mov	ax,offset DGROUP:s@+444
	push	ax
	mov	ax,104
	push	ax
	mov	ax,140
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;			writest(140,118,"AJUDA");
   ;	
	mov	ax,offset DGROUP:s@+450
	push	ax
	mov	ax,118
	push	ax
	mov	ax,140
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;			writest(140,132,"SOBRE");
   ;	
	mov	ax,offset DGROUP:s@+456
	push	ax
	mov	ax,132
	push	ax
	mov	ax,140
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;			writest(140,146,"SAIR");
   ;	
	mov	ax,offset DGROUP:s@+462
	push	ax
	mov	ax,146
	push	ax
	mov	ax,140
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;			Color=MkColor(1,6,1);
   ;	
	mov	byte ptr DGROUP:_Color,57
   ;	
   ;			DrawRect(105,88+14*op,215,98+14*op,LINE);
   ;	
	mov	al,1
	push	ax
	mov	al,byte ptr [bp-2]
	mov	ah,0
	mov	dx,14
	imul	dx
	add	ax,98
	push	ax
	mov	ax,215
	push	ax
	mov	al,byte ptr [bp-2]
	mov	ah,0
	mov	dx,14
	imul	dx
	add	ax,88
	push	ax
	mov	ax,105
	push	ax
	call	near ptr _DrawRect
	add	sp,10
   ;	
   ;			ScrnCpyUD();
   ;	
	call	near ptr _ScrnCpyUD
   ;	
   ;			tecla=getch();
   ;	
	call	near ptr _getch
	mov	byte ptr [bp-3],al
   ;	
   ;			switch(tecla)
   ;	
	mov	al,byte ptr [bp-3]
	mov	ah,0
	or	ax,ax
	je	short @45@226
	cmp	ax,13
	je	short @45@198
	jmp	short @45@590
@45@198:
   ;	
   ;			{
   ;				case 13:done=1;break;
   ;	
	mov	byte ptr [bp-1],1
	jmp	short @45@590
@45@226:
   ;	
   ;				case 0:switch(getch())
   ;	
	call	near ptr _getch
	cmp	ax,72
	je	short @45@338
	cmp	ax,80
	je	short @45@450
	jmp	short @45@562
@45@338:
   ;	
   ;						 {
   ;							case 72:if(op==0)
   ;	
	cmp	byte ptr [bp-2],0
	jne	short @45@394
   ;	
   ;											op=4;
   ;	
	mov	byte ptr [bp-2],4
	jmp	short @45@422
@45@394:
   ;	
   ;										else
   ;											op--;
   ;	
	dec	byte ptr [bp-2]
@45@422:
   ;	
   ;										break;
   ;	
	jmp	short @45@562
@45@450:
   ;	
   ;							case 80:if(op==4)
   ;	
	cmp	byte ptr [bp-2],4
	jne	short @45@506
   ;	
   ;											op=0;
   ;	
	mov	byte ptr [bp-2],0
	jmp	short @45@534
@45@506:
   ;	
   ;										else
   ;											op++;
   ;	
	inc	byte ptr [bp-2]
@45@534:
   ;	
   ;										break;
   ;	
	jmp	short @45@562
@45@562:
   ;	
   ;						 }
   ;						 break;
   ;	
	jmp	short @45@590
@45@590:
	mov	al,byte ptr [bp-1]
	mov	ah,0
	or	ax,ax
	jne	@@10
	jmp	@45@58
@@10:
   ;	
   ;			}
   ;		}
   ;		return op;
   ;	
	mov	al,byte ptr [bp-2]
	jmp	short @45@646
@45@646:
   ;	
   ;	}
   ;	
	mov	sp,bp
	pop	bp
	ret	
_Menu	endp
   ;	
   ;	void SetupMenu()
   ;	
	assume	cs:_TEXT
_SetupMenu	proc	near
	push	bp
	mov	bp,sp
	sub	sp,12
   ;	
   ;	{
   ;		BYTE done=0,op=0,sd,niv,p,mus;
   ;	
	mov	byte ptr [bp-1],0
	mov	byte ptr [bp-2],0
   ;	
   ;		BYTE st[4],tecla;
   ;		st[3]=0;
   ;	
	mov	byte ptr [bp-7],0
   ;	
   ;		mus=musica;
   ;	
	mov	al,byte ptr DGROUP:_musica
	mov	byte ptr [bp-6],al
   ;	
   ;		sd=som;
   ;	
	mov	al,byte ptr DGROUP:_som
	mov	byte ptr [bp-3],al
   ;	
   ;		niv=stl;
   ;	
	mov	al,byte ptr DGROUP:_stl
	mov	byte ptr [bp-4],al
   ;	
   ;		p=pl;
   ;	
	mov	al,byte ptr DGROUP:_pl
	mov	byte ptr [bp-5],al
   ;	
   ;		Fundo();
   ;	
	call	near ptr _Fundo
   ;	
   ;		FillColor=MkColor(2,2,1);
   ;	
	mov	byte ptr DGROUP:_FillColor,73
   ;	
   ;		Color=MkColor(3,3,2);
   ;	
	mov	byte ptr DGROUP:_Color,110
   ;	
   ;		DrawRect(100,56,220,76,LIFI);
   ;	
	mov	al,0
	push	ax
	mov	ax,76
	push	ax
	mov	ax,220
	push	ax
	mov	ax,56
	push	ax
	mov	ax,100
	push	ax
	call	near ptr _DrawRect
	add	sp,10
   ;	
   ;		TextColor=MkColor(0,0,1);
   ;	
	mov	byte ptr DGROUP:_TextColor,1
   ;	
   ;		writest(139,63,"SETUP");
   ;	
	mov	ax,offset DGROUP:s@+467
	push	ax
	mov	ax,63
	push	ax
	mov	ax,139
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;		TextColor=MkColor(1,1,3);
   ;	
	mov	byte ptr DGROUP:_TextColor,39
   ;	
   ;		writest(140,64,"SETUP");
   ;	
	mov	ax,offset DGROUP:s@+473
	push	ax
	mov	ax,64
	push	ax
	mov	ax,140
	push	ax
	call	near ptr _writest
	add	sp,6
	jmp	@46@1486
@46@58:
   ;	
   ;		while(!done)
   ;		{
   ;			FillColor=MkColor(0,0,0);
   ;	
	mov	byte ptr DGROUP:_FillColor,0
   ;	
   ;			Color=MkColor(5,5,2);
   ;	
	mov	byte ptr DGROUP:_Color,182
   ;	
   ;			DrawRect(100,78,220,162,LIFI);
   ;	
	mov	al,0
	push	ax
	mov	ax,162
	push	ax
	mov	ax,220
	push	ax
	mov	ax,78
	push	ax
	mov	ax,100
	push	ax
	call	near ptr _DrawRect
	add	sp,10
   ;	
   ;			TextColor=MkColor(0,0,2);
   ;	
	mov	byte ptr DGROUP:_TextColor,2
   ;	
   ;			writest(110,82,"NIVEL");
   ;	
	mov	ax,offset DGROUP:s@+479
	push	ax
	mov	ax,82
	push	ax
	mov	ax,110
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;			writest(110,96,"PLAYERS");
   ;	
	mov	ax,offset DGROUP:s@+485
	push	ax
	mov	ax,96
	push	ax
	mov	ax,110
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;			writest(110,110,"MUSICA");
   ;	
	mov	ax,offset DGROUP:s@+493
	push	ax
	mov	ax,110
	push	ax
	mov	ax,110
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;			writest(110,124,"SOM");
   ;	
	mov	ax,offset DGROUP:s@+500
	push	ax
	mov	ax,124
	push	ax
	mov	ax,110
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;			TextColor=MkColor(0,4,0);
   ;	
	mov	byte ptr DGROUP:_TextColor,16
   ;	
   ;			writest(110,138,"ACEITAR");
   ;	
	mov	ax,offset DGROUP:s@+504
	push	ax
	mov	ax,138
	push	ax
	mov	ax,110
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;			TextColor=MkColor(4,0,0);
   ;	
	mov	byte ptr DGROUP:_TextColor,128
   ;	
   ;			writest(110,152,"CANCELAR");
   ;	
	mov	ax,offset DGROUP:s@+512
	push	ax
	mov	ax,152
	push	ax
	mov	ax,110
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;			st[2]=0;
   ;	
	mov	byte ptr [bp-8],0
   ;	
   ;			st[1]=niv%10+48;
   ;	
	mov	al,byte ptr [bp-4]
	mov	ah,0
	mov	bx,10
	cwd	
	idiv	bx
	add	dl,48
	mov	byte ptr [bp-9],dl
   ;	
   ;			st[0]=niv/10+48;
   ;	
	mov	al,byte ptr [bp-4]
	mov	ah,0
	mov	bx,10
	cwd	
	idiv	bx
	add	al,48
	mov	byte ptr [bp-10],al
   ;	
   ;			TextColor=MkColor(0,5,0);
   ;	
	mov	byte ptr DGROUP:_TextColor,20
   ;	
   ;			writest(190,82,st);
   ;	
	lea	ax,word ptr [bp-10]
	push	ax
	mov	ax,82
	push	ax
	mov	ax,190
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;			st[0]=' ';
   ;	
	mov	byte ptr [bp-10],32
   ;	
   ;			if(p)
   ;	
	cmp	byte ptr [bp-5],0
	je	short @46@114
   ;	
   ;				st[1]='2';
   ;	
	mov	byte ptr [bp-9],50
	jmp	short @46@142
@46@114:
   ;	
   ;			else
   ;				st[1]='1';
   ;	
	mov	byte ptr [bp-9],49
@46@142:
   ;	
   ;	
   ;			st[2]=0;
   ;	
	mov	byte ptr [bp-8],0
   ;	
   ;			writest(190,96,st);
   ;	
	lea	ax,word ptr [bp-10]
	push	ax
	mov	ax,96
	push	ax
	mov	ax,190
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;			switch(mus)
   ;	
	mov	al,byte ptr [bp-6]
	mov	ah,0
	or	ax,ax
	je	short @46@310
	cmp	ax,1
	je	short @46@338
	cmp	ax,2
	je	short @46@366
	jmp	short @46@394
@46@310:
   ;	
   ;			{
   ;				case 0:TextColor=MkColor(2,2,1);st[0]='N';st[1]='/';st[2]='A';break;
   ;	
	mov	byte ptr DGROUP:_TextColor,73
	mov	byte ptr [bp-10],78
	mov	byte ptr [bp-9],47
	mov	byte ptr [bp-8],65
	jmp	short @46@394
@46@338:
   ;	
   ;				case 1:TextColor=MkColor(5,0,0);st[0]='O';st[1]='F';st[2]='F';break;
   ;	
	mov	byte ptr DGROUP:_TextColor,160
	mov	byte ptr [bp-10],79
	mov	byte ptr [bp-9],70
	mov	byte ptr [bp-8],70
	jmp	short @46@394
@46@366:
   ;	
   ;				case 2:TextColor=MkColor(0,5,0);st[0]='O';st[1]='N';st[2]=' ';break;
   ;	
	mov	byte ptr DGROUP:_TextColor,20
	mov	byte ptr [bp-10],79
	mov	byte ptr [bp-9],78
	mov	byte ptr [bp-8],32
	jmp	short @46@394
@46@394:
   ;	
   ;			}
   ;			writest(190,110,st);
   ;	
	lea	ax,word ptr [bp-10]
	push	ax
	mov	ax,110
	push	ax
	mov	ax,190
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;			if(sd)
   ;	
	cmp	byte ptr [bp-3],0
	je	short @46@450
   ;	
   ;			{
   ;				TextColor=MkColor(0,5,0);
   ;	
	mov	byte ptr DGROUP:_TextColor,20
   ;	
   ;				st[0]='O';st[1]='N';st[2]=' ';
   ;	
	mov	byte ptr [bp-10],79
	mov	byte ptr [bp-9],78
	mov	byte ptr [bp-8],32
   ;	
   ;			}
   ;	
	jmp	short @46@478
@46@450:
   ;	
   ;			else
   ;			{
   ;				TextColor=MkColor(5,0,0);
   ;	
	mov	byte ptr DGROUP:_TextColor,160
   ;	
   ;				st[0]='O';st[1]='F';st[2]='F';
   ;	
	mov	byte ptr [bp-10],79
	mov	byte ptr [bp-9],70
	mov	byte ptr [bp-8],70
@46@478:
   ;	
   ;			}
   ;			writest(190,124,st);
   ;	
	lea	ax,word ptr [bp-10]
	push	ax
	mov	ax,124
	push	ax
	mov	ax,190
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;			Color=MkColor(5,5,0);
   ;	
	mov	byte ptr DGROUP:_Color,180
   ;	
   ;			DrawRect(105,80+14*op,215,90+14*op,LINE);
   ;	
	mov	al,1
	push	ax
	mov	al,byte ptr [bp-2]
	mov	ah,0
	mov	dx,14
	imul	dx
	add	ax,90
	push	ax
	mov	ax,215
	push	ax
	mov	al,byte ptr [bp-2]
	mov	ah,0
	mov	dx,14
	imul	dx
	add	ax,80
	push	ax
	mov	ax,105
	push	ax
	call	near ptr _DrawRect
	add	sp,10
   ;	
   ;			ScrnCpyUD();
   ;	
	call	near ptr _ScrnCpyUD
   ;	
   ;			tecla=getch();
   ;	
	call	near ptr _getch
	mov	byte ptr [bp-11],al
   ;	
   ;			switch(tecla)
   ;	
	mov	al,byte ptr [bp-11]
	mov	ah,0
	or	ax,ax
	jne	@@11
	jmp	@46@1150
@@11:
	cmp	ax,13
	je	short @46@618
	jmp	@46@1486
@46@618:
   ;	
   ;			{
   ;				case 13:switch(op)
   ;	
	mov	al,byte ptr [bp-2]
	mov	ah,0
	mov	bx,ax
	cmp	bx,5
	jbe	@@12
	jmp	@46@1122
@@12:
	shl	bx,1
	jmp	word ptr cs:@46@C1426[bx]
@46@702:
   ;	
   ;							{
   ;								case 0:if(niv<9)
   ;	
	cmp	byte ptr [bp-4],9
	jae	short @46@758
   ;	
   ;											niv++;
   ;	
	inc	byte ptr [bp-4]
	jmp	short @46@786
@46@758:
   ;	
   ;										else
   ;											niv=0;
   ;	
	mov	byte ptr [bp-4],0
@46@786:
   ;	
   ;										break;
   ;	
	jmp	short @46@1122
@46@814:
   ;	
   ;								case 2:switch(mus)
   ;	
	mov	al,byte ptr [bp-6]
	mov	ah,0
	cmp	ax,1
	je	short @46@926
	cmp	ax,2
	je	short @46@954
	jmp	short @46@982
@46@926:
   ;	
   ;										{
   ;											case 1:mus=2;break;
   ;	
	mov	byte ptr [bp-6],2
	jmp	short @46@982
@46@954:
   ;	
   ;											case 2:mus=1;break;
   ;	
	mov	byte ptr [bp-6],1
	jmp	short @46@982
@46@982:
   ;	
   ;										}
   ;										break;
   ;	
	jmp	short @46@1122
@46@1010:
   ;	
   ;								case 1:p=!p;break;
   ;	
	mov	al,byte ptr [bp-5]
	mov	ah,0
	neg	ax
	sbb	ax,ax
	inc	ax
	mov	byte ptr [bp-5],al
	jmp	short @46@1122
@46@1038:
   ;	
   ;								case 3:sd=!sd;break;
   ;	
	mov	al,byte ptr [bp-3]
	mov	ah,0
	neg	ax
	sbb	ax,ax
	inc	ax
	mov	byte ptr [bp-3],al
	jmp	short @46@1122
@46@1066:
   ;	
   ;								case 4:stl=niv;
   ;	
	mov	al,byte ptr [bp-4]
	mov	ah,0
	mov	word ptr DGROUP:_stl,ax
   ;	
   ;										pl=p;
   ;	
	mov	al,byte ptr [bp-5]
	mov	ah,0
	mov	word ptr DGROUP:_pl,ax
   ;	
   ;										musica=mus;
   ;	
	mov	al,byte ptr [bp-6]
	mov	ah,0
	mov	word ptr DGROUP:_musica,ax
   ;	
   ;										som=sd;
   ;	
	mov	al,byte ptr [bp-3]
	mov	ah,0
	mov	word ptr DGROUP:_som,ax
@46@1094:
   ;	
   ;								case 5:done=1;break;
   ;	
	mov	byte ptr [bp-1],1
	jmp	short @46@1122
@46@1122:
   ;	
   ;							}
   ;							break;
   ;	
	jmp	short @46@1486
@46@1150:
   ;	
   ;				case 0:switch(getch())
   ;	
	call	near ptr _getch
	cmp	ax,72
	je	short @46@1262
	cmp	ax,80
	je	short @46@1374
	jmp	short @46@1486
@46@1262:
   ;	
   ;						 {
   ;							case 72:if(op==0)
   ;	
	cmp	byte ptr [bp-2],0
	jne	short @46@1318
   ;	
   ;											op=5;
   ;	
	mov	byte ptr [bp-2],5
	jmp	short @46@1346
@46@1318:
   ;	
   ;										else
   ;											op--;
   ;	
	dec	byte ptr [bp-2]
@46@1346:
   ;	
   ;										break;
   ;	
	jmp	short @46@1486
@46@1374:
   ;	
   ;							case 80:if(op==5)
   ;	
	cmp	byte ptr [bp-2],5
	jne	short @46@1430
   ;	
   ;											op=0;
   ;	
	mov	byte ptr [bp-2],0
	jmp	short @46@1458
@46@1430:
   ;	
   ;										else
   ;											op++;
   ;	
	inc	byte ptr [bp-2]
@46@1458:
   ;	
   ;										break;
   ;	
	jmp	short @46@1486
@46@1486:
	mov	al,byte ptr [bp-1]
	mov	ah,0
	or	ax,ax
	jne	@@13
	jmp	@46@58
@@13:
   ;	
   ;						 }
   ;			}
   ;		}
   ;	}
   ;	
	mov	sp,bp
	pop	bp
	ret	
_SetupMenu	endp
@46@C1426	label	word
	dw	@46@702
	dw	@46@1010
	dw	@46@814
	dw	@46@1038
	dw	@46@1066
	dw	@46@1094
   ;	
   ;	void SetSomOnOff()
   ;	
	assume	cs:_TEXT
_SetSomOnOff	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;		switch(musica)
   ;	
	mov	ax,word ptr DGROUP:_musica
	cmp	ax,1
	je	short @47@142
	cmp	ax,2
	je	short @47@198
	jmp	short @47@226
@47@142:
   ;	
   ;		{
   ;			case 1:
   ;					musica=2;
   ;	
	mov	word ptr DGROUP:_musica,2
   ;	
   ;					Midi_Play(MusSeg);
   ;	
	push	word ptr DGROUP:_MusSeg+2
	push	word ptr DGROUP:_MusSeg
	call	near ptr _Midi_Play
@47@170:
	pop	cx
	pop	cx
   ;	
   ;					break;
   ;	
	jmp	short @47@226
@47@198:
   ;	
   ;			case 2:
   ;					musica=1;
   ;	
	mov	word ptr DGROUP:_musica,1
   ;	
   ;					Midi_Stop(MusSeg);
   ;	
	push	word ptr DGROUP:_MusSeg+2
	push	word ptr DGROUP:_MusSeg
	call	near ptr _Midi_Stop
	jmp	short @47@170
@47@226:
   ;	
   ;					break;
   ;		}
   ;	}
   ;	
	pop	bp
	ret	
_SetSomOnOff	endp
   ;	
   ;	char WaitToStart()
   ;	
	assume	cs:_TEXT
_WaitToStart	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;		#ifdef USE_RATO
   ;		if(RatoM(ANY))
   ;			return 1;
   ;		#endif
   ;	  if(pl)
   ;	
	cmp	word ptr DGROUP:_pl,0
	je	short @48@450
   ;	
   ;	  {
   ;		switch(Game.plr)
   ;	
	mov	ax,word ptr DGROUP:_Game+9
	or	ax,ax
	je	short @48@170
	cmp	ax,1
	je	short @48@310
	jmp	short @48@422
@48@170:
   ;	
   ;		{
   ;			case 0:if(StickA[0]||StickA[1]) return 1;break;
   ;	
	cmp	byte ptr DGROUP:_StickA,0
	jne	short @48@226
	cmp	byte ptr DGROUP:_StickA+1,0
	je	short @48@282
@48@226:
	mov	al,1
@48@254:
	jmp	short @48@618
@48@282:
	jmp	short @48@422
@48@310:
   ;	
   ;			case 1:if(StickB[0]||StickB[1]) return 1;break;
   ;	
	cmp	byte ptr DGROUP:_StickB,0
	jne	short @48@366
	cmp	byte ptr DGROUP:_StickB+1,0
	je	short @48@394
@48@366:
	jmp	short @48@226
@48@394:
	jmp	short @48@422
@48@422:
   ;	
   ;		}
   ;	  }
   ;	
	jmp	short @48@590
@48@450:
   ;	
   ;	  else
   ;	  {
   ;		if(StickB[0]||StickB[1]||StickA[0]||StickA[1]) return 1;
   ;	
	cmp	byte ptr DGROUP:_StickB,0
	jne	short @48@562
	cmp	byte ptr DGROUP:_StickB+1,0
	jne	short @48@562
	cmp	byte ptr DGROUP:_StickA,0
	jne	short @48@562
	cmp	byte ptr DGROUP:_StickA+1,0
	je	short @48@590
@48@562:
	jmp	short @48@226
@48@590:
   ;	
   ;	  }
   ;	  return 0;
   ;	
	mov	al,0
	jmp	short @48@254
@48@618:
   ;	
   ;	}
   ;	
	pop	bp
	ret	
_WaitToStart	endp
   ;	
   ;	void CpyPFg(BYTE far *dest,BYTE far *orig)
   ;	
	assume	cs:_TEXT
_CpyPFg	proc	near
	push	bp
	mov	bp,sp
	push	si
	push	di
   ;	
   ;	{
   ;		int i;
   ;		int j;
   ;		for(i=0;i<PFYD;i++)
   ;	
	xor	si,si
	jmp	short @49@198
@49@58:
   ;	
   ;			for(j=0;j<PFXD;j++)
   ;	
	xor	di,di
	jmp	short @49@142
@49@86:
   ;	
   ;			{
   ;				dest[i*PFXD+j]=orig[i*PFXD+j];
   ;	
	mov	ax,si
	mov	dx,31
	imul	dx
	add	ax,di
	mov	es,word ptr [bp+10]
	add	ax,word ptr [bp+8]
	mov	bx,ax
	mov	al,byte ptr es:[bx]
	push	ax
	mov	ax,si
	mov	dx,31
	imul	dx
	add	ax,di
	mov	es,word ptr [bp+6]
	add	ax,word ptr [bp+4]
	mov	bx,ax
	pop	ax
	mov	byte ptr es:[bx],al
	inc	di
@49@142:
	cmp	di,31
	jl	short @49@86
	inc	si
@49@198:
	cmp	si,17
	jl	short @49@58
   ;	
   ;			}
   ;	}
   ;	
	pop	di
	pop	si
	pop	bp
	ret	
_CpyPFg	endp
   ;	
   ;	int mainx()
   ;	
	assume	cs:_TEXT
_mainx	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;		modo13h();
   ;	
	call	near ptr _modo13h
   ;	
   ;		fillscreen(0);
   ;	
	mov	al,0
	push	ax
	call	near ptr _fillscreen
	pop	cx
   ;	
   ;	    printf("Hello World!\r\n");
   ;	
	mov	ax,offset DGROUP:s@+521
	push	ax
	call	near ptr _printf
	pop	cx
   ;	
   ;		FillColor=40;
   ;	
	mov	byte ptr DGROUP:_FillColor,40
   ;	
   ;		Color=14;
   ;	
	mov	byte ptr DGROUP:_Color,14
   ;	
   ;		TextColor=14;
   ;	
	mov	byte ptr DGROUP:_TextColor,14
   ;	
   ;		DrawRect(10,10,40,40,LIFI);
   ;	
	mov	al,0
	push	ax
	mov	ax,40
	push	ax
	mov	ax,40
	push	ax
	mov	ax,10
	push	ax
	mov	ax,10
	push	ax
	call	near ptr _DrawRect
	add	sp,10
   ;	
   ;		writest(100,50,"Hello World!");
   ;	
	mov	ax,offset DGROUP:s@+536
	push	ax
	mov	ax,50
	push	ax
	mov	ax,100
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;		mcos_getkey();
   ;	
	call	near ptr _mcos_getkey
   ;	
   ;		modo3h();
   ;	
	call	near ptr _modo3h
   ;	
   ;		loadResourceFile("pacman.res",rmap,MAXRESOURCES);
   ;	
	mov	ax,50
	push	ax
	mov	ax,offset DGROUP:_rmap
	push	ax
	mov	ax,offset DGROUP:s@+549
	push	ax
	call	near ptr _loadResourceFile
	add	sp,6
   ;	
   ;		printf("Press a key\r\n");
   ;	
	mov	ax,offset DGROUP:s@+560
	push	ax
	call	near ptr _printf
	pop	cx
   ;	
   ;		mcos_getkey();
   ;	
	call	near ptr _mcos_getkey
   ;	
   ;		modo13h();
   ;	
	call	near ptr _modo13h
   ;	
   ;		drawpics();
   ;	
	call	near ptr _drawpics
   ;	
   ;		mcos_getkey();
   ;	
	call	near ptr _mcos_getkey
   ;	
   ;		modo3h();
   ;	
	call	near ptr _modo3h
   ;	
   ;	    return 0;
   ;	
	xor	ax,ax
	jmp	short @50@58
@50@58:
   ;	
   ;	}
   ;	
	pop	bp
	ret	
_mainx	endp
   ;	
   ;	void Show_Ghost(int g)
   ;	
	assume	cs:_TEXT
_Show_Ghost	proc	near
	push	bp
	mov	bp,sp
	sub	sp,4
	push	si
	push	di
	mov	si,word ptr [bp+4]
   ;	
   ;	{
   ;		int w;
   ;		int x,y;
   ;		w=vcm/4;
   ;	
	mov	ax,word ptr DGROUP:_vcm
	mov	bx,4
	cwd	
	idiv	bx
	mov	word ptr [bp-2],ax
   ;	
   ;		x=Ghost[g].m.x*10+PFXI+Ghost[g].m.ox;
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	mov	ax,word ptr DGROUP:_Ghost[bx]
	mov	dx,10
	imul	dx
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	add	ax,word ptr DGROUP:_Ghost[bx+4]
	add	ax,5
	mov	di,ax
   ;	
   ;		y=Ghost[g].m.y*10+PFYI+Ghost[g].m.oy;
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	mov	ax,word ptr DGROUP:_Ghost[bx+2]
	mov	dx,10
	imul	dx
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	add	ax,word ptr DGROUP:_Ghost[bx+6]
	mov	word ptr [bp-4],ax
   ;	
   ;		switch(Ghost[g].m.st)
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	mov	al,byte ptr DGROUP:_Ghost[bx+10]
	cbw	
	or	ax,ax
	je	short @51@198
	cmp	ax,1
	je	short @51@282
	cmp	ax,2
	jne	@@14
	jmp	@51@506
@@14:
	jmp	@51@534
@51@198:
   ;	
   ;		{
   ;			case 0:if(Game.count%2==0)
   ;	
	mov	ax,word ptr DGROUP:_Game+2
	mov	bx,2
	cwd	
	idiv	bx
	or	dx,dx
	jne	short @51@254
   ;	
   ;								DrawPic(x,y,Gp[g]);
   ;	
	mov	bx,si
	shl	bx,1
	shl	bx,1
	push	word ptr DGROUP:_Gp[bx+2]
	push	word ptr DGROUP:_Gp[bx]
	push	word ptr [bp-4]
	push	di
	call	near ptr _DrawPic
	add	sp,8
@51@254:
   ;	
   ;						break;
   ;	
	jmp	short @51@534
@51@282:
   ;	
   ;			case 1:if(Ghost[g].vc>w)
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	mov	ax,word ptr DGROUP:_Ghost[bx+13]
	cmp	ax,word ptr [bp-2]
	jle	short @51@338
   ;	
   ;							DrawPic(x,y,Gp[4]);
   ;	
	push	word ptr DGROUP:_Gp+18
	push	word ptr DGROUP:_Gp+16
	push	word ptr [bp-4]
	push	di
	call	near ptr _DrawPic
	add	sp,8
	jmp	short @51@478
@51@338:
   ;	
   ;					else
   ;					{
   ;						if(Game.count%10<5)
   ;	
	mov	ax,word ptr DGROUP:_Game+2
	mov	bx,10
	cwd	
	idiv	bx
	cmp	dx,5
	jge	short @51@394
   ;	
   ;								DrawPic(x,y,Gp[4]);
   ;	
	push	word ptr DGROUP:_Gp+18
	push	word ptr DGROUP:_Gp+16
	jmp	short @51@422
@51@394:
   ;	
   ;						else
   ;								DrawPic(x,y,Gp[g]);
   ;	
	mov	bx,si
	shl	bx,1
	shl	bx,1
	push	word ptr DGROUP:_Gp[bx+2]
	push	word ptr DGROUP:_Gp[bx]
@51@422:
	push	word ptr [bp-4]
	push	di
	call	near ptr _DrawPic
	add	sp,8
   ;	
   ;						break;
   ;	
	jmp	short @51@534
@51@478:
   ;	
   ;					}
   ;					break;
   ;	
	jmp	short @51@534
@51@506:
   ;	
   ;			case 2:DrawPic(x,y,Gp[5]);
   ;	
	push	word ptr DGROUP:_Gp+22
	push	word ptr DGROUP:_Gp+20
	push	word ptr [bp-4]
	push	di
	call	near ptr _DrawPic
	add	sp,8
@51@534:
   ;	
   ;		}
   ;	}
   ;	
	pop	di
	pop	si
	mov	sp,bp
	pop	bp
	ret	
_Show_Ghost	endp
   ;	
   ;	void Show_Pac()
   ;	
	assume	cs:_TEXT
_Show_Pac	proc	near
	push	bp
	mov	bp,sp
	push	si
   ;	
   ;	{
   ;		int p;
   ;		switch(Game.ast)
   ;	
	mov	bx,word ptr DGROUP:_Game+7
	cmp	bx,8
	ja	short @52@506
	shl	bx,1
	jmp	word ptr cs:@52@C626[bx]
@52@114:
   ;	
   ;		{
   ;				case 0:switch(Pac.m.st)
   ;	
	mov	al,byte ptr DGROUP:_Pac+10
	cbw	
	mov	bx,ax
	cmp	bx,3
	ja	short @52@310
	shl	bx,1
	jmp	word ptr cs:@52@C674[bx]
@52@198:
	jmp	short @52@310
@52@226:
   ;	
   ;						 {
   ;								case 0:p=0;break;
   ;								case 1:p=1;break;
   ;	
	mov	si,1
	jmp	short @52@338
@52@254:
   ;	
   ;								case 2:p=2;break;
   ;	
	mov	si,2
	jmp	short @52@338
@52@282:
   ;	
   ;								case 3:p=3;break;
   ;	
	mov	si,3
	jmp	short @52@338
@52@310:
   ;	
   ;								default:p=0;
   ;	
	xor	si,si
@52@338:
   ;	
   ;						 }
   ;						 if(Game.plr)
   ;	
	cmp	word ptr DGROUP:_Game+9,0
	je	short @52@394
   ;	
   ;							p+=8;
   ;	
	add	si,8
@52@394:
   ;	
   ;						 break;
   ;	
	jmp	short @52@534
@52@422:
   ;	
   ;				case 1:
   ;				case 2:p=4;break;
   ;	
	mov	si,4
	jmp	short @52@534
@52@450:
   ;	
   ;				case 3:
   ;				case 4:p=5;break;
   ;	
	mov	si,5
	jmp	short @52@534
@52@478:
   ;	
   ;				case 5:
   ;				case 6:p=6;break;
   ;	
	mov	si,6
	jmp	short @52@534
@52@506:
   ;	
   ;				case 7:
   ;				case 8:
   ;				default:p=7;
   ;	
	mov	si,7
@52@534:
   ;	
   ;		}
   ;	
   ;		DrawPic(Pac.m.x*10+PFXI+Pac.m.ox,Pac.m.y*10+PFYI+Pac.m.oy,Pac.pic[p]);
   ;	
	mov	bx,si
	shl	bx,1
	shl	bx,1
	push	word ptr DGROUP:_Pac[bx+15]
	push	word ptr DGROUP:_Pac[bx+13]
	mov	ax,word ptr DGROUP:_Pac+2
	mov	dx,10
	imul	dx
	add	ax,word ptr DGROUP:_Pac+6
	push	ax
	mov	ax,word ptr DGROUP:_Pac
	mov	dx,10
	imul	dx
	add	ax,word ptr DGROUP:_Pac+4
	add	ax,5
	push	ax
	call	near ptr _DrawPic
	add	sp,8
   ;	
   ;	}
   ;	
	pop	si
	pop	bp
	ret	
_Show_Pac	endp
@52@C674	label	word
	dw	@52@198
	dw	@52@226
	dw	@52@254
	dw	@52@282
@52@C626	label	word
	dw	@52@114
	dw	@52@422
	dw	@52@422
	dw	@52@450
	dw	@52@450
	dw	@52@478
	dw	@52@478
	dw	@52@506
	dw	@52@506
   ;	
   ;	void DotColide()
   ;	
	assume	cs:_TEXT
_DotColide	proc	near
	push	bp
	mov	bp,sp
	sub	sp,4
	push	si
   ;	
   ;	{
   ;		int c;
   ;		BYTE far *PFg=Player[Game.plr].PFg;
   ;	
	mov	ax,word ptr DGROUP:_Game+9
	mov	dx,14
	imul	dx
	mov	bx,ax
	mov	ax,word ptr DGROUP:_Player[bx+5]
	mov	dx,word ptr DGROUP:_Player[bx+3]
	mov	word ptr [bp-2],ax
	mov	word ptr [bp-4],dx
   ;	
   ;		if(PFg[Pac.m.y*PFXD+Pac.m.x]=='x')
   ;	
	mov	ax,word ptr DGROUP:_Pac+2
	mov	dx,31
	imul	dx
	add	ax,word ptr DGROUP:_Pac
	mov	es,word ptr [bp-2]
	add	ax,word ptr [bp-4]
	mov	bx,ax
	cmp	byte ptr es:[bx],120
	jne	short @53@86
   ;	
   ;		{
   ;			Player[Game.plr].PF_dots--;
   ;	
	mov	ax,word ptr DGROUP:_Game+9
	mov	dx,14
	imul	dx
	mov	bx,ax
	dec	word ptr DGROUP:_Player[bx+7]
   ;	
   ;			Player[Game.plr].score++;
   ;	
	mov	ax,word ptr DGROUP:_Game+9
	mov	dx,14
	imul	dx
	mov	bx,ax
	inc	word ptr DGROUP:_Player[bx+10]
   ;	
   ;			PFg[Pac.m.y*PFXD+Pac.m.x]='z';
   ;	
	mov	ax,word ptr DGROUP:_Pac+2
	mov	dx,31
	imul	dx
	add	ax,word ptr DGROUP:_Pac
	mov	es,word ptr [bp-2]
	add	ax,word ptr [bp-4]
	mov	bx,ax
	mov	byte ptr es:[bx],122
   ;	
   ;			SetSnd(50,1,1);
   ;	
	mov	ax,1
	push	ax
	mov	ax,1
	push	ax
	mov	ax,50
	push	ax
	call	near ptr _SetSnd
	add	sp,6
@53@86:
   ;	
   ;		}
   ;		if(PFg[Pac.m.y*PFXD+Pac.m.x]=='y')
   ;	
	mov	ax,word ptr DGROUP:_Pac+2
	mov	dx,31
	imul	dx
	add	ax,word ptr DGROUP:_Pac
	mov	es,word ptr [bp-2]
	add	ax,word ptr [bp-4]
	mov	bx,ax
	cmp	byte ptr es:[bx],121
	je	@@15
	jmp	@53@282
@@15:
   ;	
   ;		{
   ;			Player[Game.plr].score+=5;
   ;	
	mov	ax,word ptr DGROUP:_Game+9
	mov	dx,14
	imul	dx
	mov	bx,ax
	add	word ptr DGROUP:_Player[bx+10],5
   ;	
   ;			Player[Game.plr].PF_dots--;
   ;	
	mov	ax,word ptr DGROUP:_Game+9
	mov	dx,14
	imul	dx
	mov	bx,ax
	dec	word ptr DGROUP:_Player[bx+7]
   ;	
   ;			SetSnd(100,5,1);
   ;	
	mov	ax,1
	push	ax
	mov	ax,5
	push	ax
	mov	ax,100
	push	ax
	call	near ptr _SetSnd
	add	sp,6
   ;	
   ;			for(c=0;c<NG;c++)
   ;	
	xor	si,si
	jmp	short @53@226
@53@142:
   ;	
   ;			{
   ;				if(Ghost[c].m.st!=2)
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	cmp	byte ptr DGROUP:_Ghost[bx+10],2
	je	short @53@198
   ;	
   ;				{
   ;					Ghost[c].m.st=1;
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	mov	byte ptr DGROUP:_Ghost[bx+10],1
   ;	
   ;					Ghost[c].vc=vcm;
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	mov	ax,word ptr DGROUP:_vcm
	mov	word ptr DGROUP:_Ghost[bx+13],ax
@53@198:
	inc	si
@53@226:
	cmp	si,word ptr DGROUP:_NG
	jl	short @53@142
   ;	
   ;				}
   ;			}
   ;			PFg[Pac.m.y*PFXD+Pac.m.x]='z';
   ;	
	mov	ax,word ptr DGROUP:_Pac+2
	mov	dx,31
	imul	dx
	add	ax,word ptr DGROUP:_Pac
	mov	es,word ptr [bp-2]
	add	ax,word ptr [bp-4]
	mov	bx,ax
	mov	byte ptr es:[bx],122
@53@282:
   ;	
   ;		}
   ;	
   ;	}
   ;	
	pop	si
	mov	sp,bp
	pop	bp
	ret	
_DotColide	endp
   ;	
   ;	void GhostColide(int g)
   ;	
	assume	cs:_TEXT
_GhostColide	proc	near
	push	bp
	mov	bp,sp
	sub	sp,6
	push	si
	push	di
	mov	si,word ptr [bp+4]
   ;	
   ;	{
   ;		int gx,gy,px,py;
   ;		px=Pac.m.x*10+Pac.m.ox;
   ;	
	mov	ax,word ptr DGROUP:_Pac
	mov	dx,10
	imul	dx
	add	ax,word ptr DGROUP:_Pac+4
	mov	word ptr [bp-4],ax
   ;	
   ;		py=Pac.m.y*10+Pac.m.oy;
   ;	
	mov	ax,word ptr DGROUP:_Pac+2
	mov	dx,10
	imul	dx
	add	ax,word ptr DGROUP:_Pac+6
	mov	word ptr [bp-6],ax
   ;	
   ;		gx=Ghost[g].m.x*10+Ghost[g].m.ox;
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	mov	ax,word ptr DGROUP:_Ghost[bx]
	mov	dx,10
	imul	dx
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	add	ax,word ptr DGROUP:_Ghost[bx+4]
	mov	di,ax
   ;	
   ;		gy=Ghost[g].m.y*10+Ghost[g].m.oy;
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	mov	ax,word ptr DGROUP:_Ghost[bx+2]
	mov	dx,10
	imul	dx
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	add	ax,word ptr DGROUP:_Ghost[bx+6]
	mov	word ptr [bp-2],ax
   ;	
   ;		if((abs(px-gx)<5)&&(abs(py-gy)<5))
   ;	
	mov	ax,word ptr [bp-4]
	sub	ax,di
	jge	short @54@86
	mov	ax,word ptr [bp-4]
	neg	ax
	sub	ax,di
	jmp	short @54@114
@54@86:
	mov	ax,word ptr [bp-4]
	sub	ax,di
@54@114:
	cmp	ax,5
	jge	short @54@422
	mov	ax,word ptr [bp-6]
	sub	ax,word ptr [bp-2]
	jge	short @54@198
	mov	ax,word ptr [bp-6]
	neg	ax
	sub	ax,word ptr [bp-2]
	jmp	short @54@226
@54@198:
	mov	ax,word ptr [bp-6]
	sub	ax,word ptr [bp-2]
@54@226:
	cmp	ax,5
	jge	short @54@422
   ;	
   ;		switch(Ghost[g].m.st)
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	mov	al,byte ptr DGROUP:_Ghost[bx+10]
	cbw	
	or	ax,ax
	je	short @54@366
	cmp	ax,1
	je	short @54@394
	jmp	short @54@422
@54@366:
   ;	
   ;		{
   ;			case 0:Game.st=ANI;/*Player[Game.plr].pacs--;*/break;
   ;	
	mov	word ptr DGROUP:_Game,2
	jmp	short @54@422
@54@394:
   ;	
   ;			case 1:Ghost[g].m.st=2;Player[Game.plr].score+=10;SetSnd(500,10,1);break;
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	mov	byte ptr DGROUP:_Ghost[bx+10],2
	mov	ax,word ptr DGROUP:_Game+9
	mov	dx,14
	imul	dx
	mov	bx,ax
	add	word ptr DGROUP:_Player[bx+10],10
	mov	ax,1
	push	ax
	mov	ax,10
	push	ax
	mov	ax,500
	push	ax
	call	near ptr _SetSnd
	add	sp,6
	jmp	short @54@422
@54@422:
   ;	
   ;		}
   ;	}
   ;	
	pop	di
	pop	si
	mov	sp,bp
	pop	bp
	ret	
_GhostColide	endp
   ;	
   ;	void BaseColide(int g)
   ;	
	assume	cs:_TEXT
_BaseColide	proc	near
	push	bp
	mov	bp,sp
	sub	sp,6
	push	si
	push	di
	mov	si,word ptr [bp+4]
   ;	
   ;	{
   ;		int gx,gy,px,py;
   ;		px=GXI*10;
   ;	
	mov	ax,word ptr DGROUP:_GXI
	mov	dx,10
	imul	dx
	mov	word ptr [bp-4],ax
   ;	
   ;		py=GYI*10;
   ;	
	mov	ax,word ptr DGROUP:_GYI
	mov	dx,10
	imul	dx
	mov	word ptr [bp-6],ax
   ;	
   ;		gx=Ghost[g].m.x*10+Ghost[g].m.ox;
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	mov	ax,word ptr DGROUP:_Ghost[bx]
	mov	dx,10
	imul	dx
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	add	ax,word ptr DGROUP:_Ghost[bx+4]
	mov	di,ax
   ;	
   ;		gy=Ghost[g].m.y*10+Ghost[g].m.oy;
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	mov	ax,word ptr DGROUP:_Ghost[bx+2]
	mov	dx,10
	imul	dx
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	add	ax,word ptr DGROUP:_Ghost[bx+6]
	mov	word ptr [bp-2],ax
   ;	
   ;		if((abs(px-gx)<5)&&(abs(py-gy)<5))
   ;	
	mov	ax,word ptr [bp-4]
	sub	ax,di
	jge	short @55@86
	mov	ax,word ptr [bp-4]
	neg	ax
	sub	ax,di
	jmp	short @55@114
@55@86:
	mov	ax,word ptr [bp-4]
	sub	ax,di
@55@114:
	cmp	ax,5
	jge	short @55@310
	mov	ax,word ptr [bp-6]
	sub	ax,word ptr [bp-2]
	jge	short @55@198
	mov	ax,word ptr [bp-6]
	neg	ax
	sub	ax,word ptr [bp-2]
	jmp	short @55@226
@55@198:
	mov	ax,word ptr [bp-6]
	sub	ax,word ptr [bp-2]
@55@226:
	cmp	ax,5
	jge	short @55@310
   ;	
   ;		if(Ghost[g].m.st==2)
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	cmp	byte ptr DGROUP:_Ghost[bx+10],2
	jne	short @55@310
   ;	
   ;		{
   ;			Ghost[g].m.st=0;
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	mov	byte ptr DGROUP:_Ghost[bx+10],0
@55@310:
   ;	
   ;		}
   ;	}
   ;	
	pop	di
	pop	si
	mov	sp,bp
	pop	bp
	ret	
_BaseColide	endp
   ;	
   ;	int TestDir(char dir,TPl *m)
   ;	
	assume	cs:_TEXT
_TestDir	proc	near
	push	bp
	mov	bp,sp
	sub	sp,4
	push	si
	push	di
	mov	cx,word ptr [bp+6]
   ;	
   ;	{
   ;		int x=m->x,y=m->y;
   ;	
	mov	bx,cx
	mov	si,word ptr [bx]
	mov	bx,cx
	mov	di,word ptr [bx+2]
   ;	
   ;		BYTE far *PFg=Player[Game.plr].PFg;
   ;	
	mov	ax,word ptr DGROUP:_Game+9
	mov	dx,14
	imul	dx
	mov	bx,ax
	mov	ax,word ptr DGROUP:_Player[bx+5]
	mov	dx,word ptr DGROUP:_Player[bx+3]
	mov	word ptr [bp-2],ax
	mov	word ptr [bp-4],dx
   ;	
   ;		if(dir==STOP)
   ;	
	cmp	byte ptr [bp+4],0
	jne	short @56@114
@56@58:
   ;	
   ;			return 0;
   ;	
	xor	ax,ax
@56@86:
	jmp	@56@1010
@56@114:
   ;	
   ;		if((x<0)||(x>=PFXD)||(y<0)||(y>=PFYD))
   ;	
	or	si,si
	jl	short @56@226
	cmp	si,31
	jge	short @56@226
	or	di,di
	jl	short @56@226
	cmp	di,17
	jl	short @56@254
@56@226:
   ;	
   ;			return 0;
   ;	
	jmp	short @56@58
@56@254:
   ;	
   ;		switch(dir)
   ;	
	mov	al,byte ptr [bp+4]
	cbw	
	dec	ax
	mov	bx,ax
	cmp	bx,3
	ja	short @56@674
	shl	bx,1
	jmp	word ptr cs:@56@C914[bx]
@56@338:
   ;	
   ;		{
   ;			case UP:if(y>0) y--;break;
   ;	
	or	di,di
	jle	short @56@394
	dec	di
@56@394:
	jmp	short @56@674
@56@422:
   ;	
   ;			case DOWN:if(y<PFYD-1) y++;break;
   ;	
	cmp	di,16
	jge	short @56@478
	inc	di
@56@478:
	jmp	short @56@674
@56@506:
   ;	
   ;			case LEFT:if(x>0) x--;break;
   ;	
	or	si,si
	jle	short @56@562
	dec	si
@56@562:
	jmp	short @56@674
@56@590:
   ;	
   ;			case RIGHT:if(x<PFXD-1) x++;break;
   ;	
	cmp	si,30
	jge	short @56@646
	inc	si
@56@646:
	jmp	short @56@674
@56@674:
   ;	
   ;		}
   ;		switch(PFg[y*PFXD+x])
   ;	
	mov	ax,di
	mov	dx,31
	imul	dx
	add	ax,si
	mov	es,word ptr [bp-2]
	add	ax,word ptr [bp-4]
	mov	bx,ax
	mov	al,byte ptr es:[bx]
	mov	ah,0
	sub	ax,114
	mov	bx,ax
	cmp	bx,8
	ja	short @56@954
	shl	bx,1
	jmp	word ptr cs:@56@C962[bx]
@56@758:
	jmp	short @56@842
   ;	
   ;		{
   ;			case 'x':
   ;			case 'y':
   ;			case 'z':return 1;break;
   ;	
	jmp	short @56@982
@56@814:
   ;	
   ;			case 'r':if(m->t==GHOST)
   ;	
	mov	bx,cx
	cmp	byte ptr [bx+9],1
	jne	short @56@898
@56@842:
   ;	
   ;							return 1;
   ;	
	mov	ax,1
	jmp	@56@86
	jmp	short @56@926
@56@898:
   ;	
   ;						else
   ;							return 0;
   ;	
	jmp	@56@58
@56@926:
   ;	
   ;					  break;
   ;	
	jmp	short @56@982
@56@954:
   ;	
   ;			default:return 0;
   ;	
	jmp	@56@58
@56@982:
	jmp	@56@58
@56@1010:
   ;	
   ;		}
   ;		return 0;
   ;	}
   ;	
	pop	di
	pop	si
	mov	sp,bp
	pop	bp
	ret	
_TestDir	endp
@56@C962	label	word
	dw	@56@814
	dw	@56@954
	dw	@56@954
	dw	@56@954
	dw	@56@954
	dw	@56@954
	dw	@56@758
	dw	@56@758
	dw	@56@758
@56@C914	label	word
	dw	@56@338
	dw	@56@422
	dw	@56@590
	dw	@56@506
   ;	
   ;	void Move(TPl *m)
   ;	
	assume	cs:_TEXT
_Move	proc	near
	push	bp
	mov	bp,sp
	sub	sp,2
	push	si
	push	di
	mov	si,word ptr [bp+4]
   ;	
   ;	{
   ;			int d1=1,d2=1;
   ;	
	mov	di,1
	mov	word ptr [bp-2],1
   ;	
   ;			if(m->v)
   ;	
	cmp	word ptr [si+11],0
	je	short @57@114
   ;	
   ;			{
   ;				if(Game.count%m->v==0)
   ;	
	mov	ax,word ptr DGROUP:_Game+2
	cwd	
	idiv	word ptr [si+11]
	or	dx,dx
	jne	short @57@114
   ;	
   ;					d2=d1=2;
   ;	
	mov	ax,2
	mov	di,ax
	mov	word ptr [bp-2],ax
@57@114:
   ;	
   ;			}
   ;			if(m->ox%2)
   ;	
	mov	ax,word ptr [si+4]
	mov	bx,2
	cwd	
	idiv	bx
	or	dx,dx
	je	short @57@170
   ;	
   ;				d1=1;
   ;	
	mov	di,1
@57@170:
   ;	
   ;			if(m->oy%2)
   ;	
	mov	ax,word ptr [si+6]
	mov	bx,2
	cwd	
	idiv	bx
	or	dx,dx
	je	short @57@226
   ;	
   ;				d2=1;
   ;	
	mov	word ptr [bp-2],1
@57@226:
   ;	
   ;			switch(m->dir)
   ;	
	mov	al,byte ptr [si+8]
	cbw	
	dec	ax
	mov	bx,ax
	cmp	bx,3
	ja	short @57@422
	shl	bx,1
	jmp	word ptr cs:@57@C1042[bx]
@57@310:
   ;	
   ;			{
   ;				case LEFT:m->ox-=d1;break;
   ;	
	sub	word ptr [si+4],di
	jmp	short @57@422
@57@338:
   ;	
   ;				case RIGHT:m->ox+=d1;break;
   ;	
	add	word ptr [si+4],di
	jmp	short @57@422
@57@366:
   ;	
   ;				case UP:m->oy-=d2;break;
   ;	
	mov	ax,word ptr [bp-2]
	sub	word ptr [si+6],ax
	jmp	short @57@422
@57@394:
   ;	
   ;				case DOWN:m->oy+=d2;break;
   ;	
	mov	ax,word ptr [bp-2]
	add	word ptr [si+6],ax
	jmp	short @57@422
@57@422:
   ;	
   ;			}
   ;			if(m->ox<-4)
   ;	
	cmp	word ptr [si+4],65532
	jge	short @57@478
   ;	
   ;			{
   ;					m->x--;
   ;	
	dec	word ptr [si]
   ;	
   ;					m->ox+=10;
   ;	
	add	word ptr [si+4],10
@57@478:
   ;	
   ;			}
   ;			if(m->oy<-4)
   ;	
	cmp	word ptr [si+6],65532
	jge	short @57@534
   ;	
   ;			{
   ;					m->y--;
   ;	
	dec	word ptr [si+2]
   ;	
   ;					m->oy+=10;
   ;	
	add	word ptr [si+6],10
@57@534:
   ;	
   ;			}
   ;			if(m->oy>5)
   ;	
	cmp	word ptr [si+6],5
	jle	short @57@590
   ;	
   ;			{
   ;					m->y++;
   ;	
	inc	word ptr [si+2]
   ;	
   ;					m->oy-=10;
   ;	
	sub	word ptr [si+6],10
@57@590:
   ;	
   ;			}
   ;			if(m->ox>5)
   ;	
	cmp	word ptr [si+4],5
	jle	short @57@646
   ;	
   ;			{
   ;					m->x++;
   ;	
	inc	word ptr [si]
   ;	
   ;					m->ox-=10;
   ;	
	sub	word ptr [si+4],10
@57@646:
   ;	
   ;			}
   ;			if(m->y<0)
   ;	
	cmp	word ptr [si+2],0
	jge	short @57@702
   ;	
   ;				m->y=16;
   ;	
	mov	word ptr [si+2],16
@57@702:
   ;	
   ;			if(m->y>=PFYD)
   ;	
	cmp	word ptr [si+2],17
	jl	short @57@758
   ;	
   ;				m->y=0;
   ;	
	mov	word ptr [si+2],0
@57@758:
   ;	
   ;			switch(m->dir)
   ;	
	mov	al,byte ptr [si+8]
	cbw	
	dec	ax
	mov	bx,ax
	cmp	bx,3
	ja	short @57@1066
	shl	bx,1
	jmp	word ptr cs:@57@C1090[bx]
@57@842:
   ;	
   ;			{
   ;				case LEFT:
   ;				case RIGHT:if(m->ox==0)
   ;	
	cmp	word ptr [si+4],0
	jne	short @57@926
   ;	
   ;									if(!TestDir(m->dir,m)) m->dir=STOP;
   ;	
	push	si
	mov	al,byte ptr [si+8]
	push	ax
	call	near ptr _TestDir
	pop	cx
	pop	cx
	or	ax,ax
	jne	short @57@926
	mov	byte ptr [si+8],0
@57@926:
   ;	
   ;								break;
   ;	
	jmp	short @57@1066
@57@954:
   ;	
   ;				case UP:
   ;				case DOWN:if(m->oy==0)
   ;	
	cmp	word ptr [si+6],0
	jne	short @57@1038
   ;	
   ;									if(!TestDir(m->dir,m)) m->dir=STOP;
   ;	
	push	si
	mov	al,byte ptr [si+8]
	push	ax
	call	near ptr _TestDir
	pop	cx
	pop	cx
	or	ax,ax
	jne	short @57@1038
	mov	byte ptr [si+8],0
@57@1038:
   ;	
   ;								break;
   ;	
	jmp	short @57@1066
@57@1066:
   ;	
   ;			}
   ;	}
   ;	
	pop	di
	pop	si
	mov	sp,bp
	pop	bp
	ret	
_Move	endp
@57@C1090	label	word
	dw	@57@954
	dw	@57@954
	dw	@57@842
	dw	@57@842
@57@C1042	label	word
	dw	@57@366
	dw	@57@394
	dw	@57@338
	dw	@57@310
   ;	
   ;	void GameInit()
   ;	
	assume	cs:_TEXT
_GameInit	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;	
   ;		Player[0].score=0;
   ;	
	mov	word ptr DGROUP:_Player+10,0
   ;	
   ;		Player[1].score=0;
   ;	
	mov	word ptr DGROUP:_Player+24,0
   ;	
   ;		Player[0].OVER=0;
   ;	
	mov	byte ptr DGROUP:_Player+9,0
   ;	
   ;		Player[1].OVER=0;
   ;	
	mov	byte ptr DGROUP:_Player+23,0
   ;	
   ;		Player[0].level=-1;
   ;	
	mov	word ptr DGROUP:_Player,65535
   ;	
   ;		Player[1].level=-1;
   ;	
	mov	word ptr DGROUP:_Player+14,65535
   ;	
   ;		Player[0].pacs=3;
   ;	
	mov	word ptr DGROUP:_Player+12,3
   ;	
   ;		Player[1].pacs=4;
   ;	
	mov	word ptr DGROUP:_Player+26,4
   ;	
   ;		Player[0].flagl=0;
   ;	
	mov	byte ptr DGROUP:_Player+2,0
   ;	
   ;		Player[1].flagl=0;
   ;	
	mov	byte ptr DGROUP:_Player+16,0
   ;	
   ;		Game.count2=0;
   ;	
	mov	word ptr DGROUP:_Game+4,0
   ;	
   ;		Game.bonus=0;
   ;	
	mov	byte ptr DGROUP:_Game+6,0
   ;	
   ;		Game.plr=0;
   ;	
	mov	word ptr DGROUP:_Game+9,0
   ;	
   ;		InitLevel(stl);
   ;	
	push	word ptr DGROUP:_stl
	call	near ptr _InitLevel
	pop	cx
   ;	
   ;	}
   ;	
	pop	bp
	ret	
_GameInit	endp
   ;	
   ;	void InitLevel(int l)
   ;	
	assume	cs:_TEXT
_InitLevel	proc	near
	push	bp
	mov	bp,sp
	push	si
	mov	si,word ptr [bp+4]
   ;	
   ;	{
   ;		CpyPFg(Player[Game.plr].PFg,LPFg);
   ;	
	push	word ptr DGROUP:_LPFg+2
	push	word ptr DGROUP:_LPFg
	mov	ax,word ptr DGROUP:_Game+9
	mov	dx,14
	imul	dx
	mov	bx,ax
	push	word ptr DGROUP:_Player[bx+5]
	push	word ptr DGROUP:_Player[bx+3]
	call	near ptr _CpyPFg
	add	sp,8
   ;	
   ;		Player[Game.plr].PF_dots=PF_dotsL;
   ;	
	mov	ax,word ptr DGROUP:_Game+9
	mov	dx,14
	imul	dx
	mov	dx,word ptr DGROUP:_PF_dotsL
	mov	bx,ax
	mov	word ptr DGROUP:_Player[bx+7],dx
   ;	
   ;		SetSnd(0,0,0);
   ;	
	xor	ax,ax
	push	ax
	xor	ax,ax
	push	ax
	xor	ax,ax
	push	ax
	call	near ptr _SetSnd
	add	sp,6
   ;	
   ;		if(l<0) l=0;
   ;	
	or	si,si
	jge	short @59@86
	xor	si,si
@59@86:
   ;	
   ;		if(l>9) l=9;
   ;	
	cmp	si,9
	jle	short @59@142
	mov	si,9
@59@142:
   ;	
   ;		Player[Game.plr].level=l;
   ;	
	mov	ax,word ptr DGROUP:_Game+9
	mov	dx,14
	imul	dx
	mov	bx,ax
	mov	word ptr DGROUP:_Player[bx],si
   ;	
   ;		PlReset();
   ;	
	call	near ptr _PlReset
   ;	
   ;	}
   ;	
	pop	si
	pop	bp
	ret	
_InitLevel	endp
   ;	
   ;	void NextLevel()
   ;	
	assume	cs:_TEXT
_NextLevel	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;		if(Player[Game.plr].flagl)
   ;	
	mov	ax,word ptr DGROUP:_Game+9
	mov	dx,14
	imul	dx
	mov	bx,ax
	cmp	byte ptr DGROUP:_Player[bx+2],0
	je	short @60@142
   ;	
   ;		{
   ;			if(Player[Game.plr].pacs<3)
   ;	
	mov	ax,word ptr DGROUP:_Game+9
	mov	dx,14
	imul	dx
	mov	bx,ax
	cmp	word ptr DGROUP:_Player[bx+12],3
	jge	short @60@114
   ;	
   ;				Player[Game.plr].pacs++;
   ;	
	mov	ax,word ptr DGROUP:_Game+9
	mov	dx,14
	imul	dx
	mov	bx,ax
	inc	word ptr DGROUP:_Player[bx+12]
@60@114:
   ;	
   ;			Player[Game.plr].flagl=0;
   ;	
	mov	ax,word ptr DGROUP:_Game+9
	mov	dx,14
	imul	dx
	mov	bx,ax
	mov	byte ptr DGROUP:_Player[bx+2],0
   ;	
   ;		}
   ;	
	jmp	short @60@170
@60@142:
   ;	
   ;		else
   ;			Player[Game.plr].flagl=1;
   ;	
	mov	ax,word ptr DGROUP:_Game+9
	mov	dx,14
	imul	dx
	mov	bx,ax
	mov	byte ptr DGROUP:_Player[bx+2],1
@60@170:
   ;	
   ;		InitLevel(Player[Game.plr].level+1);
   ;	
	mov	ax,word ptr DGROUP:_Game+9
	mov	dx,14
	imul	dx
	mov	bx,ax
	mov	ax,word ptr DGROUP:_Player[bx]
	inc	ax
	push	ax
	call	near ptr _InitLevel
	pop	cx
   ;	
   ;	}
   ;	
	pop	bp
	ret	
_NextLevel	endp
   ;	
   ;	void SndC()
   ;	
	assume	cs:_TEXT
_SndC	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;		if(Snd.count)
   ;	
	cmp	word ptr DGROUP:_Snd,0
	je	short @61@422
   ;	
   ;		{
   ;	
   ;			if(Snd.count-1<=0)
   ;	
	mov	ax,word ptr DGROUP:_Snd
	dec	ax
	jg	short @61@114
   ;	
   ;				nosound();
   ;	
	call	near ptr _nosound
	jmp	short @61@394
@61@114:
   ;	
   ;			else if(som)
   ;	
	cmp	word ptr DGROUP:_som,0
	je	short @61@394
   ;	
   ;				switch(Snd.Bip)
   ;	
	mov	ax,word ptr DGROUP:_Snd+4
	cmp	ax,1
	je	short @61@254
	cmp	ax,2
	je	short @61@338
	jmp	short @61@366
@61@254:
   ;	
   ;				{
   ;					case 1:sound(Snd.freq*Snd.count);break;
   ;	
	mov	ax,word ptr DGROUP:_Snd+2
	imul	word ptr DGROUP:_Snd
@61@282:
	push	ax
@61@310:
	call	near ptr _sound
	pop	cx
	jmp	short @61@394
@61@338:
   ;	
   ;					case 2:sound(Snd.freq/Snd.count);break;
   ;	
	mov	ax,word ptr DGROUP:_Snd+2
	cwd	
	idiv	word ptr DGROUP:_Snd
	jmp	short @61@282
@61@366:
   ;	
   ;					default:sound(Snd.freq);break;
   ;	
	push	word ptr DGROUP:_Snd+2
	jmp	short @61@310
@61@394:
   ;	
   ;				}
   ;			Snd.count--;
   ;	
	dec	word ptr DGROUP:_Snd
@61@422:
   ;	
   ;		}
   ;		if(Snd.count<0)
   ;	
	cmp	word ptr DGROUP:_Snd,0
	jge	short @61@478
   ;	
   ;			Snd.count=0;
   ;	
	mov	word ptr DGROUP:_Snd,0
@61@478:
   ;	
   ;	
   ;	
   ;	}
   ;	
	pop	bp
	ret	
_SndC	endp
   ;	
   ;	void SetSnd(int freq,int count, int bip)
   ;	
	assume	cs:_TEXT
_SetSnd	proc	near
	push	bp
	mov	bp,sp
	push	si
	push	di
	mov	di,word ptr [bp+4]
	mov	si,word ptr [bp+6]
   ;	
   ;	{
   ;		if(count<Snd.count)
   ;	
	cmp	si,word ptr DGROUP:_Snd
	jge	short @62@86
   ;	
   ;			return;
   ;	
	jmp	short @62@282
@62@86:
   ;	
   ;		Snd.freq=abs(freq);
   ;	
	or	di,di
	jge	short @62@142
	mov	ax,di
	neg	ax
	jmp	short @62@170
@62@142:
	mov	ax,di
@62@170:
	mov	word ptr DGROUP:_Snd+2,ax
   ;	
   ;		Snd.count=abs(count)+1;
   ;	
	or	si,si
	jge	short @62@226
	mov	ax,si
	neg	ax
	jmp	short @62@254
@62@226:
	mov	ax,si
@62@254:
	inc	ax
	mov	word ptr DGROUP:_Snd,ax
   ;	
   ;		Snd.Bip=bip;
   ;	
	mov	ax,word ptr [bp+8]
	mov	word ptr DGROUP:_Snd+4,ax
@62@282:
   ;	
   ;	}
   ;	
	pop	di
	pop	si
	pop	bp
	ret	
_SetSnd	endp
   ;	
   ;	void PlReset()
   ;	
	assume	cs:_TEXT
_PlReset	proc	near
	push	bp
	mov	bp,sp
	push	si
   ;	
   ;	{
   ;		int c;
   ;		Game.st=WAIT;
   ;	
	mov	word ptr DGROUP:_Game,0
   ;	
   ;		Game.ast=0;
   ;	
	mov	word ptr DGROUP:_Game+7,0
   ;	
   ;		Game.count=0;
   ;	
	mov	word ptr DGROUP:_Game+2,0
   ;	
   ;		Pac.m.st=3;
   ;	
	mov	byte ptr DGROUP:_Pac+10,3
   ;	
   ;		Pac.m.dir=STOP;
   ;	
	mov	byte ptr DGROUP:_Pac+8,0
   ;	
   ;		Pac.m.x=15;
   ;	
	mov	word ptr DGROUP:_Pac,15
   ;	
   ;		Pac.m.y=13;
   ;	
	mov	word ptr DGROUP:_Pac+2,13
   ;	
   ;		Pac.m.v=nivel[Player[Game.plr].level][2];
   ;	
	mov	ax,word ptr DGROUP:_Game+9
	mov	dx,14
	imul	dx
	mov	bx,ax
	mov	bx,word ptr DGROUP:_Player[bx]
	shl	bx,1
	shl	bx,1
	shl	bx,1
	mov	ax,word ptr DGROUP:_nivel[bx+4]
	mov	word ptr DGROUP:_Pac+11,ax
   ;	
   ;		vcm=nivel[Player[Game.plr].level][0];
   ;	
	mov	ax,word ptr DGROUP:_Game+9
	mov	dx,14
	imul	dx
	mov	bx,ax
	mov	bx,word ptr DGROUP:_Player[bx]
	shl	bx,1
	shl	bx,1
	shl	bx,1
	mov	ax,word ptr DGROUP:_nivel[bx]
	mov	word ptr DGROUP:_vcm,ax
   ;	
   ;		NG=nivel[Player[Game.plr].level][1];
   ;	
	mov	ax,word ptr DGROUP:_Game+9
	mov	dx,14
	imul	dx
	mov	bx,ax
	mov	bx,word ptr DGROUP:_Player[bx]
	shl	bx,1
	shl	bx,1
	shl	bx,1
	mov	ax,word ptr DGROUP:_nivel[bx+2]
	mov	word ptr DGROUP:_NG,ax
   ;	
   ;		for(c=0;c<NG;c++)
   ;	
	xor	si,si
	jmp	@63@114
@63@58:
   ;	
   ;		{
   ;			Ghost[c].m.st=0;
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	mov	byte ptr DGROUP:_Ghost[bx+10],0
   ;	
   ;			Ghost[c].m.x=15;
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	mov	word ptr DGROUP:_Ghost[bx],15
   ;	
   ;			Ghost[c].m.y=7;
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	mov	word ptr DGROUP:_Ghost[bx+2],7
   ;	
   ;			Ghost[c].m.ox=0;
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	mov	word ptr DGROUP:_Ghost[bx+4],0
   ;	
   ;			Ghost[c].m.oy=0;
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	mov	word ptr DGROUP:_Ghost[bx+6],0
   ;	
   ;			Ghost[c].m.dir=STOP;
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	mov	byte ptr DGROUP:_Ghost[bx+8],0
   ;	
   ;			Ghost[c].m.v=nivel[Player[Game.plr].level][3];
   ;	
	mov	ax,word ptr DGROUP:_Game+9
	mov	dx,14
	imul	dx
	mov	bx,ax
	mov	bx,word ptr DGROUP:_Player[bx]
	shl	bx,1
	shl	bx,1
	shl	bx,1
	mov	ax,word ptr DGROUP:_nivel[bx+6]
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	mov	word ptr DGROUP:_Ghost[bx+11],ax
   ;	
   ;			Ghost[c].ld=H;
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	mov	byte ptr DGROUP:_Ghost[bx+15],0
	inc	si
@63@114:
	cmp	si,word ptr DGROUP:_NG
	jge	@@16
	jmp	@63@58
@@16:
   ;	
   ;		}
   ;		Pac.m.ox=0;
   ;	
	mov	word ptr DGROUP:_Pac+4,0
   ;	
   ;		Pac.m.oy=0;
   ;	
	mov	word ptr DGROUP:_Pac+6,0
   ;	
   ;	}
   ;	
	pop	si
	pop	bp
	ret	
_PlReset	endp
   ;	
   ;	void SetDir(char dir,TPl *m)
   ;	
	assume	cs:_TEXT
_SetDir	proc	near
	push	bp
	mov	bp,sp
	push	si
	mov	si,word ptr [bp+6]
   ;	
   ;	{
   ;		switch(dir)
   ;	
	mov	al,byte ptr [bp+4]
	cbw	
	dec	ax
	mov	bx,ax
	cmp	bx,3
	ja	short @64@338
	shl	bx,1
	jmp	word ptr cs:@64@C546[bx]
@64@114:
   ;	
   ;		{
   ;			case LEFT:
   ;			case RIGHT:if(m->oy==0)
   ;	
	cmp	word ptr [si+6],0
	jne	short @64@198
   ;	
   ;								if(TestDir(dir,m)) m->dir=dir;
   ;	
	push	si
	mov	al,byte ptr [bp+4]
	push	ax
	call	near ptr _TestDir
	pop	cx
	pop	cx
	or	ax,ax
	je	short @64@198
	mov	al,byte ptr [bp+4]
	mov	byte ptr [si+8],al
@64@198:
   ;	
   ;							break;
   ;	
	jmp	short @64@338
@64@226:
   ;	
   ;			case UP:
   ;			case DOWN:if(m->ox==0)
   ;	
	cmp	word ptr [si+4],0
	jne	short @64@310
   ;	
   ;								if(TestDir(dir,m)) m->dir=dir;
   ;	
	push	si
	mov	al,byte ptr [bp+4]
	push	ax
	call	near ptr _TestDir
	pop	cx
	pop	cx
	or	ax,ax
	je	short @64@310
	mov	al,byte ptr [bp+4]
	mov	byte ptr [si+8],al
@64@310:
   ;	
   ;							break;
   ;	
	jmp	short @64@338
@64@338:
   ;	
   ;		}
   ;		if(m->t==PAC)
   ;	
	cmp	byte ptr [si+9],0
	jne	short @64@562
   ;	
   ;			switch(m->dir)
   ;	
	mov	al,byte ptr [si+8]
	cbw	
	cmp	ax,3
	je	short @64@534
	cmp	ax,4
	je	short @64@478
	jmp	short @64@562
@64@478:
   ;	
   ;			{
   ;				case LEFT:m->st=m->st|0x2;break;
   ;	
	mov	al,byte ptr [si+10]
	or	al,2
@64@506:
	mov	byte ptr [si+10],al
	jmp	short @64@562
@64@534:
   ;	
   ;				case RIGHT:m->st=m->st&(~(0x2));break;
   ;	
	mov	al,byte ptr [si+10]
	and	al,253
	jmp	short @64@506
@64@562:
   ;	
   ;			}
   ;	}
   ;	
	pop	si
	pop	bp
	ret	
_SetDir	endp
@64@C546	label	word
	dw	@64@226
	dw	@64@226
	dw	@64@114
	dw	@64@114
   ;	
   ;	void ShowBonus()
   ;	
	assume	cs:_TEXT
_ShowBonus	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;		if(Game.bonus)
   ;	
	cmp	byte ptr DGROUP:_Game+6,0
	je	short @65@86
   ;	
   ;		{
   ;			DrawPic(BX*10+PFXI,BY*10+PFYI,Bp);
   ;	
	push	word ptr DGROUP:_Bp+2
	push	word ptr DGROUP:_Bp
	mov	ax,110
	push	ax
	mov	ax,155
	push	ax
	call	near ptr _DrawPic
	add	sp,8
@65@86:
   ;	
   ;		}
   ;	}
   ;	
	pop	bp
	ret	
_ShowBonus	endp
   ;	
   ;	void ControlBonus()
   ;	
	assume	cs:_TEXT
_ControlBonus	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;		if(Game.count2%2000==1500)
   ;	
	mov	ax,word ptr DGROUP:_Game+4
	mov	bx,2000
	xor	dx,dx
	div	bx
	cmp	dx,1500
	jne	short @66@86
   ;	
   ;		{
   ;			Game.bonus=1;
   ;	
	mov	byte ptr DGROUP:_Game+6,1
@66@86:
   ;	
   ;		}
   ;		if((Game.bonus)&&(Game.count2%2000<1500))
   ;	
	cmp	byte ptr DGROUP:_Game+6,0
	je	short @66@170
	mov	ax,word ptr DGROUP:_Game+4
	mov	bx,2000
	xor	dx,dx
	div	bx
	cmp	dx,1500
	jae	short @66@170
   ;	
   ;			Game.bonus=0;
   ;	
	mov	byte ptr DGROUP:_Game+6,0
@66@170:
   ;	
   ;	}
   ;	
	pop	bp
	ret	
_ControlBonus	endp
   ;	
   ;	void BonusColide()
   ;	
	assume	cs:_TEXT
_BonusColide	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;		if(!Game.bonus)
   ;	
	mov	al,byte ptr DGROUP:_Game+6
	cbw	
	or	ax,ax
	jne	short @67@86
   ;	
   ;			return;
   ;	
	jmp	short @67@170
@67@86:
   ;	
   ;		if((Pac.m.x==BX)&&(Pac.m.y==BY))
   ;	
	cmp	word ptr DGROUP:_Pac,15
	jne	short @67@170
	cmp	word ptr DGROUP:_Pac+2,11
	jne	short @67@170
   ;	
   ;		{
   ;			Player[Game.plr].score+=20;
   ;	
	mov	ax,word ptr DGROUP:_Game+9
	mov	dx,14
	imul	dx
	mov	bx,ax
	add	word ptr DGROUP:_Player[bx+10],20
   ;	
   ;			Game.bonus=0;
   ;	
	mov	byte ptr DGROUP:_Game+6,0
   ;	
   ;			SetSnd(500,5,1);
   ;	
	mov	ax,1
	push	ax
	mov	ax,5
	push	ax
	mov	ax,500
	push	ax
	call	near ptr _SetSnd
	add	sp,6
@67@170:
   ;	
   ;		}
   ;	}
   ;	
	pop	bp
	ret	
_BonusColide	endp
   ;	
   ;	void ShowPopUp()
   ;	
	assume	cs:_TEXT
_ShowPopUp	proc	near
	push	bp
	mov	bp,sp
	sub	sp,4
   ;	
   ;	{
   ;		char st[4];
   ;		st[3]=0;
   ;	
	mov	byte ptr [bp-1],0
   ;	
   ;		if(Game.st!=PAUSA)
   ;	
	cmp	word ptr DGROUP:_Game,4
	je	short @68@86
   ;	
   ;			return;
   ;	
	jmp	@68@450
@68@86:
   ;	
   ;		FillColor=MkColor(2,2,1);
   ;	
	mov	byte ptr DGROUP:_FillColor,73
   ;	
   ;		Color=MkColor(0,5,0);
   ;	
	mov	byte ptr DGROUP:_Color,20
   ;	
   ;		DrawRect(115,55,205,109,LIFI);
   ;	
	mov	al,0
	push	ax
	mov	ax,109
	push	ax
	mov	ax,205
	push	ax
	mov	ax,55
	push	ax
	mov	ax,115
	push	ax
	call	near ptr _DrawRect
	add	sp,10
   ;	
   ;		TextColor=MkColor(0,0,3);
   ;	
	mov	byte ptr DGROUP:_TextColor,3
   ;	
   ;		writest(125,63,"CONTINUAR");
   ;	
	mov	ax,offset DGROUP:s@+574
	push	ax
	mov	ax,63
	push	ax
	mov	ax,125
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;		writest(125,74,"MUSICA");
   ;	
	mov	ax,offset DGROUP:s@+584
	push	ax
	mov	ax,74
	push	ax
	mov	ax,125
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;		writest(125,85,"SOM");
   ;	
	mov	ax,offset DGROUP:s@+591
	push	ax
	mov	ax,85
	push	ax
	mov	ax,125
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;		writest(125,96,"TERMINAR");
   ;	
	mov	ax,offset DGROUP:s@+595
	push	ax
	mov	ax,96
	push	ax
	mov	ax,125
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;		switch(musica)
   ;	
	mov	ax,word ptr DGROUP:_musica
	or	ax,ax
	je	short @68@254
	cmp	ax,1
	je	short @68@282
	cmp	ax,2
	je	short @68@310
	jmp	short @68@338
@68@254:
   ;	
   ;		{
   ;				case 0:TextColor=MkColor(5,5,2);st[0]='N';st[1]='/';st[2]='A';break;
   ;	
	mov	byte ptr DGROUP:_TextColor,182
	mov	byte ptr [bp-4],78
	mov	byte ptr [bp-3],47
	mov	byte ptr [bp-2],65
	jmp	short @68@338
@68@282:
   ;	
   ;				case 1:TextColor=MkColor(5,0,0);st[0]='O';st[1]='F';st[2]='F';break;
   ;	
	mov	byte ptr DGROUP:_TextColor,160
	mov	byte ptr [bp-4],79
	mov	byte ptr [bp-3],70
	mov	byte ptr [bp-2],70
	jmp	short @68@338
@68@310:
   ;	
   ;				case 2:TextColor=MkColor(0,5,0);st[0]=' ';st[1]='O';st[2]='N ';break;
   ;	
	mov	byte ptr DGROUP:_TextColor,20
	mov	byte ptr [bp-4],32
	mov	byte ptr [bp-3],79
	mov	byte ptr [bp-2],78
	jmp	short @68@338
@68@338:
   ;	
   ;		}
   ;		writest(173,74,st);
   ;	
	lea	ax,word ptr [bp-4]
	push	ax
	mov	ax,74
	push	ax
	mov	ax,173
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;		if(som)
   ;	
	cmp	word ptr DGROUP:_som,0
	je	short @68@394
   ;	
   ;		{
   ;			TextColor=MkColor(0,5,0);st[0]=' ';st[1]='O';st[2]='N ';
   ;	
	mov	byte ptr DGROUP:_TextColor,20
	mov	byte ptr [bp-4],32
	mov	byte ptr [bp-3],79
	mov	byte ptr [bp-2],78
   ;	
   ;		}
   ;	
	jmp	short @68@422
@68@394:
   ;	
   ;		else
   ;		{
   ;			TextColor=MkColor(5,0,0);st[0]='O';st[1]='F';st[2]='F';
   ;	
	mov	byte ptr DGROUP:_TextColor,160
	mov	byte ptr [bp-4],79
	mov	byte ptr [bp-3],70
	mov	byte ptr [bp-2],70
@68@422:
   ;	
   ;		}
   ;		writest(170,85,st);
   ;	
	lea	ax,word ptr [bp-4]
	push	ax
	mov	ax,85
	push	ax
	mov	ax,170
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;		Color=MkColor(0,6,0);
   ;	
	mov	byte ptr DGROUP:_Color,24
   ;	
   ;		DrawRect(118,61+11*PopUp,202,71+11*PopUp,LINE);
   ;	
	mov	al,1
	push	ax
	mov	ax,word ptr DGROUP:_PopUp
	mov	dx,11
	imul	dx
	add	ax,71
	push	ax
	mov	ax,202
	push	ax
	mov	ax,word ptr DGROUP:_PopUp
	mov	dx,11
	imul	dx
	add	ax,61
	push	ax
	mov	ax,118
	push	ax
	call	near ptr _DrawRect
	add	sp,10
@68@450:
   ;	
   ;	}
   ;	
	mov	sp,bp
	pop	bp
	ret	
_ShowPopUp	endp
   ;	
   ;	char ControlPopUp(int op)
   ;	
	assume	cs:_TEXT
_ControlPopUp	proc	near
	push	bp
	mov	bp,sp
	sub	sp,2
   ;	
   ;	{
   ;		char r=0;
   ;	
	mov	byte ptr [bp-1],0
   ;	
   ;		if(Game.st!=PAUSA) return 0;
   ;	
	cmp	word ptr DGROUP:_Game,4
	je	short @69@114
	mov	al,0
@69@86:
	jmp	@69@702
@69@114:
   ;	
   ;		switch(op)
   ;	
	mov	bx,word ptr [bp+4]
	cmp	bx,3
	ja	short @69@674
	shl	bx,1
	jmp	word ptr cs:@69@C674[bx]
@69@198:
   ;	
   ;		{
   ;			case 1:if(PopUp>0) PopUp--; else PopUp=3; break;
   ;	
	cmp	word ptr DGROUP:_PopUp,0
	jle	short @69@254
	dec	word ptr DGROUP:_PopUp
	jmp	short @69@282
@69@254:
	mov	word ptr DGROUP:_PopUp,3
@69@282:
	jmp	short @69@674
@69@310:
   ;	
   ;			case 0:if(PopUp<3) PopUp++; else PopUp=0; break;
   ;	
	cmp	word ptr DGROUP:_PopUp,3
	jge	short @69@366
	inc	word ptr DGROUP:_PopUp
	jmp	short @69@394
@69@366:
	mov	word ptr DGROUP:_PopUp,0
@69@394:
	jmp	short @69@674
@69@422:
   ;	
   ;			case 2:switch(PopUp)
   ;	
	mov	bx,word ptr DGROUP:_PopUp
	cmp	bx,3
	ja	short @69@618
	shl	bx,1
	jmp	word ptr cs:@69@C722[bx]
@69@506:
   ;	
   ;					{
   ;						case 1:SetSomOnOff();break;
   ;	
	call	near ptr _SetSomOnOff
	jmp	short @69@618
@69@534:
   ;	
   ;						case 2:som=!som;break;
   ;	
	mov	ax,word ptr DGROUP:_som
	neg	ax
	sbb	ax,ax
	inc	ax
	mov	word ptr DGROUP:_som,ax
	jmp	short @69@618
@69@562:
   ;	
   ;						case 0:r=5;break;
   ;	
	mov	byte ptr [bp-1],5
	jmp	short @69@618
@69@590:
   ;	
   ;						case 3:r=4;break;
   ;	
	mov	byte ptr [bp-1],4
	jmp	short @69@618
@69@618:
   ;	
   ;					}
   ;					break;
   ;	
	jmp	short @69@674
@69@646:
   ;	
   ;			case 3:PopUp=3;r=5;break;
   ;	
	mov	word ptr DGROUP:_PopUp,3
	mov	byte ptr [bp-1],5
	jmp	short @69@674
@69@674:
   ;	
   ;		}
   ;		return r;
   ;	
	mov	al,byte ptr [bp-1]
	jmp	@69@86
@69@702:
   ;	
   ;	}
   ;	
	mov	sp,bp
	pop	bp
	ret	
_ControlPopUp	endp
@69@C722	label	word
	dw	@69@562
	dw	@69@506
	dw	@69@534
	dw	@69@590
@69@C674	label	word
	dw	@69@310
	dw	@69@198
	dw	@69@422
	dw	@69@646
   ;	
   ;	void Colides()
   ;	
	assume	cs:_TEXT
_Colides	proc	near
	push	bp
	mov	bp,sp
	push	si
   ;	
   ;	{
   ;		int c;
   ;		for(c=0;c<NG;c++)
   ;	
	xor	si,si
	jmp	short @70@142
@70@58:
   ;	
   ;		{
   ;			GhostColide(c);
   ;	
	push	si
	call	near ptr _GhostColide
	pop	cx
   ;	
   ;			if(Game.st!=PLAY)
   ;	
	cmp	word ptr DGROUP:_Game,1
	je	short @70@114
   ;	
   ;				break;
   ;	
	jmp	short @70@170
@70@114:
	inc	si
@70@142:
	cmp	si,word ptr DGROUP:_NG
	jl	short @70@58
@70@170:
   ;	
   ;		}
   ;		for(c=0;c<NG;c++)
   ;	
	xor	si,si
	jmp	short @70@254
@70@198:
   ;	
   ;			BaseColide(c);
   ;	
	push	si
	call	near ptr _BaseColide
	pop	cx
	inc	si
@70@254:
	cmp	si,word ptr DGROUP:_NG
	jl	short @70@198
   ;	
   ;		BonusColide();
   ;	
	call	near ptr _BonusColide
   ;	
   ;		if(Game.st==PLAY)
   ;	
	cmp	word ptr DGROUP:_Game,1
	jne	short @70@338
   ;	
   ;			DotColide();
   ;	
	call	near ptr _DotColide
@70@338:
   ;	
   ;	}
   ;	
	pop	si
	pop	bp
	ret	
_Colides	endp
   ;	
   ;	void GhostControl(int g)
   ;	
	assume	cs:_TEXT
_GhostControl	proc	near
	push	bp
	mov	bp,sp
	push	si
	mov	si,word ptr [bp+4]
   ;	
   ;	{
   ;		if(Ghost[g].m.st==1)
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	cmp	byte ptr DGROUP:_Ghost[bx+10],1
	jne	short @71@114
   ;	
   ;		{
   ;			Ghost[g].vc--;
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	dec	word ptr DGROUP:_Ghost[bx+13]
   ;	
   ;			if(Ghost[g].vc<=0)
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	cmp	word ptr DGROUP:_Ghost[bx+13],0
	jg	short @71@114
   ;	
   ;				Ghost[g].m.st=0;
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	mov	byte ptr DGROUP:_Ghost[bx+10],0
@71@114:
   ;	
   ;		}
   ;		MoveGhost(g);
   ;	
	push	si
	call	near ptr _MoveGhost
	pop	cx
   ;	
   ;		Move(&Ghost[g].m);
   ;	
	mov	ax,si
	mov	cl,4
	shl	ax,cl
	add	ax,offset DGROUP:_Ghost
	push	ax
	call	near ptr _Move
	pop	cx
   ;	
   ;	}
   ;	
	pop	si
	pop	bp
	ret	
_GhostControl	endp
   ;	
   ;	void PacControl()
   ;	
	assume	cs:_TEXT
_PacControl	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;		if(Game.count%10==0) Pac.m.st=Pac.m.st^0x1;
   ;	
	mov	ax,word ptr DGROUP:_Game+2
	mov	bx,10
	cwd	
	idiv	bx
	or	dx,dx
	jne	short @72@86
	mov	al,byte ptr DGROUP:_Pac+10
	xor	al,1
	mov	byte ptr DGROUP:_Pac+10,al
@72@86:
   ;	
   ;		Move(&Pac.m);
   ;	
	mov	ax,offset DGROUP:_Pac
	push	ax
	call	near ptr _Move
	pop	cx
   ;	
   ;	}
   ;	
	pop	bp
	ret	
_PacControl	endp
   ;	
   ;	void GameControl()
   ;	
	assume	cs:_TEXT
_GameControl	proc	near
	push	bp
	mov	bp,sp
	push	si
   ;	
   ;	{
   ;		int c;
   ;		PacControl();
   ;	
	call	near ptr _PacControl
   ;	
   ;		ControlBonus();
   ;	
	call	near ptr _ControlBonus
   ;	
   ;		for(c=0;c<NG;c++)
   ;	
	xor	si,si
	jmp	short @73@114
@73@58:
   ;	
   ;			GhostControl(c);
   ;	
	push	si
	call	near ptr _GhostControl
	pop	cx
	inc	si
@73@114:
	cmp	si,word ptr DGROUP:_NG
	jl	short @73@58
   ;	
   ;	
   ;	}
   ;	
	pop	si
	pop	bp
	ret	
_GameControl	endp
   ;	
   ;	void Kill()
   ;	
	assume	cs:_TEXT
_Kill	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;		if(Game.count%5) return;
   ;	
	mov	ax,word ptr DGROUP:_Game+2
	mov	bx,5
	cwd	
	idiv	bx
	or	dx,dx
	je	short @74@86
	jmp	@74@450
@74@86:
   ;	
   ;		if(Game.ast<=ANF)
   ;	
	cmp	word ptr DGROUP:_Game+7,9
	jg	short @74@198
   ;	
   ;		{
   ;			if(som) sound(500-40*Game.ast);
   ;	
	cmp	word ptr DGROUP:_som,0
	je	short @74@170
	mov	ax,word ptr DGROUP:_Game+7
	mov	dx,40
	imul	dx
	mov	dx,500
	sub	dx,ax
	push	dx
	call	near ptr _sound
	pop	cx
@74@170:
   ;	
   ;			Game.ast++;
   ;	
	inc	word ptr DGROUP:_Game+7
   ;	
   ;		}
   ;	
	jmp	@74@450
@74@198:
   ;	
   ;		else
   ;		{
   ;			nosound();
   ;	
	call	near ptr _nosound
   ;	
   ;			if(Player[Game.plr].pacs<1)
   ;	
	mov	ax,word ptr DGROUP:_Game+9
	mov	dx,14
	imul	dx
	mov	bx,ax
	cmp	word ptr DGROUP:_Player[bx+12],1
	jge	short @74@254
   ;	
   ;				Player[Game.plr].OVER=1;
   ;	
	mov	ax,word ptr DGROUP:_Game+9
	mov	dx,14
	imul	dx
	mov	bx,ax
	mov	byte ptr DGROUP:_Player[bx+9],1
@74@254:
   ;	
   ;	
   ;			if(TestGO())
   ;	
	call	near ptr _TestGO
	or	al,al
	je	short @74@310
   ;	
   ;				Game.st=GO;
   ;	
	mov	word ptr DGROUP:_Game,3
	jmp	short @74@450
@74@310:
   ;	
   ;			else
   ;			{
   ;				if(Player[NextPlayer()].OVER==0)
   ;	
	call	near ptr _NextPlayer
	cbw	
	mov	dx,14
	imul	dx
	mov	bx,ax
	cmp	byte ptr DGROUP:_Player[bx+9],0
	jne	short @74@366
   ;	
   ;					Game.plr=NextPlayer();
   ;	
	call	near ptr _NextPlayer
	cbw	
	mov	word ptr DGROUP:_Game+9,ax
@74@366:
   ;	
   ;				Player[Game.plr].pacs--;
   ;	
	mov	ax,word ptr DGROUP:_Game+9
	mov	dx,14
	imul	dx
	mov	bx,ax
	dec	word ptr DGROUP:_Player[bx+12]
   ;	
   ;				if(Player[Game.plr].level==-1)
   ;	
	mov	ax,word ptr DGROUP:_Game+9
	mov	dx,14
	imul	dx
	mov	bx,ax
	cmp	word ptr DGROUP:_Player[bx],65535
	jne	short @74@422
   ;	
   ;					InitLevel(stl);
   ;	
	push	word ptr DGROUP:_stl
	call	near ptr _InitLevel
	pop	cx
	jmp	short @74@450
@74@422:
   ;	
   ;				else
   ;					PlReset();
   ;	
	call	near ptr _PlReset
@74@450:
   ;	
   ;			}
   ;	
   ;		}
   ;	}
   ;	
	pop	bp
	ret	
_Kill	endp
   ;	
   ;	char NextPlayer()
   ;	
	assume	cs:_TEXT
_NextPlayer	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;		char next=Game.plr;
   ;	
	mov	al,byte ptr DGROUP:_Game+9
	mov	dl,al
   ;	
   ;	
   ;		if(pl)
   ;	
	cmp	word ptr DGROUP:_pl,0
	je	short @75@226
   ;	
   ;		switch(Game.plr)
   ;	
	mov	ax,word ptr DGROUP:_Game+9
	or	ax,ax
	je	short @75@170
	cmp	ax,1
	je	short @75@198
	jmp	short @75@226
@75@170:
   ;	
   ;		{
   ;			case 0:next=1;break;
   ;	
	mov	dl,1
	jmp	short @75@226
@75@198:
   ;	
   ;			case 1:next=0;break;
   ;	
	mov	dl,0
	jmp	short @75@226
@75@226:
   ;	
   ;		}
   ;		return next;
   ;	
	mov	al,dl
	jmp	short @75@254
@75@254:
   ;	
   ;	}
   ;	
	pop	bp
	ret	
_NextPlayer	endp
   ;	
   ;	char TestGO()
   ;	
	assume	cs:_TEXT
_TestGO	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;		if(pl)
   ;	
	cmp	word ptr DGROUP:_pl,0
	je	short @76@254
   ;	
   ;		{
   ;			if((Player[0].OVER)&&(Player[1].OVER))
   ;	
	cmp	byte ptr DGROUP:_Player+9,0
	je	short @76@198
	cmp	byte ptr DGROUP:_Player+23,0
	je	short @76@198
@76@114:
   ;	
   ;				return 1;
   ;	
	mov	al,1
@76@142:
	jmp	short @76@366
	jmp	short @76@226
@76@198:
	jmp	short @76@338
@76@226:
   ;	
   ;			else
   ;				return 0;
   ;		}
   ;	
	jmp	short @76@366
@76@254:
   ;	
   ;		else
   ;		{
   ;			if(Player[Game.plr].OVER)
   ;	
	mov	ax,word ptr DGROUP:_Game+9
	mov	dx,14
	imul	dx
	mov	bx,ax
	cmp	byte ptr DGROUP:_Player[bx+9],0
	je	short @76@338
	jmp	short @76@114
   ;	
   ;				return 1;
   ;	
	jmp	short @76@366
@76@338:
   ;	
   ;			else
   ;				return 0;
   ;	
	mov	al,0
	jmp	short @76@142
@76@366:
   ;	
   ;		}
   ;	}
   ;	
	pop	bp
	ret	
_TestGO	endp
   ;	
   ;	void AvaliaJoyStick()
   ;	
	assume	cs:_TEXT
_AvaliaJoyStick	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;		if((!pl)||(Game.plr==1))
   ;	
	cmp	word ptr DGROUP:_pl,0
	je	short @77@86
	cmp	word ptr DGROUP:_Game+9,1
	jne	short @77@310
@77@86:
   ;	
   ;		{
   ;			if(StickB[0]) SetDir(LEFT,&Pac.m);
   ;	
	cmp	byte ptr DGROUP:_StickB,0
	je	short @77@142
	mov	ax,offset DGROUP:_Pac
	push	ax
	mov	al,4
	push	ax
	call	near ptr _SetDir
	pop	cx
	pop	cx
@77@142:
   ;	
   ;			if(StickB[1]) SetDir(RIGHT,&Pac.m);
   ;	
	cmp	byte ptr DGROUP:_StickB+1,0
	je	short @77@198
	mov	ax,offset DGROUP:_Pac
	push	ax
	mov	al,3
	push	ax
	call	near ptr _SetDir
	pop	cx
	pop	cx
@77@198:
   ;	
   ;			if(StickB[2]) SetDir(UP,&Pac.m);
   ;	
	cmp	byte ptr DGROUP:_StickB+2,0
	je	short @77@254
	mov	ax,offset DGROUP:_Pac
	push	ax
	mov	al,1
	push	ax
	call	near ptr _SetDir
	pop	cx
	pop	cx
@77@254:
   ;	
   ;			if(StickB[3]) SetDir(DOWN,&Pac.m);
   ;	
	cmp	byte ptr DGROUP:_StickB+3,0
	je	short @77@310
	mov	ax,offset DGROUP:_Pac
	push	ax
	mov	al,2
	push	ax
	call	near ptr _SetDir
	pop	cx
	pop	cx
@77@310:
   ;	
   ;		}
   ;	
   ;		if((!pl)||(Game.plr==0))
   ;	
	cmp	word ptr DGROUP:_pl,0
	je	short @77@366
	cmp	word ptr DGROUP:_Game+9,0
	jne	short @77@590
@77@366:
   ;	
   ;		{
   ;			if(StickA[0]) SetDir(LEFT,&Pac.m);
   ;	
	cmp	byte ptr DGROUP:_StickA,0
	je	short @77@422
	mov	ax,offset DGROUP:_Pac
	push	ax
	mov	al,4
	push	ax
	call	near ptr _SetDir
	pop	cx
	pop	cx
@77@422:
   ;	
   ;			if(StickA[1]) SetDir(RIGHT,&Pac.m);
   ;	
	cmp	byte ptr DGROUP:_StickA+1,0
	je	short @77@478
	mov	ax,offset DGROUP:_Pac
	push	ax
	mov	al,3
	push	ax
	call	near ptr _SetDir
	pop	cx
	pop	cx
@77@478:
   ;	
   ;			if(StickA[2]) SetDir(UP,&Pac.m);
   ;	
	cmp	byte ptr DGROUP:_StickA+2,0
	je	short @77@534
	mov	ax,offset DGROUP:_Pac
	push	ax
	mov	al,1
	push	ax
	call	near ptr _SetDir
	pop	cx
	pop	cx
@77@534:
   ;	
   ;			if(StickA[3]) SetDir(DOWN,&Pac.m);
   ;	
	cmp	byte ptr DGROUP:_StickA+3,0
	je	short @77@590
	mov	ax,offset DGROUP:_Pac
	push	ax
	mov	al,2
	push	ax
	call	near ptr _SetDir
	pop	cx
	pop	cx
@77@590:
   ;	
   ;		}
   ;	
   ;		#ifdef USE_RATO
   ;		if(RatoM(LEFT)) SetDir(LEFT,&Pac.m);
   ;		if(RatoM(RIGHT)) SetDir(RIGHT,&Pac.m);
   ;		if(RatoM(UP)) SetDir(UP,&Pac.m);
   ;		if(RatoM(DOWN)) SetDir(DOWN,&Pac.m);
   ;		#endif
   ;	}
   ;	
	pop	bp
	ret	
_AvaliaJoyStick	endp
   ;	
   ;	void GhostSetDH(int g)
   ;	
	assume	cs:_TEXT
_GhostSetDH	proc	near
	push	bp
	mov	bp,sp
	push	si
	mov	si,word ptr [bp+4]
   ;	
   ;	{
   ;		switch(Ghost[g].m.st)
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	mov	al,byte ptr DGROUP:_Ghost[bx+10]
	cbw	
	or	ax,ax
	je	short @78@170
	cmp	ax,1
	je	short @78@310
	cmp	ax,2
	jne	@@17
	jmp	@78@450
@@17:
	jmp	@78@590
@78@170:
   ;	
   ;		{
   ;			case 0:
   ;					if(Pac.m.x>Ghost[g].m.x)
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	mov	ax,word ptr DGROUP:_Ghost[bx]
	cmp	ax,word ptr DGROUP:_Pac
	jge	short @78@226
   ;	
   ;					{
   ;						SetDir(SDir(RIGHT),&Ghost[g].m);
   ;	
	mov	ax,si
	mov	cl,4
	shl	ax,cl
	add	ax,offset DGROUP:_Ghost
	push	ax
	mov	al,3
	push	ax
	call	near ptr _SDir
	pop	cx
	push	ax
	call	near ptr _SetDir
	pop	cx
	pop	cx
@78@226:
   ;	
   ;					}
   ;					if(Pac.m.x<Ghost[g].m.x)
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	mov	ax,word ptr DGROUP:_Ghost[bx]
	cmp	ax,word ptr DGROUP:_Pac
	jle	short @78@282
   ;	
   ;					{
   ;						SetDir(SDir(LEFT),&Ghost[g].m);
   ;	
	mov	ax,si
	mov	cl,4
	shl	ax,cl
	add	ax,offset DGROUP:_Ghost
	push	ax
	mov	al,4
	push	ax
	call	near ptr _SDir
	pop	cx
	push	ax
	call	near ptr _SetDir
	pop	cx
	pop	cx
@78@282:
   ;	
   ;					}
   ;				break;
   ;	
	jmp	@78@590
@78@310:
   ;	
   ;			case 1:
   ;					if(Pac.m.x<=Ghost[g].m.x)
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	mov	ax,word ptr DGROUP:_Ghost[bx]
	cmp	ax,word ptr DGROUP:_Pac
	jl	short @78@366
   ;	
   ;					{
   ;						SetDir(SDir(RIGHT),&Ghost[g].m);
   ;	
	mov	ax,si
	mov	cl,4
	shl	ax,cl
	add	ax,offset DGROUP:_Ghost
	push	ax
	mov	al,3
	push	ax
	call	near ptr _SDir
	pop	cx
	push	ax
	call	near ptr _SetDir
	pop	cx
	pop	cx
@78@366:
   ;	
   ;					}
   ;					if(Pac.m.x>=Ghost[g].m.x)
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	mov	ax,word ptr DGROUP:_Ghost[bx]
	cmp	ax,word ptr DGROUP:_Pac
	jg	short @78@422
   ;	
   ;					{
   ;						SetDir(SDir(LEFT),&Ghost[g].m);
   ;	
	mov	ax,si
	mov	cl,4
	shl	ax,cl
	add	ax,offset DGROUP:_Ghost
	push	ax
	mov	al,4
	push	ax
	call	near ptr _SDir
	pop	cx
	push	ax
	call	near ptr _SetDir
	pop	cx
	pop	cx
@78@422:
   ;	
   ;					}
   ;				break;
   ;	
	jmp	short @78@590
@78@450:
   ;	
   ;			case 2:
   ;					if(GXI>Ghost[g].m.x)
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	mov	ax,word ptr DGROUP:_Ghost[bx]
	cmp	ax,word ptr DGROUP:_GXI
	jge	short @78@506
   ;	
   ;					{
   ;						SetDir(RIGHT,&Ghost[g].m);
   ;	
	mov	ax,si
	mov	cl,4
	shl	ax,cl
	add	ax,offset DGROUP:_Ghost
	push	ax
	mov	al,3
	push	ax
	call	near ptr _SetDir
	pop	cx
	pop	cx
@78@506:
   ;	
   ;					}
   ;					if(GXI<Ghost[g].m.x)
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	mov	ax,word ptr DGROUP:_Ghost[bx]
	cmp	ax,word ptr DGROUP:_GXI
	jle	short @78@562
   ;	
   ;					{
   ;						SetDir(LEFT,&Ghost[g].m);
   ;	
	mov	ax,si
	mov	cl,4
	shl	ax,cl
	add	ax,offset DGROUP:_Ghost
	push	ax
	mov	al,4
	push	ax
	call	near ptr _SetDir
	pop	cx
	pop	cx
@78@562:
   ;	
   ;					}
   ;				break;
   ;	
	jmp	short @78@590
@78@590:
   ;	
   ;		}
   ;	}
   ;	
	pop	si
	pop	bp
	ret	
_GhostSetDH	endp
   ;	
   ;	void GhostSetDV(int g)
   ;	
	assume	cs:_TEXT
_GhostSetDV	proc	near
	push	bp
	mov	bp,sp
	push	si
	mov	si,word ptr [bp+4]
   ;	
   ;	{
   ;		switch(Ghost[g].m.st)
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	mov	al,byte ptr DGROUP:_Ghost[bx+10]
	cbw	
	or	ax,ax
	je	short @79@170
	cmp	ax,1
	je	short @79@310
	cmp	ax,2
	jne	@@18
	jmp	@79@450
@@18:
	jmp	@79@590
@79@170:
   ;	
   ;		{
   ;			case 0:
   ;					if(Pac.m.y>Ghost[g].m.y)
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	mov	ax,word ptr DGROUP:_Ghost[bx+2]
	cmp	ax,word ptr DGROUP:_Pac+2
	jge	short @79@226
   ;	
   ;					{
   ;						SetDir(SDir(DOWN),&Ghost[g].m);
   ;	
	mov	ax,si
	mov	cl,4
	shl	ax,cl
	add	ax,offset DGROUP:_Ghost
	push	ax
	mov	al,2
	push	ax
	call	near ptr _SDir
	pop	cx
	push	ax
	call	near ptr _SetDir
	pop	cx
	pop	cx
@79@226:
   ;	
   ;					}
   ;					if(Pac.m.y<Ghost[g].m.y)
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	mov	ax,word ptr DGROUP:_Ghost[bx+2]
	cmp	ax,word ptr DGROUP:_Pac+2
	jle	short @79@282
   ;	
   ;					{
   ;						SetDir(SDir(UP),&Ghost[g].m);
   ;	
	mov	ax,si
	mov	cl,4
	shl	ax,cl
	add	ax,offset DGROUP:_Ghost
	push	ax
	mov	al,1
	push	ax
	call	near ptr _SDir
	pop	cx
	push	ax
	call	near ptr _SetDir
	pop	cx
	pop	cx
@79@282:
   ;	
   ;					}
   ;				break;
   ;	
	jmp	@79@590
@79@310:
   ;	
   ;			case 1:
   ;					if(Pac.m.y<=Ghost[g].m.y)
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	mov	ax,word ptr DGROUP:_Ghost[bx+2]
	cmp	ax,word ptr DGROUP:_Pac+2
	jl	short @79@366
   ;	
   ;					{
   ;						SetDir(SDir(DOWN),&Ghost[g].m);
   ;	
	mov	ax,si
	mov	cl,4
	shl	ax,cl
	add	ax,offset DGROUP:_Ghost
	push	ax
	mov	al,2
	push	ax
	call	near ptr _SDir
	pop	cx
	push	ax
	call	near ptr _SetDir
	pop	cx
	pop	cx
@79@366:
   ;	
   ;					}
   ;					if(Pac.m.y>=Ghost[g].m.y)
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	mov	ax,word ptr DGROUP:_Ghost[bx+2]
	cmp	ax,word ptr DGROUP:_Pac+2
	jg	short @79@422
   ;	
   ;					{
   ;						SetDir(SDir(UP),&Ghost[g].m);
   ;	
	mov	ax,si
	mov	cl,4
	shl	ax,cl
	add	ax,offset DGROUP:_Ghost
	push	ax
	mov	al,1
	push	ax
	call	near ptr _SDir
	pop	cx
	push	ax
	call	near ptr _SetDir
	pop	cx
	pop	cx
@79@422:
   ;	
   ;					}
   ;				break;
   ;	
	jmp	short @79@590
@79@450:
   ;	
   ;			case 2:
   ;					if(GYI>Ghost[g].m.y)
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	mov	ax,word ptr DGROUP:_Ghost[bx+2]
	cmp	ax,word ptr DGROUP:_GYI
	jge	short @79@506
   ;	
   ;					{
   ;						SetDir(DOWN,&Ghost[g].m);
   ;	
	mov	ax,si
	mov	cl,4
	shl	ax,cl
	add	ax,offset DGROUP:_Ghost
	push	ax
	mov	al,2
	push	ax
	call	near ptr _SetDir
	pop	cx
	pop	cx
@79@506:
   ;	
   ;					}
   ;					if(GYI<Ghost[g].m.y)
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	mov	ax,word ptr DGROUP:_Ghost[bx+2]
	cmp	ax,word ptr DGROUP:_GYI
	jle	short @79@562
   ;	
   ;					{
   ;						SetDir(UP,&Ghost[g].m);
   ;	
	mov	ax,si
	mov	cl,4
	shl	ax,cl
	add	ax,offset DGROUP:_Ghost
	push	ax
	mov	al,1
	push	ax
	call	near ptr _SetDir
	pop	cx
	pop	cx
@79@562:
   ;	
   ;					}
   ;				break;
   ;	
	jmp	short @79@590
@79@590:
   ;	
   ;		}
   ;	}
   ;	
	pop	si
	pop	bp
	ret	
_GhostSetDV	endp
   ;	
   ;	void MoveGhost(int g)
   ;	
	assume	cs:_TEXT
_MoveGhost	proc	near
	push	bp
	mov	bp,sp
	sub	sp,2
	push	si
	mov	si,word ptr [bp+4]
   ;	
   ;	{
   ;		char flag=0;
   ;	
	mov	byte ptr [bp-1],0
   ;	
   ;		if(Ghost[g].m.dir==STOP)
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	cmp	byte ptr DGROUP:_Ghost[bx+8],0
	jne	short @80@86
   ;	
   ;			flag=1;
   ;	
	mov	byte ptr [bp-1],1
@80@86:
   ;	
   ;		if(Game.count%200==0)
   ;	
	mov	ax,word ptr DGROUP:_Game+2
	mov	bx,200
	cwd	
	idiv	bx
	or	dx,dx
	jne	short @80@142
   ;	
   ;			flag=1;
   ;	
	mov	byte ptr [bp-1],1
@80@142:
   ;	
   ;		if((xrandom(10)<5)||(Ghost[g].m.st==2))
   ;	
	mov	ax,10
	push	ax
	call	near ptr _mrandom
	pop	cx
	cmp	ax,5
	jb	short @80@198
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	cmp	byte ptr DGROUP:_Ghost[bx+10],2
	jne	short @80@366
@80@198:
   ;	
   ;		{
   ;			if((Ghost[g].ld==H)&&(Ghost[g].m.ox==0))
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	cmp	byte ptr DGROUP:_Ghost[bx+15],0
	jne	short @80@282
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	cmp	word ptr DGROUP:_Ghost[bx+4],0
	jne	short @80@282
   ;	
   ;				flag=1;
   ;	
	mov	byte ptr [bp-1],1
@80@282:
   ;	
   ;			if((Ghost[g].ld==V)&&(Ghost[g].m.oy==0))
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	cmp	byte ptr DGROUP:_Ghost[bx+15],1
	jne	short @80@366
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	cmp	word ptr DGROUP:_Ghost[bx+6],0
	jne	short @80@366
   ;	
   ;				flag=1;
   ;	
	mov	byte ptr [bp-1],1
@80@366:
   ;	
   ;		}
   ;		if(!flag)
   ;	
	mov	al,byte ptr [bp-1]
	cbw	
	or	ax,ax
	jne	short @80@422
   ;	
   ;				return;
   ;	
	jmp	@80@898
@80@422:
   ;	
   ;		if(Ghost[g].ld==H)
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	cmp	byte ptr DGROUP:_Ghost[bx+15],0
	jne	short @80@478
   ;	
   ;		{
   ;			GhostSetDV(g);
   ;	
	push	si
	call	near ptr _GhostSetDV
	jmp	short @80@506
@80@478:
   ;	
   ;		}
   ;		else
   ;		{
   ;			GhostSetDH(g);
   ;	
	push	si
	call	near ptr _GhostSetDH
@80@506:
	pop	cx
   ;	
   ;		}
   ;	
   ;		if(Ghost[g].m.dir==STOP) SetDir(UP,&Ghost[g].m);
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	cmp	byte ptr DGROUP:_Ghost[bx+8],0
	jne	short @80@590
	mov	ax,si
	mov	cl,4
	shl	ax,cl
	add	ax,offset DGROUP:_Ghost
	push	ax
	mov	al,1
	push	ax
	call	near ptr _SetDir
	pop	cx
	pop	cx
@80@590:
   ;	
   ;		if(Ghost[g].m.dir==STOP) SetDir(LEFT,&Ghost[g].m);
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	cmp	byte ptr DGROUP:_Ghost[bx+8],0
	jne	short @80@646
	mov	ax,si
	mov	cl,4
	shl	ax,cl
	add	ax,offset DGROUP:_Ghost
	push	ax
	mov	al,4
	push	ax
	call	near ptr _SetDir
	pop	cx
	pop	cx
@80@646:
   ;	
   ;		if(Ghost[g].m.dir==STOP) SetDir(RIGHT,&Ghost[g].m);
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	cmp	byte ptr DGROUP:_Ghost[bx+8],0
	jne	short @80@702
	mov	ax,si
	mov	cl,4
	shl	ax,cl
	add	ax,offset DGROUP:_Ghost
	push	ax
	mov	al,3
	push	ax
	call	near ptr _SetDir
	pop	cx
	pop	cx
@80@702:
   ;	
   ;		if(Ghost[g].m.dir==STOP) SetDir(DOWN,&Ghost[g].m);
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	cmp	byte ptr DGROUP:_Ghost[bx+8],0
	jne	short @80@758
	mov	ax,si
	mov	cl,4
	shl	ax,cl
	add	ax,offset DGROUP:_Ghost
	push	ax
	mov	al,2
	push	ax
	call	near ptr _SetDir
	pop	cx
	pop	cx
@80@758:
   ;	
   ;		switch(Ghost[g].m.dir)
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	mov	al,byte ptr DGROUP:_Ghost[bx+8]
	cbw	
	dec	ax
	mov	bx,ax
	cmp	bx,3
	ja	short @80@898
	shl	bx,1
	jmp	word ptr cs:@80@C850[bx]
@80@842:
   ;	
   ;		{
   ;			case LEFT:
   ;			case RIGHT:Ghost[g].ld=H;break;
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	mov	byte ptr DGROUP:_Ghost[bx+15],0
	jmp	short @80@898
@80@870:
   ;	
   ;			case UP:
   ;			case DOWN:Ghost[g].ld=V;break;
   ;	
	mov	bx,si
	mov	cl,4
	shl	bx,cl
	mov	byte ptr DGROUP:_Ghost[bx+15],1
	jmp	short @80@898
@80@898:
   ;	
   ;		}
   ;	}
   ;	
	pop	si
	mov	sp,bp
	pop	bp
	ret	
_MoveGhost	endp
@80@C850	label	word
	dw	@80@870
	dw	@80@870
	dw	@80@842
	dw	@80@842
   ;	
   ;	char SDir(char dir)
   ;	
	assume	cs:_TEXT
_SDir	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;		if(xrandom(20)<15)
   ;	
	mov	ax,20
	push	ax
	call	near ptr _mrandom
	pop	cx
	cmp	ax,15
	jae	short @81@142
   ;	
   ;			return dir;
   ;	
	mov	al,byte ptr [bp+4]
@81@86:
	jmp	short @81@170
	jmp	short @81@170
@81@142:
   ;	
   ;		else
   ;			return(VDir[xrandom(4)]);
   ;	
	mov	ax,4
	push	ax
	call	near ptr _mrandom
	pop	cx
	mov	bx,ax
	mov	al,byte ptr DGROUP:_VDir[bx]
	jmp	short @81@86
@81@170:
   ;	
   ;	}
   ;	
	pop	bp
	ret	
_SDir	endp
   ;	
   ;	void writenumber(char *buffer, unsigned d, unsigned r)
   ;	
	assume	cs:_TEXT
_writenumber	proc	near
	push	bp
	mov	bp,sp
	sub	sp,6
	push	si
	push	di
	mov	di,word ptr [bp+8]
   ;	
   ;	{
   ;	  char t[6];
   ;	  unsigned rm=0;
   ;	
	xor	si,si
   ;	
   ;	  t[0] = (d/10000)%10+'0';
   ;	
	mov	ax,word ptr [bp+6]
	mov	bx,10000
	xor	dx,dx
	div	bx
	mov	bx,10
	xor	dx,dx
	div	bx
	add	dl,48
	mov	byte ptr [bp-6],dl
   ;	
   ;	  t[1] = (d/1000)%10+'0';
   ;	
	mov	ax,word ptr [bp+6]
	mov	bx,1000
	xor	dx,dx
	div	bx
	mov	bx,10
	xor	dx,dx
	div	bx
	add	dl,48
	mov	byte ptr [bp-5],dl
   ;	
   ;	  t[2] = (d/100)%10+'0';
   ;	
	mov	ax,word ptr [bp+6]
	mov	bx,100
	xor	dx,dx
	div	bx
	mov	bx,10
	xor	dx,dx
	div	bx
	add	dl,48
	mov	byte ptr [bp-4],dl
   ;	
   ;	  t[3] = (d/10)%10+'0';
   ;	
	mov	ax,word ptr [bp+6]
	mov	bx,10
	xor	dx,dx
	div	bx
	mov	bx,10
	xor	dx,dx
	div	bx
	add	dl,48
	mov	byte ptr [bp-3],dl
   ;	
   ;	  t[4] = d%10+'0';
   ;	
	mov	ax,word ptr [bp+6]
	mov	bx,10
	xor	dx,dx
	div	bx
	add	dl,48
	mov	byte ptr [bp-2],dl
   ;	
   ;	  t[5] = '\0';
   ;	
	mov	byte ptr [bp-1],0
   ;	
   ;	
   ;	  if(r>5)
   ;	
	cmp	di,5
	jbe	short @82@86
   ;	
   ;		  r=5;
   ;	
	mov	di,5
@82@86:
   ;	
   ;	  if(r<1)
   ;	
	cmp	di,1
	jae	short @82@142
   ;	
   ;		  r=1;
   ;	
	mov	di,1
@82@142:
	jmp	short @82@198
@82@170:
   ;	
   ;		while(t[rm]=='0')
   ;		  rm++;
   ;	
	inc	si
@82@198:
	cmp	byte ptr [bp+si-6],48
	je	short @82@170
   ;	
   ;	
   ;	  if(r>5-rm)
   ;	
	mov	ax,5
	sub	ax,si
	cmp	ax,di
	jae	short @82@282
   ;	
   ;		  rm=5-r;
   ;	
	mov	ax,5
	sub	ax,di
	mov	si,ax
@82@282:
   ;	
   ;	  if(rm>5)
   ;	
	cmp	si,5
	jbe	short @82@338
   ;	
   ;		  rm=5;
   ;	
	mov	si,5
@82@338:
   ;	
   ;	  strcpy(buffer,&t[rm]);
   ;	
	lea	ax,word ptr [bp-6]
	mov	dx,si
	add	dx,ax
	push	dx
	push	word ptr [bp+4]
	call	near ptr _strcpy
	pop	cx
	pop	cx
   ;	
   ;	}
   ;	
	pop	di
	pop	si
	mov	sp,bp
	pop	bp
	ret	
_writenumber	endp
_TEXT	ends
_DATA	segment word public 'DATA'
	db	24
	db	27
_DATA	ends
_TEXT	segment byte public 'CODE'
   ;	
   ;	void ShowScore()
   ;	
	assume	cs:_TEXT
_ShowScore	proc	near
	push	bp
	mov	bp,sp
	sub	sp,84
	push	si
	push	di
   ;	
   ;	{
   ;		char st[80];
   ;		char cor[2]={MkColor(0,6,0),MkColor(0,6,3)};
   ;	
	mov	ax,word ptr DGROUP:d@w+132
	mov	word ptr [bp-2],ax
   ;	
   ;		int i,c,off=0;
   ;	
	xor	di,di
   ;	
   ;		TextColor=MkColor(2,2,1);
   ;	
	mov	byte ptr DGROUP:_TextColor,73
   ;	
   ;		writest(12,180,"Pac Man");
   ;	
	mov	ax,offset DGROUP:s@+604
	push	ax
	mov	ax,180
	push	ax
	mov	ax,12
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;		TextColor=MkColor(6,6,0);
   ;	
	mov	byte ptr DGROUP:_TextColor,216
   ;	
   ;		writest(13,181,"Pac Man");
   ;	
	mov	ax,offset DGROUP:s@+612
	push	ax
	mov	ax,181
	push	ax
	mov	ax,13
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;		if(pl)
   ;	
	cmp	word ptr DGROUP:_pl,0
	je	short @83@86
   ;	
   ;			off=-5;
   ;	
	mov	di,65531
@83@86:
   ;	
   ;		for(i=0;i<=pl;i++)
   ;	
	xor	si,si
	jmp	@83@394
@83@114:
   ;	
   ;		{
   ;			off+=i*13;
   ;	
	mov	ax,si
	mov	dx,13
	imul	dx
	add	di,ax
   ;	
   ;			if(i==Game.plr)
   ;	
	cmp	si,word ptr DGROUP:_Game+9
	jne	short @83@170
   ;	
   ;			{
   ;				Color=MkColor(0,6,0);
   ;	
	mov	byte ptr DGROUP:_Color,24
   ;	
   ;				DrawRect(107,177+off,314,190+off,LINE);
   ;	
	mov	al,1
	push	ax
	mov	ax,di
	add	ax,190
	push	ax
	mov	ax,314
	push	ax
	mov	ax,di
	add	ax,177
	push	ax
	mov	ax,107
	push	ax
	call	near ptr _DrawRect
	add	sp,10
   ;	
   ;				Color=MkColor(5,0,0);
   ;	
	mov	byte ptr DGROUP:_Color,160
   ;	
   ;				DrawRect(106,176+off,313,189+off,LINE);
   ;	
	mov	al,1
	push	ax
	mov	ax,di
	add	ax,189
	push	ax
	mov	ax,313
	push	ax
	mov	ax,di
	add	ax,176
	push	ax
	mov	ax,106
	push	ax
	call	near ptr _DrawRect
	add	sp,10
@83@170:
   ;	
   ;			}
   ;			writenumber(st,Player[i].score,5);
   ;	
	mov	ax,5
	push	ax
	mov	ax,si
	mov	dx,14
	imul	dx
	mov	bx,ax
	push	word ptr DGROUP:_Player[bx+10]
	lea	ax,word ptr [bp-84]
	push	ax
	call	near ptr _writenumber
	add	sp,6
   ;	
   ;			//sprintf(st,"%05d",Player[i].score);
   ;			
   ;			TextColor=MkColor(2,2,1);
   ;	
	mov	byte ptr DGROUP:_TextColor,73
   ;	
   ;			writest(112,180+off,st);
   ;	
	lea	ax,word ptr [bp-84]
	push	ax
	mov	ax,di
	add	ax,180
	push	ax
	mov	ax,112
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;			TextColor=cor[i];
   ;	
	mov	al,byte ptr [bp+si-2]
	mov	byte ptr DGROUP:_TextColor,al
   ;	
   ;			writest(113,181+off,st);
   ;	
	lea	ax,word ptr [bp-84]
	push	ax
	mov	ax,di
	add	ax,181
	push	ax
	mov	ax,113
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;			writenumber(st,Player[i].level!=-1?Player[i].level:stl,2);
   ;	
	mov	ax,2
	push	ax
	mov	ax,si
	mov	dx,14
	imul	dx
	mov	bx,ax
	cmp	word ptr DGROUP:_Player[bx],65535
	je	short @83@226
	mov	ax,si
	mov	dx,14
	imul	dx
	mov	bx,ax
	mov	ax,word ptr DGROUP:_Player[bx]
	jmp	short @83@254
@83@226:
	mov	ax,word ptr DGROUP:_stl
@83@254:
	push	ax
	lea	ax,word ptr [bp-84]
	push	ax
	call	near ptr _writenumber
	add	sp,6
   ;	
   ;			//sprintf(st,"%02d",Player[i].level!=-1?Player[i].level:stl);
   ;			TextColor=MkColor(2,2,1);
   ;	
	mov	byte ptr DGROUP:_TextColor,73
   ;	
   ;			writest(212,180+off,st);
   ;	
	lea	ax,word ptr [bp-84]
	push	ax
	mov	ax,di
	add	ax,180
	push	ax
	mov	ax,212
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;			TextColor=cor[i];
   ;	
	mov	al,byte ptr [bp+si-2]
	mov	byte ptr DGROUP:_TextColor,al
   ;	
   ;			writest(213,181+off,st);
   ;	
	lea	ax,word ptr [bp-84]
	push	ax
	mov	ax,di
	add	ax,181
	push	ax
	mov	ax,213
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;			for(c=0;c<Player[i].pacs;c++)
   ;	
	mov	word ptr [bp-4],0
	jmp	short @83@338
@83@282:
   ;	
   ;			{
   ;				DrawPic(300-c*15,179+off,Pac.pic[0+8*i]);
   ;	
	mov	bx,si
	shl	bx,1
	shl	bx,1
	shl	bx,1
	shl	bx,1
	shl	bx,1
	push	word ptr DGROUP:_Pac[bx+15]
	push	word ptr DGROUP:_Pac[bx+13]
	mov	ax,di
	add	ax,179
	push	ax
	mov	ax,word ptr [bp-4]
	mov	dx,15
	imul	dx
	mov	dx,300
	sub	dx,ax
	push	dx
	call	near ptr _DrawPic
	add	sp,8
	inc	word ptr [bp-4]
@83@338:
	mov	ax,si
	mov	dx,14
	imul	dx
	mov	bx,ax
	mov	ax,word ptr DGROUP:_Player[bx+12]
	cmp	ax,word ptr [bp-4]
	jg	short @83@282
	inc	si
@83@394:
	cmp	si,word ptr DGROUP:_pl
	jg	@@19
	jmp	@83@114
@@19:
   ;	
   ;			}
   ;		}
   ;		switch(Game.st)
   ;	
	mov	ax,word ptr DGROUP:_Game
	cmp	ax,3
	je	short @83@534
	cmp	ax,4
	je	short @83@562
	jmp	short @83@590
@83@534:
   ;	
   ;		{
   ;			case GO:
   ;				Color=MkColor(0,6,0);
   ;	
	mov	byte ptr DGROUP:_Color,24
   ;	
   ;				FillColor=MkColor(3,3,2);
   ;	
	mov	byte ptr DGROUP:_FillColor,110
   ;	
   ;				DrawRect(120,75,199,94,LIFI);
   ;	
	mov	al,0
	push	ax
	mov	ax,94
	push	ax
	mov	ax,199
	push	ax
	mov	ax,75
	push	ax
	mov	ax,120
	push	ax
	call	near ptr _DrawRect
	add	sp,10
   ;	
   ;				TextColor=MkColor(0,0,0);
   ;	
	mov	byte ptr DGROUP:_TextColor,0
   ;	
   ;				writest(124,80,"Game Over");
   ;	
	mov	ax,offset DGROUP:s@+620
	push	ax
	mov	ax,80
	push	ax
	mov	ax,124
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;				TextColor=MkColor(0,6,0);
   ;	
	mov	byte ptr DGROUP:_TextColor,24
   ;	
   ;				writest(125,81,"Game Over");
   ;	
	mov	ax,offset DGROUP:s@+630
	push	ax
	mov	ax,81
	push	ax
	mov	ax,125
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;			break;
   ;	
	jmp	short @83@590
@83@562:
   ;	
   ;			case PAUSA:
   ;				ShowPopUp();
   ;	
	call	near ptr _ShowPopUp
   ;	
   ;			break;
   ;	
	jmp	short @83@590
@83@590:
   ;	
   ;		}
   ;		//sprintf(st,"%d fps",frames);
   ;		writenumber(st,frames,1);
   ;	
	mov	ax,1
	push	ax
	push	word ptr DGROUP:_frames
	lea	ax,word ptr [bp-84]
	push	ax
	call	near ptr _writenumber
	add	sp,6
   ;	
   ;		TextColor=MkColor(7,7,3);
   ;	
	mov	byte ptr DGROUP:_TextColor,255
   ;	
   ;		writest(1,1,st);
   ;	
	lea	ax,word ptr [bp-84]
	push	ax
	mov	ax,1
	push	ax
	mov	ax,1
	push	ax
	call	near ptr _writest
	add	sp,6
   ;	
   ;	}
   ;	
	pop	di
	pop	si
	mov	sp,bp
	pop	bp
	ret	
_ShowScore	endp
   ;	
   ;	void UpDateScreen()
   ;	
	assume	cs:_TEXT
_UpDateScreen	proc	near
	push	bp
	mov	bp,sp
	push	si
   ;	
   ;	{
   ;			int c;
   ;			fillscreen(0);
   ;	
	mov	al,0
	push	ax
	call	near ptr _fillscreen
	pop	cx
   ;	
   ;			Show_PF();
   ;	
	call	near ptr _Show_PF
   ;	
   ;			ShowBonus();
   ;	
	call	near ptr _ShowBonus
   ;	
   ;			Show_Pac();
   ;	
	call	near ptr _Show_Pac
   ;	
   ;			for(c=0;c<NG;c++)
   ;	
	xor	si,si
	jmp	short @84@114
@84@58:
   ;	
   ;				Show_Ghost(c);
   ;	
	push	si
	call	near ptr _Show_Ghost
	pop	cx
	inc	si
@84@114:
	cmp	si,word ptr DGROUP:_NG
	jl	short @84@58
   ;	
   ;			ShowScore();
   ;	
	call	near ptr _ShowScore
   ;	
   ;			Vsync();
   ;	
	call	near ptr _Vsync
   ;	
   ;			ScrnCpyUD();
   ;	
	call	near ptr _ScrnCpyUD
   ;	
   ;	}
   ;	
	pop	si
	pop	bp
	ret	
_UpDateScreen	endp
   ;	
   ;	void Show_PF()
   ;	
	assume	cs:_TEXT
_Show_PF	proc	near
	push	bp
	mov	bp,sp
	sub	sp,6
	push	si
	push	di
   ;	
   ;	{
   ;		int x,y;
   ;		int p;
   ;		BYTE far *PFg=Player[Game.plr].PFg;
   ;	
	mov	ax,word ptr DGROUP:_Game+9
	mov	dx,14
	imul	dx
	mov	bx,ax
	mov	ax,word ptr DGROUP:_Player[bx+5]
	mov	dx,word ptr DGROUP:_Player[bx+3]
	mov	word ptr [bp-4],ax
	mov	word ptr [bp-6],dx
   ;	
   ;		for(y=0;y<PFYD;y++)
   ;	
	mov	word ptr [bp-2],0
	jmp	@85@870
@85@58:
   ;	
   ;		for(x=0;x<PFXD;x++)
   ;	
	xor	di,di
	jmp	@85@814
@85@86:
   ;	
   ;		{
   ;			switch(PFg[y*PFXD+x])
   ;	
	mov	ax,word ptr [bp-2]
	mov	dx,31
	imul	dx
	add	ax,di
	mov	es,word ptr [bp-4]
	add	ax,word ptr [bp-6]
	mov	bx,ax
	mov	al,byte ptr es:[bx]
	mov	ah,0
	sub	ax,97
	mov	bx,ax
	cmp	bx,24
	jbe	@@20
	jmp	@85@730
@@20:
	shl	bx,1
	jmp	word ptr cs:@85@C1042[bx]
@85@170:
   ;	
   ;			{
   ;				case 'h':p=0;break;
   ;	
	xor	si,si
	jmp	short @85@758
@85@198:
   ;	
   ;				case 'v':p=5;break;
   ;	
	mov	si,5
	jmp	short @85@758
@85@226:
   ;	
   ;				case 'a':p=1;break;
   ;	
	mov	si,1
	jmp	short @85@758
@85@254:
   ;	
   ;				case 'b':p=2;break;
   ;	
	mov	si,2
	jmp	short @85@758
@85@282:
   ;	
   ;				case 'c':p=3;break;
   ;	
	mov	si,3
	jmp	short @85@758
@85@310:
   ;	
   ;				case 'd':p=4;break;
   ;	
	mov	si,4
	jmp	short @85@758
@85@338:
   ;	
   ;				case 'e':p=9;break;
   ;	
	mov	si,9
	jmp	short @85@758
@85@366:
   ;	
   ;				case 'f':p=8;break;
   ;	
	mov	si,8
	jmp	short @85@758
@85@394:
   ;	
   ;				case 'g':p=6;break;
   ;	
	mov	si,6
	jmp	short @85@758
@85@422:
   ;	
   ;				case 'i':p=7;break;
   ;	
	mov	si,7
	jmp	short @85@758
@85@450:
   ;	
   ;				case 'j':p=15;break;
   ;	
	mov	si,15
	jmp	short @85@758
@85@478:
   ;	
   ;				case 'k':p=14;break;
   ;	
	mov	si,14
	jmp	short @85@758
@85@506:
   ;	
   ;				case 'l':p=12;break;
   ;	
	mov	si,12
	jmp	short @85@758
@85@534:
   ;	
   ;				case 'm':p=13;break;
   ;	
	mov	si,13
	jmp	short @85@758
@85@562:
   ;	
   ;				case 'n':p=16;break;
   ;	
	mov	si,16
	jmp	short @85@758
@85@590:
   ;	
   ;				case 'p':p=10;break;
   ;	
	mov	si,10
	jmp	short @85@758
@85@618:
   ;	
   ;				case 'q':p=17;break;
   ;	
	mov	si,17
	jmp	short @85@758
@85@646:
   ;	
   ;				case 'r':p=11;break;
   ;	
	mov	si,11
	jmp	short @85@758
@85@674:
   ;	
   ;				case 'x':p=19;break;
   ;	
	mov	si,19
	jmp	short @85@758
@85@702:
   ;	
   ;				case 'y':p=20;break;
   ;	
	mov	si,20
	jmp	short @85@758
@85@730:
   ;	
   ;				default:p=18;
   ;	
	mov	si,18
@85@758:
   ;	
   ;			}
   ;			DrawPicF(x*10+PFXI,y*10+PFYI,PFp[p]);
   ;	
	mov	bx,si
	shl	bx,1
	shl	bx,1
	push	word ptr DGROUP:_PFp[bx+2]
	push	word ptr DGROUP:_PFp[bx]
	mov	ax,word ptr [bp-2]
	mov	dx,10
	imul	dx
	push	ax
	mov	ax,di
	mov	dx,10
	imul	dx
	add	ax,5
	push	ax
	call	near ptr _DrawPicF
	add	sp,8
	inc	di
@85@814:
	cmp	di,31
	jge	@@21
	jmp	@85@86
@@21:
	inc	word ptr [bp-2]
@85@870:
	cmp	word ptr [bp-2],17
	jge	@@22
	jmp	@85@58
@@22:
   ;	
   ;		}
   ;	
   ;	}
   ;	
	pop	di
	pop	si
	mov	sp,bp
	pop	bp
	ret	
_Show_PF	endp
@85@C1042	label	word
	dw	@85@226
	dw	@85@254
	dw	@85@282
	dw	@85@310
	dw	@85@338
	dw	@85@366
	dw	@85@394
	dw	@85@170
	dw	@85@422
	dw	@85@450
	dw	@85@478
	dw	@85@506
	dw	@85@534
	dw	@85@562
	dw	@85@730
	dw	@85@590
	dw	@85@618
	dw	@85@646
	dw	@85@730
	dw	@85@730
	dw	@85@730
	dw	@85@198
	dw	@85@730
	dw	@85@674
	dw	@85@702
   ;	
   ;	void Vsync()
   ;	
	assume	cs:_TEXT
_Vsync	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;		asm               mov dx,0x3DA
   ;	
               	mov	 dx,03DAH
@86@58:
   ;	
   ;			 uno:
   ;		asm               in  al,dx
   ;	
               	in	  al,dx
   ;	
   ;		asm               and al,0x8
   ;	
               	and	 al,08H
   ;	
   ;		asm               jnz uno
   ;	
	jne	short @86@58
@86@142:
   ;	
   ;			 dos:
   ;		asm               in  al,dx
   ;	
               	in	  al,dx
   ;	
   ;		asm               and al,0x8
   ;	
               	and	 al,08H
   ;	
   ;		asm               jz  dos
   ;	
	je	short @86@142
   ;	
   ;	
   ;	}
   ;	
	pop	bp
	ret	
_Vsync	endp
   ;	
   ;	unsigned char inportb(unsigned p)
   ;	
	assume	cs:_TEXT
_inportb	proc	near
	push	bp
	mov	bp,sp
	sub	sp,2
   ;	
   ;	{
   ;		unsigned char v;
   ;		asm {
   ;			mov dx, p;
   ;	
	mov	 dx, [bp+4]
   ;	
   ;			in al, dx;
   ;	
	in	 al, dx
   ;	
   ;			mov v, al;
   ;	
	mov	 [bp-1], al
   ;	
   ;		}
   ;		return v;
   ;	
	mov	al,byte ptr [bp-1]
	jmp	short @87@142
@87@142:
   ;	
   ;	}
   ;	
	mov	sp,bp
	pop	bp
	ret	
_inportb	endp
   ;	
   ;	void interrupt newkeyb(void)
   ;	
	assume	cs:_TEXT
_newkeyb	proc	far
	push	ax
	push	bx
	push	cx
	push	dx
	push	es
	push	ds
	push	si
	push	di
	push	bp
	mov	bp,DGROUP
	mov	ds,bp
	mov	bp,sp
	sub	sp,6
   ;	
   ;	{
   ;	  unsigned char tecla;
   ;	  tecla=inportb(0x60);
   ;	
	mov	ax,96
	push	ax
	call	near ptr _inportb
	pop	cx
	mov	byte ptr [bp-1],al
   ;	
   ;	
   ;	  if (tecla<0x80)
   ;	
	cmp	byte ptr [bp-1],128
	jb	@@23
	jmp	@88@646
@@23:
   ;	
   ;		  switch (tecla)
   ;	
	mov	al,byte ptr [bp-1]
	mov	ah,0
	mov	word ptr [bp-4],ax
	mov	cx,14
	mov	bx,offset @88@C1122
@88@114:
	mov	ax,word ptr cs:[bx]
	cmp	ax,word ptr [bp-4]
	je	short @88@198
	add	bx,2
	loop	short @88@114
	jmp	short @88@618
@88@198:
	jmp	word ptr cs:[bx+28]
@88@226:
   ;	
   ;		  {
   ;		case 30:   StickA[0]=1; break;
   ;	
	mov	byte ptr DGROUP:_StickA,1
	jmp	short @88@618
@88@254:
   ;	
   ;		case 32:   StickA[1]=1; break;
   ;	
	mov	byte ptr DGROUP:_StickA+1,1
	jmp	short @88@618
@88@282:
   ;	
   ;		case 17:   StickA[2]=1; break;
   ;	
	mov	byte ptr DGROUP:_StickA+2,1
	jmp	short @88@618
@88@310:
   ;	
   ;		case 31:   StickA[3]=1; break;
   ;	
	mov	byte ptr DGROUP:_StickA+3,1
	jmp	short @88@618
@88@338:
   ;	
   ;		case 20:   StickA[4]=1; break;
   ;	
	mov	byte ptr DGROUP:_StickA+4,1
	jmp	short @88@618
@88@366:
   ;	
   ;		case 22:   StickA[5]=1; break;
   ;	
	mov	byte ptr DGROUP:_StickA+5,1
	jmp	short @88@618
@88@394:
   ;	
   ;		case 21:   StickA[6]=1; break;
   ;	
	mov	byte ptr DGROUP:_StickA+6,1
	jmp	short @88@618
@88@422:
   ;	
   ;		case 75:   StickB[0]=1; break;
   ;	
	mov	byte ptr DGROUP:_StickB,1
	jmp	short @88@618
@88@450:
   ;	
   ;		case 77:   StickB[1]=1; break;
   ;	
	mov	byte ptr DGROUP:_StickB+1,1
	jmp	short @88@618
@88@478:
   ;	
   ;		case 72:   StickB[2]=1; break;
   ;	
	mov	byte ptr DGROUP:_StickB+2,1
	jmp	short @88@618
@88@506:
   ;	
   ;		case 80:   StickB[3]=1; break;
   ;	
	mov	byte ptr DGROUP:_StickB+3,1
	jmp	short @88@618
@88@534:
   ;	
   ;		case 82:   StickB[4]=1; break;
   ;	
	mov	byte ptr DGROUP:_StickB+4,1
	jmp	short @88@618
@88@562:
   ;	
   ;		case 28:   StickB[5]=1; break;
   ;	
	mov	byte ptr DGROUP:_StickB+5,1
	jmp	short @88@618
@88@590:
   ;	
   ;		case 83:   StickB[6]=1; break;
   ;	
	mov	byte ptr DGROUP:_StickB+6,1
	jmp	short @88@618
@88@618:
   ;	
   ;		  }
   ;	
	jmp	@88@1206
@88@646:
   ;	
   ;	  else
   ;		  switch (tecla-0x80)
   ;	
	mov	al,byte ptr [bp-1]
	mov	ah,0
	add	ax,65408
	mov	word ptr [bp-6],ax
	mov	cx,14
	mov	bx,offset @88@C1218
@88@702:
	mov	ax,word ptr cs:[bx]
	cmp	ax,word ptr [bp-6]
	je	short @88@786
	add	bx,2
	loop	short @88@702
	jmp	short @88@1206
@88@786:
	jmp	word ptr cs:[bx+28]
@88@814:
   ;	
   ;		  {
   ;		case 30:   StickA[0]=0; break;
   ;	
	mov	byte ptr DGROUP:_StickA,0
	jmp	short @88@1206
@88@842:
   ;	
   ;		case 32:   StickA[1]=0; break;
   ;	
	mov	byte ptr DGROUP:_StickA+1,0
	jmp	short @88@1206
@88@870:
   ;	
   ;		case 17:   StickA[2]=0; break;
   ;	
	mov	byte ptr DGROUP:_StickA+2,0
	jmp	short @88@1206
@88@898:
   ;	
   ;		case 31:   StickA[3]=0; break;
   ;	
	mov	byte ptr DGROUP:_StickA+3,0
	jmp	short @88@1206
@88@926:
   ;	
   ;		case 20:   StickA[4]=0; break;
   ;	
	mov	byte ptr DGROUP:_StickA+4,0
	jmp	short @88@1206
@88@954:
   ;	
   ;		case 22:   StickA[5]=0; break;
   ;	
	mov	byte ptr DGROUP:_StickA+5,0
	jmp	short @88@1206
@88@982:
   ;	
   ;		case 21:   StickA[6]=0; break;
   ;	
	mov	byte ptr DGROUP:_StickA+6,0
	jmp	short @88@1206
@88@1010:
   ;	
   ;		case 75:   StickB[0]=0; break;
   ;	
	mov	byte ptr DGROUP:_StickB,0
	jmp	short @88@1206
@88@1038:
   ;	
   ;		case 77:   StickB[1]=0; break;
   ;	
	mov	byte ptr DGROUP:_StickB+1,0
	jmp	short @88@1206
@88@1066:
   ;	
   ;		case 72:   StickB[2]=0; break;
   ;	
	mov	byte ptr DGROUP:_StickB+2,0
	jmp	short @88@1206
@88@1094:
   ;	
   ;		case 80:   StickB[3]=0; break;
   ;	
	mov	byte ptr DGROUP:_StickB+3,0
	jmp	short @88@1206
@88@1122:
   ;	
   ;		case 82:   StickB[4]=0; break;
   ;	
	mov	byte ptr DGROUP:_StickB+4,0
	jmp	short @88@1206
@88@1150:
   ;	
   ;		case 28:   StickB[5]=0; break;
   ;	
	mov	byte ptr DGROUP:_StickB+5,0
	jmp	short @88@1206
@88@1178:
   ;	
   ;		case 83:   StickB[6]=0; break;
   ;	
	mov	byte ptr DGROUP:_StickB+6,0
	jmp	short @88@1206
@88@1206:
   ;	
   ;		}
   ;	
   ;	  oldkeyb();
   ;	
	pushf	
	call	dword ptr DGROUP:_oldkeyb
   ;	
   ;	}
   ;	
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
_newkeyb	endp
@88@C1218	label	word
	db	17
	db	0
	db	20
	db	0
	db	21
	db	0
	db	22
	db	0
	db	28
	db	0
	db	30
	db	0
	db	31
	db	0
	db	32
	db	0
	db	72
	db	0
	db	75
	db	0
	db	77
	db	0
	db	80
	db	0
	db	82
	db	0
	db	83
	db	0
	dw	@88@870
	dw	@88@926
	dw	@88@982
	dw	@88@954
	dw	@88@1150
	dw	@88@814
	dw	@88@898
	dw	@88@842
	dw	@88@1066
	dw	@88@1010
	dw	@88@1038
	dw	@88@1094
	dw	@88@1122
	dw	@88@1178
@88@C1122	label	word
	db	17
	db	0
	db	20
	db	0
	db	21
	db	0
	db	22
	db	0
	db	28
	db	0
	db	30
	db	0
	db	31
	db	0
	db	32
	db	0
	db	72
	db	0
	db	75
	db	0
	db	77
	db	0
	db	80
	db	0
	db	82
	db	0
	db	83
	db	0
	dw	@88@282
	dw	@88@338
	dw	@88@394
	dw	@88@366
	dw	@88@562
	dw	@88@226
	dw	@88@310
	dw	@88@254
	dw	@88@478
	dw	@88@422
	dw	@88@450
	dw	@88@506
	dw	@88@534
	dw	@88@590
   ;	
   ;	void LoadAll()
   ;	
	assume	cs:_TEXT
_LoadAll	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;		VirScr=0;
   ;	
	mov	word ptr DGROUP:_VirScr+2,0
	mov	word ptr DGROUP:_VirScr,0
   ;	
   ;		if(!(Player[0].PFg=farmalloc((PFYD*PFXD))))
   ;	
	mov	ax,527
	push	ax
	call	near ptr _farmalloc
	pop	cx
	mov	word ptr DGROUP:_Player+5,dx
	mov	word ptr DGROUP:_Player+3,ax
	or	ax,dx
	jne	short @89@86
   ;	
   ;			LoadError=1;
   ;	
	mov	byte ptr DGROUP:_LoadError,1
@89@86:
   ;	
   ;	
   ;		if(!(Player[1].PFg=farmalloc((PFYD*PFXD))))
   ;	
	mov	ax,527
	push	ax
	call	near ptr _farmalloc
	pop	cx
	mov	word ptr DGROUP:_Player+19,dx
	mov	word ptr DGROUP:_Player+17,ax
	or	ax,dx
	jne	short @89@142
   ;	
   ;			LoadError=1;
   ;	
	mov	byte ptr DGROUP:_LoadError,1
@89@142:
   ;	
   ;		
   ;		
   ;		if(!(VirScr=farmalloc(0xFFFF)))
   ;	
	mov	ax,65535
	push	ax
	call	near ptr _farmalloc
	pop	cx
	mov	word ptr DGROUP:_VirScr+2,dx
	mov	word ptr DGROUP:_VirScr,ax
	or	ax,dx
	jne	short @89@198
   ;	
   ;		{
   ;			printf("Memoria insuficiente!\n");
   ;	
	mov	ax,offset DGROUP:s@+640
	push	ax
	call	near ptr _printf
	pop	cx
   ;	
   ;			LoadError=1;
   ;	
	mov	byte ptr DGROUP:_LoadError,1
@89@198:
   ;	
   ;		}
   ;		
   ;		LoadAllNew();
   ;	
	call	near ptr _LoadAllNew
   ;	
   ;		
   ;		if(LoadError)
   ;	
	cmp	byte ptr DGROUP:_LoadError,0
	je	short @89@254
   ;	
   ;		{
   ;			UnloadAll();
   ;	
	call	near ptr _UnloadAll
   ;	
   ;			exit(1);
   ;	
	mov	ax,1
	push	ax
	call	near ptr _exit
	pop	cx
@89@254:
   ;	
   ;		}
   ;	
   ;	}
   ;	
	pop	bp
	ret	
_LoadAll	endp
   ;	
   ;	void LoadAllNew()
   ;	
	assume	cs:_TEXT
_LoadAllNew	proc	near
	push	bp
	mov	bp,sp
	sub	sp,18
	push	si
	push	di
   ;	
   ;	{
   ;		TResourceMap *rs;
   ;		int count;
   ;		int i=0;
   ;	
	xor	di,di
   ;	
   ;		char buffer[11];
   ;		count=loadResourceFile("pacman.res",rmap,MAXRESOURCES);
   ;	
	mov	ax,50
	push	ax
	mov	ax,offset DGROUP:_rmap
	push	ax
	mov	ax,offset DGROUP:s@+663
	push	ax
	call	near ptr _loadResourceFile
	add	sp,6
	mov	word ptr [bp-2],ax
   ;	
   ;		if(!count) {
   ;	
	cmp	word ptr [bp-2],0
	jne	short @90@86
@90@58:
   ;	
   ;			LoadError=1;
   ;	
	mov	byte ptr DGROUP:_LoadError,1
   ;	
   ;			return;
   ;	
	jmp	@90@1402
@90@86:
   ;	
   ;		}
   ;	
   ;		if(!getResource("RS$PacMan",rmap,MAXRESOURCES)) {
   ;	
	mov	ax,50
	push	ax
	mov	ax,offset DGROUP:_rmap
	push	ax
	mov	ax,offset DGROUP:s@+674
	push	ax
	call	near ptr _getResource
	add	sp,6
	or	ax,ax
	jne	short @90@142
   ;	
   ;			printf("XIT\n");
   ;	
	mov	ax,offset DGROUP:s@+684
	push	ax
	call	near ptr _printf
	pop	cx
   ;	
   ;			LoadError=1;
   ;	
	jmp	short @90@58
@90@142:
   ;	
   ;			return;
   ;		}
   ;		
   ;		if(!getResource("RE$PacMan",rmap,MAXRESOURCES)) {
   ;	
	mov	ax,50
	push	ax
	mov	ax,offset DGROUP:_rmap
	push	ax
	mov	ax,offset DGROUP:s@+689
	push	ax
	call	near ptr _getResource
	add	sp,6
	or	ax,ax
	jne	short @90@198
	jmp	short @90@58
@90@198:
   ;	
   ;			LoadError=1;
   ;			return;
   ;		}
   ;	
   ;		if(!(rs=getResource("MusicMidi",rmap,MAXRESOURCES))) {
   ;	
	mov	ax,50
	push	ax
	mov	ax,offset DGROUP:_rmap
	push	ax
	mov	ax,offset DGROUP:s@+699
	push	ax
	call	near ptr _getResource
	add	sp,6
	mov	si,ax
	or	ax,ax
	jne	short @90@282
   ;	
   ;			printf("Resource not found MusicMidi\n");
   ;	
	mov	ax,offset DGROUP:s@+709
@90@254:
	push	ax
	call	near ptr _printf
	pop	cx
	jmp	short @90@58
@90@282:
   ;	
   ;			LoadError=1;
   ;			return;
   ;		}
   ;		MusSeg=rs->ptr;
   ;	
	mov	ax,word ptr [si+15]
	mov	dx,word ptr [si+13]
	mov	word ptr DGROUP:_MusSeg+2,ax
	mov	word ptr DGROUP:_MusSeg,dx
   ;	
   ;		MusicLen=rs->size;
   ;	
	mov	ax,word ptr [si+11]
	mov	word ptr DGROUP:_MusicLen,ax
   ;	
   ;		
   ;		if(!(rs=getResource("PlayField",rmap,MAXRESOURCES))) {
   ;	
	mov	ax,50
	push	ax
	mov	ax,offset DGROUP:_rmap
	push	ax
	mov	ax,offset DGROUP:s@+739
	push	ax
	call	near ptr _getResource
	add	sp,6
	mov	si,ax
	or	ax,ax
	jne	short @90@338
   ;	
   ;			printf("Resource not found PlayField\n");
   ;	
	mov	ax,offset DGROUP:s@+749
	jmp	short @90@254
@90@338:
   ;	
   ;			LoadError=1;
   ;			return;
   ;		}
   ;		LPFg=rs->ptr;
   ;	
	mov	ax,word ptr [si+15]
	mov	dx,word ptr [si+13]
	mov	word ptr DGROUP:_LPFg+2,ax
	mov	word ptr DGROUP:_LPFg,dx
   ;	
   ;		if(PFXD*PFYD!=rs->size)
   ;	
	cmp	word ptr [si+11],527
	je	short @90@394
   ;	
   ;		{
   ;			printf("Wrong playfield data size expected %u got %u\n", PFXD*PFYD, rs->size);
   ;	
	push	word ptr [si+11]
	mov	ax,527
	push	ax
	mov	ax,offset DGROUP:s@+779
	jmp	@90@1094
@90@394:
   ;	
   ;			LoadError=1;
   ;			return;
   ;		}
   ;	
   ;		if(!(rs=getResource("BonusPic00",rmap,MAXRESOURCES))) {
   ;	
	mov	ax,50
	push	ax
	mov	ax,offset DGROUP:_rmap
	push	ax
	mov	ax,offset DGROUP:s@+825
	push	ax
	call	near ptr _getResource
	add	sp,6
	mov	si,ax
	or	ax,ax
	jne	short @90@450
   ;	
   ;			printf("Resource not found BonusPic00\n");
   ;	
	mov	ax,offset DGROUP:s@+836
	jmp	short @90@254
@90@450:
   ;	
   ;			LoadError=1;
   ;			return;
   ;		}
   ;		Bp=rs->ptr;
   ;	
	mov	ax,word ptr [si+15]
	mov	dx,word ptr [si+13]
	mov	word ptr DGROUP:_Bp+2,ax
	mov	word ptr DGROUP:_Bp,dx
   ;	
   ;		
   ;		for(i=0;i<12;i++)
   ;	
	xor	di,di
	jmp	short @90@590
@90@478:
   ;	
   ;		{
   ;			strcpy(buffer,"PacPic00");
   ;	
	mov	ax,offset DGROUP:s@+867
	push	ax
	lea	ax,word ptr [bp-18]
	push	ax
	call	near ptr _strcpy
	pop	cx
	pop	cx
   ;	
   ;			buffer[6]=(i/10)%10 + '0';
   ;	
	mov	ax,di
	mov	bx,10
	cwd	
	idiv	bx
	mov	bx,10
	cwd	
	idiv	bx
	add	dl,48
	mov	byte ptr [bp-12],dl
   ;	
   ;			buffer[7]=i%10 + '0';
   ;	
	mov	ax,di
	mov	bx,10
	cwd	
	idiv	bx
	add	dl,48
	mov	byte ptr [bp-11],dl
   ;	
   ;			if(!(rs=getResource(buffer,rmap,MAXRESOURCES))) {
   ;	
	mov	ax,50
	push	ax
	mov	ax,offset DGROUP:_rmap
	push	ax
	lea	ax,word ptr [bp-18]
	push	ax
	call	near ptr _getResource
	add	sp,6
	mov	si,ax
	or	ax,ax
	jne	short @90@534
   ;	
   ;				printf("Resource not found %s\n", buffer);
   ;	
	lea	ax,word ptr [bp-18]
	push	ax
	mov	ax,offset DGROUP:s@+876
	jmp	@90@870
@90@534:
   ;	
   ;				LoadError=1;
   ;				return;
   ;			}
   ;			Pac.pic[i]=rs->ptr;
   ;	
	mov	ax,word ptr [si+15]
	mov	dx,word ptr [si+13]
	mov	bx,di
	shl	bx,1
	shl	bx,1
	mov	word ptr DGROUP:_Pac[bx+15],ax
	mov	word ptr DGROUP:_Pac[bx+13],dx
	inc	di
@90@590:
	cmp	di,12
	jl	short @90@478
   ;	
   ;		}
   ;	
   ;		for(i=0;i<6;i++)
   ;	
	xor	di,di
	jmp	short @90@758
@90@646:
   ;	
   ;		{
   ;			strcpy(buffer,"GhostPic00");
   ;	
	mov	ax,offset DGROUP:s@+899
	push	ax
	lea	ax,word ptr [bp-18]
	push	ax
	call	near ptr _strcpy
	pop	cx
	pop	cx
   ;	
   ;			buffer[8]=(i/10)%10 + '0';
   ;	
	mov	ax,di
	mov	bx,10
	cwd	
	idiv	bx
	mov	bx,10
	cwd	
	idiv	bx
	add	dl,48
	mov	byte ptr [bp-10],dl
   ;	
   ;			buffer[9]=i%10 + '0';
   ;	
	mov	ax,di
	mov	bx,10
	cwd	
	idiv	bx
	add	dl,48
	mov	byte ptr [bp-9],dl
   ;	
   ;			if(!(rs=getResource(buffer,rmap,MAXRESOURCES))) {
   ;	
	mov	ax,50
	push	ax
	mov	ax,offset DGROUP:_rmap
	push	ax
	lea	ax,word ptr [bp-18]
	push	ax
	call	near ptr _getResource
	add	sp,6
	mov	si,ax
	or	ax,ax
	jne	short @90@702
   ;	
   ;				printf("Resource not found %s\n", buffer);
   ;	
	lea	ax,word ptr [bp-18]
	push	ax
	mov	ax,offset DGROUP:s@+910
	jmp	short @90@870
@90@702:
   ;	
   ;				LoadError=1;
   ;				return;
   ;			}
   ;			Gp[i]=rs->ptr;
   ;	
	mov	ax,word ptr [si+15]
	mov	dx,word ptr [si+13]
	mov	bx,di
	shl	bx,1
	shl	bx,1
	mov	word ptr DGROUP:_Gp[bx+2],ax
	mov	word ptr DGROUP:_Gp[bx],dx
	inc	di
@90@758:
	cmp	di,6
	jl	short @90@646
   ;	
   ;		}
   ;		
   ;		for(i=0;i<21;i++)
   ;	
	xor	di,di
	jmp	short @90@954
@90@814:
   ;	
   ;		{
   ;			strcpy(buffer,"WallPic00");
   ;	
	mov	ax,offset DGROUP:s@+933
	push	ax
	lea	ax,word ptr [bp-18]
	push	ax
	call	near ptr _strcpy
	pop	cx
	pop	cx
   ;	
   ;			buffer[7]=(i/10)%10 + '0';
   ;	
	mov	ax,di
	mov	bx,10
	cwd	
	idiv	bx
	mov	bx,10
	cwd	
	idiv	bx
	add	dl,48
	mov	byte ptr [bp-11],dl
   ;	
   ;			buffer[8]=i%10 + '0';
   ;	
	mov	ax,di
	mov	bx,10
	cwd	
	idiv	bx
	add	dl,48
	mov	byte ptr [bp-10],dl
   ;	
   ;	
   ;			if(!(rs=getResource(buffer,rmap,MAXRESOURCES))) {
   ;	
	mov	ax,50
	push	ax
	mov	ax,offset DGROUP:_rmap
	push	ax
	lea	ax,word ptr [bp-18]
	push	ax
	call	near ptr _getResource
	add	sp,6
	mov	si,ax
	or	ax,ax
	jne	short @90@898
   ;	
   ;				printf("Resource not found %s\n", buffer);
   ;	
	lea	ax,word ptr [bp-18]
	push	ax
	mov	ax,offset DGROUP:s@+943
@90@870:
	push	ax
	call	near ptr _printf
	pop	cx
	pop	cx
	jmp	@90@58
@90@898:
   ;	
   ;				LoadError=1;
   ;				return;
   ;			}
   ;			PFp[i]=rs->ptr;
   ;	
	mov	ax,word ptr [si+15]
	mov	dx,word ptr [si+13]
	mov	bx,di
	shl	bx,1
	shl	bx,1
	mov	word ptr DGROUP:_PFp[bx+2],ax
	mov	word ptr DGROUP:_PFp[bx],dx
	inc	di
@90@954:
	cmp	di,21
	jl	short @90@814
   ;	
   ;		}
   ;	
   ;		if(!(rs=getResource("Paleta",rmap,MAXRESOURCES))) {
   ;	
	mov	ax,50
	push	ax
	mov	ax,offset DGROUP:_rmap
	push	ax
	mov	ax,offset DGROUP:s@+966
	push	ax
	call	near ptr _getResource
	add	sp,6
	mov	si,ax
	or	ax,ax
	jne	short @90@1038
   ;	
   ;			printf("Resource not found Paleta\n");
   ;	
	mov	ax,offset DGROUP:s@+973
	jmp	@90@254
@90@1038:
   ;	
   ;			LoadError=1;
   ;			return;
   ;		}
   ;		
   ;		palette=rs->ptr;
   ;	
	mov	ax,word ptr [si+15]
	mov	dx,word ptr [si+13]
	mov	word ptr DGROUP:_palette+2,ax
	mov	word ptr DGROUP:_palette,dx
   ;	
   ;		if(rs->size!=256*sizeof(Trgb)) {
   ;	
	cmp	word ptr [si+11],1024
	je	short @90@1122
   ;	
   ;			printf("Wrong palette data size expected %u got %u\n",256*sizeof(Trgb), rs->size);
   ;	
	push	word ptr [si+11]
	mov	ax,1024
	push	ax
	mov	ax,offset DGROUP:s@+1000
@90@1094:
	push	ax
	call	near ptr _printf
	add	sp,6
	jmp	@90@58
@90@1122:
   ;	
   ;			LoadError=1;
   ;			return;
   ;		}
   ;		
   ;		//LoadPic(&Pac.pic[0],"pics/gpic0.bmp");
   ;		
   ;		printf("Resources Loaded %d\n", count);
   ;	
	push	word ptr [bp-2]
	mov	ax,offset DGROUP:s@+1044
	push	ax
	call	near ptr _printf
	pop	cx
	pop	cx
   ;	
   ;		{
   ;			int x,y;
   ;			PF_dotsL=0;
   ;	
	mov	word ptr DGROUP:_PF_dotsL,0
   ;	
   ;			for(y=0;y<PFYD;y++)
   ;	
	mov	word ptr [bp-4],0
	jmp	short @90@1374
@90@1178:
   ;	
   ;			{
   ;				for(x=0;x<PFXD;x++)
   ;	
	xor	cx,cx
	jmp	short @90@1318
@90@1206:
   ;	
   ;				{
   ;					unsigned char caracter = LPFg[y*PFXD+x];
   ;	
	mov	ax,word ptr [bp-4]
	mov	dx,31
	imul	dx
	add	ax,cx
	mov	es,word ptr DGROUP:_LPFg+2
	add	ax,word ptr DGROUP:_LPFg
	mov	bx,ax
	mov	al,byte ptr es:[bx]
	mov	byte ptr [bp-5],al
   ;	
   ;					if((caracter=='x')||(caracter=='y')) PF_dotsL++;
   ;	
	cmp	byte ptr [bp-5],120
	je	short @90@1262
	cmp	byte ptr [bp-5],121
	jne	short @90@1290
@90@1262:
	inc	word ptr DGROUP:_PF_dotsL
@90@1290:
	inc	cx
@90@1318:
	cmp	cx,31
	jl	short @90@1206
	inc	word ptr [bp-4]
@90@1374:
	cmp	word ptr [bp-4],17
	jl	short @90@1178
@90@1402:
   ;	
   ;				}
   ;			}
   ;		}
   ;	}
   ;	
	pop	di
	pop	si
	mov	sp,bp
	pop	bp
	ret	
_LoadAllNew	endp
   ;	
   ;	void UnloadAll()
   ;	
	assume	cs:_TEXT
_UnloadAll	proc	near
	push	bp
	mov	bp,sp
	push	si
   ;	
   ;	{
   ;		int i;
   ;		if(Player[0].PFg)
   ;	
	mov	ax,word ptr DGROUP:_Player+3
	or	ax,word ptr DGROUP:_Player+5
	je	short @91@86
   ;	
   ;			farfree(Player[0].PFg);
   ;	
	push	word ptr DGROUP:_Player+5
	push	word ptr DGROUP:_Player+3
	call	near ptr _farfree
	pop	cx
	pop	cx
@91@86:
   ;	
   ;		if(Player[1].PFg)
   ;	
	mov	ax,word ptr DGROUP:_Player+17
	or	ax,word ptr DGROUP:_Player+19
	je	short @91@142
   ;	
   ;			farfree(Player[1].PFg);
   ;	
	push	word ptr DGROUP:_Player+19
	push	word ptr DGROUP:_Player+17
	call	near ptr _farfree
	pop	cx
	pop	cx
@91@142:
   ;	
   ;		if(VirScr) farfree(VirScr);
   ;	
	mov	ax,word ptr DGROUP:_VirScr
	or	ax,word ptr DGROUP:_VirScr+2
	je	short @91@198
	push	word ptr DGROUP:_VirScr+2
	push	word ptr DGROUP:_VirScr
	call	near ptr _farfree
	pop	cx
	pop	cx
@91@198:
   ;	
   ;		
   ;		for(i=0;i<MAXRESOURCES;i++)
   ;	
	xor	si,si
	jmp	short @91@310
@91@226:
   ;	
   ;		{
   ;			if(rmap[i].ptr)
   ;	
	mov	ax,si
	mov	dx,17
	imul	dx
	mov	bx,ax
	mov	ax,word ptr DGROUP:_rmap[bx+13]
	or	ax,word ptr DGROUP:_rmap[bx+15]
	je	short @91@282
   ;	
   ;				farfree(rmap[i].ptr);
   ;	
	mov	ax,si
	mov	dx,17
	imul	dx
	mov	bx,ax
	push	word ptr DGROUP:_rmap[bx+15]
	push	word ptr DGROUP:_rmap[bx+13]
	call	near ptr _farfree
	pop	cx
	pop	cx
@91@282:
	inc	si
@91@310:
	cmp	si,50
	jl	short @91@226
   ;	
   ;		}
   ;		//return NULL;
   ;	
   ;	}
   ;	
	pop	si
	pop	bp
	ret	
_UnloadAll	endp
_TEXT	ends
_BSS	segment word public 'BSS'
_rmap	label	word
	db	850 dup (?)
_SM	label	word
	db	7 dup (?)
_PF_dotsL	label	word
	db	2 dup (?)
_PopUp	label	word
	db	2 dup (?)
_Snd	label	word
	db	6 dup (?)
_oldkeyb	label	dword
	db	4 dup (?)
_MusSeg	label	dword
	db	4 dup (?)
_VirScr	label	dword
	db	4 dup (?)
_StickB	label	byte
	db	7 dup (?)
_StickA	label	byte
	db	7 dup (?)
_Game	label	word
	db	11 dup (?)
_Pac	label	word
	db	61 dup (?)
_Bp	label	dword
	db	4 dup (?)
_Gp	label	dword
	db	24 dup (?)
_PFp	label	dword
	db	84 dup (?)
_Player	label	word
	db	28 dup (?)
_Ghost	label	word
	db	64 dup (?)
	?debug	C E9
_BSS	ends
_DATA	segment word public 'DATA'
s@	label	byte
	db	'Cannot open resource file %s'
	db	10
	db	0
	db	'Out of memory %u'
	db	13
	db	10
	db	0
	db	'Paleta'
	db	0
	db	'Resource not found Paleta'
	db	10
	db	0
	db	'Wrong palette data size expected %u got %u'
	db	10
	db	0
	db	'BonusPic00'
	db	0
	db	'Resource not found BonusPic00'
	db	10
	db	0
	db	'PacPic00'
	db	0
	db	'Resource not found %s'
	db	10
	db	0
	db	'GhostPic00'
	db	0
	db	'Resource not found %s'
	db	10
	db	0
	db	'WallPic00'
	db	0
	db	'Resource not found %s'
	db	10
	db	0
	db	'%X %X'
	db	10
	db	0
	db	'%X %X'
	db	10
	db	0
	db	'VERSAO: '
	db	0
	db	'AUTOR: '
	db	0
	db	'DATA: '
	db	0
	db	'1.0'
	db	0
	db	'RENATO LOURENCO'
	db	0
	db	'FEVEREIRO 2002'
	db	0
	db	'VERSAO: '
	db	0
	db	'AUTOR: '
	db	0
	db	'DATA: '
	db	0
	db	'1.0'
	db	0
	db	'RENATO LOURENCO'
	db	0
	db	'FEVEREIRO 2002'
	db	0
	db	'P A C   M A N'
	db	0
	db	'P A C   M A N'
	db	0
	db	'MENU'
	db	0
	db	'MENU'
	db	0
	db	'JOGAR'
	db	0
	db	'SETUP'
	db	0
	db	'AJUDA'
	db	0
	db	'SOBRE'
	db	0
	db	'SAIR'
	db	0
	db	'SETUP'
	db	0
	db	'SETUP'
	db	0
	db	'NIVEL'
	db	0
	db	'PLAYERS'
	db	0
	db	'MUSICA'
	db	0
	db	'SOM'
	db	0
	db	'ACEITAR'
	db	0
	db	'CANCELAR'
	db	0
	db	'Hello World!'
	db	13
	db	10
	db	0
	db	'Hello World!'
	db	0
	db	'pacman.res'
	db	0
	db	'Press a key'
	db	13
	db	10
	db	0
	db	'CONTINUAR'
	db	0
	db	'MUSICA'
	db	0
	db	'SOM'
	db	0
	db	'TERMINAR'
	db	0
	db	'Pac Man'
	db	0
	db	'Pac Man'
	db	0
	db	'Game Over'
	db	0
	db	'Game Over'
	db	0
	db	'Memoria insuficiente!'
	db	10
	db	0
	db	'pacman.res'
	db	0
	db	'RS$PacMan'
	db	0
	db	'XIT'
	db	10
	db	0
	db	'RE$PacMan'
	db	0
	db	'MusicMidi'
	db	0
	db	'Resource not found MusicMidi'
	db	10
	db	0
	db	'PlayField'
	db	0
	db	'Resource not found PlayField'
	db	10
	db	0
	db	'Wrong playfield data size expected %u got %u'
	db	10
	db	0
	db	'BonusPic00'
	db	0
	db	'Resource not found BonusPic00'
	db	10
	db	0
	db	'PacPic00'
	db	0
	db	'Resource not found %s'
	db	10
	db	0
	db	'GhostPic00'
	db	0
	db	'Resource not found %s'
	db	10
	db	0
	db	'WallPic00'
	db	0
	db	'Resource not found %s'
	db	10
	db	0
	db	'Paleta'
	db	0
	db	'Resource not found Paleta'
	db	10
	db	0
	db	'Wrong palette data size expected %u got %u'
	db	10
	db	0
	db	'Resources Loaded %d'
	db	10
	db	0
_DATA	ends
_TEXT	segment byte public 'CODE'
_TEXT	ends
	extrn	_farfree:near
	extrn	_exit:near
	public	_LoadAllNew
	public	_inportb
	public	_writenumber
	extrn	_sound:near
	public	_mainx
	extrn	_nosound:near
	extrn	_newkeyb2:near
	public	_main
	public	_drawpics
	public	_DrawNextPic
	extrn	_strcmp:near
	extrn	_strcpy:near
	public	_setpal
	public	_setpali
	public	_getResource
	public	_loadResourceFile
	public	_palette
	public	_rmap
	public	_mrandom
	public	_SM
	public	_PF_dotsL
	public	_PopUp
	public	_SDir
	public	_VDir
	public	_Snd
	public	_nivel
	public	_pl
	public	_musica
	public	_som
	public	_frames
	public	_stl
	public	_NG
	public	_GYI
	public	_GXI
	public	_vcm
	public	_WaitToStart
	public	_TestGO
	public	_CpyPFg
	public	_SetSomOnOff
	public	_NextLevel
	public	_InitLevel
	public	_SetSnd
	public	_SndC
	public	_NextPlayer
	public	_ControlPopUp
	public	_ShowPopUp
	public	_BonusColide
	public	_ControlBonus
	public	_ShowBonus
	public	_ShowScore
	public	_BaseColide
	public	_Colides
	public	_GhostSetDV
	public	_GhostSetDH
	public	_MoveGhost
	public	_PlReset
	public	_Kill
	public	_DotColide
	public	_Vsync
	public	_SetDir
	public	_TestDir
	public	_GameControl
	public	_GhostColide
	public	_GhostControl
	public	_PacControl
	public	_Move
	public	_Show_Ghost
	public	_Show_Pac
	public	_Show_PF
	public	_Menu
	public	_Ajuda
	public	_Sobre
	public	_Fundo
	public	_SetupMenu
	public	_newkeyb
	public	_oldkeyb
	public	_AvaliaJoyStick
	public	_GameInit
	public	_UpDateScreen
	public	_Play_game
	public	_UnloadAll
	public	_LoadAll
	public	_LoadError
	public	_MusicLen
	public	_MusSeg
	public	_VirScr
	public	_StickB
	public	_StickA
	public	_Game
	public	_Pac
	public	_Bp
	public	_Gp
	public	_PFp
	public	_LPFg
	public	_Player
	public	_Ghost
	public	_Midi_Resume
	public	_Midi_Pause
	public	_Midi_Status
	public	_Midi_Stop
	public	_Midi_Play
	public	_Midi_GetSynth
	public	_Midi_Init
	public	_SBMIDI
	public	_cart
	public	_ScrnCpyDU
	public	_ScrnCpyUD
	public	_UserScreen
	public	_DefaultScreen
	public	_ScrnT
	public	__fmemcpy
	public	__fmemset
	public	_YPRes
	public	_XPRes
	public	_DrawPicF
	public	_DrawPicB
	public	_DrawPic
	public	_GetPic
	public	_PicSize
	public	_dcircle
	public	_dline
	public	_DrawRect
	public	_DrawPixel
	public	_SetOverScan
	public	_getcpt
	public	_modo3h
	public	_modo13h
	public	_screen
	public	_dscreen
	public	_TextBackGround
	public	_TextColor
	public	_FillColor
	public	_Color
	public	_DRAW
	public	_swap
	public	_fillscreen
	public	_writest
	public	_pchar
	extrn	_farmalloc:near
	extrn	_printf:near
	extrn	_kbhit:near
	extrn	_getch:near
	extrn	_gethseconds:near
	extrn	_mcos_getkey:near
	extrn	_mcos_read:near
	extrn	_mcos_close:near
	extrn	_mcos_open:near
_s@	equ	s@
	end
