//8 has graphics


$PI,"<1>",1$


$PI,"<2>",2$

F64 t0=tM;

U0 DrawIt(TaskStruct *task,GrDC *dc)
{
  nounusedwarn task;
  F64 dt=(tM-t0)*music_tempo,tt;
  U8 *inter;
  if (dt<16.0)
    tt=0;
  else if (dt<18.0)
    tt=((dt-16)/2)%1;
  else
    tt=1.0;
  inter=GrSpriteInterpolate($IB,"<1>",1$,$IB,"<2>",2$,tt);
  GrSpritePlot3(dc,180,40,0,inter);
  Free(inter);
}

U0 Song()
{
  SettingsPush; //See $LK,"SettingsPush","MN:SettingsPush"$
  Fs->draw_it=&DrawIt;
  Fs->task_end_cb=&SndTaskEndCB;

  music_tempo=3.0;
  try {
    while (!ScanKey) {
      t0=tM;
      LtfClear;
      Play("3qFeRRqAeRAqGDetBACeCB",
	  "God\0\0\0is\0\0a\0good\0God.\0(Thank\0\0\0God!!)\n\0\0");
      Play("qFeRRqAeRAqGDetBACeCB",
	  "God\0\0\0is\0\0a\0good\0God.\0(Thank\0\0\0God!!)\n\0\0");
      Play("qGsCECEeC2GqR3FeEDqEF",
	  "\0\0\0\0\0\0\0\0He\0is\0a\0good\0God.\n\0");
      Play("GsCECEeC2GqR3FeEDqEF",
	  "\0\0\0\0\0\0\0\0He\0is\0a\0good\0God.\n\0");
    }
  } catch
    CatchAll;
  SettingsPop;
}

Song;
           3            D      K      Q      Y      a                 3            E   ����K      T      \      b   ���� 