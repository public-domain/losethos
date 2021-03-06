BoolI64 BlkDevLock(LTBlkDev *bdev)
{
  BlkDevChk(bdev);
  while (bdev->lock_fwding)
    bdev=bdev->lock_fwding;
  if (!Bt(&bdev->locked_flags,LTBDlf_LOCKED) || bdev->owning_task!=Fs) {
    while (LBts(&bdev->locked_flags,LTBDlf_LOCKED))
      Yield;
    bdev->owning_task=Fs;
    return TRUE;
  } else
    return FALSE;
}

BoolI64 PrtLock(LTPrt *p)
{
  PrtChk(p);
  BlkDevLock(p->bdev);
  if (!Bt(&p->locked_flags,LTPlf_LOCKED) || p->owning_task!=Fs) {
    while (LBts(&p->locked_flags,LTPlf_LOCKED))
      Yield;
    p->owning_task=Fs;
    return TRUE;
  } else
    return FALSE;
}

BoolI64 BlkDevUnlock(LTBlkDev *bdev,BoolI8 rst=FALSE)
{
  BlkDevChk(bdev);
  while (bdev->lock_fwding)
    bdev=bdev->lock_fwding;
  if (Bt(&bdev->locked_flags,LTBDlf_LOCKED) && bdev->owning_task==Fs) {
    if (rst) {
      if (!Bt(&sys_semas[SYS_SEMA_NO_IO_SND],0))
	Snd(0);
      bdev->flags&=~(LTBDF_INITIALIZED|LTBDF_INIT_IN_PROGRESS);
    }
    bdev->owning_task=0;
    LBtr(&bdev->locked_flags,LTBDlf_LOCKED);
    Yield; //Prevent deadlock
    return TRUE;
  } else
    return FALSE;
}

BoolI64 PrtUnlock(LTPrt *p,BoolI8 rst=FALSE)
{
  PrtChk(p);
  if (Bt(&p->locked_flags,LTPlf_LOCKED) && p->owning_task==Fs) {
    BlkDevUnlock(p->bdev,rst);
    p->owning_task=0;
    LBtr(&p->locked_flags,LTPlf_LOCKED);
    Yield; //Prevent deadlock
    return TRUE;
  } else
    return FALSE;
}

BoolI64 BlkDevInit(LTBlkDev *bdev)
{
  LTPrt *p=Drv2Prt(bdev->drv_let_base),*p2;
  BoolI8 old_preempt=Preempt(OFF),result=FALSE;
  if (!(bdev->flags & LTBDF_INITIALIZED)) {
    bdev->flags|=LTBDF_INITIALIZED |
	       LTBDF_INIT_IN_PROGRESS;
    switch(bdev->type) {
      case LTBDT_RAM:
	bdev->RAM_dsk=AMAlloc((bdev->max_blk+1)<<BLK_SIZE_BITS);
	bdev->max_blk=MSize(bdev->RAM_dsk)>>BLK_SIZE_BITS-1;
	p->type=PT_LT;
	if (sys_boot_code.u8[0]!=BOOT_CODE_RAM ||
	    !LTFSDrvValidate(bdev->drv_let_base))
	  LTFSFmt(bdev->drv_let_base);
	else {
	  p->size=bdev->max_blk+1;
	  LTFSPrtInit(p);
	}
	break;
      case LTBDT_ATA:
	bdev->max_reads=128;
	bdev->max_writes=1;
	bdev->read_freq=2000;
	bdev->write_freq=500;
	ATAInit(bdev);
	break;
      case LTBDT_ATAPI:
	//0xFFFF*4 is too big for my taste
	bdev->max_reads=0x800*4;
	//max of maybe a quarter of disk cache
	if (bdev->max_reads>sys_ltdc_size/BLK_SIZE/4)
	  bdev->max_reads=sys_ltdc_size/BLK_SIZE/4 & ~3;
	if (bdev->max_reads<128)
	  bdev->max_reads=128;
	bdev->max_writes=0xFFFF*4;
	bdev->read_freq=1000;
	bdev->write_freq=250;
	ATAInit(bdev);
	p->size=bdev->max_blk+1;
	break;
      case LTBDT_NON_LT_FILE:
	bdev->max_reads=128;
	bdev->max_writes=128;
	bdev->read_freq=2000;
	bdev->write_freq=500;
	if (bdev->flags & LTBDF_FMT)
	  LTFSFmt(bdev->drv_let_base,TRUE);
	else {
	  p2=Drv2Prt(bdev->non_lt_drv);
	  bdev->min_blk=p->offset;
	  bdev->max_blk=p2->offset+p2->size-1; //temp
	  p->size=p2->size-(p->offset-p2->offset); //temp
	  LTFSPrtInit(p);
	  bdev->max_blk=p->offset+p->size-1;
	}
	break;
    }
    if (bdev->flags & LTBDF_READ_CACHE)
      DskCacheInvalidate(p);
    bdev->flags&=~LTBDF_INIT_IN_PROGRESS;
    result=TRUE;
  }
  Preempt(old_preempt);
  return result;
}

