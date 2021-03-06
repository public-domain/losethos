U0 LTDCRemDirtyQue(LTCacheBlk *tempc)
{
  tempc->next_dirty->last_dirty=tempc->last_dirty;
  tempc->last_dirty->next_dirty=tempc->next_dirty;
}

U0 LTDCInsDirtyQue(LTCacheBlk *tempc)
{
  LTCacheBlk *temp_n,*temp_l;
  temp_l=sys_ltdc_ctrl->last_dirty;
  temp_n=temp_l->next_dirty;
  tempc->last_dirty=temp_l;
  tempc->next_dirty=temp_n;
  temp_l->next_dirty=temp_n->last_dirty=tempc;
  tempc->time_stamp=GetTimeStamp;
  LBtr(&sys_ltdc_write_out_task->task_flags,TASKf_SUSPENDED);
}

U0 DskCacheWriteOut(U64 time_stamp_delta,BoolI8 lock)
{
  U64 timeout_val=GetTimeStamp-time_stamp_delta;
  LTCacheBlk *tempc;
  LTPrt *p;
  BoolI8 success;
  if (lock)
    while (LBts(&sys_semas[SYS_SEMA_DSK_CACHE],0))
      Yield;
  while (sys_ltdc_ctrl->next_dirty!=sys_ltdc_ctrl) {
    if (!time_stamp_delta || sys_ltdc_ctrl->next_dirty->time_stamp<timeout_val) {
      tempc=sys_ltdc_ctrl->next_dirty;
      try {
	PrtLock(p=tempc->p); //TODO: deadlock on part waiting for cache
	if (tempc==sys_ltdc_ctrl->next_dirty) {
	  LTDCRemDirtyQue(tempc);
	  if (!Bt(&sys_semas[SYS_SEMA_NO_IO_SND],0))
	    Snd(p->bdev->write_freq);
	  (*p->wblks)(p,tempc->body,tempc->blk,1);
	  tempc->flags&=~(1<<LTDCf_DIRTY);
	  if (!Bt(&sys_semas[SYS_SEMA_NO_IO_SND],0))
	    Snd(0);
	  success=TRUE;
	} else
	  success=FALSE;
	PrtUnlock(p);
	if (!success)
	  Yield;
      } catch
	PrtUnlock(p);
    } else
      break;
  }
  if (lock)
    LBtr(&sys_semas[SYS_SEMA_DSK_CACHE],0);
}

U0 DskCacheWriteOutTask(U64 dummy=0)
{
  nounusedwarn dummy;
  TaskLocalInit; //needed for disk access
  while (TRUE) {
    while (sys_ltdc_ctrl->next_dirty!=sys_ltdc_ctrl) {
      DskCacheWriteOut(time_stamp_freq,TRUE); //One second
      Sleep(1);
    }
    LBts(&Fs->task_flags,TASKf_SUSPENDED);
    Yield;
  }
}

U0 LTDCInit(U64 size_in_U8s)
{
  LTCacheBlk *tempc;
  U64 i,cnt;

  while (LBts(&sys_semas[SYS_SEMA_DSK_CACHE],0))
    Yield;
  Free(sys_ltdc_ctrl);
  Free(sys_ltdc_base);
  Free(sys_ltdc_hash_table);
//TODO:to make recallable safe(to reinitialize cache live), need write-out-cache
  if (size_in_U8s<0x2000) {
    sys_ltdc_ctrl=NULL;
    sys_ltdc_base=NULL;
    sys_ltdc_hash_table=NULL;
  } else {
    sys_ltdc_ctrl=ACAlloc(offset(LTCacheBlk.body));
    sys_ltdc_base=AMAlloc(size_in_U8s);
    sys_ltdc_ctrl->next_lru=sys_ltdc_ctrl->last_lru=sys_ltdc_ctrl;
    sys_ltdc_ctrl->next_dirty=sys_ltdc_ctrl->last_dirty=sys_ltdc_ctrl;

    cnt=MSize(sys_ltdc_base)/sizeof(LTCacheBlk);
    sys_ltdc_size=cnt*BLK_SIZE;
    for (i=0;i<cnt;i++) {
      tempc=sys_ltdc_base+i;
      InsQue(tempc,sys_ltdc_ctrl->last_lru);
      tempc->next_hash=tempc->last_hash=
	tempc->next_dirty=tempc->last_dirty=tempc;
      tempc->flags=0;
      tempc->p=NULL;
      tempc->blk=0;
    }

    sys_ltdc_hash_table=AMAlloc(LTDC_TABLE_SIZE*sizeof(U0 *)*2);
    for (i=0;i<LTDC_TABLE_SIZE;i++) {
      tempc=sys_ltdc_hash_table(U8 *)+i*sizeof(U0 *)*2-offset(LTCacheBlk.next_hash);
      tempc->next_hash=tempc->last_hash=tempc;
    }
  }
  LBtr(&sys_semas[SYS_SEMA_DSK_CACHE],0);
}

