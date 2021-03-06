





    $PI,"<1>",1$




    $PI,"<2>",2$









    $PI,"<3>",3$




















    $PI,"<4>",4$






I64 vx,vy,vz;
F64 �,speed;

#define NUM_BUSHES	512

class Bush
{
  I32 x,y,z;
  BoolI8 sym,pad[3];
  U8 *img;
} b[NUM_BUSHES];

#define TRACK_INNER	16000
#define TRACK_OUTER	18000
#define SHOULDER	200
#define SLICES		500
#define D_�		(2*�/SLICES)
#define TRACK_POLYS	(3*SLICES)
#define DIPS		5
#define DIP_DEPTH	100
class TrackPoly
{
  ColorROPU16 c,pad[3];
  P3I32 p[4];
} track[TRACK_POLYS];

I64 DipY(I64 x,I64 z)
{
  F64 m,a;
  R2P(&m,&a,x,z);
  return DIP_DEPTH*m*Cos(DIPS*a)/((TRACK_INNER+TRACK_OUTER)/2);
}

#define CAR_LENGTH	400

F64 Dip�(I64 x,I64 z)
{
  F64 y_front,y_back;
  y_front=DipY(x-CAR_LENGTH/2*Cos(�),z-CAR_LENGTH/2*Sin(�));
  y_back =DipY(x+CAR_LENGTH/2*Cos(�),z+CAR_LENGTH/2*Sin(�));
  return ASin((y_front-y_back)/CAR_LENGTH);
}

U0 InitTrackPoly(TrackPoly *tempt,ColorROPU16 c,
  I64 inner,I64 outer,F64 �1,F64 �2)
{
  tempt->c=c;
  tempt->p[0].x=inner*Cos(�1);
  tempt->p[0].z=inner*Sin(�1);
  tempt->p[0].y=DipY(tempt->p[0].x,tempt->p[0].z);
  tempt->p[1].x=outer*Cos(�1);
  tempt->p[1].z=outer*Sin(�1);
  tempt->p[1].y=DipY(tempt->p[1].x,tempt->p[1].z);
  tempt->p[2].x=outer*Cos(�2);
  tempt->p[2].z=outer*Sin(�2);
  tempt->p[2].y=DipY(tempt->p[2].x,tempt->p[2].z);
  tempt->p[3].x=inner*Cos(�2);
  tempt->p[3].z=inner*Sin(�2);
  tempt->p[3].y=DipY(tempt->p[3].x,tempt->p[3].z);
}

U0 InitTrack()
{
  TrackPoly *tempt=track;
  I64 i;
  for (i=0;i<SLICES;i++) {
    InitTrackPoly(tempt++,DKGRAY,TRACK_INNER,TRACK_OUTER,
      i*2*�/SLICES,(i+1)*2*�/SLICES);
    InitTrackPoly(tempt++,i&1?RED:WHITE,TRACK_INNER-SHOULDER,TRACK_INNER,
      i*2*�/SLICES,(i+1)*2*�/SLICES);
    InitTrackPoly(tempt++,i&1?WHITE:RED,TRACK_OUTER,TRACK_OUTER+SHOULDER,
      i*2*�/SLICES,(i+1)*2*�/SLICES);
  }
}

#define HORIZON_DIP	200

U64 mp_done_flags;

