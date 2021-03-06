#help_index "Ltf/File"
public U0 LtfLoad(Ltf *l,U8 *src2,U64 size)
{
  I64 i;
  U8 *src;
  BoolI8 unlock=LtfLock(l);
  LtfBinEntry *tempb;
  l->find_replace->filter_lines=0;
  if (src2) {
    LtfPutS(l,src2);
    src=src2+StrLen(src2)+1;
    i=size-(offset(LtfBinEntry.data)-offset(LtfBinEntry.start));
    while (src<=src2+i) {
      tempb=CAlloc(sizeof(LtfBinEntry),l->mem_task);
      MemCpy(&tempb->start,src,offset(LtfBinEntry.data)-offset(LtfBinEntry.start));
      src+=offset(LtfBinEntry.data)-offset(LtfBinEntry.start);
      tempb->data=MAlloc(tempb->size,l->mem_task);
      if (tempb->size) {
	MemCpy(tempb->data,src,tempb->size);
	src+=tempb->size;
      }
      InsQue(tempb,l->bin_root.last);
      if (tempb->num>=l->cur_bin_num)
	l->cur_bin_num=tempb->num+1;
    }
  }
  if (!(l->flags & (LTFF_PLAIN_TEXT|LTFF_PLAIN_TEXT_WITH_TABS)))
    LtfBinsValidate(l);
  LtfHome(l); //LTFT_CURSOR will set it in recalc
  LtfRecalc(l);
  if (unlock)
    LtfUnlock(l);
}

public Ltf *LtfRead(U8 *name,U64 flags=0)
{
  Ltf *l=LtfNew;
  U8 *src,*name2;
  U64 size=0;
  l->flags|=flags;
  name2=FileNameAbs(name);
  StrCpy(l->filename.name,name2);
  src=FileRead(name2,&size,&l->file_attr);
  Free(name2);

  if (src) {
    LtfLoad(l,src,size);
    Free(src);
  }
  return l;
}

