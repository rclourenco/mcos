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
	?debug	S "midilibp.c"
	?debug	C E9F5018C4B0A6D6964696C6962702E63
	?debug	C E9AD69464B1E433A5C444556544F4F4C535C54435C494E434C5544+
	?debug	C 455C737464696F2E68
	?debug	C E9AE69464B1E433A5C444556544F4F4C535C54435C494E434C5544+
	?debug	C 455C5F646566732E68
	?debug	C E9AD69464B1E433A5C444556544F4F4C535C54435C494E434C5544+
	?debug	C 455C5F6E756C6C2E68
	?debug	C E9AE69464B1F433A5C444556544F4F4C535C54435C494E434C5544+
	?debug	C 455C7374646C69622E68
	?debug	C E9AD69464B1F433A5C444556544F4F4C535C54435C494E434C5544+
	?debug	C 455C737472696E672E68
	?debug	C E9AE69464B1E433A5C444556544F4F4C535C54435C494E434C5544+
	?debug	C 455C616C6C6F632E68
	?debug	C E9AE69464B1C433A5C444556544F4F4C535C54435C494E434C5544+
	?debug	C 455C646F732E68
	?debug	C E9AD69464B21433A5C444556544F4F4C535C54435C494E434C5544+
	?debug	C 455C7379732F737461742E68
	?debug	C E9AD69464B1E433A5C444556544F4F4C535C54435C494E434C5544+
	?debug	C 455C66636E746C2E68
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
_counter	label	word
	db	0
	db	0
	db	0
	db	0
_player_status	label	word
	db	0
	db	0
_isr	label	dword
	db	0
	db	0
	db	0
	db	0
_DATA	ends
_TEXT	segment byte public 'CODE'
   ;	
   ;	void interrupt irq0handler()
   ;	
	assume	cs:_TEXT
_irq0handler	proc	far
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
   ;	
   ;	{
   ;		midi_processor(counter);
   ;	
	push	word ptr DGROUP:_counter+2
	push	word ptr DGROUP:_counter
	call	near ptr _midi_processor
	pop	cx
	pop	cx
   ;	
   ;		counter++;
   ;	
	add	word ptr DGROUP:_counter,1
	adc	word ptr DGROUP:_counter+2,0
   ;	
   ;		if( (counter%0x40)==0) {
   ;	
	mov	ax,word ptr DGROUP:_counter+2
	mov	dx,word ptr DGROUP:_counter
	and	dx,63
	and	ax,0
	or	dx,ax
	jne	short @1@86
   ;	
   ;			oldirq0handler();
   ;	
	pushf	
	call	dword ptr DGROUP:_oldirq0handler
   ;	
   ;		}
   ;	
	jmp	short @1@142
@1@86:
   ;	
   ;		else {
   ;			asm mov al, 0x20;
   ;	
 	mov	 al, 020H
   ;	
   ;			asm out 0x20, al; 
   ;	
 	out	 020H, al
@1@142:
   ;	
   ;		}
   ;	}
   ;	
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
_irq0handler	endp
   ;	
   ;	void setuptimer()
   ;	
	assume	cs:_TEXT
_setuptimer	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;		asm pushf;
   ;	
 	pushf	
   ;	
   ;		asm cli; 
   ;	
 	cli	
   ;	
   ;		asm mov al,0x0;
   ;	
 	mov	 al,00H
   ;	
   ;		asm out 0x40, al;
   ;	
 	out	 040H, al
   ;	
   ;		asm mov al, 0x4;
   ;	
 	mov	 al, 04H
   ;	
   ;		asm out 0x40, al;
   ;	
 	out	 040H, al
   ;	
   ;	
   ;		oldirq0handler=(__ISR)isr[8];
   ;	
	les	bx,dword ptr DGROUP:_isr
	mov	ax,word ptr es:[bx+34]
	mov	dx,word ptr es:[bx+32]
	mov	word ptr DGROUP:_oldirq0handler+2,ax
	mov	word ptr DGROUP:_oldirq0handler,dx
   ;	
   ;		isr[8]=(IsrVect)MK_FP(_CS,irq0handler);
   ;	
	les	bx,dword ptr DGROUP:_isr
	mov	word ptr es:[bx+34],cs
	mov	word ptr es:[bx+32],offset _irq0handler
   ;	
   ;	
   ;		asm popf;
   ;	
 	popf	
   ;	
   ;	
   ;	}
   ;	
	pop	bp
	ret	
_setuptimer	endp
   ;	
   ;	void closetimer()
   ;	
	assume	cs:_TEXT
_closetimer	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;		asm pushf;
   ;	
 	pushf	
   ;	
   ;		asm cli;
   ;	
 	cli	
   ;	
   ;		isr[8]=(IsrVect)oldirq0handler;
   ;	
	les	bx,dword ptr DGROUP:_isr
	mov	ax,word ptr DGROUP:_oldirq0handler+2
	mov	dx,word ptr DGROUP:_oldirq0handler
	mov	word ptr es:[bx+34],ax
	mov	word ptr es:[bx+32],dx
   ;	
   ;		asm popf;
   ;	
 	popf	
   ;	
   ;	}
   ;	
	pop	bp
	ret	
_closetimer	endp
   ;	
   ;	void mpu_send(unsigned char far *data, unsigned len)
   ;	
	assume	cs:_TEXT
_mpu_send	proc	near
	push	bp
	mov	bp,sp
	push	si
   ;	
   ;	{
   ;		asm {
   ;		  push ds;
   ;	
	push	 ds
   ;	
   ;			mov cx, len;
   ;	
	mov	 cx, [bp+8]
   ;	
   ;			lds si, data;
   ;	
	lds	 si, [bp+4]
   ;	
   ;			mov dx, MPU;
   ;	
	mov	 dx, 0330H
   ;	
   ;			inc dx;
   ;	
	inc	 dx
@4@170:
   ;	
   ;		}
   ;		busy:
   ;		asm {
   ;			in al, dx;
   ;	
	in	 al, dx
   ;	
   ;			test al,0x40;
   ;	
	test	 al,040H
   ;	
   ;			jnz busy;
   ;	
	jne	short @4@170
   ;	
   ;			dec dx;
   ;	
	dec	 dx
   ;	
   ;			
   ;			cld;
   ;	
	cld	
@4@310:
   ;	
   ;		}
   ;		writeloop:
   ;		asm {
   ;			lodsb;
   ;	
	lodsb	
   ;	
   ;			out dx,al;
   ;	
	out	 dx,al
   ;	
   ;			loop writeloop;
   ;	
	loop	short @4@310
   ;	
   ;			pop ds;
   ;	
	pop	 ds
   ;	
   ;		}
   ;	}
   ;	
	pop	si
	pop	bp
	ret	
_mpu_send	endp
   ;	
   ;	unsigned mpu_uart()
   ;	
	assume	cs:_TEXT
_mpu_uart	proc	near
	push	bp
	mov	bp,sp
	sub	sp,2
   ;	
   ;	{
   ;		unsigned r=1;
   ;	
	mov	word ptr [bp-2],1
   ;	
   ;		asm {
   ;			mov dx, MPU;
   ;	
	mov	 dx, 0330H
   ;	
   ;			inc dx;
   ;	
	inc	 dx
@5@114:
   ;	
   ;		}
   ;		busy:
   ;		asm {
   ;			in al, dx;
   ;	
	in	 al, dx
   ;	
   ;			test al,0x40;
   ;	
	test	 al,040H
   ;	
   ;			jnz busy;
   ;	
	jne	short @5@114
   ;	
   ;	
   ;			mov al,0x3f;
   ;	
	mov	 al,03fH
   ;	
   ;			out dx,al;
   ;	
	out	 dx,al
   ;	
   ;	
   ;			xor cx,cx;
   ;	
	xor	 cx,cx
@5@282:
   ;	
   ;		}
   ;		Empty:
   ;		asm {
   ;			in al,dx;
   ;	
	in	 al,dx
   ;	
   ;			test al,0x80;
   ;	
	test	 al,080H
   ;	
   ;			jnz NextLoop;
   ;	
	jne	short @5@506
   ;	
   ;	
   ;			dec dx;
   ;	
	dec	 dx
   ;	
   ;			in al,dx;
   ;	
	in	 al,dx
   ;	
   ;			cmp al,0xFE;
   ;	
	cmp	 al,0FEH
   ;	
   ;			je inUartMode;
   ;	
	je	short @5@562
   ;	
   ;			inc dx;
   ;	
	inc	 dx
@5@506:
   ;	
   ;		}
   ;		NextLoop:
   ;		asm {
   ;			loop Empty;
   ;	
	loop	short @5@282
   ;	
   ;			mov r,0;
   ;	
	mov	 [bp-2],0
@5@562:
   ;	
   ;		}
   ;		inUartMode:
   ;		return r;
   ;	
	mov	ax,word ptr [bp-2]
	jmp	short @5@590
@5@590:
   ;	
   ;	
   ;	}
   ;	
	mov	sp,bp
	pop	bp
	ret	
_mpu_uart	endp
   ;	
   ;	void xdelay()
   ;	
	assume	cs:_TEXT
