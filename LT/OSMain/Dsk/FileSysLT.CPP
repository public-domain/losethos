U0 LTFSFreeFreeList(LTPrt *p)
{
  LTFreeList *tempf,*tempf1;
  BoolI8 unlock;
  try {
    unlock=PrtLock(p);
 
    if (tempf=p->next_free) {
      while (tempf!=&p->next_free) {
	tempf1=tempf->next;
	Free(tempf);
	tempf=tempf1;
      }
    }
    p->next_free=NULL;
    if (unlock)
      PrtUnlock(p);
  } catch
    if (unlock)
      PrtUnlock(p);
}

U0 LTFSFreeListBuild(LTPrt *p)
{
  BoolI8 unlock;
  LTFreeList *tempf;
  I64 i,first=0,
      max_cluster=(p->size+p->offset-p->data)/p->spc;
 
  try {
    unlock=PrtLock(p);
    if (p->next_free)
      LTFSFreeFreeList(p);
    p->next_free=&p->next_free;
    p->last_free=&p->next_free;
    while (first<max_cluster) {
      i=0;  //count free clusters
      while (first+i<max_cluster) {
	SetLTBuf(p,first+i);
	if (Bt(p->cur_map_blk,(first+i)&(BLK_SIZE<<3-1)))
	  break;
	else
	  i++;
      }
      if (i) {
	tempf=AMAlloc(sizeof(LTFreeList));
	tempf->size=i;
	tempf->start=first;
	InsQue(tempf,p->last_free);
      }
      first+=i+1;
    }
    if (unlock)
      PrtUnlock(p);
  } catch
    if (unlock)
      PrtUnlock(p);
}

U0 LTFSPrtInit(LTPrt *p)
{
  LTBootStruct br;
  BoolI8 unlock;
  try {
    unlock=PrtLock(p);
    RBlks(p,&br,p->offset,1);
    if (br.signature!=MBR_PT_LT ||
    br.signature2!=0xAA55)
      throw(EXCEPT_PRT,0);
    p->type=PT_LT;
    LTFSFreeFreeList(p);
    p->spc=br.sects_per_cluster;
    p->size=br.sects;
    p->data=p->offset+1+br.bitmap_sects-p->spc;
    p->root_cluster=br.root_cluster;
    p->map1=p->offset+1;
    p->map2=p->offset+1;
    AllocMAPBuf(p,1);
    if (unlock)
      PrtUnlock(p);
  } catch
    if (unlock)
      PrtUnlock(p);
}

BoolI64 LTFSDrvValidate(U8 drv_let)
{
  LTPrt *p;
  LTBootStruct br;
  BoolI8 result=FALSE;
  try {
    p=Drv2Prt(drv_let);
    if (p->type==PT_LT &&
	RBlks(p,&br,p->offset,1) &&
	br.signature==MBR_PT_LT &&
	br.signature2==0xAA55 &&
	br.sects==p->size)
      result=TRUE;
  } catch
    Fs->catch_except=TRUE;
  return result;
}

