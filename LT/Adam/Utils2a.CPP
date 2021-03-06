#help_index "Memory"
U0 StkOverflow(TaskStruct *task)
{
  Dbg("Stk Overflow",task);
}

U0 MemSysCorrupt()
{
  Dbg("Corrupt Sys Memory");
}

U0 MemTaskCorrupt()
{
  Dbg("Corrupt Task Memory");
}

public U64 UnusedBPMem(BlkPool *bp)
{
  U64 result=0,i;
  MemBlk *tempm;

  if (!bp) return 0;

  PushFD;
  Cli;
  while (LBts(&bp->locked_flags,BPlf_LOCKED))
    Pause;

  tempm=bp->mem_free_list;
  while (tempm) {
    if (tempm->mb_signature!=MBS_UNUSED_SIGNATURE_VAL)
      MemSysCorrupt;
    result+=tempm->pages;
    tempm=tempm->next;
  }
  for (i=0;i<FREE_PAGE_HASH_SIZE;i++) {
    tempm=bp->free_page_hash[i];
    while (tempm) {
      if (tempm->mb_signature!=MBS_UNUSED_SIGNATURE_VAL)
	MemSysCorrupt;
      result+=tempm->pages;
      tempm=tempm->next;
    }
  }

  for (i=0;i<64-PAGE_BITS;i++) {
    tempm=bp->free_page_hash2[i];
    while (tempm) {
      if (tempm->mb_signature!=MBS_UNUSED_SIGNATURE_VAL)
	MemSysCorrupt;
      result+=tempm->pages;
      tempm=tempm->next;
    }
  }

  LBtr(&bp->locked_flags,BPlf_LOCKED);
  PopFD;
  return result*PAGE_SIZE;
}

public U64 UnusedSysMem()
{
  return UnusedBPMem(sys_code_bp);
}

public U64 TaskAllocatedMem(TaskStruct *task=NULL,BoolI8 override_validate=FALSE)
{
  MemBlk *m,*m1;
  U64 result=0;

  if (!task) task=Fs;
  if (!override_validate && !TaskValidate(task)) return 0;

  PushFD;
  Cli;
  while (LBts(&task->code_heap->locked_flags,HClf_LOCKED))
    Pause;
  if (task->code_heap!=task->data_heap)
    while (LBts(&task->data_heap->locked_flags,HClf_LOCKED))
      Pause;

  m=task->code_heap->next_mem_blk;
  m1=&task->code_heap->next_mem_blk;
  while (m!=m1) {
    if (m->mb_signature!=MBS_USED_SIGNATURE_VAL)
      MemTaskCorrupt;
    result+=m->pages*PAGE_SIZE;
    m=m->next;
  }

  if (task->code_heap!=task->data_heap) {
    m=task->data_heap->next_mem_blk;
    m1=&task->data_heap->next_mem_blk;
    while (m!=m1) {
      if (m->mb_signature!=MBS_USED_SIGNATURE_VAL)
	MemTaskCorrupt;
      result+=m->pages*PAGE_SIZE;
      m=m->next;
    }
  }

  if (task->code_heap!=task->data_heap)
    LBtr(&task->data_heap->locked_flags,HClf_LOCKED);
  LBtr(&task->code_heap->locked_flags,HClf_LOCKED);
  PopFD;

  return result;
}

U64 MemBlkListSize(HeapCtrl *hc)
{
  U64 result=0;
  MemBlk *m;
  m=hc->next_mem_blk;
  while (m!=&hc->next_mem_blk) {
    result+=sizeof(MemBlk);
    m=m->next;
  }
  return result;
}

