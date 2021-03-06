#help_index "Debugging;Utils;Cmd Line (Typically)"

I64 LinkChkLtf(Ltf *l,U64 *_fuf_flags)
{
  I64 result=0;
  U8 *st;
  Ltf *old_ltf=LtfCur,*l2;
  LtfEntry *ll=l->root.next;

  while (ll!=l) {
    if (ScanKey) {
      LBts(_fuf_flags,FUf_CANCEL);
      break;
    }
    if (ll->type_u8==LTFT_LINK) {
      if (ll->flags & LTFLF_AUX_STR)
	st=ll->aux_str;
      else
	st=ll->tag;
      LtfRemQue(old_ltf);
      LtfInsQue(l,Fs->last_ltf);
      l2=LtfLinkChk(st);
      LtfRemQue(l);
      LtfInsQue(old_ltf,Fs->last_ltf);
      if (l2)
	LtfDel(l2);
      else {
	PutFileLink(l->filename.name,ll->y);
	"$$FG,RED$$ %04d:%s\n$$FG$$",ll->y,st;
	result++;
      }
    } else if (ll->type_u8==LTFT_ERROR) {
      PutFileLink(l->filename.name,ll->y);
      "$$FG,RED$$ Ltf Error\n$$FG$$";
      result++;
    }
    ll=ll->next;
  }
  return result;
}

I64 LinkChkFile(U8 *filename,U64 *_fuf_flags)
{
  I64 result;
  Ltf *l=LtfRead(filename,LTFF_NO_CURSOR);
  result=LinkChkLtf(l,_fuf_flags);
  LtfDel(l);
  return result;
}

I64 LinkChkList(LTDirEntry *tempm1,U64 *_fuf_flags)
{
  I64 result=0;
  LTDirEntry *tempm2;

  while (tempm1 && !Bt(_fuf_flags,FUf_CANCEL)) {
    tempm2=tempm1->next;
    if (tempm1->attr & LT_ATTR_DIR) {
      if (tempm1->sub) {
	"%S %s\n","ST_SCANNING_DIR",tempm1->full_name;
	result+=LinkChkList(tempm1->sub,_fuf_flags);
      }
    } else
      result+=LinkChkFile(tempm1->full_name,_fuf_flags);
    LTDirEntryDel(tempm1);
    tempm1=tempm2;
  }
  return result;
}

public I64 LinkChk(U8 *files_find_mask=TEXT_FILE_MASK,U8 *fu_flags=NULL)
//Checks for broken file links.
{
  U64 fuf_flags=0;
  GetFFlags(&fuf_flags,Define("ST_FILE_UTIL_FLAGS"),"+r");
  GetFFlags(&fuf_flags,Define("ST_FILE_UTIL_FLAGS"),fu_flags);
  return LinkChkList(FilesFind(files_find_mask,fuf_flags),&fuf_flags);
}
#help_index ""
