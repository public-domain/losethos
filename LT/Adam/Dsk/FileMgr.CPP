#help_index "File/Program Routines/Low Level"
public U0 CacheLocalBlkDevDirs()
{
  I64 i;
  LTDirEntry *tempm;
  LTPrt *p;
  U8 files_find_mask[16];
  for (i=0;i<NUM_LTPRTS;i++) {
    p=&local_prts[i];
    try
      SPrintF(files_find_mask,"%C:\*",Prt2Drv(p));
    catch {
      p=NULL;
      Fs->catch_except=TRUE;
    }
    if (p && (p->type==PT_LT||p->type==PT_FAT32||
	p->type==PT_ISO9660 && p->bdev->flags&LTBDF_INITIALIZED)) {
      tempm=FilesFind(files_find_mask,1<<FUf_FLATTEN_TREE|1<<FUf_JUST_DIRS|1<<FUf_RECURSE);
      LTDirListDel(tempm);
    }
  }
}

#help_index "Ltf/Output"
U0 LTFSDirEntryFileLtf(Ltf *l,LTDirEntry *tempm)
{
  LtfEntry *ll;
  while (tempm) {
    if (tempm->attr & LT_ATTR_DIR) {
      ll=LtfPrintF(l,"$$TR,\"%s\"$$",tempm->name);
      ll->user_data=tempm;
      tempm->user_data=ll;
      LtfPutS(l,"\n$$ID,+2$$");
      if (tempm->sub)
	LTFSDirEntryFileLtf(l,tempm->sub);
      LtfPutS(l,"$$ID,-2$$");
    } else {
      ll=LtfPrintF(l,"$$MU-LE,\"%s\"$$",
	tempm->name);
      ll->user_data=tempm;
      tempm->user_data=ll;
      LtfPutS(l,"\n");
    }
    tempm=tempm->next;
  }
}

#help_index "File/Cmd Line (Typically);Cmd Line (Typically)"

#define FILEMANAGER_MENU_NAME "::/LT/Doc/FileMgr.TXZ"

class FMUncollapsedList
{
  FMUncollapsedList *next;
  U8 *name;
};

FMUncollapsedList *FMCollectUncollapsedList(Ltf *l)
{
  LtfEntry *ll=l->root.next;
  FMUncollapsedList *result=NULL,*tempc;
  LTDirEntry *tempm;
  while (ll!=l) {
    if (ll->type_u8==LTFT_TREE) {
      if (!(ll->flags&LTFLF_CHECKED_COLLAPSED)) {
	if (tempm=ll->user_data) {
	  tempc=MAlloc(sizeof(FMUncollapsedList));
	  tempc->next=result;
	  result=tempc;
	  tempc->name=StrNew(tempm->full_name);
	}
      }
    }
    ll=ll->next;
  }
  return result;
}

U0 FMMarkUncollapsed(Ltf *l,FMUncollapsedList *tempc,U8 *cur_entry,U8 *next_entry)
{
  LtfEntry *ll=l->root.next;
  FMUncollapsedList *tempc1;
  LTDirEntry *tempm;
  while (ll!=l) {
    if (ll->type_u8==LTFT_TREE) {
      tempm=ll->user_data;
      tempc1=tempc;
      while (tempc1) {
	if (!StrCmp(tempc1->name,tempm->full_name)) {
	  ll->flags&=~LTFLF_CHECKED_COLLAPSED;
	  break;
	}
	tempc1=tempc1->next;
      }
      if (cur_entry) {
	if (!StrNCmp(cur_entry,tempm->full_name,StrLen(tempm->full_name))) {
	  l->cur_entry=ll;
	  if (StrLen(tempm->full_name)==StrLen(cur_entry))
	    cur_entry=NULL;
	} else if (next_entry) {
	  if (!StrNCmp(next_entry,tempm->full_name,StrLen(tempm->full_name))) {
	    l->cur_entry=ll;
	    if (StrLen(tempm->full_name)==StrLen(next_entry))
	      cur_entry=NULL;
	  }
	}
      }
    } else if (ll->type_u8==LTFT_MENU_VAL) {
      tempm=ll->user_data;
      if (cur_entry) {
	if (!StrNCmp(cur_entry,tempm->full_name,StrLen(tempm->full_name))) {
	  l->cur_entry=ll;
	  if (StrLen(tempm->full_name)==StrLen(cur_entry))
	    cur_entry=NULL;
	} else if (next_entry) {
	  if (!StrNCmp(next_entry,tempm->full_name,StrLen(tempm->full_name))) {
	    l->cur_entry=ll;
	    if (StrLen(tempm->full_name)==StrLen(next_entry))
	      cur_entry=NULL;
	  }
	}
      }
    }
    ll=ll->next;
  }
}

