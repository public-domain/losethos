U64 ExceptDepth(TaskStruct *task=NULL)
{
  U64 result=0;
  ExceptStruct *tempt;
  if (!task) task=Fs;
  tempt=task->next_except;
  while (tempt!=&task->next_except) {
    result++;
    tempt=tempt->next;
  }
  return result;
}

asm {
USE64
#assert !((CREG_NON_PTR_MASK|CREG_VARS_MASK)&~0xFCC0)
_TEST_EXCEPT::
	XOR	RAX,RAX
	MOV	RAX,FS:U64 TaskStruct.last_except[RAX]
	MOV	RBP,U64 ExceptStruct.rbp[RAX]
	MOV	RSI,U64 ExceptStruct.rsi[RAX]
	MOV	RDI,U64 ExceptStruct.rdi[RAX]
	MOV	R10,U64 ExceptStruct.r10[RAX]
	MOV	R11,U64 ExceptStruct.r11[RAX]
	MOV	R12,U64 ExceptStruct.r12[RAX]
	MOV	R13,U64 ExceptStruct.r13[RAX]
	MOV	R14,U64 ExceptStruct.r14[RAX]
	MOV	R15,U64 ExceptStruct.r15[RAX]
	PUSH	U64 ExceptStruct.rflags[RAX]
	POPFD
	JMP	U64 ExceptStruct.handler_catch[RAX]

_TAKE_EXCEPT::
	XOR	RAX,RAX
	MOV	RAX,FS:U64 TaskStruct.last_except[RAX]
	MOV	RSP,U64 ExceptStruct.rsp[RAX]
	JMP	U64 ExceptStruct.handler_untry[RAX]

_SAVE_EXCEPT_REGS::
	ENTER	0
	MOV	RAX,U64 SF_ARG1[RBP]
	MOV	U64 ExceptStruct.rsi[RAX],RSI
	MOV	U64 ExceptStruct.rdi[RAX],RDI
	MOV	U64 ExceptStruct.r10[RAX],R10
	MOV	U64 ExceptStruct.r11[RAX],R11
	MOV	U64 ExceptStruct.r12[RAX],R12
	MOV	U64 ExceptStruct.r13[RAX],R13
	MOV	U64 ExceptStruct.r14[RAX],R14
	MOV	U64 ExceptStruct.r15[RAX],R15
	LEAVE
	RET1	8
}

LTextern _TEST_EXCEPT U0 TestExcept();
LTextern _TAKE_EXCEPT U0 TakeExcept();
LTextern _SAVE_EXCEPT_REGS U0 SaveExceptRegs(ExceptStruct *t);

#exe {NoRegVar(ON);};

class SysTryStk
{
  U64 var_tempt;
  U64 var_old_flags;
  U64 var_rbp;
  U64 var_rsp;
  U64 rbp;
  U64 ret_rip;
  U64 arg1;
  U64 arg2;
};

U0 SysTry(U8 *catch_start,U8 *untry_start)
{
  U64 *rsp=GetStkPtr,*rbp=GetBasePtr,old_flags=GetFlags;
  ExceptStruct *tempt=MAlloc(sizeof(ExceptStruct));
  tempt->handler_catch=catch_start;
  tempt->handler_untry=untry_start;
  tempt->rsp=rsp(U8 *)+sizeof(SysTryStk);
  tempt->rbp=*rbp;
  tempt->rflags=old_flags;
  SaveExceptRegs(tempt);
  Cli;
  InsQue(tempt,Fs->last_except);
  SetFlags(old_flags);
}

U0 SysUntry()
{
  ExceptStruct *tempt=Fs->last_except;
  PushFD;
//The adam task sometimes probes except depth,
//so we lock it out.A probe from a multicore
//would fail, but who'd want to do that?
  Cli;
  RemQue(tempt);
  PopFD;
  Free(tempt);
}