_xdelay	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;		asm {
   ;			mov cx, 0x0FFF;
   ;	
	mov	 cx, 00FFFH
   ;	
   ;			rep nop;
   ;	
	rep nop	
   ;	
   ;		}
   ;	}
   ;	
	pop	bp
	ret	
_xdelay	endp
   ;	
   ;	void send_midi(unsigned char far *p, unsigned long b)
   ;	
	assume	cs:_TEXT
_send_midi	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;		if(b==0)
   ;	
	mov	ax,word ptr [bp+8]
	or	ax,word ptr [bp+10]
	jne	short @7@86
   ;	
   ;			return;
   ;	
	jmp	short @7@142
@7@86:
   ;	
   ;		if( p[0]!=0xFF ) {
   ;	
	les	bx,dword ptr [bp+4]
	cmp	byte ptr es:[bx],255
	je	short @7@142
   ;	
   ;			mpu_send(p,b);
   ;	
	push	word ptr [bp+8]
	push	word ptr [bp+6]
	push	word ptr [bp+4]
	call	near ptr _mpu_send
	add	sp,6
@7@142:
   ;	
   ;		}
   ;	}
   ;	
	pop	bp
	ret	
_send_midi	endp
   ;	
   ;	unsigned long get_micros()
   ;	
	assume	cs:_TEXT
_get_micros	proc	near
	push	bp
	mov	bp,sp
	sub	sp,4
   ;	
   ;	{
   ;		unsigned ticks;
   ;		unsigned long usecs=0L;
   ;	
	mov	word ptr [bp-2],0
	mov	word ptr [bp-4],0
   ;	
   ;		asm pushf;
   ;	
 	pushf	
   ;	
   ;		asm cli;
   ;	
 	cli	
   ;	
   ;		usecs=counter;
   ;	
	mov	ax,word ptr DGROUP:_counter+2
	mov	dx,word ptr DGROUP:_counter
	mov	word ptr [bp-2],ax
	mov	word ptr [bp-4],dx
   ;	
   ;		asm popf;
   ;	
 	popf	
   ;	
   ;		return mlongmul(usecs,800);
   ;	
	xor	ax,ax
	mov	dx,800
	push	ax
	push	dx
	push	word ptr [bp-2]
	push	word ptr [bp-4]
	call	near ptr _mlongmul
	add	sp,8
	jmp	short @8@198
@8@198:
   ;	
   ;	}
   ;	
	mov	sp,bp
	pop	bp
	ret	
_get_micros	endp
   ;	
   ;	unsigned long get_ticks(MidiPlayer *mp)
   ;	
	assume	cs:_TEXT
_get_ticks	proc	near
	push	bp
	mov	bp,sp
	sub	sp,8
	push	si
	mov	si,word ptr [bp+4]
   ;	
   ;	{
   ;		unsigned long tick;
   ;		unsigned long c1=get_micros();
   ;	
	call	near ptr _get_micros
	mov	word ptr [bp-6],dx
	mov	word ptr [bp-8],ax
   ;	
   ;		if(mp->st==0) {
   ;	
	cmp	byte ptr [si+22],0
	jne	short @9@86
   ;	
   ;			mp->stick=c1;
   ;	
	mov	ax,word ptr [bp-6]
	mov	dx,word ptr [bp-8]
	mov	word ptr [si+8],ax
	mov	word ptr [si+6],dx
   ;	
   ;			mp->st=1;
   ;	
	mov	byte ptr [si+22],1
@9@86:
   ;	
   ;		}
   ;		
   ;		tick=mlongdiv((c1-mp->stick),mp->d, NULL);
   ;	
	xor	ax,ax
	push	ax
	push	word ptr [si+20]
	push	word ptr [si+18]
	mov	ax,word ptr [bp-6]
	mov	dx,word ptr [bp-8]
	sub	dx,word ptr [si+6]
	sbb	ax,word ptr [si+8]
	push	ax
	push	dx
	call	near ptr _mlongdiv
	add	sp,10
	mov	word ptr [bp-2],dx
	mov	word ptr [bp-4],ax
   ;	
   ;	//	tick = (c1-mp->stick)/mp->d;
   ;		return tick; //-mp->stick+mp->etick;
   ;	
	mov	dx,word ptr [bp-2]
	mov	ax,word ptr [bp-4]
	jmp	short @9@114
@9@114:
   ;	
   ;	}
   ;	
	pop	si
	mov	sp,bp
	pop	bp
	ret	
_get_ticks	endp
   ;	
   ;	int midi_playtrack(MidiPlayer *mp, unsigned n, unsigned long ticks)
   ;	
	assume	cs:_TEXT
_midi_playtrack	proc	near
	push	bp
	mov	bp,sp
	push	si
	push	di
	mov	di,word ptr [bp+4]
   ;	
   ;	{
   ;		MidiTrackPlayer *track=&mp->tracks[n];
   ;	
	mov	ax,word ptr [bp+6]
	mov	dx,24
	imul	dx
	mov	dx,word ptr [di+25]
	add	dx,ax
	mov	si,dx
   ;	
   ;	
   ;	  if( track->pos >= track->length )
   ;	
	mov	ax,word ptr [si+14]
	mov	dx,word ptr [si+12]
	cmp	ax,word ptr [si+6]
	jb	short @10@170
	jne	short @10@114
	cmp	dx,word ptr [si+4]
	jb	short @10@170
@10@114:
   ;	
   ;	    return 0;
   ;	
	xor	ax,ax
@10@142:
	jmp	short @10@366
@10@170:
   ;	
   ;		do {
   ;			if(ticks<track->nexttime) {
   ;	
	mov	ax,word ptr [si+10]
	mov	dx,word ptr [si+8]
	cmp	ax,word ptr [bp+10]
	jb	short @10@282
	ja	short @10@254
	cmp	dx,word ptr [bp+8]
	jbe	short @10@282
@10@254:
   ;	
   ;				return 1;
   ;	
	mov	ax,1
	jmp	short @10@142
@10@282:
   ;	
   ;			}
   ;			send_midi(&track->start[track->pos], track->mlen);
   ;	
	push	word ptr [si+18]
	push	word ptr [si+16]
	mov	ax,word ptr [si]
	add	ax,word ptr [si+12]
	push	word ptr [si+2]
	push	ax
	call	near ptr _send_midi
	add	sp,8
   ;	
   ;			//printf("E %u> %lu %lu\n", n, ticks, track->nexttime);
   ;		} while( midi_track_parse_next(track, mp) );
   ;	
	push	di
	push	si
	call	near ptr _midi_track_parse_next
	pop	cx
	pop	cx
	or	ax,ax
	jne	short @10@170
	jmp	short @10@114
@10@366:
   ;	
   ;		return 0;
   ;	}
   ;	
	pop	di
	pop	si
	pop	bp
	ret	
_midi_playtrack	endp
   ;	
   ;	int midi_playtracks(MidiPlayer *mp, unsigned long ticks)
   ;	
	assume	cs:_TEXT
_midi_playtracks	proc	near
	push	bp
	mov	bp,sp
	sub	sp,2
	push	si
	push	di
	mov	di,word ptr [bp+4]
   ;	
   ;	{
   ;		unsigned i;
   ;		unsigned playing=0;
   ;	
	mov	word ptr [bp-2],0
   ;	
   ;		for(i=0;i<mp->ntracks;i++)
   ;	
	xor	si,si
	jmp	short @11@114
@11@58:
   ;	
   ;		{
   ;	//		printf("Track %u ", i);
   ;			playing |= midi_playtrack(mp,i, ticks);
   ;	
	push	word ptr [bp+8]
	push	word ptr [bp+6]
	push	si
	push	di
	call	near ptr _midi_playtrack
	add	sp,8
	or	word ptr [bp-2],ax
	inc	si
@11@114:
	cmp	word ptr [di+23],si
	ja	short @11@58
   ;	
   ;		}
   ;		return playing;
   ;	
	mov	ax,word ptr [bp-2]
	jmp	short @11@170
@11@170:
   ;	
   ;	}
   ;	
	pop	di
	pop	si
	mov	sp,bp
	pop	bp
	ret	
_midi_playtracks	endp
   ;	
   ;	void midi_set_tempo(MidiPlayer *mp, unsigned long tempo)
   ;	
	assume	cs:_TEXT
_midi_set_tempo	proc	near
	push	bp
	mov	bp,sp
	push	si
	mov	si,word ptr [bp+4]
   ;	
   ;	{
   ;		mp->tempo=tempo;
   ;	
	mov	ax,word ptr [bp+8]
	mov	dx,word ptr [bp+6]
	mov	word ptr [si+2],ax
	mov	word ptr [si],dx
   ;	
   ;		
   ;		mp->d = mlongdiv(mp->tempo,mp->ticks_quarter,NULL);
   ;	
	xor	ax,ax
	push	ax
	xor	ax,ax
	push	ax
	push	word ptr [si+4]
	push	word ptr [si+2]
	push	word ptr [si]
	call	near ptr _mlongdiv
	add	sp,10
	mov	word ptr [si+20],dx
	mov	word ptr [si+18],ax
   ;	
   ;		printf("T> %lu %u %lu\n", tempo, mp->ticks_quarter, mp->d);
   ;	
	push	word ptr [si+20]
	push	word ptr [si+18]
	push	word ptr [si+4]
	push	word ptr [bp+8]
	push	word ptr [bp+6]
	mov	ax,offset DGROUP:s@
	push	ax
	call	near ptr _printf
	add	sp,12
   ;	
   ;	}
   ;	
	pop	si
	pop	bp
	ret	
