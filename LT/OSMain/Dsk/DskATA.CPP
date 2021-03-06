U0 ATABlkSelect(LTBlkDev *bdev,U64 blk,U64 cnt)
{
  if (bdev->type!=LTBDT_ATAPI && bdev->base1)
    OutU8(bdev->base1+ATAR1_CTRL,0x8);
  if (bdev->flags & LTBDF_EXT_SIZE) {
    OutU8(bdev->base0+ATAR0_NSECT,cnt.u8[1]);
    OutU8(bdev->base0+ATAR0_SECT,blk.u8[3]);
    OutU8(bdev->base0+ATAR0_LCYL,blk.u8[4]);
    OutU8(bdev->base0+ATAR0_HCYL,blk.u8[5]);
    OutU8(bdev->base0+ATAR0_NSECT,cnt);
    OutU8(bdev->base0+ATAR0_SECT,blk);
    OutU8(bdev->base0+ATAR0_LCYL,blk.u8[1]);
    OutU8(bdev->base0+ATAR0_HCYL,blk.u8[2]);
    OutU8(bdev->base0+ATAR0_SELECT,0xEF|bdev->unit<<4);
  } else {
    OutU8(bdev->base0+ATAR0_NSECT,cnt);
    OutU8(bdev->base0+ATAR0_SECT,blk);
    OutU8(bdev->base0+ATAR0_LCYL,blk.u8[1]);
    OutU8(bdev->base0+ATAR0_HCYL,blk.u8[2]);
    OutU8(bdev->base0+ATAR0_SELECT,0xE0|bdev->unit<<4|blk.u8[3]);
  }
}

BoolI64 ATAWaitNotBUSY(LTBlkDev *bdev,LTDate timeout)
{
  while (TRUE) {
    if (timeout && BootTime>timeout)
      return FALSE;
    if (InU8(bdev->base0+ATAR0_STAT)&ATAS_BSY)
      Yield;
    else
      return TRUE;
  }
}

BoolI64 ATAWaitDRQ(LTBlkDev *bdev,LTDate timeout)
{
  while (TRUE) {
    if (timeout && BootTime>timeout)
      return FALSE;
    if (!(InU8(bdev->base0+ATAR0_STAT)&ATAS_DRQ))
      Yield;
    else
      return TRUE;
  }
}

BoolI64 ATANop(LTBlkDev *bdev,LTDate timeout)
{
  if (bdev->flags & LTBDF_EXT_SIZE)
    OutU8(bdev->base0+ATAR0_SELECT,0xEF|bdev->unit<<4);
  else
    OutU8(bdev->base0+ATAR0_SELECT,0xE0|bdev->unit<<4);
  OutU8(bdev->base0+ATAR0_FEAT,0);
  OutU8(bdev->base0+ATAR0_CMD,ATA_NOP);
  return ATAWaitNotBUSY(bdev,timeout);
}

U0 ATACmd(LTBlkDev *bdev,U8 cmd)
{
  OutU8(bdev->base0+ATAR0_FEAT,0);
  OutU8(bdev->base0+ATAR0_CMD,cmd);
  bdev->last_time=GetTimeStamp;
  PortNop;
}

BoolI64 ATAGetResult(LTBlkDev *bdev,U8 *buf,I64 cnt,I64 _avail,BoolI64 one_read,LTDate timeout=0)
{
  I64 avail,overflow;
  bdev->flags&=~LTBDF_LAST_WAS_WRITE;
  MemSet(buf,0,cnt);
  while (cnt) {
    if (!ATAWaitDRQ(bdev,timeout))
      return FALSE;
    if (_avail)
      avail=_avail;
    else
      avail=InU8(bdev->base0+ATAR0_HCYL)<<8+InU8(bdev->base0+ATAR0_LCYL);
    if (avail) {
      if (avail>cnt) {
	overflow=avail-cnt;
	avail=cnt;
      } else
	overflow=0;
      if (avail&2)
	RepInU16(buf,avail>>1,bdev->base0+ATAR0_DATA);
      else
	RepInU32(buf,avail>>2,bdev->base0+ATAR0_DATA);
      cnt-=avail;
      buf+=avail;
      while (overflow>0) {
	if (timeout && BootTime>timeout)
	  return FALSE;
	InU16(bdev->base0+ATAR0_DATA);
	overflow-=2;
      }
      if (one_read)
	break;
    } else
      Yield;
  }
  return ATAWaitNotBUSY(bdev,timeout);
}

