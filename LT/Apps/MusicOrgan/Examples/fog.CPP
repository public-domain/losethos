//7 has graphics


$PI,"<1>",1$










#define LH_X 260
#define LH_Y 20

F64 t0=tM;

U0 DrawIt(TaskStruct *task,GrDC *dc)
{
  nounusedwarn task;
  F64 dt=(tM-t0)*music_tempo,y;
  GrSpritePlot3(dc,LH_X,LH_Y,0,$IB,"<1>",1$);
  if (dt>24) {
    dc->color=YELLOW;
    for (y=-30;y<30;y+=5)
      GrLine(dc,LH_X,LH_Y,LH_X+80*(dt-28),LH_Y+y);
  }
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
	Play("3etACDsEEEEetGG2GqGetG3BAqBetRBGCAG",
		"I\0walk\0\0bo-\0o-\0ld\0ly\0\0\0\0\0\0though\0the\0fog.\n\0\0\0\0\0\0\0");
	Play("ACDsEEEEetGG2GqGetG3BAqBetRBGCAG",
		"I\0got\0\0lit\0\0tle\0\0choice\0\0\0\0but\0to\0trust\0God.\n\0\0\0\0\0\0\0");
	Play("qBetCCCqCCeCCCDqEetEBG",
		"\0Don't\0\0kno-\0ow\0\0\0\0what\0to\0do.\n\0\0\0\0");
	Play("qBetCCCqCCeCCCDqEetEBG",
		"\0Where\0I\0go-\0o,\0\0\0\0I've\0no\0clue.\n\0\0\0\0");
  }
  } catch
    CatchAll;
  SettingsPop;
}

Song;
           �      ������������-   ��������   ����   ����   ,      ,   ����,   ����&         ����         ����	         ����                  ����   ������������������������      +      7   ����.   ����9   ����1   ����=      0      ;   ����5   ����>      2      =      4      >      ����         ����'         ����         ����.      .      1   k   4   ����0   ����1      -   k   /      -   
   (   ����(   ����,   ����,   ����-    