public U0 *LtfSave(Ltf *l,U64 *size=NULL)
{
  LtfEntry *cl;
  LtfBinEntry *b;
  BoolI8 unlock=LtfLock(l);
  U8 *st;
  U64 cnt;
  U8 *result,*dst,*src,ch;
  I64 i;

  if (l->flags&LTFF_NO_CURSOR)
    cnt=1; //terminator
  else
    cnt=2; //cursor + terminator

  if (!(l->flags & (LTFF_PLAIN_TEXT|LTFF_PLAIN_TEXT_WITH_TABS)))
    LtfBinsValidate(l);
  LtfRecalc(l);
  for (cl=l->root.next;cl!=l;cl=cl->next) {
    if (!Bt(ltf_data_types,cl->type_u8)) {
      switch (cl->type_u8) {
	case LTFT_TAB:
	case LTFT_PAGE_BREAK:
	  cnt++;
	  break;
	case LTFT_NEW_LINE:
	  if (l->flags&LTFF_CARRIAGE_RETURN)
	    cnt+=2;
	  else
	    cnt++;
	  break;
	case LTFT_TEXT://$BK,1$Update $LK,"LTFT_TEXT","FF:D:/LT/Compiler/Lex.CPZ,LTFT_TEXT"$$BK,0$
	  if (!(cl->flags & ~(LTF_BL_IV_UL|LTFLF_WORD_WRAP|LTFLF_HIGHLIGHT|LTFLF_LOCK_TO_PAGE|LTFLF_LOCK_TO_CENTER|LTFLF_SKIP|LTFLF_HARD_SKIP|LTFLF_FILTER_SKIP))) {
	    cnt+=StrLen(cl->tag);
	    if (!(l->flags & (LTFF_PLAIN_TEXT | LTFF_PLAIN_TEXT_WITH_TABS)) ||
	      l->flags&LTFF_DBL_DOLLARS)
	      cnt+=StrOcc(cl->tag,'$$');
	  } else {
	    st=Ltf2PlainText(l,cl);
	    cnt+=StrLen(st)+2;
	    Free(st);
	  }
	  break;
	default:
	  if (cl->type_u8!=LTFT_SOFT_NEW_LINE &&
	      cl->type_u8!=LTFT_CURSOR &&
	      cl->type_u8!=LTFT_BPT) {
	    st=Ltf2PlainText(l,cl);
	    cnt+=StrLen(st)+2;
	    Free(st);
	  }
       }
    }
  }
  for (b=l->bin_root.next;b!=&l->bin_root;b=b->next) {
    if (b->use_cnt>b->temp_use_cnt)
      cnt+=offset(LtfBinEntry.data)-
	   offset(LtfBinEntry.start)+b->size;
  }
  result=MAlloc(cnt);
  dst=result;
  for (cl=l->root.next;cl!=l;cl=cl->next) {
    if (!Bt(ltf_data_types,cl->type_u8)) {
      if (cl->type_u8==LTFT_TEXT &&
	!(cl->flags & ~(LTF_BL_IV_UL|LTFLF_WORD_WRAP|LTFLF_HIGHLIGHT|LTFLF_LOCK_TO_PAGE|LTFLF_LOCK_TO_CENTER|LTFLF_SKIP|LTFLF_HARD_SKIP|LTFLF_FILTER_SKIP))) {
	src=cl->tag;
	i=0;
	while (ch=*src++) {
	  if (cl==l->cur_entry && i++==l->cur_data_col && !(l->flags&LTFF_NO_CURSOR))
	    *dst++=CH_CURSOR;
	  *dst++=ch;
	  if (ch=='$$') {
	    if (!(l->flags & (LTFF_PLAIN_TEXT | LTFF_PLAIN_TEXT_WITH_TABS)) ||
	      l->flags&LTFF_DBL_DOLLARS)
	      *dst++=ch;
	  }
	}
	if (cl==l->cur_entry && i++==l->cur_data_col && !(l->flags&LTFF_NO_CURSOR))
	  *dst++=CH_CURSOR;
      } else {
	if (cl==l->cur_entry && !(l->flags&LTFF_NO_CURSOR))
	  *dst++=CH_CURSOR;
	switch (cl->type_u8) {
	  case LTFT_TAB:
	    *dst++=CH_TAB;
	    break;
	  case LTFT_PAGE_BREAK:
	    *dst++=CH_FORM_FEED;
	    break;
	  case LTFT_NEW_LINE:
	    if (l->flags&LTFF_CARRIAGE_RETURN)
	      *dst++=CH_CARRIAGE_RETURN;
	    *dst++=CH_NEW_LINE;
	    break;
	  default:
	    if (cl->type_u8!=LTFT_SOFT_NEW_LINE &&
		cl->type_u8!=LTFT_CURSOR &&
		cl->type_u8!=LTFT_BPT) {
	      *dst++='$$';
	      st=Ltf2PlainText(l,cl);
	      StrCpy(dst,st);
	      dst+=StrLen(st);
	      *dst++='$$';
	      Free(st);
	    }
	    break;
	}
      }
    }
  }
  *dst++=0;
  b=l->bin_root.next;
  if (b!=&l->bin_root) {
    do {
      if (b->use_cnt>b->temp_use_cnt) {
	MemCpy(dst,&b->type,offset(LtfBinEntry.data)-offset(LtfBinEntry.start));
	dst+=offset(LtfBinEntry.data)-offset(LtfBinEntry.start);
	MemCpy(dst,b->data,b->size);
	dst+=b->size;
      }
      b=b->next;
    } while (b!=&l->bin_root);
  } else
    cnt--;
  if (size) *size=cnt;
  if (unlock)
    LtfUnlock(l);
  return result;
}

public U0 LtfWrite(Ltf *l,BoolI8 prompt=FALSE)
{
  U64 size;
  U8 *buf=LtfSave(l,&size);
  if (prompt) {
    if (LtfFormDo(&l->filename,"EdFileNameStruct"))
      FileWrite(l->filename.name,buf,size,0,l->file_attr);
  } else
    FileWrite(l->filename.name,buf,size,0,l->file_attr);
  Free(buf);
}

