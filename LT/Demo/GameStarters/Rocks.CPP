$BG,0$
$PI,"<ship_null>",1$


$PI,"<ship_thrust>",2$



$PI,"<ship_null2>",3$


$PI,"<ship_thrust2>",4$



$PI,"<tiny_rock>",5$


$PI,"<tiny_rock2>",6$


$PI,"<small_rock>",7$


$PI,"<small_rock2>",8$



$PI,"<medium_rock>",9$


$PI,"<medium_rock2>",10$




$PI,"<large_rock>",11$


$PI,"<large_rock2>",12$


$PI,"<Bullet>",13$




$PI,"<Cargo>",14$


$BG$


#define CMD_NULL	0
#define CMD_SPIN_LEFT	1
#define CMD_SPIN_RIGHT	2
#define CMD_THRUST	3
#define CMD_FIRE	4
#define CMD_GAME_OVER	5
I64 cmd_code=CMD_NULL;

#define NUM_STARS	100
I64 stars_x[NUM_STARS],stars_y[NUM_STARS];

BoolI8 bounce;

#define OT_SHIP		1
#define OT_CARGO	2
#define OT_BULLET	3
#define OT_TINY_ROCK	4
#define OT_SMALL_ROCK	5
#define OT_MEDIUM_ROCK	6
#define OT_LARGE_ROCK	7

#define A_TRANSITION	-1
#define A_INACTIVE	0
#define A_ACTIVE	1

class Order2D2�
{
  F64 x,y,�;
  F64 DxDt,DyDt,D�Dt;
};

class Object
{
  I32 active;
  I32 type;
  U8 *img;
  F64 mass,radius;
  U64 timeout;
};

#define NUM_OBJECTS	256

Ode *ode;
Order2D2� *o_state;
Object *objs;

U0 ObjectInit(I64 i, I64 type)
{
  objs[i].active=A_ACTIVE;
  objs[i].timeout=0;
  objs[i].type=type;
  switch (type) {
    case OT_SHIP:
      if (bounce)
	objs[i].img=$IB,"<ship_null>",1$;
      else
	objs[i].img=$IB,"<ship_null>",3$;
      objs[i].radius=15;
      break;
    case OT_CARGO:
      objs[i].img=$IB,"<cargo>",14$;
      objs[i].radius=30;
      break;
    case OT_BULLET:
      objs[i].img=$IB,"<bullet>",15$;
      objs[i].radius=7;
      break;
    case OT_TINY_ROCK:
      if (i&1)
	objs[i].img=$IB,"<tiny_rock>",5$;
      else
	objs[i].img=$IB,"<tiny_rock2>",6$;
      objs[i].radius=5;
      break;
    case OT_SMALL_ROCK:
      if (i&1)
	objs[i].img=$IB,"<small_rock>",16$;
      else
	objs[i].img=$IB,"<small_rock2>",8$;
      objs[i].radius=11;
      break;
    case OT_MEDIUM_ROCK:
      if (i&1)
	objs[i].img=$IB,"<medium_rock>",17$;
      else
	objs[i].img=$IB,"<medium_rock2>",10$;
      objs[i].radius=20;
      break;
    case OT_LARGE_ROCK:
      if (i&1)
	objs[i].img=$IB,"<large_rock>",18$;
      else
	objs[i].img=$IB,"<large_rock2>",12$;
      objs[i].radius=26;
      break;
  }
  objs[i].mass=objs[i].radius*objs[i].radius*objs[i].radius;
}

I64 ObjectAlloc(I64 type)
{
  I64 i;
  for (i=0;i<NUM_OBJECTS-1;i++)
    if (objs[i].active==A_INACTIVE)
      break;
  ObjectInit(i,type);
  return i;
}