_midi_set_tempo	endp
   ;	
   ;	int midi_track_parse_next(MidiTrackPlayer *mtp, MidiPlayer *mp)
   ;	
	assume	cs:_TEXT
_midi_track_parse_next	proc	near
	push	bp
	mov	bp,sp
	sub	sp,16
	push	si
	push	di
	mov	si,word ptr [bp+4]
   ;	
   ;	{
   ;	  unsigned long delta = 0L, slen = 0L, p;
   ;	
	mov	word ptr [bp-2],0
	mov	word ptr [bp-4],0
	mov	word ptr [bp-6],0
	mov	word ptr [bp-8],0
   ;	
   ;	  unsigned char v,x;
   ;	
   ;	  mtp->pos += mtp->mlen;
   ;	
	mov	ax,word ptr [si+18]
	mov	dx,word ptr [si+16]
	add	word ptr [si+12],dx
	adc	word ptr [si+14],ax
   ;	
   ;	
   ;	  if( mtp->pos >= mtp->length )
   ;	
	mov	ax,word ptr [si+14]
	mov	dx,word ptr [si+12]
	cmp	ax,word ptr [si+6]
	jb	short @13@170
	jne	short @13@114
	cmp	dx,word ptr [si+4]
	jb	short @13@170
@13@114:
   ;	
   ;	    return 0;
   ;	
	xor	ax,ax
@13@142:
	jmp	@13@1094
@13@170:
   ;	
   ;	
   ;	  do {
   ;	    v=mtp->start[mtp->pos];
   ;	
	les	ax,dword ptr [si]
	add	ax,word ptr [si+12]
	mov	bx,ax
	mov	al,byte ptr es:[bx]
	mov	byte ptr [bp-13],al
   ;	
   ;	    delta = mlongmul(delta, 128) + (v&0x7F);
   ;	
	xor	ax,ax
	mov	dx,128
	push	ax
	push	dx
	push	word ptr [bp-2]
	push	word ptr [bp-4]
	call	near ptr _mlongmul
	add	sp,8
	mov	bl,byte ptr [bp-13]
	mov	bh,0
	and	bx,127
	push	ax
	mov	ax,bx
	push	dx
	cwd	
	pop	bx
	pop	cx
	add	cx,ax
	adc	bx,dx
	mov	word ptr [bp-2],bx
	mov	word ptr [bp-4],cx
   ;	
   ;	    mtp->pos++;
   ;	
	add	word ptr [si+12],1
	adc	word ptr [si+14],0
   ;	
   ;	    if( mtp->pos >= mtp->length ) {
   ;	
	mov	ax,word ptr [si+14]
	mov	dx,word ptr [si+12]
	cmp	ax,word ptr [si+6]
	jb	short @13@282
	jne	short @13@254
	cmp	dx,word ptr [si+4]
	jb	short @13@282
@13@254:
	jmp	short @13@114
@13@282:
   ;	
   ;	      return 0;
   ;	    }
   ;	  }while( v&0x80 );
   ;	
	test	byte ptr [bp-13],128
	jne	short @13@170
   ;	
   ;	  mtp->nexttime += delta;
   ;	
	mov	ax,word ptr [bp-2]
	mov	dx,word ptr [bp-4]
	add	word ptr [si+8],dx
	adc	word ptr [si+10],ax
   ;	
   ;	
   ;	  v = mtp->start[mtp->pos];
   ;	
	les	ax,dword ptr [si]
	add	ax,word ptr [si+12]
	mov	bx,ax
	mov	al,byte ptr es:[bx]
	mov	byte ptr [bp-13],al
   ;	
   ;	  switch(v&0xF0)
   ;	
	mov	al,byte ptr [bp-13]
	mov	ah,0
	and	ax,240
	mov	word ptr [bp-16],ax
	mov	cx,8
	mov	bx,offset @13@C994
@13@394:
	mov	ax,word ptr cs:[bx]
	cmp	ax,word ptr [bp-16]
	je	short @13@478
	add	bx,2
	loop	short @13@394
	jmp	@13@1038
@13@478:
	jmp	word ptr cs:[bx+16]
@13@506:
   ;	
   ;	  {
   ;	    case 0x80:
   ;	    case 0x90:
   ;	    case 0xA0:
   ;	    case 0xB0:
   ;	    case 0xE0: mtp->mlen=3; mtp->rlen=2; break;
   ;	
	mov	word ptr [si+18],0
	mov	word ptr [si+16],3
	mov	word ptr [si+22],0
	mov	word ptr [si+20],2
	jmp	@13@1066
@13@534:
   ;	
   ;	    case 0xC0:
   ;	    case 0xD0: mtp->mlen=2; mtp->rlen=1; break;
   ;	
	mov	word ptr [si+18],0
	mov	word ptr [si+16],2
	mov	word ptr [si+22],0
	mov	word ptr [si+20],1
	jmp	@13@1066
@13@562:
   ;	
   ;	    case 0xF0:
   ;	      if(v==0xF0 || v==0xF7 || v==0xFF) {
   ;	
	cmp	byte ptr [bp-13],240
	je	short @13@646
	cmp	byte ptr [bp-13],247
	je	short @13@646
	cmp	byte ptr [bp-13],255
	je	@@0
	jmp	@13@982
@@0:
@13@646:
   ;	
   ;	        p = (v==0xFF) ? 2 : 1;
   ;	
	cmp	byte ptr [bp-13],255
	jne	short @13@702
	mov	ax,2
	jmp	short @13@730
@13@702:
	mov	ax,1
@13@730:
	cwd	
	mov	word ptr [bp-10],dx
	mov	word ptr [bp-12],ax
@13@758:
   ;	
   ;	
   ;	        do {
   ;	          x=mtp->start[mtp->pos+p];
   ;	
	mov	es,word ptr [si+2]
	mov	ax,word ptr [si+12]
	add	ax,word ptr [bp-12]
	add	ax,word ptr [si]
	mov	bx,ax
	mov	al,byte ptr es:[bx]
	mov	byte ptr [bp-14],al
   ;	
   ;	          slen = mlongmul(slen, 128) + (x&0x7F);
   ;	
	xor	ax,ax
	mov	dx,128
	push	ax
	push	dx
	push	word ptr [bp-6]
	push	word ptr [bp-8]
	call	near ptr _mlongmul
	add	sp,8
	mov	bl,byte ptr [bp-14]
	mov	bh,0
	and	bx,127
	push	ax
	mov	ax,bx
	push	dx
	cwd	
	pop	bx
	pop	cx
	add	cx,ax
	adc	bx,dx
	mov	word ptr [bp-6],bx
	mov	word ptr [bp-8],cx
   ;	
   ;	          p++;
   ;	
	add	word ptr [bp-12],1
	adc	word ptr [bp-10],0
   ;	
   ;	        }while(x&0x80);
   ;	
	test	byte ptr [bp-14],128
	jne	short @13@758
   ;	
   ;	        if(v==0xFF && mtp->start[mtp->pos+1]==0x51 && slen==3) {
   ;	
	cmp	byte ptr [bp-13],255
	je	@@1
	jmp	@13@954
@@1:
	mov	es,word ptr [si+2]
	mov	ax,word ptr [si+12]
	add	ax,word ptr [si]
	mov	bx,ax
	cmp	byte ptr es:[bx+1],81
	jne	short @13@954
	cmp	word ptr [bp-6],0
	jne	short @13@954
	cmp	word ptr [bp-8],3
	jne	short @13@954
   ;	
   ;				midi_set_tempo(mp,
   ;	
   ;	
   ;					 mlongmul(mtp->start[mtp->pos+3],0x10000)
   ;					+mlongmul(mtp->start[mtp->pos+4],0x100)
   ;					+mtp->start[mtp->pos+5]);
   ;	
	mov	ax,1
	xor	dx,dx
	push	ax
	push	dx
	mov	es,word ptr [si+2]
	mov	ax,word ptr [si+12]
	add	ax,word ptr [si]
	mov	bx,ax
	mov	al,byte ptr es:[bx+3]
	mov	ah,0
	cwd	
	push	dx
	push	ax
	call	near ptr _mlongmul
	add	sp,8
	push	ax
	push	dx
	xor	ax,ax
	mov	dx,256
	push	ax
	push	dx
	mov	es,word ptr [si+2]
	mov	ax,word ptr [si+12]
	add	ax,word ptr [si]
	mov	bx,ax
	mov	al,byte ptr es:[bx+4]
	mov	ah,0
	cwd	
	push	dx
	push	ax
	call	near ptr _mlongmul
	add	sp,8
	pop	bx
	pop	cx
	add	cx,ax
	adc	bx,dx
	mov	es,word ptr [si+2]
	mov	ax,word ptr [si+12]
	add	ax,word ptr [si]
	mov	di,ax
	mov	al,byte ptr es:[di+5]
	mov	ah,0
	cwd	
	add	cx,ax
	adc	bx,dx
	push	bx
	push	cx
	push	word ptr [bp+6]
	call	near ptr _midi_set_tempo
	add	sp,6
@13@954:
   ;	
   ;			}
   ;	        mtp->mlen = slen+p;
   ;	
	mov	ax,word ptr [bp-6]
	mov	dx,word ptr [bp-8]
	add	dx,word ptr [bp-12]
	adc	ax,word ptr [bp-10]
	mov	word ptr [si+18],ax
	mov	word ptr [si+16],dx
   ;	
   ;	      }
   ;	
	jmp	short @13@1010
@13@982:
   ;	
   ;	      else {
   ;	        mtp->mlen=0;
   ;	
	mov	word ptr [si+18],0
	mov	word ptr [si+16],0
@13@1010:
   ;	
   ;	      }
   ;	    break;
   ;	
	jmp	short @13@1066
@13@1038:
   ;	
   ;	    default: mtp->mlen=mtp->rlen;
   ;	
	mov	ax,word ptr [si+22]
	mov	dx,word ptr [si+20]
	mov	word ptr [si+18],ax
	mov	word ptr [si+16],dx
@13@1066:
   ;	
   ;	  }
   ;	  return 1;
   ;	
	mov	ax,1
	jmp	@13@142
@13@1094:
   ;	
   ;	}
   ;	
	pop	di
	pop	si
	mov	sp,bp
	pop	bp
	ret	
