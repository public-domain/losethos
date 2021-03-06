U0 CbPutClass(ClassStruct *c)
{
  I64 i;
  if (!c) return;
  if (c->ptr_cnt>4) {
    PrintFErr("put_class ptrcnt=%d\n",c->ptr_cnt);
    while (TRUE)
//TODO:
      Yield;
  }
  for (i=0;i<c->ptr_cnt;i++)
    '*';
  c-=c->ptr_cnt;
  if (c->str)
    "" c->str;
  else {
    PrintFErr("put_class str=NULL\n");
    while (TRUE)
//TODO:
      Yield;
  }
  '' CH_SPACE;
}


U0 CbPutIcArg(IcArg *p)
{
  if (p->ptype)
    "%Z %Z %Z ",
      p->type&IT_MASK,"ST_IT_TYPES",
      p->ptype&IT_MASK,"ST_IT_TYPES",
      p->type>>4,"ST_TY_TYPES";
  else
    "%Z %Z ",
      p->type&IT_MASK,"ST_IT_TYPES",
      p->type>>4,"ST_TY_TYPES";
  switch (p->type>>4) {
    case TY_STK>>4:
      "STK";
      break;
    case TY_IMM>>4:
      "#%X",p->disp;
      break;
    case TY_REG>>4:
      "%Z",p->reg,"ST_U64_REGS";
      break;
    case TY_DISP>>4:
      "%X[%Z]",p->disp,p->reg,"ST_U64_REGS";
      break;
    case TY_RIP_DISP32>>4:
      "[%X]",p->disp;
      break;
    case TY_SIB>>4:
      if (p->disp)
	"%X",p->disp;
      if (p->reg==CREG_RIP)
	'[';
      else
	"[%Z+",p->reg&7,"ST_U64_REGS";
      "%Z",p->reg>>8&7,"ST_U64_REGS";
      switch (p->reg>>14) {
	case 0: "]"; break;
	case 1: "*2]"; break;
	case 2: "*4]"; break;
	case 3: "*8]"; break;
      }
      break;
  }
}

U0 CbPutIc(IntermediateCode *tempi)
{
  U64 opcode=tempi->ic_opcode,i;
  if (opcode>=EC_END_EXP && opcode!=EC_NOP1) {
//"%d,%d ",tempi->ic_line+tempi->ic_start_line_offset,tempi->ic_indent_level;
    "%15ts %016X ",
      DefineSub(opcode,"ST_INTERMEDIATE_CODE")+4,
      tempi->ic_data;
    if (tempi->r.type&TY_MASK) {
      CbPutIcArg(&tempi->r);
      "<-";
    }
    if (tempi->p1.type&TY_MASK)
      CbPutIcArg(&tempi->p1);
    ',';
    if (tempi->p2.type&TY_MASK)
      CbPutIcArg(&tempi->p2);
    ',';
    if (tempi->p3.type&TY_MASK)
      CbPutIcArg(&tempi->p3);
    '' CH_SPACE;
    CbPutClass(tempi->ic_class);
    if (tempi->ic_flags & ICF_LOCK)
      "$$FG,BROWN$$lock$$FG$$ ";
    if (tempi->ic_flags & ICF_P3_TO_F64)
      "$$FG,LTBLUE$$p3d$$FG$$ ";
    if (tempi->ic_flags & ICF_P3_TO_INT)
      "$$FG,GREEN$$p3i$$FG$$ ";
    if (tempi->ic_flags & ICF_P2_TO_F64)
      "$$FG,LTBLUE$$p2d$$FG$$ ";
    if (tempi->ic_flags & ICF_P2_TO_INT)
      "$$FG,GREEN$$p2i$$FG$$ ";
    if (tempi->ic_flags & ICF_P1_TO_F64)
      "$$FG,LTBLUE$$p1d$$FG$$ ";
    if (tempi->ic_flags & ICF_P1_TO_INT)
      "$$FG,GREEN$$p1i$$FG$$ ";
    if (tempi->ic_flags & ICF_R_TO_F64)
      "$$FG,LTBLUE$$rd$$FG$$ ";
    if (tempi->ic_flags & ICF_R_TO_INT)
      "$$FG,GREEN$$ri$$FG$$ ";
    if (tempi->ic_flags & ICF_USE_F64)
      "[F64] ";
    if (tempi->ic_flags & ICF_USE_UNSIGNED)
      "[unsigned] ";
    if (tempi->ic_flags & ICF_USE_INT)
      "[int] ";
    if (tempi->ic_flags & ICF_NO_DEPEND_RESULT)
      "-DEP ";
    if (tempi->ic_flags & ICF_DEPEND_RESULT)
      "+DEP ";
    if (tempi->ic_flags & ICF_NOT_ADDR)
      "NOT_ADDR ";
    if (tempi->ic_flags & ICF_PUSH_RESULT)
      "PUSH ";
    if (tempi->ic_flags & ICF_PUSH_CMP)
      "PUSH_CMP ";
    if (tempi->ic_flags & ICF_POP_CMP)
      "POP_CMP ";
    if (tempi->ic_flags & ICF_DEL_PREVIOUS_INS)
      "DELINS ";
    for (i=0;i<4;i++) {
      if (Bt(&tempi->ic_flags,ICf_DONT_PUSH_FLOAT0+i))
	"DONT_PUSH#%d ",i;
      if (Bt(&tempi->ic_flags,ICf_DONT_POP_FLOAT0+i))
	"DONT_POP#%d ",i;
    }
    if (tempi->ic_flags & ICF_ALTERNATE_TEMPLATE)
      "ALT_TEMP ";
    NewLine;
  }
}

