//9 has graphics

F64 t0;
#define HORIZON_Y	50
#define LEFT_EDGE	200

U0 DrawIt(TaskStruct *task,GrDC *dc)
{
  I64 i;
  F64 dt=(tM-t0)*music_tempo,d=Limit(0.1*dt,0,2),d2;

  dc->color=ROPF_PROBABILITY_DITHER+LTCYAN<<16+CYAN;
  for (i=HORIZON_Y;i>=0;i--) {
    d2=ToF64(HORIZON_Y-i)/HORIZON_Y;
    dc->dither_probability=1<<16*Limit((4.0-d2)/4.0*d*d-0.15,0,1.0);
    GrLine(dc,LEFT_EDGE,i,task->win_pixel_width-1,i);
  }

  d=(task->win_pixel_width-LEFT_EDGE)/2+LEFT_EDGE;
  dc->color=YELLOW;
  GrCircle(dc,d,HORIZON_Y+50-dt,40);
  GrFloodFill(dc,d,HORIZON_Y+15-dt,TRUE);

  dc->color=GREEN;
  GrRect(dc,200,HORIZON_Y,task->win_pixel_width-200,task->win_pixel_height-HORIZON_Y);
}

U0 Song()
{
  SettingsPush; //See $LK,"SettingsPush","MN:SettingsPush"$
  Fs->draw_it=&DrawIt;
  Fs->task_end_cb=&SndTaskEndCB;

  MusicSettingsRst;
  music_tempo= 2.500;
  music_stacatto_factor= 0.900;
  try {
    while (!ScanKey) {
      t0=tM;
      Play("3qB!eB2G!3qA!!eF!!AsF!AFA!!qC2sG!3A!2G3ARBRB",
	  "New\0day\0is\0born.\n\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");
      Play("qB!eB2G!3qA!!eF!!AsF!AFA!!qC2sG!3A!2G3ARBRB",
	  "Though\0it's\0the\0mourn',\n\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");
      Play("!eDDR!!CqG!2eGR3qE!!etEA!!FqCsDGDG",
	  "cheer-\0up\0\0the\0heart\n\0\0\0\0\0\0\0\0\0as\0\0you\0");
      Play("!eDDR!!CqG!2eGR3qE!!etEA!!FqCsDGDG",
	  "hitch-\0up\0\0the\0cart.\n\0\0\0\0\0\0\0\0\0\0\0\0");
    }
  } catch
    CatchAll;
  SettingsPop;
}

Song;
