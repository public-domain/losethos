//This highlights the 4th dimension
//of the transformation matrix which
//is used for storing translations (shifts)
//in matricies so they can be combined
//with a matrix multiplication.

//Scroll bar works.

//The multiplication takes place in the
//$LK,"GrSpritePlotRotZ3b","MN:GrSpritePlotRotZ3b"$ routine.


#define SLIDER_SPACING 20
#define SLIDER_RANGE   30
#define SLIDER_BORDER  2

class SliderState
{
  I64 s1,s2,s3;
  F64 a1,a2,scale;
} s;

U0 DrawSliderCtrl(GrDC *dc,Ctrl *c)
{
  SliderState *s=c->state;

  dc->color=LTGREEN;
  GrRect(dc, c->left,c->top,SLIDER_SPACING*4+3,SLIDER_SPACING*2+SLIDER_RANGE);
  dc->color=BROWN;
  GrRect(dc, c->left+SLIDER_BORDER,c->top+SLIDER_BORDER,
    SLIDER_SPACING*4+3-2*SLIDER_BORDER,SLIDER_SPACING*2+SLIDER_RANGE-2*SLIDER_BORDER);
  dc->color=BLACK;
  GrLine(dc,c->left+SLIDER_SPACING,c->top+SLIDER_SPACING,
	      c->left+SLIDER_SPACING,c->top+SLIDER_SPACING+SLIDER_RANGE-1);
  GrLine(dc,c->left+2*SLIDER_SPACING+1,c->top+SLIDER_SPACING,
	      c->left+2*SLIDER_SPACING+1,c->top+SLIDER_SPACING+SLIDER_RANGE-1);
  GrLine(dc,c->left+3*SLIDER_SPACING+2,c->top+SLIDER_SPACING,
	      c->left+3*SLIDER_SPACING+2,c->top+SLIDER_SPACING+SLIDER_RANGE-1);

  dc->color=LTGREEN;
  GrPrintF(dc,c->left+SLIDER_SPACING-FONT_WIDTH/2,
	      c->top+SLIDER_SPACING+SLIDER_RANGE+3,
    "%d",s->s1*10/SLIDER_RANGE);
  GrPrintF(dc,c->left+2*SLIDER_SPACING+1-FONT_WIDTH/2,
	      c->top+SLIDER_SPACING+SLIDER_RANGE+3,
    "%d",s->s2*10/SLIDER_RANGE);
  GrPrintF(dc,c->left+3*SLIDER_SPACING+2-FONT_WIDTH/2,
	      c->top+SLIDER_SPACING+SLIDER_RANGE+3,
    "%d",s->s3*10/SLIDER_RANGE);
  GrRect(dc,c->left+SLIDER_SPACING-3,	 c->top+SLIDER_SPACING+SLIDER_RANGE-1-s->s1-2 ,7,5);
  GrRect(dc,c->left+2*SLIDER_SPACING+1-3,c->top+SLIDER_SPACING+SLIDER_RANGE-1-s->s2-2,7,5);
  GrRect(dc,c->left+3*SLIDER_SPACING+2-3,c->top+SLIDER_SPACING+SLIDER_RANGE-1-s->s3-2,7,5);
  dc->color=GREEN;
  GrRect(dc,c->left+SLIDER_SPACING-2,	 c->top+SLIDER_SPACING+SLIDER_RANGE-1-s->s1-1 ,5,3);
  GrRect(dc,c->left+2*SLIDER_SPACING+1-2,c->top+SLIDER_SPACING+SLIDER_RANGE-1-s->s2-1,5,3);
  GrRect(dc,c->left+3*SLIDER_SPACING+2-2,c->top+SLIDER_SPACING+SLIDER_RANGE-1-s->s3-1,5,3);
}

U0 UpdateDerivedSliderCtrl(Ctrl *c)
{
  SliderState *s=c->state;
  c->left=c->win_task->win_pixel_width/2-(SLIDER_SPACING*4+3)/2;
  c->right=c->left+SLIDER_SPACING*4+3;
  c->top=c->win_task->win_pixel_height/2-(SLIDER_SPACING*2+SLIDER_RANGE)/2;
  c->bottom=c->top+SLIDER_SPACING*2+SLIDER_RANGE;
  s->s1=LimitI64(s->s1,0,SLIDER_RANGE-1);
  s->s2=LimitI64(s->s2,0,SLIDER_RANGE-1);
  s->s3=LimitI64(s->s3,1,SLIDER_RANGE-1);
  s->a1=�/2.0*s->s1/SLIDER_RANGE;
  s->a2=1.0*(s->s2-SLIDER_RANGE/2)/SLIDER_RANGE;
  s->scale=2.0*s->s3/SLIDER_RANGE;
}

U0 LeftClickSlider(Ctrl *c,I64 x,I64 y,BoolI8 down)
{
  nounusedwarn down;
  SliderState *s=c->state;
  if (x<c->left+(c->right-c->left)/3)
    s->s1=SLIDER_RANGE-1-(y-(c->top+SLIDER_SPACING));
  else if (x<c->left+2*(c->right-c->left)/3)
    s->s2=SLIDER_RANGE-1-(y-(c->top+SLIDER_SPACING));
  else
    s->s3=SLIDER_RANGE-1-(y-(c->top+SLIDER_SPACING));
  if (c->update_derived_vals)
    (*c->update_derived_vals)(c);
}