_midi_track_parse_next	endp
@13@C994	label	word
	db	128
	db	0
	db	144
	db	0
	db	160
	db	0
	db	176
	db	0
	db	192
	db	0
	db	208
	db	0
	db	224
	db	0
	db	240
	db	0
	dw	@13@506
	dw	@13@506
	dw	@13@506
	dw	@13@506
	dw	@13@534
	dw	@13@534
	dw	@13@506
	dw	@13@562
   ;	
   ;	void midi_add_track(MidiPlayer *mp, MidiTrackPlayer *mtp, unsigned *tt, void far *ptr, unsigned len)
   ;	
	assume	cs:_TEXT
_midi_add_track	proc	near
	push	bp
	mov	bp,sp
	push	si
	push	di
	mov	di,word ptr [bp+6]
	mov	si,word ptr [bp+8]
   ;	
   ;	{
   ;	  if(*tt<MAXTRACKS)
   ;	
	cmp	word ptr [si],40
	jb	@@2
	jmp	@14@86
@@2:
   ;	
   ;	  {
   ;	    mtp[*tt].start  = (unsigned char far *)ptr;
   ;	
	mov	ax,word ptr [si]
	mov	dx,24
	imul	dx
	mov	dx,word ptr [bp+12]
	mov	bx,word ptr [bp+10]
	push	bx
	mov	bx,ax
	mov	word ptr [bx+di+2],dx
	pop	ax
	mov	word ptr [bx+di],ax
   ;	
   ;	    mtp[*tt].length = len;
   ;	
	mov	ax,word ptr [bp+14]
	push	ax
	mov	ax,word ptr [si]
	mov	dx,24
	imul	dx
	mov	bx,ax
	mov	word ptr [bx+di+6],0
	pop	ax
	mov	word ptr [bx+di+4],ax
   ;	
   ;	    mtp[*tt].nexttime  = 0L;
   ;	
	mov	ax,word ptr [si]
	mov	dx,24
	imul	dx
	mov	bx,ax
	mov	word ptr [bx+di+10],0
	mov	word ptr [bx+di+8],0
   ;	
   ;	    mtp[*tt].pos       = 0L;
   ;	
	mov	ax,word ptr [si]
	mov	dx,24
	imul	dx
	mov	bx,ax
	mov	word ptr [bx+di+14],0
	mov	word ptr [bx+di+12],0
   ;	
   ;	    mtp[*tt].mlen      = 0L;
   ;	
	mov	ax,word ptr [si]
	mov	dx,24
	imul	dx
	mov	bx,ax
	mov	word ptr [bx+di+18],0
	mov	word ptr [bx+di+16],0
   ;	
   ;	    mtp[*tt].rlen      = 0L;
   ;	
	mov	ax,word ptr [si]
	mov	dx,24
	imul	dx
	mov	bx,ax
	mov	word ptr [bx+di+22],0
	mov	word ptr [bx+di+20],0
   ;	
   ;	    midi_track_parse_next(&mtp[*tt], mp);
   ;	
	push	word ptr [bp+4]
	mov	ax,word ptr [si]
	mov	dx,24
	imul	dx
	mov	dx,di
	add	dx,ax
	push	dx
	call	near ptr _midi_track_parse_next
	pop	cx
	pop	cx
   ;	
   ;	    printf("=> Nxt %lu s: %lu p: %lu Cmd %0X\n", 
   ;	
   ;	
   ;	      mtp[*tt].nexttime,
   ;	      mtp[*tt].mlen,
   ;	      mtp[*tt].pos, 
   ;	      mtp[*tt].start[ mtp[*tt].pos ]
   ;	    );
   ;	
	mov	ax,word ptr [si]
	mov	dx,24
	imul	dx
	mov	bx,ax
	les	ax,dword ptr [bx+di]
	push	es
	push	ax
	mov	ax,word ptr [si]
	mov	dx,24
	imul	dx
	mov	bx,ax
	pop	ax
	add	ax,word ptr [bx+di+12]
	mov	bx,ax
	pop	es
	mov	al,byte ptr es:[bx]
	mov	ah,0
	push	ax
	mov	ax,word ptr [si]
	mov	dx,24
	imul	dx
	mov	bx,ax
	push	word ptr [bx+di+14]
	push	word ptr [bx+di+12]
	mov	ax,word ptr [si]
	mov	dx,24
	imul	dx
	mov	bx,ax
	push	word ptr [bx+di+18]
	push	word ptr [bx+di+16]
	mov	ax,word ptr [si]
	mov	dx,24
	imul	dx
	mov	bx,ax
	push	word ptr [bx+di+10]
	push	word ptr [bx+di+8]
	mov	ax,offset DGROUP:s@+15
	push	ax
	call	near ptr _printf
	add	sp,16
   ;	
   ;	    (*tt)++;
   ;	
	inc	word ptr [si]
@14@86:
   ;	
   ;	  }
   ;	}
   ;	
	pop	di
	pop	si
	pop	bp
	ret	
_midi_add_track	endp
   ;	
   ;	void midi_start_iterator(MidiFileIterator *mit, void far *data, unsigned long size)
   ;	
	assume	cs:_TEXT
_midi_start_iterator	proc	near
	push	bp
	mov	bp,sp
	push	si
	mov	si,word ptr [bp+4]
   ;	
   ;	{
   ;	  mit->nextptr=data;
   ;	
	mov	ax,word ptr [bp+8]
	mov	dx,word ptr [bp+6]
	mov	word ptr [si+15],ax
	mov	word ptr [si+13],dx
   ;	
   ;	  mit->total_len=size;
   ;	
	mov	ax,word ptr [bp+12]
	mov	dx,word ptr [bp+10]
	mov	word ptr [si+19],ax
	mov	word ptr [si+17],dx
   ;	
   ;	  mit->cur_track_len=0;
   ;	
	mov	word ptr [si+7],0
	mov	word ptr [si+5],0
   ;	
   ;	  mit->cur_track_type[0]=0;
   ;	
	mov	byte ptr [si],0
   ;	
   ;	  mit->cur_pos=0;
   ;	
	mov	word ptr [si+23],0
	mov	word ptr [si+21],0
   ;	
   ;	  mit->cur_track_ptr=NULL;
   ;	
	mov	word ptr [si+11],0
	mov	word ptr [si+9],0
   ;	
   ;	}
   ;	
	pop	si
	pop	bp
	ret	
_midi_start_iterator	endp
   ;	
   ;	int midi_next_iterator(MidiFileIterator *mit)
   ;	
	assume	cs:_TEXT
