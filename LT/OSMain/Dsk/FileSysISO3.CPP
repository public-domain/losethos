//ISO1 is ISO9660
//ISO2 is ISO13490
//ISO3 is ISO13346

import U0 ClassRep(U8 *class_name,U8 *d,I64 max_depth=2,BoolI8 fun=FALSE,I64 offset=0);

BoolI64 ISO3PrtInitPrt(LTPrt *p,U64 blk)
{
  LTBlkDev *bdev=p->bdev;
  U64 spc=bdev->blk_size>>BLK_SIZE_BITS,i=blk/spc;
  ISO3FileSetDesc *iso=MAlloc(bdev->blk_size);
  BoolI8 unlock,result=FALSE;

  try {
    unlock=PrtLock(p);
    while (TRUE) {
      p->size=MaxI64(p->size,(i+1)*spc);
      RBlks(p,iso,i*spc,spc);
      "%X:%X\n",i,iso->tag.id;
      switch (iso->tag.id) {
	case ISO3T_FILE_SET_DESC:
	  ClassRep("ISO3FileSetDesc",iso,3);
//	  p->offset=blk;
//	  p->root_cluster=iso->root_dir_icb.loc.log_blk_num+p->offset/spc;
	  result=TRUE;
	  break;
	case ISO3T_EXT_FILE_ENTRY:
	  ClassRep("ISO3ExtFileEntry",iso,3);
	  break;
	case ISO3T_FILE_ENTRY:
	  ClassRep("ISO3FileEntry",iso,3);
	  break;
	case ISO3T_FILE_ID_DESC:
	  ClassRep("ISO3FileIdDesc",iso,3);
	  break;
	case ISO3T_ALLOC_EXTENT_DESC:
	case ISO3T_INDIRECT_ENTRY:
	case ISO3T_TERMINAL_ENTRY:
	case ISO3T_EXT_ATTR_HEADER_DESC:
	case ISO3T_UNALLOC_SPACE_ENTRY:
	case ISO3T_SPACE_BITMAP_DESC:
	case ISO3T_PRT_INTEGRITY_ENTRY:
	  break;
	default
	  goto i3pip_done;
      }
      i++;
    }
i3pip_done:
    Free(iso);
    if (unlock)
      PrtUnlock(p);
  } catch {
    Free(iso);
    if (unlock)
      PrtUnlock(p);
  }
  return result;
}

BoolI64 ISO3PrtInit(LTPrt *p,U64 blk)
{
  LTBlkDev *bdev=p->bdev;
  U64 spc=bdev->blk_size>>BLK_SIZE_BITS,i=blk/spc,offset=0;
  ISO3Prt *iso=MAlloc(bdev->blk_size);
  BoolI8 unlock,result=FALSE;

  try {
    unlock=PrtLock(p);
    while (TRUE) {
      p->size=MaxI64(p->size,(i+1)*spc);
      RBlks(p,iso,i*spc,spc);
"%X:%X\n",i,iso->tag.id;
      switch (iso->tag.id) {
	case ISO3T_PRT_DESC:
	  ClassRep("ISO3Prt",iso,3);
	  offset=iso->starting_loc*spc; //one partition for now
	  break;
	case ISO3T_PRI_VOL_DESC:
	  ClassRep("ISO3PriDesc",iso,3);
	  break;
	case ISO3T_LOG_VOL_DESC:
	  ClassRep("ISO3LogVolDesc",iso,3);
	  D(&iso(ISO3LogVolDesc *)->prt_maps);
	  break;
	case ISO3T_ANCHOR_VOL_DESC_PTR:
	case ISO3T_VOL_DESC_PTR:
	case ISO3T_IMPLEMENTATION_VOL_DESC:
	case ISO3T_UNALLOCATED_SPACE_DESC:
	case ISO3T_LOG_VOL_INTEGRITY_DESC:
	  break;
	case ISO3T_TERMINATOR_DESC:
	  goto i3pi_done;
	default:
	  PutDefineErr("ST_UNSUPPORTED_FILE_SYSTEM");
	  throw(EXCEPT_PRT,12);
      }
      i++;
    }
i3pi_done:
    if (offset)
      result=ISO3PrtInitPrt(p,offset);
    Free(iso);
    if (unlock)
      PrtUnlock(p);
  } catch {
    Free(iso);
    if (unlock)
      PrtUnlock(p);
  }
  return result;
}


/****
U0 LTDate2ISO3(ISO3Date *dst,LTDate ltdate)
{
  LTDateStruct d;
  LTDate2LTDateStruct(&d,ltdate);
  MemSet(dst,0,sizeof(ISO3Date));
  dst->year=d.year;
  dst->mon=d.mon;
  dst->day=d.day_of_mon;
  dst->hour=d.hour;
  dst->min=d.min;
  dst->sec=d.sec;
  dst->sec100=d.sec100;
}

LTDate ISO3LTDateStruct2LTDate(ISO3Date *dt)
{
  LTDateStruct ltdt;
  MemSet(&ltdt,0,sizeof(LTDateStruct));
  ltdt.day_of_mon=dt->day;
  ltdt.mon=dt->mon;
  ltdt.year=dt->year;
  ltdt.sec100=dt->sec100;
  ltdt.sec=dt->sec;
  ltdt.min=dt->min;
  ltdt.hour=dt->hour;
  return LTDateStruct2LTDate(&ltdt);
}
****/
