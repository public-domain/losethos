U8 char_map[26]="22233344455566677778889999";


/*
Fmt of word list entry:
  U8 WSD_WORD_CHAR
  U8 word[] with terminating zero
  I16 block; //definition offset in ::/LT/Adam/WordStat/WsDefs.DTA
*/
U0 PhoneNumWords(U8 *num)
{
  U8 ch,*_num=StrNew(num),*dst=_num,
      *dict=ws_dict_word_list,*st,*src;
  while (*num) {
    if ('0'<=*num<='9')
      *dst++=*num;
    num++;
  }
  *dst=0;

  while (*dict++) {
    if (StrLen(dict)>2) {
      dst=st=StrNew(dict);
      while (ch=ToUpper(*dst)) {
	if ('A'<=ch<='Z')
	  *dst=char_map[ch-'A'];
	else
	  *dst=CH_SPACE;
	dst++;
      }
      *dst=0;

      src=_num;
      while (*src) {
	if (!StrNCmp(st,src,StrLen(st))) {
	  ch=*src;
	  *src=0;
	  "" _num;
	  *src=ch;
	  "$$FG,GREEN$$%s$$FG$$",dict;
	  "" src+StrLen(st);
	  NewLine;
	}
	src++;
      }
      Free(st);
    }
    dict+=StrLen(dict)+3;
  }
  Free(_num);
  NewLine;
}

U0 PNWDemo()
{
  U8 *st;
  while ((st=PmtStr("Phone Num:")) && *st) {
    PhoneNumWords(st);
    Free(st);
  }
  Free(st);
}

PNWDemo;