public U64 TaskUnusedAllocatedMem(TaskStruct *task=NULL,BoolI8 override_validate=FALSE)
{
  U64 result=0,i;
  UnusedAllocatedMem *uum;
  if (!task) task=Fs;
  if (!override_validate && !TaskValidate(task)) return 0;

  PushFD;
  Cli;
  while (LBts(&task->code_heap->locked_flags,HClf_LOCKED))
    Pause;
  if (task->data_heap!=task->code_heap)
    while (LBts(&task->data_heap->locked_flags,HClf_LOCKED))
      Pause;

  result+=MemBlkListSize(task->code_heap);
  for (i=0;i<HEAP_HASH_SIZE>>3;i++) {
    uum=task->code_heap->heap_hash[i];
    while (uum) {
      result+=uum->size;
      uum=uum->next;
    }
  }
  uum=task->code_heap->malloc_free_list;
  while (uum) {
    result+=uum->size;
    uum=uum->next;
  }

  if (task->data_heap!=task->code_heap) {
    result+=MemBlkListSize(task->data_heap);
    for (i=0;i<HEAP_HASH_SIZE>>3;i++) {
      uum=task->data_heap->heap_hash[i];
      while (uum) {
	result+=uum->size;
	uum=uum->next;
      }
    }
    uum=task->data_heap->malloc_free_list;
    while (uum) {
      result+=uum->size;
      uum=uum->next;
    }
  }

  if (task->data_heap!=task->code_heap)
    LBtr(&task->data_heap->locked_flags,HClf_LOCKED);
  LBtr(&task->code_heap->locked_flags,HClf_LOCKED);
  PopFD;

  return result;
}

#help_index "Memory/Heap Ctrl"

public U64 UnusedHCMem(HeapCtrl *hc)
{
  U64 result,i;
  UnusedAllocatedMem *u;

  if (!hc) return 0;

  result=UnusedBPMem(hc->bp);

  PushFD;
  Cli;
  while (LBts(&hc->locked_flags,HClf_LOCKED))
    Pause;

  for (i=0;i<HEAP_HASH_SIZE>>3;i++) {
    u=hc->heap_hash[i];
    while (u) {
      result+=u->size;
      u=u->next;
    }
  }
  u=hc->malloc_free_list;
  while (u) {
    result+=u->size;
    u=u->next;
  }

  LBtr(&hc->locked_flags,HClf_LOCKED);
  PopFD;

  return result;
}

#help_index "Info;Memory/Info;Debugging"
public BoolI64 HeapRep(TaskStruct *task)
{
  I64 i,cnt;
  UnusedAllocatedMem *uum;

  if (!task || task==Fs) {
    "Task can't HeapRep on self.\n";
    return FALSE;
  }
  if (!TaskValidate(task)) return FALSE;

  PushFD;
  Cli;
  while (LBts(&task->code_heap->locked_flags,HClf_LOCKED))
    Pause;
  if (task->data_heap!=task->code_heap)
    while (LBts(&task->data_heap->locked_flags,HClf_LOCKED))
      Pause;

  for (i=0;i<HEAP_HASH_SIZE>>3;i++) {
    cnt=0;
    uum=task->code_heap->heap_hash[i];
    while (uum) {
      cnt+=uum->size;
      uum=uum->next;
    }
    if (task->data_heap!=task->code_heap) {
      uum=task->data_heap->heap_hash[i];
      while (uum) {
	cnt+=uum->size;
	uum=uum->next;
      }
    }
    if (cnt)
      "%03X:%08X\n",i<<3,cnt;
  }
  NewLine;

  uum=task->code_heap->malloc_free_list;
  while (uum) {
    "%X, ",uum->size;
    uum=uum->next;
  }
  if (task->data_heap!=task->code_heap) {
    uum=task->data_heap->malloc_free_list;
    while (uum) {
      "%X, ",uum->size;
      uum=uum->next;
    }
  }

  if (task->data_heap!=task->code_heap)
    LBtr(&task->data_heap->locked_flags,HClf_LOCKED);
  LBtr(&task->code_heap->locked_flags,HClf_LOCKED);
  PopFD;

  NewLine;
}

#help_index "Memory;Debugging"

public BoolI64 IsInHeap(U8 *a,TaskStruct *task=NULL,BoolI8 lock=TRUE)
{
  MemBlk *m;

  if (!task) task=Fs;
  if (!TaskValidate(task)) return FALSE;

  PushFD;
  Cli;
  if (lock) {
    while (LBts(&task->code_heap->locked_flags,HClf_LOCKED))
      Pause;
    if (task->data_heap!=task->code_heap)
      while (LBts(&task->data_heap->locked_flags,HClf_LOCKED))
	Pause;
  }

  m=task->code_heap->next_mem_blk;
  while (m!=&task->code_heap->next_mem_blk) {
    if (a>=m && a<m(U8 *)+m->pages*PAGE_SIZE) {
      if (lock)
	LBtr(&task->code_heap->locked_flags,HClf_LOCKED);
      PopFD;
      return TRUE;
    }
    m=m->next;
  }
  if (task->data_heap!=task->code_heap) {
    m=task->data_heap->next_mem_blk;
    while (m!=&task->code_heap->next_mem_blk) {
      if (a>=m && a<m(U8 *)+m->pages*PAGE_SIZE) {
	if (lock)
	  LBtr(&task->data_heap->locked_flags,HClf_LOCKED);
	PopFD;
	return TRUE;
      }
      m=m->next;
    }
  }

  if (lock) {
    LBtr(&task->code_heap->locked_flags,HClf_LOCKED);
    if (task->data_heap!=task->code_heap)
      LBtr(&task->data_heap->locked_flags,HClf_LOCKED);
  }
  PopFD;

  return FALSE;
}

