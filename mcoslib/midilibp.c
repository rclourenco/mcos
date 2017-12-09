#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>


typedef struct {
  unsigned char cur_track_type[5];
  unsigned long cur_track_len;
  void *cur_track_ptr;
  void *nextptr;
  unsigned long total_len;
  unsigned long cur_pos;
} MidiFileIterator;

typedef struct {
  unsigned char type[4];
  unsigned char length[4];
} MidiFilePrefix;

typedef struct {
  unsigned char format[2];
  unsigned char tracks[2];
  unsigned char division[2];
} MidiFileHeader;

typedef struct {
  unsigned int format;
  unsigned int tracks;
  unsigned int ticks_quarter;
  unsigned int frames_second;
  unsigned int ticks_frame;
} MidiHeader;

typedef struct {
  unsigned char *start;
  unsigned long length;
  unsigned long nexttime;
  unsigned long pos;
  unsigned long mlen;
  unsigned long rlen;
} MidiTrackPlayer;

typedef struct {
	unsigned long tempo;
	unsigned int  ticks_quarter;
	unsigned long stick;
	unsigned long etick;
	unsigned long ltick;
	unsigned long d;
	char st;
	unsigned ntracks;
	MidiTrackPlayer *tracks;
	unsigned status;
} MidiPlayer;


volatile unsigned long counter=0;
volatile unsigned player_status=0;

void interrupt (*oldirq0handler)();

void midi_prepare(void *data, unsigned long size);
void midi_processor(unsigned long counter);

void interrupt irq0handler()
{
	midi_processor(counter);
	counter++;
	if( (counter%0x40)==0) {
		oldirq0handler();
	}
	else {
		asm mov al, 0x20;
		asm out 0x20, al; 
	}
}


void setuptimer()
{
	asm pushf;
	asm cli; 
	asm mov al,0x0;
	asm out 0x40, al;
	asm mov al, 0x4;
	asm out 0x40, al;

	oldirq0handler=getvect(0x8);
	setvect(0x8,irq0handler);
	asm popf;
}

void closetimer()
{
	asm pushf;
	asm cli;
	setvect(0x8,oldirq0handler);
	asm popf;
}

#define MPU 0x330
void mpu_send(unsigned char data[], unsigned len)
{
	asm {
		mov cx, len;
		mov si, data;
		mov dx, MPU;
		inc dx;
	}
	busy:
	asm {
		in al, dx;
		test al,0x40;
		jnz busy;
		dec dx;
		
		cld;
	}
	writeloop:
	asm {
		lodsb;
		out dx,al;
		loop writeloop;
	}
}

unsigned mpu_uart()
{
	unsigned r=1;
	asm {
		mov dx, MPU;
		inc dx;
	}
	busy:
	asm {
		in al, dx;
		test al,0x40;
		jnz busy;

		mov al,0x3f;
		out dx,al;

		xor cx,cx;
	}
	Empty:
	asm {
		in al,dx;
		test al,0x80;
		jnz NextLoop;

		dec dx;
		in al,dx;
		cmp al,0xFE;
		je inUartMode;
		inc dx;
	}
	NextLoop:
	asm {
		loop Empty;
		mov r,0;
	}
	inUartMode:
	return r;

}


#define MAXTRACKS 40

MidiTrackPlayer tracks[MAXTRACKS];
unsigned total_tracks;

unsigned read_counter()
{
	asm {
		pushf;
		cli
		mov dx, 0x43;
		mov al, 0x0;
		out dx, al;
		
 		mov dx, 0x40;
		in al, dx;
		mov ah, al;
		in al, dx;
		xchg ah, al;
		popf;
	}
}

unsigned xdelay()
{
	asm {
		mov cx, 0x0FFF;
		rep nop;
	}
}


void send_midi(unsigned char *p, unsigned long b)
{
	if(b==0)
		return;
	if( p[0]!=0xFF ) {
		mpu_send(p,b);
	}
}

unsigned long get_micros()
{
	unsigned ticks;
	unsigned long usecs=0L;
	asm pushf;
	asm cli;
	usecs=counter;
	asm popf;
	return usecs*800;
}

unsigned long get_ticks(MidiPlayer *mp)
{
	unsigned long tick;
	unsigned long c1=get_micros();
	if(mp->st==0) {
		mp->stick=c1;
		mp->st=1;
	}
	
	tick = (c1-mp->stick)/mp->d;
	return tick; //-mp->stick+mp->etick;
}