U0 BlkDevsRelease()
{
  I64 i;
  LTBlkDev *bdev;
  for (i=0;i<NUM_LTBLKDEVS;i++) {
    bdev=&local_blkdevs[i];
    if (Bt(&bdev->locked_flags,LTBDlf_LOCKED) && bdev->owning_task==Fs)
      BlkDevUnlock(bdev,TRUE);
  }
}

U0 PrtsRelease()
{
  I64 i;
  LTPrt *p;
  for (i=0;i<NUM_LTPRTS;i++) {
    p=&local_prts[i];
    if (Bt(&p->locked_flags,LTPlf_LOCKED) && p->owning_task==Fs)
      PrtUnlock(p,TRUE);
  }
}

U0 AllocMAPBuf(LTPrt *p,I64 num_blks)
{
  PrtChk(p);
  Free(p->cur_map_blk);
  Free(p->buffered_map_dirty_bits);
  p->num_buffered_map_blks=num_blks;
  p->cur_map_blk=AMAlloc(num_blks<<BLK_SIZE_BITS);
  p->buffered_map_dirty_bits=ACAlloc((num_blks+7)>>3);
  p->cur_map_blk_num=0;
  RBlks(p,p->cur_map_blk,p->map1,p->num_buffered_map_blks);
}


U0 CleanMAPBuf(LTPrt *p,I64 MAP_select=-1)
{
  I64 i;
  if ((MAP_select==-1 || !MAP_select) && p->map1!=p->map2)
    for (i=0;i<p->num_buffered_map_blks;i++)
      if (Bt(p->buffered_map_dirty_bits,i))
	WBlks(p,p->cur_map_blk(U64)+i<<BLK_SIZE_BITS,p->map1+i+p->cur_map_blk_num,1,TRUE);
  if (MAP_select==-1 || MAP_select==1 || p->map1==p->map2)
    for (i=0;i<p->num_buffered_map_blks;i++)
      if (LBtr(p->buffered_map_dirty_bits,i))
	WBlks(p,p->cur_map_blk(U64)+i<<BLK_SIZE_BITS,p->map2+i+p->cur_map_blk_num,1,TRUE);
}

U0 SetLTBuf(LTPrt *p,I64 c,I64 map_select=-1)
{
  I64 MAP_blk_num;
  if (p->type==PT_FAT32)
    MAP_blk_num=c/(BLK_SIZE>>2);
  else
    MAP_blk_num=c>>(BLK_SIZE_BITS+3);
  if (MAP_blk_num!=p->cur_map_blk_num) {
    CleanMAPBuf(p,map_select);
    p->cur_map_blk_num=MAP_blk_num;
    if (map_select==-1 || !map_select)
      RBlks(p,p->cur_map_blk,p->map1+p->cur_map_blk_num,1);
    else
      RBlks(p,p->cur_map_blk,p->map2+p->cur_map_blk_num,1);
  }
}