U0 DrawIt(TaskStruct *task,GrDC *dc)
{
  nounusedwarn task;
  I64 i,x,y;

  task->text_attr=BLACK<<4+WHITE;

  dc->color=WHITE;
  for (i=0;i<NUM_STARS;i++)
    GrPlot(dc,stars_x[i],stars_y[i]);

  for (i=0;i<NUM_OBJECTS;i++) {
    if (objs[i].active==A_ACTIVE) {
      if (objs[i].timeout && objs[i].timeout<GetTimeStamp)
	objs[i].active=A_INACTIVE;
      else {
	x=o_state[i].x;
	y=o_state[i].y;

	if (x<0) {
	  x=GR_WIDTH-AbsI64(x)%GR_WIDTH;
	  o_state[i].x=x;
	} else if (x>GR_WIDTH) {
	  x%=GR_WIDTH;
	  o_state[i].x=x;
	}
	if (y<0) {
	  y=GR_HEIGHT-AbsI64(y)%GR_HEIGHT;
	  o_state[i].y=y;
	} else if (y>GR_HEIGHT) {
	  y%=GR_HEIGHT;
	  o_state[i].y=y;
	}
	switch (objs[i].type) {
	  case OT_SHIP:
	    if (bounce) {
	      if (cmd_code==CMD_THRUST && GetTimeStamp&8192)
		GrSpritePlotRotZ3b(dc,x,y,0,$IB,"<ship_thrust>",2$,o_state[i].�);
	      else
		GrSpritePlotRotZ3b(dc,x,y,0,$IB,"<ship_null>",1$,o_state[i].�);
	    } else {
	      if (cmd_code==CMD_THRUST && GetTimeStamp&8192)
		GrSpritePlotRotZ3b(dc,x,y,0,$IB,"<ship_thrust>",4$,o_state[i].�);
	      else
		GrSpritePlotRotZ3b(dc,x,y,0,$IB,"<ship_null>",3$,o_state[i].�);
	    }
	    break;
	  case OT_BULLET:
	  case OT_TINY_ROCK:
	  case OT_SMALL_ROCK:
	  case OT_MEDIUM_ROCK:
	  case OT_LARGE_ROCK:
	  case OT_CARGO:
	    GrSpritePlotRotZ3b(dc,x,y,0,objs[i].img,o_state[i].�);
	    break;
	}
      }
    }
  }
  dc->color=WHITE;
  GrPutS(dc,0,0,"Escort the cargo ship.");
}

U0 Init()
{
  I64 i,j;

  cmd_code=CMD_NULL;

  for (i=0;i<NUM_STARS;i++) {
    stars_x[i]=RandU16%GR_WIDTH;
    stars_y[i]=RandU16%GR_HEIGHT;
  }

  MemSet(objs,0,sizeof(Object)*NUM_OBJECTS);
  MemSet(o_state,0,sizeof(Order2D2�)*NUM_OBJECTS);

  i=ObjectAlloc(OT_SHIP); //always #0
  objs[i].mass=2.0*objs[i].radius*objs[i].radius*objs[i].radius;
  o_state[i].x=GR_WIDTH/2;
  o_state[i].y=GR_HEIGHT/2;

  i=ObjectAlloc(OT_CARGO); //always #1
  objs[i].mass=0.5*objs[i].radius*objs[i].radius*objs[i].radius;
  o_state[i].x=0;
  o_state[i].DxDt=16.0;
  o_state[i].y=GR_HEIGHT/2;
  o_state[i].�=�/2;

  for (j=0;j<4;j++) {
    i=ObjectAlloc(OT_LARGE_ROCK);
    o_state[i].x=RandU32%GR_WIDTH;
    o_state[i].y=RandU32%GR_HEIGHT;
    o_state[i].DxDt=RandI16/2000.0;
    o_state[i].DyDt=RandI16/2000.0;
    o_state[i].D�Dt=RandI16/25000.0;
    o_state[i].�=2*�*RandI16/16384.0;
  }
}

