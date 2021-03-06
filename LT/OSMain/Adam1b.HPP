/*
In this file, syms defined OSMain assembly
files are given C+ definitions.  This file
is included when you compile the kernel, after
the assembly files have been assembled, making
the C+ definitions available for the C+ OSMain
files.	It is also included as part of booting.

The assembly global labels are stored as HTT_EXPORT_SYS_SYM
entries in the hash table.  These symols are placed
into the OSMain binary module and added to the
sym hash table at boot before this file
gets #included during boot.

See
$LK,"Adam1b","FF:::/LT/OSMain/OSMain.SPZ,Adam1b"$ for where it is included during compiling the kernel.
$LK,"Adam1b","FF:::/LT/OSMain/Adam1.APZ,Adam1b"$ for where it is included during boots.
$LK,"LoadOSMain","FF:::/LT/OSMain/OSEnd.CPZ,LoadOSMain"$ for where the OSMain syms are added to the hash tables.

*/

#help_index "Misc;Messages"
public LTextern SYS_FOCUS_TASK TaskStruct *sys_focus_task;

#help_index "Graphics/Screen"
//This is the INT 0x10 mode number.
public LTextern SYS_BIOS_GR_MODE U16 bios_gr_mode;
#help_index "PCI"
public LTextern SYS_PCI_BUSSES U16 sys_pci_busses;
#help_index "Misc"
public LTextern SYS_CTRL_ALT_FLAGS U64 sys_ctrl_alt_flags[1];
public LTextern SYS_COMPILE_TIME LTDate sys_compile_time;

#help_index "Boot"
LTextern SYS_BOOT_BASE U32 sys_boot_base;
LTextern SYS_BOOT_CODE U32 sys_boot_code;
LTextern SYS_RUN_LEVEL U32 sys_run_level;
LTextern SYS_BOOT_PATCH_TABLE_BASE U32 sys_boot_patch_table_base;

#help_index "Misc/Progress Bars"
public LTextern SYS_PROGRESSES ProgressStruct sys_progresses[NUM_PROGRESS_BARS];
public LTextern SYS_PROGRESS1	   U64 progress1;
public LTextern SYS_PROGRESS1_MAX  U64 progress1_max;
public LTextern SYS_PROGRESS1_DESC U8  progress1_desc[PROGRESS_DESC_LEN+1];
public LTextern SYS_PROGRESS2	   U64 progress2;
public LTextern SYS_PROGRESS2_MAX  U64 progress2_max;
public LTextern SYS_PROGRESS2_DESC U8  progress2_desc[PROGRESS_DESC_LEN+1];
public LTextern SYS_PROGRESS3	   U64 progress3;
public LTextern SYS_PROGRESS3_MAX  U64 progress3_max;
public LTextern SYS_PROGRESS3_DESC U8  progress3_desc[PROGRESS_DESC_LEN+1];
public LTextern SYS_PROGRESS4	   U64 progress4;
public LTextern SYS_PROGRESS4_MAX  U64 progress4_max;
public LTextern SYS_PROGRESS4_DESC U8  progress4_desc[PROGRESS_DESC_LEN+1];
 
#help_index "Bit"
#help_file "::/LT/Doc/Bit.TXZ"
public LTintern EC_BT BoolI64 Bt(U0 *bit_field,U64 bit);
public LTintern EC_BTS BoolI64 Bts(U0 *bit_field,U64 bit);
public LTintern EC_BTR BoolI64 Btr(U0 *bit_field,U64 bit);
public LTintern EC_BTC BoolI64 Btc(U0 *bit_field,U64 bit);
public LTintern EC_BEQU BoolI64 BEqu(U0 *bit_field,U64 bit,BoolI8 val);

//Locked for (for multitple CPU's)
public LTintern EC_LBTS BoolI64 LBts(U0 *bit_field,U64 bit);
public LTintern EC_LBTR BoolI64 LBtr(U0 *bit_field,U64 bit);
public LTintern EC_LBTC BoolI64 LBtc(U0 *bit_field,U64 bit);
public LTintern EC_LBEQU BoolI64 LBEqu(U0 *bit_field,U64 bit,BoolI8 val);

//Scan U64 sized item fwd for first set bit, starting at lowest.
//Return -1 if not found.
public LTintern EC_BSF I64 Bsf(U64 bit_field_val);

//Scan U64 sized item reverse for first set bit, starting at highest.
//Return -1 if not found.
public LTintern EC_BSR I64 Bsr(U64 bit_field_val);

public LTextern _BIT_FIELD_OR_U32 U0 BFieldOrU32(
	 U0 *bit_field,U64 bit,U32 pattern);
