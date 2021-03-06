



$PI,"<1>",1$




class MPCtrl {
  BoolI8 core0_done,core1_done;
} mp;


I64 *r1,*r2;

U0 DrawIt(TaskStruct *task,GrDC *dc)
{
  I64 *old_r;

  if (mp.core0_done) return;
  old_r=dc->r;
  dc->color=BLACK;
  GrPutS(dc,0,0,"This is not art.");
  GrPutS(dc,0,FONT_HEIGHT,"It's just a test pattern.");

  GrSetRotMat(dc,r1);
  dc->x=200;
  dc->y=200;
  dc->flags|=DCF_TRANSFORMATION;

  dc->pen_width=2;
  GrSpritePlot3(dc,0,0,0,$IB,"<1>",1$);

  dc->color=BLUE;
  GrRect3(dc,50,75,0,100,50);
  GrRect3(dc,75,50,0,50,100);
  dc->color=GREEN;
  GrEllipse3(dc,100,100,0,50,25);
  GrEllipse3(dc,100,100,0,50,25,�/2);
  dc->color=LTGREEN;
  GrEllipse3(dc,100,100,0,50,25,�/6);
  GrEllipse3(dc,100,100,0,50,25,�/3);

  GrSetRotMat(dc,old_r);
  mp.core0_done=TRUE;
}


U0 MPDrawIt(TaskStruct *main_task)
{
  GrDC *dc;
  I64 *old_r;

  if (mp.core1_done) return;

  dc=GrDCAlias;
  old_r=dc->r;
  dc->win_task=main_task;
  GrSetRotMat(dc,r2);
  dc->x=200;
  dc->y=200;
  dc->flags|=DCF_TRANSFORMATION;

  GrDCFill(dc);

  dc->pen_width=2;
  GrSpritePlot3(dc,0,0,0,$IB,"<1>",1$);

  dc->color=BLUE;
  GrRect3(dc,50,75,0,100,50);
  GrRect3(dc,75,50,0,50,100);

  dc->color=GREEN;
  GrEllipse3(dc,100,100,0,50,25);
  GrEllipse3(dc,100,100,0,50,25,�/2);

  dc->color=LTGREEN;
  GrEllipse3(dc,100,100,0,50,25,�/6);
  GrEllipse3(dc,100,100,0,50,25,�/3);

  GrSetRotMat(dc,old_r);
  GrDCDel(dc);
  mp.core1_done=TRUE;
}

U0 Main()
{
  F64 �2=0,�2=0,�3=0,�3=0;
  mp.core0_done=TRUE;
  mp.core1_done=TRUE;
  r1=GrIdent;
  r2=GrIdent;

  SettingsPush; //See $LK,"SettingsPush","MN:SettingsPush"$
  LtfClear;
  Fs->draw_it=&DrawIt;
  Preempt(OFF); //Don't access r1,r2 while being changed

  while (!ScanChar) {
    GrIdentEqu(r1);
    GrRotXEqu(r1,�2);
    GrRotZEqu(r1,�2);

    GrIdentEqu(r2);
    GrRotXEqu(r2,�3);
    GrRotZEqu(r2,�3);

    mp.core0_done=FALSE;
    mp.core1_done=FALSE;

    //Pass the task of the window as data item to job.
    MPJob(&MPDrawIt,Fs,1<<MPCf_FREE_ON_COMPLETE,1<<1);

    WinSync;
    while (!mp.core0_done || !mp.core1_done)
      Yield;

    �2+=2*�/100;
    �2	+=2*�/130;
    �3-=2*�/100;
    �3	-=2*�/130;
  }
  GrDCFill;

  SettingsPop;
  Free(r1);
  Free(r2);
}

Main;
           �            #   ����#   ����.      .                             ����   J      I   ����������������         !      ��%9R��      =      ���7R��             lU9R��/                   � 