int midi_playtrack(MidiPlayer *mp, unsigned n, unsigned long ticks)
{
	MidiTrackPlayer *track=&mp->tracks[n];

  if( track->pos >= track->length )
    return 0;
	do {
		if(ticks<track->nexttime) {
			return 1;
		}
		send_midi(&track->start[track->pos], track->mlen);
		//printf("E %u> %lu %lu\n", n, ticks, track->nexttime);
	} while( midi_track_parse_next(track, mp) );
	return 0;
}

int midi_playtracks(MidiPlayer *mp, unsigned long ticks)
{
	unsigned i;
	unsigned playing=0;
	for(i=0;i<mp->ntracks;i++)
	{
//		printf("Track %u ", i);
		playing |= midi_playtrack(mp,i, ticks);
	}
	return playing;
}


void midi_set_tempo(MidiPlayer *mp, unsigned long tempo)
{
	mp->tempo=tempo;
	
	mp->d = mp->tempo/mp->ticks_quarter;
	printf("T> %lu %u %lu\n", tempo, mp->ticks_quarter, mp->d);
}

int midi_track_parse_next(MidiTrackPlayer *mtp, MidiPlayer *mp)
{
  unsigned long delta = 0L, slen = 0L, p;
  unsigned char v,x;

  mtp->pos += mtp->mlen;

  if( mtp->pos >= mtp->length )
    return 0;

  do {
    v=mtp->start[mtp->pos];
    delta = delta * 128 + (v&0x7F);
    mtp->pos++;
    if( mtp->pos >= mtp->length ) {
      return 0;
    }
  }while( v&0x80 );
  mtp->nexttime += delta;

  v = mtp->start[mtp->pos];
  switch(v&0xF0)
  {
    case 0x80:
    case 0x90:
    case 0xA0:
    case 0xB0:
    case 0xE0: mtp->mlen=3; mtp->rlen=2; break;
    case 0xC0:
    case 0xD0: mtp->mlen=2; mtp->rlen=1; break;
    case 0xF0:
      if(v==0xF0 || v==0xF7 || v==0xFF) {
        p = (v==0xFF) ? 2 : 1;

        do {
          x=mtp->start[mtp->pos+p];
          slen = slen * 128 + (x&0x7F);
          p++;
        }while(x&0x80);
        if(v==0xFF && mtp->start[mtp->pos+1]==0x51 && slen==3) {
			midi_set_tempo(mp,
				 mtp->start[mtp->pos+3]*0x10000
				+mtp->start[mtp->pos+4]*0x100
				+mtp->start[mtp->pos+5]);
		}
        mtp->mlen = slen+p;
      }
      else {
        mtp->mlen=0;
      }
    break;
    default: mtp->mlen=mtp->rlen;
  }
  return 1;
}

void midi_add_track(MidiPlayer *mp, MidiTrackPlayer *mtp, unsigned *tt, void *ptr, unsigned len)
{
  if(*tt<MAXTRACKS)
  {
    mtp[*tt].start  = (unsigned char *)ptr;
    mtp[*tt].length = len;
    mtp[*tt].nexttime  = 0L;
    mtp[*tt].pos       = 0L;
    mtp[*tt].mlen      = 0L;
    mtp[*tt].rlen      = 0L;
    midi_track_parse_next(&mtp[*tt], mp);
    printf("=> Nxt %lu s: %lu p: %lu Cmd %0X\n", 
      mtp[*tt].nexttime,
      mtp[*tt].mlen,
      mtp[*tt].pos, 
      mtp[*tt].start[ mtp[*tt].pos ]
    );
    (*tt)++;
  }
}


void midi_start_iterator(MidiFileIterator *mit, void *data, unsigned long size)
{
  mit->nextptr=data;
  mit->total_len=size;
  mit->cur_track_len=0;
  mit->cur_track_type[0]=0;
  mit->cur_pos=0;
  mit->cur_track_ptr=NULL;
}

int midi_next_iterator(MidiFileIterator *mit)
{
  if(mit->cur_pos<mit->total_len)
  {
    unsigned long offset;
    MidiFilePrefix *mfp = (MidiFilePrefix *)mit->nextptr;
    mit->cur_track_len = (mfp->length[0]<<24) + (mfp->length[1]<<16) + (mfp->length[2]<<8) + mfp->length[3];
    strncpy(mit->cur_track_type,mfp->type,4);
    mit->cur_track_type[4]='\0';
    offset = sizeof(MidiFilePrefix)+mit->cur_track_len;
    mit->cur_pos+=offset;
    mit->cur_track_ptr=&((char *)mfp)[sizeof(MidiFilePrefix)];
    mit->nextptr=&((char *)mfp)[offset];
    return 1;
  }
  return 0;
}