BoolI64 ATAPIWritePktWord(LTBlkDev *bdev,U16 w,LTDate timeout=0)
{
  if (!ATAWaitDRQ(bdev,timeout))
    return FALSE;
  OutU16(bdev->base0+ATAR0_DATA,BigEndianU16(w));
  bdev->last_time=GetTimeStamp;
  return TRUE;
}

BoolI64 ATAPISetMaxSpeed(LTBlkDev *bdev)
{
  if (bdev->flags & LTBDF_EXT_SIZE)
    OutU8(bdev->base0+ATAR0_SELECT,0xEF|bdev->unit<<4);
  else
    OutU8(bdev->base0+ATAR0_SELECT,0xE0|bdev->unit<<4);
  OutU8(bdev->base0+ATAR0_LCYL,0);
  OutU8(bdev->base0+ATAR0_HCYL,0);
  ATACmd(bdev,ATA_PACKET);
  ATAPIWritePktWord(bdev,0xBB00); //Set speed
  ATAPIWritePktWord(bdev,0xFFFF); //read at max
  ATAPIWritePktWord(bdev,0xFFFF); //write at max
  ATAPIWritePktWord(bdev,0);
  ATAPIWritePktWord(bdev,0);
  ATAPIWritePktWord(bdev,0);
  return ATAWaitNotBUSY(bdev,0);
}


BoolI64 ATAPISeek(LTBlkDev *bdev,U64 native_blk)
{
  if (bdev->flags & LTBDF_EXT_SIZE)
    OutU8(bdev->base0+ATAR0_SELECT,0xEF|bdev->unit<<4);
  else
    OutU8(bdev->base0+ATAR0_SELECT,0xE0|bdev->unit<<4);
  OutU8(bdev->base0+ATAR0_LCYL,0);
  OutU8(bdev->base0+ATAR0_HCYL,0);
  ATACmd(bdev,ATA_PACKET);
  ATAPIWritePktWord(bdev,0x2B00); //Seek
  ATAPIWritePktWord(bdev,native_blk>>16);
  ATAPIWritePktWord(bdev,native_blk);
  ATAPIWritePktWord(bdev,0);
  ATAPIWritePktWord(bdev,0);
  ATAPIWritePktWord(bdev,0x0000);
  return ATAWaitNotBUSY(bdev,0);
}

BoolI64 ATAPIStartStop(LTBlkDev *bdev,BoolI8 start,LTDate timeout)
{
  if (bdev->flags & LTBDF_EXT_SIZE)
    OutU8(bdev->base0+ATAR0_SELECT,0xEF|bdev->unit<<4);
  else
    OutU8(bdev->base0+ATAR0_SELECT,0xE0|bdev->unit<<4);
  ATACmd(bdev,ATA_PACKET);

  //Start/Stop
  if (!ATAPIWritePktWord(bdev,0x1B00,timeout))		return FALSE;
  if (!ATAPIWritePktWord(bdev,0x0000,timeout))		return FALSE;
  if (start) {
    if (!ATAPIWritePktWord(bdev,0x0100,timeout))	return FALSE;
  } else {
    if (!ATAPIWritePktWord(bdev,0x0000,timeout))	return FALSE;
  }
  if (!ATAPIWritePktWord(bdev,0x0000,timeout))		return FALSE;
  if (!ATAPIWritePktWord(bdev,0x0000,timeout))		return FALSE;
  if (!ATAPIWritePktWord(bdev,0x0000,timeout))		return FALSE;
  return ATAWaitNotBUSY(bdev,timeout);
}

