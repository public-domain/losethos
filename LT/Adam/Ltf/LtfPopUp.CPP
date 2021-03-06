public I64 LtfEntryRun(Ltf *l,LtfEntry *cl,BoolI8 exited)
{ //sometimes returns locked, sometimes unlocked
  I64 result=-1;
  U8 ch=l->cmd_U8,*st;
  LtfLock(l);
  if (!exited) {
    if (cl->flags & LTFLF_ESC) {
      Msg(MSG_KEY_DOWN,CH_ESC,0,1<<TASKCf_DONT_FILTER);
      return 0;
    } if (cl->flags & LTFLF_QUIT) {
      Msg(MSG_KEY_DOWN,CH_SHIFT_ESC,0,1<<TASKCf_DONT_FILTER);
      return 0;
    } else if (cl->flags & LTFLF_CHECK_COLLAPSABLE)
      cl->flags^=LTFLF_CHECKED_COLLAPSED;
  }
  l->best_d=MAX_I64;
  try {
    if (ch==CH_SPACE) {
      if (cl->flags & LTFLF_LINK && l->left_click_link)
	result=(*l->left_click_link)(l,cl);
      if (cl->flags & LTFLF_LEFT_EXP) {
	result=cl->left_exp;
	Msg(MSG_CMD,result,0,1<<TASKCf_DONT_FILTER);
      }
      if (cl->flags & LTFLF_LEFT_CB && cl->left_cb) {
	LtfUnlock(l);
	result=(*cl->left_cb)(l,cl);
	Msg(MSG_CMD,result,0,1<<TASKCf_DONT_FILTER);
      }
      if (cl->flags & LTFLF_LEFT_MACRO) {
	if (cl->flags & LTFLF_POPUP) {
	  st=StrNew(cl->left_macro);
	  LtfUnlock(l);
	  PopUp(st,Fs);
	  Free(st);
	} else {
	  if (cl->flags & LTFLF_LEFT_AUTO)
	    AutoStr("%s",cl->left_macro);
	  else
	    Auto("%s",cl->left_macro);
	}
      }
    } else if (ch==CH_NEW_LINE) {
      if (cl->flags & LTFLF_LINK && l->right_click_link)
	result=(*l->right_click_link)(l,cl);
      if (cl->flags & LTFLF_RIGHT_EXP) {
	result=cl->right_exp;
	Msg(MSG_CMD,result,0,1<<TASKCf_DONT_FILTER);
      }
      if (cl->flags & LTFLF_RIGHT_CB && cl->right_cb) {
	LtfUnlock(l);
	result=(*cl->right_cb)(l,cl);
	Msg(MSG_CMD,result,0,1<<TASKCf_DONT_FILTER);
      }
      if (cl->flags & LTFLF_RIGHT_MACRO) {
	if (cl->flags & LTFLF_POPUP) {
	  st=StrNew(cl->right_macro);
	  LtfUnlock(l);
	  PopUp(st,Fs);
	  Free(st);
	} else {
	  if (cl->flags & LTFLF_RIGHT_AUTO)
	    AutoStr("%s",cl->right_macro);
	  else
	    Auto("%s",cl->right_macro);
	}
      }
    }
  }
  catch
    LtfBottom(l);
  l->cmd_U8=CH_SPACE;
  return result;
}

U8 *EdOverStrikeCB(Ltf *l,LtfEntry *ll,TaskStruct *mem_task)
{
  Ltf *ltf=ll->user_data;
  U8 *st=MAlloc(8,mem_task);
  nounusedwarn l;
  *st=ltf->flags & LTFF_OVERSTRIKE ? 'O':CH_SINGLE_HORZ;
  st[1]=0;
  return st;
}

U8 *EdFilterCB(Ltf *l,LtfEntry *ll,TaskStruct *mem_task)
{
  Ltf *ltf=ll->user_data;
  U8 *st=MAlloc(8,mem_task);
  nounusedwarn l;
  *st=ltf->find_replace->filter_lines ? 'F':CH_SINGLE_HORZ;
  st[1]=0;
  return st;
}

U8 *EdDollarCB(Ltf *l,LtfEntry *ll,TaskStruct *mem_task)
{
  Ltf *ltf=ll->user_data;
  U8 *st=MAlloc(8,mem_task);
  nounusedwarn l;
  *st=ltf->flags & LTFF_IN_DOLLAR ? '$$':CH_SINGLE_HORZ;
  st[1]=0;
  return st;
}

U8 *EdMoreCB(Ltf *l,LtfEntry *ll,TaskStruct *mem_task)
{
  Ltf *ltf=ll->user_data;
  U8 *st=MAlloc(8,mem_task);
  nounusedwarn l;
  if (ltf->flags&LTFF_MORE)
    StrCpy(st,"More�");
  else
    StrCpy(st,"�����");
  return st;
}

