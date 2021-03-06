$BG,11$
$PI,"<1>",1$





$PI,"<2>",2$




$PI,"<3>",3$

$BG$
#define NUM_QUAIL	128
#define SKY_HEIGHT	(0.6*GR_HEIGHT)
class QuailStruct
{
  F64 x,y,dx,dy,phase;
  BoolI8 dead,pad[7];
} q[NUM_QUAIL];

F64 t0,t_last;

U0 DrawQuail(TaskStruct *task,GrDC *dc)
{
  I64 i;
  U8 *inter;
  F64 tt,t1=tP(task)-t0;
  for (i=0;i<NUM_QUAIL;i++) {
    tt=Tri(t1+q[i].phase,1.0);
    if (q[i].dead) {
      q[i].x+=(t1-t_last)*q[i].dx;
      q[i].y+=50*(t1-t_last);
      if (q[i].y>SKY_HEIGHT) {
	q[i].y=SKY_HEIGHT;
	q[i].dx=0;
      }
      GrSpritePlot3(dc,q[i].x,q[i].y,0,$IB,"<3>",3$);
    } else {
      q[i].x+=(t1-t_last)*q[i].dx;
      q[i].y+=(t1-t_last)*q[i].dy;
      if (!(0<q[i].y<SKY_HEIGHT-20)) {
	q[i].dy=-q[i].dy;
	q[i].y+=(t1-t_last)*q[i].dy;
      }
      inter=GrSpriteInterpolate($IB,"<1>",1$,$IB,"<2>",2$,tt);
      GrSpritePlot3(dc,q[i].x,q[i].y,0,inter);
      Free(inter);
      if (q[i].x>0 && t1-t_last>10*Rand)
	q[i].dead=TRUE;
    }
  }
  t_last=t1;
}

U0 Quail()
{
  I64 i;
  for (i=0;i<NUM_QUAIL;i++) {
    q[i].x=RandI16%GR_WIDTH;
    q[i].y=RandU16%SKY_HEIGHT;
    q[i].dx=50*Rand+10;
    q[i].dy=20*(Rand-0.5);
    q[i].phase=Rand;
    q[i].dead=FALSE;
  }
  SettingsPush(Fs,TSF_SAME_SONG); //See $LK,"SettingsPush","MN:SettingsPush"$
  WinMax;

  LtfBottom;
  "$$CL$$$$BG,YELLOW$$";

  PutBibleVerse("Numbers,11:11",88);
  LtfTop;
  PopUpOk("Scroll down to finish reading.\n"
	"$$FG,GREEN$$<SHIFT-ESC>$$FG$$ when done.");
  View;

  LtfBottom;
  "$$CL$$$$BG,LTCYAN$$";
  NewLine(SKY_HEIGHT/FONT_HEIGHT);
  "$$BG,YELLOW$$";
  NewLine(3);

  t0=tP;
  t_last=0;
  Fs->draw_it=&DrawQuail;
  PressAKey;
  LtfClear;
  SettingsPop(Fs,TSF_SAME_SONG);
  "$$BG$$\n";
}
           e        ����   ����   ����   ����������������   ����   ����
   ����
   ����                                 ����            
      
      ����   ����   ������������   ����   ����   ����   ����   ����   ������������    ����   ��������������������                       ����               ����           e        ����   ����   ����   ����    ����       ����   ����                                          ����      ����   ����            ����   ����   ����   ����   ������������    ����������������������������   ����   ����   ����   ����   ����   ����   ����                                         �        ����������������������������    ����                   ����   ����   ����   ����   ����   ����   ������������               ����   ����������������������������������������   ����    ����   ����
   ����   ����������������������������������������������������������������      ����   ����   ����   ��������������������   ����   ��������������������   ����   �������������������� 