#help_index "Ltf"
public U0 LtfInsertLtf(Ltf *l,Ltf *b)
{  //TODO: $LK,"LtfRst","MN:LtfRst"$
  U8 *dst;
  BoolI8 unlock_l=LtfLock(l),
	 unlock_b=LtfLock(b);
  LtfEntry *nl,*cl=b->root.next;
  LtfRemSoftNewLines(l,NULL);
  if (l->cur_entry->type_u8==LTFT_TEXT &&
      l->cur_data_col>l->cur_entry->min_col) {
    if (l->cur_data_col<l->cur_entry->max_col) {
      nl=CAlloc(sizeof(LtfEntryBase)+sizeof(U0 *),l->mem_task);
      nl->type=LTFT_TEXT | l->cur_entry->type & 0xFFFFFF00;
      nl->flags=l->cur_entry->flags;
      nl->x=l->cur_entry->x;
      nl->y=l->cur_entry->y;
      nl->page_line_num=l->cur_entry->page_line_num;
      dst=l->cur_entry->tag+l->cur_data_col;
      nl->tag=StrNew(dst,l->mem_task);
      nl->max_col=StrLen(dst);
      *dst=0;
      l->cur_entry->max_col=l->cur_data_col;
      InsQue(nl,l->cur_entry);
      l->cur_entry=nl;
      l->cur_data_col=nl->min_col;
    } else
      if (l->cur_entry!=l)
	l->cur_entry=l->cur_entry->next;
  }
  while (cl!=b) {
    if (cl->type_u8!=LTFT_SOFT_NEW_LINE) {
      nl=LtfEntryCopy(l,cl);
      InsQue(nl,l->cur_entry->last);
    }
    cl=cl->next;
  }
  LtfRecalc(l);
  if (unlock_b)
    LtfUnlock(b);
  if (unlock_l)
    LtfUnlock(l);
}

#help_index "Ltf/Clipboard"
public U0 ClipBoardDel()
{
  LtfRst(sys_clipboard_ltf,TRUE);
}

public U0 EdCopyToClipboard(Ltf *l)
{
  Ltf *b=sys_clipboard_ltf;
  BoolI8 unlock_l=LtfLock(l),
	 unlock_b=LtfLock(b);
  LtfEntry *cl=l->root.next,*nl;
  ClipBoardDel;
  while (cl!=l) {
    if (cl->type & LTFLT_SELECTED) {
      cl->type&=~LTFLT_SELECTED;
      if (!Bt(ltf_data_types,cl->type_u8)) {
	nl=LtfEntryCopy(b,cl);
	InsQue(nl,b->root.last);
      }
    }
    cl=cl->next;
  }
  if (unlock_b)
    LtfUnlock(b);
  if (unlock_l)
    LtfUnlock(l);
}

public U0 EdCutToClipboard(Ltf *l)
{
  Ltf *b=sys_clipboard_ltf;
  BoolI8 unlock_l=LtfLock(l),
	 unlock_b=LtfLock(b);
  LtfEntry *cl=l->root.next,*cl1,*nl,*cl2=NULL;
  ClipBoardDel;
  while (cl!=l) {
    cl1=cl->next;
    if (cl->type & LTFLT_SELECTED) {
      cl->type&=~LTFLT_SELECTED;
      if (!Bt(ltf_data_types,cl->type_u8)) {
	nl=LtfEntryCopy(b,cl);
	InsQue(nl,b->root.last);
      }
      if (cl==l->cur_entry || cl==cl2)
	cl2=cl->next;
      LtfEntryDel(l,cl);
    }
    cl=cl1;
  }
  if (cl2) {
    l->cur_entry=cl2;
    l->cur_data_col=cl2->min_col;
  }
  l->recalc_start=l;
  LtfRemSoftNewLines(l,NULL);
  if (unlock_b)
    LtfUnlock(b);
  if (unlock_l)
    LtfUnlock(l);
}

public U0 EdPasteClipboard(Ltf *l)
{
  Ltf *b=sys_clipboard_ltf;
  BoolI8 unlock_l=LtfLock(l),
      unlock_b=LtfLock(b);
  LtfEntry *cl=l->cur_entry,*ll;
  if (cl->type_u8==LTFT_DATA) {
    ll=b->root.next;
    while (ll!=b) {
      if (Bt(ltf_tag_types,ll->type_u8))
	LtfPutSPartial(l,ll->tag);
      ll=ll->next;
    }
  } else
    LtfInsertLtf(l,b);
  if (unlock_b)
    LtfUnlock(b);
  if (unlock_l)
    LtfUnlock(l);
}

