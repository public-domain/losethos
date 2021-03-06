


$PI,"<1>",1$



$BG,14$






$PI,"<2>",2$








$PI,"<3>",3$





$PI,"<4>",4$



$PI,"<5>",5$

$BG,9$



 
   $PI,"<6>",6$




$PI,"<7>",7$

$BG$
#define AFT_MAST	16
#define BOW_MAST	14
#define AFT_LEN	36
#define BOW_LEN	43
#define BERTH		28

F64 screen_x,screen_y,d_screen_x,d_screen_y,
   wind_speed,

   //This is on a wave close to ship
   //in world coordinates.
   ship_wave_x,ship_wave_y,

   church_x,church_y,

   wave_speed,wave_�,d_wave_�,wave_period,
   wave_amplitude;

BoolI8 wave_sound,ship_landed;
U64 song_time,game_over_time;

F64 view_scale,new_view_scale;
GrDC *master_map;
U8 *vm;

#define NUM_DOLPHINS	5
#define NUM_PILGRIMS	10
	
class Object
{
  F64 x,y,radius,
    �,d_�,
    speed;
} ship,dolphins[NUM_DOLPHINS],pilgrims[NUM_PILGRIMS];

#define MAX_BUBBLES	1024
class Bubble
{
  I32 x,y;
} bubbles[MAX_BUBBLES];

#define MAX_GARDENS	128
class Garden
{
  I32 x,y;
} gardens[MAX_GARDENS];
I64 num_gardens;


F64 WaterHeight(F64 x,F64 y)
{ //Returns wave height at world coordinate point
  F64 dxn=Cos(wave_�),
      dyn=Sin(wave_�),
  �;
  �=2*�*((x-ship_wave_x)*dxn+(y-ship_wave_y)*dyn)/wave_period;
  return wave_amplitude*Cos(�);
}

I64 *ShipMat()
{ //Returns rotation matrix for ship bobbing on waves
  I64 *result;
  F64 x1,y1,z1,x2,y2,z2,a;

  x1=ship.x-AFT_LEN/2*Cos(ship.�)/view_scale;
  y1=ship.y-AFT_LEN/2*Sin(ship.�)/view_scale;
  z1=WaterHeight(x1,y1);
  x2=ship.x+BOW_LEN/2*Cos(ship.�)/view_scale;
  y2=ship.y+BOW_LEN/2*Sin(ship.�)/view_scale;
  z2=WaterHeight(x2,y2);
  a=ASin((z2-z1)/((AFT_LEN/2+BOW_LEN/2)/view_scale));
  result=GrRotY(a);

  z1=WaterHeight(ship.x,ship.y);
  x2=ship.x-BERTH/2*Sin(ship.�)/view_scale;
  y2=ship.y+BERTH/2*Cos(ship.�)/view_scale;
  z2=WaterHeight(x2,y2);
  a=ASin((z2-z1)/(BERTH/view_scale));
  GrRotXEqu(result,a);

  GrRotZEqu(result,ship.�);

  return result;
}

U0 BubbleAdd(I64 x,I64 y)
{
  U64 i=RandU16&(MAX_BUBBLES-1);
  bubbles[i].x=x;
  bubbles[i].y=y;
}

U0 PosChurch(F64 x,F64 y)
{
  I64 i;
  GrDC *dc=GrDCAlias(master_map,Fs);
  dc->color  =ROP_COLLISION;
  dc->bkcolor=BLACK;
  while (x>0) {
    dc->collision_cnt=0;
    GrSpritePlot3(dc,x,y,0,$IB,"<2>",2$);
    if (dc->collision_cnt>2000)
      break;
    x--;
  }
  church_x=x; church_y=y;
  GrDCDel(dc);
  for (i=0;i<NUM_PILGRIMS;i++) {
    pilgrims[i].x=church_x;
    pilgrims[i].y=church_y;
    pilgrims[i].�=2*�*Rand;
    pilgrims[i].d_�=0;
    pilgrims[i].speed=10.0;
  }
}

