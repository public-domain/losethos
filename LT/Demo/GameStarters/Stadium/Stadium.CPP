/*
This shows how you can make a
background very easily.
*/




$PI,"<fan>",1$



 
$PI,"<ball>",2$


GrDC *background;

#define BALL_TIME	0.2
#define MAX_FANS 10
I64 x[MAX_FANS],y[MAX_FANS];
BoolI8 hit[MAX_FANS];
F64 �[MAX_FANS],ball_t;
I64 target_x,target_y,pitcher_x,pitcher_y;

U0 DrawIt(TaskStruct *task,GrDC *dc)
{
  I64 i,*r;
  F64 xx,yy,t0;
  background->flags|=DCF_NO_TRANSPARENTS;
  GrDCBlot(dc,0,0,background);
  for (i=0;i<MAX_FANS;i++)
    GrSpritePlotRotZ3b(dc,x[i],y[i],0,$IB,"<fan>",1$,�[i]);
  if (ball_t) {
    t0=(tP(task)-ball_t)/BALL_TIME;
    if (t0>1.0)
      ball_t=0;
    else {
      xx=t0*target_x+(1.0-t0)*pitcher_x;
      yy=t0*target_y+(1.0-t0)*pitcher_y;
      xx/=1.5-t0;
      yy/=1.5-t0;
      r=GrScaleMat(dc->r,1.5-t0,dc->mem_task);
      Free(dc->r);
      GrSetRotMat(dc,r);
      dc->flags|=DCF_TRANSFORMATION;
      GrSpritePlotRotZ3b(dc,xx,yy,0,$IB,"<ball>",2$,t0);
    }
  }
  dc->color=RED;
  GrPutS(dc,FONT_WIDTH,FONT_HEIGHT,"Peg the Fans");
}

U0 AnimateTask(U64 dummy=0)
{
  nounusedwarn dummy;
  I64 i;
  F64 xx,yy,t0;
  while (TRUE) {
    if (ball_t) {
      t0=(tP(Fs->parent_task)-ball_t)/BALL_TIME;
      xx=t0*target_x+(1.0-t0)*pitcher_x;
      yy=t0*target_y+(1.0-t0)*pitcher_y;
    }
    for (i=0;i<MAX_FANS;i++) {
      if (ball_t)
	if (Sqr(x[i]-xx)+Sqr(y[i]-yy)<200) {
	  hit[i]=TRUE;
	  �[i]=-�/2;
	}
      if (!hit[i]) {
	x[i]+=SignI64(RandI16);
	y[i]+=SignI64(RandI16);
	�[i]+=Sign(RandI16)/25.0;
	if (!(0<=x[i]<GR_WIDTH)) x[i]=GR_WIDTH/2;
	if (!(10<=y[i]<100)) y[i]=50;
	if (!(-0.75<=�[i]<0.75)) �[i]=0;
      }
    }
    Sleep(10);
  }
}


U0 Init()
{
  I64 i;
  for (i=0;i<MAX_FANS;i++) {
    x[i]=RandU16%GR_WIDTH;
    y[i]=50;
    �[i]=0;
    hit[i]=FALSE;
  }
}

U0 Stadium()
{
  I64 msg_code,p1,p2,ch=0;

  SettingsPush; //See $LK,"SettingsPush","MN:SettingsPush"$
  Fs->win_inhibit|=WIF_DBL_CLICK;

  MenuPush(
"File {"
"  Abort(,CH_SHIFT_ESC);"
"  Exit(,CH_ESC);"
"}"
"Play {"
"  Restart(,CH_NEW_LINE);"
"}"
);
  WinMax;
  WinBorder;
  WordStat;
  Init;
  Fs->animate_task=Spawn(&AnimateTask,NULL,"Animate",Fs);

  background=LGRRead("::/LT/Demo/GameStarters/Stadium/Stadium");

  LtfCursor;
  LtfClear;
  Fs->draw_it=&DrawIt;
  do {
    msg_code=GetMsg(&p1,&p2,1<<MSG_KEY_DOWN+1<<MSG_IP_L_DOWN);
    if (msg_code==MSG_KEY_DOWN) {
      ch=p1;
      if (ch==CH_NEW_LINE)
	Init;
    } else {
      target_x=p1; target_y=p2;
      pitcher_x=GR_WIDTH/2; pitcher_y=GR_HEIGHT;
      ball_t=tP;
    }
  } while (ch!=CH_SHIFT_ESC && ch!=CH_ESC);

  GrDCDel(background);
  SettingsPop;
  MenuPop;
}

Stadium;
           �     ����������������������������������������������������������������������������������������    ����    ����    ����    ��������������������   ����   ����   ����   ����   ����   ����   ����   ����   ����   ����
   ����
   ����
   ����
   ����   ����   ����   ����   ����
   ����
   ����   ����   ����   ����   ����      ����������������������������              R           ����������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������� 