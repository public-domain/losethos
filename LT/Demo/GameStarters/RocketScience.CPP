/*

If you find this interesting, you might want to check-out
SimStructure, a simulation program I wrote you can downlload.


http://www.losethos.com/files/SimStrSetup.exe
Product Key: L00-W10-M70

I did study ctrl systems for 2 years, but lost enthusiasm
when I learned everything in real-life is nonlinear.
You're supposed to linearize around points and apply
control theory.I just like to appy a PID ctrller
and fiddle intelligently with the feedback coefficients.

Real rockets have dynamics like fuel slosh to consider!

*/

#define THRUST	1000

BoolI8 blast_off,plane_hit;

MassStruct m1, //Bottom of rocket
	   m2, //Top of rocket
	   m3; //Plane
SpringStruct s;

#define PT_SPAN	40
$BG,11$


   $PI,"<1>",1$




   $PI,"<2>",2$



  $PI,"<3>",3$
 
$BG$GrDC *dc2;
Ode *ode;

#define STATE_NOZZLE_ANGLE		0
#define STATE_NOZZLE_ANGLE_VELOCITY	1
#define STATE_NUM			2

D3 target;
F64 dbg,antispin_coefficient;

U0 DrawIt(TaskStruct *task,GrDC *dc)
{
  I64 i,x,y,cx=task->win_pixel_width/2,cy=(task->win_height-4)*FONT_HEIGHT-PT_SPAN/2;
  F64 �=Arg(m2.x-m1.x,m2.y-m1.y),
     nozzle_angle=ode->state[STATE_NOZZLE_ANGLE];
  if (blast_off) {
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
  }

  if (plane_hit)
    GrSpritePlot3(dc,cx+m3.x,cy-m3.y,0,$IB,"<2>",2$);
  else
    GrSpritePlot3(dc,cx+m3.x,cy-m3.y,0,$IB,"<1>",1$);

  if (blast_off && !plane_hit) {
    dc->color=ROP_COLLISION;
    dc->bkcolor=LTCYAN;
    dc->collision_cnt=0;
    GrSpritePlotRotZ3b(dc,cx+(m1.x+m2.x)/2,cy-(m1.y+m2.y)/2,0,$IB,"<3>",3$,-�);
    if (dc->collision_cnt>100) {
      Noise(1000,500,1500);
      plane_hit=TRUE;
    } else
      Snd(50);
  } else if (!plane_hit)
    Snd(0);

  dc->color=ROP_EQU;
  GrSpritePlotRotZ3b(dc,cx+(m1.x+m2.x)/2,cy-(m1.y+m2.y)/2,0,$IB,"<3>",3$,-�);

  dc->color=RED;
  GrCircle(dc,cx+target.x,cy-target.y,5);

  dc->color=BLUE;
  GrCircle(dc,cx+m3.x,cy-m3.y,5);

  dc->color=BLACK;
  GrPrintF(dc,0,FONT_HEIGHT,"%12.6f",dbg);
}


U0 MyDerivative(Ode *ode,F64 t,F64 *state,F64 *DstateDt)
{
  nounusedwarn ode,t;
  F64 d,discriminant,v,a,�=Arg(m2.state->x-m1.state->x,m2.state->y-m1.state->y),
     D�Dt,
     collision_estimate_t,
     target_heading,target_angle_error,
     desired_nozzle_angle;
  D3 p,p_target,p_body;

  //Unit vect pointing to top of rocket from bottom.
  D3Sub(&m2.state->x,&m1.state->x,&p_body);
  D3Unit(&p_body);

  //D�Dt lets us prevent too spin.
  D�Dt=antispin_coefficient*
    (m2.state->DyDt*p_body.x-m2.state->DxDt*p_body.y-
     m1.state->DyDt*p_body.x+m1.state->DxDt*p_body.y)/PT_SPAN;

  //p_target is vect from top of rocket to plane.
  D3Sub(&m3.state->x,&m2.state->x,&p_target);


  //d=0.5at^2+vt
  d=D3Norm(&p_target);
 
  D3Copy(&p,&p_target);
  D3Unit(&p);
  v=(m2.state->DxDt*p.x+m2.state->DyDt*p.y)-
    (m3.state->DxDt*p.x+m3.state->DyDt*p.y);

  a=THRUST/(m1.mass+m2.mass);

  discriminant=v*v+4*0.5*a*d;
  if (discriminant>0)
    collision_estimate_t=(-v+Sqrt(discriminant))/a;
  else
    collision_estimate_t=0;
  dbg=collision_estimate_t;

  //Aim for projected pos of plane at time of impact.
  D3Copy(&p,&m3.state->DxDt);
  D3MulEqu(&p,collision_estimate_t);
  D3AddEqu(&p_target,&p);

  D3Copy(&target,&p_target);
  D3AddEqu(&target,&m2.state->x);

  target_heading=Arg(p_target.x,p_target.y);
  target_angle_error=Unwrap(�-target_heading); //Force to range [-�,�)
  desired_nozzle_angle=Limit(50.0*D�Dt+750*target_angle_error,-�/8,�/8);

  //For realism we limit the speed the nozzle angle can change.
  DstateDt[STATE_NOZZLE_ANGLE]=state[STATE_NOZZLE_ANGLE_VELOCITY];
  DstateDt[STATE_NOZZLE_ANGLE_VELOCITY]=
    Limit(10000*(desired_nozzle_angle-state[STATE_NOZZLE_ANGLE]),-1000,1000)+
    -10.0*state[STATE_NOZZLE_ANGLE_VELOCITY]; //Damping

  if (blast_off) {
    m1.DstateDt->DxDt+=THRUST*Cos(�+state[STATE_NOZZLE_ANGLE]);
    m1.DstateDt->DyDt+=THRUST*Sin(�+state[STATE_NOZZLE_ANGLE]);

    m1.DstateDt->DyDt-=25; //Gravity
    m2.DstateDt->DyDt-=25;
  }

  //For more realism reduce the mass of the rocket because of fuel.
  //You might also factor-in fuel slosh in the tank.

  //To do this, you would have to set-up state vars for mass and
  //do A=F/m manually instead of relyin on $LK,"OdeCallDerivative","MN:OdeCallDerivative"$() to divide
  //by mass.
}