#define ZERO_BUF_SIZE	2048
U0 WZeroBlks(LTPrt *p,I64 blk,I64 cnt)
{
  I64 n;
  U8 *z=CAlloc(ZERO_BUF_SIZE<<BLK_SIZE_BITS);
  BoolI8 show_progress;
  if (cnt>ZERO_BUF_SIZE && p->bdev->type!=LTBDT_RAM) {
    progress1=0; progress1_max=cnt;
    StrCpy(progress1_desc,"Zeroing");
    show_progress=TRUE;
  } else
    show_progress=FALSE;
  while (cnt) {
    n=cnt;
    if (n>ZERO_BUF_SIZE)
      n=ZERO_BUF_SIZE;
    WBlks(p,z,blk,n);
    blk+=n;
    cnt-=n;
    if (show_progress)
      progress1+=n;
    Yield;  //Prevent locking
  }
  Free(z);
  if (show_progress) {
    *progress1_desc=0;
    progress1=progress1_max=0;
  }
}

//Very dangerous
BoolI64 SetDrvType(U8 drv_let,U64 type=PT_LT)
{
  I64 i,j,ext_base,num_prt;
  I64 offset,cur_type;
  MasterBootStruct mbr;
  LTBlkDev *bdev=Drv2BlkDev(drv_let);
  num_prt=bdev->drv_let_base-'A';
  switch (bdev->type) {
    case LTBDT_ATA:
      offset=0;
      ext_base=INVALID_CLUSTER;
      while (TRUE) {
	ATAReadBlks(bdev,&mbr,offset,1);
	j=-1;
	for (i=0;i<4;i++) {
	  cur_type=mbr.p[i].type;
	  if (cur_type) {
	    if (cur_type==5 || cur_type==15)
	      j=i;
	    else {
	      if (num_prt+'A'==ToUpper(drv_let)) {
		switch (type) {
		  case PT_LT:
		    mbr.p[i].type=MBR_PT_LT;
		    break;
		  case PT_FAT32:
		    mbr.p[i].type=MBR_PT_FAT32a;
		    break;
		  default:
		    Dbg;
		}
		mbr.p[i].active=0x80;
		ATAWriteBlks(bdev,&mbr,offset,1);
		return TRUE;
	      }
	      num_prt++;
	    }
	  }
	}
	if (j<0)
	  break;
	if (!mbr.p[j].offset)
	  break;
	if (ext_base==INVALID_CLUSTER) {
	  offset=mbr.p[j].offset;
	  ext_base=offset;
	} else
	  offset=mbr.p[j].offset+ext_base;
      }
      break;
  }
}


BoolI64 RBlks(LTPrt *p,U8 *buf, I64 blk, I64 cnt)
{
  BoolI8 result=TRUE;
  LTBlkDev *bdev=p->bdev;
  BoolI8 unlock;
  PrtChk(p);
  if (!cnt) return result;
  try {
    unlock=PrtLock(p);
    BlkDevInit(bdev);
    if (p->offset && blk<p->offset || blk+cnt>p->offset+p->size)
      throw(EXCEPT_PRT,3);
    blk+=bdev->offset;
 
    if (bdev->flags & LTBDF_READ_CACHE)
      RCache(p,&buf,&blk,&cnt);
    if (cnt) {
      if (!Bt(&sys_semas[SYS_SEMA_NO_IO_SND],0))
	Snd(bdev->read_freq);
      result=(*p->rblks)(p,buf,blk,cnt);
      bdev->last_time=GetTimeStamp;
      bdev->most_recent_blk=blk+cnt-1;
      if (!Bt(&sys_semas[SYS_SEMA_NO_IO_SND],0))
	Snd(0);
      if (bdev->flags & LTBDF_READ_CACHE)
	LTDCAdd(p,buf,blk,cnt,FALSE,FALSE);
    }
    if (unlock)
      PrtUnlock(p);
  } catch
    if (unlock)
      PrtUnlock(p);
  return result;
}

