/*
This was done with <CTRL-O> managed code
that was modified.

This is an advanced demo that shows
that you can place ctrls on the
wall paper.

See $LK,"::/LT/Demo/Graphics/SliderCtrl.CPZ","FI:::/LT/Demo/Graphics/SliderCtrl.CPZ"$
and $LK,"::/LT/Demo/Graphics/MyWallPaper.APZ","FI:::/LT/Demo/Graphics/MyWallPaper.APZ"$
before messing with this program.

It must be "Adam Included".
*/

#define SLIDER_RANGE   30
#define SLIDER_SPACING 20
#define SLIDER_BORDER  2

class SliderState
{
  I64 left_pos;
  I64 right_pos;
};

U0 DrawSliderCtrl(GrDC *dc,Ctrl *c)
{
  SliderState *s=c->state;

  dc->color=LTRED;
  GrRect(dc, c->left,c->top,SLIDER_SPACING*3+2,SLIDER_SPACING*2+SLIDER_RANGE);
  dc->color=BLUE;
  GrRect(dc, c->left+SLIDER_BORDER,c->top+SLIDER_BORDER,
    SLIDER_SPACING*3+2-2*SLIDER_BORDER,SLIDER_SPACING*2+SLIDER_RANGE-2*SLIDER_BORDER);
  dc->color=BLACK;
  GrLine(dc,c->left+1*SLIDER_SPACING+0,c->top+SLIDER_SPACING,
	      c->left+1*SLIDER_SPACING+0,c->top+SLIDER_SPACING+SLIDER_RANGE-1);
  GrLine(dc,c->left+2*SLIDER_SPACING+1,c->top+SLIDER_SPACING,
	      c->left+2*SLIDER_SPACING+1,c->top+SLIDER_SPACING+SLIDER_RANGE-1);
  dc->color=LTRED;
  GrPrintF(dc,c->left+1*SLIDER_SPACING+0-FONT_WIDTH/2,
    c->top+SLIDER_SPACING+SLIDER_RANGE+3,
    "%d",s->left_pos*10/SLIDER_RANGE);
  GrPrintF(dc,c->left+2*SLIDER_SPACING+1-FONT_WIDTH/2,
    c->top+SLIDER_SPACING+SLIDER_RANGE+3,
    "%d",s->right_pos*10/SLIDER_RANGE);
  GrRect(dc,c->left+1*SLIDER_SPACING+0-3,c->top+SLIDER_SPACING+SLIDER_RANGE-1-s->left_pos-2,7,5);
  GrRect(dc,c->left+2*SLIDER_SPACING+1-3,c->top+SLIDER_SPACING+SLIDER_RANGE-1-s->right_pos-2,7,5);
  dc->color=YELLOW;
  GrRect(dc,c->left+1*SLIDER_SPACING+0-2,c->top+SLIDER_SPACING+SLIDER_RANGE-1-s->left_pos-1,5,3);
  GrRect(dc,c->left+2*SLIDER_SPACING+1-2,c->top+SLIDER_SPACING+SLIDER_RANGE-1-s->right_pos-1,5,3);
}

U0 UpdateDerivedSliderCtrl(Ctrl *c)
{
  SliderState *s=c->state;
  c->left=c->win_task->win_pixel_width/2-(SLIDER_SPACING*3+2)/2;
  c->right=c->left+3*SLIDER_SPACING+2;
  c->top=c->win_task->win_pixel_height/2-(SLIDER_SPACING*2+SLIDER_RANGE)/2;
  c->bottom=c->top+SLIDER_SPACING*2+SLIDER_RANGE;
  s->left_pos=LimitI64(s->left_pos,0,SLIDER_RANGE-1);
  s->right_pos=LimitI64(s->right_pos,0,SLIDER_RANGE-1);
}

U0 LeftClickSlider(Ctrl *c,I64 x,I64 y,BoolI8 down)
{
  nounusedwarn down;
  SliderState *s=c->state;
  if (x<c->left+1*SLIDER_SPACING+0+SLIDER_SPACING/2)
    s->left_pos=SLIDER_RANGE-1-(y-(c->top+SLIDER_SPACING));
  else
    s->right_pos=SLIDER_RANGE-1-(y-(c->top+SLIDER_SPACING));
  if (c->update_derived_vals)
    (*c->update_derived_vals)(c);
}

Ctrl *SliderNew()
{
  BoolI8 old_preempt;
  Ctrl *c=ACAlloc(sizeof(Ctrl));
  c->win_task=sys_winmgr_task;
  c->flags=CTRLF_SHOW|CTRLF_CAPTURE_LEFT_IP;
  c->type=CTRLT_GENERIC;
  c->state=ACAlloc(sizeof(SliderState));
  c->draw_it=&DrawSliderCtrl;
  c->left_click=&LeftClickSlider;
  c->update_derived_vals=&UpdateDerivedSliderCtrl;
  old_preempt=Preempt(OFF);
  InsQue(c,sys_winmgr_task->last_ctrl);
  TaskDerivedValsUpdate;
  Preempt(old_preempt);
  return c;
}

U0 SliderDel(Ctrl *c)
{
  BoolI8 old_preempt=Preempt(OFF);
  RemQue(c);
  Preempt(old_preempt);
  Free(c->state);
  Free(c);
}

SliderNew;
