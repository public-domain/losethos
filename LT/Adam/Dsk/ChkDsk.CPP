#help_index "Info;File/Cmd Line (Typically);Cmd Line (Typically)"

BoolI64 ChkDskConfirm(BoolI8 *_fix,BoolI8 *_confirm)
{
  if (*_fix && *_confirm) {
    "Fix ";
    if (!YorN)
      *_fix=FALSE;
    *_confirm=FALSE;
  }
  return *_fix;
}

I64 LTFSChkDskList(LTDirEntry *tempm1,U8 *bits,U8 *bits2,U64 size,U64 bpc)
{
  LTDirEntry *tempm2;
  U64 i,j;
  I64 errs=0;
  while (tempm1) {
    tempm2=tempm1->next;
    if (tempm1->attr & LT_ATTR_DIR) {
      if (tempm1->sub)
	errs+=LTFSChkDskList(tempm1->sub,bits,bits2,size,bpc);
    }
    j=(tempm1->size+bpc-1)/bpc;
    for (i=0;i<j;i++) {
      if (i+tempm1->cluster>size) {
	PrintFErr("Invalid Cluster:%s Cluster:%X\n",tempm1->full_name,
	       i+tempm1->cluster);
	errs++;
	break;
      }
      if (LBts(bits,i+tempm1->cluster)) {
	PrintFErr("Dbl Alloc:%s Cluster:%X\n",tempm1->full_name,
	       i+tempm1->cluster);
	errs++;
      }
      if (!LBtr(bits2,i+tempm1->cluster)) {
	PrintFErr("UnAlloc:%s Cluster:%X\n",tempm1->full_name,
	       i+tempm1->cluster);
	errs++;
      }
    }
    LTDirEntryDel(tempm1);
    tempm1=tempm2;
  }
  return errs;
}

I64 LTFSChkDsk(U8 drv_let,BoolI8 *_fix,BoolI8 *_confirm)
{
  I64 i,j,bpc,size,errs=0;
  LTPrt *p=Drv2Prt(drv_let),*old_prt=Fs->cur_prt;
  U8 *files_find_mask=MSPrintF("%c:/*",Prt2Drv(p)),
     *old_dir=StrNew(Fs->cur_dir);
  U8 *bits,*bits2;
  LTDirEntry *ptr,*ptr2;

  Drv(drv_let);
  "Scanning...\n";
  size=(p->size-(p->data-p->offset))/p->spc;
  bpc=p->spc<<BLK_SIZE_BITS;
  bits=CAlloc((size+7)>>3);
  bits2=CAlloc((size+7)>>3+BLK_SIZE);
  RBlks(p,bits2,p->map1,((size+7)>>3+BLK_SIZE-1)>>BLK_SIZE_BITS);

  //Get Root Dir size
  ptr2=MAlloc(bpc);
  RClusters(p,ptr2,p->root_cluster,1);
  ptr=ptr2(U8 *)-offset(LTDirEntry.start);
  j=(ptr->size+bpc-1)/bpc;
  Free(ptr2);

  for (i=0;i<j;i++) {
    if (i+p->root_cluster>size) {
      PrintFErr("Invalid Cluster: RootDir Cluster:%X\n",i+p->root_cluster);
      errs++;
      break;
    }
    if (LBts(bits,i+p->root_cluster)) {
      PrintFErr("Dbl Alloc: RootDir Cluster:%X\n",i+p->root_cluster);
      errs++;
    }
    if (!LBtr(bits2,i+p->root_cluster)) {
      PrintFErr("UnAlloc: RootDir Cluster:%X\n",i+p->root_cluster);
      errs++;
    }
  }

  errs+=LTFSChkDskList(FilesFind(files_find_mask,1<<FUf_RECURSE),bits,bits2,size,bpc);
  for (i=1;i<size;i++)
    if (Bt(bits2,i)) {
      PrintFWarn("Shouldn't Alloc Cluster:%0X\n",i);
      errs++;
      if (ChkDskConfirm(_fix,_confirm))
	LTFSFreeClusters(p,i,1);
    }

  Free(files_find_mask);
  Free(bits);
  Free(bits2);
  Drv(Prt2Drv(old_prt));
  Cd(old_dir);
  Free(old_dir);
  return errs;
}