_midi_next_iterator	proc	near
	push	bp
	mov	bp,sp
	sub	sp,8
	push	si
	mov	si,word ptr [bp+4]
   ;	
   ;	{
   ;	  if(mit->cur_pos<mit->total_len)
   ;	
	mov	ax,word ptr [si+23]
	mov	dx,word ptr [si+21]
	cmp	ax,word ptr [si+19]
	jbe	@@3
	jmp	@16@170
@@3:
	jb	short @16@114
	cmp	dx,word ptr [si+17]
	jb	@@4
	jmp	@16@170
@@4:
@16@114:
   ;	
   ;	  {
   ;	    unsigned long offset;
   ;	    MidiFilePrefix far *mfp = (MidiFilePrefix far *)mit->nextptr;
   ;	
	mov	ax,word ptr [si+15]
	mov	dx,word ptr [si+13]
	mov	word ptr [bp-6],ax
	mov	word ptr [bp-8],dx
   ;	
   ;	    mit->cur_track_len = (mfp->length[0]<<24) + (mfp->length[1]<<16) + (mfp->length[2]<<8) + mfp->length[3];
   ;	
	les	bx,dword ptr [bp-8]
	mov	al,byte ptr es:[bx+4]
	mov	ah,0
	mov	cl,24
	shl	ax,cl
	les	bx,dword ptr [bp-8]
	mov	dl,byte ptr es:[bx+5]
	mov	dh,0
	mov	cl,16
	shl	dx,cl
	add	ax,dx
	les	bx,dword ptr [bp-8]
	mov	dl,byte ptr es:[bx+6]
	mov	dh,0
	mov	cl,8
	shl	dx,cl
	add	ax,dx
	les	bx,dword ptr [bp-8]
	mov	dl,byte ptr es:[bx+7]
	mov	dh,0
	add	ax,dx
	cwd	
	mov	word ptr [si+7],dx
	mov	word ptr [si+5],ax
   ;	
   ;	    _fstrncpy(mit->cur_track_type,mfp->type,4);
   ;	
	mov	ax,4
	push	ax
	push	word ptr [bp-6]
	push	word ptr [bp-8]
	push	ds
	push	si
	call	far ptr __fstrncpy
	add	sp,10
   ;	
   ;	    mit->cur_track_type[4]='\0';
   ;	
	mov	byte ptr [si+4],0
   ;	
   ;	    offset = sizeof(MidiFilePrefix)+mit->cur_track_len;
   ;	
	mov	ax,word ptr [si+7]
	mov	dx,word ptr [si+5]
	add	dx,8
	adc	ax,0
	mov	word ptr [bp-2],ax
	mov	word ptr [bp-4],dx
   ;	
   ;	    mit->cur_pos+=offset;
   ;	
	mov	ax,word ptr [bp-2]
	mov	dx,word ptr [bp-4]
	add	word ptr [si+21],dx
	adc	word ptr [si+23],ax
   ;	
   ;	    mit->cur_track_ptr=&((char far *)mfp)[sizeof(MidiFilePrefix)];
   ;	
	mov	ax,word ptr [bp-6]
	mov	dx,word ptr [bp-8]
	add	dx,8
	mov	word ptr [si+11],ax
	mov	word ptr [si+9],dx
   ;	
   ;	    mit->nextptr=&((char far *)mfp)[offset];
   ;	
	mov	ax,word ptr [bp-6]
	mov	dx,word ptr [bp-8]
	add	dx,word ptr [bp-4]
	mov	word ptr [si+15],ax
	mov	word ptr [si+13],dx
   ;	
   ;	    return 1;
   ;	
	mov	ax,1
@16@142:
	jmp	short @16@198
@16@170:
   ;	
   ;	  }
   ;	  return 0;
   ;	
	xor	ax,ax
	jmp	short @16@142
@16@198:
   ;	
   ;	}
   ;	
	pop	si
	mov	sp,bp
	pop	bp
	ret	
_midi_next_iterator	endp
   ;	
   ;	void display_header(MidiHeader *header)
   ;	
	assume	cs:_TEXT
_display_header	proc	near
	push	bp
	mov	bp,sp
	push	si
	mov	si,word ptr [bp+4]
   ;	
   ;	{
   ;	  printf("======= Midi File Header ========\n");
   ;	
	mov	ax,offset DGROUP:s@+49
	push	ax
	call	near ptr _printf
	pop	cx
   ;	
   ;	  printf("Format:        %u\n", header->format);
   ;	
	push	word ptr [si]
	mov	ax,offset DGROUP:s@+84
	push	ax
	call	near ptr _printf
	pop	cx
	pop	cx
   ;	
   ;	  printf("Tracks:        %u\n", header->tracks);
   ;	
	push	word ptr [si+2]
	mov	ax,offset DGROUP:s@+103
	push	ax
	call	near ptr _printf
	pop	cx
	pop	cx
   ;	
   ;	  printf("TicksQuarter:  %u\n", header->ticks_quarter);
   ;	
	push	word ptr [si+4]
	mov	ax,offset DGROUP:s@+122
	push	ax
	call	near ptr _printf
	pop	cx
	pop	cx
   ;	
   ;	  printf("Frames/Second: %u\n", header->frames_second);
   ;	
	push	word ptr [si+6]
	mov	ax,offset DGROUP:s@+141
	push	ax
	call	near ptr _printf
	pop	cx
	pop	cx
   ;	
   ;	  printf("Ticks/Frame:   %u\n", header->ticks_frame);
   ;	
	push	word ptr [si+8]
	mov	ax,offset DGROUP:s@+160
	push	ax
	call	near ptr _printf
	pop	cx
	pop	cx
   ;	
   ;	}
   ;	
	pop	si
	pop	bp
	ret	
_display_header	endp
   ;	
   ;	void parse_header(void far *data,MidiHeader *header)
   ;	
	assume	cs:_TEXT
_parse_header	proc	near
	push	bp
	mov	bp,sp
	sub	sp,6
	push	si
	mov	si,word ptr [bp+8]
   ;	
   ;	{
   ;	  MidiFileHeader far *mfh=(MidiFileHeader far *)data;
   ;	
	mov	ax,word ptr [bp+6]
	mov	dx,word ptr [bp+4]
	mov	word ptr [bp-2],ax
	mov	word ptr [bp-4],dx
   ;	
   ;	
   ;	  header->format = (mfh->format[0] * 256) + mfh->format[1];
   ;	
	les	bx,dword ptr [bp-4]
	mov	al,byte ptr es:[bx]
	mov	ah,0
	mov	cl,8
	shl	ax,cl
	les	bx,dword ptr [bp-4]
	mov	dl,byte ptr es:[bx+1]
	mov	dh,0
	add	ax,dx
	mov	word ptr [si],ax
   ;	
   ;	  header->tracks = (mfh->tracks[0] * 256) + mfh->tracks[1];
   ;	
	les	bx,dword ptr [bp-4]
	mov	al,byte ptr es:[bx+2]
	mov	ah,0
	mov	cl,8
	shl	ax,cl
	les	bx,dword ptr [bp-4]
	mov	dl,byte ptr es:[bx+3]
	mov	dh,0
	add	ax,dx
	mov	word ptr [si+2],ax
   ;	
   ;	  printf("Division: %02X %02X\n", mfh->division[0], mfh->division[1]);
   ;	
	les	bx,dword ptr [bp-4]
	mov	al,byte ptr es:[bx+5]
	mov	ah,0
	push	ax
	les	bx,dword ptr [bp-4]
	mov	al,byte ptr es:[bx+4]
	mov	ah,0
	push	ax
	mov	ax,offset DGROUP:s@+179
	push	ax
	call	near ptr _printf
	add	sp,6
   ;	
   ;	  if( mfh->division[0] & 0x80 ) {
   ;	
	les	bx,dword ptr [bp-4]
	test	byte ptr es:[bx+4],128
	je	short @18@114
   ;	
   ;	    char x;
   ;	    header->ticks_frame = mfh->division[1];
   ;	
	les	bx,dword ptr [bp-4]
	mov	al,byte ptr es:[bx+5]
	mov	ah,0
	mov	word ptr [si+8],ax
   ;	
   ;	    x = (mfh->division[0] | 0x80);
   ;	
	les	bx,dword ptr [bp-4]
	mov	al,byte ptr es:[bx+4]
	or	al,128
	mov	byte ptr [bp-5],al
   ;	
   ;	    printf("frames_second neg %d\n", x);
   ;	
	mov	al,byte ptr [bp-5]
	cbw	
	push	ax
	mov	ax,offset DGROUP:s@+200
	push	ax
	call	near ptr _printf
	pop	cx
	pop	cx
   ;	
   ;	    header->frames_second = -x;
   ;	
	mov	al,byte ptr [bp-5]
	cbw	
	neg	ax
	mov	word ptr [si+6],ax
   ;	
   ;	    header->ticks_quarter = 0;
   ;	
	mov	word ptr [si+4],0
   ;	
   ;	  }
   ;	
	jmp	short @18@142
@18@114:
   ;	
   ;	  else {
   ;	    header->ticks_quarter = (mfh->division[0]&0x7F)*256+mfh->division[1];
   ;	
	les	bx,dword ptr [bp-4]
	mov	al,byte ptr es:[bx+4]
	mov	ah,0
	and	ax,127
	mov	cl,8
	shl	ax,cl
	les	bx,dword ptr [bp-4]
	mov	dl,byte ptr es:[bx+5]
	mov	dh,0
	add	ax,dx
	mov	word ptr [si+4],ax
   ;	
   ;	    header->ticks_frame   = 0;
   ;	
	mov	word ptr [si+8],0
   ;	
   ;	    header->frames_second = 0;
   ;	
	mov	word ptr [si+6],0
@18@142:
   ;	
   ;	  }
   ;	
   ;	}
   ;	
	pop	si
	mov	sp,bp
	pop	bp
	ret	
_parse_header	endp
   ;	
   ;	void far *load_midi(const char *file, unsigned long *sz)
   ;	
	assume	cs:_TEXT