U0 DrawIt(TaskStruct *task,GrDC *dc)
{
  I64 i,
      w=task->win_pixel_width, cx=w>>1,
      h=task->win_pixel_height,cy=h>>1,
      c=1.05*(MaxI64(cx,cy)+wave_period),
      *ship_r,x,y,z;
  F64 d,xx,yy,dx,dy,dx2,dy2,wave_x,wave_y;

  task->text_attr=LTBLUE<<4;

  //Draw Bubbles
  dc->color=BLUE;
  for (i=0;i<MAX_BUBBLES;i++)
    GrPlot(dc,bubbles[i].x*view_scale-screen_x,bubbles[i].y*view_scale-screen_y);

  //Draw Waves
  wave_x=ship_wave_x*view_scale-screen_x;
  wave_y=ship_wave_y*view_scale-screen_y;

  dx=wave_period*Cos(wave_�)*view_scale;
  dy=wave_period*Sin(wave_�)*view_scale;
  dx2=-dy*c;
  dy2=dx*c;

  dc->color=BLUE;
  if (Abs(dx)>Abs(dy)) {
    xx=wave_x; yy=wave_y;
    while (xx>-c) {
      GrLine(dc,xx-dx2,yy-dy2,xx+dx2,yy+dy2);
      if (dx>0) { xx-=dx; yy-=dy;
      } else {	  xx+=dx; yy+=dy; }
    }
    xx=wave_x; yy=wave_y;
    while (xx<w+c) {
      GrLine(dc,xx-dx2,yy-dy2,xx+dx2,yy+dy2);
      if (dx<0) { xx-=dx; yy-=dy;
      } else {	  xx+=dx; yy+=dy; }
    }
  } else {
    xx=wave_x; yy=wave_y;
    while (yy>-c) {
      GrLine(dc,xx-dx2,yy-dy2,xx+dx2,yy+dy2);
      if (dy>0) { xx-=dx; yy-=dy;
      } else {	  xx+=dx; yy+=dy; }
    }
    xx=wave_x; yy=wave_y;
    while (yy<h+c) {
      GrLine(dc,xx-dx2,yy-dy2,xx+dx2,yy+dy2);
      if (dy<0) { xx-=dx; yy-=dy;
      } else {	  xx+=dx; yy+=dy; }
    }
  }

  //Draw Dolphins
  for (i=0;i<NUM_DOLPHINS;i++)
    GrSpritePlotRotZ3b(dc,dolphins[i].x*view_scale-screen_x,dolphins[i].y*view_scale-screen_y,0,$IB,"<7>",7$,dolphins[i].�);

  //Draw Land
  dc->color=ROP_MONO+YELLOW;
  GrVectMapBlotMP(dc,-screen_x,-screen_y,vm,view_scale);

  //Draw Ship
  dc->color=ROP_EQU;
  ship_r=ShipMat;
  GrSpritePlotRotMat3b(dc,ship.x*view_scale-screen_x,ship.y*view_scale-screen_y,0,$IB,"<6>",6$,ship_r);

  //Rudder
  x=-AFT_LEN; y=0; z=0;
  GrRot(ship_r,&x,&y,&z);
  x+=ship.x*view_scale; y+=ship.y*view_scale;
  dc->color=BLACK;
  dc->pen_width=2;
  GrLine3(dc,x-screen_x,y-screen_y,0,x-8*Cos(ship.�-2.5*ship.d_�)-screen_x,y-8*Sin(ship.�-2.5*ship.d_�)-screen_y,0);
  dc->pen_width=1;

  //Wake
  x=(-(AFT_LEN+2)+RandU16%13-6)/view_scale; y=(RandU16%13-6)/view_scale; z=0;
  GrRot(ship_r,&x,&y,&z);
  x+=ship.x; y+=ship.y;
  BubbleAdd(x,y);
  for (i=0;i<NUM_DOLPHINS;i++) {
    if (!(RandU16&15)) {
      x=dolphins[i].x-(7*Cos(dolphins[i].�)+RandU16%3-1)/view_scale;
      y=dolphins[i].y-(7*Sin(dolphins[i].�)+RandU16%3-1)/view_scale;
      BubbleAdd(x,y);
    }
  }
 

  //Draw Sails
  d=Unwrap(ship.�-Unwrap(wave_�,ship.�-�));
  if (Abs(d)<�)
    d=(Abs(d)-�)*Sign(-d);
  dx=-1.5*BERTH/2*Sin(ship.�+d/2);
  dy= 1.5*BERTH/2*Cos(ship.�+d/2);

  //Aft Sail
  x=-AFT_MAST; y=0; z=0;
  GrRot(ship_r,&x,&y,&z);
  x+=ship.x*view_scale; y+=ship.y*view_scale;
  x-=1.5*Cos(wave_�); y-=1.5*Sin(wave_�);
  dc->color=WHITE;
  GrLine3(dc,x-dx-screen_x,y-dy-screen_y,0,x+dx-screen_x,y+dy-screen_y,0);

  x=-AFT_MAST; y=0; z=0;
  GrRot(ship_r,&x,&y,&z);
  x+=ship.x*view_scale; y+=ship.y*view_scale;
  dc->color=BLACK;
  GrLine3(dc,x-dx-screen_x,y-dy-screen_y,0,x+dx-screen_x,y+dy-screen_y,0);

  //Bow Sail
  x=BOW_MAST; y=0; z=0;
  GrRot(ship_r,&x,&y,&z);
  x+=ship.x*view_scale; y+=ship.y*view_scale;
  x-=1.5*Cos(wave_�); y-=1.5*Sin(wave_�);
  dc->color=WHITE;
  GrLine3(dc,x-dx-screen_x,y-dy-screen_y,0,x+dx-screen_x,y+dy-screen_y,0);

  x=BOW_MAST; y=0; z=0;
  GrRot(ship_r,&x,&y,&z);
  x+=ship.x*view_scale; y+=ship.y*view_scale;
  dc->color=BLACK;
  GrLine3(dc,x-dx-screen_x,y-dy-screen_y,0,x+dx-screen_x,y+dy-screen_y,0);

  dc->color=BLACK;
  GrPrintF(dc,0,0,"Speed:%5.1f Knots",ship.speed);

  //Check for wreck
  x=BOW_LEN; y=0; z=0;
  GrRot(ship_r,&x,&y,&z);
  x+=ship.x*view_scale; y+=ship.y*view_scale;
  if (!ship_landed && GrPeek(dc,x-screen_x,y-screen_y)==YELLOW) {
    Noise(500,500,1000);
    song_time=GetTimeStamp+time_stamp_freq/2;
    game_over_time=GetTimeStamp+time_stamp_freq;
    wave_sound=FALSE;
    ship_landed=TRUE;
    PosChurch(x/view_scale,y/view_scale);
  }

  if (ship_landed) {
    //Draw Gardens
    for (i=0;i<MAX_GARDENS;i++)
      GrSpritePlot3(dc,gardens[i].x*view_scale-screen_x,gardens[i].y*view_scale-screen_y,0,$IB,"<5>",5$);

    //Draw Church
    GrSpritePlot3(dc,church_x*view_scale-screen_x,church_y*view_scale-screen_y,0,$IB,"<2>",2$);
    //Draw Pilgrims
    for (i=0;i<NUM_PILGRIMS;i++)
      GrSpritePlot3(dc,pilgrims[i].x*view_scale-screen_x,pilgrims[i].y*view_scale-screen_y,0,(i&1)?$IB,"<3>",3$:$IB,"<4>",4$);
    if (GetTimeStamp>game_over_time && Blink(,tP(task))) {
      dc->color=LTRED;
      GrPrintF(dc,(task->win_pixel_width-14*FONT_WIDTH)/2,
	  (task->win_pixel_height-FONT_HEIGHT)/2,"Game Completed");
    }
  }
  Free(ship_r);
}