U0 FMDelUncollapsedList(FMUncollapsedList *tempc)
{
  FMUncollapsedList *tempc1;
  while (tempc) {
    tempc1=tempc->next;
    Free(tempc->name);
    Free(tempc);
    tempc=tempc1;
  }
}

LTDirEntry *FMRebuildLtfDrv(U8 drv_let,Ltf *l,LTDirEntry **root,BoolI8 init)
{
  LtfEntry *ll;
  LTDirEntry *tempm,*tempm1;
  U8 *st;
  tempm=CAlloc(sizeof(LTDirEntry));
  tempm->full_name=MSPrintF("%C:/",drv_let);
  tempm->attr=LT_ATTR_DIR;
  st=MSPrintF("%c:/*",drv_let);
  if (init)
    tempm->sub=tempm1=FilesFind(st,1<<FUf_RECURSE);
  else
    tempm1=NULL;
  Free(st);
  ll=LtfPrintF(l,"$$TR,\"%s\"$$",tempm->full_name);
  ll->user_data=tempm;
  tempm->user_data=ll;
  tempm->next=*root;
  *root=tempm;
  LtfPutS(l,"\n$$ID,+2$$");
  LtfBottom(l);
  if (init) {
    LTFSDirEntryFileLtf(l,tempm1);
    while (tempm1) {
      tempm1->parent=tempm;
      tempm1=tempm1->next;
    }
  }
  LtfPutS(l,"$$ID,-2$$");
  return tempm;
}

U0 FMRebuildLtf(Ltf **l_,LTDirEntry **root,I64 mode)
{
  LTPrt *p;
  I64 i;
  Ltf *l=*l_,*m,*b=sys_clipboard_ltf,*parent_ltf;
  FMUncollapsedList *tempc=NULL;
  U8 *cur_entry=NULL,*next_entry=NULL;
  LtfEntry *cl;
  LTDirEntry *tempm,*tempm1,*cur_tree_entry;
  if (!l)
    parent_ltf=LtfCur;
  else {
    parent_ltf=l->parent_ltf;
    LtfRemQue(l);
    LtfUnlock(l);
    WinSync;
    LtfLock(l);
    cur_tree_entry=NULL;
    cl=l->cur_entry;
    if (cl->type_u8==LTFT_TREE || cl->type_u8==LTFT_MENU_VAL)
      cur_tree_entry=cl->user_data;
    if (cur_tree_entry)
      cur_entry=StrNew(cur_tree_entry->full_name);
    tempm=NULL;
    if (cl!=l)
      cl=cl->next;
    while (cl!=l) {
      if (cl->type_u8==LTFT_TREE || cl->type_u8==LTFT_MENU_VAL)
	tempm=cl->user_data;
      else
	tempm=NULL;
      if (tempm) {
	tempm1=tempm->parent;
	while (tempm1) {
	  if (tempm1==cur_tree_entry) {
	    tempm=NULL;
	    break;
	  } else
	    tempm1=tempm1->parent;
	}
	if (tempm)
	  break;
      }
      cl=cl->next;
    }
    if (tempm)
      next_entry=StrNew(tempm->full_name);

    tempc=FMCollectUncollapsedList(l);
    LtfDel(l);
  }
  if (*root) {
    LTDirListDel(*root);
    *root=NULL;
  }
  l=LtfNew;
  StrCpy(l->desc,"FileMan");
  l->parent_ltf=parent_ltf;
  l->flags|=LTFF_FORM;
  switch (mode) {
    case FM_NORMAL:
      LtfPutS(l,"$$FG,PURPLE$$File Manager$$FG$$\n\n");
      break;
    case FM_PICK_FILE:
      LtfPutS(l,"$$FG,PURPLE$$Pick file and press <ESC>$$FG$$\n\n");
      l->flags|=LTFF_MIN_SIZE;
      break;
    case FM_PICK_DIR:
      LtfPutS(l,"$$FG,PURPLE$$Pick directory and press <ESC>$$FG$$\n\n");
      l->flags|=LTFF_MIN_SIZE;
      break;
  }
  m=LtfRead(FILEMANAGER_MENU_NAME);
  m->settings_root.dft_text_attr=WHITE<<4+LTBLUE;
  l->menu_ltf=m;
  for (i=0;i<NUM_LTPRTS;i++) {
    p=&local_prts[i];
    if (p->type==PT_LT||p->type==PT_FAT32)
      FMRebuildLtfDrv(Prt2Drv(p),l,root,TRUE);
    else if (p->type==PT_ISO9660) {
      if (p->bdev->flags&LTBDF_INITIALIZED)
	tempm=FMRebuildLtfDrv(Prt2Drv(p),l,root,TRUE);
      else {
	tempm=FMRebuildLtfDrv(Prt2Drv(p),l,root,FALSE);
	tempm->flags|=LTDEF_NOT_INITIALIZED;
      }
      tempm->flags|=LTDEF_REMOVABLE;
    }
  }
  LtfHome(l);
  FMMarkUncollapsed(l,tempc,cur_entry,next_entry);
  LtfCenter(l);
  LtfRst(b,TRUE);
  FMDelUncollapsedList(tempc);
  Free(cur_entry);
  Free(next_entry);
  *l_=l;
  LtfLock(l);
  LtfRemQue(LtfCur);
  LtfInsQue(l,Fs->last_ltf);
}

