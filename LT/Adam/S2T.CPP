#help_index "Utils;Cmd Line (Typically)"

//$LK,"StrUtil","MN:StrUtil"$() operations on files

public U0 SUFile(U8 *name,U64 su_flags,F64 indent_scale_factor)
{//String utility on a single file
//See $LK,"SU Flags","MN:SUF_SINGLE_SPACE"$
  U8 *dst;
  BoolI8 chged=FALSE;
  I64 reduced=0;
  Ltf *l=LtfRead(name,LTFF_PLAIN_TEXT_WITH_TABS | LTFF_NO_CURSOR);
  LtfEntry *ll=l->root.next;
  while (ll!=l) {
    if (ll->type_u8==LTFT_TEXT) {
      dst=MStrUtil(ll->tag,su_flags,indent_scale_factor);
      if (StrCmp(dst,ll->tag)) {
	reduced+=StrLen(ll->tag)-StrLen(dst);
	chged=TRUE;
	Free(ll->tag);
	ll->tag=dst;
      } else
	Free(dst);
    }
    ll=ll->next;
  }
  if (chged) {
    "Reduced %s by %d chars\n",name,reduced;
    StrCpy(l->filename.name,name);
    LtfWrite(l);
  }
  LtfDel(l);
}

U0 SUList(LTDirEntry *tempm1,U64 su_flags,F64 indent_scale_factor)
{
  LTDirEntry *tempm2;
  while (tempm1) {
    tempm2=tempm1->next;
    if (tempm1->attr & LT_ATTR_DIR) {
      if (tempm1->sub) {
	"%S%s\n","ST_SCANNING_DIR",tempm1->full_name;
	SUList(tempm1->sub,su_flags,indent_scale_factor);
      }
    } else
      SUFile(tempm1->full_name,su_flags,indent_scale_factor);
    LTDirEntryDel(tempm1);
    tempm1=tempm2;
  }
}

public U0 SU(U8 *files_find_mask,U64 su_flags,U8 *fu_flags=NULL,F64 indent_scale_factor=0)
{//Apply string utilities to files
//You can convert spaces to tabs, for example
//or removing trailing spaces on lines.
//See $LK,"SU Flags","MN:SUF_SINGLE_SPACE"$
  U64 fuf_flags=0;
  GetFFlags(&fuf_flags,Define("ST_FILE_UTIL_FLAGS"),fu_flags);
  SUList(FilesFind(files_find_mask,fuf_flags),su_flags,indent_scale_factor);
}

public U0 S2T(U8 *files_find_mask,U8 *fu_flags=NULL)
{
//Spaces to tabs.
//Use "Hard Space" (SHIFT-SPACE) for spaces
//in string constants in your code.
  SU(files_find_mask,SUF_S2T|SUF_REM_TRAILING,fu_flags);
}

#help_index ""