public LTextern _BIT_FIELD_EXTRACT_U32 U32 BFieldExtU32(
	 U0 *bit_field,U64 bit,U64 size);

#help_index "Circular Queue"
#help_file "::/LT/Doc/Queue.TXZ"

//Insert item into queue after predicessor.
public LTintern EC_INSQUE U0 InsQue(U0 *entry,U0 *pred);
//Reversed insert into queue.  Insert
//item into queue before successor.
public LTintern EC_INSQUE_REV U0 InsQueRev(U0 *entry,U0 *succ);
//Remove item from queue.
public LTintern EC_REMQUE U0 RemQue(U0 *entry);

#help_index "Task"
public LTintern EC_PREEMPT BoolI64 Preempt(BoolI8 val);

#help_index "I/O;Processor/IO Port"
public LTintern EC_INU8  U64 InU8(U64 port);
public LTintern EC_INU16 U64 InU16(U64 port);
public LTintern EC_INU32 U64 InU32(U64 port);
public LTintern EC_OUTU8  U0 OutU8(U64 port, U64 val);
public LTintern EC_OUTU16 U0 OutU16(U64 port, U64 val);
public LTintern EC_OUTU32 U0 OutU32(U64 port, U64 val);
public LTextern _REP_IN_U8  U0 RepInU8(U0 *buf,U64 cnt,U64 port);
public LTextern _REP_IN_U16 U0 RepInU16(U0 *buf,U64 cnt,U64 port);
public LTextern _REP_IN_U32 U0 RepInU32(U0 *buf,U64 cnt,U64 port);
public LTextern _REP_OUT_U8  U0 RepOutU8(U0 *buf,U64 cnt,U64 port);
public LTextern _REP_OUT_U16 U0 RepOutU16(U0 *buf,U64 cnt,U64 port);
public LTextern _REP_OUT_U32 U0 RepOutU32(U0 *buf,U64 cnt,U64 port);

#help_index "Memory"

//Only goes fwd.
public LTintern EC_MEMCPY    U0  MemCpy(U0 *dst,U0 *src,U64 cnt);
public LTintern EC_MEMSET    U8 *MemSet(U0 *dst,U64 val,U64 cnt);
public LTintern EC_MEMSET    U8 *MemSetU8(U0 *dst,U64 val,U64 U8cnt);
public LTintern EC_MEMSET_U16 U16 *MemSetU16(U0 *dst,U64 val,U64 U16cnt);
public LTintern EC_MEMSET_U32 U32 *MemSetU32(U0 *dst,U64 val,U64 U32cnt);
public LTintern EC_MEMSET_U64 U64 *MemSetU64(U0 *dst,U64 val,U64 U64cnt);
public LTextern SYS_CODE_BP BlkPool *sys_code_bp;
public LTextern SYS_DATA_BP BlkPool *sys_data_bp;
public LTextern SYS_HEAP_BASE U32 sys_heap_base;
public LTextern SYS_HEAP_LIMIT U32 sys_heap_limit;

LTextern SYS_HEAP_INIT_FLAG	U8 sys_heap_init_flag;
LTextern SYS_HEAP_INIT_VAL	U8 sys_heap_init_val;
LTextern SYS_MEM_INIT_FLAG	U8 sys_mem_init_flag;
LTextern SYS_MEM_INIT_VAL	U8 sys_mem_init_val;
LTextern SYS_VAR_INIT_FLAG	U8 sys_var_init_flag;
LTextern SYS_VAR_INIT_VAL	U8 sys_var_init_val;
LTextern SYS_HEAP_DBG_FLAG	U8 sys_heap_dbg;

#help_index "Char/Conversion"
public LTintern EC_TOUPPER I64 ToUpper(U8 ch);
#help_index "Char/Operations"
public LTintern EC_STRLEN U64 StrLen(U8 *st);

#help_index "Processor"
public LTextern _CPUID U0 CpuId(U32 rax,RaxRbxRcxRdx *result);
public LTintern EC_SET_RAX U0 SetRAX(U64 d);
public LTintern EC_GET_RAX U64 GetRAX();
public LTintern EC_CLI U0 Cli();
public LTintern EC_STI U0 Sti();
public LTintern EC_PUSHFD U0 PushFD();
public LTintern EC_POPFD U0 PopFD();
public LTintern EC_HLT U0 Hlt();
public LTextern _SYS_HLT U0 SysHlt(); //Loops

//This instruction hints to the processor
//that a spin-loop is taking place.
public LTintern EC_PAUSE U0 Pause();

