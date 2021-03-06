//ISO1 is ISO9660
//ISO2 is ISO13490
//ISO3 is ISO13346

BoolI64 ISOPrtInit(LTPrt *p,U64 blk)
{
  LTBlkDev *bdev=p->bdev;
  U64 spc=bdev->blk_size>>BLK_SIZE_BITS,i=blk/spc;
  ISO1PriDesc *iso=MAlloc(bdev->blk_size);
  ISO1DirEntry *de;
  BoolI8 unlock,result=FALSE;
  U8 buf[8];

  try {
    unlock=PrtLock(p);
    p->type=PT_ISO9660; //default
    p->spc=spc;
    p->offset=p->data=p->root_cluster=p->size=0;
    while (TRUE) {
      p->size=MaxI64(p->size,(i+1)*spc);
      RBlks(p,iso,i*spc,spc);
      buf[0](U32)=iso->id[0](U32);
      buf[4](U16)=iso->id[4](U8);
      switch (ListEntryMatch(buf,"CD001\0CDW02\0BEA01\0BOOT2\0NSR02\0NSR03\0TEA01\0",LEF_EXACT)) {
	case 0:
	  p->type=PT_ISO9660;
	  switch (iso->type) {
	    case ISO1T_SUPPLEMENTARY_DESC:
	      p->data=0;
	      de=&iso->root_dir_record;
	      p->root_cluster=de->loc.little;
	      p->size=iso->vol_space_size.little*bdev->blk_size>>BLK_SIZE_BITS;
	      result=TRUE;
	      goto ipi_done;
	    case ISO1T_TERMINATOR:
	      throw(EXCEPT_PRT,10);
	  }
	  break;
	case 2...6:
	  p->type=PT_ISO13346;
	  break;
	case 1: //PT_ISO13490 (Not supported)
	default: //Its normal for ISO13346 to read NULL blk as terminator
	  switch (p->type) {
	    case PT_ISO13346:
	      i++; //Terminator is blk after last valid.  Advance to next blk
	      if (ISO3PrtInit(p,i*spc)) {
		result=TRUE;
		goto ipi_done;
	      }
	      break;
	  }
	  PutDefineErr("ST_UNSUPPORTED_FILE_SYSTEM");
	  throw(EXCEPT_PRT,11);
      }
      i++;
    }
ipi_done:
    Free(iso);
    if (unlock)
      PrtUnlock(p);
  } catch {
    p->type=PT_ISO9660;
    p->spc=spc;
    p->offset=p->data=p->root_cluster=0;
    Free(iso);
    if (unlock)
      PrtUnlock(p);
  }
  return result;
}

U0 LTDate2ISO1(ISO1Date *dst,LTDate ltdate)
{
  LTDateStruct d;
  LTDate2LTDateStruct(&d,ltdate);
  dst->year=d.year-ISO1_BASE_YEAR;
  dst->mon=d.mon;
  dst->day=d.day_of_mon;
  dst->hour=d.hour;
  dst->min=d.min;
  dst->sec=d.sec;
  dst->sec100=d.sec100;
}

LTDate ISO1LTDateStruct2LTDate(ISO1Date *dt)
{
  LTDateStruct ltdt;
  MemSet(&ltdt,0,sizeof(LTDateStruct));
  ltdt.day_of_mon=dt->day;
  ltdt.mon=dt->mon;
  ltdt.year=dt->year+ISO1_BASE_YEAR;
  ltdt.sec100=dt->sec100;
  ltdt.sec=dt->sec;
  ltdt.min=dt->min;
  ltdt.hour=dt->hour;
  return LTDateStruct2LTDate(&ltdt);
}

BoolI64 ISO1FromName(U8 *dst,U8 *src)
{
  I64 i,j,n;
  MemSet(dst,0,LT_MAX_FILENAME_LEN+1);
  n=*src++;
  if (n==1 && !*src) {
    *dst='.';
  } else if (n==1 && *src==1) {
    *dst='.';
    dst[1]='.';
  } else {
    n>>=1;
    j=0;
    for (i=0;i<n;i++) {
      src++;
      if (*src==';')
	break;
      if (Bt(filename_chars_bitmap,*src)) {
	if (j>=LT_MAX_FILENAME_LEN)
	  return FALSE;
	dst[j++]=*src++;
      } else
	return FALSE;
    }
  }
  return FileNameChk(dst);
}