U0 FMRename(Ltf *l)
{
  EdFileNameStruct fn;
  LtfEntry *ll=l->cur_entry;
  LTDirEntry *tempm=NULL,*parent;
  if (ll->type_u8==LTFT_MENU_VAL) {
    tempm=ll->user_data;
    if (parent=tempm->parent) {
      Cd(parent->full_name);
      StrCpy(fn.name,tempm->name);
      if (LtfFormDo(fn.name,"EdFileNameStruct")) {
	Silent(ON);
	Move(tempm->name,fn.name);
	Silent(OFF);
      }
    }
  } else if (ll->type_u8==LTFT_TREE) {
    tempm=ll->user_data;
    if (parent=tempm->parent) {
      Cd(parent->full_name);
      StrCpy(fn.name,tempm->name);
      if (LtfFormDo(fn.name,"EdFileNameStruct")) {
	if (StrCmp(tempm->name,fn.name)) {
	  Silent(ON);
	  if (CopyTree(tempm->name,fn.name))
	    DelTree(tempm->name);
	  Silent(OFF);
	}
      }
    }
  }
}

U0 FMMkDir(Ltf *l)
{
  EdFileNameStruct fn;
  LtfEntry *ll=l->cur_entry;
  LTDirEntry *tempm=NULL,*parent;
  *fn.name=0;
  if (ll->type_u8==LTFT_MENU_VAL) {
    tempm=ll->user_data;
    if (parent=tempm->parent) {
      Cd(parent->full_name);
      if (LtfFormDo(fn.name,"EdFileNameStruct")) {
	Silent(ON);
	MkDir(fn.name);
	Silent(OFF);
      }
    }
  } else if (ll->type_u8==LTFT_TREE) {
    tempm=ll->user_data;
    Cd(tempm->full_name);
    if (LtfFormDo(fn.name,"EdFileNameStruct")) {
      Silent(ON);
      MkDir(fn.name);
      Silent(OFF);
    }
  }
}

U0 FMDelete(Ltf *l)
{
  U8 *st;
  LtfEntry *cl=l->cur_entry;
  LTDirEntry *tempm;
  if (cl->type_u8==LTFT_MENU_VAL) {
    tempm=cl->user_data;
    Silent(ON);
    st=MSPrintF("Delete: %s",tempm->full_name);
    if (PopUpCancelOk(st))
      Del(tempm->full_name);
    Free(st);
    Silent(OFF);
  } else if (cl->type_u8==LTFT_TREE) {
    tempm=cl->user_data;
    Silent(ON);
    st=MSPrintF("Delete: %s",tempm->full_name);
    if (PopUpCancelOk(st))
      DelTree(tempm->full_name);
    Free(st);
    Silent(OFF);
  }
}

