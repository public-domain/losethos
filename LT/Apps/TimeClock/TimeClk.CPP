#define TIME_FILENAME	"HOME/TimeClock/TimeFile.DTZ"

#define TET_EOF		0
#define TET_PUNCH_IN	1
#define TET_PUNCH_OUT	2

class TimeEntry
{
  TimeEntry *next,*last;
  U8 type;
  LTDate d;
  U8 *desc;
};


U0 TimeFileRead(TimeEntry *header)
{
  U8 *buf,*ptr,type;
  TimeEntry *tempt;
  buf=FileRead(TIME_FILENAME);
  if (!buf)
    buf=CAlloc(1);
  header->next=header->last=header;
  ptr=buf;
  while (type=*ptr++) {
    tempt=CAlloc(sizeof(TimeEntry));
    tempt->type=type;
    tempt->d=*ptr(LTDate *)++;
    tempt->desc=StrNew(ptr);
    ptr+=StrLen(ptr)+1;
    InsQue(tempt,header->last);
  }
  Free(buf);
}

U0 TimeFileWrite(TimeEntry *header)
{
  U8 *buf,*ptr;
  TimeEntry *tempt;
  U64 size=1; //for eof

  tempt=header->next;
  while (tempt!=header) {
    size+=sizeof(U8)+sizeof(LTDate)+StrLen(tempt->desc)+1;
    tempt=tempt->next;
  }
  buf=MAlloc(size);

  ptr=buf;
  tempt=header->next;
  while (tempt!=header) {
    *ptr++=tempt->type;
    *ptr(LTDate *)++=tempt->d;
    StrCpy(ptr,tempt->desc);
    ptr+=StrLen(tempt->desc)+1;
    tempt=tempt->next;
  }
  *ptr=TET_EOF;
  FileWrite(TIME_FILENAME,buf,size);
  Free(buf);
}

U0 TimeEntriesDel(TimeEntry *header)
{
  TimeEntry *tempt=header->next,*tempt1;
  while (tempt!=header) {
    tempt1=tempt->next;
    Free(tempt->desc);
    Free(tempt);
    tempt=tempt1;
  }
  Free(header);
}

public BoolI64 TimeRep(TimeEntry **header=NULL)
{
  BoolI8 is_in=FALSE,first=TRUE;
  TimeEntry *tempt;
  U64 week,cur_week=-1,week_total;
  TimeEntry *my_header=MAlloc(sizeof(TimeEntry));
  if (header)
    *header=my_header;
  TimeFileRead(my_header);
  tempt=my_header->next;
  while (tempt!=my_header) {
    week=tempt->d.date/7;  //TODO
    if (week!=cur_week) {
      if (!first) {
	if (is_in)
	  week_total+=GetCurTimeLTDate;
	"Week Total:%h7T\n",week_total;
      } else
	first=FALSE;
      cur_week=week;
      week_total=0;
    }
    if (tempt->type==TET_PUNCH_IN) {
      "$$FG,RED$$IN ";
      if (!is_in)
	week_total-=tempt->d;
      is_in=TRUE;
    } else {
      "$$FG,RED$$OUT";
      if (is_in)
	week_total+=tempt->d;
      is_in=FALSE;
    }
    " %h2D %T:$$FG$$\n%s\n",tempt->d,tempt->d,tempt->desc;
    tempt=tempt->next;
  }
  if (is_in)
    week_total+=GetCurTimeLTDate;
  "$$FG,RED$$Week Total:%h7T$$FG$$\n",week_total;
  if (!header)
    TimeEntriesDel(my_header);
  return is_in;
}

public U0 PunchOut()
{
  BoolI8 is_in;
  TimeEntry *tempt,*header;
  is_in=TimeRep(&header);
  if (!is_in)
    "$$BK,1$$Already Punched-Out$$BK,0$$\n";
  else {
    tempt=MAlloc(sizeof(TimeEntry));
    tempt->type=TET_PUNCH_OUT;
    tempt->d=GetCurTimeLTDate;
    "\nEnter Description.\nPress <ESC> when done.\n";
    if (!(tempt->desc=MGetS(GETSF_WITH_NEW_LINE)))
      tempt->desc=CAlloc(1);
    InsQue(tempt,header->last);
    TimeFileWrite(header);
  }
  TimeEntriesDel(header);
}

public U0 PunchIn()
{
  BoolI8 is_in;
  TimeEntry *tempt,*header;
  is_in=TimeRep(&header);
  if (is_in)
    "$$BK,1$$Already Punched-In$$BK,0$$\n";
  else {
    tempt=MAlloc(sizeof(TimeEntry));
    tempt->type=TET_PUNCH_IN;
    tempt->d=GetCurTimeLTDate;
    "\nEnter Description.\nPress <ESC> when done.\n";
    if (!(tempt->desc=MGetS(GETSF_WITH_NEW_LINE)))
      tempt->desc=CAlloc(1);
    InsQue(tempt,header->last);
    TimeFileWrite(header);
  }
  TimeEntriesDel(header);
}