U0 MPUpdateWin(GrDC *dc2)
{
  TaskStruct *task=dc2->win_task;
  I64 i,j,x,y,z,lo,hi,
      w=task->win_pixel_width,
      h=task->win_pixel_height,r[16],
      cx=w>>1;
  F64 s,dip_�=Dip�(vx,vz);
  P3I32 poly[4];
  TrackPoly *tempt;
  GrDC *dc=GrDCAlias(gr_dc,task);

  GrIdentEqu(r);
  GrRotYEqu(r,-�);
  GrRotXEqu(r,75*�/180-dip_�);

  dc->depth_buf=dc2->depth_buf;

  //Track
  lo=Gs->num*TRACK_POLYS/mp_cnt;
  hi=(Gs->num+1)*TRACK_POLYS/mp_cnt;
  for (i=lo,tempt=&track[lo];i<hi;i++,tempt++) {
    dc->color=tempt->c;
    for (j=0;j<4;j++) {
      x=tempt->p[j].x+vx; y=tempt->p[j].y+vy; z=tempt->p[j].z+vz;
      GrRot(r,&x,&y,&z);
      s=100.0/(AbsI64(z)+50);
      poly[j].x=s*x+cx;
      poly[j].y=s*y+h;
      poly[j].z=z;
    }
    if (-h<poly[0].y<h*2 && -h<poly[1].y<h*2 &&
	-h<poly[2].y<h*2 && -h<poly[3].y<h*2)
      GrFillPoly3(dc,4,poly);
  }

  for (i=Gs->num;i<NUM_BUSHES;i+=mp_cnt) {
    x=b[i].x+vx; y=b[i].y+vy; z=b[i].z+vz;
    GrRot(r,&x,&y,&z);
    s=100.0/(AbsI64(z)+50);
    if (s*y<0) {
      GrIdentEqu(dc->r);
      GrScaleMatEqu(dc->r,s*2);
      GrSetRotMat(dc,dc->r);
      dc->flags|=DCF_TRANSFORMATION;
      if (b[i].sym) {
	dc->flags|=DCF_SYMMETRY|DCF_JUST_MIRROR;
	GrSetSymmetry(dc,s*x+cx,s*y+h,s*x+cx,s*y+h+10);
      }
      GrSpritePlot3b(dc,s*x+cx,s*y+h,z,b[i].img);
      dc->flags&=~(DCF_SYMMETRY|DCF_JUST_MIRROR);
    }
  }
  dc->depth_buf=NULL;
  GrDCDel(dc);
  LBts(&mp_done_flags,Gs->num);
}

U0 DrawIt(TaskStruct *task,GrDC *dc)
{
  I64 i,x,y,z,
      w=task->win_pixel_width,
      h=task->win_pixel_height,r[16],
      cx=w>>1;
  F64 s,dip_�=Dip�(vx,vz);

  task->text_attr=YELLOW<<4;
  dc->color=LTCYAN;
  GrRect(dc,0,0,w,HORIZON_DIP*Sin(dip_�)+FONT_HEIGHT*4.5);

  GrIdentEqu(r);
  GrRotYEqu(r,-�);
  GrRotXEqu(r,75*�/180-dip_�);

  GrDCAllocDepthBuf(dc);

  //Sun
  x=vx; y=0; z=1000000+vz;
  GrRot(r,&x,&y,&z);
  s=100.0/(AbsI64(z)+50);
  if (y<0) {
    dc->color=BROWN;
    GrCircle(dc,s*x+cx,15+HORIZON_DIP*Sin(dip_�),15);
    dc->color=YELLOW;
    GrFloodFill(dc,s*x+cx,15+HORIZON_DIP*Sin(dip_�));
  }

  mp_done_flags=0;
  for (i=1;i<mp_cnt;i++)
    MPJob(&MPUpdateWin,dc,1<<MPCf_FREE_ON_COMPLETE,1<<i);
  MPUpdateWin(dc);

  while (mp_done_flags!=1<<mp_cnt-1)
    Yield;

  //Map
  dc->color=LTGRAY;
  GrRect(dc,w-64,h-64,64,64);
  dc->color=LTGREEN;
  for (i=0;i<NUM_BUSHES;i++)
    GrPlot(dc,w-64+64*(b[i].x+0x5000)/0xA000,h-64*(b[i].z+0x5000)/0xA000);

  dc->color=WHITE;
  GrPlot(dc,w-64+64*(-vx+0x5000)/0xA000,h-64*(-vz+0x5000)/0xA000);
}


U0 AnimateTask(U64 dummy=0)
{
  nounusedwarn dummy;
  while (TRUE) {
    Snd(speed*3+10);
    vx-=speed*Sin(�);
    vz-=speed*Cos(�);
    vy=DipY(vx,vz);
    Sleep(10);
  }
}

