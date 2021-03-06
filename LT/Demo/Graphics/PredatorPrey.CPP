//This is a classic 2nd order nonlinear ordinary differential equation

U64 mp_done_flags;

#define OX	10
#define OY	GR_HEIGHT/2-FONT_HEIGHT

#define	MICRO_STEP	0.000001

class State
{
  F64 wabbits,hawks,
      d_wabbits,d_hawks; //Technically, these are not part of the state.
};

U0 Steps(State *s)
{
  I64 i;
  for (i=0;i<10000;i++) {
    s->d_wabbits=s->wabbits*(1.0-0.10*s->hawks);
    s->d_hawks	=-s->hawks *(1.0-0.01*s->wabbits);
    s->hawks  +=s->d_hawks  *MICRO_STEP;
    s->wabbits+=s->d_wabbits*MICRO_STEP;
  }
}

U0 PlotTrajectory(TaskStruct *parent)
{
  I64 i;
  GrDC *dc=GrDCAlias(gr_dc2,parent);
  State s;

  MemSet(&s,0,sizeof(State));
  s.wabbits=RandU16%100 +10;
  s.hawks  =RandU16%10	+1;

  for (i=0;i<1000;i++) {
    dc->color=LTGREEN;
    GrPlot(dc,s.wabbits+OX,OY-s.d_wabbits);
    dc->color=LTRED;
    GrPlot(dc,s.hawks  +OX,OY-s.d_hawks);
    Steps(&s);
  }
  GrDCDel(dc);
  LBts(&mp_done_flags,Gs->num);
}

U0 PredatorPrey()
{
  I64 i;
  GrDC *dc=GrDCAlias;

  PopUpOk("This will plot multiple predator-prey\n"
	"trajectories.It restarts many times\n"
	"with different, random, initial populations.\n");
  SettingsPush; //See $LK,"SettingsPush","MN:SettingsPush"$
  try {
    LtfClear;
    WinMax;
    WinBorder;
    WinSync;
    WordStat;

    dc->color=BLACK;
    GrLine(dc,OX,0,OX,GR_HEIGHT-FONT_HEIGHT-1);
    GrLine(dc,0,OY,GR_WIDTH-1,OY);
    while (!ScanChar) {
      mp_done_flags=0;
      for (i=1;i<mp_cnt;i++)
	MPJob(&PlotTrajectory,Fs,1<<MPCf_FREE_ON_COMPLETE,1<<i);
      PlotTrajectory(Fs);
      while (mp_done_flags!=1<<mp_cnt-1)
	Yield;
    }
  } catch
    CatchAll;
  SettingsPop;
  GrDCFill(dc);
  GrDCDel(dc);
}

PredatorPrey;
