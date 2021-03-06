U0 DrvDftSelect()
{
  dft_prt=Drv2Prt(dft_drv);
  Fs->cur_prt=INVALID_PTR;  //Null screws-up cache
  Fs->cur_dir=StrNew("/");
  ChgDsk(':');
}

BoolI64 CopySingleZ(U8 *f1,U8 *f2) //Just one file
{
  U8 *file_buf=NULL;
  U64 size,attr=0,c;
  LTDirEntry de;
 
  if (FileFind(f1,&de,FALSE,TRUE)) {
    file_buf=FileRead(f1,&size,&attr);
    attr=FileAttr(f2,attr);
    if (file_buf) {
      "Copying%s to %s\n",f1,f2;
      c=FileWrite(f2,file_buf,size,de.datetime,attr);
      Free(file_buf);
      return ToBool(c);
    } else PutDefineErr("ST_FILE_NOT_FOUND");
  } else PutDefineErr("ST_FILE_NOT_FOUND");
  return FALSE;
}

#define COPY_BUF_BLKS	0x80
BoolI64 CopySingle(U8 *f1,U8 *f2) //Just one file
{
  U8 *absf1=FileNameAbs(f1),
     *absf2=FileNameAbs(f2);
  LTDirEntry de1,de2;
  U8 *buf;
  U64 cnt,n,size,attr1=FileAttr(f1),attr2=FileAttr(f2);
  LTFile *in_file=NULL,*out_file=NULL;
  U64 xsum=LT_XSUM,i,j;

  if (!StrCmp(absf1,absf2)) { //onto self?
    Free(absf1);
    Free(absf2);
    return FALSE;
  }

  if (Bt(&sys_semas[SYS_SEMA_RISKY_COPY],0) && //probably identical file?
    FileFind(f1,&de1,FALSE,TRUE) &&
    FileFind(f2,&de2,FALSE,TRUE) &&
    de1.datetime==de2.datetime &&
    de1.size==de2.size &&
    de1.attr==de2.attr &&
    !StrCmp(de1.name,de2.name) &&
    de1.expanded_size==de2.expanded_size &&
    de1.xsum==de2.xsum)
    return TRUE;

  Free(absf1);
  Free(absf2);
  if (attr1!=attr2)
    return CopySingleZ(f1,f2);
  buf=MAlloc(COPY_BUF_BLKS<<BLK_SIZE_BITS);
  if (attr1 & LT_ATTR_CONTIGUOUS)
    in_file=FOpen(f1,"rc");
  else
    in_file=FOpen(f1,"r");
  if (in_file) {
    size=FSize(in_file);
    cnt=(size+BLK_SIZE-1)>>BLK_SIZE_BITS;
    if (attr2 & LT_ATTR_CONTIGUOUS)
      out_file=FOpen(f2,"wc",cnt);
    else
      out_file=FOpen(f2,"w",cnt);
    if (out_file) {
      "Copying %s to %s\n",f1,f2;
      j=size;
      while (cnt) {
	if (cnt>COPY_BUF_BLKS) {
	  n=COPY_BUF_BLKS;
	  i=n<<BLK_SIZE_BITS;
	} else {
	  n=cnt;
	  i=j;
	}
	FRBlks(in_file, buf,LTFB_NEXT_BLK,n);
	xsum^=ArcXSum(buf,i);
	FWBlks(out_file,buf,LTFB_NEXT_BLK,n);
	cnt-=n;
	j-=n<<BLK_SIZE_BITS;
      }
      if (in_file->de.xsum &&
	  in_file->de.xsum!=xsum)
	throw(EXCEPT_ARC_XSUM);
      out_file->de.xsum=xsum;
      out_file->flags|=LTF_USE_OLD_DATETIME;
      out_file->de.datetime=in_file->de.datetime;
      out_file->de.size=size;
      out_file->de.expanded_size=in_file->de.expanded_size;
      out_file->de.attr=FileAttr(f2,in_file->de.attr);
      FClose(out_file);
      FClose(in_file);
      Free(buf);
      return TRUE;
    } else PutDefineErr("ST_FILE_NOT_FOUND");
    FClose(in_file);
  } else PutDefineErr("ST_FILE_NOT_FOUND");
  Free(buf);
  return FALSE;
}

BoolI64 MkDir(U8 *filename)
{
  BoolI8 result=FALSE;
  U8 *name;
  if (FileFind(filename,NULL,TRUE))
    return FALSE;
  LTFileAccess *fa=LTFileAccessNew(filename);
  if (fa) {
    if (*fa->mask) {
      if (!FileNameChk(fa->mask))
	PutDefineErr("ST_INVALID_FILENAME");
      else {
	"Make Directory:";
	"" filename;
	NewLine;
	name=MStrUtil(fa->mask,
	SUF_REM_LEADING|SUF_REM_TRAILING|SUF_REM_CTRL_CHARS);
	switch (fa->p->type) {
	  case PT_LT:
	    result=LTFSMkDir(fa->p,Fs->cur_dir,name);
	    break;
	  case PT_FAT32:
	    result=FAT32MkDir(fa->p,Fs->cur_dir,name);
	    break;
	  case PT_ISO9660:
	    PutDefineErr("ST_NOT_WRITABLE");
	    break;
	  default:
	    PutDefineErr("ST_UNSUPPORTED_FILE_SYSTEM");
	}
	Free(name);
      }
    }
    LTFileAccessDel(fa);
  }
  return result;
}

