#define MT_HOSE		1
#define MT_DROPLET	2
class MyMass:MassStruct
{
  I64 type;
  F64 radius;
};

#define ST_HOSE		1
class MySpring:SpringStruct
{
  I64 type;
};

$BG,11$


$PI,"<1>",1$





$PI,"<2>",2$



$PI,"<3>",3$

$BG$

#define HOSE_RADIUS	3
#define LINK_SIZE	6
#define NOZZLE_START_Y	(GR_HEIGHT-15*FONT_HEIGHT)
#define NOZZLE_LEN	18
#define FAUCET_X	(5*HOSE_RADIUS)
#define FAUCET_Y	(GR_HEIGHT-12*FONT_HEIGHT)
#define GROUND_Y	(GR_HEIGHT-3*FONT_HEIGHT)
MyMass *faucet,*nozzle;
F64 nozzle_�;

Ode *ode=NULL;
I64 start_up_timeout;


U0 DrawIt(TaskStruct *task,GrDC *dc)
{
  BoolI8 old_suspend,first;
  F64 dx,dy,d;
  I64  x1b,y1b,x2b,y2b,
      x1a,y1a,x2a,y2a;
  MyMass   *tempm,*tempm1;
  MySpring *temps;
  P3I32 poly[4];

  if (start_up_timeout>GetTimeStamp) {
    ode->drag_v2=0.01; //Let hose settle during start-up
    ode->drag_v3=0.0001;
    dc->color=RED;
    GrPutS(dc,(GR_WIDTH-FONT_WIDTH*6)>>1,GR_HEIGHT>>1,"Squirt");
    return;
  } else {
    ode->drag_v2=0.0005;
    ode->drag_v3=0.0000025;
  }

  if (TaskValidate(task))
    old_suspend=LBts(&task->task_flags,TASKf_SUSPENDED);

  tempm=faucet;
  dc->color=BLACK;
  GrRect(dc,tempm->x+8,tempm->y,8,GROUND_Y-FAUCET_Y);
  GrSpritePlot3(dc,tempm->x,tempm->y,0,$IB,"<1>",1$);
  dc->color=BLACK;
  GrCircle(dc,tempm->x,tempm->y,tempm->radius);
  dc->color=GREEN;
  GrFloodFill(dc,tempm->x,tempm->y);

  tempm=nozzle;
  tempm1=nozzle->last;
  dx=tempm->x-tempm1->x;
  dy=tempm->y-tempm1->y;
  nozzle_�=Unwrap(Arg(dx,dy));
  GrSpritePlotRotZ3b(dc,tempm->x,tempm->y,0,$IB,"<2>",2$,nozzle_�);
  dc->color=BLACK;
  GrCircle(dc,tempm->x,tempm->y,tempm->radius);
  dc->color=GREEN;
  GrFloodFill(dc,tempm->x,tempm->y);

  first=TRUE;
  tempm=ode->next_mass;
  while (tempm!=&ode->next_mass) {
    if (tempm->type==MT_HOSE) {
      tempm1=tempm->last;
      dx=tempm->x-tempm1->x;
      dy=tempm->y-tempm1->y;
      d=HOSE_RADIUS/Max(Sqrt(dx*dx+dy*dy),0.001);
      dx*=d;
      dy*=d;
      x2a=tempm->x-dy;
      y2a=tempm->y+dx;
      x2b=tempm->x+dy;
      y2b=tempm->y-dx;

      if (first)
	first=FALSE;
      else {
	dc->color=GREEN;
	poly[0].x=x1a;
	poly[0].y=y1a;
	poly[0].z=0;
	poly[1].x=x2a;
	poly[1].y=y2a;
	poly[1].z=0;
	poly[2].x=x2b;
	poly[2].y=y2b;
	poly[2].z=0;
	poly[3].x=x1b;
	poly[3].y=y1b;
	poly[3].z=0;
	GrFillPoly3(dc,4,poly);
      }

      //Fill gaps
      GrLine(dc,x2a,y2a,x2b,y2b);

      x1a=x2a;
      y1a=y2a;
      x1b=x2b;
      y1b=y2b;
    } else if (tempm->type==MT_DROPLET)
      GrSpritePlot3(dc,tempm->x,tempm->y,0,$IB,"<3>",3$);
    tempm=tempm->next;
  }

  temps=ode->next_spring;
  while (temps!=&ode->next_spring) {
    if (temps->type==ST_HOSE) {
      dx=temps->end1->x-temps->end2->x;
      dy=temps->end1->y-temps->end2->y;
      d=HOSE_RADIUS/Max(Sqrt(dx*dx+dy*dy),0.001);
      dx*=d;
      dy*=d;
      dc->color=BLACK;
      GrLine(dc,temps->end1->x-dy,temps->end1->y+dx,
	  temps->end2->x-dy,temps->end2->y+dx);
      GrLine(dc,temps->end1->x+dy,temps->end1->y-dx,
	  temps->end2->x+dy,temps->end2->y-dx);
    }
    temps=temps->next;
  }

  if (TaskValidate(task))
    LBEqu(&task->task_flags,TASKf_SUSPENDED,old_suspend);
}


