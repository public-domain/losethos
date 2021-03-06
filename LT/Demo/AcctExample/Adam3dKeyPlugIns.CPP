//Change anything you want in this
//acct file or others.

U0 FillInTime()
{
  LTDate ltdt;
  ltdt=GetCurTimeLTDate;
  "$$IV,1$$----%D %T----$$IV,0$$\n",ltdt,ltdt;
}


BoolI64 MyPutKey(U64 ch,U64 sc)
{  //ch=ASCII; sc=scancode

//See $LK,"Char","HI:Char"$ for definition of scan codes.
//See $LK,"Keyboard Handling","FI:::/LT/Doc/KeyBoard.TXZ"$

//You can customize keys.  This routine
//is called before the main editor
//key handler $LK,"LtfPutKey","MN:LtfPutKey"$().
//You can intercept any key.

//Return TRUE if you completely
//handled the key.

  U64 i,sc2=sc.u8[0];

  if (SC_F1<=sc2<=SC_F10 &&
      sc&SCF_ALT &&
      !(sc & SCF_CTRL)) {
    switch (sc2) {
      case SC_F1:
	if (sc&SCF_KEY_DESC) {
	  if (sc & SCF_SHIFT)
	    SetKeyDesc("Dollar/LTPURPLE");
	  else
	    SetKeyDesc("Dollar/PURPLE");
	} else {
	  if (sc & SCF_SHIFT)
	    "$$FG,LTPURPLE$$";
	  else
	    "$$FG,PURPLE$$";
	}
	break;
      case SC_F2:
	if (sc&SCF_KEY_DESC) {
	  if (sc & SCF_SHIFT)
	    SetKeyDesc("Dollar/LTRED");
	  else
	    SetKeyDesc("Dollar/RED");
	} else {
	  if (sc & SCF_SHIFT)
	    "$$FG,LTRED$$";
	  else
	    "$$FG,RED$$";
	}
	break;
      case SC_F3:
	if (sc&SCF_KEY_DESC) {
	  if (sc & SCF_SHIFT)
	    SetKeyDesc("Dollar/LTGREEN");
	  else
	    SetKeyDesc("Dollar/GREEN");
	} else {
	  if (sc & SCF_SHIFT)
	    "$$FG,LTGREEN$$";
	  else
	    "$$FG,GREEN$$";
	}
	break;
      case SC_F4:
	if (sc&SCF_KEY_DESC) {
	  if (sc & SCF_SHIFT)
	    SetKeyDesc("Dollar/LTBLUE");
	  else
	    SetKeyDesc("Dollar/Default Color");
	} else {
	  if (sc & SCF_SHIFT)
	    "$$FG,LTBLUE$$";
	  else
	    "$$FG$$";
	}
	break;
      case SC_F6:
	if (sc&SCF_KEY_DESC) {
	  if (!(sc & SCF_SHIFT))
	    SetKeyDesc("Cmd/Init WordStat LT");
	} else if (!(sc & SCF_SHIFT))
	  PopUp("WsInit(\"/LT/\" TEXT_FILE_MASK \";~*Bible.TXZ\");");
	break;
      case SC_F7:
	if (sc&SCF_KEY_DESC) {
	  if (sc & SCF_SHIFT)
	    SetKeyDesc("Dollar/Insert Hidden Widget");
	  else
	    SetKeyDesc("Misc/Insert Datetime");
	} else {
	  if (sc & SCF_SHIFT)
	    LtfInsertSafe(LtfCur,
		"$$HS+C$$$$FG$$$$HE$$");
	  else
	    FillInTime();
	}
	break;
    }
    return TRUE;
  } else {
    if (!(sc & SCF_CTRL)) {
      if (sc & SCF_ALT) {
	switch (ch) {
	  case 'h':
	    if (sc&SCF_KEY_DESC)
	      SetKeyDesc("Win/Tile Horizontally");
	    else
	      WinTileHorz;
	    return TRUE;
	  case 'm':
	    if (sc&SCF_KEY_DESC)
	      SetKeyDesc("Win/Maximize");
	    else {
	      WinMax;
	      WinBorder;
	    }
	    return TRUE;
	  case 'v':
	    if (sc&SCF_KEY_DESC)
	      SetKeyDesc("Win/Tile Vertically");
	    else
	      WinTileVert;
	    return TRUE;
	  case 'V':
	    if (sc&SCF_KEY_DESC)
	      SetKeyDesc("Sys/Frame Grabber Toggle");
	    else
	      FrameGrabberToggle;
	    return TRUE;

	  case 'l':
	    if (sc&SCF_KEY_DESC)
	      SetKeyDesc("Edit/Put Link to Cur Pos on Clipboard");
	    else {
	      ClipBoardDel;
	      LtfPrintF(sys_clipboard_ltf,"$$LK-A,\"FL:%s,%d\"$$",
		LtfCur->filename.name,LtfCur->cur_entry->y+1);
	    }
	    return TRUE;
	  case 'L':
	    if (sc&SCF_KEY_DESC)
	      SetKeyDesc("Edit/Place Anchor, Put Link to Clipboard");
	    else {
	      i=RandU32;
	      ClipBoardDel;
	      LtfPrintF(sys_clipboard_ltf,"$$LK,\"<TODO>\",\"FA:%s,ANC%d\"$$",
		LtfCur->filename.name,i);
	      "$$AN,\"<TODO>\",\"ANC%d\"$$",i;
	    }
	    return TRUE;

	  case 'w':
	    if (sc&SCF_KEY_DESC)
	      SetKeyDesc("Win/WordStat(ON);");
	    else
	      WordStat(ON);
	    return TRUE;
	  case 'W':
	    if (sc&SCF_KEY_DESC)
	      SetKeyDesc("Win/WordStat;");
	    else
	      WordStat;
	    return TRUE;

//Insert your own ALT-key plug-ins

	  case 'f':
	    if (sc&SCF_KEY_DESC)
	      SetKeyDesc("Cmd/Jump to Notes File");
	    else
	      Ed("HOME/PersonalMenu.TXZ");
	    return TRUE;

	  case 'p':
	    if (sc&SCF_KEY_DESC)
	      SetKeyDesc("JukeBox(TAD)");
	    else
	      JukeBox("::/LT/Accts/TAD/MusicOrgan");
	    return TRUE;
	  case 'P':
	    if (sc&SCF_KEY_DESC)
	      SetKeyDesc("JukeBox(Examples)");
	    else
	      JukeBox("::/LT/Apps/MusicOrgan/Examples");
	    return TRUE;
	}
      }
    }
  }
  return FALSE;
}

BoolI64 MyPutS(U8 *st)
{
  nounusedwarn st;
  return FALSE;
}

KeyDevAdd(&MyPutKey,&MyPutS,0x20000000,TRUE);
