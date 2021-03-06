#define LASER_MAX_TEMP		30.0
#define LASER_THRESHOLD_TEMP	10.0
#define MASS_MAX_TEMP		5.0

class CtrlPanelState
{
  I64 bullet_radius,fuse_time;
  F64 laser_temperature;
  BoolI8 spacewalk;
} ctrl_panel;


#define CTRL_PANEL_SPACING 33
#define CTRL_PANEL_RANGE   30
#define CTRL_PANEL_BORDER  2
#define CTRL_PANEL_BUTTON_SIZE	12

#define TMP_Y	(+2)

U0 DrawCtrlPanelCtrl(GrDC *dc,Ctrl *c)
{
  CtrlPanelState *s=c->state;
  F64 d,d_range;

  dc->color=LTRED;
  GrRect(dc, c->left,c->top,CTRL_PANEL_SPACING*3+2,CTRL_PANEL_SPACING*2+CTRL_PANEL_RANGE+CTRL_PANEL_BUTTON_SIZE);
  dc->color=DKGRAY;
  GrRect(dc, c->left+CTRL_PANEL_BORDER,c->top+CTRL_PANEL_BORDER,
  CTRL_PANEL_SPACING*3+2-2*CTRL_PANEL_BORDER,
      CTRL_PANEL_SPACING*2+CTRL_PANEL_RANGE+CTRL_PANEL_BUTTON_SIZE-2*CTRL_PANEL_BORDER);
  dc->color=BLACK;
  GrLine(dc,c->left+CTRL_PANEL_SPACING,(c->top+CTRL_PANEL_BUTTON_SIZE)+CTRL_PANEL_SPACING,
      c->left+CTRL_PANEL_SPACING,(c->top+CTRL_PANEL_BUTTON_SIZE)+CTRL_PANEL_SPACING+CTRL_PANEL_RANGE-1);
  GrLine(dc,c->left+2*CTRL_PANEL_SPACING+1,(c->top+CTRL_PANEL_BUTTON_SIZE)+CTRL_PANEL_SPACING,
      c->left+2*CTRL_PANEL_SPACING+1,(c->top+CTRL_PANEL_BUTTON_SIZE)+CTRL_PANEL_SPACING+CTRL_PANEL_RANGE-1);

  dc->color=BLACK;
  GrRect(dc,(c->left+c->right+1)/2-9*FONT_WIDTH/2-3,
      c->top+CTRL_PANEL_BORDER+(CTRL_PANEL_SPACING-FONT_HEIGHT)/2,
      9*FONT_WIDTH+6,FONT_HEIGHT+6);

  if (s->spacewalk)
    dc->color=YELLOW;
  else
    dc->color=DKGRAY;
  GrRect(dc,(c->left+c->right+1)/2-9*FONT_WIDTH/2-2,
      c->top+CTRL_PANEL_BORDER+1+(CTRL_PANEL_SPACING-FONT_HEIGHT)/2,
      9*FONT_WIDTH+4,FONT_HEIGHT+4);

  dc->color=RED;
  GrPutS(dc,(c->left+c->right+1)/2-9*FONT_WIDTH/2,
      c->top+CTRL_PANEL_BORDER+3+(CTRL_PANEL_SPACING-FONT_HEIGHT)/2,"SpaceWalk");
  GrPutS(dc,c->left+CTRL_PANEL_SPACING-4*FONT_WIDTH/2,
      (c->top+CTRL_PANEL_BUTTON_SIZE)+CTRL_PANEL_SPACING-FONT_HEIGHT-2,"Size");
  GrPutS(dc,c->left+2*CTRL_PANEL_SPACING+1-4*FONT_WIDTH/2,
      (c->top+CTRL_PANEL_BUTTON_SIZE)+CTRL_PANEL_SPACING-FONT_HEIGHT-2,"Fuse");
  dc->color=LTRED;
  GrPrintF(dc,c->left+CTRL_PANEL_SPACING-FONT_WIDTH/2,
      (c->top+CTRL_PANEL_BUTTON_SIZE)+CTRL_PANEL_SPACING+CTRL_PANEL_RANGE+3,
      "%d",s->bullet_radius*10/CTRL_PANEL_RANGE);
  GrPrintF(dc,c->left+2*CTRL_PANEL_SPACING+1-FONT_WIDTH/2,
      (c->top+CTRL_PANEL_BUTTON_SIZE)+CTRL_PANEL_SPACING+CTRL_PANEL_RANGE+3,
      "%d",s->fuse_time*10/CTRL_PANEL_RANGE);
  GrRect(dc,c->left+CTRL_PANEL_SPACING-3,	 (c->top+CTRL_PANEL_BUTTON_SIZE)+CTRL_PANEL_SPACING+CTRL_PANEL_RANGE-1-s->bullet_radius-2 ,7,5);
  GrRect(dc,c->left+2*CTRL_PANEL_SPACING+1-3,(c->top+CTRL_PANEL_BUTTON_SIZE)+CTRL_PANEL_SPACING+CTRL_PANEL_RANGE-1-s->fuse_time-2,7,5);
  dc->color=YELLOW;
  GrRect(dc,c->left+CTRL_PANEL_SPACING-2,	 (c->top+CTRL_PANEL_BUTTON_SIZE)+CTRL_PANEL_SPACING+CTRL_PANEL_RANGE-1-s->bullet_radius-1 ,5,3);
  GrRect(dc,c->left+2*CTRL_PANEL_SPACING+1-2,(c->top+CTRL_PANEL_BUTTON_SIZE)+CTRL_PANEL_SPACING+CTRL_PANEL_RANGE-1-s->fuse_time-1,5,3);

  d_range=c->right-c->left-5-5;
  d=s->laser_temperature*d_range/LASER_MAX_TEMP;
  dc->color=BLACK;
  GrRect(dc,c->left+5-1,c->bottom+TMP_Y-2,d_range+2,2+1+1);
  if (d>1.0) {
    dc->color=GREEN;
    GrRect(dc,c->left+5,c->bottom+TMP_Y-1,Min(d,d_range/3),2);
    if (d>=d_range/3) {
      dc->color=YELLOW;
      GrRect(dc,c->left+5+d_range/3,c->bottom+TMP_Y-1,Min(d,2*d_range/3)-d_range/3,2);
      if (d>=2*d_range/3) {
	dc->color=RED;
	GrRect(dc,c->left+5+2*d_range/3,c->bottom+TMP_Y-1,d-2*d_range/3,2);
      }
    }
  }
}