U0 Init()
{
  I64 i;
  screen_x=400;   d_screen_x=0;
  screen_y=500;   d_screen_y=0;
  view_scale=1.0; new_view_scale=1.0;
  MemSetU32(&bubbles,MAX_I32,sizeof(bubbles)/sizeof(U32));

  num_gardens=0;
  MemSetU32(&gardens,MAX_I32,sizeof(gardens)/sizeof(U32));

  MemSet(&dolphins,0,sizeof(dolphins));
  for (i=0;i<NUM_DOLPHINS;i++) {
    dolphins[i].radius =5;
    dolphins[i].x	   =screen_x+Rand*(GR_WIDTH-2*dolphins[i].radius)+dolphins[i].radius;
    dolphins[i].y	   =screen_y+Rand*(GR_WIDTH-2*dolphins[i].radius)+dolphins[i].radius;
    dolphins[i].�  =Rand*2*�;
    dolphins[i].d_�=0;
    dolphins[i].speed  =15;
  }

  MemSet(&ship,0,sizeof(ship));
  ship.radius =MaxI64(AFT_LEN,BOW_LEN);
  ship.x	    =screen_x+GR_WIDTH/2-ship.radius;
  ship.y	    =screen_y+GR_HEIGHT/2;
  ship.�  =�;
  ship.d_�=0;
  ship.speed  =1.0;
  ship_wave_x=0; ship_wave_y=0;

  wave_�=Rand*�*2;
  d_wave_�=0;
  wave_amplitude=10;
  wave_period=75;
  wave_speed=20*2*�/wave_period;
  wind_speed=25;
  wave_sound=TRUE;
  ship_landed=FALSE;
  song_time=0;
  game_over_time=0;

}