public LTintern EC_POP U64 Pop();
public LTintern EC_PUSH U0 Push(U64 d);
public LTintern EC_CLD U0 Cld();
public LTintern EC_STD U0 Std();
public LTintern EC_GET_FLAGS U64 GetFlags();
public LTintern EC_SET_FLAGS U0 SetFlags(U64 d);
public LTintern EC_GET_STK_PTR U0 *GetStkPtr();
public LTintern EC_SET_STK_PTR U0 SetStkPtr(U0 *d);
public LTintern EC_GET_BASE_PTR U0 *GetBasePtr();
public LTintern EC_SET_BASE_PTR U0 SetBasePtr(U0 *d);
public LTextern SYS_CACHE_LINE_WIDTH U64 sys_cache_line_width;
LTextern SYS_GDTTAB U8 sys_gdttab[GDT_SIZE];
public LTextern _XCHG_U64 U64 XchgU64(U64 *dst,U64 d);
public LTextern _XCHG_U32 U64 XchgU32(U32 *dst,U32 d);
public LTextern _XCHG_U16 U64 XchgU16(U16 *dst,U16 d);
public LTextern _XCHG_U8  U64 XchgU8(U8 *dst,U8 d);
public LTextern _LXCHG_U64 U64 LXchgU64(U64 *dst,U64 d);
public LTextern _LXCHG_U32 U64 LXchgU32(U32 *dst,U32 d);
public LTextern _LXCHG_U16 U64 LXchgU16(U16 *dst,U16 d);
public LTextern _LXCHG_U8  U64 LXchgU8(U8 *dst,U8 d);

//Model Specific REgisters
//See $LK,"MSRs","MN:IA32_EFER"$
public LTextern _SET_MSR U0 SetMSR(U64 model_specific_reg,U64 val);

#help_index "Processor/Cache;Memory/Cache"
//Invalidate cache
public LTintern EC_INVD U0 Invd();
//Write back and invalidate cache.
public LTintern EC_WBINVD U0 WbInvd();
public LTintern EC_INVLPG U0 InvlPg(U0 *a);
//Flush cache line.
public LTintern EC_CLFLUSH U0 CLFlush(U0 *a);

#help_index "MultiCore"
#help_file "::/LT/Doc/MultiCore.TXZ"
LTextern _SET_GS_BASE U0 SetGs(CPUStruct *c);
public LTextern SYS_MP_CNT U64 mp_cnt;
LTextern SYS_MP_CNT_INITIAL U64 mp_cnt_initial; //only used during $LK,"MPStart","MN:MPStart"$
LTextern SYS_MP_CNT_LOCK U64 mp_cnt_lock;
LTextern SYS_TEMP_CPU0_STRUCT CPUStruct sys_temp_cpu0_struct;
public LTextern SYS_CPU_STRUCTS	 CPUStruct *cpu_structs;
LTextern SYS_SEMAS Sema sys_semas[NUM_SYS_SEMAS];

#help_index "MultiCore;Processor"
//This points to the current CPU structure.
//It returns the val in the GS segment register.
public LTintern EC_GS CPUStruct *Gs();

#help_index "Time/CPU Cycles"
public LTintern EC_RDTSC U64 GetTimeStamp();

#help_index "Time/HPET"
public LTintern EC_HPET U64 HPET();

#help_index "Time/Jiffies"
#help_file "::/LT/Doc/TimeJiffy"
LTextern SYS_TIMER U64 sys_timer;
public LTextern SYS_JIFFIES U64 sys_jiffies;