U0 MyDerivative(Ode *ode,F64 t,Order2D2� *state,Order2D2� *DstateDt)
{
  I64 i,j;
  F64 f,d,d1,dd,dx,dy;
  nounusedwarn ode,t;

  MemSet(DstateDt,0,sizeof(Order2D2�)*NUM_OBJECTS);
  for (i=0;i<NUM_OBJECTS;i++) {
    if (objs[i].active==A_ACTIVE) {
      DstateDt[i].x=state[i].DxDt;
      DstateDt[i].y=state[i].DyDt;
      DstateDt[i].�=state[i].D�Dt;
      for (j=0;j<NUM_OBJECTS;j++) {
	if (i!=j && objs[j].active==A_ACTIVE) {
	  dx=state[i].x-state[j].x;
	  dy=state[i].y-state[j].y;
	  dd=Sqr(dx)+Sqr(dy);
	  d=Sqrt(dd);

	  if (d) {
	    dx/=d;
	    dy/=d;
	  } else {
	    dx=0;
	    dy=0;
	  }

	  d1=objs[i].radius+objs[j].radius;
	  f=0;
	  if (dd<Sqr(d1)) {
	    if (bounce && objs[i].type!=OT_CARGO && objs[j].type!=OT_CARGO) {
	      f=Sqr(Sqr(Sqr(d1-Abs(d)+0.1))); //spring force
	      if (f>1000000.0) f=1000000.0;
	    }
	  } else
	    f=-2.5*objs[i].mass*objs[j].mass/(dd+1.0);

	  DstateDt[i].DxDt+=f*dx/objs[i].mass;
	  DstateDt[i].DyDt+=f*dy/objs[i].mass;
	}
      }
    }
  }
  switch (cmd_code) {
    case CMD_SPIN_LEFT:
      DstateDt[0].D�Dt-=10.0;
      break;
    case CMD_SPIN_RIGHT:
      DstateDt[0].D�Dt+=10.0;
      break;
    case CMD_THRUST:
      DstateDt[0].DxDt+=100.0*Cos(state[0].�-�/2.0);
      DstateDt[0].DyDt+=100.0*Sin(state[0].�-�/2.0);
      break;
  }

  DstateDt[1].DxDt+=2.0*Cos(state[1].�-�/2.0);
  DstateDt[1].DyDt+=2.0*Sin(state[1].�-�/2.0);
}

