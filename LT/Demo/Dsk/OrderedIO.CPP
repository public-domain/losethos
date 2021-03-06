/*
$WW,1$This gets a list of files, flattens the list from a tree into an array and sorts them by cluster num, to speed-up access time by avoiding moving the disk head around.
$WW,0$*/

U0 Main()
{
  I64 i,cnt=0;
  LTDirEntry *tempm=FilesFind("::/LT/*",1<<FUf_RECURSE),
      **a=FilesOrder(tempm,&cnt);
  for (i=0;i<cnt;i++) {
    "%08X %s\n",a[i]->cluster,a[i]->full_name;
    LTDirEntryDel(a[i]);
  }
  Free(a);
}

Main;
