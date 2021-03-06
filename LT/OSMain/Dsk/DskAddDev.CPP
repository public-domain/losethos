U0 SetBlkDevLockFwding(LTBlkDev *bdev)
{
  LTBlkDev *bd1;
  I64 i;
  switch (bdev->type) {
    case LTBDT_RAM:
      break;
    case LTBDT_ATA:
    case LTBDT_ATAPI:
      for (i=0;i<num_blkdevs;i++) {
	bd1=&local_blkdevs[i];
	if (bdev!=bd1 && (bd1->type==LTBDT_ATAPI || bd1->type==LTBDT_ATA) &&
	    bd1->base0==bdev->base0) {
	  bdev->lock_fwding=bd1;
	  break;
	}
      }
      break;
    case LTBDT_NON_LT_FILE:
      bdev->lock_fwding=Drv2BlkDev(bdev->non_lt_drv);
      break;
  }
}

U64 BlkDevAdd(LTBlkDev *bdev,BoolI8 one_prt)
{
  I64 i,j,ext_base,num_prt;
  U64 offset;
  LTPrt *p;
  LTBootStruct br;
  MasterBootStruct mbr;

  bdev->bd_signature=LTBLKDEV_SIGNATURE_VAL;
  num_prt=bdev->drv_let_base-'A';
  p=&local_prts[num_prt];
  MemSet(p,0,sizeof(LTPrt));
  p->drv_let=num_prt+'A';
  p->pt_signature=LTPRT_SIGNATURE_VAL;
  p->bdev=bdev;
  p->size=bdev->max_blk+1;
  switch (bdev->type) {
    case LTBDT_RAM:
      p->type=PT_LT;
      p->rblks=&RAMRBlks;
      p->wblks=&RAMWBlks;
      num_prt++;

//This is to force creation of a RAM
//drive during boot, so it is probably
//MAlloced to the same addr and can
//be assumed to be already formatted.
//If this line is removed, RAM Drives
//will be allocated on a just-in-time
//basis.
      BlkDevInit(bdev);

      break;
    case LTBDT_ATA:
      BlkDevInit(bdev);
      if (one_prt) {
	p->type=PT_LT;
	p->rblks=&ATARBlks;
	p->wblks=&ATAWBlks;
	p->size=bdev->max_blk+1;
	num_prt++;
	//The following read is a test read.
	//if it hangs, the drive is not supported.
	ATAReadBlks(bdev,&mbr,0,1);
	break;
      }
      offset=0;
      ext_base=INVALID_CLUSTER;
      while (TRUE) {
	ATAReadBlks(bdev,&mbr,offset,1);
	if (mbr.signature!=0xAA55)
	  break;
	j=-1;
	for (i=0;i<4;i++) {
	  if (mbr.p[i].type) {
	    p=&local_prts[num_prt];
	    MemSet(p,0,sizeof(LTPrt));
	    p->drv_let=num_prt+'A';
	    p->pt_signature=LTPRT_SIGNATURE_VAL;
	    p->bdev=bdev;
	    p->rblks=&ATARBlks;
	    p->wblks=&ATAWBlks;
	    p->offset=mbr.p[i].offset+offset;
	    p->size  =mbr.p[i].size;
	    p->type=PT_NULL;
	    switch (mbr.p[i].type) {
	      case MBR_PT_LT:
		p->type=PT_LT;
		LTFSPrtInit(p);
		num_prt++;
		break;
	      case MBR_PT_FAT32a:
	      case MBR_PT_FAT32b:
	      case MBR_PT_FAT32c:
	      case MBR_PT_FAT32d:
	      case MBR_PT_FAT32e:
	      case MBR_PT_FAT32f:
		ATAReadBlks(bdev,&br,p->offset,1);
		if (br.signature==MBR_PT_LT) {
		  p->type=PT_LT;
		  LTFSPrtInit(p);
		} else {
		  p->type=PT_FAT32;
		  FAT32PrtInit(p);
		}
		num_prt++;
		break;
	      case MBR_PT_NTFS:
		p->type=PT_NTFS;
		num_prt++;
		break;
	      case 5:
	      case 15:
		j=i;
		break;
	      default:
		p->type=PT_UNKNOWN;
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
    case LTBDT_ATAPI:
      p->type=PT_ISO9660; //Start with this
      p->rblks=&ATARBlks;
      p->wblks=&ATAWBlks;
      p->size =0;
      num_prt++;
      break;
    case LTBDT_NON_LT_FILE:
      p->type=PT_LT;
      p->offset=bdev->non_lt_offset;
      p->rblks=&NonLTFileRBlks;
      p->wblks=&NonLTFileWBlks;
      if (bdev->flags & LTBDF_FMT)
	p->size=bdev->max_blk-bdev->min_blk+1;
      num_prt++;
      break;
  }
  SetBlkDevLockFwding(bdev);
  return num_prt-(bdev->drv_let_base-'A');
}


BoolI64 DrvEnable(U8 drv_let,BoolI8 val)
{//This can be used to unmount a drive or remount,
//but not mount the first time.
  BoolI8 result=FALSE;
  LTPrt *p;
  try {
    p=Drv2Prt(drv_let);
    result=!LBEqu(&p->type,PTf_DISABLE,!val);
  } catch {
    Fs->ignore_except=TRUE;
  }
  return result;
}

ATARepStruct *ATARepFind(ATARepStruct *root,I64 num)
{
  ATARepStruct *tempha=root;
  while (tempha) {
    if (tempha->num==num)
      return tempha;
    tempha=tempha->next;
  }
  return tempha;
}

U0 Mount(BoolI8 repartition=FALSE)
{
  LTBlkDev *bdev;
  I64 drv_let_base,drv_let,type,k;
  U8 buf[32],buf2[128],buf3[128];
  BoolI8 add;
  ATARepStruct *root,*tempha;
  I64 num_hints;

  do {
    PrtRep;
    PutDefine("ST_MOUNT_BASE_LET");
    drv_let_base=ToUpper(GetChar)-'A';
    NewLine;
    if (0<=drv_let_base<NUM_LTPRTS) {
      PutDefine("ST_MOUNT_DRV_TYPE");
      type=GetChar-'0';
      NewLine;

      k=0;

      if (type==LTBDT_ATA || type==LTBDT_ATAPI) {
	num_hints=CallExtStr("ATARep",TRUE,&root);
	do {
	  if (num_hints)
	    PutDefine("ST_MOUNT_BASE0_HINT");
	  else
	    PutDefine("ST_MOUNT_BASE0");
	  GetS(buf2,128);
	} while (!A2I(buf2));
	if (1<=A2I(buf2)<=num_hints) {
	  tempha=ATARepFind(root,A2I(buf2));
	  SPrintF(buf2,"0x%X",tempha->base0);
	  SPrintF(buf3,"0x%X",tempha->base1);
	  k=tempha->unit;
	} else {
	  if (type==LTBDT_ATAPI)
	    StrCpy(buf3,"0");
	  else
	    do {
	      PutDefine("ST_MOUNT_BASE1");
	      GetS(buf3,128);
	    } while (!A2I(buf3));
	  do {
	    PutDefine("ST_MOUNT_UNIT");
	    k=GetChar-'0';
	  } while (!(0<=k<=1));
	  NewLine;
	}
	LinkedListDel(root);
      }
      if (type==LTBDT_NON_LT_FILE) {
	PutDefine("ST_MOUNT_NONLT_LET");
	drv_let=GetChar;
	NewLine;
	PutDefine("ST_INCLUDE_0X");
	PutDefine("ST_MOUNT_NONLT_BLK");
	GetS(buf2,128);
      }
      if (type==LTBDT_RAM) {
	PutDefine("ST_MOUNT_BLKS");
	GetS(buf,32);
      }

      if (LTBDT_NULL<type<=LTBDT_NON_LT_FILE &&
	  0<=k<=1) {
	bdev=&local_blkdevs[num_blkdevs++];
	bdev->drv_let_base=drv_let_base+'A';
	bdev->unit=k;
	bdev->type=type;

	if (type==LTBDT_ATAPI)
	  bdev->flags=LTBDF_REMOVABLE;
	else
	  bdev->flags=0;
	if (type!=LTBDT_RAM)
	  bdev->flags|=LTBDF_READ_CACHE;
 
	if (type==LTBDT_ATAPI) {
	  bdev->blk_size=CD_BLK_SIZE;
	  bdev->flags|=LTBDF_READ_ONLY;
	} else
	  bdev->blk_size=BLK_SIZE;

	if (type==LTBDT_RAM)
	  bdev->max_blk=A2I(buf)-1;
	else
	  bdev->max_blk=0xEFFFFFFF;

	if (type==LTBDT_NON_LT_FILE) {
	  bdev->non_lt_drv=drv_let;
	  bdev->non_lt_offset=A2I(buf2);
	} else {
	  bdev->non_lt_drv=0;
	  bdev->non_lt_offset=0;
	}

	if (type==LTBDT_ATA ||
	    type==LTBDT_ATAPI) {
	  bdev->base0=A2I(buf2);
	  bdev->base1=A2I(buf3);
	}

	add=TRUE;
	if (type==LTBDT_ATA && repartition) {
	  PutDefine("ST_MOUNT_FMT_WHOLE_DRV");
	  if (YorN) {
	    if (!BlkDevAdd(bdev,TRUE))
	      PutDefine("ST_FAILED");
	    else
	      CallExtStr("PrtDsk",bdev->drv_let_base);
	    add=FALSE;
	  }
	}
	if (add && !BlkDevAdd(bdev,FALSE))
	  PutDefine("ST_FAILED");
      } else
	PutDefine("ST_FAILED");
    }
  } while (0<=drv_let_base<NUM_LTPRTS);
}

I64 SysPmtI64()
{
  U8 st[136];
  StrCpy(st,"0x");
  GetS(st+2,134,FALSE);
  return A2I(st);
}

BoolI64 PmtBaseUnit(LTBlkDev *bdev)
{
  U64 ch;
  BoolI8 probe;
  "The LoseThos Operating System\n\n\n";
#exe {
  if (osmain_cfg->opts[CFG_DONT_PROBE])
    StreamPutS("probe=FALSE;");
  else
    StreamPutS("probe=TRUE;");
};
  if (!probe || !BootCdProbeAll(bdev)) {
	"\nThis is not a product registration.LoseThos\n"
	"needs the I/O port numbers for the CD-ROM.\n"
	"Retry the ports above or check Windows\n"
	"system information under I/O ports for\n"
	"'IDE', 'ATA' or 'SATA'.\n\n"
	"\nEnter 4-digit hex I/O Port number.\n"
	"CD-ROM I/O Port Base0: 0x";
    bdev->base0=SysPmtI64;
    bdev->base1=0;

    "\nUnit (0 or 1): ";
    do ch=GetChar(NULL,FALSE);
    while (!('0'<=ch<='1'));
    '' ch;
    bdev->unit=ch-'0';
    return TRUE;
  }
  return FALSE;
}

U0 BlkDevsInitAll()
{
  LTBlkDev *bdev;
  I64 i;
  num_blkdevs=0;
  local_blkdevs=ACAlloc(sizeof(LTBlkDev)*NUM_LTBLKDEVS);
  local_prts=ACAlloc(sizeof(LTPrt)*NUM_LTPRTS);
  for (i=0;i<NUM_LTPRTS;i++)
    drv_to_prt[i]=&local_prts[i];
  sys_raw_dc_col=0;
#exe {
   StreamPutS("#exe {OptOff(OPTf_PAREN_WARN);}");
   StreamLtf(osmain_cfg->add_dev);
   StreamPutS("#exe {OptOn(OPTf_PAREN_WARN);}");
};
  PrtRep;
}