U0 Collision(I64 i,I64 j)
{
  I64 i2;
  switch (objs[i].type) {
    case OT_SHIP:
      if (!bounce) {
	cmd_code=CMD_GAME_OVER;
	objs[i].active=A_INACTIVE;
      }
      break;
    case OT_CARGO:
      cmd_code=CMD_GAME_OVER;
      objs[i].active=A_INACTIVE;
      break;
    case OT_TINY_ROCK:
      if (objs[j].type==OT_BULLET && !bounce)
	objs[i].active=A_INACTIVE;
      break;
    case OT_BULLET:
      if (!bounce)
	objs[i].active=A_INACTIVE;
      break;
    case OT_SMALL_ROCK:
    case OT_MEDIUM_ROCK:
      i2=ObjectAlloc(objs[i].type-1);
      objs[i2].active=A_TRANSITION;
      o_state[i2].x=o_state[i].x+objs[i2].radius+2.0;
      o_state[i2].y=o_state[i].y;
      o_state[i2].�=o_state[i].�;
      o_state[i2].DxDt=o_state[i].DxDt;
      o_state[i2].DyDt=o_state[i].DyDt;
      o_state[i2].D�Dt=o_state[i].D�Dt;

      i2=ObjectAlloc(objs[i].type-1);
      objs[i2].active=A_TRANSITION;
      o_state[i2].x=o_state[i].x-objs[i2].radius-2.0;
      o_state[i2].y=o_state[i].y;
      o_state[i2].�=o_state[i].�;
      o_state[i2].DxDt=o_state[i].DxDt;
      o_state[i2].DyDt=o_state[i].DyDt;
      o_state[i2].D�Dt=o_state[i].D�Dt;

      objs[i].active=A_INACTIVE;
      break;
    case OT_LARGE_ROCK:
      i2=ObjectAlloc(objs[i].type-1);
      objs[i2].active=A_TRANSITION;
      o_state[i2].x=o_state[i].x+objs[i2].radius+4.0;
      o_state[i2].y=o_state[i].y+objs[i2].radius+4.0;
      o_state[i2].�=o_state[i].�;
      o_state[i2].DxDt=o_state[i].DxDt;
      o_state[i2].DyDt=o_state[i].DyDt;
      o_state[i2].D�Dt=o_state[i].D�Dt;

      i2=ObjectAlloc(objs[i].type-1);
      objs[i2].active=A_TRANSITION;
      o_state[i2].x=o_state[i].x-objs[i2].radius-4.0;
      o_state[i2].y=o_state[i].y-objs[i2].radius-4.0;
      o_state[i2].�=o_state[i].�;
      o_state[i2].DxDt=o_state[i].DxDt;
      o_state[i2].DyDt=o_state[i].DyDt;
      o_state[i2].D�Dt=o_state[i].D�Dt;

      i2=ObjectAlloc(objs[i].type-1);
      objs[i2].active=A_TRANSITION;
      o_state[i2].x=o_state[i].x-objs[i2].radius-4.0;
      o_state[i2].y=o_state[i].y+objs[i2].radius+4.0;
      o_state[i2].�=o_state[i].�;
      o_state[i2].DxDt=o_state[i].DxDt;
      o_state[i2].DyDt=o_state[i].DyDt;
      o_state[i2].D�Dt=o_state[i].D�Dt;

      i2=ObjectAlloc(objs[i].type-1);
      objs[i2].active=A_TRANSITION;
      o_state[i2].x=o_state[i].x+objs[i2].radius+4.0;
      o_state[i2].y=o_state[i].y-objs[i2].radius-4.0;
      o_state[i2].�=o_state[i].�;
      o_state[i2].DxDt=o_state[i].DxDt;
      o_state[i2].DyDt=o_state[i].DyDt;
      o_state[i2].D�Dt=o_state[i].D�Dt;

      objs[i].active=A_INACTIVE;
      break;
  }

  switch (objs[j].type) {
    case OT_SHIP:
      if (!bounce) {
	cmd_code=CMD_GAME_OVER;
	objs[j].active=A_INACTIVE;
      }
      break;
    case OT_CARGO:
      cmd_code=CMD_GAME_OVER;
      objs[j].active=A_INACTIVE;
      break;
    case OT_TINY_ROCK:
      if (objs[i].type==OT_BULLET && !bounce)
	objs[j].active=A_INACTIVE;
      break;
    case OT_BULLET:
      if (!bounce)
	objs[j].active=A_INACTIVE;
      break;
    case OT_SMALL_ROCK:
    case OT_MEDIUM_ROCK:
      i2=ObjectAlloc(objs[j].type-1);
      objs[i2].active=A_TRANSITION;
      o_state[i2].x=o_state[j].x+objs[i2].radius+2.0;
      o_state[i2].y=o_state[j].y;
      o_state[i2].�=o_state[j].�;
      o_state[i2].DxDt=o_state[j].DxDt;
      o_state[i2].DyDt=o_state[j].DyDt;
      o_state[i2].D�Dt=o_state[j].D�Dt;

      i2=ObjectAlloc(objs[j].type-1);
      objs[i2].active=A_TRANSITION;
      o_state[i2].x=o_state[j].x-objs[i2].radius-2.0;
      o_state[i2].y=o_state[j].y;
      o_state[i2].�=o_state[j].�;
      o_state[i2].DxDt=o_state[j].DxDt;
      o_state[i2].DyDt=o_state[j].DyDt;
      o_state[i2].D�Dt=o_state[j].D�Dt;

      objs[j].active=A_INACTIVE;
      break;

    case OT_LARGE_ROCK:
      i2=ObjectAlloc(objs[j].type-1);
      objs[i2].active=A_TRANSITION;
      o_state[i2].x=o_state[j].x+objs[i2].radius+4.0;
      o_state[i2].y=o_state[j].y+objs[i2].radius+4.0;
      o_state[i2].�=o_state[j].�;
      o_state[i2].DxDt=o_state[j].DxDt;
      o_state[i2].DyDt=o_state[j].DyDt;
      o_state[i2].D�Dt=o_state[j].D�Dt;

      i2=ObjectAlloc(objs[j].type-1);
      objs[i2].active=A_TRANSITION;
      o_state[i2].x=o_state[j].x-objs[i2].radius-4.0;
      o_state[i2].y=o_state[j].y-objs[i2].radius-4.0;
      o_state[i2].�=o_state[j].�;
      o_state[i2].DxDt=o_state[j].DxDt;
      o_state[i2].DyDt=o_state[j].DyDt;
      o_state[i2].D�Dt=o_state[j].D�Dt;

      i2=ObjectAlloc(objs[j].type-1);
      objs[i2].active=A_TRANSITION;
      o_state[i2].x=o_state[j].x-objs[i2].radius-4.0;
      o_state[i2].y=o_state[j].y+objs[i2].radius+4.0;
      o_state[i2].�=o_state[j].�;
      o_state[i2].DxDt=o_state[j].DxDt;
      o_state[i2].DyDt=o_state[j].DyDt;
      o_state[i2].D�Dt=o_state[j].D�Dt;

      i2=ObjectAlloc(objs[j].type-1);
      objs[i2].active=A_TRANSITION;
      o_state[i2].x=o_state[j].x+objs[i2].radius+4.0;
      o_state[i2].y=o_state[j].y-objs[i2].radius-4.0;
      o_state[i2].�=o_state[j].�;
      o_state[i2].DxDt=o_state[j].DxDt;
      o_state[i2].DyDt=o_state[j].DyDt;
      o_state[i2].D�Dt=o_state[j].D�Dt;

      objs[j].active=A_INACTIVE;
      break;
  }
}