Ctrl *SliderNew()
{
  BoolI8 old_preempt;
  Ctrl *c=CAlloc(sizeof(Ctrl));
  c->win_task=Fs;
  c->flags=CTRLF_SHOW|CTRLF_CAPTURE_LEFT_IP;
  c->type=CTRLT_GENERIC;
  c->state=&s;
  MemSet(&s,0,sizeof(s));
  c->draw_it=&DrawSliderCtrl;
  c->left_click=&LeftClickSlider;
  c->update_derived_vals=&UpdateDerivedSliderCtrl;
  old_preempt=Preempt(OFF);
  InsQue(c,Fs->last_ctrl);
  TaskDerivedValsUpdate;
  Preempt(old_preempt);
  return c;
}

U0 SliderDel(Ctrl *c)
{
  BoolI8 old_preempt=Preempt(OFF);
  RemQue(c);
  Preempt(old_preempt);
  Free(c);
}





$PI,"Tree",1$



#define MAP_HEIGHT	2048
#define MAP_WIDTH	2048

#define NUM_TREES	256

I64 tree_x[NUM_TREES],tree_y[NUM_TREES];

class MPCtrl {
  I64 mp_cnt;
  I64 r[16];
  F64 a;
} mp;


U0 MPDrawIt(TaskStruct *task)
{
  GrDC *dc=GrDCAlias(gr_dc,task);
  I64 i,lo=Gs->num*NUM_TREES/mp.mp_cnt,hi=(Gs->num+1)*NUM_TREES/mp.mp_cnt;
  MemCpy(dc->r,mp.r,sizeof(I64)*16);
  dc->flags|=DCF_TRANSFORMATION;
  for (i=lo;i<hi;i++)
    GrSpritePlotRotZ3b(dc,tree_x[i],
		     tree_y[i],0,$IB,"<tree>",1$,mp.a);
  GrDCDel(dc);
}

U0 DrawIt(TaskStruct *task,GrDC *dc)
{
  I64 i,h,v;
  task->horz_scroll.min=-s.scale*MAP_HEIGHT*Sin(s.a1);
  task->horz_scroll.max=s.scale*MAP_WIDTH*Cos(s.a1)-task->win_pixel_width;
  task->vert_scroll.min=0;
  task->vert_scroll.max=s.scale*(MAP_HEIGHT*Cos(s.a1)+MAP_WIDTH*Sin(s.a1))-task->win_pixel_height;
  TaskDerivedValsUpdate(task);

  h=task->horz_scroll.pos;
  v=task->vert_scroll.pos;

  GrRotZEqu(dc->r,s.a1);
  GrScaleMatEqu(dc->r,s.scale);
  GrSetRotMat(dc,dc->r);
  GrSetTranslation(dc->r,-h,-v,0);
  MemCpy(mp.r,dc->r,sizeof(I64)*16);
  mp.a=s.a2;

  for (i=1;i<mp.mp_cnt;i++)
    MPJob(&MPDrawIt,task,1<<MPCf_FREE_ON_COMPLETE,1<<i);
  MPDrawIt(task);

  dc->flags|=DCF_TRANSFORMATION;
  dc->color=BROWN;
  dc->pen_width=4;
  GrLine3(dc,2,2,0,MAP_WIDTH-3,2,0);
  GrLine3(dc,2,MAP_HEIGHT-3,0,MAP_WIDTH-3,MAP_HEIGHT-3,0);
  GrLine3(dc,2,2,0,2,MAP_HEIGHT-3,0);
  GrLine3(dc,MAP_WIDTH-3,2,0,MAP_WIDTH-3,MAP_HEIGHT-3,0);
}


U0 Init()
{
  I64 i;
  for (i=0;i<NUM_TREES;i++) {
    tree_x[i]=RandU16%MAP_WIDTH;
    tree_y[i]=RandU16%MAP_HEIGHT;
  }
}

U0 TransformDemo(I64 _mp_cnt=mp_cnt)
{
  SettingsPush; //See $LK,"SettingsPush","MN:SettingsPush"$
  Init;
  Fs->win_inhibit=WIF_ALL-WIF_BORDER-WIF_GRAB_SCROLL-WIF_MENU-WIF_CTRLS;
  LtfCursor;
  LtfClear;
  LtfScroll;
  Fs->draw_it=&DrawIt;
  WinBorder(ON);

  Fs->horz_scroll.pos=0;
  Fs->vert_scroll.pos=0;
  Ctrl *c=SliderNew;
  s.s1=0;
  s.s2=SLIDER_RANGE/2;
  s.s3=SLIDER_RANGE/2;

  MemSet(&mp,0,sizeof(MPCtrl));
  mp.mp_cnt=_mp_cnt;

  View;
  SliderDel(c);
  SettingsPop;
  //If you care, you might want to
  //save the original state of the
  //scroll bars.
}

TransformDemo;
           j     (   O   �������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
�������������������������������������


������������������������������������


����������������������������������


��������������������������������




�������������������������������


������������������������������




�����������������������������



����������������������������


��������������������������



������������������������



����������������������



���������������������









������������������������












�������������������������


������������������������


���������������������






������������������






������������������



����������������������




�������������������������












�������������������������


������������������������


���������������������






������������������






������������������



����������������������




��������������������������






���������������������������










��������������������������
������������������������


����������������������


�������������������






����������������




����������������






������������������



���������������������











�������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������� 