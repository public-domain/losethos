//This uses $LK,"fixed-point","FI:::/LT/Demo/Lectures/FixedPoint.CPZ"$.

#define p1_color	    (ROP_EQU+LTCYAN)
#define p2_color	    (ROP_EQU+LTPURPLE)
#define enemy_fire_color    (ROP_EQU+LTBLUE)
#define friendly_fire_color (ROP_EQU+WHITE)
$BG,2$

$PI,"<1>",1$



$PI,"<2>",2$



$PI,"<3>",3$



$PI,"<4>",4$




$PI,"<5>",5$


$BG$
#define NUM_FRAMES 6

U8 *gr_trooper[NUM_FRAMES]={
$IB,"<1>",1$,$IB,"<2>",2$,$IB,"<3>",3$,$IB,"<4>",4$,$IB,"<3>",3$,$IB,"<2>",2$};

U64 fire_end_time=0;
BoolI8 snd_off=TRUE;

class Trooper
{
  I64 num,x,y,dx,dy,att,def,rng,player;
  U64 fire_end_time;
  Trooper *target;
};

#define NUM_TROOPERS 100
Trooper tr[NUM_TROOPERS<<1];
U64 cnt1,cnt2;

BoolI8 time_lapse=FALSE;

#define TAP_MODE_RADIUS	50
BoolI8 tap_mode;
I64 cmd_intensity;


#define AI_NOTHING 0
#define AI_TARGET  1
#define AI_RANDOM  2
#define AI_NUM_AI  3
U64 ai;

I64 targets[10];

U0 DrawTrooper(TaskStruct *task,GrDC *dc,Trooper *tempt,U64 color)
{
  U8 *inter;
  I64 x=tempt->dx>>28,y=tempt->dy>>28,s,gx,gy;
  F64 speed=0.5*Sqrt(x*x+y*y),
	 tt=tempt->num+tP(task)*speed;

  if (time_lapse) {
    x=(tempt->x+500*tempt->dx)>>32;
    y=(tempt->y+500*tempt->dy)>>32;
  } else {
    x=tempt->x.i32[1];
    y=tempt->y.i32[1];
  }
  s=tempt->def;

  if (s>0) {
    s/=3;
    dc->color=color;
    gx=x;gy=y;
    if (tempt->dx<0) {
      dc->flags|=DCF_SYMMETRY|DCF_JUST_MIRROR;
      GrSetSymmetry(dc,x,y,x,y+1);
      gx-=13;
      gy-=7;
    } else {
      dc->flags&=~DCF_SYMMETRY|DCF_JUST_MIRROR;
      gx+=13;
      gy-=7;
    }
    if (GetTimeStamp<tempt->fire_end_time) {
      GrSpritePlot3(dc,x,y,0,$IB,"<5>",5$);
      dc->flags&=~(DCF_SYMMETRY|DCF_JUST_MIRROR);
      if (!tempt->player)
	dc->color=friendly_fire_color;
      else
	dc->color=enemy_fire_color;
      Snd(2000);
      snd_off=FALSE;
      GrLine3(dc,gx,gy,0,tempt->target->x.i32[1],tempt->target->y.i32[1],0);
    } else {
      inter=GrSpriteInterpolate(gr_trooper[tt%NUM_FRAMES],gr_trooper[(tt+1.0)%NUM_FRAMES],tt%1.0);
      GrSpritePlot3(dc,x,y,0,inter);
      Free(inter);
      dc->flags&=~(DCF_SYMMETRY|DCF_JUST_MIRROR);
    }

    if (GetTimeStamp>=fire_end_time)
      if (!snd_off) {
	snd_off=TRUE;
	Snd(0);
      }
  }
}