U0 LTFSFmt(U8 drv_let,BoolI8 quick=TRUE)
{
  U8 *root_dir;
  LTDirEntry *dLT;
  LTBootStruct *br=CAlloc(BLK_SIZE);
  LTPrt *p=Drv2Prt(drv_let);
  I64 i,n;
  try {
    PrtLock(p);
    //	SetDrvType(drv_let,PT_LT);
    SetDrvType(drv_let,PT_FAT32);
    p->type=PT_LT;
    br->U8s_per_sect=BLK_SIZE;
    if	  (p->size<0x20000)
      br->sects_per_cluster=1;
    else if (p->size<0x200000)
      br->sects_per_cluster=2;
    else if (p->size<0x2000000)
      br->sects_per_cluster=4;
    else if (p->size<0x20000000)
      br->sects_per_cluster=8;
    else if (p->size<0x200000000)
      br->sects_per_cluster=16;
    else
      br->sects_per_cluster=32;
 
    br->signature=MBR_PT_LT;
    br->signature2=0xAA55;
    br->sects=p->size;
    n=br->sects_per_cluster*(BLK_SIZE<<3);
    n=(br->sects+n-1)/n;
    br->bitmap_sects=n;
    br->unique_id=BootTime;
    br->root_cluster=0;
 
    if (quick)
      i=n+1;
    else
      i=p->size;
    WZeroBlks(p,p->offset,i);
 
    WBlks(p,br,p->offset,1,TRUE);
    LTFSPrtInit(p);
    AllocClusters(p,0,1,FALSE);  //Alloc #1
    br->root_cluster=AllocClusters(p,0,1,FALSE);
    WBlks(p,br,p->offset,1,TRUE);
    root_dir=CAlloc(p->spc<<BLK_SIZE_BITS);
 
    dLT=root_dir-offset(LTDirEntry.start);
 
    dLT->attr=LT_ATTR_DIR|LT_ATTR_CONTIGUOUS;
    *dLT->name='.';
    dLT->cluster=br->root_cluster;
    dLT->size=p->spc<<BLK_SIZE_BITS;
    dLT->expanded_size=dLT->size;
    dLT->datetime=GetCurTimeLTDate;
 
    dLT(U8 *)+=LT_DIR_ENTRY_SIZE;
 
    *dLT->name='.';
    dLT->name[1]='.';
    dLT->attr=LT_ATTR_DIR|LT_ATTR_CONTIGUOUS;
    dLT->cluster=br->root_cluster;
    dLT->datetime=GetCurTimeLTDate;
 
    WBlks(p,root_dir,p->data+br->root_cluster*p->spc,p->spc,TRUE);
    LTFSPrtInit(p);
    PrtUnlock(p);
  } catch
    PrtUnlock(p);
  Free(br);
  Free(root_dir);
}