public U0 LtfCut(Ltf *l,LtfEntry *start,LtfEntry *end)
{ //deletes including end points
  BoolI8 unlock_l=LtfLock(l);
  LtfEntry *cl1;
  while (start!=l) {
    cl1=start->next;
    if (start==l->cur_entry) {
      l->cur_entry=start->next;
      l->cur_data_col=l->cur_entry->min_col;
    }
    LtfEntryDel(l,start);
    if (start==end)
      break;
    start=cl1;
  }
  l->recalc_start=l;
  LtfRemSoftNewLines(l,NULL);
  if (unlock_l)
    LtfUnlock(l);
}

public Ltf *LtfCopy(Ltf *l,LtfEntry *start,LtfEntry *end)
{  //copies including end points
  Ltf *b=LtfNew;
  BoolI8 unlock_l=LtfLock(l);
  LtfEntry *nl;
  while (start!=l) {
    if (!Bt(ltf_data_types,start->type_u8)) {
      nl=LtfEntryCopy(b,start);
      InsQue(nl,b->root.last);
    }
    if (start==end)
      break;
    start=start->next;
  }
  if (unlock_l)
    LtfUnlock(l);
  return b;
}

#help_index "Ltf;Compiler/Directive"

public U0 StreamLtf(Ltf *b)
{  //TODO: $LK,"LtfRst","MN:LtfRst"$
  BoolI8 unlock_b=LtfLock(b);
  LtfEntry *cl=b->root.next;
  while (cl!=b) {
    if (cl->type_u8==LTFT_TEXT)
      StreamPutS(cl->tag);
    else if (cl->type_u8==LTFT_NEW_LINE)
      StreamPutS("\n");
    else if (cl->type_u8==LTFT_TAB)
      StreamPutS("\t");
    cl=cl->next;
  }
  if (unlock_b)
    LtfUnlock(b);
}

#help_index "Ltf"

BoolI64 LtfCaptureUndo(Ltf *l,BoolI8 force=FALSE)
{
  BoolI8 result=FALSE,unlock=LtfLock(l);
  U64 i=GetTimeStamp;
  LtfUndo *u;
  if ((force || l->flags&LTFF_UNDO_DIRTY &&
      i>l->undo_root.last->time_stamp+15*time_stamp_freq) &&
      l->flags&LTFF_ALLOW_UNDO) {
    u=CAlloc(sizeof(LtfUndo),l->mem_task);
    u->time_stamp=i;
    u->body=LtfSave(l,&u->size);
    InsQue(u,l->undo_root.last);
    l->flags&=~LTFF_UNDO_DIRTY;
    l->undo_cnt++;
    u->ltf_flags=l->flags;
  }
  if (unlock)
    LtfUnlock(l);
  return result;
}
 
U0 LtfUndoRestore(Ltf *l)
{
  BoolI8 unlock=LtfLock(l);
  LtfUndo *u=l->undo_root.last,*u_next,*u_last;
  Ltf *m;
  if (u!=&l->undo_root) {
    RemQue(u);
    u_next=l->undo_root.next;
    u_last=l->undo_root.last;
    l->undo_root.next=l->undo_root.last=&l->undo_root;
    m=l->menu_ltf;
    LtfRst(l,TRUE);
    l->flags=u->ltf_flags;
    LtfLoad(l,u->body,u->size);
    LtfUndoDel(l,u);
    l->undo_root.next=u_next;
    l->undo_root.last=u_last;
    l->menu_ltf=m;
  }
  LtfUndoCntSet(l);
  l->flags&=~LTFF_UNDO_DIRTY;
  if (unlock)
    LtfUnlock(l);
}


#help_index "File/Cmd Line (Typically);Ltf/Cmd Line (Typically);Cmd Line (Typically)"

public U0 Type(U8 *name)
//Output a file to the cmd line.
//Even graphics and macros will be sent.
{
  Ltf *l=LtfRead(name);
  if (LtfCur)
    LtfInsertLtf(LtfCur,l);
  LtfDel(l);
}
#help_index "Ltf"
