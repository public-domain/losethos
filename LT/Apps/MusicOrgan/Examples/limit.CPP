//9 has graphics

F64 t0=tM;

$PI,"<1>",1$

U0 DrawIt(TaskStruct *task,GrDC *dc)
{
  nounusedwarn task;
  F64 dt=(tM-t0)*music_tempo;

  dc->color=BLACK;
  if (dt<21.5)
    GrPutS(dc,235,30,"- -");
  else {
    GrSpritePlot3(dc,235,30,0,$IB,"<1>",1$);
    GrPutS(dc,235,30,"o o");
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
	Play("W13qBDAGFFeBAsD2G3D2G",
		"Do\0not\0push\0your\0luck.\n\0\0\0\0\0\0\0\0");
	Play("3qBDAGFFeBAsD2G3D2G",
		"Do\0not\0push\0your\0luck.\n\0\0\0\0\0\0\0\0");
	Play("3qGFCeCGqFetAAFqEF",
		"God\0is\0not\0\0a\0schmuck.\n\0\0\0\0\0\0");
	Play("GFCeCGqFetAAFqEF",
		"God\0is\0not\0\0a\0schmuck.\0\0\0\0No!\n\0\0");
	t0=tM;
	Play("BDAGFFeBAsD2G3D2G",
		"\nDo\0not\0tell\0the\0Lord\n\0\0\0\0\0\0\0\0");
	Play("3qBDAGFFeBAsD2G3D2G",
		"that\0you're\0feel\0ing\0bored.\n\0\0\0\0\0\0\0\0");
	Play("3qGFCeCGqFetAAFqEF",
		"He\0will\0send\0\0the\0sword.\n\0\0\0\0\0\0");
	Play("GFCeCGqFetAAFqEF",
		"He\0will\0send\0\0the\0sword.\0\0\0\0Yep!\n\n\0\0");
	"$$FG,GREEN$$(Boredom is an insult to the Creator.\nWe are like guest at a banquet.)\n$$FG$$";
  }
  } catch
    CatchAll;
  SettingsPop;
}

Song;
           G          ����   ����   ����   ����   ����   ����   ����   ���� 