U8 *EdQuotesCB(Ltf *l,LtfEntry *ll,TaskStruct *mem_task)
{
  LexStruct *lx=mem_task->last_lex;
  U8 *st=MAlloc(8,mem_task);
  nounusedwarn l,ll;
  *st=lx->flags & LF_IN_QUOTES ? '"':CH_SINGLE_HORZ;
  st[1]=0;
  return st;
}

U8 *EdBracesCB(Ltf *l,LtfEntry *ll,TaskStruct *mem_task)
{
  LexStruct *lx=mem_task->last_lex;
  U8 *st=MAlloc(8,mem_task);
  nounusedwarn l,ll;
  *st=lx->braces_cnt ? '}':CH_SINGLE_HORZ;
  st[1]=0;
  return st;
}

U8 *EdSemicolonCB(Ltf *l,LtfEntry *ll,TaskStruct *mem_task)
{
  LexStruct *lx=mem_task->last_lex;
  U8 *st=MAlloc(8,mem_task);
  nounusedwarn l,ll;
  *st=lx->statement_cnt ? ';':CH_SINGLE_HORZ;
  st[1]=0;
  return st;
}

U8 *EdDollarTypeCB(Ltf *l,LtfEntry *ll,TaskStruct *mem_task)
{
  Ltf *ltf=ll->user_data;
  U8 *src=DefineSub(ltf->cur_entry->type_u8,"ST_LTF_CMDS");
  U8 *st=CAlloc(8,mem_task);
  nounusedwarn l;
  if (ltf->cur_entry==ltf)
    src="EOF";
  else if (!src)
    src="ERR";
  SPrintF(st,"%-3ts",src);
  return st;
}

public U64 LtfGetKey(U64 *_sc=NULL)
{
  U64 ch,sc;
  BoolI8 cont,old_preempt=IsPreempt;
  do {
    ch=GetKey(&sc,TRUE);
    LtfRecalc(LtfCur);
    cont=FALSE;
    if (ch==CH_ESC || ch==CH_SHIFT_ESC) { //Check for exit filter mode
      Preempt(OFF);
      if (LtfCur && LtfCur->find_replace->filter_lines) {
	EdUndoFilter(LtfCur);
	cont=TRUE;
      }
      Preempt(old_preempt);
    }
  } while (cont);
  if (_sc) *_sc=sc;
  return ch;
}

#help_index "Ltf/Output"
public BoolI64 View()
{
  U64 ch;
  do ch=LtfGetKey;
  while (ch!=CH_ESC && ch!=CH_SHIFT_ESC);
  return ch==CH_ESC;
}

