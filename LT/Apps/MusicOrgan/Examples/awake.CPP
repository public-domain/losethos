//8 has graphics

#define R1	13
#define R2	21

U0 DrawIt(TaskStruct *task,GrDC *dc)
{
  static F64 �=0;
  nounusedwarn task;
  I64 x1=200,y1=20,x2=x1,y2=y1+R2;
  F64 tt=tM*music_tempo%16.0,�;
  �+=1/(tt*tt+1);
  dc->color=BLACK;
  dc->pen_width=4;
  GrCircle3(dc,x2,y2,0,R2+10);
  for (�=�;�<�+2*�;�+=�/3) {
    dc->color=BLACK;
    dc->pen_width=2;
    GrCircle3(dc,x2+R2*Cos(�),y2-R2*Sin(�),0,8);
    dc->color=WHITE;
    dc->pen_width=2;
    GrCircle3(dc,x2+(R2+15)*Cos(�+�/6),y2-(R2+15)*Sin(�+�/6),0,3);
  }
  dc->color=BROWN;
  GrFloodFill(dc,x2+R2*Cos(�),y2-R2*Sin(�));
  dc->color=DKGRAY;
  GrFloodFill(dc,x2,y2);
  dc->pen_width=6;
  dc->color=RED;
  GrCircle3(dc,x1,y1,0,R1-6);
  dc->color=DKGRAY;
  GrCircle3(dc,x1,y1,0,R1-2);
  dc->color=LTGRAY;
  GrRect(dc,x1-2,y1-R2-1,4,10);
  GrCircle3(dc,x1,y1,0,R1);
  dc->color=RED;
  GrFloodFill(dc,x1,y1,TRUE);
  dc->color=BLACK;
  GrFloodFill(dc,x1,y1);
}


U0 Song()
{
  TaskStruct *task;
  SettingsPush; //See $LK,"SettingsPush","MN:SettingsPush"$
  Fs->draw_it=&DrawIt;
  Fs->task_end_cb=&SndTaskEndCB;
  MusicSettingsRst;
  music_tempo= 2.480;
  music_stacatto_factor= 0.902;
  try {
  task=PopUpViewStr(
    "$LK,"Mark,14:37","BF:Mark,14:37"$\n"
    );
  TaskWaitIdle(task);
  task->win_right=Fs->win_right;
  task->win_left=Fs->win_left;
  task->win_top=Fs->win_bottom+3;
  task->win_bottom=task->win_top+12;
  TaskDerivedValsUpdate(task);
  while (!ScanKey) {
	"$$FG,GREEN$$Jesus talking...$$FG$$\n";
	Play("W13eBC!qB2sG3D2G3DetC!!GFsCBCB!etCGBeB!!GsBCB!C",
		"Stay\0a\0wake.\n\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");
	Play("eBC!qB2sG3D2G3DetC!!GFsCBCB!etCGBeB!!GsBCB!C",
		"Stay\0a\0wake.\n\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");
	Play("qD!sD!BD!BetA2G3GqB2G3sAD!ADqD!etGF!D",
		"\0\0\0\0\0\0\0\0\0\0\0I\0\0am\0a\0dead\0\0man.\n\0");
	Play("qD!sD!BD!BetA2G3GqB2G3sAD!ADqD!etGF!D",
		"\0\0\0\0\0\0\0\0\0\0\0I\0\0am\0a\0dead\0\0man.\n\0");
  }
  } catch
    CatchAll;
  SettingsPop;
}

Song;