BoolI64 LTFSFileFind(LTPrt *p,U64 cur_dir_cluster,U8 *name,LTDirEntry *result,
		     BoolI8 dir_only,BoolI8 file_only)
{
  LTDirEntry *buf,*buf2,*ptr;
  U8 dname[LT_MAX_FILENAME_LEN+1];
  U64 ch;
  BoolI8 res=FALSE,unlock;
 
  MemSet(result,0,sizeof(LTDirEntry));
  PrtChk(p);
  if (p->type!=PT_LT)
    PutDefineErr("ST_NOT_LT");
  else if (!LTFileNameTo(dname,name))
    PutDefineErr("ST_INVALID_FILENAME");
  else {
    try {
      unlock=PrtLock(p);
      buf2=MAlloc(p->spc<<BLK_SIZE_BITS);
      RClusters(p,buf2,cur_dir_cluster,1);
 
      ptr=buf2(U8 *)-offset(LTDirEntry.start);
      buf=MAlloc(ptr->size);
      RClustersBlks(p,buf,cur_dir_cluster,ptr->size>>BLK_SIZE_BITS);
      Free(buf2);
 
      ptr=buf(U8 *)-offset(LTDirEntry.start);
      *ptr->name='.';
      ptr->name[1]=0;
      while (TRUE) {
	if (!(ch=*ptr->name))
	  break;
	else if (!(ptr->attr & LT_ATTR_DELETED) &&
	!(dir_only && !(ptr->attr & LT_ATTR_DIR)) &&
	!(file_only && ptr->attr & LT_ATTR_DIR) &&
	!StrCmp(dname,ptr->name)) {
	  MemCpy(&result->attr,&ptr->attr,LT_DIR_ENTRY_SIZE);
	  res=TRUE;
		goto exit_this;
	}
	ptr(U8 *)+=LT_DIR_ENTRY_SIZE;
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



U8 *LTFSFileRead(LTPrt *p,U8 *cur_dir,U8 *filename,I64 *size,
  I64 *attr,BoolI8 raw,ArcCompressStruct **ac2)
{
  U8 *buf=NULL;
  LTDirEntry de;
  I64 c,blk_cnt,cur_dir_cluster;
  ArcCompressStruct *ac;
 
  PrtChk(p);
  if (size)
    *size=0;
  if (ac2)
    *ac2=NULL;
  if (attr)
    *attr=0;
  if (p->type!=PT_LT)
    PutDefineErr("ST_NOT_LT");
  else {
    try {
      PrtLock(p);
      cur_dir_cluster=Name2DirCluster(p,cur_dir);
      if (LTFSFileFind(p,cur_dir_cluster,filename,&de,FALSE,TRUE)) {
	if (attr)
	  *attr=de.attr;
	blk_cnt=(de.size+BLK_SIZE-1)>>BLK_SIZE_BITS;
	buf=MAlloc(blk_cnt<<BLK_SIZE_BITS+1);
	c=de.cluster;
	c=RClustersBlks(p,buf,c,blk_cnt);
	if (de.xsum) {
	  if (de.xsum^ArcXSum(buf,de.size) !=LT_XSUM)
	    throw(EXCEPT_ARC_XSUM);
	}
	if (FileAttr(de.name) & LT_ATTR_COMPRESSED && !raw) {
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

BoolI64 LTFSCd(U8 *name,U64 cur_dir_cluster)
{
  LTDirEntry de;
  if (Fs->cur_prt->type!=PT_LT)
    PutDefineErr("ST_NOT_LT");
  else {
    if (LTFSFileFind(Fs->cur_prt,cur_dir_cluster,
			name,&de,TRUE,FALSE))
      return TRUE;
    else {
      "%s ",name;
      PutDefineErr("ST_FILE_NOT_FOUND");
    }
  }
  return FALSE;
}

U0 LTFSFreeClusters(LTPrt *p,I64 c,I64 cnt)
{
  LTFreeList *tempf;
  BoolI8 found=FALSE,unlock,unlock_break;
  PrtChk(p);
  if (!c) return;
  if (p->type!=PT_LT)
    PutDefineErr("ST_NOT_LT");
  else {
    try {
      unlock_break=BreakLock;
      unlock=PrtLock(p);
      if (!p->next_free)
	LTFSFreeListBuild(p);
      tempf=p->next_free;
      while (!found && tempf!=&p->next_free) {
	if (tempf->start+tempf->size==c) {
	  tempf->size+=cnt;
	  found=TRUE;
	} else if (c+cnt==tempf->start) {
	  tempf->size+=cnt;
	  tempf->start=c;
	  found=TRUE;
	}
	tempf=tempf->next;
      }
      if (!found) {
	tempf=AMAlloc(sizeof(LTFreeList));
	tempf->size=cnt;
	tempf->start=c;
	InsQue(tempf,p->last_free);
      }
      while (cnt--) {
	SetLTBuf(p,c);
	LBtr(p->cur_map_blk,c&(BLK_SIZE<<3-1));
	LBts(p->buffered_map_dirty_bits,0);
	c++;
      }
      CleanMAPBuf(p);
 
      if (unlock)
	PrtUnlock(p);
      if (unlock_break)
	BreakUnlock;
    } catch {
      if (unlock)
	PrtUnlock(p);
      if (unlock_break)
	BreakUnlock;
    }
  }
}

I64 LTFSAllocClusters(LTPrt *p,I64 cnt)
{
  LTFreeList *tempf,*best_free=NULL;
  I64 i,first,best_size=MAX_U64;
  BoolI8 unlock,unlock_break;
  if (!cnt)
    Dbg;
  try {
    unlock_break=BreakLock;
    unlock=PrtLock(p);
    if (!p->next_free)
      LTFSFreeListBuild(p);
    tempf=p->next_free;
    while (tempf!=&p->next_free) {
      if (tempf->size>=cnt && tempf->size<best_size) {
	best_free=tempf;
	best_size=tempf->size;
	if (tempf->size==cnt)
	  break;
      }
      tempf=tempf->next;
    }
    if (!best_free)
      Dbg;
    first=best_free->start;
    for (i=0;i<cnt;i++) {
      SetLTBuf(p,first+i);
      LBts(p->cur_map_blk,(first+i)&(BLK_SIZE<<3-1));
      LBts(p->buffered_map_dirty_bits,0);
    }
    CleanMAPBuf(p);
    if (best_free->size-=cnt)
      best_free->start+=cnt;
    else {
      RemQue(best_free);
      Free(best_free);
    }
    if (unlock)
      PrtUnlock(p);
    if (unlock_break)
      BreakUnlock;
  } catch {
    if (unlock)
      PrtUnlock(p);
    if (unlock_break)
      BreakUnlock;
  }
  return first;
}


BoolI64 LTFSDirEntryNew(LTPrt *p,U8 *cur_dir,LTDirEntry *de,BoolI8 free_old_chain)
{
  LTDirEntry *buf,*buf2,*ptr,de2;
  LTBootStruct *br;
  I64 c,ch,i=1,j=0,n=BLK_SIZE/LT_DIR_ENTRY_SIZE,dir_size,cur_dir_cluster;
  BoolI8 written=FALSE,unlock,unlock_break;
  U8 *temp,*parent_dir;
  try {
    unlock_break=BreakLock;
    de->attr|=LT_ATTR_CONTIGUOUS;
    unlock=PrtLock(p);
    cur_dir_cluster=Name2DirCluster(p,cur_dir);
    buf2=MAlloc(p->spc<<BLK_SIZE_BITS);
    RClusters(p,buf2,cur_dir_cluster,1);
 
    ptr=buf2(U8 *)-offset(LTDirEntry.start);
    buf=MAlloc(ptr->size);
    RClustersBlks(p,buf,cur_dir_cluster,ptr->size>>BLK_SIZE_BITS);
 
    dir_size=ptr->size;
    ptr=buf(U8 *)-offset(LTDirEntry.start)+LT_DIR_ENTRY_SIZE;
    Free(buf2);
    while (TRUE) {
      if (!(ch=*ptr->name)) {
	if (!written)
	  MemCpy(&ptr->start,&de->start,LT_DIR_ENTRY_SIZE);
	if ((i+1)*LT_DIR_ENTRY_SIZE+j<<BLK_SIZE_BITS<dir_size)
	  WBlks(p,buf(U8 *)+j<<BLK_SIZE_BITS,
	  p->data+cur_dir_cluster*p->spc+j,1,TRUE);
	else {
	  buf2=CAlloc(dir_size+p->spc<<BLK_SIZE_BITS);
	  MemCpy(buf2,buf,dir_size);
	  LTFSFreeClusters(p,cur_dir_cluster,
	    (dir_size>>BLK_SIZE_BITS+p->spc-1)/p->spc);
	  dir_size+=p->spc<<BLK_SIZE_BITS;
	  c=AllocClusters(p,0,(dir_size>>BLK_SIZE_BITS+p->spc-1)/p->spc,TRUE);
	  Free(buf);
	  buf=buf2;
	  ptr=buf(U8 *)-offset(LTDirEntry.start);
	  ptr->size=dir_size;
	  ptr->expanded_size=dir_size;
	  ptr->cluster=c;
	  WClustersBlks(p,buf,c,dir_size>>BLK_SIZE_BITS,TRUE);
	  if (cur_dir_cluster==p->root_cluster) {
	    br=CAlloc(BLK_SIZE);
	    RBlks(p,br,p->offset,1);
	    br->root_cluster=c;
	    WBlks(p,br,p->offset,1,TRUE);
	    Free(br);
	    p->root_cluster=c;
	  } else {
	    temp=StrNew(cur_dir);
	    parent_dir=StrNew(cur_dir);
	    StrLastRem(parent_dir,"/",temp);
	    if (!*parent_dir) {
	      Free(parent_dir);
	      parent_dir=StrNew("/");
	    }
	    if (LTFSFileFind(p,Name2DirCluster(p,parent_dir),
	    temp,&de2,TRUE,FALSE)) {
	      de2.cluster=c;
	      de2.size=dir_size;
	      de2.expanded_size=dir_size;
	      LTFSDirEntryNew(p,parent_dir,&de2,FALSE);
	    } else
	      Dbg;
	    Free(temp);
	    Free(parent_dir);
	  }
	}
	break;
      } else if (ptr->attr & LT_ATTR_DELETED) {
	if (!written) {
	  MemCpy(&ptr->start,&de->start,LT_DIR_ENTRY_SIZE);
	  WBlks(p,buf(U8 *)+j<<BLK_SIZE_BITS,
	  p->data+cur_dir_cluster*p->spc+j,1,TRUE);
	  written=TRUE;
	}
      } else {
	if (!StrCmp(de->name,ptr->name)) {
	  if (free_old_chain)
	    LTFSFreeClusters(p,ptr->cluster,
	    (ptr->size+p->spc<<BLK_SIZE_BITS-1)/p->spc<<BLK_SIZE_BITS);
	  if (!written)
	    MemCpy(&ptr->start,&de->start,LT_DIR_ENTRY_SIZE);
	  else
	    ptr->attr|=LT_ATTR_DELETED;
	    WBlks(p,buf(U8 *)+j<<BLK_SIZE_BITS,
	  p->data+cur_dir_cluster*p->spc+j,1,TRUE);
	  break;
	}
      }
      ptr(U8 *)+=LT_DIR_ENTRY_SIZE;
      if (++i>=n) {
	j++;
	i=0;
      }
    }
    Free(buf);
    if (unlock)
      PrtUnlock(p);
    if (unlock_break)
      BreakUnlock;
  } catch {
    if (unlock)
      PrtUnlock(p);
    if (unlock_break)
      BreakUnlock;
  }
  return FALSE;
}

BoolI64 LTFSFilesDel(LTPrt *p,U8 *cur_dir,U8 *files_find_mask,U64 fuf_flags,
		     BoolI8 del_dir,BoolI8 print_msg)
{
  LTDirEntry *buf,*buf2,*ptr;
  I64 i=0,ch,j=0,n=BLK_SIZE/LT_DIR_ENTRY_SIZE,cur_dir_cluster;
  BoolI8 unlock_break;
  try {
    unlock_break=BreakLock;
    PrtLock(p);
    cur_dir_cluster=Name2DirCluster(p,cur_dir);
    buf2=MAlloc(p->spc<<BLK_SIZE_BITS);
    RClusters(p,buf2,cur_dir_cluster,1);
 
    ptr=buf2(U8 *)-offset(LTDirEntry.start);
    buf=MAlloc(ptr->size);
    RClustersBlks(p,buf,cur_dir_cluster,ptr->size>>BLK_SIZE_BITS);
    Free(buf2);
 
    ptr=buf(U8 *)-offset(LTDirEntry.start);
    *ptr->name='.';
    ptr->name[1]=0;
    while (TRUE) {
      if (!(ch=*ptr->name))
	break;
      else if (!(ptr->attr & LT_ATTR_DELETED) && ch!='.' && (del_dir ||
	  !(ptr->attr & LT_ATTR_DIR)) &&
	  FilesFindMatch(files_find_mask,ptr->name,fuf_flags)) {
	if (print_msg) {
	  PutDefine("ST_DELETE");
	  "" ptr->name;
	  NewLine;
	}
	ptr->attr|=LT_ATTR_DELETED;
	WBlks(p,buf(U8 *)+j<<BLK_SIZE_BITS,
	p->data+cur_dir_cluster*p->spc+j,1,TRUE);
	LTFSFreeClusters(p,ptr->cluster,
	(ptr->size+p->spc<<BLK_SIZE_BITS-1)/p->spc<<BLK_SIZE_BITS);
      }
      ptr(U8 *)+=LT_DIR_ENTRY_SIZE;
      if (++i>=n) {
	j++;
	i=0;
      }
    }
    Free(buf);
    PrtUnlock(p);
    if (unlock_break)
      BreakUnlock;
  } catch {
    PrtUnlock(p);
    if (unlock_break)
      BreakUnlock;
  }
  return FALSE;
}

I64 LTFSFileWrite(LTPrt *p,U8 *cur_dir,U8 *name,U8 *buf,I64 size,
		LTDate date,I64 attr,BoolI8 raw,
		ArcCompressStruct **ac2)
{
  LTDirEntry de;
  I64 c=0,flags=0,blk_cnt;
  ArcCompressStruct *ac=NULL;
  BoolI8 contiguous;
  MemSet(&de,0,sizeof(LTDirEntry));
  if (ac2) *ac2=NULL;
  if (size<0) size=0;
  if (!date)
    date=GetCurTimeLTDate;

  if (p->type!=PT_LT)
    PutDefineErr("ST_NOT_LT");
  else if (!LTFileNameTo(de.name,name))
    PutDefineErr("ST_INVALID_FILENAME");
  else {
    LTFSFilesDel(p,cur_dir,de.name,0,FALSE,FALSE);
    attr=FileAttr(name,attr);
    attr|=LT_ATTR_CONTIGUOUS;
    contiguous=TRUE;
    de.expanded_size=size;
    if (attr&LT_ATTR_COMPRESSED && !raw) {
      ac=CompressBuf(buf,size,flags);
      size=ac->compressed_size;
      buf=ac;
    }
    de.size=size;
    de.xsum=ArcXSum(buf,size)^LT_XSUM;
    if (blk_cnt=(size+BLK_SIZE-1)>>BLK_SIZE_BITS)
      c=AllocClusters(p,0,(blk_cnt+p->spc-1)/p->spc,contiguous);
    else
      c=INVALID_CLUSTER;
    de.cluster=c;
    de.attr=attr;
    de.datetime=date;
    if (blk_cnt)
      WClustersBlks(p,buf,c,blk_cnt,FALSE);
    LTFSDirEntryNew(p,cur_dir,&de,TRUE);
    if (ac2)
      *ac2=ac;
    else
      Free(ac);
  }
  return c;
}

LTDirEntry *LTFSFilesFind(U8 *files_find_mask,U64 fuf_flags,LTDirEntry *parent=NULL)
{
  LTPrt *p=Fs->cur_prt;
  LTDirEntry *buf,*buf2,*ptr;
  U64 ch,cur_dir_cluster;
  LTDirEntry *result=NULL,*tempm;
 
  try {
    PrtLock(p);
    cur_dir_cluster=Name2DirCluster(p,Fs->cur_dir);
    buf2=MAlloc(p->spc<<BLK_SIZE_BITS);
    RClusters(p,buf2,cur_dir_cluster,1);
 
    ptr=buf2(U8 *)-offset(LTDirEntry.start);
    buf=MAlloc(ptr->size);
    RClustersBlks(p,buf,cur_dir_cluster,ptr->size>>BLK_SIZE_BITS);
    Free(buf2);
 
    ptr=buf(U8 *)-offset(LTDirEntry.start);
    *ptr->name='.';
    ptr->name[1]=0;
    ptr(U8 *)+=LT_DIR_ENTRY_SIZE;
    ptr->cluster=Name2ParentDirCluster(p,Fs->cur_dir);
    ptr(U8 *)-=LT_DIR_ENTRY_SIZE;
    while (TRUE) {
      if (!(ch=*ptr->name))
	break;
      else if (!(ptr->attr & LT_ATTR_DELETED)) {
	tempm=CAlloc(sizeof(LTDirEntry));
	MemCpy(&tempm->start,&ptr->start,LT_DIR_ENTRY_SIZE);
	tempm->parent=parent;
	if (Bt(&fuf_flags,FUf_RECURSE) && tempm->attr&LT_ATTR_DIR && *tempm->name!='.') {
	  tempm->next=result;
	  result=tempm;
	  tempm->full_name=DirNameAbs(tempm->name);
	  PrtUnlock(p);
	  if (Cd(tempm->name)) {
	    tempm->sub=LTFSFilesFind(files_find_mask,fuf_flags,tempm);
	    Cd("..");
	  }
	  PrtLock(p);
	} else if ((tempm->attr&LT_ATTR_DIR || !Bt(&fuf_flags,FUf_JUST_DIRS)) &&
	!(Bt(&fuf_flags,FUf_RECURSE) && *tempm->name=='.' && tempm->attr&LT_ATTR_DIR) &&
	  FilesFindMatch(files_find_mask,tempm->name,fuf_flags)) {
	  tempm->next=result;
	  result=tempm;
	  tempm->full_name=FileNameAbs(tempm->name);
	} else
	  LTDirEntryDel(tempm);
      }
      ptr(U8 *)+=LT_DIR_ENTRY_SIZE;
    }
    Free(buf);
    PrtUnlock(p);
  } catch
    PrtUnlock(p);
  return result;
}

BoolI64 LTFSMkDir(LTPrt *p,U8 *cur_dir,U8 *name)
{
  I64 size=p->spc<<BLK_SIZE_BITS,
  cur_dir_cluster=Name2DirCluster(p,cur_dir);
  U8 *buf=CAlloc(size);
  LTDirEntry *dLT=buf-offset(LTDirEntry.start);
  I64 c;
  BoolI8 unlock_break;
 
  try {
    unlock_break=BreakLock;
    c=FileWrite(name,buf,size,0,LT_ATTR_DIR,FALSE);
    dLT->attr=LT_ATTR_DIR|LT_ATTR_CONTIGUOUS;
    StrCpy(dLT->name,name);
    dLT->cluster=c;
    dLT->size=size;
    dLT->expanded_size=size;
    dLT->datetime=GetCurTimeLTDate;
    dLT(U8 *)+=LT_DIR_ENTRY_SIZE;
 
    dLT->attr=LT_ATTR_DIR|LT_ATTR_CONTIGUOUS;
    *dLT->name='.';
    dLT->name[1]='.';
    dLT->name[2]=0;
    dLT->cluster=cur_dir_cluster;
    dLT->size=0;
    dLT->expanded_size=0;
    dLT->datetime=GetCurTimeLTDate;
    WClusters(p,buf,c,1,TRUE);
    Free(buf);
    if (unlock_break)
      BreakUnlock;
  } catch
    if (unlock_break)
      BreakUnlock;
  return TRUE;
}