#help_index "Ltf"
public BoolI64 LtfDo(Ltf *l,U64 do_flags=0,U8 *help=NULL)
{
  Ltf *menu=NULL,*old_ltf=LtfCur,*old_aux_ltf=LtfAux,*aux;
  I64 old_attr=Fs->text_attr;
  LtfEntry *ll;
  I64 old_top=Fs->win_top,old_bottom=Fs->win_bottom,
      old_left=Fs->win_left,old_right=Fs->win_right;
  BoolI8 unlock,old_preempt,result;

  unlock=LtfLock(l);
  if (help && !l->menu_ltf) {
    if (do_flags&DOF_HELP_IS_FILE)
      menu=LtfRead(help);
    else {
      menu=LtfNew;
      StrCpy(menu->desc,"SysMenu");
      menu->settings_root.cur_text_attr=menu->settings_root.dft_text_attr=
	  menu->start_text_attr=l->start_text_attr;
      LtfPutS(menu,help);
    }
    l->menu_ltf=menu;
  }

  l->win_task=Fs;
  aux=LtfNew;
  aux->flags|=LTFF_AUX_LTF;
  aux->settings_root.dft_text_attr=Fs->border_attr;
  LtfPutS(aux,"$$CM+TY+LX+NC,0,-1$$");
  if (Bt(&Fs->display_flags,DISPLAYf_HAS_CLOSE_WIN))
    LtfPutS(aux,"$$TX+RX+BD,\"[X]\"$$");
  if (l->menu_ltf)
    LtfPutS(aux,"$$BK,1$$$$TX+LX+BD,\"MENU\"$$$$BK,0$$");

  ll=LtfPutS(aux,"$$DA-T-P+BD+RD+CX+SCX+IV,256,\"%s...\",16$$");
  ll->data=&l->filename.name;
  LtfDataFmt(ll);

  if (l->flags & LTFF_ALLOW_UNDO) {
    LtfPutS(aux,"$$CM+BY+LX+NC,1,1$$");
    ll=LtfPutS(aux,"$$DA+BD+RD-T,32,\"Undo:%03d\"$$\n");
    ll->data=&l->undo_cnt;
    LtfDataFmt(ll);
  }

  LtfPutS(aux,"$$CM+BY+RX+NC,-30,1$$");
  ll=LtfPutS(aux,"$$TX+BD+TC,\" \"$$");
  ll->user_data=l;
  ll->tag_cb=&EdMoreCB;
  ll=LtfPutS(aux,"$$TX+BD+TC,\" \"$$");
  ll->user_data=l;
  ll->tag_cb=&EdDollarTypeCB;
  ll=LtfPutS(aux,"$$TX+BD+TC,\" \"$$");
  ll->user_data=l;
  ll->tag_cb=&EdFilterCB;
  ll=LtfPutS(aux,"$$TX+BD+TC,\" \"$$");
  ll->user_data=l;
  ll->tag_cb=&EdOverStrikeCB;
  ll=LtfPutS(aux,"$$TX+BD+TC,\" \"$$");
  ll->user_data=l;
  ll->tag_cb=&EdDollarCB;
  ll=LtfPutS(aux,"$$DA+BD+RD-T,32,\"Line:%04d \"$$");
  ll->data=&l->line;
  LtfDataFmt(ll);
  ll=LtfPutS(aux,"$$DA+BD+RD-T,32,\"Col:%04d\"$$\n");
  ll->data=&l->col;
  LtfDataFmt(ll);

  LtfRecalc(aux);
  LtfRecalc(l);
  if (!(do_flags&DOF_DONT_HOME))
    LtfHome(l);
  old_preempt=Preempt(OFF);
  LtfRemQue(old_aux_ltf);
  LtfInsQueRev(aux,Fs->next_ltf);
  if (l!=old_ltf)
    l->parent_ltf=old_ltf;
  LtfRemQue(old_ltf);
  LtfInsQue(l,Fs->last_ltf);
  Fs->text_attr=l->settings_root.dft_text_attr;
  LBEqu(&Fs->display_flags,DISPLAYf_HAS_MENU,l->menu_ltf);
  LBts(&Fs->display_flags,DISPLAYf_SHOW);
  WinZBufUpdate;
  if (do_flags&DOF_MIN_SIZE)
    l->flags|=LTFF_MIN_SIZE;

  Preempt(old_preempt);
  LtfUnlock(l);
  WinSync(TRUE);
  if (l->flags&LTFF_MIN_SIZE)
    WinSync(TRUE);
  result=View;

  Preempt(OFF);
  LtfLock(l);
  if (result) {
    ll=l->root.next;
    while (ll!=l) {
      if (ll->type_u8==LTFT_DATA || ll->type_u8==LTFT_CHECK_BOX)
	LtfDataGet(ll);
      ll=ll->next;
    }
  }
  if (unlock)
    LtfUnlock(l);

  LtfRemQue(aux);
  LtfInsQueRev(old_aux_ltf,Fs->next_ltf);
  LtfDel(aux);
  LtfRemQue(l);
  LtfInsQue(old_ltf,Fs->last_ltf);
  LBEqu(&Fs->display_flags,DISPLAYf_HAS_MENU,old_ltf->menu_ltf);
  Fs->text_attr=old_attr;
  if (menu) {
    l->menu_ltf=NULL;
    LtfDel(menu);
  }
  if (do_flags&DOF_MIN_SIZE) {
    Fs->win_top=old_top; Fs->win_bottom=old_bottom;
    Fs->win_left=old_left; Fs->win_right=old_right;
    TaskDerivedValsUpdate;
  }
  Preempt(old_preempt);
  return result;
}


#help_index "Ltf/Input"
public BoolI64 LtfFormDo(U8 *D,U8 *class_name,
    U64 do_flags=0,U8 *help=NULL,
    U8 *header=NULL,U8 *footer=NULL)