I64 ATAGetDevId(LTBlkDev *bdev,U16 **_id_record,LTDate timeout)
{
  U16 *id_record;
  if (_id_record) *_id_record=NULL;
  if (bdev->type!=LTBDT_ATAPI && bdev->base1)
    OutU8(bdev->base1+ATAR1_CTRL,0x8);
  if (bdev->flags & LTBDF_EXT_SIZE)
    OutU8(bdev->base0+ATAR0_SELECT,0xEF|bdev->unit<<4);
  else
    OutU8(bdev->base0+ATAR0_SELECT,0xE0|bdev->unit<<4);
  ATACmd(bdev,ATA_ID_DEV);
  if (!ATAWaitNotBUSY(bdev,timeout))
    return LTBDT_NULL;
  if (InU8(bdev->base0+ATAR0_STAT)&ATAS_ERR)
    return LTBDT_ATAPI;
  id_record=ACAlloc(512);
  if (!ATAGetResult(bdev,id_record,512,512,FALSE,timeout)) {
    Free(id_record);
    return LTBDT_NULL;
  }
  if (_id_record) *_id_record=id_record;
  return LTBDT_ATA;
}

I64 ATAReadNativeMax(LTBlkDev *bdev)
{ //returns zero on err
  I64 blk=0;
  BoolI8 okay=TRUE;

  if (bdev->type==LTBDT_ATAPI) {
    if (bdev->flags & LTBDF_EXT_SIZE)
      OutU8(bdev->base0+ATAR0_SELECT,0xEF|bdev->unit<<4);
    else
      OutU8(bdev->base0+ATAR0_SELECT,0xE0|bdev->unit<<4);
    ATACmd(bdev,ATA_DEV_RST);
    if (!ATAWaitNotBUSY(bdev,0))
      okay=FALSE;
  } else {
    while (InU8(bdev->base0+ATAR0_STAT) & ATAS_BSY) {
      if (bdev->flags&LTBDF_LAST_WAS_WRITE)
	OutU16(bdev->base0+ATAR0_DATA,0);
      else
	InU16(bdev->base0+ATAR0_DATA);
      Yield;
    }

    Free(bdev->dev_id_record);
    if (ATAGetDevId(bdev,&bdev->dev_id_record,0)==LTBDT_NULL)
      okay=FALSE;
    else
      BEqu(&bdev->flags,LTBDf_EXT_SIZE,Bt(&bdev->dev_id_record[86],10));
  }
  if (okay) {
    if (bdev->flags & LTBDF_EXT_SIZE && bdev->base1) {
      OutU8(bdev->base1+ATAR1_CTRL,0x8);
      OutU8(bdev->base0+ATAR0_SELECT,0xEF|bdev->unit<<4);
      ATACmd(bdev,ATA_READ_NATIVE_MAX_EXT);
      if (ATAWaitNotBUSY(bdev,0)) {
	blk.u8[0]=InU8(bdev->base0+ATAR0_SECT);
	blk.u8[1]=InU8(bdev->base0+ATAR0_LCYL);
	blk.u8[2]=InU8(bdev->base0+ATAR0_HCYL);

	OutU8(bdev->base1+ATAR1_CTRL,0x80);
	blk.u8[3]=InU8(bdev->base0+ATAR0_SECT);
	blk.u8[4]=InU8(bdev->base0+ATAR0_LCYL);
	blk.u8[5]=InU8(bdev->base0+ATAR0_HCYL);
 
	if (blk>>24==blk&0xFFFFFF) {//Kludge to make qemu-kvm work
	  Btr(&bdev->flags,LTBDf_EXT_SIZE);
	  blk&=0xFFFFFF;
	}
      }
    } else {
      if (bdev->type!=LTBDT_ATAPI && bdev->base1)
	OutU8(bdev->base1+ATAR1_CTRL,0x8);
      OutU8(bdev->base0+ATAR0_SELECT,0xE0|bdev->unit<<4);
      ATACmd(bdev,ATA_READ_NATIVE_MAX);
      if (ATAWaitNotBUSY(bdev,0)) {
	blk.u8[0]=InU8(bdev->base0+ATAR0_SECT);
	blk.u8[1]=InU8(bdev->base0+ATAR0_LCYL);
	blk.u8[2]=InU8(bdev->base0+ATAR0_HCYL);
	blk.u8[3]=InU8(bdev->base0+ATAR0_SELECT) & 0xF;
      }
    }
  }
  return bdev->max_blk=blk;
}