BoolI64 ISO1LTDirFill(LTPrt *p,LTDirEntry *tempm,ISO1DirEntry *de)
{
  ArcCompressStruct *buf;
  BoolI8 result;
  MemSet(tempm,0,sizeof(LTDirEntry));
  result=ISO1FromName(tempm->name,&de->name_len);
  tempm->cluster=de->loc.little;
  tempm->size=de->size.little;
  tempm->attr=FileAttr(tempm->name);
  if (de->flags & ISO1_ATTR_DIR)
    tempm->attr|=LT_ATTR_DIR;
  tempm->datetime=ISO1LTDateStruct2LTDate(&de->date);
  if (tempm->attr & LT_ATTR_COMPRESSED) {
    buf=MAlloc(BLK_SIZE);
    RClustersBlks(p,buf,tempm->cluster,1);
    tempm->expanded_size=buf->expanded_size;
    Free(buf);
  } else
    tempm->expanded_size=tempm->size;
  return result;
}

BoolI64 ISO1FileFind(LTPrt *p,U64 cur_dir_cluster,U8 *name,LTDirEntry *result,
		     BoolI8 dir_only,BoolI8 file_only)
{
  ISO1DirEntry *isoptr,*buf;
  U8 dname[LT_MAX_FILENAME_LEN+1];
  BoolI8 res=FALSE,unlock;
  I64 i;
 
  PrtChk(p);
  if (p->type!=PT_ISO9660)
    PutDefineErr("ST_NOT_ISO9660");
  else {
    try {
      unlock=PrtLock(p);
      isoptr=MAlloc(p->spc<<BLK_SIZE_BITS);
      RClusters(Fs->cur_prt,isoptr,cur_dir_cluster,1);
      if (isoptr->name_len==1 && !isoptr->name) { //curdir
	i=(isoptr->size.little+p->spc<<BLK_SIZE_BITS-1)/p->spc<<BLK_SIZE_BITS;
	buf=MAlloc(p->spc<<BLK_SIZE_BITS*i);
	RClusters(Fs->cur_prt,buf,cur_dir_cluster,i);
	Free(isoptr);
      } else {
	buf=isoptr;
	i=1;
      }
      i*=p->spc<<BLK_SIZE_BITS;
      isoptr=buf;
      while (i>0) {
	if (!isoptr->len) {
	  isoptr(U8 *)++;
	  i--;
	} else {
	  ISO1FromName(dname,&isoptr->name_len);
	  if (*dname) {
	    if (!StrCmp(name,dname)) {
	      res=ISO1LTDirFill(p,result,isoptr);
	      if (res &&
	      !(dir_only && !(result->attr & LT_ATTR_DIR)) &&
	      !(file_only && result->attr & LT_ATTR_DIR) )
		goto exit_this;
	      else
		res=FALSE;
	    }
	  }
	  i-=isoptr->len;
	  isoptr(U8 *)+=isoptr->len;
	}
      }
exit_this:
      Free(buf);
      if (unlock)
	PrtUnlock(p);
    } catch
      if (unlock)
	PrtUnlock(p);
  }
  return res;
}

