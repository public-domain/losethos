#help_index "Training"

public U0 PressKeyHelp(TaskStruct *task=NULL)
{
  U64 p1=0,p2=0;
  BoolI8 old_preempt=Preempt(OFF);
  TaskStruct *old_hash=Fs->hash_table;
  if (!task) task=Fs;

//This feat is a burden in code.
//It would be nice to list key
//cmds for all applications,
//but it makes the code too ugly.
//
task=Fs;

  Fs->hash_table=task->hash_table;
  "Press keys to show handler code.\n"
  "<SHIFT-ESC> to exit.\n"
  "Do not press <CTRL-ALT-keys>.\n";
  while (p1!=CH_ESC && p1!=CH_SHIFT_ESC) {
    GetMsg(&p1,&p2,1<<MSG_KEY_DOWN);
    *key_desc=0;
    key_handler=NULL;
    if (task==Fs)
      PutKey(p1,p2|SCF_KEY_DESC);
    else if (TaskValidate(task)) {
      PostMsg(task,MSG_KEY_DOWN,p1,p2|SCF_KEY_DESC);
      while (!key_handler)
	Yield;
    }
    if (*key_desc)
      EdAddSrc(key_handler,512);
  }
  Fs->hash_table=old_hash;
  Preempt(old_preempt);
}

U0 KeyMapCtrlAltFamily()
{
  U64 i,p2;
  U8 *st,*st2;

  p2=SC_DELETE+SCF_CTRL+SCF_ALT;
  st2=ScanCode2KeyName(p2);
  st=SrcEdLink(&Reboot,256);
  "%20s$$LK,\"%s\",\"%s\"$$\n",st2,"Sys/Reboot",st;
  Free(st);
  Free(st2);

  p2=SC_ESC+SCF_CTRL+SCF_ALT;
  st2=ScanCode2KeyName(p2);
  st=SrcEdLink(&SpawnUser,256);
  "%20s$$LK,\"%s\",\"%s\"$$\n",st2,"Sys/Spawn New User Task",st;
  Free(st);
  Free(st2);

  p2=SC_TAB+SCF_CTRL+SCF_ALT;
  st2=ScanCode2KeyName(p2);
  st=SrcEdLink(&WinToTop,256);
  "%20s$$LK,\"%s\",\"%s\"$$\n",st2,"Sys/Switch Focus to Next Task",st;
  Free(st);
  Free(st2);

  for (i=0;i<26;i++) {
    if (fp_ctrl_alt_let_cbs[i] &&
	ctrl_alt_let_descs[i]) {
      p2=A2ScanCode(i+'a')+SCF_CTRL+SCF_ALT;
      st2=ScanCode2KeyName(p2);
      st=SrcEdLink(fp_ctrl_alt_let_cbs[i],256);
      "%20s$$LK,\"%s\",\"%s\"$$\n",st2,ctrl_alt_let_descs[i],st;
      Free(st);
      Free(st2);
    }
  }
}

I64 KMCompare(U8 *e1,U8 *e2)
{
  I64 i;
  U8 *ee1,*ee2,ch1,ch2;
  BoolI8 e1_shift=FALSE,e2_shift=FALSE;
  if (e1) {
    if (e2) {
      while (*e1==CH_SPACE)
	e1++;
      while (*e2==CH_SPACE)
	e2++;
      while (*e1 && *e1==*e2) {
	e1++;
	e2++;
      }
      if (!StrNCmp(e1,"SHIFT",5)) {
	e1+=6;
	e1_shift=TRUE;
      } else if (!StrNCmp(e1,"",5))
	e1+=6;
      if (!StrNCmp(e2,"SHIFT",5)) {
	e2+=6;
	e2_shift=TRUE;
      } else if (!StrNCmp(e2,"",5))
	e2+=6;

      ee1=e1;
      while (*ee1 && *ee1!='$$')
	ee1++;
      ch1=*ee1;
      *ee1=0;

      ee2=e2;
      while (*ee2 && *ee2!='$$')
	ee2++;
      ch2=*ee2;
      *ee2=0;

      i=StrCmp(e1,e2);

      *ee1=ch1;
      *ee2=ch2;

      if (i)
	return i;
      else
	return e1_shift-e2_shift;
    } else
      return 1;
  } else {
    if (e2)
      return -1;
    else
      return 0;
  }
}

