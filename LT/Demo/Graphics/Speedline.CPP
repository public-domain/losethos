//This is a drawing program

public U0 SpeedLineDemo()
{
  I64 msg_code,cnt,x1,y1,x2,y2,p1,p2,color=BLACK;
  GrDC *dc=GrDCAlias;
  dc->color=ROP_EQU+color;

  SettingsPush; //See $LK,"SettingsPush","MN:SettingsPush"$
  Fs->win_inhibit=WIF_ALL-WIF_BORDER;

  WinMax;
  WordStat;
  GrDCFill;
  "$$CL$$";
  do {
    msg_code=GetMsg(&p1,&p2,
      1<<MSG_KEY_DOWN+1<<MSG_IP_L_DOWN+
      1<<MSG_IP_R_UP);
    switch (msg_code) {
      case MSG_IP_R_UP:
	color=PopUpColor;
	if (color>=0)
	  dc->color=ROP_EQU+color;
	break;
      case MSG_IP_L_DOWN:
	x1=p1; y1=p2;
	x2=p1; y2=p2;
	cnt=0;
	while (msg_code!=MSG_IP_L_UP) {
	  GrSpeedLine(dc,x1,y1,x2,y2,ip_speed);
	  msg_code=GetMsg(&p1,&p2,
	    1<<MSG_IP_L_UP+1<<MSG_IP_MOVE);
	  x1=x2; y1=y2;
	  x2=p1; y2=p2;
	}
	GrLine3(dc,x1,y1,0,x2,y2,0);
	break;
      case MSG_KEY_DOWN:
	break;
    }
  } while (msg_code!=MSG_KEY_DOWN || !p1);
  GrDCFill;
  GrDCDel(dc);
  SettingsPop;
}

SpeedLineDemo;	//Execute when #included