IntermediateCode *CbIcAdd(LexStruct *lx,
   U64 opcode_and_precedence,U64 arg, ClassStruct *c,U64 flags=0)
{
  IntermediateCode *tempi=lx->cbh.cb_out_ptr;
  CodeBlk *cb,*cb1;
  lx->cbh.cb_last_out_ptr=tempi;
  tempi->ic_opcode=opcode_and_precedence.u16[0];
  tempi->ic_precedence=opcode_and_precedence.u16[1];
  tempi->ic_data=arg;
  tempi->ic_class=c;
  if (Bt(&lx->opts,OPTf_OPT_TRACE)) {
    Bts(&lx->flags,Lf_OPT_TRACE_PRESENT);
    flags|=ICF_TRACE;
  }
  if (lx->lock_cnt)
    flags|=ICF_LOCK;
  tempi->ic_flags=flags;
  tempi->ic_line=lx->last_line_num;
  tempi->ic_start_line_offset=lx->indent_line_offset;
  lx->indent_line_offset=0;
  tempi->ic_indent_level=lx->last_indent_level;
  if (lx->flags&LF_INSIDE_EXPRESSION &&
      lx->start_line_num>lx->expression_start_line)
    tempi->ic_indent_level+=2;
  if (lx->opts & OPTF_PARSE_TRACE)
    CbPutIc(tempi);
  tempi++;
  if (tempi>=lx->cbh.cb_out_end) {
    cb=MAlloc(sizeof(CodeBlk));
    cb->next=NULL;
    cb1=lx->cbh.cb_out_last;
    cb1->next=lx->cbh.cb_out_last=cb;
    tempi=&cb->data;
    lx->cbh.cb_out_end=tempi+CB_IC_CNT;
  }
  lx->cbh.cb_out_ptr=tempi;
  return lx->cbh.cb_last_out_ptr;
}

U0 CbOutPtrsInit(LexStruct *lx)
{
  LexCbHeader *tempcbh=&lx->cbh;
  CodeBlk *cb=MAlloc(sizeof(CodeBlk));
  cb->next=NULL;
  tempcbh->cb_out_first=tempcbh->cb_out_last=cb;
  tempcbh->cb_misc_list=NULL;
  tempcbh->cb_misc_list_end=&tempcbh->cb_misc_list;
  tempcbh->cb_out_ptr=&cb->data;
  tempcbh->cb_out_end=tempcbh->cb_out_ptr+CB_IC_CNT;
  tempcbh->cb_last_out_ptr=&cb->data;
}