BoolI64 WBlks(LTPrt *p,U8 *buf, I64 blk, I64 cnt,BoolI8 write_cache=FALSE)
{
  BoolI8 result=TRUE,unlock;
  LTBlkDev *bdev=p->bdev;
  PrtChk(p);
  if (!cnt) return result;
  try {
    unlock=PrtLock(p);
    BlkDevInit(bdev);
    if (bdev->flags&LTBDF_READ_ONLY &&
    !(bdev->flags & LTBDF_READ_ONLY_OVERRIDE))
      throw(EXCEPT_BLKDEV,0);
    if (p->offset && blk<p->offset || blk+cnt>p->offset+p->size)
      throw(EXCEPT_PRT,4);
    blk+=bdev->offset;
    if (cnt) {
      if (p->flags & 1<<LTPf_CACHE_ALL_WRITES)
	write_cache=TRUE;
      if (!(p->flags & 1<<LTPf_CACHE_WRITES))
	write_cache=FALSE;
      if (!write_cache) {
	if (!Bt(&sys_semas[SYS_SEMA_NO_IO_SND],0))
	  Snd(bdev->write_freq);
	result=(*p->wblks)(p,buf,blk,cnt);
	if (!Bt(&sys_semas[SYS_SEMA_NO_IO_SND],0))
	  Snd(0);
      }
      bdev->last_time=GetTimeStamp;
      bdev->most_recent_blk=blk+cnt-1;
      if (bdev->flags & LTBDF_READ_CACHE)
	LTDCAdd(p,buf,blk,cnt,TRUE,write_cache);
    }
    if (unlock)
      PrtUnlock(p);
  } catch
    if (unlock)
      PrtUnlock(p);
  return result;
}


U0 DskCacheInvalidate(LTPrt *p)
{
  BoolI8 unlock;
  LTBlkDev *bdev=p->bdev;
  PrtChk(p);
  try {
    unlock=PrtLock(p);
    BlkDevInit(bdev);
    if (bdev->flags & LTBDF_READ_CACHE)
      LTDCInvalidate(p);
    if (p->type==PT_ISO9660) {
      if (!(bdev->flags & LTBDF_READ_ONLY_OVERRIDE))
	ISOPrtInit(p,(32767/bdev->blk_size+1)*
	  bdev->blk_size>>BLK_SIZE_BITS);
    }
    if (unlock)
      PrtUnlock(p);
  } catch
    if (unlock)
      PrtUnlock(p);
}

U0 ChgDsk(U8 drv_let=0)
{ //This is needed for removable media
  LTPrt *p=Drv2Prt(drv_let);
  LTBlkDev *bdev=p->bdev;
  BoolI8 old_preempt=Preempt(OFF);
  if (!BlkDevInit(bdev) && bdev->flags & LTBDF_REMOVABLE)
    DskCacheInvalidate(p);
  Drv(drv_let);
  LTFSFreeFreeList(p);
  Preempt(old_preempt);
}


U0 PrtRep()
{ //Partition report
  LTPrt *p;
  LTBlkDev *bdev;
  U16 type_u16;
  I64 i;
  "\nDefined Partitions:\n\n";
  p=local_prts;
  for (i=0;i<NUM_LTPRTS;i++,p++) {
    bdev=p->bdev;
    if (type_u16=p->type.u16[0]) {
      "$$FG,PURPLE$$";
      if (p==dft_prt)
	'*';
      else
	'' CH_SPACE;
      if (Bt(&p->type,PTf_DISABLE))
	"Disabled";
      "%C %Z %Z %04X %04X %02X$$FG$$\n%016X-%016X\n",
	Prt2Drv(p),type_u16,"ST_PRT_TYPES",
	bdev->type,"ST_BLKDEV_TYPES",bdev->base0,bdev->base1,bdev->unit,
	p->offset,p->offset+p->size-1;
    }
  }
}