U8 *imgs[8]={$IB,"<1>",1$,$IB,"<1>",1$,$IB,"<2>",2$,$IB,"<2>",2$,$IB,"<3>",3$,$IB,"<4>",4$,$IB,"<4>",4$,$IB,"<4>",4$};

U0 Init()
{
  Bush *tempb;
  I64 i;
  for (i=0;i<NUM_BUSHES;i++) {
    tempb=&b[i];
    do {
      tempb->x=RandU32&0x7FFF-0x4000;
      tempb->z=RandU32&0x7FFF-0x4000;
    } while (TRACK_INNER-SHOULDER<=Sqrt(SqrI64(tempb->x)+SqrI64(tempb->z))<=TRACK_OUTER+SHOULDER);
    tempb->y=DipY(tempb->x,tempb->z);
    tempb->sym=RandU16&1;
    tempb->img=imgs[i&7];
  }
  InitTrack;
  vx=-(TRACK_INNER+TRACK_OUTER)/2; vz=0; vy=DipY(vx,vz);
  �=0; speed=0;
}

U0 Varoom()
{
  U64 ch,sc;
  MenuPush(
  "File {"
  "  Abort(,CH_SHIFT_ESC);"
  "  Exit(,CH_ESC);"
  "}"
  "Play {"
  "  Restart(,CH_NEW_LINE);"
  "  Accelerator(,,SC_CURSOR_UP);"
  "  Break(,,SC_CURSOR_DOWN);"
  "  Left(,,SC_CURSOR_LEFT);"
  "  Right(,,SC_CURSOR_RIGHT);"
      "}"
      );
  SettingsPush(Fs,TSF_SAME_SONG); //See $LK,"SettingsPush","MN:SettingsPush"$
  try {
    Fs->win_inhibit=WIF_ALL-WIF_GRAB_SCROLL-WIF_MENU;
    WordStat;
    WinMax;
    WinBorder;
    Preempt(OFF);
    Init;
    LtfCursor;
    LtfClear;
    Fs->draw_it=&DrawIt;
    Fs->animate_task=Spawn(&AnimateTask,NULL,"Animate",Fs);
    while (TRUE) {
      while (ScanKey(&ch,&sc))
	switch (ch) {
	  case 0:
	    switch (sc.u8[0]) {
	      case SC_CURSOR_LEFT:
		�-=�/100;
		break;
	      case SC_CURSOR_RIGHT:
		�+=�/100;
		break;
	      case SC_CURSOR_UP:
		speed+=3;
		break;
	      case SC_CURSOR_DOWN:
		speed-=3;
		break;
	    }
	    break;
	  case CH_NEW_LINE:
	    Init;
	    break;
	  case CH_SHIFT_ESC:
	  case CH_ESC:
	    goto vr_done;
	}
      WinSync;
    }
vr_done: //Don't goto out of try
  } catch
    Fs->catch_except=TRUE;
  SettingsPop(Fs,TSF_SAME_SONG);
  MenuPop;
}

Varoom;
           �            ����    ����    ����   ��������������������    ������������   ����   ����   ������������    ����
   ����   ����   ����       ��������   ����   ����   ����            ������������   ����������������������������           �         ��������   ��������       ������������      ����   ����   ����   ����   ����"   ����   ����$   ����   ����$   ��������������������������������������������   ����"   ����&   ����   ����   ����                   
         ����   ����         ����   ����   ����   ����   ����   ����   ����   ����   ����   ����   ����   ����   ����   ����
   ����    ����   ����   ����   ����   ����   ����   ����   ����   ����   ����   ����           ����           �           ����    ����������������������������    ����������������������������������������������������   ��������������������
   ����
   ������������   ����   ����   ����   z���   z���   ����   ����    ����    ����   v���   v�������`�������`�����������������������������������������������������������������������������������������������������������
   ����   ����   ����   ����������������j�������j�����������������������������������������������������������
   ����   ������������    h���   ����   ��������~���   n���    ~���   ����
   ����   ��������������������������������j������������������� 