U8 key[4][256];

U0 InitKey()
{
  I64 i,j;
  for (i=0;i<4;i++) {
    for (j=0;j<256;j++)
      key[i][j]=j;
    for (j=0;j<255;j++)
      SwapU8(&key[i][j],&key[i][j+RandU32%(256-j)]);
  }
}

U0 LogScrambleFile(U8 *name)
{
  Ltf *l=LtfRead(name,LTFF_PLAIN_TEXT);
  LtfEntry *ll=l->root.next;
  I64 i;
  U32 num;
  U8 *st,*st2;
  "Scrambling:%s\n",name;
  while (ll!=l) {
    if (ll->type_u8==LTFT_TEXT) {
      st=MAlloc(StrLen(ll->tag)+1);
      st2=MAlloc(StrLen(ll->tag)+1);
      StrFirstRem(ll->tag," ",st);

      for (i=3;i>=0;i--) {
	StrFirstRem(st,".",st2);
	num.u8[i]=key[i][A2I(st2)&255];
      }
 
      Free(st);
      Free(st2);

      st=MSPrintF("%d.%d.%d.%d %s",num.u8[3],num.u8[2],num.u8[1],num.u8[0],ll->tag);
      Free(ll->tag);
      ll->tag=st;
    }
    ll=ll->next;
  }
  LtfWrite(l);
  LtfDel(l);
}

U0 WebLogScramble(U8 *wild)
{
  LTDirEntry *tempm=FilesFind(wild,FUF_RECURSE|FUF_JUST_FILES),
	     *tempm1;
  InitKey;
  while (tempm) {
    tempm1=tempm->next;
    LogScrambleFile(tempm->full_name);
    LTDirEntryDel(tempm);
    tempm=tempm1;
  }
}

//Danger scrambles all log files in the /LT tree

ST_WARN_ST "This will scramble all .log files in the /LT Tree.\n\n";
if (AreYouSure)
  WebLogScramble("/LT/*.log*");
