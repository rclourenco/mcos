#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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


#define MAXTRACKS 40

MidiTrackPlayer tracks[MAXTRACKS];
unsigned total_tracks;

int midi_track_parse_next(MidiTrackPlayer *mtp)
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

void midi_add_track(MidiTrackPlayer *mtp, unsigned *tt, void *ptr, unsigned len)
{
  if(*tt<MAXTRACKS)
  {
    mtp[*tt].start  = (unsigned char *)ptr;
    mtp[*tt].length = len;
    mtp[*tt].nexttime  = 0L;
    mtp[*tt].pos       = 0L;
    mtp[*tt].mlen      = 0L;
    mtp[*tt].rlen      = 0L;
    midi_track_parse_next(&mtp[*tt]);
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
    header->ticks_frame = mfh->division[1];
    char x = (mfh->division[0] | 0x80);
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

int main(int argc, char **argv)
{
  void *data=NULL;
  unsigned long size=0L;
  if(argc<2) {
    fprintf(stderr, "Syntax: %s <midifile>\n", argv[0]);
    return -2;
  }
  data=load_midi(argv[1], &size);
  if(data) {
    MidiHeader mh;
    MidiFileIterator it;

    printf("Loaded %ld bytes on %p\n", size, data);
    midi_start_iterator(&it,data,size);
    while( midi_next_iterator(&it) ) {
      printf( "Type %s Len %lu Starts @ %p\n", it.cur_track_type, it.cur_track_len, it.cur_track_ptr);
      if(!strcmp(it.cur_track_type,"MThd")) {
        parse_header(it.cur_track_ptr,&mh);
        display_header(&mh);
      }
      else if(!strcmp(it.cur_track_type,"MTrk")) {
        midi_add_track(tracks, &total_tracks, it.cur_track_ptr, it.cur_track_len);
      }
      else {
        printf("Invalid midi file\n");
        break;
      }
    }
    if(total_tracks) {
      printf("================================\n");
      unsigned t = 1;
      while( midi_track_parse_next(&tracks[t]) ) {
        printf("=> Nxt %lu s: %lu p: %lu Cmd %0X\n", 
          tracks[t].nexttime,
          tracks[t].mlen,
          tracks[t].pos, 
          tracks[t].start[ tracks[t].pos ]
        );
      }
    }
    printf("Total Tracks %d\n", total_tracks);
    free(data);
  }
}