U64 LTDCHash(U64 blk)
{
  U64 i=blk & (LTDC_TABLE_SIZE-1);
  return sys_ltdc_hash_table(U8 *)+i<<4-offset(LTCacheBlk.next_hash);
}

U0 LTDCRemQue(LTCacheBlk *tempc)
{
  RemQue(tempc);
  tempc->next_hash->last_hash=tempc->last_hash;
  tempc->last_hash->next_hash=tempc->next_hash;
}

U0 LTDCInsQue(LTCacheBlk *tempc)
{
  LTCacheBlk *temp_n,*temp_l;
  InsQue(tempc,sys_ltdc_ctrl->last_lru);
  temp_l=LTDCHash(tempc->blk);
  temp_n=temp_l->next_hash;
  tempc->last_hash=temp_l;
  tempc->next_hash=temp_n;
  temp_l->next_hash=temp_n->last_hash=tempc;
}

LTCacheBlk *LTDCFindBlk(LTPrt *p,U64 blk)
{
  LTCacheBlk *tempc,*tempc1=LTDCHash(blk);
  tempc=tempc1->next_hash;
  while (tempc!=tempc1) {
    if (tempc->p==p && tempc->blk==blk)
      return tempc;
    tempc=tempc->next_hash;
  }
  return NULL;
}

U0 LTDCAdd(LTPrt *p,U8 *buf, U64 blk, U64 cnt,BoolI8 write,BoolI8 write_cache)
{
  U64 i,*dd1,*dd2;
  LTCacheBlk *tempc;
  BoolI8 cache;
  if (sys_ltdc_base) {
    while (LBts(&sys_semas[SYS_SEMA_DSK_CACHE],0))
      Yield;
    while (cnt--) {
      cache=TRUE;
      if (tempc=LTDCFindBlk(p,blk)) {
	if (write_cache) {
	  if (tempc->flags & 1<<LTDCf_DIRTY) {
	    LTDCRemDirtyQue(tempc);
	    LTDCInsDirtyQue(tempc);
	  } else {
	    dd1=&tempc->body;
	    dd2=buf;
	    for (i=0;i<BLK_SIZE/sizeof(U64);i++)
	      if (dd1[i]!=dd2[i]) {
		tempc->flags=1<<LTDCf_DIRTY;
		LTDCInsDirtyQue(tempc);
		break;
	      }
	  }
	} else if (write && tempc->flags&1<<LTDCf_DIRTY) {
	  LTDCRemDirtyQue(tempc);
	  tempc->flags&=~(1<<LTDCf_DIRTY);
	}
      } else {
	tempc=sys_ltdc_ctrl->next_lru;
	if (tempc->p && tempc->p->flags & 1<<LTPf_CACHE_WRITES &&
	    tempc->flags & 1<<LTDCf_DIRTY) { //cache is full
	  cache=FALSE;
	  if (write_cache) {
	    if (!Bt(&sys_semas[SYS_SEMA_NO_IO_SND],0))
	      Snd(p->bdev->write_freq);
	    (*p->wblks)(p,buf,blk,1);
	    if (!Bt(&sys_semas[SYS_SEMA_NO_IO_SND],0))
	      Snd(0);
	  }
	} else {
	  if (write_cache) {
	    tempc->flags=1<<LTDCf_DIRTY;
	    LTDCInsDirtyQue(tempc);
	  } else
	    tempc->flags=0;
	}
      }
      if (cache) {
	LTDCRemQue(tempc);
	MemCpy(&tempc->body,buf,BLK_SIZE);
	tempc->p=p;
	tempc->blk=blk;
	LTDCInsQue(tempc);
      }
      blk++;
      buf+=BLK_SIZE;
    }
    LBtr(&sys_semas[SYS_SEMA_DSK_CACHE],0);
  }
}

