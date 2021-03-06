U0 CvtBMPFile(U8 *fname)
{
  GrDC *dc=BMPFileRead(fname,TRUE);
  GrSprite *elems=GrDC2Sprite(dc);
  NewLine(TEXT_ROWS/2);
  PutSprite(elems);
  NewLine(TEXT_ROWS/2);
  Free(elems);
  GrDCDel(dc);
}

public U0 CvtBMPDir()
{
  U8 *dname=PopUpPickDir,*mask,*st;
  LTDirEntry *tempm,*tempm1;

  mask=MSPrintF("%s/*",dname);
  Free(dname);

  tempm=tempm1=FilesFind(mask,FUF_FLATTEN_TREE+FUF_JUST_FILES);
  Free(mask);

  LtfClear;
  while (tempm) {
    st=MStrUtil(tempm->name,SUF_TO_UPPER);
    if (WildCardMatch("*.BM?",st))
      CvtBMPFile(tempm->full_name);
    Free(st);
    tempm=tempm->next;
  }

  LTDirListDel(tempm1);
}

/*
Save your files in Windows as 16-color BMP.
Then, convert them to GrSprite with this
program.

Cut and paste the converted bmp files
after running this program and selecting
a directory.
*/

CvtBMPDir;