U0 CheckCollisions()
{
  I64 i,j;
  F64 d,d1,dd,dx,dy;
  GrDC *b;
  for (i=0;i<NUM_OBJECTS;i++) {
    if (objs[i].active==A_ACTIVE) {
      for (j=i+1;j<NUM_OBJECTS;j++) {
	if (objs[j].active==A_ACTIVE) {
	  dx=o_state[i].x-o_state[j].x;
	  dy=o_state[i].y-o_state[j].y;
	  dd=Sqr(dx)+Sqr(dy);
	  d=Sqrt(dd);

	  if (d) {
	    dx/=d;
	    dy/=d;
	  } else {
	    dx=0;
	    dy=0;
	  }

	  d1=objs[i].radius+objs[j].radius;
	  if (dd<Sqr(d1)) {
	    b=GrDCNew(DCT_COLOR8,64,64,Fs);
	    GrSpritePlotRotZ3b(b,32,32,0,objs[i].img,o_state[i].�);
	    b->color=ROP_COLLISION;
	    b->bkcolor=BLACK;
	    GrSpritePlotRotZ3b(b,o_state[j].x-o_state[i].x+32,
			       o_state[j].y-o_state[i].y+32,0,
			       objs[j].img,o_state[j].�);
	    if (b->collision_cnt>0)
	      Collision(i,j);
	    GrDCDel(b);
	  }
	}
      }
    }
  }
  for (i=0;i<NUM_OBJECTS;i++)
    if (objs[i].active==A_TRANSITION)
      objs[i].active=A_ACTIVE;
}

#define MUZZLE_VELOCITY 500.0

U0 Fire()
{
  F64 �=o_state[0].�;
  I64 i=ObjectAlloc(OT_BULLET);
  objs[i].timeout=GetTimeStamp+time_stamp_freq/2;
  objs[i].mass*=10.0;
  o_state[i].x=o_state[0].x+(objs[0].radius+objs[i].radius+1.0)*Cos(�-�/2.0);
  o_state[i].y=o_state[0].y+(objs[0].radius+objs[i].radius+1.0)*Sin(�-�/2.0);
  o_state[i].DxDt=o_state[0].DxDt+MUZZLE_VELOCITY*Cos(�-�/2.0);
  o_state[i].DyDt=o_state[0].DyDt+MUZZLE_VELOCITY*Sin(�-�/2.0);
  o_state[i].D�Dt=RandI16/3000.0;

//not real physics with the 0.005 factor
  o_state[0].DxDt-=0.005*MUZZLE_VELOCITY*Cos(�-�/2.0)*objs[i].mass/objs[0].mass;
  o_state[0].DyDt-=0.005*MUZZLE_VELOCITY*Sin(�-�/2.0)*objs[i].mass/objs[0].mass;
}