U0 FMMount(Ltf *l)
{
  LtfEntry *cl=l->cur_entry;
  LTDirEntry *tempm;
  if (cl->type_u8==LTFT_TREE || cl->type_u8==LTFT_MENU_VAL)
    tempm=cl->user_data;
  else
    tempm=NULL;
  if (tempm) {
    while (tempm->parent)
      tempm=tempm->parent;
    Silent(ON);
    ChgDsk(*tempm->full_name);
    Silent(OFF);
  }
}

U0 FMFmtDrv(Ltf *l)
{
  LtfEntry *cl=l->cur_entry;
  LTDirEntry *tempm;
  U8 *st=NULL;
  if (cl->type_u8==LTFT_TREE || cl->type_u8==LTFT_MENU_VAL)
    tempm=cl->user_data;
  else
    tempm=NULL;
  if (tempm) {
    while (tempm->parent)
      tempm=tempm->parent;
    st=MSPrintF("Format Drive '%c'?\nAre You Sure?\n",*tempm->full_name);
    if (PopUpCancelOk(st)) {
      Silent(ON);
      Fmt(*tempm->full_name,TRUE,FALSE);
      Silent(OFF);
    }
  }
  Free(st);
}

U0 FMMakeISO(Ltf *l)
{
  LtfEntry *cl=l->cur_entry;
  LTDirEntry *tempm;
  U8 *st;
  if (cl->type_u8==LTFT_TREE || cl->type_u8==LTFT_MENU_VAL)
    tempm=cl->user_data;
  else
    tempm=NULL;
  if (tempm) {
    if (cl->type_u8==LTFT_MENU_VAL)
      tempm=tempm->parent;
    if (tempm && *tempm->full_name) {
      Silent(ON);
      if (tempm->full_name[StrLen(tempm->full_name)-1]=='/')
	st=MSPrintF("%s*",tempm->full_name);
      else
	st=MSPrintF("%s/*",tempm->full_name);
      CDFileCreate(,st);
      Free(st);
      Silent(OFF);
    }
  }
}

U0 FMBurnISO(Ltf *l)
{
  LtfEntry *cl=l->cur_entry;
  LTDirEntry *tempm;
  if (cl->type_u8==LTFT_TREE || cl->type_u8==LTFT_MENU_VAL)
    tempm=cl->user_data;
  else
    tempm=NULL;
  if (tempm) {
    while (tempm->parent)
      tempm=tempm->parent;
    Silent(ON);
    CDImageWrite(*tempm->full_name);
    Silent(OFF);
  }
}

U0 FMCopy(Ltf *l)
{
  Ltf *b=sys_clipboard_ltf;
  U8 *st;
  LtfEntry *cl=l->cur_entry,*ll;
  LTDirEntry *tempm,*tempm1=NULL,*tempm2;
  BoolI8 unlock=LtfLock(b);
  ll=b->root.next;

  tempm1=cl->user_data;
  if (cl->type_u8==LTFT_MENU_VAL)
    tempm1=tempm1->parent;
  else if (cl->type_u8!=LTFT_TREE)
    tempm1=NULL;
  if (tempm1) {
    while (ll!=b) {
      if (ll->type_u8==LTFT_MENU_VAL) {
	tempm=ll->user_data;
	tempm->flags|=LTDEF_PROCESSED;
	tempm2=tempm->parent;
	if (!tempm2 || !(tempm2->flags&LTDEF_PROCESSED)) {
	  Silent(ON);
	  Copy(tempm->full_name,tempm1->full_name);
	  Silent(OFF);
	}
      } else if (ll->type_u8==LTFT_TREE) {
	tempm=ll->user_data;
	tempm->flags|=LTDEF_PROCESSED;
	tempm2=tempm->parent;
	if (!tempm2 || !(tempm2->flags&LTDEF_PROCESSED)) {
	  Silent(ON);
	  if (*tempm1->name)
	    st=MSPrintF("%s/%s",tempm1->full_name,tempm->name);
	  else
	    st=MSPrintF("%s%s",tempm1->full_name,tempm->name);
	  CopyTree(tempm->full_name,st);
	  Free(st);
	  Silent(OFF);
	}
      }
      ll=ll->next;
    }
  }
  if (unlock)
    LtfUnlock(b);
}