U0 DrawIt(TaskStruct *task,GrDC *dc)
{
  I64 i,c,x=ipx-task->win_pixel_left-task->win_scroll_x,y=ipy-task->win_pixel_top-task->win_scroll_y;
  cmd_intensity=(ip_throttle*128)>>32;
  BoolI8 repulsive=cmd_intensity>0 ^^ ip_rb,active=!win_grab_scroll && (ip_lb||ip_rb);

  task->text_attr=GREEN<<4;
  cnt1=0;
  cnt2=0;
  for (i=0;i<NUM_TROOPERS;i++)
    if (tr[i].def>0) {
      DrawTrooper(task,dc,&tr[i],p1_color);
      cnt1++;
    }
  for (i=NUM_TROOPERS;i<NUM_TROOPERS<<1;i++)
    if (tr[i].def>0) {
      DrawTrooper(task,dc,&tr[i],p2_color);
      cnt2++;
    }

  if (tap_mode) {
    dc->color=YELLOW;
    GrCircle(dc,x,y,TAP_MODE_RADIUS);
  } else {
    if (repulsive) {
      if (active)
	c=ROP_EQU+LTRED;
      else
	c=ROP_EQU+RED;
    } else {
      if (active)
	c=ROP_EQU+LTBLUE;
      else
	c=ROP_EQU+BLUE;
    }
    dc->color=c;
    GrCircle(dc,x,y,AbsI64(cmd_intensity));
  }
  dc->color=p1_color&15;
  GrPrintF(dc,(task->win_right-8)*FONT_WIDTH,0,"%03d",cnt1);
  dc->color=p2_color&15;
  GrPrintF(dc,(task->win_right-4)*FONT_WIDTH,0,"%03d",cnt2);
}

U0 DoAiTarget()
{
  I64 i,j;
  Trooper *tempt,*tempt2;
  for (i=0;i<10;i++) {
    tempt2=&tr[targets[i]];
    for (j=0;j<10;j++) {
      tempt=&tr[i*10+j+NUM_TROOPERS];
      tempt->dx=(tempt2->x-tempt->x)>>11;
      tempt->dy=(tempt2->y-tempt->y)>>11;
    }
  }
}

U0 UpdatePos()
{
  I64 i;
  Trooper *tempt;
  for (i=0;i<NUM_TROOPERS<<1;i++) {
    tempt=&tr[i];
    tempt->x+=tempt->dx;
    if (tempt->x>=GR_WIDTH<<32) tempt->x-=GR_WIDTH<<32;
    if (tempt->x<0) tempt->x+=GR_WIDTH<<32;
    tempt->y+=tempt->dy;
    if (tempt->y>=GR_HEIGHT<<32) tempt->y-=GR_HEIGHT<<32;
    if (tempt->y<0) tempt->y+=GR_HEIGHT<<32;
  }
}

U0 ResolveFiring()
{
  I64 i,j,d,dx,dy;
  for (i=0;i<NUM_TROOPERS;i++)
    for (j=NUM_TROOPERS;j<NUM_TROOPERS<<1;j++) {
      if (tr[i].def>0 && tr[j].def>0) {
	if (tr[i].target) {
	  if (tr[i].fire_end_time<GetTimeStamp) {
	    tr[i].target->def-=tr[i].att;
	    tr[i].target=NULL;
	  }
	}
	if (tr[j].target) {
	  if (tr[j].fire_end_time<GetTimeStamp) {
	    tr[j].target->def-=tr[j].att;
	    tr[j].target=NULL;
	  }
	}
	dx=(tr[i].x-tr[j].x)>>32;
	dy=(tr[i].y-tr[j].y)>>32;
	d=dx*dx+dy*dy;
	if (d<tr[i].rng && !tr[i].target) {
	  tr[i].fire_end_time=GetTimeStamp+time_stamp_freq>>3;
	  fire_end_time=tr[i].fire_end_time;
	  tr[i].target=&tr[j];
	}
	if (d<tr[j].rng && !tr[j].target) {
	  tr[j].fire_end_time=GetTimeStamp+time_stamp_freq>>3;
	  fire_end_time=tr[i].fire_end_time;
	  tr[j].target=&tr[i];
	}
      }
    }
}

U0 UpdateVelocities()
{
  F64 intensity;
  I64 i,j,x=ipx-Fs->win_pixel_left-Fs->win_scroll_x,y=ipy-Fs->win_pixel_top-Fs->win_scroll_y,dx,dy,d;
  BoolI8 active=!win_grab_scroll&&(ip_lb||ip_rb);
  Trooper *tempt;
  if (tap_mode) {
    for (i=0;i<NUM_TROOPERS;i++) {
      tempt=&tr[i];
      dx=x- tempt->x.i32[1];
      dy=y- tempt->y.i32[1];
      if ((d=dx*dx+dy*dy) && d<TAP_MODE_RADIUS*TAP_MODE_RADIUS) {
	intensity=SqrI64(SqrI64(SqrI64(TAP_MODE_RADIUS)-d));
	dx=intensity*dx/d;
	dy=intensity*dy/d;
	tempt->dx=tempt->dx-dx;
	tempt->dy=tempt->dy-dy;
      } else {
	tempt->dx-=0.2*tempt->dx;
	tempt->dy-=0.2*tempt->dy;
      }
    }
  } else if (active) {
    j=cmd_intensity<<29;
    if (ip_rb)
      j=-j;
    for (i=0;i<NUM_TROOPERS;i++) {
      tempt=&tr[i];
      dx=x- tempt->x.i32[1];
      dy=y- tempt->y.i32[1];
      if (d=dx*dx+dy*dy) {
	dx=j*dx/d;
	dy=j*dy/d;
	tempt->dx=tempt->dx-dx;
	tempt->dy=tempt->dy-dy;
      }
    }
  }
}

