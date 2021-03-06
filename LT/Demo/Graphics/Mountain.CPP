



$PI,"<1>",1$	$PI,"<2>",2$	  $PI,"<3>",3$


BoolI8 animate_done;
GrSprite *man_imgs[4]={$IB,"<1>",1$,$IB,"<2>",2$,$IB,"<3>",3$,$IB,"<2>",2$};
I64 x=0,y=0;

U0 DrawMountain(TaskStruct *task,GrDC *dc)
{
//"tP()" is a function holding time since boot, less pauses.
  I64 frame=4*Saw(tP(task),0.5),
      cx=task->win_pixel_width/2,
      cy=task->win_pixel_height/2;
  task->text_attr=YELLOW<<4;
  dc->color=BROWN;
  dc->pen_width=2;
  GrLine3(dc,cx-400,cy,0,    cx-30 ,cy-120,0);
  GrLine3(dc,cx-30,cy-120,0, cx    ,cy-160,0);
  GrLine3(dc,cx+160,cy,0,    cx,cy-160,0);
  GrLine3(dc,cx+80 ,cy-80,0, cx+160,cy-120,0);
  GrLine3(dc,cx+160,cy-120,0,cx+400,cy,0);

  GrLine3(dc,cx,cy,0,cx+100,cy-20,0);
  GrLine3(dc,cx+100,cy-20,0,cx-90,cy-40,0);
  GrLine3(dc,cx-90,cy-40,0,cx+70,cy-60,0);
  GrLine3(dc,cx+70,cy-60,0,cx-70,cy-60,0);
  GrLine3(dc,cx-70,cy-60,0,cx+60,cy-80,0);
  GrLine3(dc,cx+60,cy-80,0,cx-50,cy-100,0);
  GrLine3(dc,cx-50,cy-100,0,cx+30,cy-120,0);
  GrLine3(dc,cx+30,cy-120,0,cx-15,cy-140,0);

  dc->color=LTCYAN;
  GrRect(dc,0,0,task->win_pixel_width,74);
  GrFloodFill(dc,0,75);

  dc->color=BROWN;
  GrCircle(dc,cx+70,35,15);
  dc->color=YELLOW;
  GrFloodFill(dc,cx+70,35);

  dc->color=BROWN;
  GrPutS(dc,cx-8*FONT_WIDTH/2,cy-168,"Mt. Horeb");

  GrSpritePlot3(dc,x+cx,y+cy,0,man_imgs[frame&3]);
}

//This is the path of the man.
P3I32 mountain[19]={{0,0,0},{100,-20,0},{100,-20,0},{-100,-40,0},{-100,-40,0},
		  {80,-60,0},{80,-60,0},{-80,-60,0},{-80,-60,0},
		  {60,-80,0},{60,-80,0},{-60,-100,0},{-60,-100,0},{40,-120,0},
		  {40,-120,0},{-37,-140,0},{-37,-140,0},{0,-160,0},{0,-160,0}};

BoolI64 PlotMan(U0 *aux,I64 _x,I64 _y,I64 _z)
{ nounusedwarn aux,_z;
  x=_x; y=_y;
  Sleep(4);
  return TRUE;
}

U0 MountainTask(U64 dummy=0)
{
  nounusedwarn dummy;
//Most of the graphic shape routines can be called with
//a custom routine to plot the points.
//See $LK,"Graphics/Math","HI:Graphics/Math"$.
  BSpline2(NULL,&mountain,19,&PlotMan);
  animate_done=TRUE;
}

U0 Mountain()
{
  SettingsPush(Fs,TSF_SAME_SONG); //See $LK,"SettingsPush","MN:SettingsPush"$
  try {
    WordStat;
    WinMax;
    WinBorder;
    LtfCursor;
    LtfClear;
    Fs->draw_it=&DrawMountain;

    animate_done=FALSE;
    Spawn(&MountainTask,NULL,"Animate",Fs);
    while (!animate_done)
      Yield;
  } catch
    CatchAll;
  SettingsPop(Fs,TSF_SAME_SONG);
}

//This is so I can use this file stand alone
//or as a component of a program.
#if IncludeDepth<2
Mountain;
#endif
                    ����       ����   ����          ����   ����   ����    ����   ����   ����   ����
   ��������������������           �         ����        ����    ����   ����   ����           ����   ��������������������    ����    ����   ����   ��������������������           �             ������������������������        ����   ����   ����           ����   ��������������������    ����    ����   ����   �������������������� 