_load_midi	proc	near
	push	bp
	mov	bp,sp
	sub	sp,42
	push	si
	push	di
	mov	si,word ptr [bp+4]
	mov	di,word ptr [bp+6]
   ;	
   ;	{
   ;	  void far *data;
   ;	  unsigned long size = 0L;
   ;	
	mov	word ptr [bp-6],0
	mov	word ptr [bp-8],0
   ;	
   ;	  int handle;
   ;	  struct stat statbuf;
   ;	  stat((char *)file,&statbuf);
   ;	
	lea	ax,word ptr [bp-42]
	push	ax
	push	si
	call	near ptr _stat
	pop	cx
	pop	cx
   ;	
   ;	  size=statbuf.st_size;
   ;	
	mov	ax,word ptr [bp-26]
	mov	dx,word ptr [bp-28]
	mov	word ptr [bp-6],ax
	mov	word ptr [bp-8],dx
   ;	
   ;	  if(_dos_open(file,O_RDONLY, &handle)!=0) {
   ;	
	lea	ax,word ptr [bp-10]
	push	ax
	mov	ax,1
	push	ax
	push	si
	call	near ptr __dos_open
	add	sp,6
	or	ax,ax
	je	short @19@114
   ;	
   ;	    fprintf(stderr, "Cannot open: %s\n", file);
   ;	
	push	si
	mov	ax,offset DGROUP:s@+222
	push	ax
	mov	ax,offset DGROUP:__streams+32
	push	ax
	call	near ptr _fprintf
	add	sp,6
   ;	
   ;	    return NULL;
   ;	
	xor	dx,dx
	xor	ax,ax
@19@86:
	jmp	@19@366
@19@114:
   ;	
   ;	  }
   ;	
   ;	  data=farmalloc(size);
   ;	
	push	word ptr [bp-6]
	push	word ptr [bp-8]
	call	near ptr _farmalloc
	pop	cx
	pop	cx
	mov	word ptr [bp-2],dx
	mov	word ptr [bp-4],ax
   ;	
   ;	  if(data) {
   ;	
	mov	ax,word ptr [bp-4]
	or	ax,word ptr [bp-2]
	je	short @19@226
   ;	
   ;	    unsigned x;
   ;	    if(_dos_read(handle,data,size,&x)!=0) {
   ;	
	lea	ax,word ptr [bp-12]
	push	ax
	push	word ptr [bp-8]
	push	word ptr [bp-2]
	push	word ptr [bp-4]
	push	word ptr [bp-10]
	call	near ptr __dos_read
	add	sp,10
	or	ax,ax
	je	short @19@198
   ;	
   ;	      farfree(data);
   ;	
	push	word ptr [bp-2]
	push	word ptr [bp-4]
	call	near ptr _farfree
	pop	cx
	pop	cx
   ;	
   ;	      data=NULL;
   ;	
	mov	word ptr [bp-2],0
	mov	word ptr [bp-4],0
   ;	
   ;	      fprintf(stderr, "Cannot load midi file\n");
   ;	
	mov	ax,offset DGROUP:s@+239
	push	ax
	mov	ax,offset DGROUP:__streams+32
	push	ax
	call	near ptr _fprintf
	pop	cx
	pop	cx
@19@198:
   ;	
   ;	    }
   ;	    
   ;	  } else {
   ;	
	jmp	short @19@254
@19@226:
   ;	
   ;	    fprintf(stderr, "Cannot alloc mem (%ld)\n", size);
   ;	
	push	word ptr [bp-6]
	push	word ptr [bp-8]
	mov	ax,offset DGROUP:s@+262
	push	ax
	mov	ax,offset DGROUP:__streams+32
	push	ax
	call	near ptr _fprintf
	add	sp,8
@19@254:
   ;	
   ;	  }
   ;	
   ;	  _dos_close(handle);
   ;	
	push	word ptr [bp-10]
	call	near ptr __dos_close
	pop	cx
   ;	
   ;	
   ;	  *sz= (data ? size : 0);
   ;	
	mov	ax,word ptr [bp-4]
	or	ax,word ptr [bp-2]
	je	short @19@310
	mov	dx,word ptr [bp-6]
	mov	ax,word ptr [bp-8]
	jmp	short @19@338
@19@310:
	xor	dx,dx
	xor	ax,ax
@19@338:
	mov	word ptr [di+2],dx
	mov	word ptr [di],ax
   ;	
   ;	
   ;	  return data;
   ;	
	mov	dx,word ptr [bp-2]
	mov	ax,word ptr [bp-4]
	jmp	@19@86
@19@366:
   ;	
   ;	}
   ;	
	pop	di
	pop	si
	mov	sp,bp
	pop	bp
	ret	
_load_midi	endp
   ;	
   ;	int main(int argc, char **argv)
   ;	
	assume	cs:_TEXT
_main	proc	near
	push	bp
	mov	bp,sp
	sub	sp,8
	push	si
	push	di
	mov	si,word ptr [bp+6]
   ;	
   ;	{
   ;	  void far *data=NULL;
   ;	
	mov	word ptr [bp-2],0
	mov	word ptr [bp-4],0
   ;	
   ;	  unsigned long size=0L;
   ;	
	mov	word ptr [bp-6],0
	mov	word ptr [bp-8],0
   ;	
   ;	  if(argc<2) {
   ;	
	cmp	word ptr [bp+4],2
	jge	short @20@142
   ;	
   ;	    fprintf(stderr, "Syntax: %s <midifile>\n", argv[0]);
   ;	
	push	word ptr [si]
	mov	ax,offset DGROUP:s@+286
	push	ax
	mov	ax,offset DGROUP:__streams+32
	push	ax
	call	near ptr _fprintf
	add	sp,6
@20@86:
   ;	
   ;	    return -2;
   ;	
	mov	ax,65534
@20@114:
	jmp	@20@702
@20@142:
   ;	
   ;	  }
   ;	  if(!mpu_uart()) {
   ;	
	call	near ptr _mpu_uart
	or	ax,ax
	jne	short @20@198
   ;	
   ;		  fprintf(stderr, "Cannot init midi mpu\n");
   ;	
	mov	ax,offset DGROUP:s@+309
	push	ax
	mov	ax,offset DGROUP:__streams+32
	push	ax
	call	near ptr _fprintf
	pop	cx
	pop	cx
	jmp	short @20@86
@20@198:
   ;	
   ;		  return -2;
   ;	  }
   ;	  player_status=0;
   ;	
	mov	word ptr DGROUP:_player_status,0
   ;	
   ;	  setuptimer();
   ;	
	call	near ptr _setuptimer
   ;	
   ;	  data=load_midi(argv[1], &size);
   ;	
	lea	ax,word ptr [bp-8]
	push	ax
	push	word ptr [si+2]
	call	near ptr _load_midi
	pop	cx
	pop	cx
	mov	word ptr [bp-2],dx
	mov	word ptr [bp-4],ax
   ;	
   ;	  if(data) {
   ;	
	mov	ax,word ptr [bp-4]
	or	ax,word ptr [bp-2]
	jne	@@5
	jmp	@20@674
@@5:
   ;	
   ;		midi_prepare(data,size);
   ;	
	push	word ptr [bp-6]
	push	word ptr [bp-8]
	push	word ptr [bp-2]
	push	word ptr [bp-4]
	call	near ptr _midi_prepare
	add	sp,8
   ;	
   ;	    if(total_tracks) {
   ;	
	cmp	word ptr DGROUP:_total_tracks,0
	je	short @20@646
   ;	
   ;			unsigned playing=1;
   ;	
	mov	di,1
   ;	
   ;			asm pushf;
   ;	
 	pushf	
   ;	
   ;			asm cli;
   ;	
 	cli	
   ;	
   ;			player_status=1;
   ;	
	mov	word ptr DGROUP:_player_status,1
   ;	
   ;			asm popf;
   ;	
 	popf	
   ;	
   ;			printf("Playing....\n");
   ;	
	mov	ax,offset DGROUP:s@+331
	push	ax
	call	near ptr _printf
	pop	cx
	jmp	short @20@562
@20@422:
   ;	
   ;			while(playing && !kbhit() ) {
   ;				xdelay();
   ;	
	call	near ptr _xdelay
   ;	
   ;				asm pushf;
   ;	
 	pushf	
   ;	
   ;				asm cli;
   ;	
 	cli	
   ;	
   ;				playing=player_status;
   ;	
	mov	di,word ptr DGROUP:_player_status
   ;	
   ;				asm popf;
   ;	
 	popf	
@20@562:
	or	di,di
	je	short @20@618
	call	near ptr _kbhit
	or	ax,ax
	je	short @20@422
@20@618:
   ;	
   ;			}
   ;			printf("Done.");
   ;	
	mov	ax,offset DGROUP:s@+344
	push	ax
	call	near ptr _printf
	pop	cx
@20@646:
   ;	
   ;	    }
   ;	    printf("Total Tracks %d\n", total_tracks);
   ;	
	push	word ptr DGROUP:_total_tracks
	mov	ax,offset DGROUP:s@+350
	push	ax
	call	near ptr _printf
	pop	cx
	pop	cx
   ;	
   ;	    farfree(data);
   ;	
	push	word ptr [bp-2]
	push	word ptr [bp-4]
	call	near ptr _farfree
	pop	cx
	pop	cx
@20@674:
   ;	
   ;	  }
   ;	  closetimer();
   ;	
	call	near ptr _closetimer
   ;	
   ;	  return 0;
   ;	
	xor	ax,ax
	jmp	@20@114
@20@702:
   ;	
   ;	}
   ;	
	pop	di
	pop	si
	mov	sp,bp
	pop	bp
	ret	