//Supply a class name that has fmtstr definitions
//and this routine will allow user input.
{
  BoolI8 result=FALSE;
  MemberListStruct *ml;
  LtfEntry *ll;
  U8 *fmtstr;
  ClassStruct *tempc;
  Ltf *l;
  if (!(tempc=HashFind(class_name,Fs->hash_table,HTT_CLASS)))
    return FALSE;
  l=LtfNew;
  StrCpy(l->desc,"Form");
  if (header) LtfPutS(l,header);
  l->flags|=LTFF_OVERSTRIKE|LTFF_FORM;
  if (do_flags&DOF_MIN_SIZE)
    l->flags|=LTFF_MIN_SIZE;
  ml=tempc->member_list;
  while (ml) {
    if (fmtstr=MemberMetaData("fmtstr",ml)) {
      ll=LtfPutS(l,fmtstr);
      ll->data=D+ml->offset;
      ll->my_fmt_data=MemberMetaData("fmtdata",ml);
      LtfDataFmt(ll);
    }
    ml=ml->next;
  }
  if (footer) LtfPutS(l,footer);
  if (l->root.next!=l)	{
    l->flags&=~(LTFF_ATTR_BY_PRT | LTFF_ATTR_BY_FILENAME);
    LtfRecalc(l);
    if (LtfDo(l,do_flags,help)) {
      ll=l->cur_entry;
      if (ll!=l) {
	LtfEntryRun(l,ll,TRUE);
	LtfUnlock(l);
      }
      result=TRUE;
    }
  }
  LtfDel(l);
  return result;
}

U0 LtfMenuDoEndTaskCB()
{
  WinToTop;
  throw(EXCEPT_LOCAL);
}

public I64 LtfMenuDo(Ltf *m,U64 do_flags=0,U8 *help=NULL)
{ //returns m unlocked
  U0 *old_end_cb=Fs->task_end_cb;
  BoolI8 old_break_shift_esc=LBts(&Fs->task_flags,TASKf_BREAK_TO_SHIFT_ESC);
  LtfEntry *ll;
  I64 result=LTFM_CANCEL;
  U64 old_ltf_flags=m->flags;
  Fs->task_end_cb=&LtfMenuDoEndTaskCB;
  try {
    if (m) {
      StrCpy(m->desc,"Menu");
      m->flags&=~(LTFF_ATTR_BY_PRT | LTFF_ATTR_BY_FILENAME);
      if (LtfDo(m,do_flags,help)) {
	ll=m->cur_entry;
	if (ll!=m) {
	  result=LtfEntryRun(m,ll,TRUE);
	  LtfUnlock(m);
	}
      }
    }
  } catch {
    if (Fs->except_argc==1 && Fs->except_argv[0]==EXCEPT_LOCAL) {
      if (!(do_flags & DOF_INTERCEPT_TASK_END))
	Exit;
      Fs->catch_except=TRUE;
    }
  }
  m->flags|=old_ltf_flags&(LTFF_ATTR_BY_PRT | LTFF_ATTR_BY_FILENAME);
  LBEqu(&Fs->task_flags,TASKf_BREAK_TO_SHIFT_ESC,old_break_shift_esc);
  Fs->task_end_cb=old_end_cb;
  return result;
}

#help_index "Ltf"
public I64 PopUp(U8 *msg,TaskStruct *parent=NULL,TaskStruct **pu_task=NULL)
{
  U64 result=0;
  BoolI8 old_preempt=Preempt(OFF);
  TaskCmdStruct *tempc;
  TaskStruct *task=Spawn(&ServantUserCmdLine,NULL,"System",parent);
  if (!parent) {
    TaskRequestQueue(task,parent,msg,1<<TASKCf_EXIT_ON_COMPLETE);
    if (pu_task) *pu_task=task;
    Preempt(old_preempt);
    return 0;
  } else {
    Fs->popup_task=task;
    tempc=TaskRequestQueue(task,parent,msg,
      1<<TASKCf_WAKE_MASTER | 1<<TASKCf_FOCUS_MASTER);
    GetRequestResult(tempc,&result);
    Fs->popup_task=NULL;
    Kill(task);
    if (pu_task) *pu_task=NULL;
    Preempt(old_preempt);
    return result;
  }
}

#help_index "Ltf/Output"
public TaskStruct *PopUpViewLtf(Ltf *l,U64 do_flags=0,U8 *help=NULL)
{
  U8 *buf=MSPrintF("if (!IsSysDeterministic) Preempt(ON);LtfDo(0x%X,0x%X,0x%X);",l,do_flags,help);
  TaskStruct *task=Spawn(&ServantUserCmdLine,NULL,"View",Fs);
  TaskRequestQueue(task,NULL,buf,1<<TASKCf_EXIT_ON_COMPLETE);
  Free(buf);
  return task;
}

U0 PopUpViewStrTaskEnd()
{
  LtfDel(NamedFramePtr("ViewStrFrame"));
  Exit;
}

public TaskStruct *PopUpViewStr(U8 *msg)
{
  TaskStruct *task;
  Ltf *l=LtfNew;
  LtfPrintF(l,msg);
  task=PopUpViewLtf(l);
  NamedFramePtrAdd("ViewStrFrame",l,task);
  task->task_end_cb=&PopUpViewStrTaskEnd;
  return task;
}