U64 ATAPIReadCapacity(LTBlkDev *bdev,U64 *_blk_size=NULL)
{ //Supposedly this can return a result +/- 75 sects.
//Error might just be for music.
  BoolI8 unlock=BlkDevLock(bdev);
  U32 buf[2];
  if (ATAWaitNotBUSY(bdev,0)) {
    if (bdev->flags & LTBDF_EXT_SIZE)
      OutU8(bdev->base0+ATAR0_SELECT,0xEF|bdev->unit<<4);
    else
      OutU8(bdev->base0+ATAR0_SELECT,0xE0|bdev->unit<<4);
    OutU8(bdev->base0+ATAR0_LCYL,8);
    OutU8(bdev->base0+ATAR0_HCYL,0);
    ATACmd(bdev,ATA_PACKET);
    ATAPIWritePktWord(bdev,0x2500);  //Read Capacity
    ATAPIWritePktWord(bdev,0x0000);
    ATAPIWritePktWord(bdev,0x0000);
    ATAPIWritePktWord(bdev,0x0000);
    ATAPIWritePktWord(bdev,0x0000);
    ATAPIWritePktWord(bdev,0x0000);
    if (!ATAGetResult(bdev,buf,8,0,TRUE))
      buf[0]=buf[1]=0;
  } else
    buf[0]=buf[1]=0;

  if (unlock) BlkDevUnlock(bdev);
  if (_blk_size) *_blk_size=BigEndianU32(buf[1]);
  return BigEndianU32(buf[0]);
}

ATAPITrackInfo *ATAPIReadTrackInfo(LTBlkDev *bdev,U64 blk)
{
  ATAPITrackInfo *result=CAlloc(sizeof(ATAPITrackInfo));
  BoolI8 unlock=BlkDevLock(bdev);
  if (ATAWaitNotBUSY(bdev,0)) {
    if (bdev->flags & LTBDF_EXT_SIZE)
      OutU8(bdev->base0+ATAR0_SELECT,0xEF|bdev->unit<<4);
    else
      OutU8(bdev->base0+ATAR0_SELECT,0xE0|bdev->unit<<4);
    OutU8(bdev->base0+ATAR0_LCYL,sizeof(ATAPITrackInfo)&0xFF);
    OutU8(bdev->base0+ATAR0_HCYL,sizeof(ATAPITrackInfo)>>8);
    ATACmd(bdev,ATA_PACKET);
    ATAPIWritePktWord(bdev,0x5200);  //Read Track Info
    ATAPIWritePktWord(bdev,blk.u16[1]);
    ATAPIWritePktWord(bdev,blk.u16[0]);
    ATAPIWritePktWord(bdev,(sizeof(ATAPITrackInfo)&0xFF00)>>8);
    ATAPIWritePktWord(bdev,(sizeof(ATAPITrackInfo)&0x00FF)<<8);
    ATAPIWritePktWord(bdev,0x0000);
    if (!ATAGetResult(bdev,result,sizeof(ATAPITrackInfo),0,TRUE)) {
      Free(result);
      result=NULL;
    }
  } else {
    Free(result);
    result=NULL;
  }
  if (unlock) BlkDevUnlock(bdev);
  return result;
}