_main	endp
   ;	
   ;	void midi_set_player_status(unsigned st)
   ;	
	assume	cs:_TEXT
_midi_set_player_status	proc	near
	push	bp
	mov	bp,sp
   ;	
   ;	{
   ;		asm pushf;
   ;	
 	pushf	
   ;	
   ;		asm cli;
   ;	
 	cli	
   ;	
   ;		player_status=st;
   ;	
	mov	ax,word ptr [bp+4]
	mov	word ptr DGROUP:_player_status,ax
   ;	
   ;		asm popf;
   ;	
 	popf	
   ;	
   ;	}
   ;	
	pop	bp
	ret	
_midi_set_player_status	endp
   ;	
   ;	unsigned midi_get_player_status()
   ;	
	assume	cs:_TEXT
_midi_get_player_status	proc	near
	push	bp
	mov	bp,sp
	sub	sp,2
   ;	
   ;	{
   ;		unsigned st;
   ;		asm pushf;
   ;	
 	pushf	
   ;	
   ;		asm cli;
   ;	
 	cli	
   ;	
   ;		st=player_status;
   ;	
	mov	ax,word ptr DGROUP:_player_status
	mov	word ptr [bp-2],ax
   ;	
   ;		asm popf;
   ;	
 	popf	
   ;	
   ;		return st;
   ;	
	mov	ax,word ptr [bp-2]
	jmp	short @22@170
@22@170:
   ;	
   ;	}
   ;	
	mov	sp,bp
	pop	bp
	ret	
_midi_get_player_status	endp
   ;	
   ;	void midi_prepare(void far *data, unsigned long size)
   ;	
	assume	cs:_TEXT
_midi_prepare	proc	near
	push	bp
	mov	bp,sp
	sub	sp,36
	push	si
   ;	
   ;	{
   ;	    MidiHeader mh;
   ;	    MidiFileIterator it;
   ;		unsigned ok=1;
   ;	
	mov	si,1
   ;	
   ;		
   ;		mp.tempo=500000L;
   ;	
	mov	word ptr DGROUP:_mp+2,7
	mov	word ptr DGROUP:_mp,41248
   ;	
   ;		mp.ticks_quarter=120;
   ;	
	mov	word ptr DGROUP:_mp+4,120
   ;	
   ;		mp.st=0;
   ;	
	mov	byte ptr DGROUP:_mp+22,0
   ;	
   ;		mp.ltick=0;
   ;	
	mov	word ptr DGROUP:_mp+16,0
	mov	word ptr DGROUP:_mp+14,0
   ;	
   ;		mp.etick=0;
   ;	
	mov	word ptr DGROUP:_mp+12,0
	mov	word ptr DGROUP:_mp+10,0
   ;	
   ;		mp.stick=0;
   ;	
	mov	word ptr DGROUP:_mp+8,0
	mov	word ptr DGROUP:_mp+6,0
   ;	
   ;		mp.d=900;
   ;	
	mov	word ptr DGROUP:_mp+20,0
	mov	word ptr DGROUP:_mp+18,900
   ;	
   ;	
   ;	    //printf("Loaded %ld bytes on %p\n", size, data);
   ;	    midi_start_iterator(&it,data,size);
   ;	
	push	word ptr [bp+10]
	push	word ptr [bp+8]
	push	word ptr [bp+6]
	push	word ptr [bp+4]
	lea	ax,word ptr [bp-36]
	push	ax
	call	near ptr _midi_start_iterator
	add	sp,10
	jmp	short @23@198
@23@58:
   ;	
   ;	    while( midi_next_iterator(&it) ) {
   ;	      if(!strcmp(it.cur_track_type,"MThd")) {
   ;	
	mov	ax,offset DGROUP:s@+367
	push	ax
	lea	ax,word ptr [bp-36]
	push	ax
	call	near ptr _strcmp
	pop	cx
	pop	cx
	or	ax,ax
	jne	short @23@114
   ;	
   ;	        parse_header(it.cur_track_ptr,&mh);
   ;	
	lea	ax,word ptr [bp-10]
	push	ax
	push	word ptr [bp-25]
	push	word ptr [bp-27]
	call	near ptr _parse_header
	add	sp,6
   ;	
   ;			mp.ticks_quarter=mh.ticks_quarter;
   ;	
	mov	ax,word ptr [bp-6]
	mov	word ptr DGROUP:_mp+4,ax
   ;	
   ;	        display_header(&mh);
   ;	
	lea	ax,word ptr [bp-10]
	push	ax
	call	near ptr _display_header
	pop	cx
   ;	
   ;	      }
   ;	
	jmp	short @23@198
@23@114:
   ;	
   ;	      else if(!strcmp(it.cur_track_type,"MTrk")) {
   ;	
	mov	ax,offset DGROUP:s@+372
	push	ax
	lea	ax,word ptr [bp-36]
	push	ax
	call	near ptr _strcmp
	pop	cx
	pop	cx
	or	ax,ax
	jne	short @23@170
   ;	
   ;	        midi_add_track(&mp,tracks, &total_tracks, it.cur_track_ptr, it.cur_track_len);
   ;	
	push	word ptr [bp-31]
	push	word ptr [bp-25]
	push	word ptr [bp-27]
	mov	ax,offset DGROUP:_total_tracks
	push	ax
	mov	ax,offset DGROUP:_tracks
	push	ax
	mov	ax,offset DGROUP:_mp
	push	ax
	call	near ptr _midi_add_track
	add	sp,12
   ;	
   ;	      }
   ;	
	jmp	short @23@198
@23@170:
   ;	
   ;	      else {
   ;			ok=0;
   ;	
	xor	si,si
   ;	
   ;	        break;
   ;	
	jmp	short @23@226
@23@198:
	lea	ax,word ptr [bp-36]
	push	ax
	call	near ptr _midi_next_iterator
	pop	cx
	or	ax,ax
	jne	short @23@58
@23@226:
   ;	
   ;	      }
   ;	    }
   ;		
   ;		if(ok && total_tracks) {
   ;	
	or	si,si
	je	short @23@310
	cmp	word ptr DGROUP:_total_tracks,0
	je	short @23@310
   ;	
   ;			mp.ntracks=total_tracks;
   ;	
	mov	ax,word ptr DGROUP:_total_tracks
	mov	word ptr DGROUP:_mp+23,ax
   ;	
   ;			mp.tracks=tracks;
   ;	
	mov	word ptr DGROUP:_mp+25,offset DGROUP:_tracks
   ;	
   ;			midi_set_player_status(2);
   ;	
	mov	ax,2
	push	ax
	call	near ptr _midi_set_player_status
	pop	cx
@23@310:
   ;	
   ;		}
   ;	}
   ;	
	pop	si
	mov	sp,bp
	pop	bp
	ret	
_midi_prepare	endp
   ;	
   ;	void midi_processor(unsigned long msecs)
   ;	
	assume	cs:_TEXT
_midi_processor	proc	near
	push	bp
	mov	bp,sp
	sub	sp,4
   ;	
   ;	{
   ;		unsigned playing;
   ;		if(player_status==1) {
   ;	
	cmp	word ptr DGROUP:_player_status,1
	jne	short @24@114
   ;	
   ;			unsigned long ticks=get_ticks(&mp);
   ;	
	mov	ax,offset DGROUP:_mp
	push	ax
	call	near ptr _get_ticks
	pop	cx
	mov	word ptr [bp-2],dx
	mov	word ptr [bp-4],ax
   ;	
   ;			if(!midi_playtracks(&mp, ticks)) {
   ;	
	push	word ptr [bp-2]
	push	word ptr [bp-4]
	mov	ax,offset DGROUP:_mp
	push	ax
	call	near ptr _midi_playtracks
	add	sp,6
	or	ax,ax
	jne	short @24@114
   ;	
   ;				player_status=0;
   ;	
	mov	word ptr DGROUP:_player_status,0
@24@114:
   ;	
   ;			}
   ;		}
   ;		(void)msecs;
   ;	
   ;	
   ;	}
   ;	
	mov	sp,bp
	pop	bp
	ret	
_midi_processor	endp
   ;	
   ;	unsigned long mlongmul(unsigned long a, unsigned long b)
   ;	
	assume	cs:_TEXT