U0 AdamLogExcept(I64 argc,I64 *argv)
{
  U64 i;
  if (!Gs->num) { //TODO: Other cores
    AdamLog("Depth:%02X Task:%08X throw(",
      ExceptDepth(Fs),Fs);
    for (i=0;i<argc;i++) {
      AdamLog("%X",argv[i]);
      if (i<argc-1)
	AdamLog(",");
    }
    AdamLog(");\n");
  }
}

U0 PutExcept()
{
  I64 i;
  if (Fs->except_argc>0) {
    if (Fs->except_argv[0]<DefineSubCnt("ST_EXCEPT_TYPES")) {
      "Exception Type :%Z\n",Fs->except_argv[0],"ST_EXCEPT_TYPES";
      if (Fs->except_argv[0]==EXCEPT_UNDEF_DEFINE)
	"Name:'%s'\n",Fs->except_argv[1];
      else if (Fs->except_argv[0]==EXCEPT_DIVIDE_BY_ZERO)
	"Addr:%P\n",Fs->except_argv[1];
      else {
	for (i=1;i<Fs->except_argc;i++)
	  "%016X ",Fs->except_argv[i];
	NewLine;
      }
    } else {
      "Exception:";
      for (i=0;i<Fs->except_argc;i++)
	"%016X ",Fs->except_argv[i];
      NewLine;
    }
  } else
    "Generic Exception\n";
}

U0 throw_join(I64 argc,I64 *argv)
{
  ExceptStruct *tempt=Fs->last_except;
  BoolI8 was_raw;

  Fs->except_argc=argc;
  Fs->except_argv=argv;
  Fs->except_rbp=GetBasePtr;
  Fs->catch_except=FALSE;
  Fs->ignore_except=FALSE;
  while (Fs->next_except!=&Fs->next_except) {
    TestExcept;
    if (Fs->catch_except)
      TakeExcept;
    if (Fs->ignore_except)
      return;
    SetBasePtr(Fs->except_rbp);
    tempt=Fs->last_except;
    RemQue(tempt);
    Free(tempt);
  }
  was_raw=Raw(ON);
  PutExcept;
  if (!Gs->num)
    Dbg("Unhandled Exception");
  else {
    DbgStr("Unhandled MP Exception",5000);
    while (TRUE)
      Beep;
  }
  Raw(was_raw);
}

U0 throw_no_log(...)
{
  throw_join(argc,argv);
}

U0 throw(...)
{
  AdamLogExcept(argc,argv);
  throw_join(argc,argv);
}

#exe {NoRegVar(OFF);};

U0 SysThrowBreak()
{
  if (Bt(&Fs->task_flags,TASKf_BREAK_TO_SHIFT_ESC))
    Msg(MSG_KEY_DOWN,CH_SHIFT_ESC,0x20100000201);
  else { //TODO: Might want lock
    Fs->wake_jiffy=0;
    TaskRstAwaitingMsg;
    PrtsRelease();
    BlkDevsRelease();
    FlushMsgs;
    throw(EXCEPT_BREAK);
  }
}

//This can be used to exit an app, but not a task.
U0 ThrowBreak(TaskStruct *task=NULL)
{
  if (task && task!=Fs) {
    if (!TaskValidate(task)) return;
    //TODO: Might want lock
    sys_focus_task=task;
    LBts(sys_ctrl_alt_flags,SYSf_CTRL_ALT_C);
  } else
    SysThrowBreak;
}

BoolI64 BreakLock(TaskStruct *task=NULL)
{
  if (!task) task=Fs;
  return !LBts(&task->task_flags,TASKf_BREAK_LOCKED);
}

BoolI64 BreakUnlock(TaskStruct *task=NULL)
{
  BoolI8 result;
  if (!task) task=Fs;
  result=LBtr(&task->task_flags,TASKf_BREAK_LOCKED);
  if (LBtr(&task->task_flags,TASKf_PENDING_BREAK)) {
    if (task==Fs)
      ThrowBreak;
    else
      task->rip=&ThrowBreak;
  }
  return result;
}

U0 CatchAll()
{
  PutExcept;
  Fs->catch_except=TRUE;
}
