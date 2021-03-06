#define THRUST	100

BoolI8 blast_off;
MassStruct m1, //Bottom of rocket
	   m2; //Top of rocket
SpringStruct s;

#define PT_SPAN	40
$BG,11$


  $PI,"<1>",1$
$BG$
GrDC *dc2;


U0 DrawIt(TaskStruct *task,GrDC *dc)
{
  I64 i,x,y,cx=task->win_pixel_width/2,cy=(task->win_height-4)*FONT_HEIGHT-PT_SPAN/2;
  BoolI8 engine_on;
  F64 nozzle_angle,�=Arg(m2.x-m1.x,m2.y-m1.y);

  if (Bt(key_down_bitmap,SC_CURSOR_UP)) {
    nozzle_angle=0;
    engine_on=TRUE;
  } else if (Bt(key_down_bitmap,SC_CURSOR_LEFT)) {
    nozzle_angle=�/8;
    engine_on=TRUE;
  } else if (Bt(key_down_bitmap,SC_CURSOR_RIGHT)) {
    nozzle_angle=-�/8;
    engine_on=TRUE;
  } else
    engine_on=FALSE;

  if (engine_on) {
    x=m1.x-10*Cos(�+nozzle_angle);
    y=m1.y-10*Sin(�+nozzle_angle);
    for (i=0;i<6;i++) {
      if ((i^gr_screen_updates)&1)
	dc->color=YELLOW;
      else
	dc->color=RED;
      GrLine(dc,cx+(m1.x+i*Cos(�-�/2)),
		  cy-(m1.y+i*Sin(�-�/2)),
		  cx+x,cy-y);
      GrLine(dc,cx+(m1.x+i*Cos(�+�/2)),
		  cy-(m1.y+i*Sin(�+�/2)),
		  cx+x,cy-y);
    }

    for (i=0;i<10;i++) {
      switch (RandU16&3) {
	case 0: dc2->color=ROP_EQU+WHITE; break;
	case 1: dc2->color=ROP_EQU+LTGRAY; break;
	case 2: dc2->color=ROP_EQU+DKGRAY; break;
	case 3: dc2->color=ROP_EQU+BLACK; break;
      }
      GrPlot(dc2,cx+(x+RandU16%12-6),cy-(y+RandU16%12-6));
    }
    Snd(50);
  } else
    Snd(0);
  GrSpritePlotRotZ3b(dc,cx+(m1.x+m2.x)/2,cy-(m1.y+m2.y)/2,0,$IB,"<1>",1$,-�);
}

U0 MyDerivative(Ode *ode,F64 t,Order2D3 *state,Order2D3 *DstateDt)
{
  nounusedwarn ode,t,state,DstateDt;
  BoolI8 engine_on;
  F64 nozzle_angle,�=Arg(m2.state->x-m1.state->x,m2.state->y-m1.state->y);

  if (Bt(key_down_bitmap,SC_CURSOR_UP)) {
    nozzle_angle=0;
    engine_on=TRUE;
  } else if (Bt(key_down_bitmap,SC_CURSOR_LEFT)) {
    nozzle_angle=�/8;
    engine_on=TRUE;
  } else if (Bt(key_down_bitmap,SC_CURSOR_RIGHT)) {
    nozzle_angle=-�/8;
    engine_on=TRUE;
  } else
    engine_on=FALSE;

  if (engine_on) {
    m1.DstateDt->DxDt+=THRUST*Cos(�+nozzle_angle);
    m1.DstateDt->DyDt+=THRUST*Sin(�+nozzle_angle);
  }
  if (blast_off) {
    m1.DstateDt->DyDt-=25; //Gravity
    m2.DstateDt->DyDt-=25;
  }
}

U0 Init()
{
  blast_off=FALSE;

//We don't clear queue links.
  MemSet(&m1.start_saved_area,0,offset(MassStruct.end_saved_area)-offset(MassStruct.start_saved_area));
  m1.y=-PT_SPAN/2;

  MemSet(&m2.start_saved_area,0,offset(MassStruct.end_saved_area)-offset(MassStruct.start_saved_area));
  m2.y=PT_SPAN/2;

  MemSet(&s.start_saved_area,0,offset(SpringStruct.end_saved_area)-offset(SpringStruct.start_saved_area));
  s.end1=&m1;
  s.end2=&m2;
  s.rest_len=PT_SPAN;
  s.constant=10000;

  GrDCFill;
}

U0 TaskEndCB()
{
  GrDCFill;
  SndTaskEndCB;
}

U0 Rocket()
{
  Ode *ode=OdeNew(0,1e-2,ODEF_HAS_MASSES);

  SettingsPush; //See $LK,"SettingsPush","MN:SettingsPush"$
  LtfCursor;
  WordStat;

//The ode nodes are traversed by win mgr.
  Preempt(OFF);

  MenuPush(
"File {"
"  Abort(,CH_SHIFT_ESC);"
"  Exit(,CH_ESC);"
"}"
"Play {"
"  Restart(,CH_NEW_LINE);"
"  Up(,,SC_CURSOR_UP);"
"  UpLeft(,,SC_CURSOR_LEFT);"
"  UpRight(,,SC_CURSOR_RIGHT);"
"}"
);

  WinMax;
  dc2=GrDCAlias;
  Fs->task_end_cb=&TaskEndCB;

  "$$CL$$$$BG,LTCYAN$$$$FG,GREEN$$Up, Left, Right$$FG$$";
  NewLine(Fs->win_height-4);
  "$$BG,YELLOW$$\n";
 
  ode->derivative=&MyDerivative;
  ode->drag_v2=0.002;
  ode->drag_v3=0.00001;
  ode->acceleration_limit=5e3;

  Init;
  InsQue(&m1,ode->last_mass);
  InsQue(&m2,ode->last_mass);
  InsQue(&s,ode->last_spring);

  InsQue(ode,Fs->last_ode);

  Fs->draw_it=&DrawIt;

  try {
    GetKey;
    blast_off=TRUE;
    while (TRUE) {
      switch (GetChar(NULL,FALSE)) {
	case CH_NEW_LINE:
	  Init;
	  GetKey;
	  blast_off=TRUE;
	  break;
	case CH_ESC:
	case CH_SHIFT_ESC:
	  goto rk_done;
      }
    }
rk_done:
  } catch
    CatchAll;
  RemQue(ode);
  OdeDel(ode);
  "$$CL$$$$BG$$$$FG$$\n";
  SettingsPop;
  GrDCFill;
  GrDCDel(dc2);
  MenuPop;
}

Rocket;
           r     .      ������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������ 