U0 CheckCrazy()
{
  I64 i;
  F64 d;
  for (i=0;i<NUM_OBJECTS;i++)
    if (objs[i].active==A_ACTIVE) {
      d=Sqrt(Sqr(o_state[i].DxDt)+Sqr(o_state[i].DyDt));
      if (d>MUZZLE_VELOCITY*5.0)
	Auto(" ");
    }
}

U0 SongTask(U64 dummy=0)
{ //Song by Terry A. Davis
  nounusedwarn dummy;
  Fs->task_end_cb=&SndTaskEndCB;
  MusicSettingsRst;
  while (TRUE) {
    Play("W13eGGsGGeGGGe.GeCDsEDeCBAe.AeG");
    Play("GsGGeGGGe.GeFEsDEeDCBe.AeFD");
    Play("EDFDEDECDCECDCsGGeG");
    Play("GG2hG3sGGeGGG2hG");
  }
}

U0 Rocks()
{
  U64 ch,sc,msg_code;
  MenuEntry *tempse;

  PopUpOk(
	"I refuse to rip-off the original\n"
	"so this is intentionally unplayable\n"
	"and included for demonstration\n"
	"purposes.\n\n"
	"Write games, don't play them.\n");

  SettingsPush; //See $LK,"SettingsPush","MN:SettingsPush"$
  Fs->song_task=Spawn(&SongTask,NULL,"Song",Fs);
  Preempt(OFF),
  WinBorder;
  WordStat;

  MenuPush(
"File {"
"  Abort(,CH_SHIFT_ESC);"
"  Exit(,CH_ESC);"
"}"
"Play {"
"  Restart(,CH_NEW_LINE);"
"  Fire(,CH_SPACE);"
"  Fwd(,,SC_CURSOR_UP);"
"  Left(,,SC_CURSOR_LEFT);"
"  Right(,,SC_CURSOR_RIGHT);"
"  ToggleBounce(,'b');"
"}"
);
  tempse=MenuEntryFind(Fs->cur_menu,"Play/ToggleBounce");
  tempse->checked=bounce=TRUE;

  ode=OdeNew(NUM_OBJECTS*6,0.01);
  ode->derivative=&MyDerivative;
  ode->min_tolerance=0.001*ode->max_tolerance;
  o_state=ode->state;
  objs=MAlloc(NUM_OBJECTS*sizeof(Object));

  WinMax;

  Init;
  InsQue(ode,Fs->last_ode);
  LtfCursor;
  LtfClear;
  Fs->draw_it=&DrawIt;

  try {
    while (TRUE) {
      while (msg_code=ScanMsg(&ch,&sc,1<<MSG_KEY_DOWN|1<<MSG_KEY_UP)) {
	if (msg_code==MSG_KEY_DOWN)
	  switch (ch) {
	    case 0:
	      switch (sc.u8[0]) {
		case SC_CURSOR_RIGHT:
		  cmd_code=CMD_SPIN_RIGHT;
		  break;
		case SC_CURSOR_LEFT:
		  cmd_code=CMD_SPIN_LEFT;
		  break;
		case SC_CURSOR_UP:
		  cmd_code=CMD_THRUST;
		  break;
	      }
	      break;
	    case CH_SPACE:
	      Fire;
	      break;
	    case CH_NEW_LINE:
	      Init;
	      break;
	    case CH_ESC:
	    case CH_SHIFT_ESC:
	      goto rk_done;
	    case 'b':
	      tempse=MenuEntryFind(Fs->cur_menu,"Play/ToggleBounce");
	      tempse->checked=bounce=!bounce;
	      break;
	  }
	else //MSG_KEY_UP
	  cmd_code=CMD_NULL;
      }
      CheckCollisions;
      CheckCrazy;
      if (cmd_code==CMD_GAME_OVER) {
	Sleep(1000);
	Init;
      }
      WinSync; //msgs are only queued by winmngr
    }
rk_done:
    GetMsg(,,1<<MSG_KEY_UP);
  } catch
    CatchAll;

  RemQue(ode);
  OdeDel(ode);
  Free(objs);
  MenuPop;
 
  SettingsPop;
}

