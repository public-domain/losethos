#help_index "Snd"

class SndWaveCtrl
{
  I64 sample_rate,sample_bits,channels;
  F64 phase,last_y,last_dydt,next_y;
};

U64 snd_obuf_num,snd_ibuf_num;

#define Sf_FILLING_OUT		0
U64 snd_flags;

public U0 SndTaskEndCB()
//This will turn-off snd when a task gets killed.
{
  Snd(0);
  Exit;
}


class SndData
{
  SndData *next,*last;
  U8 waveform,pad[7];
  F64 amp; //0.0-1.0
  F64 freq; //Hz
  F64 time_stamp;
} snd_record_root;

U0 SndRecord(F64 freq,U64 waveform,F64 amp)
{
  SndData *d;
  d=ACAlloc(sizeof(SndData));
  d->freq=freq;
  d->time_stamp=tNP;
  d->waveform=waveform;
  d->amp=amp;
  InsQue(d,snd_record_root.last);
}

U0 CtrlAltV(U64 sc)
{
  if (sc&SCF_SHIFT)
    snd_vol/=1.5;
  else
    snd_vol*=1.5;
}
SetCtrlAltLetCB('V',&CtrlAltV,"Sys/Snd Volume +/-");

U0 SndInit()
{
  snd_record_root.next=snd_record_root.last=&snd_record_root;
  snd_record_root.freq=0;
  snd_record_root.time_stamp=GetTimeStamp;
  fp_snd_record=&SndRecord;
}

SndInit;

#include "MathAudio"
#include "Music"
#include "SndEffects"
#help_index ""