#help_index "Ltf/Input"
public I64 PopUpMenu(Ltf *l,U64 do_flags=0,U8 *help=NULL)
{
  l->flags|=LTFF_MIN_SIZE | LTFF_FORM;
  return ServantUser("LtfMenuDo(0x%X,0x%X,0x%X);",l,do_flags,help);
}

public BoolI64 PopUpForm(U8 *D,U8 *class_name,U64 do_flags=DOF_MIN_SIZE,U8 *help=NULL,
    U8 *header=NULL,U8 *footer=NULL)
{
  return ServantUser("LtfFormDo(0x%X,0x%X,0x%X,0x%X,0x%X,0x%X);",D,class_name,
    do_flags,help,header,footer);
}

#help_index "Ltf/Input;File/FileNames"
public U8 *PopUpPickFile(U8 *dir=NULL)
{
  U8 *st,*st2,*result;
  if (dir)
    st=MSPrintF("Cd(\"%Q\");FileMgr(FM_PICK_FILE,Fs->parent_task);",dir);
  else {
    st2=CurDir;
    st=MSPrintF("Cd(\"%Q\");FileMgr(FM_PICK_FILE,Fs->parent_task);",st2);
    Free(st2);
  }
  result=PopUp(st,Fs);
  Free(st);
  return result;
}

public U8 *PopUpPickDir(U8 *dir=NULL)
{
  U8 *st,*st2,*result;
  if (dir)
    st=MSPrintF("Cd(\"%Q\");FileMgr(FM_PICK_DIR,Fs->parent_task);",dir);
  else {
    st2=CurDir;
    st=MSPrintF("Cd(\"%Q\");FileMgr(FM_PICK_DIR,Fs->parent_task);",st2);
    Free(st2);
  }
  result=PopUp(st,Fs);
  Free(st);
  return result;
}

public U8 *FileNameForm(U8 *dft=NULL,U64 do_flags=0,TaskStruct *mem_task=NULL)
{
  EdFileNameStruct fn;
  if (dft)
    StrCpy(fn.name,dft);
  else
    *fn.name=0;
  if (LtfFormDo(fn.name,"EdFileNameStruct",do_flags))
    return StrNew(fn.name,mem_task);
  else
    return NULL;
}

public U8 *PopUpFileName(U8 *dft=NULL,U64 do_flags=0)
{
  U8 *st,*result;
  st=MSPrintF("FileNameForm(\"%Q\",0x%X,Fs->parent_task);",dft,do_flags|DOF_MIN_SIZE);
  result=PopUp(st,Fs);
  Free(st);
  return result;
}

#help_index "Ltf"
public BoolI64 PopUpCd()
{
  U8 *st=PopUpPickDir;
  BoolI8 result;
  if (st) {
    result=Cd(st);
    Free(st);
  } else
    result=FALSE;
  return result;
}

#help_index "Ltf/Input"
public U8 *PopUpPickListEntry(U8 *lst)
{
  I64 i=0;
  Ltf *l=LtfNew;
  while (*lst) {
    if (*lst=='@') { /* Check for '@' alias list entry*/
      i--;
      lst++;
    }
    LtfPrintF(l,"$$MU,\"%s\",0x%08X$$\n",
	lst,i++);
    lst+=StrLen(lst)+1;
  }
  LtfPutS(l,"\n$$MU,\"CANCEL\",LTFM_CANCEL$$\n");
  i=PopUpMenu(l);
  LtfDel(l);
  return i;
}

public U8 *PopUpPickDefineSub(U8 *st)
{
  return PopUpPickListEntry(Define(st));
}

public I64 PopUp1(U8 *b1,I64 n1,U8 *header=NULL,U8 *footer=NULL)
{
  I64 i,l1=StrLen(b1);
  Ltf *l=LtfNew;
  if (header) LtfPutS(l,header);
  LtfPrintF(l,"$$CM+CX,%d,4$$$$BT,\"%s\",%d$$\n",-l1/2,b1,n1);
  if (footer) LtfPutS(l,footer);
  i=PopUpMenu(l);
  LtfDel(l);
  return i;
}

public I64 PopUp2(U8 *b1,I64 n1,U8 *b2,I64 n2,U8 *header=NULL,U8 *footer=NULL)
{
  I64 i,l1=StrLen(b1),l2=StrLen(b2),y;
  Ltf *l=LtfNew;
  if (header) {
    LtfPutS(l,header);
    y=4;
  } else {
    LtfPrintF(l,"%*s\n",l1+l2+10,"");
    y=3;
  }
  LtfPrintF(l,"$$CM+CX,%d,%d$$$$BT,\"%s\",%d$$",-(l1+l2+3)>>1,y,b1,n1);
  LtfPrintF(l,"$$CM+CX,%d,0$$$$BT,\"%s\",%d$$\n" ,-(l1+l2+3)>>1+l1+6,b2,n2);
  if (footer) LtfPutS(l,footer);
  i=PopUpMenu(l);
  LtfDel(l);
  return i;
}