BoolI64 ATAInit(LTBlkDev *bdev)
{
  BoolI8 unlock=BlkDevLock(bdev),okay=FALSE;

  if (bdev->type==LTBDT_ATAPI)
    bdev->flags&=~LTBDF_EXT_SIZE;
  else
    bdev->flags|=LTBDF_EXT_SIZE;

  if (ATAReadNativeMax(bdev)) {
    ATABlkSelect(bdev,bdev->max_blk,0);
    if (bdev->flags&LTBDF_EXT_SIZE)
      ATACmd(bdev,ATA_SET_MAX_EXT);
    else
      ATACmd(bdev,ATA_SET_MAX);
    if (ATAWaitNotBUSY(bdev,0)) {
      okay=TRUE;
      if (bdev->type==LTBDT_ATAPI) {
	if (ATAPIStartStop(bdev,TRUE,0)) {
	  if(!ATAPISetMaxSpeed(bdev))
	    okay=FALSE;
	} else
	  okay=FALSE;
      }
    }
  }
  if (unlock) BlkDevUnlock(bdev);
  return okay;
}

BoolI64 ATAPIWaitReady(LTBlkDev *bdev,LTDate timeout)
{
  while (TRUE) {
    if (!ATAWaitNotBUSY(bdev,timeout) ||
	!ATANop(bdev,timeout) ||
	!ATAPIStartStop(bdev,TRUE,timeout))
      return FALSE;
    if (InU8(bdev->base0+ATAR0_STAT) & ATAS_DRDY &&
	!InU8(bdev->base0+ATAR0_FEAT));
      return TRUE;
    if (timeout && BootTime>timeout)
      return FALSE;
    ATAInit(bdev);
    Yield;
  }
}

U0 ATAReadBlks(LTBlkDev *bdev,U8 *buf, U64 blk, U64 cnt)
{
  I64 retries=3;
  BoolI8 unlock=BlkDevLock(bdev);

retry:
  ATABlkSelect(bdev,blk,cnt);
  if (bdev->flags & LTBDF_EXT_SIZE)
    ATACmd(bdev,ATA_READ_MULTI_EXT);
  else
    ATACmd(bdev,ATA_READ_MULTI);
  if (!ATAGetResult(bdev,buf,cnt*bdev->blk_size,BLK_SIZE,FALSE,BootTime+LTDATE_FREQ)) {
    if (retries--) {
      ATAWaitNotBUSY(bdev,0);
      goto retry;
    } else
      throw(EXCEPT_BLKDEV,6);
  }

  sys_dsk_reads+=(cnt*bdev->blk_size)>>BLK_SIZE_BITS;
  if (unlock) BlkDevUnlock(bdev);
}

I64 ATAProbe(I64 base0,I64 base1,I64 unit)
{
  LTBlkDev bdev;
  MemSet(&bdev,0,sizeof(LTBlkDev));
  bdev.type=LTBDT_ATAPI;
  bdev.base0=base0;
  bdev.base1=base1;
  bdev.unit=unit;
  bdev.blk_size=CD_BLK_SIZE;
  return ATAGetDevId(&bdev,NULL,BootTime+LTDATE_FREQ/10);
}

