/*
This demo uses gr_dc, so you must add a
draw_it routine because gr_dc is not
persistent.  (It gets erased each update by
the system.)
*/








      $PI,"<1>",1$







GrDC *dc;

U0 DrawIt(TaskStruct *task,GrDC *_dc)
{
  nounusedwarn task,_dc;
//We can change the pen width because it was not
//define in <CTRL-R>.
  dc->pen_width=2;
  GrDCDepthBufRst(dc);
  GrSpritePlot3(dc,0,0,0,$IB,"<1>",1$);
}

U0 GrSprite2()
{
  F64 �=0,�=0;
  dc=GrDCAlias(gr_dc,Fs);

  SettingsPush; //See $LK,"SettingsPush","MN:SettingsPush"$
  Fs->draw_it=&DrawIt;
  Preempt(OFF); //We don't want update during matrix change
  dc->x=200;
  dc->y=200;
  dc->z=GR_Z_ALL;
  dc->flags|=DCF_TRANSFORMATION;
  GrDCAllocDepthBuf(dc);
  while (!ScanChar) {
    GrIdentEqu(dc->r);
    GrRotXEqu(dc->r,�);
    GrRotZEqu(dc->r,�);
    GrSetRotMat(dc,dc->r);

    Sleep(40);

    �+=2*�/100;
    �+=2*�/130;
  }
  SettingsPop;
  GrDCDel(dc);
}

GrSprite2;
           8           #   ����#   ����1   	   1   	   "      "                   *      "               ��������    2   ����        2       2   ����2   ��������2       2   2                                                                                                       