U64 ClusterNumNext(LTPrt *p,I64 c,I64 cnt=1)
{
  BoolI8 unlock;
  PrtChk(p);
  if (!cnt) return c;
  try {
    unlock=PrtLock(p);
    switch (p->type) {
      case PT_ISO9660:
      case PT_LT:
	c+=cnt;
	break;
      case PT_FAT32:
	while (cnt-- && c && c<0x0FFFFFF8) {
	  SetLTBuf(p,c);
	  c=p->cur_map_blk[c & (BLK_SIZE/4-1)];
	}
	if (!c || c>=0xFFFFFF8)
	  c=INVALID_CLUSTER;
	break;
      default:
	Dbg;
    }
    if (unlock)
      PrtUnlock(p);
  } catch
    if (unlock)
      PrtUnlock(p);
  return c;
}


I64 Cluster2Blk(LTPrt *p,I64 c)
{
  PrtChk(p);
  return p->data+c*p->spc;
}

I64 RClustersBlks(LTPrt *p,U8 *buf,I64 c,I64 blks)
{ //This accepts blk count, so padding on last cluster is not read.
  I64 i;
  BoolI8 unlock;
  PrtChk(p);
  if (!blks) return c;
  try {
    unlock=PrtLock(p);
    switch (p->type) {
      case PT_ISO9660:
      case PT_LT:
	RBlks(p,buf,p->data+c*p->spc,blks);
	c+=(blks+p->spc-1)/p->spc;
	break;
      case PT_FAT32:
	while (blks && c && c!=INVALID_CLUSTER) {
	  i=blks;
	  if (i>p->spc)
	    i=p->spc;
	  RBlks(p,buf,p->data+c*p->spc,i);
	  buf+=i<<BLK_SIZE_BITS;
	  c=ClusterNumNext(p,c,1);
	  blks-=i;
	}
	if (blks)
	  Dbg;
	break;
      default:
	Dbg;
    }
    if (unlock)
      PrtUnlock(p);
  } catch
    if (unlock)
      PrtUnlock(p);
  return c;
}

I64 RClusters(LTPrt *p,U8 *buf,I64 c,I64 cnt)
{
  return RClustersBlks(p,buf,c,cnt*p->spc);
}

BoolI64 LTFileNameTo(U8 *dst,U8 *src)
{
  MemSet(dst,0,LT_MAX_FILENAME_LEN+1);
  if (!FileNameChk(src))
    return FALSE;
  StrCpy(dst,src);
  return TRUE;
}

I64 Name2DirCluster(LTPrt *p,U8 *dirname)
{
  BoolI8 cont=TRUE;
  I64 cur_dir_cluster;
  LTDirEntry de;
  U8 *buf=StrNew(dirname),*buf2=StrNew(dirname);
  BoolI8 unlock;
  PrtChk(p);
  try {
    unlock=PrtLock(p);
    cur_dir_cluster=p->root_cluster;
    while (*buf && cont) {
      StrFirstRem(buf,"/",buf2);
      if (*buf2) {
	switch (p->type) {
	  case PT_LT:
	    cont=LTFSFileFind(p,cur_dir_cluster,
	      buf2,&de,TRUE,FALSE);
	    break;
	  case PT_FAT32:
	    cont=FAT32FileFind(p,cur_dir_cluster,
	      buf2,&de,TRUE,FALSE);
	    break;
	  case PT_ISO9660:
	    cont=ISO1FileFind(p,cur_dir_cluster,
	      buf2,&de,TRUE,FALSE);
	    break;
	  default:
	    Dbg;
	}
	cur_dir_cluster=de.cluster;
      }
    }
    if (!cont) {
      "%s ",dirname;
      PutDefineErr("ST_FILE_NOT_FOUND");
      cur_dir_cluster=0;
    }
    if (unlock)
      PrtUnlock(p);
  } catch
    if (unlock)
      PrtUnlock(p);
  Free(buf);
  Free(buf2);
  return cur_dir_cluster;
}