U0 KeyMapFamily2(U8 **entries,U8 *family,TaskStruct *task,I64 scf)
{
  U64 i,p2,p1;
  U8 *st,*st2;
  TaskStruct *old_hash=Fs->hash_table;
  Fs->hash_table=task->hash_table;
  for (i=0;i<256;i++) {
    p2=scf|i|SCF_KEY_DESC;
    p1=ScanCode2A(p2);
    *key_desc=0;
    key_handler=NULL;
    if (task==Fs)
      PutKey(p1,p2);
    else if (TaskValidate(task)) {
      PostMsg(task,MSG_KEY_DOWN,p1,p2);
      while (!key_handler)
	Yield;
    }
    if (*key_desc && StrNCmp(key_desc,"Char/",5)) {
      st=ScanCode2KeyName(p2&~(SCF_CTRL|SCF_ALT|SCF_SHIFT));
      st2=MSPrintF("%s%s",family,st);
      Free(st);
      st=SrcEdLink(key_handler,256);
      entries[i]=MSPrintF("%20s$$LK,\"%s\",\"%s\"$$\n",
	  st2,key_desc,st);
      Free(st);
      Free(st2);
    }
  }
  Fs->hash_table=old_hash;
}

U0 KeyMapFamily(U8 *family,TaskStruct *task,I64 scf)
{
  U64 i;
  U8 **entries=CAlloc(2*256*sizeof(U8 *)),st1[32],st2[32];
  SPrintF(st1,"%s",family);
  SPrintF(st2,"%sSHIFT",family);
  KeyMapFamily2(entries    ,st1,task,scf);
  KeyMapFamily2(entries+256,st2,task,scf+SCF_SHIFT);
  QSortU64(entries,2*256,&KMCompare);
  for (i=0;i<2*256;i++)
    if (entries[i]) {
      "" entries[i];
      Free(entries[i]);
    }
  Free(entries);
}

public U0 KeyMap(TaskStruct *task=NULL)
{
  if (!task) task=Fs;

//This feat is a burden in code.
//It would be nice to list key
//cmds for all applications,
//but it makes the code too ugly.
//
task=Fs;

  LtfMax;
  KeyMapFamily("",task,0);
  KeyMapFamily("CTRL",task,SCF_CTRL);
  KeyMapFamily("ALT",task,SCF_ALT);
  KeyMapCtrlAltFamily;
  "\n$$MA+A-X+PU,\"Click for help on key pressed\",\"PressKeyHelp(0x%X);\"$$\n",task;
}

public U0 TipOfDay(U8 *tip_file="::/LT/Doc/Tips.TXZ")
{
  I64 i=RandU16;
  Ltf *l=LtfRead(tip_file),*l2=LtfNew;
  LtfEntry *ll=l->root.next;
  "$$WW,1$$\n";
  while (TRUE) {
    if (ll->type_u8==LTFT_TEXT && *ll->tag=='*')
      if (!i--) break;
    ll=ll->next;
  }
  if (ll->type_u8==LTFT_TEXT && *ll->tag=='*') {
    while (ll!=l) {
      if (ll->type_u8!=LTFT_ERROR)
	LtfInsert(l2,LtfEntryCopy(l2,ll));
      ll=ll->next;
      if (ll->type_u8==LTFT_TEXT && *ll->tag=='*')
	break;
    }
  }
  LtfInsertLtf(LtfCur,l2);
  LtfDel(l2);
  LtfDel(l);
}

/****
//This feat is a burden in code.
//It would be nice to list key
//cmds for all applications,
//but it makes the code too ugly.
//
U0 CtrlAltK(U64 sc)
{
  nounusedwarn sc;
  U8 buf[136];
  SPrintF(buf,"KeyMap(0x%X);View;",
    sys_focus_task);
  PopUp(buf);
}
SetCtrlAltLetCB('K',&CtrlAltK,"Sys/Show KeyMap");
****/