U0 LTDCInvalidate(LTPrt *p)
{
  LTCacheBlk *tempc,*tempc1;
  if (sys_ltdc_base) {
    while (LBts(&sys_semas[SYS_SEMA_DSK_CACHE],0))
      Yield;
    DskCacheWriteOut(0,FALSE);
    tempc=sys_ltdc_ctrl->last_lru;
    while (tempc!=sys_ltdc_ctrl) {
      tempc1=tempc->last_lru;
      if (tempc->p==p) {
	LTDCRemQue(tempc);
	tempc->p=NULL;
	tempc->blk=0;
	tempc->flags=0;
	tempc->next_hash=tempc->last_hash=tempc;
	InsQue(tempc,sys_ltdc_ctrl->last_lru);
      }
      tempc=tempc1;
    }
    LBtr(&sys_semas[SYS_SEMA_DSK_CACHE],0);
  }
}

U0 RCache(LTPrt *p,U8 **buf, U64 *blk, U64 *cnt)
{
  LTCacheBlk *tempc;
  if (sys_ltdc_base) {
    while (LBts(&sys_semas[SYS_SEMA_DSK_CACHE],0))
      Yield;
    //fetch leading blks from cache
    while (*cnt) {
      if (tempc=LTDCFindBlk(p,*blk)) {
	MemCpy(*buf,&tempc->body,BLK_SIZE);
	*cnt-=1;
	*buf+=BLK_SIZE;
	*blk+=1;
      } else
	break;
    }
    //fetch trailing blks from cache
    while (*cnt) {
      if (tempc=LTDCFindBlk(p,*blk+*cnt-1)) {
	MemCpy(*buf+(*cnt-1)<<BLK_SIZE_BITS,&tempc->body,BLK_SIZE);
	*cnt-=1;
      } else
	break;
    }
    LBtr(&sys_semas[SYS_SEMA_DSK_CACHE],0);
  }
}

BoolI64 CacheWrites(U8 drv_let,BoolI8 val,BoolI8 all_writes=FALSE)
//This is risky but might boost performance.
//$BK,1$TODO:This feat is broken!  Deadlocks.$BK,0$
{
nounusedwarn drv_let,val,all_writes;
"Disk Write Caching is not implemented.\n";
return FALSE;
/*
  LTPrt *p=Drv2Prt(drv_let),*old_prt=Fs->cur_prt;
  U8 *old_dir=StrNew(Fs->cur_dir);
  Drv(drv_let);
  while (LBts(&sys_semas[SYS_SEMA_DSK_CACHE],0))
    Yield;
  if (val) {
    if (p->bdev->flags & LTBDF_READ_CACHE) {
      if (!sys_ltdc_write_out_task)
	sys_ltdc_write_out_task=Spawn(&DskCacheWriteOutTask,NULL,"Dsk Cache");
      LBts(&p->flags,LTPf_CACHE_WRITES);
      if (all_writes)
	LBts(&p->flags,LTPf_CACHE_ALL_WRITES);
    }
  } else
    if (LBtr(&p->flags,LTPf_CACHE_WRITES))
      DskCacheWriteOut(0,FALSE);
  LBtr(&sys_semas[SYS_SEMA_DSK_CACHE],0);

  Drv(Prt2Drv(old_prt));
  Cd(old_dir);
  Free(old_dir);
  return TRUE;
*/
}

BoolI64 RAMRBlks(LTPrt *p,U8 *buf,U64 blk,U64 cnt)
{
  LTBlkDev *bdev=p->bdev;
  BlkDevChk(bdev);
  MemCpy(buf,bdev->RAM_dsk+blk<<BLK_SIZE_BITS,cnt<<BLK_SIZE_BITS);
  return TRUE;
}

BoolI64 RAMWBlks(LTPrt *p,U8 *buf,U64 blk,U64 cnt)
{
  LTBlkDev *bdev=p->bdev;
  BlkDevChk(bdev);
  MemCpy(bdev->RAM_dsk+blk<<BLK_SIZE_BITS,buf,cnt<<BLK_SIZE_BITS);
  return TRUE;
}

BoolI64 NonLTFileRBlks(LTPrt *p,U8 *buf,U64 blk,U64 cnt)
{
  LTBlkDev *bdev=p->bdev;
  LTPrt *bp=Drv2Prt(bdev->non_lt_drv);
  BlkDevChk(bdev);
  return (*bp->rblks)(bp,buf,blk,cnt);
}

BoolI64 NonLTFileWBlks(LTPrt *p,U8 *buf,U64 blk,U64 cnt)
{
  LTBlkDev *bdev=p->bdev;
  LTPrt *bp=Drv2Prt(bdev->non_lt_drv);
  BlkDevChk(bdev);
  return (*bp->wblks)(bp,buf,blk,cnt);
}