#help_index "Math"
public LTintern EC_TO_I64 I64 ToI64(F64 d);  //This truncates
public LTintern EC_TO_F64 F64 ToF64(I64 i);
public LTintern EC_TO_BOOL BoolI64 ToBool(I64 i);
public LTintern EC_ABS_I64 I64 AbsI64(I64 i);
public LTintern EC_LIMIT_I64 I64 LimitI64(I64 p,I64 lo,I64 hi);
public LTintern EC_LIMIT_U64 U64 LimitU64(U64 p,U64 lo,U64 hi);
public LTintern EC_SIGN_I64 I64 SignI64(I64 i);
public LTintern EC_SQR_I64  I64 SqrI64(I64 i);
public LTintern EC_SQR_U64  I64 SqrU64(U64 i);
public LTintern EC_SWAP_U8  U0 SwapU8(U8 *n1,U8 *n2);
public LTintern EC_SWAP_U16 U0 SwapU16(U16 *n1,U16 *n2);
public LTintern EC_SWAP_U32 U0 SwapU32(U32 *n1,U32 *n2);
public LTintern EC_SWAP_U64 U0 SwapU64(U64 *n1,U64 *n2);
public LTintern EC_MIN_I64 I64 MinI64(I64 n1,I64 n2);
public LTintern EC_MAX_I64 I64 MaxI64(I64 n1,I64 n2);
public LTintern EC_MIN_U64 U64 MinU64(U64 n1,U64 n2);
public LTintern EC_MAX_U64 U64 MaxU64(U64 n1,U64 n2);
public LTintern EC_MOD_U64 U64 ModU64(U64 *q,U64 d);
public LTextern _FSQRT F64 Sqrt(F64 d);
public LTextern _FABS F64 Abs(F64 d);
public LTextern _SIGN F64 Sign(F64 d);
public LTextern _SQR  F64 Sqr(F64 d);
public LTextern _FSIN F64 Sin(F64 d);
public LTextern _FCOS F64 Cos(F64 d);
public LTextern _FTAN F64 Tan(F64 d);
public LTextern _ARG  F64 Arg(F64 x,F64 y);
public LTextern _FATAN F64 ATan(F64 d);
public LTextern _ROUND F64 Round(F64 d);
public LTextern _TRUNC F64 Trunc(F64 d);
public LTextern _FLOOR F64 Floor(F64 d);
public LTextern _CEIL  F64 Ceil(F64 d);
public LTextern _POW F64 Pow(F64 base,F64 power);
public LTextern _POW10 F64 Pow10(F64 d);
public LTextern _LOG2  F64 Log2(F64 d);
public LTextern _LOG10 F64 Log10(F64 d);
public LTextern _LN  F64 Ln(F64 d);
public LTextern _EXP F64 Exp(F64 d);
LTextern _FLDCW U0 Fldcw(U16 w);
LTextern _FCLEX U0 Fclex();
LTextern _FSTCW U16 Fstcw();
LTextern _FSTSW U16 Fstsw();
LTextern _FXSAVE  U0 Fxsave(FPUState *fpu_mmx);
LTextern _FXRSTOR U0 Fxrstor(FPUState *fpu_mmx);
LTextern SYS_INIT_FPU_MMX FPUState sys_init_fpu_mmx;

#help_index "Math/D3;Ode/D3"
#help_file "::/LT/Doc/D3.TXZ"
public LTextern _D3_EQU D3 *D3Equ(D3 *dst,F64 x,F64 y,F64 z);
public LTextern _D3_ZERO D3 *D3Zero(D3 *dst);
public LTextern _D3_COPY D3 *D3Copy(D3 *dst,D3 *src);
public LTextern _D3_NORM F64 D3Norm(D3 *d);
public LTextern _D3_SQR_NORM F64 D3SqrNorm(D3 *d);
public LTextern _D3_ADD D3 *D3Add(D3 *d1,D3 *d2,D3 *sum);
public LTextern _D3_SUB D3 *D3Sub(D3 *d1,D3 *d2,D3 *diff);
public LTextern _D3_MUL D3 *D3Mul(F64 c,D3 *d,D3 *prod);
public LTextern _D3_DIV D3 *D3Div(F64 c,D3 *d,D3 *quot);
public LTextern _D3_ADD_EQU D3 *D3AddEqu(D3 *sum,D3 *d);
public LTextern _D3_SUB_EQU D3 *D3SubEqu(D3 *diff,D3 *d);
public LTextern _D3_MUL_EQU D3 *D3MulEqu(D3 *prod,F64 c);
public LTextern _D3_DIV_EQU D3 *D3DivEqu(D3 *quot,F64 c);
public LTextern _D3_DIST F64 D3Dist(D3 *d1,D3 *d2);
public LTextern _D3_SQR_DIST F64 D3SqrDist(D3 *d1,D3 *d2);
//Dot Product
public LTextern _D3_DOT F64 D3Dot(D3 *d1,D3 *d2);
//Cross Product
public LTextern _D3_CROSS D3 *D3Cross(D3 *d1,D3 *d2,D3 *prod);
//Convert to unit vect
public LTextern _D3_UNIT D3 *D3Unit(D3 *d);


#help_index "Task;Processor"
//This points to the current task record.
//It returns the val in the FS segment register.
public LTintern EC_FS TaskStruct *Fs();

#help_index "Call"
public LTextern _CALL I64 Call(U0 *s);

//fp_add can have any fixed number of arguments.
public LTextern _CALLIND I64 CallInd(/*argpop*/I64 (*fp_add)(...),...);

//This is just an array of ptrs to funs.
//See $LK,"Extern Num Definitions","MN:EXT_TABLE_SIZE"$
public LTextern SYS_EXTERN_TABLE /*argpop*/ I64 (**ext)(...);
//Can have any fixed number of arguments.
public LTextern _CALLEXTNUM I64 CallExtNum(U64 num,...);

//Search sym table for fun name and pass it args.
//Can have any fixed number of arguments.
public LTextern _CALLEXTSTR I64 CallExtStr(/*argpop*/ U8 *name,...);

#help_index ""
