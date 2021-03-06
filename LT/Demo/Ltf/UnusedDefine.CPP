/*
This scans the sym table and checks
each $LK,"HTT_DEFINE_STR","MN:HTT_DEFINE_STR"$ entry to see if
it only occurs once in files.

It's a brute force solution, but
gets the job done... slowly.

$LK,"Grep","MN:Grep"$() returns a count of matches.

$LK,"FileOcc","MN:FileOcc"$() is $LK,"Grep","MN:Grep"$() with
output $LK,"Silent","MN:Silent"$().

*/

U0 UnusedDefineScan()
{
  Ltf *old_ltf=LtfCur,*l;
  I64 cnt=0;
  SysHashTable *tempht;
  SysHashEntry *temph;
  LtfEntry *ll,*ll2;
  BoolI8 old_silent=IsSilent;

  //Technically, we might want to
  //turn-off preemption in case the
  //table was modified.

  try {
    tempht=Fs->hash_table;
    while (tempht) {
      temph=tempht->next_added;
      while (temph!=tempht) {
	if (temph->type&HTT_DEFINE_STR)
	  cnt++;
	temph=temph->next_added;
      }
      tempht=tempht->next;
    }

    progress1=0;
    progress1_max=cnt;
    StrCpy(progress1_desc,"Define Scan");

    tempht=Fs->hash_table;
    while (tempht) {
      temph=tempht->next_added;
      while (temph!=tempht) {
	if (temph->type&HTT_DEFINE_STR) {
	  progress1++;
	  if (FileOcc(temph->str,"/LT/"TEXT_FILE_MASK,"+l-i")==1) {
	    l=LtfNew;
	    LtfRemQue(old_ltf);
	    LtfInsQue(l,Fs->last_ltf);
	    Grep(temph->str,"/LT/"TEXT_FILE_MASK,"+l-i");
	    LtfRemQue(l);
	    LtfInsQue(old_ltf,Fs->last_ltf);
	    ll=l->root.next;
	    while (ll!=l) {
	      if (ll->type_u8==LTFT_LINK) {
		NewLine;
		"" temph->str;
		ll2=LtfEntryCopy(l,ll);
		LtfInsert(old_ltf,ll2);
		NewLine;
	      }
	      ll=ll->next;
	    }
	    LtfDel(l);
	  }
	}
	temph=temph->next_added;
      }
      tempht=tempht->next;
    }
  } catch
    CatchAll;

  Silent(old_silent);
  NewLine;
  ProgressBarsRst;
}

UnusedDefineScan;