public BoolI64 HeapWalk(TaskStruct *task=NULL)
{
  I64 i;
  UnusedAllocatedMem *uum;
  BoolI8 result=TRUE;

  if (!task) task=Fs;
  if (!TaskValidate(task)) return FALSE;

  PushFD;
  Cli;
  while (LBts(&task->code_heap->locked_flags,HClf_LOCKED))
    Pause;
  if (task->data_heap!=task->code_heap)
    while (LBts(&task->data_heap->locked_flags,HClf_LOCKED))
      Pause;

  for (i=0;i<HEAP_HASH_SIZE>>3;i++) {
    uum=task->code_heap->heap_hash[i];
    while (uum) {
      if (!IsInHeap(uum,task,FALSE))
	result=FALSE;
      uum=uum->next;
    }
    if (task->data_heap!=task->code_heap) {
      uum=task->data_heap->heap_hash[i];
      while (uum) {
	if (!IsInHeap(uum,task,FALSE))
	  result=FALSE;
	uum=uum->next;
      }
    }
  }

  uum=task->code_heap->malloc_free_list;
  while (uum) {
    if (!IsInHeap(uum,task,FALSE))
      result=FALSE;
    uum=uum->next;
  }

  if (task->data_heap!=task->code_heap) {
    uum=task->data_heap->malloc_free_list;
    while (uum) {
      if (!IsInHeap(uum,task,FALSE))
	result=FALSE;
      uum=uum->next;
    }
  }

#if _CFG_HEAP_DBG
  UsedAllocatedMem *um,*um1;
  um1=(&task->code_heap->next_um)(U8 *)-offset(UsedAllocatedMem.next);
  um=um1->next;
  while (um!=um1) {
    if (!IsInHeap(um,task,FALSE))
      result=FALSE;
    um=um->next;
  }
  if (task->data_heap!=task->code_heap) {
    um1=(&task->data_heap->next_um)(U8 *)-offset(UsedAllocatedMem.next);
    um=um1->next;
    while (um!=um1) {
      if (!IsInHeap(um,task,FALSE))
	result=FALSE;
      um=um->next;
    }
  }
#endif

  if (task->data_heap!=task->code_heap)
    LBtr(&task->data_heap->locked_flags,HClf_LOCKED);
  LBtr(&task->code_heap->locked_flags,HClf_LOCKED);
  PopFD;

  return result;
}

#help_index "Info;Task"

U0 TaskRepTask(TaskStruct *task,I64 indent)
{
  TaskStruct *task1;
  U8 *st,*desc=MSPrintF("%q",task->task_title);
  st=MSPrintF("$$MA+A,\"%08X\",\"Kill(0x%X);\n\",$$",task,task,task);
  Spaces(indent);
  "%s $$TX+SCX,\"%Q...\",16$$ Mem:%08X\n",
    st,desc,TaskAllocatedMem(task);
  Spaces(indent+2);
  "Flags:%04X:%04X %02X\n",
    task->task_flags,task->display_flags,ExceptDepth(task);
  "Time:%9.2fm\n",task->total_time/60.0/time_stamp_freq_initial;
  Free(st);
  Free(desc);
  task1=task->next_child_task;
  while (task1!=(&task->next_child_task)(U8 *)-offset(TaskStruct.next_sibling_task)) {
    TaskRepTask(task1,indent+2);
    task1=task1->next_sibling_task;
  }
}

public U0 TaskRep()
{
  U8 i;
  CPUStruct *c;
  PushFD;
  Cli;
  for (i=0;i<mp_cnt;i++) {
    c=&cpu_structs[i];
    "$$FG,RED$$CPU:%d$$FG$$\n",i;
    TaskRepTask(c->seth_task,2);
  }
  PopFD;
}

#help_index ""