U0 CbInPtrsInit(LexCbHeader *tempcbh)
{
  CodeBlk *cb=tempcbh->cb_out_first;
  tempcbh->cb_in_first=cb;
  tempcbh->cb_in_ptr=&cb->data;
  tempcbh->cb_in_end=tempcbh->cb_in_ptr+CB_IC_CNT;
}

U0 CbPutAllIc(LexStruct *lx)
{
  IntermediateCode *tempi;
  CbInPtrsInit(&lx->cbh);
  tempi=lx->cbh.cb_in_ptr;
  while (tempi->ic_opcode) {
    if (tempi->ic_flags&ICF_TRACE)
      CbPutIc(tempi);
    tempi++;
  }
}

IntermediateCode *CbGetIc1(LexCbHeader *lx)
{
  IntermediateCode *tempi=lx->cb_in_ptr,
      *tempi1=tempi+1;
  CodeBlk *cb;
  if (tempi1>=lx->cb_in_end) {
    cb=tempi1(U8 *)-sizeof(IntermediateCode)*CB_IC_CNT-offset(CodeBlk.data);
    cb=cb->next;
    tempi1=&cb->data;
    lx->cb_in_end=tempi1+CB_IC_CNT;
  }
  lx->cb_in_ptr=tempi1;
  return tempi;
}

U0 CbPush(LexStruct *lx)
{
  LexCbHeader *tempcbh=MAlloc(sizeof(LexCbHeader));
  MemCpy(tempcbh,&lx->cbh,sizeof(LexCbHeader));
  if (lx->cbh.cb_misc_list_end==&lx->cbh.cb_misc_list)
    tempcbh->cb_misc_list_end=&tempcbh->cb_misc_list;
  lx->cbh.cb_next=tempcbh;
}

U0 CbPop(LexStruct *lx)
{
  LexCbHeader *tempcbh=lx->cbh.cb_next;
  MemCpy(&lx->cbh,tempcbh,sizeof(LexCbHeader));
  if (tempcbh->cb_misc_list_end==&tempcbh->cb_misc_list)
    lx->cbh.cb_misc_list_end=&lx->cbh.cb_misc_list;
  Free(tempcbh);
}

CbMiscStruct *CbGoToLabelFind(LexStruct *lx,U8 *name)
{
  CbMiscStruct *mc=lx->cbh.cb_misc_list;
  while (mc) {
    if (mc->type==CBMT_GOTO_LABEL) {
      if (!StrCmp(mc->str,name))
	return mc;
    }
    mc=mc->next;
  }
  return NULL;
}

CbMiscStruct *CbMiscNew(LexStruct *lx,U64 ty)
{
  CbMiscStruct *result=CAlloc(sizeof(CbMiscStruct));
  lx->cbh.cb_misc_list_end->next=result;
  lx->cbh.cb_misc_list_end=result;
  result->addr=INVALID_PTR;
  result->type=ty;
  return result;
}

I64 CbFloatConstFind(LexStruct *lx,F64 d)
{
  I64 i;
  CbMiscStruct *mc=lx->cbh.cb_misc_list;
  while (mc) {
    if (mc->type==CBMT_FLOAT_CONSTS) {
      for (i=0;i<mc->num_consts;i++)
	if (mc->float_consts[i]==d)
	  return mc->addr+i*sizeof(F64);
      if (mc->num_consts<CBM_MAX_CONSTS)  {
	mc->float_consts[mc->num_consts++]=d;
	return mc->addr+i*sizeof(F64);
      }
    }
    mc=mc->next;
  }
  mc=CbMiscNew(lx,CBMT_FLOAT_CONSTS);
  mc->float_consts=MAlloc(CBM_MAX_CONSTS*sizeof(F64));
  mc->float_consts[mc->num_consts++]=d;
  return mc->addr;
}

