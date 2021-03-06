BoolI64 LTDirEntryNew(LTPrt *p,U8 *cur_dir,LTDirEntry *tempm,BoolI8 free_old_chain=TRUE)
{
  switch (p->type) {
    case PT_LT:
      return LTFSDirEntryNew(p,cur_dir,tempm,free_old_chain);
    case PT_FAT32:
      return FAT32DirEntryNew(p,cur_dir,tempm,free_old_chain);
    case PT_ISO9660:
      PutDefineErr("ST_NOT_WRITABLE");
      return FALSE;
    default:
      PutDefineErr("ST_UNSUPPORTED_FILE_SYSTEM");
      return FALSE;
  }
}

U0 LTDirEntryDel(LTDirEntry *tempm)
{ //Doesn't Free user_data
  if (tempm) {
    Free(tempm->full_name);
    Free(tempm);
  }
}

U0 LTDirEntryDel2(LTDirEntry *tempm)
{ //Frees user_data
  if (tempm) {
    Free(tempm->full_name);
    Free(tempm->user_data);
    Free(tempm);
  }
}

U0 LTDirListDel(LTDirEntry *tempm)
{ //Doesn't Free user_data
  LTDirEntry *tempm2;
  while (tempm) {
    tempm2=tempm->next;
    if (tempm->sub)
      LTDirListDel(tempm->sub);
    LTDirEntryDel(tempm);
    tempm=tempm2;
  }
}

U0 LTDirListDel2(LTDirEntry *tempm)
{ //Frees user_data
  LTDirEntry *tempm2;
  while (tempm) {
    tempm2=tempm->next;
    if (tempm->sub)
      LTDirListDel2(tempm->sub);
    LTDirEntryDel2(tempm);
    tempm=tempm2;
  }
}

I64 LTDirCompare(LTDirEntry *e1,LTDirEntry *e2)
{
  U8 buf1[LT_MAX_FILENAME_LEN+1],buf2[LT_MAX_FILENAME_LEN+1];
  U8 buf3[LT_MAX_FILENAME_LEN+1],buf4[LT_MAX_FILENAME_LEN+1];
  I64 d1=0,d2=0;
  if (e1->attr & LT_ATTR_DIR)
    d1=1;
  if (e2->attr & LT_ATTR_DIR)
    d2=1;
  if (d1!=d2)
    return d2-d1;
  else {
    StrCpy(buf1,e1->name);
    StrCpy(buf2,e2->name);
    FileExtRem(buf1,buf3);
    FileExtRem(buf2,buf4);
    if (d1=StrCmp(buf3,buf4))
      return d1;
    return StrCmp(buf1,buf2);
  }
}

U0 LTDirFilesSort(LTDirEntry **_tempm)
{
  I64 i,cnt=0;
  LTDirEntry *tempm=*_tempm,*tempm1,**sort_buf;
  if (tempm) {
    tempm1=tempm;
    while (tempm1) {
      cnt++;
      tempm1=tempm1->next;
    }
    if (cnt>1) {
      sort_buf=MAlloc(cnt*sizeof(U0 *));
      i=0;
      tempm1=tempm;
      while (tempm1) {
	sort_buf[i++]=tempm1;
	tempm1=tempm1->next;
      }
      QSortU64(sort_buf,cnt,&LTDirCompare);
      tempm=sort_buf[0];
      *_tempm=tempm;
      for (i=0;i<cnt-1;i++) {
	tempm1=sort_buf[i];
	tempm1->next=sort_buf[i+1];
      }
      tempm1=sort_buf[i];
      tempm1->next=NULL;
      Free(sort_buf);

      tempm1=tempm;
      while (tempm1) {
	if (tempm1->sub)
	  LTDirFilesSort(&tempm1->sub);
	tempm1=tempm1->next;
      }
    } else
      if (tempm->sub)
	LTDirFilesSort(&tempm->sub);
  }
}

LTDirEntry *LTDirFilesFlatten(LTDirEntry *tempm,LTDirEntry **result,U64 fuf_flags)
{ //returns last node
  LTDirEntry *tempm1;
  BoolI8 del;
  if (tempm)
    while (TRUE) {
      tempm1=tempm->next;
      if (!(tempm->attr&LT_ATTR_DIR)||!(fuf_flags&FUF_JUST_FILES)) {
	result=*result=tempm;
	del=FALSE;
      } else
	del=TRUE;
      if (tempm->sub) {
	result=LTDirFilesFlatten(tempm->sub,result,fuf_flags);
	tempm->sub=NULL;
      }
      if (del)
	LTDirEntryDel(tempm);
      if (tempm1)
	tempm=tempm1;
      else
	break;
    }
  *result=NULL;
  return result;
}