U0 SysDel(U8 *wild,BoolI8 make_mask)	//this is dangerous
{
  LTFileAccess *fa=LTFileAccessNew(wild,make_mask);
  if (fa) {
    switch (fa->p->type) {
      case PT_LT:
	LTFSFilesDel(fa->p,Fs->cur_dir,fa->mask,0,TRUE,TRUE);
	break;
      case PT_FAT32:
	FAT32FilesDel(fa->p,Fs->cur_dir,fa->mask,0,TRUE,TRUE);
	break;
      case PT_ISO9660:
	PutDefineErr("ST_NOT_WRITABLE");
	break;
      default:
	PutDefineErr("ST_UNSUPPORTED_FILE_SYSTEM");
    }
    LTFileAccessDel(fa);
  }
}

U0 Fmt(I64 drv_let,BoolI8 quick=TRUE,BoolI8 confirm=TRUE,U64 type=PT_FAT32)
{//See $LK,"PT_FAT32","MN:PT_FAT32"$ for a list of supported file system types.

  LTPrt *p=Drv2Prt(drv_let);
  BoolI8 old_silent=IsSilent;
  if (confirm) {
    if (!AreYouSure)
      return;
  }
  if (!p)  {
    PutDefineErr("ST_INVALID_DRV");
    return;
  }
  switch (type) {
    case PT_LT:
      Silent(ON);
      Drv(drv_let);
      Silent(old_silent);
      LTFSFmt(drv_let,quick);
      DskCacheInvalidate(Drv2Prt(drv_let));
      Drv(drv_let);
      break;
    case PT_FAT32:
      Silent(ON);
      Drv(drv_let);
      Silent(old_silent);
      FAT32Fmt(drv_let,quick);
      DskCacheInvalidate(Drv2Prt(drv_let));
      Drv(drv_let);
      break;
    case PT_ISO9660:
      PutDefineErr("ST_NOT_WRITABLE");
      break;
    default:
      PutDefineErr("ST_UNSUPPORTED_FILE_SYSTEM");
  }
}


U0 CDImageRead(U8 cd_drv_let,U8 *out_name)
{
  LTBlkDev *bdev=Drv2BlkDev(cd_drv_let);
  LTPrt *p=Drv2Prt(cd_drv_let);
  U8 *buf=MAlloc(COPY_BUF_BLKS<<BLK_SIZE_BITS);
  U8 *out_name2=DftExt(out_name,"ISO");
  LTFile *f=FOpen(out_name2,"w");
  U64 n,spc=bdev->blk_size>>BLK_SIZE_BITS,blk=0,cnt,retry;
  BlkDevInit(bdev);
  if (bdev->type!=LTBDT_ATAPI)
    throw(EXCEPT_BLKDEV,3);
  if (!out_name)
    out_name=sys_dft_iso_name;
  cnt=CeilU64(p->size,spc);
  while (cnt) {
    if (cnt>COPY_BUF_BLKS)
      n=COPY_BUF_BLKS;
    else
      n=cnt;
    if (n>bdev->max_reads)
      n=bdev->max_reads;
    if (!Bt(&sys_semas[SYS_SEMA_NO_IO_SND],0))
      Snd(bdev->read_freq);

    retry=4;
    while (--retry)
      if (ATAPIReadBlks2(bdev,buf,blk/spc,n/spc,TRUE,
	//n is 0x800 if max_reads.  Up to 8 additional seconds
	BootTime+7*LTDATE_FREQ+n/spc*LTDATE_FREQ/0x100))
	break;

    if (!retry)
      ATAPIReadBlks2(bdev,buf,blk/spc,n/spc,TRUE,0);

    if (!Bt(&sys_semas[SYS_SEMA_NO_IO_SND],0))
      Snd(0);
    FWBlks(f,buf,blk,n);
    cnt-=n;
    blk+=n;
  }
  FClose(f);
  Free(buf);
  Free(out_name2);
}

class LTDualBuf
{
  U8 *buf0,*buf1;
  U64 in_buf,out_buf,cnt;
  U8 *filename;
  LTBlkDev *CD_bdev;
};