U0 CbMiscListDel(LexStruct *lx)
{
  CbMiscStruct *mc=lx->cbh.cb_misc_list,*mc1;
  U8 *undef=NULL;
  while (mc) {
    mc1=mc->next;
    switch (mc->type) {
      case CBMT_GOTO_LABEL:
	if (!(mc->flags&CBMF_DEFINED)) {
	  undef=mc->str;
	  mc->str=NULL;
	} else if (!(mc->flags&CBMF_USED)) {
	  PrintFWarn("%S%s\n","ST_UNUSED_LABEL",mc->str);
	  PutLexWarn(lx,"ST_UNUSED_LABEL_AT");
	}
	break;
      case CBMT_JMP_TABLE:
	Free(mc->jmp_table);
	break;
      case CBMT_FLOAT_CONSTS:
	Free(mc->float_consts);
	break;
      case CBMT_ARRAY_DIM:
	LinkedListDel(mc->dim);
	break;
      case CBMT_HASH_ENTRY:
	HashDel(mc->h);
	break;
    }
    Free(mc->str);
    Free(mc);
    mc=mc1;
  }
  if (undef) {
    PrintFErr("%S%s\n","ST_UNDEFINED_GOTO_LABEL",undef);
    Free(undef);
    PrsExcept(lx,"ST_UNDEFINED_GOTO_LABEL_AT");
  }
}

LexStruct *CbPopNoFree(LexStruct *lx)
{
  LexCbHeader *tempcbh=lx->cbh.cb_next;
  MemCpy(&lx->cbh,tempcbh,sizeof(LexCbHeader));
  if (tempcbh->cb_misc_list_end==&tempcbh->cb_misc_list)
    lx->cbh.cb_misc_list_end=&lx->cbh.cb_misc_list;
  return tempcbh;
}

U0 CbAppend(LexStruct *lx, LexCbHeader *tempcbh)
{
  IntermediateCode *tempi,*tempi1;
  CbInPtrsInit(tempcbh);
  do {
    tempi=CbGetIc1(tempcbh);
    if (tempi->ic_opcode) {
      tempi1=CbIcAdd(lx,tempi->ic_opcode+tempi->ic_precedence<<16,tempi->ic_data,
	tempi->ic_class,tempi->ic_flags);
      tempi1->ic_indent_level=tempi->ic_indent_level;
      tempi1->ic_line=tempi->ic_line;
    }
  } while (tempi->ic_opcode);
  LinkedListDel(tempcbh->cb_out_first);

  if (tempcbh->cb_misc_list_end!=&tempcbh->cb_misc_list) {
    lx->cbh.cb_misc_list_end->next=tempcbh->cb_misc_list;
    lx->cbh.cb_misc_list_end=tempcbh->cb_misc_list_end;
  }
  Free(tempcbh);
}

U0 CbCombine(LexStruct *lx)
{
  CodeBlk *cb=lx->cbh.cb_out_first,*cb1,*cb2;
  I64 i=0,j=sizeof(IntermediateCode)*CB_IC_CNT;
  U8 *ptr;
  while (cb) {
    i++;
    cb=cb->next;
  }
  if (i>1) {
    cb1=MAlloc(i*j+offset(CodeBlk.data));
    cb1->next=NULL;
    cb=lx->cbh.cb_out_first;
    ptr=&cb1->data;
    i--;
    while (i--) {
      cb2=cb->next;
      MemCpy(ptr,&cb->data,j);
      ptr+=j;
      Free(cb);
      cb=cb2;
    }
    MemCpy(ptr,&cb->data,lx->cbh.cb_out_ptr(U64)-(&cb->data)(U64));
    Free(cb);
    lx->cbh.cb_out_first=cb1;
  }
}