U0 AnimateObject(Object *tempo,F64 dt)
{
  tempo->x+=tempo->speed*dt*Cos(tempo->�);
  tempo->y+=tempo->speed*dt*Sin(tempo->�);
  tempo->�=Unwrap(tempo->�+tempo->d_�*dt);
  tempo->d_�=LowPass1(0.5,tempo->d_�,0,dt);
}

U0 SongTask(U64 dummy=0)
{
  nounusedwarn dummy;
  Fs->task_end_cb=&SndTaskEndCB;
  MusicSettingsRst;
  music_tempo= 2.480;
  music_stacatto_factor= 0.902;
  Play("M3/4");
  Play("hRW13qDhG4eB3G");
  Play("4hBqA3hGqE");
  Play("hDqDhG4eB3G");
  Play("4hBqAh.DhD");
}

#define SCREEN_BUMP	100

U0 AnimateTask(U64 dummy=0)
{
  nounusedwarn dummy;
  I64 i,j,
     w=Fs->parent_task->win_pixel_width,  cx=w/2,
     h=Fs->parent_task->win_pixel_height, cy=h/2;
  F64 d,dx,dy,dx2,dy2,dxn,dyn,t0=tP(Fs->parent_task),dt,old_view_scale;

  while (TRUE) {
    dt=tP(Fs->parent_task)-t0;
    t0=tP(Fs->parent_task);

    old_view_scale=view_scale;
    view_scale=Exp(0.5*dt*(Ln(new_view_scale)-Ln(view_scale))+Ln(view_scale));
    screen_x=(screen_x+cx)*view_scale/old_view_scale-cx;
    screen_y=(screen_y+cy)*view_scale/old_view_scale-cy;

    for (i=0;i<NUM_DOLPHINS;i++) {
      dolphins[i].d_�+=10.0*(Rand-0.5)*dt;
      AnimateObject(&dolphins[i],dt);
      if (dolphins[i].x-screen_x<dolphins[i].radius)
	dolphins[i].x+=(w-2*dolphins[i].radius)/view_scale;
      if (dolphins[i].x-screen_x>w-1-dolphins[i].radius)
	dolphins[i].x-=(w-2*dolphins[i].radius)/view_scale;
      if (dolphins[i].y-screen_y<dolphins[i].radius)
	dolphins[i].y+=(h-2*dolphins[i].radius)/view_scale;
      if (dolphins[i].y-screen_y>h-1-dolphins[i].radius)
	dolphins[i].y-=(h-2*dolphins[i].radius)/view_scale;
    }

    if (ship_landed) {
      for (i=0;i<NUM_PILGRIMS;i++) {
	pilgrims[i].d_�+=10.0*(Rand-0.5)*dt;
	AnimateObject(&pilgrims[i],dt);
	if (GrPeek(master_map,pilgrims[i].x,pilgrims[i].y)!=1) {
	  pilgrims[i].speed=-pilgrims[i].speed;
	  AnimateObject(&pilgrims[i],dt);
	} else if (!(ToI64(pilgrims[i].x)&15) && !(ToI64(pilgrims[i].y)&15)) {
	  for (j=0;j<MAX_GARDENS;j++)
	    if (gardens[j].x==ToI64(pilgrims[i].x)&&
		gardens[j].y==ToI64(pilgrims[i].y))
	      break;
	  if (j==MAX_GARDENS) {
	    gardens[num_gardens  &(MAX_GARDENS-1)].x=pilgrims[i].x;
	    gardens[num_gardens++&(MAX_GARDENS-1)].y=pilgrims[i].y;
	  }
	}
      }
      if (church_x*view_scale-screen_x-0.9*d_screen_x<w/4)
	d_screen_x-=SCREEN_BUMP;
      if (church_x*view_scale-screen_x-0.9*d_screen_x>w-1-w/4)
	d_screen_x+=SCREEN_BUMP;
      if (church_y*view_scale-screen_y-0.9*d_screen_y<h/4)
	d_screen_y-=SCREEN_BUMP;
      if (church_y*view_scale-screen_y-0.9*d_screen_y>h-1-h/4)
	d_screen_y+=SCREEN_BUMP;
    } else {
      d=Unwrap(ship.�-Unwrap(wave_�,ship.�-�));
      if (Abs(d)<�)
	d=(Abs(d)-�)*Sign(-d);
      d=wind_speed*Cos(d/2);
      ship.speed=LowPass1(0.25,ship.speed,d,dt);
      AnimateObject(&ship,dt);
      if (ship.x*view_scale-screen_x-0.9*d_screen_x<w/4)
	d_screen_x-=SCREEN_BUMP;
      if (ship.x*view_scale-screen_x-0.9*d_screen_x>w-1-w/4)
	d_screen_x+=SCREEN_BUMP;
      if (ship.y*view_scale-screen_y-0.9*d_screen_y<h/4)
	d_screen_y-=SCREEN_BUMP;
      if (ship.y*view_scale-screen_y-0.9*d_screen_y>h-1-h/4)
	d_screen_y+=SCREEN_BUMP;
      if (ship.x<0) ship.x=0;
      if (ship.y<0) ship.y=0;
    }

    if (screen_x<0) {
      screen_x=0;
      d_screen_x=0;
    }
    if (screen_y<0) {
      screen_y=0;
      d_screen_x=0;
    }

    wave_�=Unwrap(wave_�+d_wave_�*dt);
    d_wave_�=Limit(LowPass1(1.0,d_wave_�,
	d_wave_�+1.0*(Rand-0.5),dt),-0.05,0.05);

    screen_x+=0.9*d_screen_x*dt;
    d_screen_x-=0.9*d_screen_x*dt;
    screen_y+=0.9*d_screen_y*dt;
    d_screen_y-=0.9*d_screen_y*dt;

    dxn=Cos(wave_�);
    dyn=Sin(wave_�);
    dx=wave_period*dxn;
    dy=wave_period*dyn;
    ship_wave_x-=wave_speed*dt/(2*�)*dx;
    ship_wave_y-=wave_speed*dt/(2*�)*dy;
    if (Abs(dx)>Abs(dy)) {
      i=(ship_wave_x-ship.x)/dx;
      ship_wave_x-=i*dx;
      ship_wave_y-=i*dy;
      dx2=ship_wave_x-ship.x;
      dy2=ship_wave_y-ship.y;
      d=dyn*dx2-dxn*dy2;
      ship_wave_x-=dyn*d;
      ship_wave_y+=dxn*d;
    } else {
      i=(ship_wave_y-ship.y)/dy;
      ship_wave_x-=i*dx;
      ship_wave_y-=i*dy;
      dx2=ship_wave_x-ship.x;
      dy2=ship_wave_y-ship.y;
      d=dyn*dx2-dxn*dy2;
      ship_wave_x-=dyn*d;
      ship_wave_y+=dxn*d;
    }

    if (wave_sound)
      Snd(200+8*WaterHeight(ship.x,ship.y));
    else if (song_time && GetTimeStamp>song_time) {
      Spawn(&SongTask,NULL,"Song",Fs);
      song_time=0;
    }
    WinSync;
  }
}