void display_header(MidiHeader *header)
{
  printf("======= Midi File Header ========\n");
  printf("Format:        %u\n", header->format);
  printf("Tracks:        %u\n", header->tracks);
  printf("TicksQuarter:  %u\n", header->ticks_quarter);
  printf("Frames/Second: %u\n", header->frames_second);
  printf("Ticks/Frame:   %u\n", header->ticks_frame);
}

void parse_header(void *data,MidiHeader *header)
{
  MidiFileHeader *mfh=(MidiFileHeader *)data;

  header->format = (mfh->format[0] * 256) + mfh->format[1];
  header->tracks = (mfh->tracks[0] * 256) + mfh->tracks[1];
  printf("Division: %02X %02X\n", mfh->division[0], mfh->division[1]);
  if( mfh->division[0] & 0x80 ) {
    char x;
    header->ticks_frame = mfh->division[1];
    x = (mfh->division[0] | 0x80);
    printf("frames_second neg %d\n", x);
    header->frames_second = -x;
    header->ticks_quarter = 0;
  }
  else {
    header->ticks_quarter = (mfh->division[0]&0x7F)*256+mfh->division[1];
    header->ticks_frame   = 0;
    header->frames_second = 0;
  }

}

void *load_midi(const char *file, unsigned long *sz)
{
  void *data;
  unsigned long size = 0L;
  FILE *fp = fopen(file,"rb");
  if(!fp) {
    fprintf(stderr, "Cannot open: %s\n", file);
    return NULL;
  }
  fseek(fp,0L,SEEK_END);
  size = ftell(fp);
  rewind(fp);
  data=malloc(size);
  if(data) {
    if(fread(data,size,1,fp)!=1) {
      free(data);
      data=NULL;
      fprintf(stderr, "Cannot load midi file\n");
    }
    
  } else {
    fprintf(stderr, "Cannot alloc mem (%ld)\n", size);
  }

  fclose(fp);

  *sz= (data ? size : 0);

  return data;
}

MidiPlayer mp;

unsigned midi_get_player_status();

int main(int argc, char **argv)
{
  void *data=NULL;
  unsigned long size=0L;
  if(argc<2) {
    fprintf(stderr, "Syntax: %s <midifile>\n", argv[0]);
    return -2;
  }
  if(!mpu_uart()) {
	  fprintf(stderr, "Cannot init midi mpu\n");
	  return -2;
  }
  player_status=0;
  setuptimer();
  data=load_midi(argv[1], &size);
  if(data) {
	midi_prepare(data,size);
    if(total_tracks) {
		unsigned playing=1;
		asm pushf;
		asm cli;
		player_status=1;
		asm popf;
		printf("Playing....\n");
		while(playing && !kbhit() ) {
			xdelay();
			asm pushf;
			asm cli;
			playing=player_status;
			asm popf;
		}
		printf("Done.");
    }
    printf("Total Tracks %d\n", total_tracks);
    free(data);
  }
  closetimer();
  return 0;
}

void midi_set_player_status(unsigned st)
{
	asm pushf;
	asm cli;
	player_status=st;
	asm popf;
}

unsigned midi_get_player_status()
{
	unsigned st;
	asm pushf;
	asm cli;
	st=player_status;
	asm popf;
}


void midi_prepare(void *data, unsigned long size)
{
    MidiHeader mh;
    MidiFileIterator it;
	unsigned ok=1;
	
	mp.tempo=500000L;
	mp.ticks_quarter=120;
	mp.st=0;
	mp.ltick=0;
	mp.etick=0;
	mp.stick=0;
	mp.d=900;

    //printf("Loaded %ld bytes on %p\n", size, data);
    midi_start_iterator(&it,data,size);
    while( midi_next_iterator(&it) ) {
      if(!strcmp(it.cur_track_type,"MThd")) {
        parse_header(it.cur_track_ptr,&mh);
		mp.ticks_quarter=mh.ticks_quarter;
        display_header(&mh);
      }
      else if(!strcmp(it.cur_track_type,"MTrk")) {
        midi_add_track(&mp,tracks, &total_tracks, it.cur_track_ptr, it.cur_track_len);
      }
      else {
		ok=0;
        break;
      }
    }
	
	if(ok && total_tracks) {
		mp.ntracks=total_tracks;
		mp.tracks=tracks;
		midi_set_player_status(2);
	}
}

void midi_processor(unsigned long msecs)
{
	unsigned playing;
	if(player_status==1) {
		unsigned long ticks=get_ticks(&mp);
		if(!midi_playtracks(&mp, ticks)) {
			player_status=0;
		}
	}
}