U8 *CbCompile(LexStruct *lx,FunStruct *fun_class,
		 U64 *code_size,DbgInfo **_dbg,IndentInfo **_indent,U64 *type=NULL)
{
  U8 *result;
  OptRegStruct register_offsets[NUM_REGS];
  I64 min_line=MAX_I64,max_line=0;
  if (_dbg)
    *_dbg=NULL;
  if (_indent)
    *_indent=NULL;

  CbCombine(lx);
  if (Bt(&lx->flags,Lf_OPT_TRACE_PRESENT)) {
    if (Bt(&lx->pass_trace_mask,0)) {
      "$$IV,1$$Before Pass 1:$$IV,0$$\n";
      CbPutAllIc(lx);
    }
  }
  lx->pass=1;
  OptPass012(lx,fun_class,TRUE);
  if (Bt(&lx->flags,Lf_OPT_TRACE_PRESENT)) {
    if (Bt(&lx->pass_trace_mask,1)) {
      "$$IV,1$$After Pass 1:$$IV,0$$\n";
      CbPutAllIc(lx);
    }
  }
  lx->pass=2;
  OptPass012(lx,fun_class,TRUE);
  if (Bt(&lx->flags,Lf_OPT_TRACE_PRESENT)) {
    if (Bt(&lx->pass_trace_mask,2)) {
      "$$IV,1$$After Pass 2:$$IV,0$$\n";
      CbPutAllIc(lx);
    }
  }
  lx->pass=3;
  OptPass3(lx,fun_class,register_offsets);
  if (Bt(&lx->flags,Lf_OPT_TRACE_PRESENT)) {
    if (Bt(&lx->pass_trace_mask,3)) {
      "$$IV,1$$After Pass 3:$$IV,0$$\n";
      CbPutAllIc(lx);
    }
  }
  lx->pass=4;
  OptPass4(lx,register_offsets,type);
  if (Bt(&lx->flags,Lf_OPT_TRACE_PRESENT)) {
    if (Bt(&lx->pass_trace_mask,4)) {
      "$$IV,1$$After Pass 4:$$IV,0$$\n";
      CbPutAllIc(lx);
    }
  }
  lx->pass=5;
  OptPass5(lx);
  if (Bt(&lx->flags,Lf_OPT_TRACE_PRESENT)) {
    if (Bt(&lx->pass_trace_mask,5)) {
      "$$IV,1$$After Pass 5:$$IV,0$$\n";
      CbPutAllIc(lx);
    }
  }
  lx->pass=6;
  OptPass6(lx,fun_class);
  if (Bt(&lx->flags,Lf_OPT_TRACE_PRESENT)) {
    if (Bt(&lx->pass_trace_mask,6)) {
      "$$IV,1$$After Pass 6:$$IV,0$$\n";
      CbPutAllIc(lx);
    }
  }

  if (Bt(&lx->flags,Lf_OPT_TRACE_PRESENT))
    if (Bt(&lx->pass_trace_mask,7))
      "$$IV,1$$Pass 7:$$IV,0$$\n";
  lx->pass=7;
  OptPass789A(lx,fun_class,register_offsets,NULL,_dbg,_indent,&min_line,&max_line);

  if (Bt(&lx->flags,Lf_OPT_TRACE_PRESENT))
    if (Bt(&lx->pass_trace_mask,8))
      "$$IV,1$$Pass 8:$$IV,0$$\n";
  lx->pass=8;
  OptPass789A(lx,fun_class,register_offsets,NULL,_dbg,_indent,&min_line,&max_line);

  if (Bt(&lx->flags,Lf_OPT_TRACE_PRESENT))
    if (Bt(&lx->pass_trace_mask,9))
      "$$IV,1$$Pass 9:$$IV,0$$\n";
  lx->pass=9;
  *code_size=OptPass789A(lx,fun_class,register_offsets,NULL,_dbg,_indent,&min_line,&max_line);
  if (lx->flags&LF_STATIC_COMPILE)
    result=MAlloc(*code_size);
  else
    result=MAlloc(*code_size,Fs->code_heap);

  if (Bt(&lx->flags,Lf_OPT_TRACE_PRESENT))
    if (Bt(&lx->pass_trace_mask,0xA))
      "$$IV,1$$Pass A:$$IV,0$$\n";
  lx->pass=0xA;
  *code_size=OptPass789A(lx,fun_class,register_offsets,result,_dbg,_indent,&min_line,&max_line);
  compiled_bytes+=*code_size;

  LinkedListDel(lx->cbh.cb_out_first);
  CbMiscListDel(lx);
  if (Bt(&lx->opts,OPTf_COMPILE_TRACE)) {
    if (lx->flags&LF_STATIC_COMPILE) {
      if (lx->a->seg_size==16)
	Un(result,*code_size,16);
      else if (lx->a->seg_size==64)
	Un(result,*code_size,64);
      else
	Un(result,*code_size,32);
    } else
      Un(result,*code_size,64);
  }
  return result;
}