#define FMR_INCLUDE		0
#define FMR_ADAM_INCLUDE	1
#define FMR_DELETE		2
#define FMR_RENAME		3
#define FMR_MKDIR		4
#define FMR_PLAIN		5
#define FMR_PASTE		6
#define FMR_CHG_DSK		7
#define FMR_FORMAT		8
#define FMR_MAKE_ISO		9
#define FMR_BURN_ISO		10
#define FMR_HELP		11

I64 PopUpCancelPasteDel(U8 *header=NULL,U8 *footer=NULL)
{
  I64 i;
  Ltf *l=LtfNew;
  if (header) LtfPutS(l,header);
  LtfPutS(l,"$$CM+LX,1,1 $$$$BT,\"INCLUDE\",FMR_INCLUDE$$"
	"$$CM+LX,27,0$$$$BT,\"ADAMINCLUDE\",FMR_ADAM_INCLUDE$$"
	"$$CM+LX,1,3 $$$$BT,\"DELETE\",FMR_DELETE$$"
	"$$CM+LX,27,0$$$$BT,\"RENAME\",FMR_RENAME$$"
	"$$CM+LX,1,3 $$$$BT,\"MAKEDIRECTORY\",FMR_MKDIR$$"
	"$$CM+LX,27,0$$$$BT,\"PLAIN-TEXTEDIT\",FMR_PLAIN$$"
	"$$CM+LX,1,3 $$$$BT,\"PASTECLIPBOARDFILES\",FMR_PASTE$$"
	"$$CM+LX,27,0$$$$BT,\"CHANGE DISK(MOUNTIT)\",FMR_CHG_DSK$$"
	"$$CM+LX,1,3 $$$$BT,\"FORMAT\",FMR_FORMAT$$"
	"$$CM+LX,1,3 $$$$BT,\"MAKEISO(CD-ROM)FILE\",FMR_MAKE_ISO$$"
	"$$CM+LX,27,0$$$$BT,\"BURNISO(CD-ROM)FILE\",FMR_BURN_ISO$$"
	"$$CM+LX,1,3 $$$$BT,\"HELP\",FMR_HELP$$"
	"$$CM+LX,27,0$$$$BT,\"CANCEL\",LTFM_CANCEL$$\n");
  if (footer) LtfPutS(l,footer);
  i=PopUpMenu(l);
  LtfDel(l);
  return i;
}

U0 FMRightClick()
{
  switch (PopUpCancelPasteDel) {
    case FMR_INCLUDE:
      Msg(MSG_KEY_DOWN,0,0x3F0000003F);
      break;
    case FMR_ADAM_INCLUDE:
      Msg(MSG_KEY_DOWN,0,0x23F0000023F);
      break;
    case FMR_DELETE:
      Msg(MSG_KEY_DOWN,CH_CTRLY,0);
      break;
    case FMR_RENAME:
      Msg(MSG_KEY_DOWN,'r',0);
      break;
    case FMR_MKDIR:
      Msg(MSG_KEY_DOWN,'d',0);
      break;
    case FMR_PLAIN:
      Msg(MSG_KEY_DOWN,CH_SHIFT_SPACE,0);
      break;
    case FMR_PASTE:
      Msg(MSG_KEY_DOWN,0,SC_INSERT+SCF_SHIFT);
      break;
    case FMR_CHG_DSK:
      Msg(MSG_KEY_DOWN,'m',0);
      break;
    case FMR_FORMAT:
      Msg(MSG_KEY_DOWN,'f',0);
      break;
    case FMR_MAKE_ISO:
      Msg(MSG_KEY_DOWN,'M',0);
      break;
    case FMR_BURN_ISO:
      Msg(MSG_KEY_DOWN,'B',0);
      break;
    case FMR_HELP:
      Msg(MSG_KEY_DOWN,0,SC_GUI);
      break;
  }
}


U8 *fm_ip_str=NULL;
U0 (*fp_old_final_screen_update)(GrDC *dc);

U0 FMFinalScreenUpdate(GrDC *dc)
{
  if (fm_ip_str) {
    dc->color=LTRED;
    GrPutS(dc,ipx,ipy,fm_ip_str);
  }
  (*fp_old_final_screen_update)(dc);
}