BoolI64 ATAPIReadBlks2(LTBlkDev *bdev,U8 *buf, U64 native_blk, U64 cnt,BoolI8 lock,LTDate timeout)
{
  BoolI8 result,unlock;
  if (!cnt)
    return FALSE;
  if (lock)
    unlock=BlkDevLock(bdev);
  if (!ATAPIWaitReady(bdev,timeout))			goto arb_false_done;
  if (bdev->flags & LTBDF_EXT_SIZE)
    OutU8(bdev->base0+ATAR0_SELECT,0xEF|bdev->unit<<4);
  else
    OutU8(bdev->base0+ATAR0_SELECT,0xE0|bdev->unit<<4);
  OutU8(bdev->base0+ATAR0_LCYL,bdev->blk_size);
  OutU8(bdev->base0+ATAR0_HCYL,bdev->blk_size.u8[1]);
  ATACmd(bdev,ATA_PACKET);
  if (!ATAPIWritePktWord(bdev,0xA800,timeout))		goto arb_false_done;
  if (!ATAPIWritePktWord(bdev,native_blk.u16[1],timeout))	goto arb_false_done;
  if (!ATAPIWritePktWord(bdev,native_blk,timeout))		goto arb_false_done;
  if (!ATAPIWritePktWord(bdev,cnt.u16[1],timeout))	goto arb_false_done;
  if (!ATAPIWritePktWord(bdev,cnt,timeout))		goto arb_false_done;
  if (!ATAPIWritePktWord(bdev,0x0000,timeout))		goto arb_false_done;
  if (!ATAGetResult(bdev,buf,cnt*bdev->blk_size,0,FALSE,timeout)) {
arb_false_done:
    result=FALSE;
  } else {
    sys_dsk_reads+=(cnt*bdev->blk_size)>>BLK_SIZE_BITS;
    result=TRUE;
  }
//  ATAPIStartStop(bdev,FALSE,0);
  if (lock && unlock) BlkDevUnlock(bdev);
  return result;
}

U0 ATAPIReadBlks(LTBlkDev *bdev,U8 *buf, U64 blk, U64 cnt)
{
  LTPrt *p=Drv2Prt(bdev->drv_let_base);
  U64 retry,spc=bdev->blk_size>>BLK_SIZE_BITS,n,blk2,
    l2=bdev->max_reads<<1+spc<<1;
  U8 *cd_buf=MAlloc(l2<<BLK_SIZE_BITS);
  if (cnt) {
    if (blk<=bdev->max_reads)
      blk2=0;
    else
      blk2=FloorU64(blk-bdev->max_reads,spc);
    if (blk2+l2>p->size)
      l2=p->size-blk2;
    n=(l2+spc-1)/spc;

    retry=4;
    while (--retry)
      if (ATAPIReadBlks2(bdev,cd_buf,blk2/spc,n,TRUE,
	//n is 0x800 if max_reads.  Up to 8 additional seconds
	BootTime+7*LTDATE_FREQ+n*LTDATE_FREQ/0x100))
	break;

    if (!retry)
      ATAPIReadBlks2(bdev,cd_buf,blk2/spc,n,TRUE,0);
    if (bdev->flags & LTBDF_READ_CACHE)
      LTDCAdd(p,cd_buf,blk2,n*spc,FALSE,FALSE);
    MemCpy(buf,cd_buf+(blk-blk2)<<BLK_SIZE_BITS,cnt<<BLK_SIZE_BITS);
  }
  Free(cd_buf);
}

U0 ATARBlks(LTPrt *p,U8 *buf, U64 blk, U64 cnt)
{
  U64 n;
  LTBlkDev *bdev=p->bdev;
  BlkDevChk(bdev);
  while (cnt>0) {
    n=cnt;
    if (n>bdev->max_reads)
      n=bdev->max_reads;
    if (bdev->type==LTBDT_ATAPI)
      ATAPIReadBlks(bdev,buf,blk,n);
    else
      ATAReadBlks(bdev,buf,blk,n);
    buf+=n<<BLK_SIZE_BITS;
    blk+=n;
    cnt-=n;
  }
}