U0 Init()
{
  I64 i,j,dx,dy;
  Trooper *tempt;
  ip_throttle=-1<<30;
  tap_mode=FALSE;
  cmd_intensity=128;
  time_lapse=FALSE;
  for (i=0;i<10;i++) {
    targets[i]=RandU16%NUM_TROOPERS;
    dx=RandI32;
    dy=RandI32;
    for (j=0;j<10;j++) {
      tempt=&tr[i*10+j];
      tempt->target=NULL;
      tempt->num=i*10+j;
      tempt->x=(GR_WIDTH-100-i*10)<<32;
      tempt->y=(GR_HEIGHT>>1-50+j*10)<<32;
      tempt->dx=0;
      tempt->dy=0;
      tempt->fire_end_time=GetTimeStamp;
      tempt->att=3;
      tempt->def=10;
      tempt->rng=50*50;
      tempt->player=0;

      tempt=&tr[i*10+j+NUM_TROOPERS];
      tempt->target=NULL;
      tempt->num=i*10+j;
      tempt->x=(100+i*10)<<32;
      tempt->y=(GR_HEIGHT>>1-50+j*10)<<32;
      tempt->dx=0;
      tempt->dy=0;
      if (ai==AI_RANDOM) {
	tempt->dx=dx;
	tempt->dy=dy;
      }
      tempt->fire_end_time=GetTimeStamp;
      tempt->att=3;
      tempt->def=10;
      tempt->rng=50*50;
      tempt->player=1;
    }
  }
  ai=RandU16%AI_NUM_AI;
}

U0 WarGame()
{
  MenuEntry *tempse;

  SettingsPush; //See $LK,"SettingsPush","MN:SettingsPush"$
  Preempt(OFF);

  MenuPush(
  "File {"
  "  Abort(,CH_SHIFT_ESC);"
  "  Exit(,CH_ESC);"
  "}"
  "Play {"
  "  Restart(,CH_NEW_LINE);"
  "  TimeLapse(,'1');"
  "  TapMode(,'2');"
      "}"
      );
  WinMax;
  WordStat;

  Init;
  LtfCursor;
  LtfClear;
  Fs->draw_it=&DrawIt;
  PopUpOk("Wheel Ctrls Cmd Strength.\n"
	"Attract or repel with\n"
	"left or right button.\n");

  Fs->win_inhibit=WIF_ALL-WIF_BORDER-WIF_MENU;
  try {
    while (TRUE) {
      switch (ScanChar) {
	case '1':
	  time_lapse=!time_lapse;
	  tempse=MenuEntryFind(Fs->cur_menu,"Play/TimeLapse");
	  tempse->checked=time_lapse;
	  break;
	case '2':
	  tap_mode=!tap_mode;
	  tempse=MenuEntryFind(Fs->cur_menu,"Play/TapMode");
	  tempse->checked=tap_mode;
	  break;
	case CH_ESC:
	case CH_SHIFT_ESC:
	  goto wg_done;
	case CH_NEW_LINE:
	  Init;
	  break;
      }
      Sleep(40);
      if (ai==AI_TARGET)
	DoAiTarget;
      UpdateVelocities;
      UpdatePos;
      ResolveFiring;
    }
wg_done:
  } catch
    CatchAll;
  SettingsPop;
  MenuPop;
}

WarGame;
           �         ������������   ����   ����   ����   ����    ������������������������������������                  ������������   ����   ����              �         ������������   ����   ����   ����          ������������������������   ��������                   ������������   ����   ����              �         ������������   ������������������������        ����   ����   ����      ������������   ����   ����   ��������                             �         ����    ����   ������������������������       ����   ����   ����          ��������   ����   ����       ����                             �         ������������   ������������   ����   ����    ����   ����   ����   ����������������   ����   ����   ��������                   