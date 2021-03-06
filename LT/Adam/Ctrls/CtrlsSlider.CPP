class MgdMCSlider
{
  MgdMCSlider *next,*last;
  U8 *name;
  I64 num;
};

class MgdMC
{
  MgdMCSlider *next_slider,*last_slider;
  I64 num_sliders,range,spacing,border;
  U8 *glbl_name;
};


MgdMC *PmtSliderCtrl()
{
  MgdMC *result=CAlloc(sizeof(MgdMC));
  MgdMCSlider *temps;
  U8 *st,pmt[136];

  "$$FG,PURPLE$$Slider Ctrl$$FG$$\n\n";

  result->glbl_name=PmtStr("Glbl Struct Name (ENTER for No Glbl):");
  result->range=PmtI64	("Range   (%4d):",30);
  result->spacing=PmtI64("Spacing (%4d):",20);
  result->border=PmtI64 ("Border  (%4d):",2);

  result->next_slider=result->last_slider=&result->next_slider;
  while (TRUE) {
    SPrintF(pmt,"Slider #%d Name:",result->num_sliders+1);
    st=PmtStr(pmt);
    if (!*st) {
      Free(st);
      break;
    }
    temps=CAlloc(sizeof(MgdMCSlider));
    temps->name=st;
    InsQue(temps,result->last_slider);
    temps->num=result->num_sliders++;
  }
  return result;
}

U0 MgdSliderCtrl(Ltf *l)
{
  TaskStruct *task;
  MgdMC *m=NULL;
  MgdMCSlider *temps;
  I64 i;

  LtfUnlock(l);
  LtfRecalc(l);
  LtfCaptureUndo(l,TRUE);

  task=SpawnUser;
  XTalk(task,"MgdMC **_m=0x%X;*_m=PmtSliderCtrl;\n",&m);
  while (!m)
    Yield;

  LtfPrintF(l,
"#define SLIDER_RANGE	%d
#define SLIDER_SPACING %d
#define SLIDER_BORDER  %d

class SliderState
{
",m->range,m->spacing,m->border);


  temps=m->next_slider;
  while (temps!=&m->next_slider) {
    LtfPrintF(l,"I64 %s;\n",temps->name);
    temps=temps->next;
  }
  if (*m->glbl_name)
    LtfPrintF(l,"} %s;\n",m->glbl_name);
  else
    LtfPutS(l,"};\n");


  LtfPrintF(l,
"
U0 DrawSliderCtrl(GrDC *dc,Ctrl *c)
{
SliderState *s=c->state;

dc->color=LTRED;
GrRect(dc, c->left,c->top,%d*SLIDER_SPACING+%d,SLIDER_SPACING*2+SLIDER_RANGE);
dc->color=BLUE;
GrRect(dc, c->left+SLIDER_BORDER,c->top+SLIDER_BORDER,
%d*SLIDER_SPACING+%d-2*SLIDER_BORDER,SLIDER_SPACING*2+SLIDER_RANGE-2*SLIDER_BORDER);
dc->color=BLACK;
",m->num_sliders+1,m->num_sliders,m->num_sliders+1,m->num_sliders);

  for (i=0;i<m->num_sliders;i++)
    LtfPrintF(l,
"GrLine(dc,c->left+%d*SLIDER_SPACING+%d,c->top+SLIDER_SPACING,
	      c->left+%d*SLIDER_SPACING+%d,c->top+SLIDER_SPACING+SLIDER_RANGE-1);
",i+1,i,i+1,i);

  LtfPutS(l,"dc->color=LTRED;\n");

  temps=m->next_slider;
  while (temps!=&m->next_slider) {
    LtfPrintF(l,
"GrPrintF(dc,c->left+%d*SLIDER_SPACING+%d-FONT_WIDTH/2,
c->top+SLIDER_SPACING+SLIDER_RANGE+3,
\"%%d\",s->%s*10/SLIDER_RANGE);\n",
      temps->num+1,temps->num,temps->name);
    temps=temps->next;
  }

  temps=m->next_slider;
  while (temps!=&m->next_slider) {
    LtfPrintF(l,"GrRect(dc,c->left+%d*SLIDER_SPACING+%d-3,c->top+SLIDER_SPACING+SLIDER_RANGE-1-s->%s-2,7,5);\n",
      temps->num+1,temps->num,temps->name);
    temps=temps->next;
  }
  LtfPutS(l,"dc->color=YELLOW;\n");

  temps=m->next_slider;
  while (temps!=&m->next_slider) {
    LtfPrintF(l,"GrRect(dc,c->left+%d*SLIDER_SPACING+%d-2,c->top+SLIDER_SPACING+SLIDER_RANGE-1-s->%s-1,5,3);\n",
      temps->num+1,temps->num,temps->name);
    temps=temps->next;
  }
  LtfPrintF(l,
"}

U0 UpdateDerivedSliderCtrl(Ctrl *c)
{
  SliderState *s=c->state;
  c->left=c->win_task->win_pixel_width/2-(SLIDER_SPACING*3+2)/2;
  c->right=c->left+%d*SLIDER_SPACING+%d;
  c->top=c->win_task->win_pixel_height/2-(SLIDER_SPACING*2+SLIDER_RANGE)/2;
  c->bottom=c->top+SLIDER_SPACING*2+SLIDER_RANGE;
",m->num_sliders+1,m->num_sliders);

  temps=m->next_slider;
  while (temps!=&m->next_slider) {
    LtfPrintF(l,"s->%s=LimitI64(s->%s,0,SLIDER_RANGE-1);\n",temps->name,temps->name);
    temps=temps->next;
  }
  LtfPutS(l,
"}

U0 LeftClickSlider(Ctrl *c,I64 x,I64 y,BoolI8 down)
{
nounusedwarn down;
SliderState *s=c->state;
");

  temps=m->next_slider;
  while (temps!=&m->next_slider) {
    LtfPutS(l,"");
    if (temps!=m->next_slider)
      LtfPutS(l,"else");
    if (temps->next==&m->next_slider)
      LtfPutS(l,"\n");
    else
      LtfPrintF(l,"if (x<c->left+%d*SLIDER_SPACING+%d+SLIDER_SPACING/2)\n",
	temps->num+1,temps->num);
    LtfPrintF(l,"s->%s=SLIDER_RANGE-1-(y-(c->top+SLIDER_SPACING));\n",
       temps->name);
    temps=temps->next;
  }
 
  LtfPutS(l,
"if (c->update_derived_vals)
(*c->update_derived_vals)(c);
}

Ctrl *SliderNew()
{
BoolI8 old_preempt;
Ctrl *c=CAlloc(sizeof(Ctrl));
c->win_task=Fs;
c->flags=CTRLF_SHOW|CTRLF_CAPTURE_LEFT_IP;
c->type=CTRLT_GENERIC;
");
  if (*m->glbl_name)
    LtfPrintF(l,"c->state=&%s;\n"
		"MemSet(&%s,0,sizeof(SliderState));\n",m->glbl_name,m->glbl_name);
  else
    LtfPutS(l,	"c->state=CAlloc(sizeof(SliderState));\n");
  LtfPutS(l,
"c->draw_it=&DrawSliderCtrl;
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
");
  if (!*m->glbl_name)
    LtfPutS(l,"Free(c->state);\n");
  LtfPutS(l,
"Free(c);
}
");

  Kill(task);  //This frees memory.

  AcctOneTimePopUp(ARf_MANAGE_SLIDER,
    "Modify the code to your heart's content.\n"
    "The code is no longer managed.\n");

  LtfLock(l);
}