U0 MyDerivative(Ode *ode,F64 t,Order2D3 *state,Order2D3 *DstateDt)
{
//The forces due to springs and drag are
//automatically handled by the
//ode code.We can add new forces
//here.
  nounusedwarn t,state,DstateDt;
//  F64 d,dd;
//  D3 p;
  MyMass *tempm1,*tempm2;
  nounusedwarn tempm2;

  tempm1=ode->next_mass;
  while (tempm1!=&ode->next_mass) {
    if (tempm1->type==MT_HOSE) {
      if (tempm1->state->y+tempm1->radius>GROUND_Y)
	tempm1->DstateDt->DyDt-=Sqr(Sqr(tempm1->state->y+tempm1->radius-GROUND_Y))*tempm1->mass;
      else
	tempm1->DstateDt->DyDt+=500*tempm1->mass;
      if (tempm1==nozzle || tempm1==faucet) {
	tempm1->DstateDt->DxDt=0;
	tempm1->DstateDt->DyDt=0;
      }
    } else if (tempm1->type==MT_DROPLET)
      tempm1->DstateDt->DyDt=500*tempm1->mass;
    tempm1=tempm1->next;
  }
}

MyMass *PlaceMass(I64 type,I64 x, I64 y,F64 r,F64 dx,F64 dy,F64 mass,TaskStruct *mem_task)
{
  MyMass *tempm=CAlloc(sizeof(MyMass),mem_task);
  tempm->type=type;
  tempm->mass=mass;
  tempm->drag_profile_factor=250.0;
  tempm->x=x;
  tempm->y=y;
  tempm->DxDt=dx;
  tempm->DyDt=dy;
  tempm->radius=r;
  InsQue(tempm,ode->last_mass);
  return tempm;
}

MySpring PlaceSpring(MyMass *tempm1,MyMass *tempm2)
{
  MySpring *temps=CAlloc(sizeof(MySpring));
  temps->end1=tempm1;
  temps->end2=tempm2;
  temps->constant=20000;
  InsQue(temps,ode->last_spring);
  return temps;
}

U0 HoseNew()
{
  I64 i;
  MyMass *tempm1=NULL,*tempm;
  MySpring *temps;
  for (i=FAUCET_X;i<GR_WIDTH;i+=LINK_SIZE) {
    tempm=PlaceMass(MT_HOSE,i/2,GROUND_Y-HOSE_RADIUS,HOSE_RADIUS,0,0,1.0,Fs);
    if (tempm1) {
      temps=PlaceSpring(tempm,tempm1);
      temps->rest_len=LINK_SIZE;
      temps->type=ST_HOSE;
      nozzle=tempm;
    } else
      faucet=tempm;
    tempm1=tempm;
  }
  faucet->y=FAUCET_Y;
  nozzle->y=NOZZLE_START_Y;
  nozzle_�=0;
}

U0 AnimateTask(U64 dummy=0)
{
  nounusedwarn dummy;
//Preempt is off by default for new task
  MyMass   *tempm,*tempm1;
  F64 dx,dy;
  while (TRUE) {

//The WinMgr task suspends our parent when it
//updates the ODE's.We use this to know when
//the ODE's are being updated.
    if (!Bt(&Fs->parent_task->task_flags,TASKf_SUSPENDED)) {
      dx=Cos(nozzle_�),
      dy=Sin(nozzle_�),
      PlaceMass(MT_DROPLET,
		nozzle->x+NOZZLE_LEN*dx,nozzle->y+NOZZLE_LEN*dy,
		HOSE_RADIUS,
		500*dx,500*dy,
		100.0,Fs->parent_task);
      if (Rand<0.05) //faucet drip
	PlaceMass(MT_DROPLET,
		faucet->x,faucet->y,
		HOSE_RADIUS,
		0,0,
		100.0,Fs->parent_task);

      tempm=ode->next_mass;
      while (tempm!=&ode->next_mass) {
	tempm1=tempm->next;
	if (tempm->type==MT_DROPLET &&
	    tempm->y+tempm->radius>GROUND_Y) {
	  RemQue(tempm);
	  Free(tempm);
	}
	tempm=tempm1;
      }
    }
    WinSync;
  }
}

#define NOZZLE_MOVE_STEPS	5
#define NOZZLE_MOVE		15.0
U0 MoveNozzleTaskX(I64 sign)
{
  I64 i;
  for (i=0;i<NOZZLE_MOVE_STEPS;i++) {
    nozzle->x=Limit(nozzle->x+
      sign*NOZZLE_MOVE/NOZZLE_MOVE_STEPS,
      HOSE_RADIUS*3,GR_WIDTH-HOSE_RADIUS*3);
    WinSync;
  }
}