U0 Pilgrims()
{
  U64 msg_code,ch,sc;

  "Initializing Maps...\n";
  master_map=LGRRead("NewEngland"); //See $LK,"Acknowledgements","FF:::/LT/Doc/Acknowledgements.TXZ,US Geological"$ (7).

  vm=GrDC2VectMap(master_map);
  PopUpOk("This is about the pilgrims who fled\n"
	"religious persecution in England.\n"
	"They sailed in 1620 on the Mayflower\n"
	"and landed at Plymoth, Massachusetts.\n"
	"\n"
	"This game is comic, mostly.Be sure\n"
	"to try 'z' or 'Z' for zooming.\n");
  SettingsPush; //See $LK,"SettingsPush","MN:SettingsPush"$

  MenuPush(
  "File {"
  "  Abort(,CH_SHIFT_ESC);"
  "  Exit(,CH_ESC);"
  "}"
  "Play {"
  "  Restart(,CH_NEW_LINE);"
  "  Port(,,SC_CURSOR_LEFT);"
  "  Starboard(,,SC_CURSOR_RIGHT);"
  "}"
  "View {"
  "  ZoomIn(,'z');"
  "  ZoomOut(,'Z');"
      "}"
      );

  WordStat;
  Preempt(OFF);
  WinMax;
  LtfCursor;
  LtfClear;
  Fs->draw_it=&DrawIt;
  Fs->animate_task=Spawn(&AnimateTask,NULL,"Animate",Fs);
  try {
    Init;
    while (TRUE) {
      switch (msg_code=GetMsg(&ch,&sc,1<<MSG_KEY_DOWN|1<<MSG_KEY_UP)) {
	case MSG_KEY_DOWN:
	  switch (ch) {
	    case 0:
	      switch (sc.u8[0]) {
		case SC_CURSOR_LEFT:
		  ship.d_�=Limit(ship.d_�-5*�/180,-.5,.5);
		  break;
		case SC_CURSOR_RIGHT:
		  ship.d_�=Limit(ship.d_�+5*�/180,-.5,.5);
		  break;
	      }
	      break;
	    case 'z':
	      if (new_view_scale<10.0)
		new_view_scale*=1.5;
	      break;
	    case 'Z':
	      if (new_view_scale>0.1)
		new_view_scale/=1.5;
	      break;
	    case CH_NEW_LINE:
	      Init;
	      break;
	    case CH_ESC:
	    case CH_SHIFT_ESC:
	      goto pilgrim_done;
	  }
	  break;
      }
    }
pilgrim_done:
  } catch
    CatchAll;
  MenuPop;
  SettingsPop;
  GrVectMapDel(vm);
  GrDCDel(master_map);
}
                X   #   �����������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������                                    ������������������������������������������������           ���������������������������������������          ���������������������������������        �����������������������������     ��������������������������      �������������������������    �����������������������     ��������������������     ������������������      �����������������     ��������������       ������������               �����������          ���������               ��������              � ��������     ������������      ��������������       �����������������      �������������������      ���������������������    �������������������������     ����������������������������     ������������������������������           ����������������������������������          �������������������������������������������           �����������������������������������������������������                               ������������������������������������������������������������� ������������������  �������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������                   
   ��������������������������      ������������������      ���  ��������  ���      ��� �������   ���      ���         ��      ��    ��      ��       �      ��   �����     ��      �� ����������� ���      ������������������                 B     /   !      ��������������������������������������������������� ����������������������������������������������� ����������������      ������������������������� ���������������             ������������������� ���������������              ������������������ ��������������               ������������������ ��������������               ������������������ ��������������        ����������������� �������������         ����������������� �������������              ���������������� ������������               ���������������� ������������ ����� �������� ��������������� ������������ ����� �������� ��������������� ������������ ����� �������� ��������������� ����������� ������ ��������� �������������� ����������� ������ ��������� �������������� ����������� ���         ����� ������������� ����������         ����   ��� ������������� ����������         ����     �� ������������ ���������                  ������������ ���������                  ������������ ���������                           ����������� ��������                            ����������� ��������                             ���������� ��������                             ���������� �������                               ��������� �������                               ��������� �������                ��������� ������                    �������� ������         ��������        ������ ����   ����    �����������������          ��� �   ���    �������������������������     �               ������������������������������� �����             J     1   A   ���������������������������������������������������������       �������������������������������������������������       �������������������������������������������������       ��������������� ��������������   ����������������       ��������������� �������������  ���������������       ��������������� ������������  ��������������       ��������������� ����������    ������������       �����������         �����  �����������       ��������������� ��������  ����������       ��������������� ������    ��������       ��������������� �����  �������       ��������������� ����  ������       ��������������� ��    ����       ��������������� �  ���       ���������������   ��       ���������������   �       ��������������    �       �������������    �       ������������    �       ������������     �       �����������    �       ����������    �       ���������    �       ���������     �       ��������    �       �������    �       ������    �       ������     �       �����    �       ����    �       ���    �       ���   �       ���   �       ���   �       ���   �       ���   �       ���   �       ���   �       ���   �       ���            �       ���            ��       ���       ���       ���       ����       ���       �����       ���       ������       ���       �������       ���       ��������       ���       ���������       ���       ����������       ���       �����������       ���       ������������       ���            �������������       ���   ��������������       ���                ���������������       ���     ����������������       ���                    �����������������       ���        ������������������       ���                  �������������������       ���        ��������������������       ���                         ���������������������       ���     ��������������������������       ���    ���������������������������       ���   ����������������������������       ���                 �����������������������������       �������������������������������������������������                  "        !   ����������������������� ������     ���� �����    ������ �����  ����� ����     ����� ��           �� ����  ����� �����  ����� �����  ������ ������   ������ ���        ���� ��         ���  ��  �     ��  ��  �     ��  ��  �      �  ��  �      �  �   �         �  ��    �    �  ��    �    �  ��    ��   �  ��      ���  �  ��      ���  �����  ��  ���  �����  ��  ���  �����  ��  ���  �����  ��  ���  �����  ��  ���  �����  ��   ��  �����  ���  ��  �����  ���  ��  �����  ���  ��  �����  ���  ��  ����������   �                        ���������������������   �����   �����   �����   ���   ���� �    ���   ����   ����   ����  ����   �����  ����   �����  ����   ���      ��   ��     �   �      �   �      �   �     �    �   �    �  � �    ��  � �    ��  � �    ��  � �    ��  �      ��  ���    ��  ���     �   ���   ���   ��   ���   ��   ���   ��   ��   �   ��   �   ��   �   ��              ����� ���� ��   ����   ��   �              �           ��������������������                    �����������  




 �       ����������  




 ��       ���������  




 ���       ��������  




 ����       �������  



 �����       ������           ������       �����   �������       ����  ��������       ���   ���������       ��  ����������       �   �����������                    ������������        