U0 CDImageWriteTask(LTDualBuf *d)
{
  U8 *buf;
  U64 n,blk=0,cnt=d->cnt;
  LTFile *f;
  if (!IsSysDeterministic)
    Preempt(ON);
  TaskLocalInit; //needed for disk access
  if (FileAttr(d->filename)&LT_ATTR_CONTIGUOUS)
    f=FOpen(d->filename,"rc");
  else
    f=FOpen(d->filename,"r");
  while (cnt) {
    if (cnt>COPY_BUF_BLKS)
      n=COPY_BUF_BLKS;
    else
      n=cnt;
    if (n>d->CD_bdev->max_writes)
      n=d->CD_bdev->max_writes;
    buf=d->in_buf&1 ? d->buf1:d->buf0;
    while (d->in_buf>d->out_buf+1)
      Yield;
    FRBlks(f,buf,blk,n);
    if (!Bt(&sys_semas[SYS_SEMA_NO_IO_SND],0))
      Snd(d->CD_bdev->write_freq);
    d->in_buf++;
    cnt-=n;
    blk+=n;
  }
  FClose(f);
}

U0 CDImageWrite(U8 cd_drv_let,U8 *in_name=NULL,I64 media_type=MT_CD)
{
  LTDualBuf *d=CAlloc(sizeof(LTDualBuf));
  U8 *buf;
  U8 *in_name2,*in_name3;
  U64 i,n,spc,blk=0,cnt;
  LTPrt *p=Drv2Prt(cd_drv_let);
  LTBlkDev *bdev=p->bdev,*bdev2;
  TaskStruct *task;
  BoolI8 old_preempt;
  LTFile *f;

  if (!in_name)
    in_name=sys_dft_iso_name;
  in_name3=DftExt(in_name,"ISO");
  in_name2=FileNameAbs(in_name3);
  f=FOpen(in_name2,"r");
  if (!f) {
    Free(d);
    return;
  }

  cnt=(FSize(f)+BLK_SIZE-1)>>BLK_SIZE_BITS;
  FClose(f);
  if (bdev->type!=LTBDT_ATAPI)
    throw(EXCEPT_BLKDEV,4);
  bdev2=Drv2BlkDev(*in_name2);
  while (bdev2->lock_fwding)
    bdev2=bdev2->lock_fwding;
  if ((bdev2->type==LTBDT_ATA || bdev2->type==LTBDT_ATAPI) &&
       bdev2->base0==bdev->base0) {
    "Can't Burn CD on same ATA ctrller as File.\n\n";
    throw(EXCEPT_BLKDEV,5);
  }

  bdev->flags|=LTBDF_READ_ONLY_OVERRIDE;
  BlkDevInit(bdev);
  spc=bdev->blk_size>>BLK_SIZE_BITS;
  if (p->size<cnt)
    p->size=cnt;

  d->filename=in_name2;
  d->CD_bdev=bdev;
  d->buf0=MAlloc(COPY_BUF_BLKS<<BLK_SIZE_BITS);
  d->buf1=MAlloc(COPY_BUF_BLKS<<BLK_SIZE_BITS);
  d->cnt=cnt;

  old_preempt=Preempt(OFF);
  task=Spawn(&CDImageWriteTask,d,"Write CD");

  while (d->in_buf<=d->out_buf)
    Yield;

  if (!Bt(&sys_semas[SYS_SEMA_NO_IO_SND],0))
    Snd(bdev->write_freq);
  BlkDevLock(bdev);
  ATAPIWaitReady(bdev,0);

  progress1=0; progress1_max=cnt;
  StrCpy(progress1_desc,"Writing");
  while (cnt) {
    if (cnt>COPY_BUF_BLKS)
      n=COPY_BUF_BLKS;
    else
      n=cnt;
    if (n>bdev->max_writes)
      n=bdev->max_writes;
    buf=d->out_buf&1 ? d->buf1:d->buf0;
    while (d->in_buf<=d->out_buf)
      Yield;
    ATAPIWriteBlks(bdev,buf,blk/spc,(n+spc-1)/spc);
    d->out_buf++;
    cnt-=n;
    blk+=n;
    progress1+=n;
  }
  ATAPISync(bdev);

  progress1=0; progress1_max=2;
  StrCpy(progress1_desc,"Closing");
  for (i=0;i<2;i++) {
    ATAPIClose(bdev,0x100,i); //Close tracks
    progress1++;
  }

  ATAPISync(bdev);

  ATAPIClose(bdev,0x200); //close disk
  ATAPISync(bdev);
  if (media_type==MT_DVD) {
    ATAPIClose(bdev,0x300);
    ATAPISync(bdev);
  }

  *progress1_desc=0;
  progress1=progress1_max=0;

  if (!Bt(&sys_semas[SYS_SEMA_NO_IO_SND],0))
    Snd(0);
  bdev->flags&=~LTBDF_READ_ONLY_OVERRIDE;
  BlkDevUnlock(bdev);
  Free(d->buf0);
  Free(d->buf1);
  Free(in_name2);
  Free(in_name3);
  Preempt(old_preempt);
  Free(d);
}

