$BG,14$





$PI,"<1>",1$










$PI,"<2>",2$














$PI,"<3>",3$










$PI,"<4>",4$




$BG$
F64 t0,xx,t_last;
BoolI8 held_up;

#define HACK_PERIOD	0.25
#define SPACING			45

U0 DrawBattle(TaskStruct *task,GrDC *dc)
{
  I64 cx=task->win_pixel_width>>1,
     cy=task->win_pixel_height>>1;
  U8 *inter;
  F64 tt;
  if (held_up)
    tt=0;
  else {
    tt=(tP(task)-t0)/2;
    tt*=tt;
    tt*=tt;
    if (tt>1.0) tt=1.0;
  }
  inter=GrSpriteInterpolate($IB,"<3>",3$,$IB,"<4>",4$,tt);
  GrSpritePlot3(dc,cx,cy+SPACING,0,inter);
  Free(inter);

  if (t_last) {
    if (tt<0.5)
      xx-=50.0*(tP(task)-t_last);
    else
      xx+=50.0*(tP(task)-t_last);
  }

  tt=Saw(tP(task)+0.0*HACK_PERIOD,HACK_PERIOD);
  tt*=2; if (tt>1.0) tt=2.0-tt;
  inter=GrSpriteInterpolate($IB,"<1>",1$,$IB,"<2>",2$,tt);
  GrSpritePlot3(dc,xx+cx+SPACING,cy-SPACING,0,inter);
  Free(inter);

  tt=Saw(tP(task)+0.333*HACK_PERIOD,HACK_PERIOD);
  tt*=2; if (tt>1.0) tt=2.0-tt;
  inter=GrSpriteInterpolate($IB,"<1>",1$,$IB,"<2>",2$,tt);
  GrSpritePlot3(dc,xx+cx+2*SPACING,cy-SPACING,0,inter);
  Free(inter);

  tt=Saw(tP(task)+0.666*HACK_PERIOD,HACK_PERIOD);
  tt*=2; if (tt>1.0) tt=2.0-tt;
  inter=GrSpriteInterpolate($IB,"<1>",1$,$IB,"<2>",2$,tt);
  GrSpritePlot3(dc,xx+cx+SPACING,cy-2*SPACING,0,inter);
  Free(inter);

  t_last=tP(task);
}

U0 Battle()
{
  I64 msg_code,ch,sc;

  SettingsPush(Fs,TSF_SAME_SONG); //See $LK,"SettingsPush","MN:SettingsPush"$
  try {
    LtfClear;
    "$$BG,YELLOW$$";

    PutBibleVerse("Exodus,17:11",8);
    "\n$$BK,1$$Hold <SPACE BAR>$$BK,0$$\n";

    held_up=FALSE;
    t0=tP;
    t_last=0;
    xx=0;
    Fs->draw_it=&DrawBattle;
    do {
      msg_code=GetMsg(&ch,&sc,1<<MSG_KEY_DOWN|1<<MSG_KEY_UP);
      if (ch==CH_SPACE) {
	if (msg_code==MSG_KEY_DOWN)
	  held_up=TRUE;
	else if (msg_code==MSG_KEY_UP) {
	  held_up=FALSE;
	  t0=tP;
	}
      }
    } while (msg_code!=MSG_KEY_DOWN || ch!=CH_SHIFT_ESC && ch!=CH_ESC);
  } catch
    CatchAll;
  "$$BG$$\n";
  SettingsPop(Fs,TSF_SAME_SONG);
}

           �         ����   ����   ����                                 ����   ����              ��������������������   ����   ����   ����   ����   ����   ������������������������������������   ����&   ����           �         ����   ����   ����                                 ����   ����              ��������������������   ����   ����   ����   ����   ����   ������������������������������������   ����                  �        -   #   3   !   3   !   0      0      2   ����2   ����.   ����1   ����9   ����3   ����1   ����8   ����:   ����3   ����5      5      9   "   -   ����(   ����(   ����   ����9   ����,   ����,   ����6   ����6   ����:   ����:   ����7      7      -      -      )   ����)   ����-   ����*   ����6   ����3   ����.   ����,   ����6   ����0   ����4   ����/      4         ����   ����   ����)   ����           �        '   #   .      -      *      )      2      0      '   ����1   ����4   ����.   ����+   ����6   ����5   ����3   ����9      9      B      (   ����%   ����%   ����   ����3   ����'   ����+   ����2   ����2   ����6   ����6   ����2      1      '      '      %   ����)   ����(   ����*   ����3   ����3   ����)   ����&   ����3   ����&   ����4   ����'      3         ����   ����   ������������ 