I64 Name2ParentDirCluster(LTPrt *p,U8 *dirname)
{
  BoolI8 cont=TRUE;
  I64 cur_dir_cluster,cur_dir_cluster2;
  LTDirEntry de;
  U8 *buf=StrNew(dirname),*buf2=StrNew(dirname);
  BoolI8 unlock;
  PrtChk(p);
  try {
    unlock=PrtLock(p);
    cur_dir_cluster=p->root_cluster;
    cur_dir_cluster2=p->root_cluster;
    while (*buf && cont) {
      cur_dir_cluster2=cur_dir_cluster;
      StrFirstRem(buf,"/",buf2);
      if (*buf2) {
	switch (p->type) {
	  case PT_LT:
	    cont=LTFSFileFind(p,cur_dir_cluster,
	      buf2,&de,TRUE,FALSE);
	    break;
	  case PT_FAT32:
	    cont=FAT32FileFind(p,cur_dir_cluster,
	      buf2,&de,TRUE,FALSE);
	    break;
	  case PT_ISO9660:
	    cont=ISO1FileFind(p,cur_dir_cluster,
	      buf2,&de,TRUE,FALSE);
	    break;
	  default:
	    Dbg;
	}
	cur_dir_cluster=de.cluster;
      }
    }
    if (!cont) {
      "%s ",dirname;
      PutDefineErr("ST_FILE_NOT_FOUND");
      cur_dir_cluster2=0;
    }
    if (unlock)
      PrtUnlock(p);
  } catch
    if (unlock)
      PrtUnlock(p);
  Free(buf);
  Free(buf2);
  return cur_dir_cluster2;
}


I64 WClustersBlks(LTPrt *p,U8 *buf,I64 c,I64 blks,BoolI8 write_cache=FALSE)
{ //This accepts blk count, so padding on last cluster is not written.
  I64 i;
  BoolI8 unlock;
  PrtChk(p);
  try {
    unlock=PrtLock(p);
    switch (p->type) {
      case PT_LT:
	WBlks(p,buf,p->data+c*p->spc,blks,write_cache);
	c=0;
	break;
      case PT_FAT32:
	while (blks) {
	  if (!c || c>=0x0FFFFFF8)
	    Dbg;
	  i=blks;
	  if (i>p->spc)
	    i=p->spc;
	  WBlks(p,buf,p->data+c*p->spc,i,write_cache);
	  buf+=i<<BLK_SIZE_BITS;
	  c=ClusterNumNext(p,c);
	  blks-=i;
	}
	break;
      default:
	Dbg;
    }
    if (unlock)
      PrtUnlock(p);
  } catch
    if (unlock)
      PrtUnlock(p);
  return c;
}

I64 WClusters(LTPrt *p,U8 *buf,I64 c,I64 cnt,BoolI8 write_cache=FALSE)
{
  return WClustersBlks(p,buf,c,cnt*p->spc,write_cache);
}

//c=0 means first cluster in chain
I64 AllocClusters(LTPrt *p,I64 c=0,I64 cnt=1,BoolI8 contiguous=FALSE)
{
  PrtChk(p);
  if (cnt<=0)
    return c;
  switch (p->type) {
    case PT_LT:
      return LTFSAllocClusters(p,cnt);
    case PT_FAT32:
      if (contiguous) {
	if (c) throw(EXCEPT_FILE,3);
	return FAT32AllocContiguousClusters(p,cnt);
      } else
	return FAT32AllocClusters(p,c,cnt);
    default:
      Dbg;
  }
}
