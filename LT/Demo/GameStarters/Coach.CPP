$BG,2$




  $PI,"<1>",1$










  $PI,"<2>",2$ $PI,"<3>",3$ $PI,"<4>",4$ $PI,"<5>",5$ $PI,"<6>",6$ $PI,"<7>",7$ $PI,"<8>",8$ $PI,"<9>",9$ $PI,"<10>",10$

$BG$

#define BASE_X		(GR_WIDTH/2)
#define BASE_Y		(3*GR_HEIGHT/4)
#define TEE_HEIGHT	34

#define BATTER_T	0.25
#define NUM_BATTER_IMGS	4
U8 *batter_imgs[NUM_BATTER_IMGS]={$IB,"<2>",2$,$IB,"<3>",3$,$IB,"<4>",4$,$IB,"<5>",5$};

#define RUNNER1_T		1.5
#define NUM_RUNNER1_IMGS	6
U8 *runner1_imgs[NUM_RUNNER1_IMGS]={$IB,"<5>",5$,$IB,"<6>",6$,$IB,"<7>",7$,$IB,"<7>",7$,$IB,"<7>",7$,$IB,"<8>",8$};

#define RUNNER2_T		0.4
#define NUM_RUNNER2_IMGS	4
U8 *runner2_imgs[NUM_RUNNER2_IMGS]={$IB,"<8>",8$,$IB,"<9>",9$,$IB,"<8>",8$,$IB,"<10>",10$};

F64 t0,hit_t0;
F64 ball_x,ball_y;

U0 DrawIt(TaskStruct *task,GrDC *dc)
{
  F64 tt=tP(task)-t0;
  I64 i=(NUM_BATTER_IMGS-1)*tt/BATTER_T;
  U8 *inter;

  task->text_attr=GREEN<<4;
  dc->color=WHITE;
  dc->pen_width=3;
  GrLine3(dc,BASE_X,BASE_Y,0,BASE_X-200,BASE_Y-200,0);
  GrLine3(dc,BASE_X,BASE_Y,0,BASE_X+200,BASE_Y-200,0);

  GrSpritePlot3(dc,BASE_X,BASE_Y,0,$IB,"<1>",1$);
  dc->color=WHITE;
  GrCircle(dc,ball_x+BASE_X,ball_y+BASE_Y-TEE_HEIGHT,2);
  if (!t0)
    GrSpritePlot3(dc,BASE_X-23,BASE_Y,0,batter_imgs[0]);
  else if (i<NUM_BATTER_IMGS-1) {
    inter=GrSpriteInterpolate(batter_imgs[i],batter_imgs[i+1],
      ((NUM_BATTER_IMGS-1)/BATTER_T)*(tt%(BATTER_T/(NUM_BATTER_IMGS-1))));
    GrSpritePlot3(dc,BASE_X-23,BASE_Y,0,inter);
    Free(inter);
    if (!hit_t0 && tt>BATTER_T/2)
      hit_t0=tP(task);
  } else {
    tt-=BATTER_T;
    i=(NUM_RUNNER1_IMGS-1)*tt/RUNNER1_T;
    if (i<NUM_RUNNER1_IMGS-1 ) {
      inter=GrSpriteInterpolate(runner1_imgs[i],runner1_imgs[i+1],
	((NUM_RUNNER1_IMGS-1)/RUNNER1_T)*(tt%(RUNNER1_T/(NUM_RUNNER1_IMGS-1))));
      GrSpritePlot3(dc,BASE_X-23,BASE_Y,0,inter);
      Free(inter);
    } else {
      tt-=RUNNER1_T;
      i=NUM_RUNNER2_IMGS*tt/RUNNER2_T;
      if (i<30) {
	inter=GrSpriteInterpolate(runner2_imgs[i%NUM_RUNNER2_IMGS],runner2_imgs[(i+1)%NUM_RUNNER2_IMGS],
	  (NUM_RUNNER2_IMGS/RUNNER2_T)*(tt%(RUNNER2_T/NUM_RUNNER2_IMGS)));
	GrSpritePlot3(dc,BASE_X-23-50*tt,BASE_Y-50*tt,0,inter);
	Free(inter);
      } else {
	t0=0;
	ball_x=0;
	ball_y=0;
      }
    }
  }
}

#define BALL_PATH_POINTS	7
P3I32 ball_path[BALL_PATH_POINTS]={
  {0,0,0},{20,-40,0},{40,20,0},
  {40,20,0},{50,-30,0},{55,10,0},
  {55,10,0}
};

BoolI64 PlotBall(U0 *aux,I64 _x,I64 _y,I64 _z)
{ nounusedwarn aux,_z;
  ball_x=_x; ball_y=_y;
  Sleep(3);
  return TRUE;
}

U0 AnimateTask(U64 dummy=0)
{
  nounusedwarn dummy;
  while (TRUE) {
    if (hit_t0) {
      Noise(25,2000,3000);
      BSpline2(NULL,&ball_path,BALL_PATH_POINTS,&PlotBall);
      hit_t0=0;
    }
    Sleep(1);
  }
}

U0 Init()
{
  t0=0;
  hit_t0=0;
  ball_x=0;
  ball_y=0;
}

U0 TeeBall()
{
  SettingsPush; //See $LK,"SettingsPush","MN:SettingsPush"$
  WinMax;
  WinBorder;
  Preempt(OFF);

  Fs->win_inhibit|=WIF_DBL_CLICK;
  WordStat;
  Fs->animate_task=Spawn(&AnimateTask,NULL,"Animate",Fs);

  MenuPush(
  "File {"
  "  Abort(,CH_SHIFT_ESC);"
  "  Exit(,CH_ESC);"
  "}"
  "Play {"
  "  Swing(,CH_SPACE);"
  "  Restart(,CH_NEW_LINE);"
      "}"
      );

  Init;
  LtfCursor;
  LtfClear;
  Fs->draw_it=&DrawIt;

  try {
    while (TRUE) {
      switch (GetChar(NULL,FALSE)) {
	case CH_NEW_LINE:
	  Init;
	  break;
	case CH_SPACE:
	  t0=tP;
	  break;
	case CH_SHIFT_ESC:
	case CH_ESC:
	  goto tb_done;
      }
    }
tb_done:
  } catch
    CatchAll;
  SettingsPop;
  MenuPop;
}

TeeBall;
           �         ������������������������   ������������   ����������������������������    ����    ����   ����   ������������   ����
   ����           �         ������������������������   ������������   ����������������������������    ����    ����
   ����   ������������   ����   ����           �         ������������������������    ������������   ����������������������������    ����   ����
   ����   ����   ����
   ����   ����           �         ������������������������   ������������   ��������������������    ����    ������������   ����   ������������   ����
   ����           2                    ����       �    ����    ����           �            ����������������������������������������������������   ����    ��������������������   ����   ������������   ����   ����           �            ��������������������������������������������   ����   ��������������������������������������������������������������������           �            ������������������������������������    ����������������������������������������������������   ����    ����   ����   ����    	       �             ������������������������������������    ����������������������������������������������������   ����    ����   ����   ����    
       �            ������������������������������������    ������������������������������������������������������������    ������������   ���� 