public BoolI64 PopUpOk(U8 *header=NULL,U8 *footer=NULL)
{
  return PopUp1("OKAY",1,header,footer)>0;
}

public BoolI64 PopUpNoYes(U8 *header=NULL,U8 *footer=NULL)
{
  return $WW,0$PopUp2("YES",1,"NO",0,header,footer)>0;
}

public BoolI64 PopUpCancelOk(U8 *header=NULL,U8 *footer=NULL)
{
  return PopUp2("OKAY",1,"CANCEL",0,header,footer)>0;
}


#define RSAC_REPLACE	0
#define RSAC_SKIP	1
#define RSAC_ALL	2

public I64 PopUpReplaceSkipAllCancel(U8 *header=NULL,U8 *footer=NULL)
{
  I64 i;
  Ltf *l=LtfNew;
  if (header) LtfPutS(l,header);
  LtfPutS(l,"$$CM+LX,1,4$$$$BT,\"REPLACE\",RSAC_REPLACE$$"
	"$$CM+LX,17,0$$$$BT,\"SKIP\",RSAC_SKIP$$"
	"$$CM+LX,1,3$$$$BT,\"ALL\",RSAC_ALL$$"
	"$$CM+LX,17,0$$$$BT,\"CANCEL\",LTFM_CANCEL$$\n");
  if (footer) LtfPutS(l,footer);
  i=PopUpMenu(l);
  LtfDel(l);
  return i;
}

U8 *DoPopUpMGetS(U8 *header,TaskStruct *mem_task)
{
  U8 *result=NULL,*st;
  if (header)
    "" header;
  st=MGetS(GETSF_WITH_NEW_LINE);
  result=StrNew(st,mem_task);
  Free(st);
  return result;
}

public U8 *PopUpMGetS(U8 *header=NULL)
{
  U8 *result,*st=MSPrintF("DoPopUpMGetS(0x%X,0x%X);",header,Fs);
  result=PopUp(st,Fs);
  Free(st);
  return result;
}

public I64 PopUpRangeI64(I64 lo,I64 hi,I64 step=1,U8 *header=NULL,U8 *footer=NULL)
{
  I64 i;
  Ltf *l=LtfNew;
  if (header)
    LtfPutS(l,header);
  for (i=lo;i<=hi;i+=step)
    LtfPrintF(l,"$$MU,\"%d\",%d$$\n",i,i);
  if (footer)
    LtfPutS(l,footer);
  i=PopUpMenu(l);
  LtfDel(l);
  return i;
}

public F64 PopUpRangeF64(F64 lo,F64 hi,F64 step,
    U8 *fmt="%9.4f",U8 *header=NULL,U8 *footer=NULL)
{
  F64 d;
  I64 i;
  U8 buf[136];
  Ltf *l=LtfNew;
  if (header)
    LtfPutS(l,header);
  for (d=lo;d<=hi;d+=step) {
    SPrintF(buf,fmt,d);
    LtfPrintF(l,"$$MU,\"%s\",0x%X$$\n",buf,d);
  }
  if (footer)
    LtfPutS(l,footer);
  i=PopUpMenu(l);
  LtfDel(l);
  return i(F64);
}

public F64 PopUpRangeF64Exp(F64 lo,F64 hi,F64 factor,
    U8 *fmt="%9.4f",U8 *header=NULL,U8 *footer=NULL)
{
  F64 d;
  I64 i;
  U8 buf[136];
  Ltf *l=LtfNew;
  if (header)
    LtfPutS(l,header);
  for (d=lo;d<=hi;d*=factor) {
    SPrintF(buf,fmt,d);
    LtfPrintF(l,"$$MU,\"%s\",0x%X$$\n",buf,d);
  }
  if (footer)
    LtfPutS(l,footer);
  i=PopUpMenu(l);
  LtfDel(l);
  return i(F64);
}

public F64 PopUpRangeF64Log(F64 lo,F64 hi,I64 steps,
    U8 *fmt="%9.4f",U8 *header=NULL,U8 *footer=NULL)
{
  return PopUpRangeF64Exp(lo,hi,Exp(Ln(hi/lo)/(steps-1)),fmt,header,footer);
}