Rocks;
           �           ��������   ����                  ��������   ����   ����                        ����   ����   ����                                          ����   ��������   ����   ����            ����          ����      	      	      	   
      
      	      	         ����   ����    ����    ��������   ����   ����   ����   ����	   ����	      	                         ��������   ����                  ��������   ����   ����                        ����   ����   ����                                          ����   ��������   ����   ����            ����          ����   ����   ����
   ����
                  
      
                                                   	             ����
      
      
   
      
         ����   ��������   ����   ����	   ����	       	              3            ���������l�$�	n[�   ����   d�     	       �         g   ��������n[϶m�$C�$�$Q�eۼ�ar�ͳ�ٞW�"C���m۲      �����m�6M�+   ����   �m    
   ���������i�   
      I�           G        r   ����������y^�ڳm�$�۶m۳g��FCQ��d["��(	�l��l��n         $I�T�$�   ����   �=�$�       �����]��m��IFI�$I��IrY�ms�z��m�k�&M۶m��%��l�e[�$�Hҷm�<I�$I�$I��m۶m۶m[D��   ���������Mߧm{      ����I�ɶm��y�   ����   v]ף�,K�$�W       	   $��Q�I�K��2۴MoK�dɶm�=�3O��=   ����������u�           �      ��������                   ����������������    ������������      ������������    ��������������������      ����   ��������    ����      ��������    ������������   ������������
        ������������               �      ��������                   ����������������    ������������      ������������    ��������������������      ����   ��������    ����      ��������    ������������   ������������
        ������������               3            ���������l�$�	n[�   ����   d�            �         g   ��������n[϶m�$C�$�$Q�eۼ�ar�ͳ�ٞW�"C���m۲      �����m�6M�+   ����   �m    
   ���������i�   
      I�           G        r   ����������y^�ڳm�$�۶m۳g��FCQ��d["��(	�l��l��n         $I�T�$�   ����   �=�$�       �����]��m��IFI�$I��IrY�ms�z��m�k�&M۶m��%��l�e[�$�Hҷm�<I�$I�$I��m۶m۶m[D��   ���������Mߧm{      ����I�ɶm��y�   ����   v]ף�,K�$�W       	   $��Q�I�K��2۴MoK�dɶm�=�3O��=   ����������u�           6         (   ���������lۤm�d$�m�d   ���������i�    
       k         M      ����ض��kۦϓ$�$�$� A�$$m��6��fG   ��������u���H�EQdl���-Q$�� ��l�m   ��������                   �      ����s��m�֭ϟ�m�$�$�"mY�%��(#I���mۮk�@�,˶m��$a��$m۾i������m۶m��vm[I����DG      ���������%�m۶I���zۚ���oI&IG         A�$I�$II�m���m۶՞�$
�!�+   ���������k�����m�$�0L�e[       ����I� �    ����   �]�"   	   ������Ͷm&	�����           �            ����      ����   ����               ��������   ����   ��������    ����    ����   ����            ����   ����   ����   ����   ����                                 ����
      
   ������������      ����                            	                   	   ����   ��������	   ����                                                 ����                                ��������   ����                  ��������   ����   ����                        ����   ����   ����                                          ����   ��������   ����   ����            ����          ����           �           ��������   ����                  ��������   ����   ����                        ����   ����   ����                                          ����   ��������   ����   ����            ����          ����   ����   ����
   ����
                  
      
                                                   	                     '         ��������4In]Kj   ����   	           (         ����   6�%If   ����    $I 