LTDirEntry *SysFilesFind(U8 *files_find_mask,U64 fuf_flags)
{
  LTPrt *p=Fs->cur_prt;
  LTDirEntry *result=NULL;
  PrtChk(p);
  switch (p->type) {
    case PT_LT:
      result=LTFSFilesFind(files_find_mask,fuf_flags);
      break;
    case PT_FAT32:
      result=FAT32FilesFind(files_find_mask,fuf_flags);
      break;
    case PT_ISO9660:
      result=ISO1FilesFind(files_find_mask,fuf_flags);
      break;
    default:
      PutDefineErr("ST_UNSUPPORTED_FILE_SYSTEM");
      result=NULL;
  }
  if (result) {
    LTDirFilesSort(&result);
    if (fuf_flags&(FUF_FLATTEN_TREE|FUF_JUST_FILES))
      LTDirFilesFlatten(result,&result,fuf_flags);
  }
  return result;
}

U0 PutFileLink(U8 *filename,I64 line=0,BoolI8 plain_text=FALSE)
{
  U8 *buf;
  if (!filename) return;
  if (IsRaw) {
    if (line)
      "%s,%04d",filename,line;
    else
      "" filename;
  } else {
    buf=FileNameAbs(filename);
    if (plain_text) {
      if (line)
	"$$LK,\"%s,%04d\",\"PL:%s,%d\"$$",filename,line,buf,line;
      else
	"$$LK,\"%s\",\"PI:%s\"$$",filename,buf;
    } else {
      if (line)
	"$$LK,\"%s,%04d\",\"FL:%s,%d\"$$",filename,line,buf,line;
      else
	"$$LK,\"%s\",\"FI:%s\"$$",filename,buf;
    }
    Free(buf);
  }
}

U0 PutDirLink(U8 *dirname)
{
  U8 *buf;
  if (!dirname) return;
  if (IsRaw)
    "" dirname;
  else {
    buf=DirNameAbs(dirname);
    "$$MA+A,\"%s\",\"Cd(\\\"%s\\\");Dir;\n\"$$",dirname,buf;
    Free(buf);
  }
}

U0 DirLT(U8 *files_find_mask,U64 fuf_flags,BoolI8 full)
{
  LTDirEntry *tempm1=NULL,*tempm2;
  U8 *st;
  I64 csize=0xFFFF,esize=0xFFFF,c=0xFFFF;
  tempm1=SysFilesFind(files_find_mask,fuf_flags);

  if (!(st=CurDir))
    PutDefine("ST_INVALID_DRV");
  else {
    if (tempm1) {
      //Find max columns
      tempm2=tempm1;
      while (tempm2) {
	if (tempm2->size>csize)
	  csize=tempm2->size;
	if (tempm2->expanded_size>esize)
	  esize=tempm2->expanded_size;
	if (tempm2->cluster>c)
	  c=tempm2->cluster;
	tempm2=tempm2->next;
      }
      csize=Bsr(csize)/4+1;
      esize=Bsr(esize)/4+1;
      c=Bsr(c)/4+1;

      "$$MA+A,\"Directory \",\"PopUpCd;Dir;\n\"$$ of %s\n",st;
      if (full)
	"__DATE__ __TIME__ %*ts %*ts %*ts\n",
	csize,"SIZE",esize,"UNCMPSIZE",c,"CLUSTER";
      else
	"DATE_ TIME_ %*ts\n",csize,"SIZE";
      while (tempm1) {
	tempm2=tempm1->next;
	if (full)
	  "%D %T %0*X %0*X %0*X ",tempm1->datetime,tempm1->datetime,
	    csize,tempm1->size,esize,tempm1->expanded_size,
	    c, tempm1->cluster;
	else
	  "%h1D %h1T %0*X ",tempm1->datetime,tempm1->datetime,
	    csize,tempm1->size;
	if (tempm1->attr & LT_ATTR_DIR)
	  PutDirLink(tempm1->name);
	else
	  PutFileLink(tempm1->name);

	NewLine;
	LTDirEntryDel(tempm1);
	tempm1=tempm2;
      }
    } else
      PutDefine("ST_NO_MATCHING_ENTRIES");
    Free(st);
  }
}
