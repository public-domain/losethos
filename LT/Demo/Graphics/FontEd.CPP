//After making a font, you can save it as a binary
//file with:
//	FileWrite("filename.BIZ",gr_font,256*FONT_HEIGHT);
//You can load it with:
//	U0 *myfont=FileRead("filename.BIZ");
//	gr_font=myfont; //preserve gr_font if you want to go back.

//If you want to change the system font permanently,
//save to a file with this font editor program
//and cut and paste the code into $LK,"::/LT/OSMain/Font8x8.CPZ","FI:::/LT/OSMain/Font8x8.CPZ"$.
//You will need to recompile OSMain by calling $LK,"InstallBoot","MN:InstallBoot"$().

//See $LK,"::/LT/Demo/ExtendedChars.CPZ","FI:::/LT/Demo/ExtendedChars.CPZ"$ and
//$LK,"::/LT/Demo/ScreenCodes.CPZ","FI:::/LT/Demo/ScreenCodes.CPZ"$.

#define BLOW_UP_CHAR_X	(18*FONT_WIDTH)
#define BLOW_UP_CHAR_Y	(2*FONT_HEIGHT)

U8 cur_ch;

U0 DrawIt(TaskStruct *task,GrDC *dc)
{
  I64 i,j,k,c;
  k=0;
  for (i=0;i<16;i++)
    for (j=0;j<16;j++) {
      if (k==cur_ch) {
	if (Blink(,tP(task)))
	  c=(BLUE<<4+YELLOW)<<8 + k++;
	else
	  c=(YELLOW<<4+BLUE)<<8 + k++;
      } else
	c=(BLUE<<4+WHITE)<<8 + k++;
      PlotWinU8(task,c,j,i);
    }
 
  k=0;
  for (i=0;i<FONT_HEIGHT;i++)
    for (j=0;j<FONT_WIDTH;j++) {
      if (Bt(&gr_font[FONT_HEIGHT*cur_ch],k++))
	dc->color=YELLOW;
      else
	dc->color=BLUE;
      GrRect(dc,BLOW_UP_CHAR_X+j*FONT_WIDTH,
		  BLOW_UP_CHAR_Y+i*FONT_HEIGHT,
	     FONT_WIDTH,FONT_HEIGHT);
    }
}

U0 FESave(BoolI64 pmt)
{
  U8 old_draw_it=Fs->draw_it;
  Ltf *l=LtfNew;
  I64 i,j,k=0;
  for (i=0;i<256;i++) {
    for (j=0;j<FONT_HEIGHT;j++)
      LtfPrintF(l,"0x%02X,",gr_font[k++]);
    LtfPutS(l,"\n");
  }
  Fs->draw_it=NULL;
  LtfWrite(l,pmt);
  Fs->draw_it=old_draw_it;
  LtfDel(l);
}

U0 FontEd()
{
  I64 msg_code,p1,p2,k;
  SettingsPush; //See $LK,"SettingsPush","MN:SettingsPush"$
  MenuPush(
  "File {"
  "  SaveAs(,CH_CTRLA);"
  "  Abort(,CH_SHIFT_ESC);"
  "  Exit(,CH_ESC);"
  "}");
  WordStat;
  LtfClear;
  LtfCursor;
  Fs->win_inhibit|=WIF_DBL_CLICK;
  cur_ch=0;
  try {
    Fs->draw_it=&DrawIt;
    while (TRUE) {
      switch (msg_code=GetMsg(&p1,&p2,1<<MSG_KEY_DOWN|1<<MSG_IP_L_DOWN|1<<MSG_IP_R_DOWN|1<<MSG_IP_MOVE)) {
	case MSG_KEY_DOWN:
	  switch (p1) {
	    case 0:
	      switch (p2.u8[0]) {
		case SC_CURSOR_LEFT:
		  cur_ch--;
		  break;
		case SC_CURSOR_RIGHT:
		  cur_ch++;
		  break;
		case SC_CURSOR_UP:
		  cur_ch-=16;
		  break;
		case SC_CURSOR_DOWN:
		  cur_ch+=16;
		  break;

	      }
	      break;
	      goto fe_done;
	    case CH_CTRLA:
	      FESave(TRUE);
	      break;
	    case CH_ESC:
	      FESave(FALSE);
	    case CH_SHIFT_ESC:
	      goto fe_done;
	    default:
	      cur_ch=p1;
	  }
	  break;
	case MSG_IP_L_DOWN:
	case MSG_IP_R_DOWN:
	  if (0<=p1<FONT_WIDTH*16 &&
	      0<=p2<FONT_HEIGHT*16) {
	    cur_ch=(p2/FONT_HEIGHT)*16+p1/FONT_WIDTH;
	    break;
	  } //fall through
	case MSG_IP_MOVE:
	  k=((p2-BLOW_UP_CHAR_Y)/FONT_HEIGHT)*FONT_WIDTH+
	    (p1-BLOW_UP_CHAR_X)/FONT_WIDTH;
	  if (0<=k<FONT_WIDTH*FONT_HEIGHT) {
	    if (ip_lb||msg_code==MSG_IP_L_DOWN)
	      Bts(&gr_font[FONT_HEIGHT*cur_ch],k);
	    if (ip_rb||msg_code==MSG_IP_R_DOWN)
	      Btr(&gr_font[FONT_HEIGHT*cur_ch],k);
	  }
	  break;
      }
    }
fe_done:
  } catch
    CatchAll;
  MenuPop;
  SettingsPop;
}

FontEd;
