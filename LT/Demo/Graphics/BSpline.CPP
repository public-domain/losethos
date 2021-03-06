U0 BSplineDemo()
{
  GrDC *b=GrDCAlias;
  I64 p1,p2,i=0,msg_code;
  P3I32 c[64];

  SettingsPush;
  Fs->win_inhibit=WIF_ALL-WIF_BORDER;

  "$$CL$$Select ctrl points with left mouse.Right when finished.\n";
  GrDCFill;
  do {
    msg_code=GetMsg(&p1,&p2,1<<MSG_IP_L_UP+1<<MSG_IP_R_UP);
    if (msg_code==MSG_IP_L_UP) {
      c[i].x=p1; c[i].y=p2; c[i].z=0;
      b->color=GREEN;
      GrCircle(b,p1,p2,10);
      i++;
    }
  } while (msg_code!=MSG_IP_R_UP);
  b->color=RED;
  Gr2BSpline(b,c,i,TRUE);
  b->color=BLUE;
  Gr3BSpline(b,c,i,TRUE);
  GrDCDel(b);
  SettingsPop;

  PressAKey;
  GrDCFill;
}

BSplineDemo;
