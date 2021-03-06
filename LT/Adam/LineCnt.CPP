#help_index "Utils;Cmd Line (Typically)"

U64 LineCntRepFiles(LTDirEntry *tempm)
{
  Ltf *l;
  U64 result=0,i;
  while (tempm) {
    i=0;
    if (tempm->attr&LT_ATTR_DIR)
      i=LineCntRepFiles(tempm->sub);
    else {
      if (IsSrcFile(tempm->name)) {
	l=LtfRead(tempm->full_name);
	if (l->root.next!=l)
	  i=l->root.last->y+2;
	LtfDel(l);
      }
    }
    tempm->user_data=i;
    result+=i;
    tempm=tempm->next;
  }
  return result;
}

U0 PutLineCntFiles(Ltf *l,LTDirEntry *tempm)
{
  while (tempm) {
    if (tempm->attr&LT_ATTR_DIR) {
      LtfPrintF(l,"%8,d $$TR+C,\"%s\"$$\n$$ID,2$$",tempm->user_data,tempm->name);
      PutLineCntFiles(l,tempm->sub);
      LtfPutS(l,"$$ID,-2$$");
    } else {
      if (IsSrcFile(tempm->name)) {
	LtfPrintF(l,"%8,d ",tempm->user_data);
	LtfPrintF(l,"$$LK,\"%s\",\"FI:%s\"$$\n",
	  tempm->name,tempm->full_name);
      }
    }
    tempm=tempm->next;
  }
}

public U64 LineCntRep(U8 *files_find_mask="/LT/*",U8 *fu_flags=NULL)
{
  U64 fuf_flags=0,result=0;
  Ltf *l=LtfNew;
  LTDirEntry *tempm1=NULL;
  GetFFlags(&fuf_flags,Define("ST_FILE_UTIL_FLAGS"),"+r");
  GetFFlags(&fuf_flags,Define("ST_FILE_UTIL_FLAGS"),fu_flags);
  if (tempm1=FilesFind(files_find_mask,fuf_flags)) {
    result=LineCntRepFiles(tempm1);
    LtfPrintF(l,"%8,d $$TR+C,\"%s\"$$\n$$ID,2$$",result,files_find_mask);
    PutLineCntFiles(l,tempm1);
    LtfPutS(l,"$$ID,-2$$");
    LTDirListDel(tempm1);
  }
  LtfInsertLtf(LtfCur,l);
  LtfDel(l);
  return result;
}

U64 TreeSizeRepFiles(LTDirEntry *tempm)
{
  U64 result=0,i;
  while (tempm) {
    if (tempm->attr&LT_ATTR_DIR)
      i=TreeSizeRepFiles(tempm->sub);
    else
      i=tempm->size;
    tempm->user_data=i;
    result+=i;
    tempm=tempm->next;
  }
  return result;
}

U0 PutTreeSizeFiles(Ltf *l,LTDirEntry *tempm)
{
  while (tempm) {
    if (tempm->attr&LT_ATTR_DIR) {
      LtfPrintF(l,"%10,d $$TR+C,\"%s\"$$\n$$ID,2$$",tempm->user_data,tempm->name);
      PutTreeSizeFiles(l,tempm->sub);
      LtfPutS(l,"$$ID,-2$$");
    } else {
      LtfPrintF(l,"%10,d ",tempm->user_data);
      LtfPrintF(l,"$$LK,\"%s\",\"FI:%s\"$$\n",
	  tempm->name,tempm->full_name);
    }
    tempm=tempm->next;
  }
}

public U64 TreeSizeRep(U8 *files_find_mask="/LT/*",U8 *fu_flags=NULL)
{
  U64 fuf_flags=0,result=0;
  Ltf *l=LtfNew;
  LTDirEntry *tempm1=NULL;
  GetFFlags(&fuf_flags,Define("ST_FILE_UTIL_FLAGS"),"+r");
  GetFFlags(&fuf_flags,Define("ST_FILE_UTIL_FLAGS"),fu_flags);
  if (tempm1=FilesFind(files_find_mask,fuf_flags)) {
    result=TreeSizeRepFiles(tempm1);
    LtfPrintF(l,"%10,d $$TR+C,\"%s\"$$\n$$ID,2$$",result,files_find_mask);
    PutTreeSizeFiles(l,tempm1);
    LtfPutS(l,"$$ID,-2$$");
    LTDirListDel(tempm1);
  }
  LtfInsertLtf(LtfCur,l);
  LtfDel(l);
  return result;
}

#help_index ""