U0 MoveNozzleTaskY(I64 sign)
{
  I64 i;
  for (i=0;i<NOZZLE_MOVE_STEPS;i++) {
    nozzle->y=Limit(nozzle->y+
      sign*NOZZLE_MOVE/NOZZLE_MOVE_STEPS,
      HOSE_RADIUS*3,GROUND_Y);
    WinSync;
  }
}


U0 DrawBackGroundTextLayer()
{
  LtfClear;
  "$$BG,LTCYAN$$";
  NewLine(GROUND_Y/FONT_HEIGHT);
  "$$BG,YELLOW$$";
}

U0 Init()
{
  DrawBackGroundTextLayer;

//Allow hose to settle.
  start_up_timeout=GetTimeStamp+time_stamp_freq>>1;

  ode=OdeNew(0,5e-2,ODEF_HAS_MASSES);
  ode->derivative=&MyDerivative;
  ode->acceleration_limit=5e3;

  HoseNew;
  InsQue(ode,Fs->last_ode);
}

U0 CleanUp()
{
  MyMass   *tempm,*tempm1;
  MySpring *temps,*temps1;
  I64 i;

  for (i=0;i<NOZZLE_MOVE_STEPS;i++)
    WinSync;	//Let nozzle move tasks die

  RemQue(ode);

  tempm=ode->next_mass;
  while (tempm!=&ode->next_mass) {
    tempm1=tempm->next;
    RemQue(tempm);
    Free(tempm);
    tempm=tempm1;
  }

  temps=ode->next_spring;
  while (temps!=&ode->next_spring) {
    temps1=temps->next;
    RemQue(temps);
    Free(temps);
    temps=temps1;
  }

  OdeDel(ode);
  LtfClear;
  "$$BG$$";
}

U0 SongTask(U64 dummy=0)
{
  nounusedwarn dummy;
  Fs->task_end_cb=&SndTaskEndCB;
  MusicSettingsRst;
  while (TRUE) {
    Play("W13eAM4/4");
    Play("2qGeGqG3eAq.BqBeBqA2eG3qA");
    Play("eB2hG3q.BqBeCq.DD");
    Play("qCeBqCeDhB");
  }
}

U0 Squirt()
{
  U64 sc;
  SettingsPush; //See $LK,"SettingsPush","MN:SettingsPush"$
  Fs->song_task=Spawn(&SongTask,NULL,"Song",Fs);
  WinMax;
  WinBorder;
  Preempt(OFF);
  LtfCursor;

  Fs->win_inhibit|=WIF_DBL_CLICK;
  WordStat;

  MenuPush(
  "File {"
  "  Abort(,CH_SHIFT_ESC);"
  "  Exit(,CH_ESC);"
  "}"
  "Play {"
  "  Restart(,CH_NEW_LINE);"
  "  Left(,,SC_CURSOR_LEFT);"
  "  Right(,,SC_CURSOR_RIGHT);"
  "  Up(,,SC_CURSOR_UP);"
  "  Down(,,SC_CURSOR_DOWN);"
      "}"
      );

  Init;
  Fs->animate_task=Spawn(&AnimateTask,NULL,"Animate",Fs);
  Fs->draw_it=&DrawIt;

  try {
    while (TRUE) {
      switch (GetKey(&sc)) {
	case 0:
	  switch (sc.u8[0]) {
	    case SC_CURSOR_LEFT:
	      Spawn(&MoveNozzleTaskX,-1,"Move Nozzle",Fs);
	      break;
	    case SC_CURSOR_RIGHT:
	      Spawn(&MoveNozzleTaskX,1,"Move Nozzle",Fs);
	      break;
	    case SC_CURSOR_UP:
	      Spawn(&MoveNozzleTaskY,-1,"Move Nozzle",Fs);
	      break;
	    case SC_CURSOR_DOWN:
	      Spawn(&MoveNozzleTaskY,1,"Move Nozzle",Fs);
	      break;
	  }
	  break;
	case CH_NEW_LINE:
	  CleanUp;
	  Init;
	  break;
	case CH_SHIFT_ESC:
	case CH_ESC:
	  goto sq_done;
      }
    }
sq_done: //Don't goto out of try
  } catch
    CatchAll;
  SettingsPop;
  CleanUp;
  MenuPop;
}

Squirt;
           R           �������������������������������������   ������   ��������������������   ���        ���������    �����   ��               ���   �                        ��   ���         �       �   �����     ����            ��   ������� ������   ��        ��   ������������   � �������   �����������   ��������   ����������   ������   ��������������   �������������   �����������   ���������   ���� ����   ��������   ���������   ����������   ����������   ���������� ����   ������������ ����   ����������������   ������������������   ������������������������   �����������������������������              2        	   ���������������������������������       ��      �����������������       ����    ���������       ��������  ��       ����� ����� ��       ���� � ��       ��               ��       � � ����������������� ��       �������������������������                  J            �����������				��	�	��			�				�������������� 