#help_index "Task/Requests"
public I64 AdamFile(U8 *filename,BoolI8 warn_ext=TRUE)
{
  I64 result=0;
  BoolI8 okay=TRUE;
  U8 *name=FileNameAbs(filename),
     *name2=DftExt(name,"APZ");
  if (warn_ext &&
      !WildCardMatch("*.AP?",name2) &&
      !PopUpCancelOk(ST_WARN_ST "Not .AP? File\n\n"))
    okay=FALSE;
  if (okay)
    result=Adam("#include \"%s\";",name2);
  Free(name2);
  Free(name);
  return result;
}
#help_index "Ltf"

public I64 PopUpFile(U8 *filename,BoolI8 warn_ext=TRUE,TaskStruct *parent=NULL,TaskStruct **pu_task=NULL)
{
  I64 result=0;
  BoolI8 okay=TRUE;
  U8 *st,*name=FileNameAbs(filename),
     *name2=DftExt(name,"CPZ");
  if (warn_ext &&
      !WildCardMatch("*.CP?",name2) &&
      !PopUpCancelOk(ST_WARN_ST "Not .CP? File\n\n"))
    okay=FALSE;
  if (okay) {
    st=MSPrintF("\"$$$$WW+H,1$$$$\";Auto(\"ExeFile(\\\"%s\\\");\\n\");UserTaskCont;",name2);
    result=PopUp(st,parent,pu_task);
    Free(st);
  }
  Free(name2);
  Free(name);
  return result;
}

#help_index "Ltf/Input;Input Pointer"
public I64 PopUpInputPtr()
{ //Not yet relevant
  ip_dev=PopUp2("Mouse",IP_MOUSE,"JoyStick",IP_JOYSTICK0);
  return ip_dev;
}

#help_index "Graphics/Input"
public I64 PopUpColor(U8 *header=NULL,BoolI8 allow_transparent=TRUE,BoolI8 allow_dft=TRUE)
{
  I64 i;
  Ltf *l=LtfNew;
  if (header)
    LtfPutS(l,header);
  if (allow_transparent)
    LtfPutS(l,"$$BG,WHITE$$$$MU,\"TRANSPARENT\",COLOR_TRANSPARENT$$$$BG$$\n");
  LtfPutS(l,"$$FM,BLACK$$");
  for (i=0;i<16;i++)
    LtfPrintF(l,"$$BG,%d$$$$MU,\"__%-8Z__\",%d$$$$BG$$\n",i,i,"ST_COLORS",i);
  if (allow_dft)
    LtfPutS(l,"$$BG,WHITE$$$$MU,\"DEFAULT\",LTF_DFT$$$$BG$$");
  i=PopUpMenu(l);
  LtfDel(l);
  return i;
}

public I64 PopUpLightingColor(U8 *header=NULL)
{
  I64 i;
  Ltf *l=LtfNew;
  if (header)
    LtfPutS(l,header);
  LtfPutS(l,"\n$$FM,BLACK$$Single-Sided Full-Range Double-Sided Full-Range\n");
  for (i=0;i<8;i++) {
    LtfPrintF(l,"$$BG,%d$$$$MU,\"__%-19Z__\",%d$$$$BG$$ ",i,i,"ST_COLORS",i);
    LtfPrintF(l,"$$BG,%d$$$$MU,\"__%-19Z__\",%d$$$$BG$$\n",i,i,"ST_COLORS",ROPF_TWO_SIDED+i);
  }
  LtfPutS(l,"\nSingle-Sided Half-Range Double-Sided Half-Range\n");
  for (i=0;i<16;i++) {
    LtfPrintF(l,"$$BG,%d$$$$MU,\"__%-19Z__\",%d$$$$BG$$ ",i,i,"ST_COLORS",ROPF_HALF_RANGE_COLOR+i);
    LtfPrintF(l,"$$BG,%d$$$$MU,\"__%-19Z__\",%d$$$$BG$$\n",i,i,"ST_COLORS",ROPF_TWO_SIDED+ROPF_HALF_RANGE_COLOR+i);
  }
  i=PopUpMenu(l);
  LtfDel(l);
  return i;
}

class TransformFormStruct
{
  F64 x_scale	fmtstr "$$DA-T,32,\"XScale:%12.6f\"$$\n";
  F64 y_scale	fmtstr "$$DA-T,32,\"YScale:%12.6f\"$$\n";
  F64 z_scale	fmtstr "$$DA-T,32,\"ZScale:%12.6f\"$$\n";
  F64 �_z	fmtstr "\nAngles are in degrees\n$$DA-T,32,\"Z-Axis:%12.6f\"$$\n";
  F64 �_y	fmtstr "$$DA-T,32,\"Y-Axis:%12.6f\"$$\n";
  F64 �_x	fmtstr "$$DA-T,32,\"X-Axis:%12.6f\"$$\n";
  F64 x_offset	fmtstr "\n$$DA-T,32,\"XOffset:%12.6f\"$$\n";
  F64 y_offset	fmtstr "$$DA-T,32,\"YOffset:%12.6f\"$$\n";
  F64 z_offset	fmtstr "$$DA-T,32,\"ZOffset:%12.6f\"$$\n";
};