I64 FAT32ChkDskList(LTPrt *p,LTDirEntry *tempm1,U8 *bits,U32 *bits2,U64 size,U64 bpc)
{
  LTDirEntry *tempm2;
  I64 i,c,errs=0;
  while (tempm1) {
    tempm2=tempm1->next;
    if (tempm1->attr & LT_ATTR_DIR) {
      if (tempm1->sub)
	errs+=FAT32ChkDskList(p,tempm1->sub,bits,bits2,size,bpc);
    }
    i=0;
    c=tempm1->cluster;
    while (c && c!=INVALID_CLUSTER) {
      if (c>size) {
	PrintFErr("Invalid Cluster:%s Cluster:%X\n",tempm1->full_name,c);
	errs++;
	break;
      }
      if (LBts(bits,c)) {
	PrintFErr("Dbl Alloc:%s Cluster:%X\n",tempm1->full_name,c);
	errs++;
      }
      if (!bits2[c]) {
	PrintFErr("UnAlloc:%s Cluster:%X\n",tempm1->full_name,c);
	errs++;
      } else
	bits2[c]=0;
      c=ClusterNumNext(p,c);
      i++;
    }
    if (!(tempm1->attr & LT_ATTR_DIR)) {
      i*=bpc;
      if (tempm1->size>i) {
	PrintFErr("Allocated File Too Short:%s\n",tempm1->full_name);
	errs++;
      }
      if (i>tempm1->size+bpc-1) {
	PrintFWarn("Allocated File Too Long:%s\n",tempm1->full_name);
	errs++;
      }
    }
    LTDirEntryDel(tempm1);
    tempm1=tempm2;
  }
  return errs;
}

I64 FAT32ChkDsk(U8 drv_let,BoolI8 *_fix,BoolI8 *_confirm)
{
  U64 i,bpc,size;
  LTPrt *p=Drv2Prt(drv_let),*old_prt=Fs->cur_prt;
  U8 *files_find_mask=MSPrintF("%c:/*",Prt2Drv(p)),
     *old_dir=StrNew(Fs->cur_dir);
  U8 *bits;
  U32 *bits2;
  I64 c,errs=0;
  Drv(drv_let);
  "Scanning...\n";
  size=(p->size-(p->data-p->offset))/p->spc;
  bpc=p->spc<<BLK_SIZE_BITS;
  bits=CAlloc((size+7)>>3);
  bits2=CAlloc(size*4+BLK_SIZE);
  RBlks(p,bits2,p->map1,(size*4+BLK_SIZE-1)>>BLK_SIZE_BITS);

  c=p->root_cluster;
  while (c!=INVALID_CLUSTER) {
    if (c>size) {
      PrintFErr("Invalid Cluster: RootDir Cluster:%X\n",c);
      errs++;
      break;
    }
    if (LBts(bits,c)) {
      PrintFErr("Dbl Alloc: RootDir Cluster:%X\n",c);
      errs++;
    }
    if (!bits2[c]) {
      PrintFErr("UnAlloc: RootDir Cluster:%X\n",c);
      errs++;
    } else
      bits2[c]=0;
    c=ClusterNumNext(p,c);
  }

  errs+=FAT32ChkDskList(p,FilesFind(files_find_mask,1<<FUf_RECURSE),bits,bits2,size,bpc);

  bits2[1]=0; //See $LK,"FAT32Fmt","MN:FAT32Fmt"$()
  for (i=1;i<size;i++)
    if (bits2[i]) {
      PrintFWarn("Shouldn't Alloc Cluster:%0X\n",i);
      errs++;
      if (ChkDskConfirm(_fix,_confirm))
	FAT32FreeClusters(p,i);
    }
  Free(files_find_mask);
  Free(bits);
  Free(bits2);
  Drv(Prt2Drv(old_prt));
  Cd(old_dir);
  Free(old_dir);
  return errs;
}

public I64 ChkDsk(U8 drv_let=0,BoolI8 fix=FALSE,BoolI8 confirm=TRUE)
{ //Check for allocation errors.
  I64 errs=0;
  LTPrt *p=Drv2Prt(drv_let);
  switch (p->type) {
    case PT_LT:
      errs=LTFSChkDsk(drv_let,&fix,&confirm);
      break;
    case PT_FAT32:
      errs=FAT32ChkDsk(drv_let,&fix,&confirm);
      break;
    default:
      PutDefineErr("ST_UNSUPPORTED_FILE_SYSTEM");
  }
  if (errs) {
    if (fix)
      "It might be a little better.";
    "Copy files to another partition or CD-ROM, "
    "reformat, and copy back."
    "Or, copy from a back-up.\n";
  }
  return errs;
}

#help_index ""