public U8 *FileMgr(I64 mode=FM_NORMAL,TaskStruct *mem_task=NULL)
{
  LTDirEntry *root=NULL,*tempm,*tempm1,*tempm2;
  I64 sc,ch,p1,p2,msg_code;
  Ltf *l=NULL,*old_ltf=LtfCur;
  U8 *result=NULL,*st,*st2,*old_cur_dir=CurDir;
  LtfEntry *cl=NULL,*ll;
  BoolI8 okay;

  SettingsPush; //See $LK,"SettingsPush","MN:SettingsPush"$
  fp_old_final_screen_update=fp_final_screen_update;

  MenuFilePush("::/LT/Doc/FileMgrPullDown.TXZ");
  if (mode==FM_NORMAL)
    AcctOneTimePopUp(ARf_FILE_MANAGER_HELP,
	"$$FG,GREEN$$<WINDOWS KEY>$$FG$$ for help.\n");

  FMRebuildLtf(&l,&root,mode);
  if (tempm1=LTCdDir(root,old_cur_dir))
    l->cur_entry=tempm1->user_data;
  while (tempm1) {
    if (tempm1->attr&LT_ATTR_DIR)
      tempm1->user_data(LtfEntry *)->flags&=~LTFLF_CHECKED_COLLAPSED;
    tempm1=tempm1->parent;
  }
  do {
    LtfUnlock(l);
    do msg_code=GetMsg(&p1,&p2,
	1<<MSG_KEY_DOWN|1<<MSG_IP_L_DOWN|1<<MSG_IP_L_UP|
	1<<MSG_IP_R_UP);
    while (Fs!=sys_focus_task);
    LtfLock(l);
    switch (msg_code) {
      case MSG_IP_R_UP:
	LtfUnlock(l);
	FMRightClick;
	LtfLock(l);
	break;
      case MSG_IP_L_DOWN:
	cl=l->cur_entry;
	fm_ip_str=cl->tag;
	fp_final_screen_update=&FMFinalScreenUpdate;
	break;
      case MSG_IP_L_UP:
	if (cl) {
	  fp_final_screen_update=fp_old_final_screen_update;
	  if (SetCursorPos(Fs,p1/FONT_WIDTH+Fs->win_left+Fs->win_scroll_x/FONT_WIDTH,p2/FONT_HEIGHT+Fs->win_top+Fs->win_scroll_y/FONT_HEIGHT,TRUE)) {
	    ll=l->cur_entry;
	    if (ll!=cl) {
	      st2=NULL;
	      if (ll->type_u8==LTFT_MENU_VAL) {
		tempm1=ll->user_data;
		if (tempm1=tempm1->parent)
		  st2=StrNew(tempm1->full_name);
	      } else if (ll->type_u8==LTFT_TREE) {
		tempm1=ll->user_data;
		st2=StrNew(tempm1->full_name);
	      }
	      if (st2 && cl->type_u8==LTFT_MENU_VAL) {
		tempm=cl->user_data;
		Silent(ON);
		Move(tempm->full_name,st2);
		Silent(OFF);
		FMRebuildLtf(&l,&root,mode);
	      } else if (st2 && cl->type_u8==LTFT_TREE) {
		tempm=cl->user_data;
		okay=TRUE;
		tempm2=tempm1;
		while (tempm2) {
		  if (tempm2!=tempm)
		    tempm2=tempm2->parent;
		  else {
		    okay=FALSE;
		    break;
		  }
		}
		if (okay) {
		  if (*tempm1->name)
		    st=MSPrintF("%s/%s",tempm1->full_name,tempm->name);
		  else
		    st=MSPrintF("%s%s",tempm1->full_name,tempm->name);
		  if (StrCmp(tempm->full_name,st)) {
		    Silent(ON);
		    CopyTree(tempm->full_name,st);
		    DelTree(tempm->full_name);
		    Silent(OFF);
		    FMRebuildLtf(&l,&root,mode);
		  }
		  Free(st);
		}
	      }
	      Free(st2);
	      FlushMsgs;
	    } else
	      if (ll->type_u8==LTFT_MENU_VAL) {
		LtfUnlock(l);
		Ed(ll->user_data(LTDirEntry *)->full_name);
		LtfLock(l);
	      }
	    cl=NULL;
	  }
	}
	break;
      case MSG_KEY_DOWN:
	cl=NULL;
	ch=p1; sc=p2;
	if (sc.u8[0]==SC_DELETE && !(sc&(SCF_SHIFT|SCF_CTRL)))
	  ch=CH_CTRLY;
	if (ch && sc&SCF_ALT) goto regular_key;
	switch (ch) {
	  case CH_NEW_LINE:
	    LtfUnlock(l);
	    FMRightClick;
	    LtfLock(l);
	    break;
	  case CH_CTRLV:
	    FMCopy(l);
	    FMRebuildLtf(&l,&root,mode);
	    break;
	  case 'r':
	    LtfUnlock(l);
	    FMRename(l);
	    FMRebuildLtf(&l,&root,mode);
	    break;
	  case 'd':
	    LtfUnlock(l);
	    FMMkDir(l);
	    FMRebuildLtf(&l,&root,mode);
	    break;
	  case CH_CTRLY:
	    LtfUnlock(l);
	    FMDelete(l);
	    FMRebuildLtf(&l,&root,mode);
	    break;
	  case 'm':
	    LtfUnlock(l);
	    FMMount(l);
	    FMRebuildLtf(&l,&root,mode);
	    break;
	  case 'M':
	    LtfUnlock(l);
	    FMMakeISO(l);
	    FMRebuildLtf(&l,&root,mode);
	    break;
	  case 'B':
	    LtfUnlock(l);
	    FMBurnISO(l);
	    FMRebuildLtf(&l,&root,mode);
	    break;
	  case 'f':
	    LtfUnlock(l);
	    FMFmtDrv(l);
	    FMRebuildLtf(&l,&root,mode);
	    break;
	  case CH_SHIFT_ESC:
	    break;
	  case CH_SPACE:
	    if (l->cur_entry->type_u8==LTFT_MENU_VAL) {
	      LtfUnlock(l);
	      Ed(l->cur_entry->user_data(LTDirEntry *)->full_name);
	      LtfLock(l);
	    } else
	      goto regular_key;
	    break;
	  case CH_SHIFT_SPACE:
	    if (l->cur_entry->type_u8==LTFT_MENU_VAL) {
	      LtfUnlock(l);
	      Plain(l->cur_entry->user_data(LTDirEntry *)->full_name);
	      LtfLock(l);
	    } else
	      goto regular_key;
	    break;
	  case CH_ESC:
	    cl=l->cur_entry;
	    tempm=cl->user_data;
	    if (mode==FM_PICK_FILE && cl->type_u8==LTFT_MENU_VAL)
	      result=StrNew(tempm->full_name,mem_task);
	    else if (mode==FM_PICK_DIR) {
	      if (cl->type_u8==LTFT_TREE)
		result=StrNew(tempm->full_name,mem_task);
	      else if (cl->type_u8==LTFT_MENU_VAL && (tempm=tempm->parent))
		result=StrNew(tempm->full_name,mem_task);
	    }
	    break;
	  default:
	    if (sc.u8[0]==SC_INSERT && sc&SCF_SHIFT && !(sc&SCF_CTRL)) {
	      FMCopy(l);
	      FMRebuildLtf(&l,&root,mode);
	    } else if (sc.u8[0]==SC_F5) {
	      if (l->cur_entry->type_u8==LTFT_MENU_VAL) {
		tempm=l->cur_entry->user_data;
		LtfUnlock(l);
		if (sc&SCF_SHIFT)
		  AdamFile(tempm->full_name);
		else
		  PopUpFile(tempm->full_name);
		LtfLock(l);
	      }
	    } else {
regular_key:
	      LtfUnlock(l);
	      PutKey(ch,sc);
	      LtfLock(l);
	    }
	}
	break;
    }
  } while (ch!=CH_ESC && ch!=CH_SHIFT_ESC);
  fp_final_screen_update=fp_old_final_screen_update;
  LtfRemQue(l);
  LtfInsQue(old_ltf,Fs->last_ltf);
  SettingsPop;
  LtfDel(l);
  LTDirListDel(root);
  Cd(old_cur_dir);
  Free(old_cur_dir);
  if (mode!=FM_NORMAL && !result)
    result=StrNew("",mem_task);
  MenuPop;
  return result;
}
#help_index ""
