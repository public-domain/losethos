//This uses $LK,"fixed-point","FI:::/LT/Demo/Lectures/FixedPoint.CPZ"$.

#define NUM_CLOUDS		16
#define SKY_LINES		30
#define CLOUD_POINTS		512
#define CLOUD_PENS		8
#define CLOUD_PEN_POINTS	16
#define CLOUD_PEN_SIZE		16

class Cloud
{
  I64 i,x,y,w,h,dx,dy;
  U64 color;
  I64 px[CLOUD_POINTS],py[CLOUD_POINTS],pc[CLOUD_POINTS];
} clouds[NUM_CLOUDS];

class CloudPen
{
  GrDC *img;
  I64 px[CLOUD_PEN_POINTS],py[CLOUD_PEN_POINTS];
} cloud_pens[CLOUD_PENS];

GrDC *cloud_pen;

I64 mp_done_flags;
U0 MPDrawClouds(TaskStruct *task)
{
  Cloud *c;
  GrDC *dc=GrDCAlias(gr_dc,task);
  I64 i,j,k,xx,yy,
     lo= Gs->num   *CLOUD_POINTS/mp_cnt,
     hi=(Gs->num+1)*CLOUD_POINTS/mp_cnt;

  for (j=0,c=clouds;j<NUM_CLOUDS;j++,c++)
    for (i=lo;i<hi;i++) {
      k=c->pc[i];
      if (k<c->color)
	dc->color=ROP_MONO+LTGRAY;
      else
	dc->color=ROP_MONO+WHITE;
      xx=c->x.i32[1]+c->px[i];
      yy=c->y.i32[1]+c->py[i];

      k=RandU16&31-16;
      if (k==-16)
	k=-c->px[i];
      c->px[i]+=SignI64(k);

      k=RandU16&31-16;
      if (k==-16)
	k=-c->py[i];
      c->py[i]+=SignI64(k);

      GrDCBlot(dc,xx,yy,cloud_pens[i&(CLOUD_PENS-1)].img);
    }
  GrDCDel(dc);
  LBts(&mp_done_flags,Gs->num);
}

U0 DrawIt(TaskStruct *task,GrDC *dc)
{
  nounusedwarn dc;
  I64 i;
  mp_done_flags=0;
  for (i=1;i<mp_cnt;i++)
    MPJob(&MPDrawClouds,task,1<<MPCf_FREE_ON_COMPLETE,1<<i);
  MPDrawClouds(task);
  while (mp_done_flags!=1<<mp_cnt-1)
    Yield;
}

U0 AnimateTask(U64 dummy=0)
{
  nounusedwarn dummy;
  I64 i,j;
  Cloud *c;
  CloudPen *cp;
  while (TRUE) {
    WinSync;

    for (i=0,cp=cloud_pens;i<CLOUD_PENS;i++,cp++) {
      GrDCClear(cp->img);
      cp->img->color=COLOR_MONO;
      for (j=0;j<CLOUD_PEN_POINTS;j++) {
	GrPlot(cp->img,cp->px[j],cp->py[j]);
	cp->px[j]=LimitI64(cp->px[j]+RandU16%3-1,0,CLOUD_PEN_SIZE-1);
	cp->py[j]=LimitI64(cp->py[j]+RandU16%3-1,0,CLOUD_PEN_SIZE-1);
      }
    }

    for (i=0,c=clouds;i<NUM_CLOUDS;i++,c++) {
      c->x+=c->dx;
      c->y=LimitI64(c->y+c->dy,0,0.7*(SKY_LINES*FONT_HEIGHT)<<32);
      c->color=MAX_U16*c->y.i32[1]/(0.8*(SKY_LINES*FONT_HEIGHT));
    }

    Sleep(20);
  }
}

#define SAMPLES	6
U0 Init()
{
  Cloud *c=clouds;
  CloudPen *cp=cloud_pens;
  I64 i,j,k,l,
     w=Fs->win_pixel_width,
     h=SKY_LINES*FONT_HEIGHT;

  for (i=0;i<NUM_CLOUDS;i++,c++) {
    c->x=ToI64(w/2+Rand*w/2-w/4)<<32;
    c->y=ToI64(h/2+Rand*h/2-h/4)<<32;
    c->dx=RandI32;
    c->dy=RandI32;
    c->w=100;
    c->h=50;
    c->color=RandU16;
    for (l=0;l<CLOUD_POINTS;l++) {
      k=0;
      for (j=0;j<SAMPLES;j++)
	k+=RandI16;
      c->px[l]=(k*c->w/MAX_I16)/SAMPLES;

      k=0;
      for (j=0;j<SAMPLES;j++)
	k+=RandI16;
      c->py[l]=(k*c->h/MAX_I16)/SAMPLES;

      c->pc[l]=RandU16;
    }
  }

  for (i=0;i<CLOUD_PENS;i++,cp++) {
    cp->img=GrDCNew(DCT_COLOR8,CLOUD_PEN_SIZE,CLOUD_PEN_SIZE);
    for (j=0;j<CLOUD_PEN_POINTS;j++) {
      cp->px[j]=RandU16%CLOUD_PEN_SIZE;
      cp->py[j]=RandU16%CLOUD_PEN_SIZE;
    }
  }
}

U0 CleanUp()
{
  I64 i;
  CloudPen *cp=cloud_pens;
  for (i=0;i<CLOUD_PENS;i++,cp++)
    GrDCDel(cp->img);
}

U0 SongTask(U64 dummy=0)
{ //Song by the Holy Spirit
  nounusedwarn dummy;
//This specifies a call-back routine
//to be called when the task dies.
  Fs->task_end_cb=&SndTaskEndCB;
  MusicSettingsRst;
  while (TRUE) {
    Play("W13qBetDA2G3qD2sG!3E2G!3EetCEDqFEeDC");
    Play("qBetDA2G3qD2sG!3E2G!3EetCEDqFEeDC");
    Play("C!GqDeADsDCDCqGEetDAD!2sG3D2G!!3D");
    Play("eC!GqDeADsDCDCqGEetDAD!2sG3D2G!!3D");
  }
}

U0 CloudScene()
{
  SettingsPush; //See $LK,"SettingsPush","MN:SettingsPush"$
  try {
    Fs->song_task=Spawn(&SongTask,NULL,"Song",Fs);
    LtfCursor;
    WinMax;
    WordStat;
    Init;
    Fs->animate_task=Spawn(&AnimateTask,NULL,"Animate",Fs);

    "$$CL$$$$BG,LTCYAN$$"; NewLine(SKY_LINES);
    "$$BG,YELLOW$$";
    NewLine(5);
    PutBibleVerse("Exodus,14:19",7);

    Preempt(OFF);
    Fs->draw_it=&DrawIt;
    GetChar;
    WinSync;

    "$$BG$$\n";
  } catch
    CatchAll;
  SettingsPop;
  CleanUp;
}

//This is so I can use this file stand alone
//or as a component of a program.
#if IncludeDepth<2
CloudScene;
#endif
