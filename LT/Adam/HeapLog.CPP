#help_index "Debugging"

class HeapLogStruct
{
  HeapLogStruct *next,*last;
  U8 *addr;
  U8 *caller1;
  U8 *caller2;
  U8 *caller3;
  U8 *caller4;
};

class HeapLogHashStruct
{
  HeapLogStruct *next,*last;
};

#define HEAPLOG_HASH_SIZE	0x1000

TaskStruct *heaplog_task=NULL;
HeapCtrl *heaplog_hc;
HeapLogHashStruct *heaplog_root=NULL;


U0 HeapLogTask(U64 dummy=0)
{
  nounusedwarn dummy;
  while (TRUE)
    Yield;
}

U0 HeapLogMAlloc(U8 *addr)
{
  HeapLogStruct *temph;
  U64 i;
  if (MHeapCtrl(addr)==heaplog_hc) {
    temph=MAlloc(sizeof(HeapLogStruct),heaplog_task);
    temph->addr=addr;
    temph->caller1=Caller(2);
    temph->caller2=Caller(3);
    temph->caller3=Caller(4);
    temph->caller4=Caller(5);
    i=addr>>3 &(HEAPLOG_HASH_SIZE-1);
    PushFD;
    Cli;
    while (LBts(&sys_semas[SYS_SEMA_HEAPLOG_LOCK],0))
      Pause;
    InsQue(temph,heaplog_root[i].last);
    LBtr(&sys_semas[SYS_SEMA_HEAPLOG_LOCK],0);
    PopFD;
  }
}

U0 HeapLogFree(U8 *addr)
{
  U64 i;
  HeapLogStruct *temph;
  if (!addr) return;
  if (MHeapCtrl(addr)==heaplog_hc) {
    i=addr>>3 &(HEAPLOG_HASH_SIZE-1);
    PushFD;
    Cli;
    while (LBts(&sys_semas[SYS_SEMA_HEAPLOG_LOCK],0))
      Pause;
    temph=heaplog_root[i].next;
    while (temph!=&heaplog_root[i]) {
      if (addr==temph->addr) {
	RemQue(temph);
	LBtr(&sys_semas[SYS_SEMA_HEAPLOG_LOCK],0);
	PopFD;
	Free(temph);
	return;
      }
      temph=temph->next;
    }
    LBtr(&sys_semas[SYS_SEMA_HEAPLOG_LOCK],0);
    PopFD;
  }
}

public U0 HeapLogRep()
{
  U64 i,total=0;
  HeapLogStruct *temph;
  if (!LBtr(&sys_semas[SYS_SEMA_HEAPLOG_ACTIVE],0)) {
    "HeapLog Not Active\n";
    return;
  }
  "$$WW,0$$";

  PushFD;
  Cli;
  while (LBts(&sys_semas[SYS_SEMA_HEAPLOG_LOCK],0))
    Pause;
  for (i=0;i<HEAPLOG_HASH_SIZE;i++) {
    temph=heaplog_root[i].next;
    while (temph!=&heaplog_root[i]) {
      "$$FG,PURPLE$$%08X$$FG$$ %08X ",MSize(temph->addr),temph->addr;
      if (!PutSrcLink(temph->caller1,64))
	"%P",temph->caller1;
      '' CH_SPACE;
      if (!PutSrcLink(temph->caller2,64))
	"%P",temph->caller2;
      '' CH_SPACE;
      if (!PutSrcLink(temph->caller3,64))
	"%P",temph->caller3;
      '' CH_SPACE;
      if (!PutSrcLink(temph->caller4,64))
	"%P",temph->caller4;
      NewLine;

      total+=MSize(temph->addr);
      temph=temph->next;
    }
  }
  LBtr(&sys_semas[SYS_SEMA_HEAPLOG_LOCK],0);
  PopFD;
  "\n$$FG,LTRED$$Total:%08X$$FG$$\n",total;
  LBts(&sys_semas[SYS_SEMA_HEAPLOG_ACTIVE],0);
}

public U0 HeapLog(BoolI8 val=TRUE,TaskStruct *task=NULL)
{
  U64 i;
  if (val) {
    if (Bt(&sys_semas[SYS_SEMA_HEAPLOG_ACTIVE],0))
      "HeapLog Already Active\n";
    else {
      if (!task) task=Fs;
      if (TaskValidate(task))
	heaplog_hc=task->data_heap;
      else
	heaplog_hc=task;
      PushFD;
      Cli;
      while (LBts(&sys_semas[SYS_SEMA_HEAPLOG_LOCK],0))
	Pause;
      heaplog_task=Spawn(&HeapLogTask);
      ext[EXT_HEAPLOG_MALLOC]=&HeapLogMAlloc;
      ext[EXT_HEAPLOG_FREE]=&HeapLogFree;
      heaplog_root=MAlloc(sizeof(HeapLogHashStruct)*HEAPLOG_HASH_SIZE,heaplog_task);
      for (i=0;i<HEAPLOG_HASH_SIZE;i++)
	heaplog_root[i].next=heaplog_root[i].last=&heaplog_root[i];
      LBtr(&sys_semas[SYS_SEMA_HEAPLOG_LOCK],0);
      PopFD;
      LBts(&sys_semas[SYS_SEMA_HEAPLOG_ACTIVE],0);
    }
  } else {
    if (!LBtr(&sys_semas[SYS_SEMA_HEAPLOG_ACTIVE],0))
      "HeapLog Not Active\n";
    else {
      Kill(heaplog_task);
      heaplog_root=heaplog_task=NULL;
      ext[EXT_HEAPLOG_MALLOC]=NULL;
      ext[EXT_HEAPLOG_FREE]=NULL;
    }
  }
}

#help_index ""
