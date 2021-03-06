#help_index "Compiler"
DefineListLoad("ST_BIN_FILE_TYPES",
"REL_EXPORT\0"
"LIT_EXPORT\0"
"REL_U8\0"
"LIT_U8\0"
"REL_U16\0"
"LIT_U16\0"
"REL_U32\0"
"LIT_U32\0"
"REL_U64\0"
"LIT_U64\0"
"\0\0\0\0\0\0"
"MAIN\0"
"MODULE_ALIGN\0"
"\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
"ABS_ADDR\0"
"HEAP_GLBL\0"
);


public U0 BinRep(U8 *filename)
{//This reports imports, exports, etc
 //for a .BI? file.
//See $LK,"Loader Patch Table Generation","FF:::/LT/Compiler/CmpMain.CPZ,EIE_MODULE_ALIGNMENT"$ and $LK,"Load","MN:Load"$().
  U8 *fbuf;
  U8 *ptr,etype;
  U8 *sptr,*absname;
  U64 size;
  BinFileHeaderStruct *bfh,*bfh2;
  I64 i,j;
  ExportSysSymHashEntry *saved_hash_ptr=NULL;
  fbuf=DftExt(filename,"BIZ");
  if (!(bfh2=FileRead(fbuf,&size))) {
    Free(fbuf);
    return;
  }
  bfh=bfh2;

  if (ArcXSum(bfh,bfh->file_size)!=LT_XSUM) {
    "Bad XSum\n\n";
    return;
  }

  absname=FileNameAbs(fbuf);
  Free(fbuf);
  fbuf=StrNew(absname);
  FileExtRem(fbuf);
  if (fbuf[1]==':' && StrLen(fbuf)>2)
    "Module Name:%s\n",fbuf+2;
  else
    "No Module Name\n";
  Free(fbuf);

  ptr=bfh(U8 *)+bfh->loader_patch_table_offset;
  while ((etype=*ptr++)!=EIE_END) {
    i=*ptr(U32 *)++;
    sptr=ptr;
    ptr+=StrLen(sptr)+1;
    "%16Z:$$FG,GREEN$$%s$$FG$$:%X ",etype,"ST_BIN_FILE_TYPES",sptr,i;
    switch (etype) {
      case EIE_REL_U8...EIE_LIT_U64:
	if (*sptr)
	  saved_hash_ptr=HashFind(sptr,Fs->hash_table,HTT_ALL);
	if (!saved_hash_ptr)
	  "%S%s\n","ST_UNRESOLVED_REFERENCE",sptr;
	break;
      case EIE_ABS_ADDR:
	NewLine;
	for (j=0;j<i;j++)
	  "%X ",*ptr(U32 *)++;
	break;
      case EIE_HEAP_GLBL:
	"Size:%X ",*ptr(U64 *)++;
	for (j=0;j<i;j++)
	  "%X ",*ptr(U32 *)++;
	break;
    }
    NewLine;
  }
  Free(absname);
  Free(bfh);
}
#help_index ""