U0 UpdateDerivedCtrlPanelCtrl(Ctrl *c)
{
  CtrlPanelState *s=c->state;
  c->left=c->win_task->win_pixel_width-(CTRL_PANEL_SPACING*3+2);
  c->right=c->left+CTRL_PANEL_SPACING*3+2;
  c->top=c->win_task->win_pixel_height-(CTRL_PANEL_SPACING*2+CTRL_PANEL_RANGE+CTRL_PANEL_BUTTON_SIZE);
  c->bottom=c->top+CTRL_PANEL_SPACING*2+CTRL_PANEL_RANGE;

  s->bullet_radius =LimitI64(s->bullet_radius,0,CTRL_PANEL_RANGE-1);
  s->fuse_time=LimitI64(s->fuse_time,0,CTRL_PANEL_RANGE-1);
}

U0 LeftClickCtrlPanel(Ctrl *c,I64 x,I64 y,BoolI8 down)
{
  nounusedwarn down;
  CtrlPanelState *s=c->state;
  if (y<c->top+CTRL_PANEL_BORDER+(CTRL_PANEL_SPACING-FONT_HEIGHT)/2+FONT_HEIGHT+6)
    s->spacewalk=TRUE;
  else {
    if (x<(c->right+c->left)/2)
      s->bullet_radius=CTRL_PANEL_RANGE-1-(y-(c->top+CTRL_PANEL_SPACING+CTRL_PANEL_BUTTON_SIZE));
    else
      s->fuse_time=CTRL_PANEL_RANGE-1-(y-(c->top+CTRL_PANEL_SPACING+CTRL_PANEL_BUTTON_SIZE));
  }
  if (c->update_derived_vals)
    (*c->update_derived_vals)(c);
}

Ctrl *CtrlPanelNew()
{
  BoolI8 old_preempt;
  Ctrl *c=CAlloc(sizeof(Ctrl));
  MemSet(&ctrl_panel,0,sizeof(ctrl_panel));
  c->win_task=Fs;
  c->flags=CTRLF_SHOW|CTRLF_CAPTURE_LEFT_IP;
  c->type=CTRLT_GENERIC;
  c->state=&ctrl_panel;
  c->draw_it=&DrawCtrlPanelCtrl;
  c->left_click=&LeftClickCtrlPanel;
  c->update_derived_vals=&UpdateDerivedCtrlPanelCtrl;
  old_preempt=Preempt(OFF);
  InsQue(c,Fs->last_ctrl);
  TaskDerivedValsUpdate;
  Preempt(old_preempt);
  return c;
}

U0 CtrlPanelDel(Ctrl *c)
{
  BoolI8 old_preempt=Preempt(OFF);
  RemQue(c);
  Preempt(old_preempt);
  Free(c);
}