_mlongmul	proc	near
	push	bp
	mov	bp,sp
	sub	sp,4
   ;	
   ;	{
   ;	  unsigned long ac=0;
   ;	
	mov	word ptr [bp-2],0
	mov	word ptr [bp-4],0
	jmp	short @25@142
@25@58:
   ;	
   ;	  while(b) {
   ;	    if(b&1)
   ;	
	mov	ax,word ptr [bp+10]
	mov	dx,word ptr [bp+8]
	and	dx,1
	and	ax,0
	or	dx,ax
	je	short @25@114
   ;	
   ;	      ac+=a;
   ;	
	mov	ax,word ptr [bp+6]
	mov	dx,word ptr [bp+4]
	add	word ptr [bp-4],dx
	adc	word ptr [bp-2],ax
@25@114:
   ;	
   ;	    a=a<<1;
   ;	
	mov	ax,word ptr [bp+6]
	mov	dx,word ptr [bp+4]
	shl	dx,1
	rcl	ax,1
	mov	word ptr [bp+6],ax
	mov	word ptr [bp+4],dx
   ;	
   ;	    b=b>>1;
   ;	
	mov	ax,word ptr [bp+10]
	mov	dx,word ptr [bp+8]
	shr	ax,1
	rcr	dx,1
	mov	word ptr [bp+10],ax
	mov	word ptr [bp+8],dx
@25@142:
	mov	ax,word ptr [bp+8]
	or	ax,word ptr [bp+10]
	jne	short @25@58
   ;	
   ;	  }
   ;	  return ac;
   ;	
	mov	dx,word ptr [bp-2]
	mov	ax,word ptr [bp-4]
	jmp	short @25@198
@25@198:
   ;	
   ;	}
   ;	
	mov	sp,bp
	pop	bp
	ret	
_mlongmul	endp
   ;	
   ;	unsigned long mlongdiv(unsigned long N, unsigned long D, unsigned long *r)
   ;	
	assume	cs:_TEXT
_mlongdiv	proc	near
	push	bp
	mov	bp,sp
	sub	sp,8
	push	si
	push	di
	mov	di,word ptr [bp+12]
   ;	
   ;	{
   ;	  #define NBITS (sizeof(unsigned long)*8)
   ;	  #define XXX (0x1L<<(NBITS-1))
   ;	
   ;		unsigned long Q = 0L;
   ;	
	mov	word ptr [bp-2],0
	mov	word ptr [bp-4],0
   ;	
   ;		unsigned long R = 0L;
   ;	
	mov	word ptr [bp-6],0
	mov	word ptr [bp-8],0
   ;	
   ;		register unsigned i;
   ;		if(D==0) {
   ;	
	mov	ax,word ptr [bp+8]
	or	ax,word ptr [bp+10]
	jne	short @26@86
   ;	
   ;			fprintf(stderr,"Cannot divide by zero\r\n");
   ;	
	mov	ax,offset DGROUP:s@+377
	push	ax
	mov	ax,offset DGROUP:__streams+32
	push	ax
	call	near ptr _fprintf
	pop	cx
	pop	cx
   ;	
   ;			exit(0);
   ;	
	xor	ax,ax
	push	ax
	call	near ptr _exit
	pop	cx
@26@86:
   ;	
   ;		}
   ;	
   ;	  i=NBITS;
   ;	
	mov	si,32
@26@114:
   ;	
   ;		do
   ;		{
   ;			R=R<<1;
   ;	
	mov	ax,word ptr [bp-6]
	mov	dx,word ptr [bp-8]
	shl	dx,1
	rcl	ax,1
	mov	word ptr [bp-6],ax
	mov	word ptr [bp-8],dx
   ;	
   ;			R |= ( N & XXX ) ? 1 : 0;
   ;	
	mov	ax,word ptr [bp+6]
	mov	dx,word ptr [bp+4]
	and	dx,0
	and	ax,32768
	or	dx,ax
	je	short @26@170
	mov	ax,1
	jmp	short @26@198
@26@170:
	xor	ax,ax
@26@198:
	cwd	
	or	word ptr [bp-8],ax
	or	word ptr [bp-6],dx
   ;	
   ;			N=N<<1;
   ;	
	mov	ax,word ptr [bp+6]
	mov	dx,word ptr [bp+4]
	shl	dx,1
	rcl	ax,1
	mov	word ptr [bp+6],ax
	mov	word ptr [bp+4],dx
   ;	
   ;			Q=Q<<1;
   ;	
	mov	ax,word ptr [bp-2]
	mov	dx,word ptr [bp-4]
	shl	dx,1
	rcl	ax,1
	mov	word ptr [bp-2],ax
	mov	word ptr [bp-4],dx
   ;	
   ;			if( R>=D ) {
   ;	
	mov	ax,word ptr [bp-6]
	mov	dx,word ptr [bp-8]
	cmp	ax,word ptr [bp+10]
	jb	short @26@310
	jne	short @26@282
	cmp	dx,word ptr [bp+8]
	jb	short @26@310
@26@282:
   ;	
   ;				R = R - D;
   ;	
	mov	ax,word ptr [bp-6]
	mov	dx,word ptr [bp-8]
	sub	dx,word ptr [bp+8]
	sbb	ax,word ptr [bp+10]
	mov	word ptr [bp-6],ax
	mov	word ptr [bp-8],dx
   ;	
   ;				Q |= 1;
   ;	
	or	word ptr [bp-4],1
	or	word ptr [bp-2],0
@26@310:
   ;	
   ;			}
   ;		}while(--i);
   ;	
	dec	si
	mov	ax,si
	or	ax,ax
	je	@@6
	jmp	@26@114
@@6:
   ;	
   ;	  
   ;	  if(r)
   ;	
	or	di,di
	je	short @26@394
   ;	
   ;	    *r=R;
   ;	
	mov	ax,word ptr [bp-6]
	mov	dx,word ptr [bp-8]
	mov	word ptr [di+2],ax
	mov	word ptr [di],dx
@26@394:
   ;	
   ;		return Q;
   ;	
	mov	dx,word ptr [bp-2]
	mov	ax,word ptr [bp-4]
	jmp	short @26@422
@26@422:
   ;	
   ;	
   ;	}
   ;	
	pop	di
	pop	si
	mov	sp,bp
	pop	bp
	ret	
_mlongdiv	endp
_TEXT	ends
_BSS	segment word public 'BSS'
_mp	label	word
	db	29 dup (?)
_total_tracks	label	word
	db	2 dup (?)
_tracks	label	word
	db	960 dup (?)
_oldirq0handler	label	dword
	db	4 dup (?)
	?debug	C E9
_BSS	ends
_DATA	segment word public 'DATA'
s@	label	byte
	db	'T> %lu %u %lu'
	db	10
	db	0
	db	'=> Nxt %lu s: %lu p: %lu Cmd %0X'
	db	10
	db	0
	db	'======= Midi File Header ========'
	db	10
	db	0
	db	'Format:        %u'
	db	10
	db	0
	db	'Tracks:        %u'
	db	10
	db	0
	db	'TicksQuarter:  %u'
	db	10
	db	0
	db	'Frames/Second: %u'
	db	10
	db	0
	db	'Ticks/Frame:   %u'
	db	10
	db	0
	db	'Division: %02X %02X'
	db	10
	db	0
	db	'frames_second neg %d'
	db	10
	db	0
	db	'Cannot open: %s'
	db	10
	db	0
	db	'Cannot load midi file'
	db	10
	db	0
	db	'Cannot alloc mem (%ld)'
	db	10
	db	0
	db	'Syntax: %s <midifile>'
	db	10
	db	0
	db	'Cannot init midi mpu'
	db	10
	db	0
	db	'Playing....'
	db	10
	db	0
	db	'Done.'
	db	0
	db	'Total Tracks %d'
	db	10
	db	0
	db	'MThd'
	db	0
	db	'MTrk'
	db	0
	db	'Cannot divide by zero'
	db	13
	db	10
	db	0
_DATA	ends
_TEXT	segment byte public 'CODE'
_TEXT	ends
	public	_midi_set_player_status
	extrn	_kbhit:near
	extrn	__setargv__:far
	public	_main
	public	_midi_get_player_status
	public	_mp
	public	_load_midi
	public	_parse_header
	public	_display_header
	public	_midi_next_iterator
	public	_midi_start_iterator
	public	_midi_add_track
	public	_midi_set_tempo
	public	_midi_playtracks
	public	_midi_track_parse_next
	public	_midi_playtrack
	public	_get_ticks
	public	_get_micros
	public	_send_midi
	public	_xdelay
	public	_total_tracks
	public	_tracks
	public	_mpu_uart
	public	_mpu_send
	public	_closetimer
	public	_setuptimer
	public	_irq0handler
	public	_isr
	public	_midi_processor
	public	_midi_prepare
	public	_oldirq0handler
	public	_player_status
	public	_counter
	public	_mlongdiv
	public	_mlongmul
	extrn	_stat:near
	extrn	__dos_read:near
	extrn	__dos_open:near
	extrn	__dos_close:near
	extrn	_farmalloc:near
	extrn	_farfree:near
	extrn	__fstrncpy:far
	extrn	_strcmp:near
	extrn	_exit:near
	extrn	_printf:near
	extrn	_fprintf:near
	extrn	__streams:word
_s@	equ	s@
	end