BoolI64 PopUpTransform(I64 *r)
{
  TransformFormStruct *tf=MAlloc(sizeof(TransformFormStruct));
  tf->x_scale=1.0;
  tf->y_scale=1.0;
  tf->z_scale=1.0;
  tf->�_x=0.0;
  tf->�_y=0.0;
  tf->�_z=0.0;
  tf->x_offset=0.0;
  tf->y_offset=0.0;
  tf->z_offset=0.0;
  if (PopUpForm(tf,"TransformFormStruct")) {
    MemSet(r,0,16*sizeof(I64));
    r[0] =tf->x_scale*gr_scale;
    r[5] =tf->y_scale*gr_scale;
    r[10]=tf->z_scale*gr_scale;
    r[15]=gr_scale;

    GrRotZEqu(r,tf->�_z*�/180.0);
    GrRotYEqu(r,tf->�_y*�/180.0);
    GrRotXEqu(r,tf->�_x*�/180.0);

    r[3] =tf->x_offset*gr_scale;
    r[7] =tf->y_offset*gr_scale;
    r[11]=tf->z_offset*gr_scale;
    return TRUE;
  } else
    return FALSE;
}


#define PU_SPACING	10
U0 DrawIt(TaskStruct *task,GrDC *dc)
{
  I64 i,j,i1,j1;
  Ltf *l=LtfCur(task);
  for (j=0;j<17;j++)
    for (i=0;i<17;i++) {
      if (j<16)
	j1=j;
      else
	j1=COLOR_TRANSPARENT;
      if (i<16)
	i1=i;
      else
	i1=COLOR_TRANSPARENT;
      dc->color=j1+i1<<16+ROPF_DITHER;
      GrRect(dc,i*PU_SPACING,j*PU_SPACING+NamedFramePtr("DitherDoFrame",task),PU_SPACING,PU_SPACING);
    }
  i=(17*PU_SPACING+FONT_WIDTH-1)/FONT_WIDTH;
  if (l->max_x>i) i=l->max_x;
  task->win_right=task->win_left+i;
  task->win_bottom=task->win_top+l->max_y+(17*PU_SPACING+FONT_WIDTH-1)/FONT_WIDTH;
}

I64 DitherColorDo(U8 *header=NULL)
{
  I64 msg_code,p1,p2,result=-1,i,j;
  LtfClear;
  "" header;
  LtfRecalc(LtfCur);
  NamedFramePtrAdd("DitherDoFrame",(LtfCur->max_y+1)*FONT_HEIGHT);
  SettingsPush; //See $LK,"SettingsPush","MN:SettingsPush"$
  Fs->draw_it=&DrawIt;
  do {
    msg_code=GetMsg(&p1,&p2,1<<MSG_IP_L_DOWN|1<<MSG_KEY_DOWN);
    if (msg_code==MSG_IP_L_DOWN) {
      msg_code=GetMsg(&p1,&p2,1<<MSG_IP_L_UP);
      i=p1/PU_SPACING;
      j=(p2-NamedFramePtr("DitherDoFrame"))/PU_SPACING;
      if (p1>=0 && p2>=0 && 0<=j<17 && 0<=i<17) {
	if (j==16) j=COLOR_TRANSPARENT;
	if (i==16) i=COLOR_TRANSPARENT;
	result=j+i<<16+ROPF_DITHER;
	goto dcd_done;
      }
    }
  } while (!(msg_code==MSG_KEY_DOWN && (p1==CH_ESC || p1==CH_SHIFT_ESC)));
  do GetMsg(&p1,&p2,1<<MSG_KEY_UP);
  while (!p1);
dcd_done:
  SettingsPop;
  NamedFramePtrDel("DitherDoFrame");
  return result;
}

public I64 PopUpDitherColor(U8 *header=NULL)
{
  U8 buf[136];
  SPrintF(buf,"DitherColorDo(0x%X);",header);
  return PopUp(buf,Fs);
}


U0 PopUpSnap(SnapStruct *g)
{
  F64 d;
  I64 i=PopUpRangeI64(1,16,1,"Snap\n");
  if (i>=1) {
    d=PopUpRangeF64Exp(1.0/16.0,4.0,2.0,"%5.3f","Pointer Speed Scaler\n");
    if (d>0)
      SetSnap(g,i,d);
  }
}

#help_index "Ltf"