//This is for low level disk access
//Call WBlks() instead
U0 ATAWriteBlks(LTBlkDev *bdev,U8 *buf, U64 blk, U64 cnt)
{
  U64 i,U32s_avail,sects_avail,retries=3;
  LTDate timeout;
  BoolI8 unlock=BlkDevLock(bdev);
retry:
  ATABlkSelect(bdev,blk,cnt);
  if (bdev->flags&LTBDF_EXT_SIZE)
    ATACmd(bdev,ATA_WRITE_MULTI_EXT);
  else
    ATACmd(bdev,ATA_WRITE_MULTI);
  bdev->flags|=LTBDF_LAST_WAS_WRITE;
  while (cnt) {
    timeout=BootTime+LTDATE_FREQ;
    while (TRUE) {
      i=InU8(bdev->base0+ATAR0_STAT);
      if (!(i & ATAS_DRDY)||!(i & ATAS_DRQ)) {
	Yield;
      } else
	break;
      if (/* i&ATAS_ERR||*/ BootTime>timeout) {
	if (retries--) {
	  ATAWaitNotBUSY(bdev,0);
	  goto retry;
	} else
	  throw(EXCEPT_BLKDEV,7);
      }
    }
    sects_avail=1;
    U32s_avail=sects_avail<<BLK_SIZE_BITS>>2;
    RepOutU32(buf,U32s_avail,bdev->base0+ATAR0_DATA);
    buf+=U32s_avail<<2;
    cnt-=sects_avail;
    retries=3;
  }
  ATAWaitNotBUSY(bdev,0);
  if (unlock) BlkDevUnlock(bdev);
}

BoolI64 ATAPISync(LTBlkDev *bdev)
{
  BoolI8 okay=TRUE;
  if (!ATAWaitNotBUSY(bdev,0))
    okay=FALSE;
  else {
    if (bdev->flags & LTBDF_EXT_SIZE)
      OutU8(bdev->base0+ATAR0_SELECT,0xEF|bdev->unit<<4);
    else
      OutU8(bdev->base0+ATAR0_SELECT,0xE0|bdev->unit<<4);
    OutU8(bdev->base0+ATAR0_LCYL,0);
    OutU8(bdev->base0+ATAR0_HCYL,0);
    ATACmd(bdev,ATA_PACKET);
    ATAPIWritePktWord(bdev,0x3500);  //Sync
    ATAPIWritePktWord(bdev,0x0000);
    ATAPIWritePktWord(bdev,0x0000);
    ATAPIWritePktWord(bdev,0x0000);
    ATAPIWritePktWord(bdev,0x0000);
    ATAPIWritePktWord(bdev,0x0000);
    if (!ATAWaitNotBUSY(bdev,0))
      okay=FALSE;
  }
  return okay;
}

U0 ATAPIClose(LTBlkDev *bdev,I64 close_field=0x200,I64 track=0)
{
//0x200 CD/DVD part 1
//0x300    DVD part 2
  if (bdev->flags & LTBDF_EXT_SIZE)
    OutU8(bdev->base0+ATAR0_SELECT,0xEF|bdev->unit<<4);
  else
    OutU8(bdev->base0+ATAR0_SELECT,0xE0|bdev->unit<<4);
  OutU8(bdev->base0+ATAR0_LCYL,0);
  OutU8(bdev->base0+ATAR0_HCYL,0);
  ATACmd(bdev,ATA_PACKET);
  ATAPIWritePktWord(bdev,0x5B00);  //Close
  ATAPIWritePktWord(bdev,close_field);
  ATAPIWritePktWord(bdev,track);
  ATAPIWritePktWord(bdev,0x0000);
  ATAPIWritePktWord(bdev,0x0000);
  ATAPIWritePktWord(bdev,0x0000);
  ATAWaitNotBUSY(bdev,0);
}

