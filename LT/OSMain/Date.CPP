//See $LK,"::/LT/Doc/TimeLTDate.TXZ","FI:::/LT/Doc/TimeLTDate.TXZ"$

U16 mon_start_days1[12]={
0,31,59,90,120,151,181,212,243,273,304,334};
U16 mon_start_days2[12]={
0,31,60,91,121,152,182,213,244,274,305,335};

I64 YearStartDate(I64 year)
{
  I64 y1=year-1;
  I64 yd4000=y1/4000,yd400=y1/400,yd100=y1/100,yd4=y1/4;
  return year*365+yd4-yd100+yd400-yd4000;
}

LTDate LTDateStruct2LTDate(LTDateStruct *d)
{
  LTDate ltdt;
  I64 i1,i2;
  i1=YearStartDate(d->year);
  i2=YearStartDate(d->year+1);
  if (i2-i1==365)
     i1+=mon_start_days1[d->mon-1];
   else
     i1+=mon_start_days2[d->mon-1];
  ltdt.date=i1+d->day_of_mon-1;
  ltdt.time=(d->sec10000+100*(d->sec100+100*(d->sec+60*(d->min+60*d->hour))))<<21/(15*15*3*625);
  return ltdt;
}

I64 DayOfWeek(I64 i)
{
  i+=LT_BASE_DAY_OF_WEEK;
  if (i>=0)
    return i % 7;
  else
    return 6-(6-i)%7;
}

U0 LTDate2LTDateStruct(LTDateStruct *d,LTDate ltdt)
{
  I64 i,date=ltdt.date;
  U64 k;
  d->day_of_week=DayOfWeek(date);
  d->year=(date+1)*100000/LT_YEAR_DAYS_INT;
  i=YearStartDate(d->year);
  while (i>date) {
    d->year--;
    i=YearStartDate(d->year);
  }
  date-=i;
  if (YearStartDate(d->year+1)-i==365) {
    k=0;
    while (date>=mon_start_days1[k+1] && k<11)
      k++;
    date-=mon_start_days1[k];
  } else {
    k=0;
    while (date>=mon_start_days2[k+1] && k<11)
      k++;
    date-=mon_start_days2[k];
  }
  d->mon=k+1;
  d->day_of_mon=date+1;
  k=(625*15*15*3*ltdt.time)>>21+1;
  d->sec10000=ModU64(&k,100);
  d->sec100=ModU64(&k,100);
  d->sec=ModU64(&k,60);
  d->min=ModU64(&k,60);
  d->hour  =k;
}

I64 FirstDayOfMon(I64 i)
{
  LTDateStruct d;
  LTDate ltdt=0;
  ltdt.date=i;
  LTDate2LTDateStruct(&d,ltdt);
  d.day_of_mon=1;
  ltdt=LTDateStruct2LTDate(&d);
  return ltdt.date;
}

I64 LastDayOfMon(I64 i)
{
  LTDateStruct d;
  LTDate ltdt=0;
  ltdt.date=i;
  LTDate2LTDateStruct(&d,ltdt);
  d.mon++;
  if (d.mon==13) {
    d.mon=0;
    d.year++;
  }
  d.day_of_mon=1;
  ltdt=LTDateStruct2LTDate(&d);
  return ltdt.date-1;
}

I64 FirstDayOfYear(I64 i)
{
  LTDateStruct d;
  LTDate ltdt=0;
  ltdt.date=i;
  LTDate2LTDateStruct(&d,ltdt);
  d.day_of_mon=1;
  d.mon=1;
  ltdt=LTDateStruct2LTDate(&d);
  return ltdt.date;
}

I64 LastDayOfYear(I64 i)
{
  LTDateStruct d;
  LTDate ltdt=0;
  ltdt.date=i;
  LTDate2LTDateStruct(&d,ltdt);
  d.day_of_mon=1;
  d.mon=1;
  d.year++;
  ltdt=LTDateStruct2LTDate(&d);
  return ltdt.date-1;
}


U64 Bcd2Bin(U64 b)
{
  U64 i,result=0;
  for (i=0;i<16;i++) {
    result=result*10+b>>60;
    b<<=4;
  }
  return result;
}


U0 GetCurTimeLTDateStruct(LTDateStruct *d)
{
  U64 i;
  U8 *b=d;
  BoolI8 is_bcd;

  MemSet(d,0,sizeof(LTDateStruct));
  PushFD;
  Cli;
  while (LBts(&sys_semas[SYS_SEMA_SYS_DATE],0))
    Pause;

  OutU8(0x70,0x0A);
  do {
    while (InU8(0x71) & 0x80)
      Pause;

    OutU8(0x70,0);
    b[2]=InU8(0x71);
    OutU8(0x70,2);
    b[3]=InU8(0x71);
    OutU8(0x70,4);
    b[4]=InU8(0x71);

    OutU8(0x70,6);
    b[5]=InU8(0x71);
    OutU8(0x70,7);
    b[6]=InU8(0x71);
    OutU8(0x70,8);
    b[7]=InU8(0x71);
    OutU8(0x70,9);
    b[8]=InU8(0x71);

    OutU8(0x70,0x0A);
  } while (InU8(0x71) & 0x80);

  OutU8(0x70,0x0B);
  if (InU8(0x71) & 4)
    is_bcd=FALSE;
  else
    is_bcd=TRUE;

  LBtr(&sys_semas[SYS_SEMA_SYS_DATE],0);
  PopFD;
  if (is_bcd)
    for (i=2;i<9;i++)
      b[i]=Bcd2Bin(b[i]);

  if (d->year>255) d->year=255;
  d->year+=2000;
  if (d->mon>12) d->mon=12;
  if (d->day_of_mon>31) d->day_of_mon=31;
  if (d->day_of_week>6) d->day_of_week=6;
  if (d->hour>23) d->hour=23;
  if (d->min>59) d->min=59;
  if (d->sec>59) d->sec=59;
}

LTDate GetCurTimeLTDate()
{
  LTDateStruct d;
  GetCurTimeLTDateStruct(&d);
  return LTDateStruct2LTDate(&d)-local_time_offset;
}
