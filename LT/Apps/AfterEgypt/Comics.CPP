U0 ViewComics()
{
  I64 i=0;
  U8 *st;
  LTDirEntry *tempm,*tempm1;
  Ltf *l=LtfNew;
  tempm=FilesFind("Comics/*.TXZ");
  tempm1=tempm;
  while (tempm1) {
    if (!(i++&3))
      LtfPutS(l,"\n");
    st=StrNew(tempm1->name);
    FileExtRem(st);
    tempm1->user_data=LtfPrintF(l,"$$MU-UL,\"%-10ts\",%d$$ ",st,tempm1);
    Free(st);
    tempm1=tempm1->next;
  }
  while (TRUE) {
    if ((tempm1=PopUpMenu(l))>0)
      Ed(tempm1->full_name);
    else
      break;
  }
  LtfDel(l);
  LTDirListDel(tempm);
}

