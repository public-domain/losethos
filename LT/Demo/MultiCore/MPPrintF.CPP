U0 Job(Ltf *l)
{
  I64 i;
  for (i=0;i<10;i++)
    LtfPrintF(l,"$$FG,%d$$CPU",Gs->num);
}


U0 MPPrintFDemo()
{
  I64 i;
  for (i=0;i<1000;i++)
    MPJob(&Job,LtfCur);
  Sleep(1000);
  "$$FG$$\n";
}

MPPrintFDemo;