U8 *ISO1FileRead(LTPrt *p,U8 *cur_dir,U8 *filename,U64 *size,
  U64 *attr,BoolI8 raw,ArcCompressStruct **ac2)
{
  U8 *buf=NULL;
  LTDirEntry de;
  U64 c,blk_cnt;
  ArcCompressStruct *ac;
  U64 cur_dir_cluster;
 
  PrtChk(p);
  if (size)
    *size=0;
  if (ac2)
    *ac2=NULL;
  if (attr)
    *attr=0;
  if (p->type!=PT_ISO9660)
    PutDefineErr("ST_NOT_ISO9660");
  else {
    try {
      PrtLock(p);
      cur_dir_cluster=Name2DirCluster(p,cur_dir);
      if (ISO1FileFind(p,cur_dir_cluster,filename,&de,FALSE,TRUE)) {
	if (attr)
	  *attr=de.attr;
	blk_cnt=(de.size+BLK_SIZE-1)>>BLK_SIZE_BITS;
	buf=MAlloc(blk_cnt<<BLK_SIZE_BITS+1);
	c=de.cluster;
	c=RClustersBlks(p,buf,c,blk_cnt);
	if (FileAttr(de.name)&LT_ATTR_COMPRESSED && !raw) {
	  ac=buf;
	  if (attr)
	    *attr=FileAttr(de.name,*attr);
	  if (size)
	    *size=ac->expanded_size;
	  if (ac2) {
	    *ac2=ac;
	    buf=ExpandBuf(ac);
	  } else {
	    buf=ExpandBuf(ac);
	    Free(ac);
	  }
	} else {
	  buf[de.size]=0; //Terminate
	  if (size)
	    *size=de.size;
	}
	if (attr)
	  *attr=FileAttr(de.name,*attr);
      }
      PrtUnlock(p);
    } catch
      PrtUnlock(p);
  }
  return buf;
}

BoolI64 ISO1Cd(U8 *name,U64 cur_dir_cluster)
{
  LTDirEntry de;
  if (Fs->cur_prt->type!=PT_ISO9660)
    PutDefineErr("ST_NOT_ISO9660");
  else {
    if (ISO1FileFind(Fs->cur_prt,cur_dir_cluster,name,&de,TRUE,FALSE))
      return TRUE;
    else {
      "%s ",name;
      PutDefineErr("ST_FILE_NOT_FOUND");
    }
  }
  return FALSE;
}

LTDirEntry *ISO1FilesFind(U8 *files_find_mask,U64 fuf_flags,LTDirEntry *parent=NULL)
{
  LTPrt *p=Fs->cur_prt;
  ISO1DirEntry *buf,*buf2,*isoptr;
  U64 cur_dir_cluster=Name2DirCluster(p,Fs->cur_dir);
  LTDirEntry *result=NULL,*tempm;
  I64 i;

  isoptr=MAlloc(p->spc<<BLK_SIZE_BITS);
  RClusters(Fs->cur_prt,isoptr,cur_dir_cluster,1);
  if (isoptr->name_len==1 && !isoptr->name) { //curdir
    i=(isoptr->size.little+p->spc<<BLK_SIZE_BITS-1)/p->spc<<BLK_SIZE_BITS;
    buf=MAlloc(p->spc<<BLK_SIZE_BITS*i);
    RClusters(Fs->cur_prt,buf,cur_dir_cluster,i);
    Free(isoptr);
  } else {
    buf=isoptr;
    i=1;
  }
  buf2=buf;
  i*=p->spc<<BLK_SIZE_BITS;
  while (i>0) {
    if (!buf->len) {
      buf(U8 *)++;
      i--;
    } else {
      tempm=MAlloc(sizeof(LTDirEntry));
      if (ISO1LTDirFill(Fs->cur_prt,tempm,buf)) {
	tempm->parent=parent;
	if (Bt(&fuf_flags,FUf_RECURSE) && tempm->attr&LT_ATTR_DIR && *tempm->name!='.') {
	  tempm->next=result;
	  result=tempm;
	  tempm->full_name=DirNameAbs(tempm->name);
	  if (Cd(tempm->name)) {
	    tempm->sub=ISO1FilesFind(files_find_mask,fuf_flags,tempm);
	    Cd("..");
	  }
	} else if ((tempm->attr & LT_ATTR_DIR || !Bt(&fuf_flags,FUf_JUST_DIRS)) &&
	!(Bt(&fuf_flags,FUf_RECURSE) && *tempm->name=='.' && tempm->attr&LT_ATTR_DIR) &&
	   FilesFindMatch(files_find_mask,tempm->name,fuf_flags)) {
	  tempm->next=result;
	  result=tempm;
	  tempm->full_name=FileNameAbs(tempm->name);
	} else
	  LTDirEntryDel(tempm);
      } else
	LTDirEntryDel(tempm);
      i-=buf->len;
      buf(U8 *)+=buf->len;
    }
  }
  Free(buf2);
  return result;
}