U0 ATAPIWriteBlks(LTBlkDev *bdev,U8 *buf, U64 native_blk, U64 cnt)
{
  U64 U32s_avail;
  U8 *buf2;
  ATAWaitNotBUSY(bdev,0);
  ATAPISeek(bdev,native_blk);

  OutU8(bdev->base0+ATAR0_FEAT,0);
  OutU8(bdev->base0+ATAR0_LCYL,bdev->blk_size);
  OutU8(bdev->base0+ATAR0_HCYL,bdev->blk_size.u8[1]);
  if (bdev->flags & LTBDF_EXT_SIZE)
    OutU8(bdev->base0+ATAR0_SELECT,0xEF|bdev->unit<<4);
  else
    OutU8(bdev->base0+ATAR0_SELECT,0xE0|bdev->unit<<4);
  OutU8(bdev->base0+ATAR0_CMD,ATA_PACKET);
  ATAPIWritePktWord(bdev,0x0400);  //FMT
  ATAPIWritePktWord(bdev,native_blk.u16[1]);
  ATAPIWritePktWord(bdev,native_blk);
  ATAPIWritePktWord(bdev,cnt.u16[1]);
  ATAPIWritePktWord(bdev,cnt);
  ATAPIWritePktWord(bdev,0x0000);
  bdev->flags|=LTBDF_LAST_WAS_WRITE;
  ATAWaitNotBUSY(bdev,0);

  ATAPISeek(bdev,native_blk);

  if (bdev->flags & LTBDF_EXT_SIZE)
    OutU8(bdev->base0+ATAR0_SELECT,0xEF|bdev->unit<<4);
  else
    OutU8(bdev->base0+ATAR0_SELECT,0xE0|bdev->unit<<4);
  OutU8(bdev->base0+ATAR0_LCYL,bdev->blk_size);
  OutU8(bdev->base0+ATAR0_HCYL,bdev->blk_size.u8[1]);
  ATACmd(bdev,ATA_PACKET);
  ATAPIWritePktWord(bdev,0xAA00);  //Write
  ATAPIWritePktWord(bdev,native_blk.u16[1]);
  ATAPIWritePktWord(bdev,native_blk);
  ATAPIWritePktWord(bdev,cnt.u16[1]);
  ATAPIWritePktWord(bdev,cnt);
  ATAPIWritePktWord(bdev,0x0000);
  buf2=buf+bdev->blk_size*cnt;
  while (buf<buf2) {
    ATAWaitDRQ(bdev,0);
    U32s_avail=(InU8(bdev->base0+ATAR0_HCYL)<<8+InU8(bdev->base0+ATAR0_LCYL))>>2;
    if (buf+U32s_avail<<2>buf2)
      U32s_avail=(buf2-buf)>>2;
    if (U32s_avail) {
      RepOutU32(buf,U32s_avail,bdev->base0+ATAR0_DATA);
      buf+=U32s_avail<<2;
      sys_dsk_writes+=U32s_avail>>(BLK_SIZE_BITS-2);
    }
  }
  ATAWaitNotBUSY(bdev,0);
}

U0 ATAWBlks(LTPrt *p,U8 *buf, U64 blk, U64 cnt)
{
  U64 n,spc;
  LTBlkDev *bdev=p->bdev;
  BoolI8 unlock;
  BlkDevChk(bdev);
  spc=bdev->blk_size>>BLK_SIZE_BITS;
  if (bdev->type==LTBDT_ATAPI) {
    unlock=BlkDevLock(bdev);
    ATAPIWaitReady(bdev,0);
  }
  while (cnt>0) {
    n=cnt;
    if (n>bdev->max_writes)
      n=bdev->max_writes;
    if (bdev->type==LTBDT_ATAPI)
      ATAPIWriteBlks(bdev,buf,blk/spc,(n+spc-1)/spc);
    else
      ATAWriteBlks(bdev,buf,blk,n);
    buf+=n<<BLK_SIZE_BITS;
    blk+=n;
    cnt-=n;
    sys_dsk_writes+=n;
  }
  if (bdev->type==LTBDT_ATAPI) {
    ATAPISync(bdev);
//    ATAPIStartStop(bdev,FALSE,0);
    if (unlock) BlkDevUnlock(bdev);
  }
}