U0 Init()
{
  blast_off=FALSE;
  plane_hit=FALSE;

  do antispin_coefficient=PopUpRangeF64Exp(0.1,10.001,Sqrt(10),
    "%9.4f","Anti-spin Coefficient\n\n");
  while (!(0.1<=antispin_coefficient<10.001));

//We don't clear queue links.
  MemSet(&m1.start_saved_area,0,offset(MassStruct.end_saved_area)-offset(MassStruct.start_saved_area));
  m1.y=-PT_SPAN/2;
  m1.mass=1.0;

  MemSet(&m2.start_saved_area,0,offset(MassStruct.end_saved_area)-offset(MassStruct.start_saved_area));
  m2.y=PT_SPAN/2;
  m2.mass=1.0;

  MemSet(&m3.start_saved_area,0,offset(MassStruct.end_saved_area)-offset(MassStruct.start_saved_area));
  m3.y=380;
  m3.x=-300;
  m3.DxDt=50;
  m3.mass=1.0;

  MemSet(&s.start_saved_area,0,offset(SpringStruct.end_saved_area)-offset(SpringStruct.start_saved_area));
  s.end1=&m1;
  s.end2=&m2;
  s.rest_len=PT_SPAN;
  s.constant=10000;

  ode->state[STATE_NOZZLE_ANGLE]=0;
  ode->state[STATE_NOZZLE_ANGLE_VELOCITY]=0;

  GrDCFill;
}

U0 TaskEndCB()
{
  GrDCFill;
  SndTaskEndCB;
}

U0 RocketScience()
{
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
"  Launch(,CH_SPACE);"
"}"
);

  WinMax;
  dc2=GrDCAlias;
  Fs->task_end_cb=&TaskEndCB;

  "$$CL$$$$BG,LTCYAN$$";
  NewLine(Fs->win_height-4);
  "$$BG,YELLOW$$\n";
 
  ode=OdeNew(STATE_NUM,1e-6,ODEF_HAS_MASSES);
  ode->derivative=&MyDerivative;
  ode->drag_v2=0.002;
  ode->drag_v3=0.00001;
  ode->acceleration_limit=5e3;

//  ode->t_scale=0.1; //Uncomment this to go in slow motion.

  Init;
  InsQue(&m1,ode->last_mass);
  InsQue(&m2,ode->last_mass);
  InsQue(&m3,ode->last_mass);
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
	case CH_SHIFT_ESC:
	case CH_ESC:
	  goto rs_done;
      }
    }
rs_done:
  } catch
    CatchAll;
  RemQue(ode);
  OdeDel(ode);
  "$$CL$$$$BG$$\n";
  SettingsPop;
  GrDCFill;
  GrDCDel(dc2);
  MenuPop;
}

RocketScience;
           r     .      ������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������                5      �������������������������������������������������������������   �����������������������������������������������������   ����       ������������������������������������������   ����  ������������������������������������������   ����  �����������������������������������������   �����  �����������������������������������������   �����  ����������������������������������������   �����  ���������������������������������������   ������  ����������������������������         ��   ������  ������������������          ��� ��   �������  ����������         ���� �   �������            ���� �   �������                     ��������    �   ��������      �   ��������    ��   ���������                ��   ���������      ��������   ��������                 �������������   �������������������               �������������������   ���������������������������� ������������������������   �����������������������������������������������������   �����������������������������������������������������              �     K   /   ��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������� 