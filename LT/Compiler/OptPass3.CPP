I64 CmpType(ClassStruct *tempc)
{
  if (tempc) {
    while (tempc->fwd_class)
      tempc=tempc->fwd_class;
    return tempc->sub_type;
  }
  return 0;
}

I64 CmpPType(ClassStruct *tempc)
{
  if (tempc) {
    if (tempc->ptr_cnt)
      tempc--;
    while (tempc->fwd_class)
      tempc=tempc->fwd_class;
    return tempc->sub_type;
  }
  return 0;
}

U0 CmpF1PushPop(IntermediateCode *tempi,IntermediateCode *tempi2)
{
  if (Bt(fpop_bitmap,tempi2->ic_opcode)|| tempi2->ic_flags&ICF_R_TO_F64)
    Bts(&tempi->ic_flags,ICf_DONT_PUSH_FLOAT0);
}

U0 CmpF2PushPop(IntermediateCode *tempi,IntermediateCode *tempi1,IntermediateCode *tempi2)
{
  if ((tempi2->ic_opcode==EC_MOV || tempi2->ic_opcode==EC_F64) &&
       !(tempi2->ic_flags&ICF_R_TO_F64) &&
      (Bt(fpop_bitmap,tempi1->ic_opcode) || tempi1->ic_flags&ICF_R_TO_F64))
    Bts(&tempi->ic_flags,ICf_DONT_PUSH_FLOAT0);
  else if ((Bt(fpop_bitmap,tempi2->ic_opcode) || tempi2->ic_flags&ICF_R_TO_F64)&&
       !(tempi1->ic_flags&ICF_R_TO_F64))
    Bts(&tempi->ic_flags,ICf_DONT_PUSH_FLOAT0);
}

class OptMemberVar
{
  I64 score,offset,lea_balance;
  MemberListStruct *m;
};

I64 OptMVCompare(OptMemberVar *mv1,OptMemberVar *mv2)
{
  return mv2->score-mv1->score;
}

U0 OptPass3(LexStruct *lx,FunStruct *fun_class,OptRegStruct *register_offsets)
{
  ClassStruct *tempc,*tempc1,*tempc2,*tempc3;
  CbMiscStruct *lb;
  IntermediateCode *tempi,*tempi1,*tempi2,*tempi3,
      *tempil1,*tempil2,
  *temp_lag[4];
  I64 code,i,j,l,member_cnt,used_reg_mask=0;
  MemberListStruct *tempm;
  PrsStk *ps=OptStart(lx,temp_lag);
  OptMemberVar *mv=NULL;
  for (i=0;i<NUM_REGS;i++) {
    register_offsets[i].offset=MAX_I64;
    register_offsets[i].m=NULL;
  }
  if (fun_class) {
    member_cnt=fun_class->member_cnt;
    if (Bt(&fun_class->flags,Ff_DOT_DOT_DOT))
      member_cnt+=2;
    mv=CAlloc(member_cnt*sizeof(OptMemberVar));
    member_cnt=0;
    tempm=fun_class->member_list;
    while (tempm) {
      tempc=tempm->member_class;
      while (tempc->fwd_class)
	tempc=tempc->fwd_class;
      if (0<=tempm->register<NUM_REGS) {
	if (Bts(&used_reg_mask,tempm->register))
	  PrintFWarn("%S\n $$LK-A,\"FL:%s,%d\"$$'%s' in '%s'\n","ST_REG_IN_USE",
	    lx->cur_lfn->name,lx->cur_lfn->line_num,tempm->str,fun_class->str);
	register_offsets[tempm->register].offset=tempm->offset;
	register_offsets[tempm->register].m=tempm;
      } else if (tempc->sub_type!=IT_F64 && tempm->register!=CREG_NONE ||
	       tempm->register==CREG_ALLOC) {
	if (tempm->register==CREG_ALLOC)
	  mv[member_cnt].score=MAX_I64/2; //big but not too big
	mv[member_cnt].offset=tempm->offset;
	mv[member_cnt++].m=tempm;
      } else {
	if (tempm->register==CREG_ALLOC)
	  PrintFWarn("%S\n $$LK-A,\"FL:%s,%d\"$$'%s' in '%s'\n","ST_CANT_REG_VAR",
	    lx->cur_lfn->name,lx->cur_lfn->line_num,tempm->str,fun_class->str);
      }
      tempm=tempm->next;
    }
  } else
    member_cnt=0;

  tempi=lx->cbh.cb_in_ptr;
  if (tempi->ic_opcode==EC_START)
    tempi++;
  while (code=tempi->ic_opcode) {
    if (code!=EC_NOP) {
      OptNext(tempi,temp_lag);
      tempil1=temp_lag[1];
      tempil2=temp_lag[2];
      tempc=tempi->ic_class;
      nobound_switch (cmp_arg_cnt[code]) {
	case 9:
	  ps->ptr-=tempi->ic_data>>3;
	  break;
	case 3:
	  tempi3=PrsPop(ps);
	  if (tempi3->ic_flags&ICF_R_TO_F64)
	    tempc3=sys_internal_types[IT_F64];
	  else if (tempi3->ic_flags & ICF_R_TO_INT)
	    tempc3=sys_internal_types[IT_I64];
	  else
	    tempc3=tempi->t.p3c;
	  tempi->p3.type=TY_STK+CmpType(tempc3);
	  tempi->p3.ptype=CmpPType(tempc3);
	  tempi->ic_flags|=ICF_P3_WAS_STK;
	case 2:
	  tempi2=PrsPop(ps);
	  if (tempi2->ic_flags&ICF_R_TO_F64)
	    tempc2=sys_internal_types[IT_F64];
	  else if (tempi2->ic_flags & ICF_R_TO_INT)
	    tempc2=sys_internal_types[IT_I64];
	  else
	    tempc2=tempi->t.p2c;
	  tempi->p2.type=TY_STK+CmpType(tempc2);
	  tempi->p2.ptype=CmpPType(tempc2);
	  tempi->ic_flags|=ICF_P2_WAS_STK;
	case 1:
	  tempi1=PrsPop(ps);
	  if (tempi1->ic_flags&ICF_R_TO_F64)
	    tempc1=sys_internal_types[IT_F64];
	  else if (tempi1->ic_flags & ICF_R_TO_INT)
	    tempc1=sys_internal_types[IT_I64];
	  else
	    tempc1=tempi->t.p1c;
	  tempi->p1.type=TY_STK+CmpType(tempc1);
	  tempi->p1.ptype=CmpPType(tempc1);
	  tempi->ic_flags|=ICF_P1_WAS_STK;
	  break;
	case 0: //nobound_switch
	  break;
      }
      if (cmp_result_cnt[code]) {
	tempi->r.type=TY_STK+CmpType(tempc);
	tempi->r.ptype=CmpPType(tempc);
	tempi->ic_flags|=ICF_R_WAS_STK;
	PrsPush(ps,tempi);
      }
      nobound_switch (code) {
	case EC_F64:
	  tempi->ic_flags&=~ICF_R_TO_F64;
	  if (tempi->ic_flags&ICF_R_TO_INT) {
	    tempi->ic_data=ToI64(tempi->ic_data(F64));
	    tempi->ic_flags&=~ICF_R_TO_INT;
	    tempi->ic_opcode=EC_IMM;
	  }
	  break;
	case EC_IMM:
	  tempi->ic_flags&=~ICF_R_TO_INT;
	  if (tempi->ic_flags&ICF_R_TO_F64) {
	    tempi->ic_data(F64)=ToF64(tempi->ic_data);
	    tempi->ic_flags&=~ICF_R_TO_F64;
	    tempi->ic_opcode=EC_F64;
	  }
	  break;
	case EC_STR_CONST:
	case EC_FS:
	case EC_GS:
	case EC_MOV_FS:
	case EC_MOV_GS:
	case EC_RIP:
	case EC_PUSH_CMP:
	  break;
	case EC_RBP:
	  tempi->ic_opcode=EC_REG;
	  tempi->p1.reg=CREG_RBP;
	  break;
	case EC_REG:
	case EC_COM:
	case EC_LT_TYPECAST:
	case EC_NOT:
	case EC_UNARY_MINUS:
	  break;
	case EC_DEREF:
	  tempi->p1.ptype=tempi->r.type&IT_MASK;
	  if (tempi1->ic_opcode==EC_LEA) {
	    tempi->ic_flags|=tempi1->ic_flags;
	    tempi1->ic_opcode=EC_NOP;
	    tempi->p1.reg=tempi1->p1.reg;
	    tempi->p1.disp=tempi1->p1.disp;
	    tempi->p1.type=TY_DISP+tempi->r.type&IT_MASK;
	    tempi->p1.ptype=tempi->r.ptype;
	    tempi->ic_opcode=EC_MOV;
	    if (tempi->p1.reg==CREG_RBP)
	      for (i=0;i<member_cnt;i++)
		if (mv[i].offset==tempi->p1.disp) {
		  mv[i].lea_balance--;
		  mv[i].score+=1+tempi->ic_indent_level;
		  break;
		}
	      } else if (tempil1->ic_opcode==EC_ADD_CONST) {
	    if (tempil2->ic_opcode==EC_REG) {
	      tempi->ic_flags|=tempil2->ic_flags|tempil1->ic_flags;
	      tempil2->ic_opcode=EC_NOP;
	      tempil1->ic_opcode=EC_NOP;
	      tempi->p1.reg=tempil2->p1.reg;
	      tempi->p1.disp=tempi->ic_data;
	      tempi->p1.type=TY_DISP+tempi->r.type&IT_MASK;
	      tempi->p1.ptype=tempi->r.ptype;
	      tempi->ic_opcode=EC_MOV;
	    }
	  }
	  break;
	case EC__PP:
	case EC__MM:
	case EC_PP_:
	case EC_MM_:
	  tempi->r.ptype=CmpPType(tempi->t.p1c);
	case EC_DEREF_PP:
	case EC_DEREF_MM:
	  if (tempi1->ic_opcode==EC_LEA &&
	      tempi1->p1.type&TY_MASK==TY_DISP &&
	      tempi1->p1.reg==CREG_RBP)
	    for (i=0;i<member_cnt;i++)
	      if (mv[i].offset==tempi1->p1.disp) {
		mv[i].lea_balance--;
		mv[i].score+=1+tempi->ic_indent_level;
		break;
	      }
	  tempi->p1.ptype=tempi->r.type&IT_MASK;
	  break;
	case EC_POWER:
	case EC_SHL:
	case EC_SHR:
	case EC_MOD:
	case EC_AND:
	case EC_OR:
	case EC_XOR:
	  break;

	case EC_MUL:
	case EC_DIV:
	  if (tempc->sub_type==IT_F64) {
	    CmpF2PushPop(tempi,tempi1,tempi2);
	    break;
	  }
	  break;
	case EC_ADD:
	  if (tempc->sub_type==IT_F64) {
	    CmpF2PushPop(tempi,tempi1,tempi2);
	    break;
	  }
	  if (OptFixupBinaryOp2(&tempi1,&tempi2)) {
	    tempi->ic_flags|=tempi1->ic_flags;
	    tempi1->ic_opcode=EC_NOP;
	    if (tempi->t.p1t!=tempi2)
	      tempi->t.p1c=tempi->t.p2c;
	    tempi->ic_data=tempi1->ic_data;
	    tempi->ic_opcode=EC_ADD_CONST;
	    tempi->p1.ptype=tempi->r.type&IT_MASK;
	    tempi->p2.type=TY_NULL;
	    if (tempil2->ic_opcode==EC_REG &&
		tempil2->p1.reg==CREG_RBP) {
	      tempi->ic_flags|=tempil2->ic_flags;
	      tempil2->ic_opcode=EC_NOP;
	      tempi->ic_opcode=EC_LEA;
	      tempi->p1.reg=CREG_RBP;
	      tempi->p1.type=TY_DISP+tempi->p1.type&IT_MASK;
	      tempi->p1.disp=tempi->ic_data;
	      for (i=0;i<member_cnt;i++)
		if (mv[i].offset==tempi->ic_data) {
		  mv[i].lea_balance++;
		  mv[i].score+=1+tempi->ic_indent_level;
		  break;
		}
	      }
	  }
	  break;
	case EC_SUB:
	  if (tempc->sub_type==IT_F64) {
	    CmpF2PushPop(tempi,tempi1,tempi2);
	    break;
	  }
	  if (tempi2->ic_opcode==EC_IMM) {
	    tempi->ic_flags|=tempi2->ic_flags;
	    tempi2->ic_opcode=EC_NOP;
	    tempi->ic_data=tempi2->ic_data;
	    tempi->ic_opcode=EC_SUB_CONST;
	    tempi->p2.type=TY_NULL;
	  }
	  break;
	case EC_LESS:
	case EC_GREATER:
	case EC_LESS_EQUAL:
	case EC_GREATER_EQUAL:
	  if (tempi->ic_flags&ICF_USE_F64)
	    CmpF2PushPop(tempi,tempi1,tempi2);
	  break;
	case EC_EQUAL_EQUAL:
	case EC_NOT_EQUAL:
	case EC_AND_AND:
	case EC_OR_OR:
	case EC_XOR_XOR:
	case EC_COLON:
	  break;

	case EC_MUL_EQUAL:
	case EC_DIV_EQUAL:
	case EC_ADD_EQUAL:
	case EC_SUB_EQUAL:
	  if (tempc->sub_type==IT_F64)
	    CmpF1PushPop(tempi,tempi2);
	case EC_ASSIGN_PP:
	case EC_ASSIGN_MM:
	case EC_ASSIGN:
	case EC_SHL_EQUAL:
	case EC_SHR_EQUAL:
	case EC_MOD_EQUAL:
	case EC_AND_EQUAL:
	case EC_OR_EQUAL:
	case EC_XOR_EQUAL:
	  if (tempi1->ic_opcode==EC_LEA &&
	      tempi1->p1.type&TY_MASK==TY_DISP &&
	      tempi1->p1.reg==CREG_RBP)
	    for (i=0;i<member_cnt;i++)
	      if (mv[i].offset==tempi1->p1.disp) {
		mv[i].lea_balance--;
		mv[i].score+=1+tempi->ic_indent_level;
		break;
	      }
	    tempi->p1.ptype=tempi->r.type&IT_MASK;
	  tempi->r.ptype=CmpPType(tempi->t.p1c);
	  break;
	case EC_RETURN_VAL:
	case EC_RETURN_VAL2:
	  if (tempc) {
	    if (tempc->sub_type==IT_F64 && tempil1->ic_class->sub_type!=IT_F64)
	      tempil1->ic_flags|=ICF_R_TO_F64;
	    else if (tempc->sub_type!=IT_F64 && tempil1->ic_class->sub_type==IT_F64)
	      tempil1->ic_flags|=ICF_R_TO_INT;
	  }
	  break;
	case EC_SHL_CONST:
	case EC_SHR_CONST:
	case EC_ENTER:
	case EC_ADD_RSP:
	case EC_ADD_RSP1:
	case EC_CALL:
	case EC_CALL_INDIRECT:
	case EC_CALL_INDIRECT2:
	case EC_CALL_EXTERN:
	case EC_CALL_IMPORT:
	case EC_CLI:
	case EC_STI:
	case EC_HLT:
	case EC_PUSHFD:
	case EC_POPFD:
	case EC_PUSH:
	case EC_POP:
	case EC_CLD:
	case EC_STD:
	case EC_WBINVD:
	case EC_INVD:
	case EC_INVLPG:
	case EC_CLFLUSH:
	case EC_PAUSE:
	case EC_GET_FLAGS:
	case EC_RDTSC:
	case EC_HPET:
	case EC_SET_FLAGS:
	case EC_GET_BASE_PTR:
	case EC_SET_BASE_PTR:
	case EC_GET_STK_PTR:
	case EC_GET_RAX:
	case EC_SET_STK_PTR:
	case EC_SET_RAX:
	case EC_ABS_ADDR:
	case EC_HEAP_GLBL:
	case EC_ADDR_IMPORT:
	case EC_TYPE:
	case EC_BT:
	case EC_BTS:
	case EC_BTR:
	case EC_BTC:
	case EC_BEQU:
	case EC_LBTS:
	case EC_LBTR:
	case EC_LBTC:
	case EC_LBEQU:
	case EC_BSF:
	case EC_BSR:
	case EC_PREEMPT:
	case EC_SIGN_I64:
	case EC_TOUPPER:
	case EC_TO_I64:
	case EC_TO_F64:
	case EC_TO_BOOL:
	case EC_ABS_I64:
	case EC_LIMIT_I64:
	case EC_LIMIT_U64:
	case EC_MIN_I64:
	case EC_MAX_I64:
	case EC_MIN_U64:
	case EC_MAX_U64:
	case EC_MOD_U64:
	case EC_SQR_I64:
	case EC_SQR_U64:
	case EC_SWAP_U8:
	case EC_SWAP_U16:
	case EC_SWAP_U32:
	case EC_SWAP_U64:
	case EC_INSQUE:
	case EC_INSQUE_REV:
	case EC_REMQUE:
	case EC_MEMCPY:
	case EC_MEMSET:
	case EC_MEMSET_U16:
	case EC_MEMSET_U32:
	case EC_MEMSET_U64:
	case EC_INU32:
	case EC_INU16:
	case EC_INU8:
	case EC_STRLEN:
	case EC_OUTU32:
	case EC_OUTU16:
	case EC_OUTU8:
	case EC_ASM:
	  break;
	case EC_NOBOUND_SWITCH:
	case EC_SWITCH:
	  lb=tempi->ic_data;
	  lb=lb->dft;
	  while (lb->fwd)
	    lb=lb->fwd;
	  lb->flags|=CBMF_USED;
	  break;
	case EC_BR_ZERO:
	case EC_BR_NOT_ZERO:
	  if (tempi1->ic_opcode==EC_IMM ||
	      tempi1->ic_opcode==EC_F64) {
	    tempi1->ic_opcode=EC_NOP;
	    tempi->p1.type=TY_NULL;
	    tempi1->r.type=TY_NULL;
	    if (code==EC_BR_ZERO ^^ tempi1->ic_data)
	      tempi->ic_opcode=EC_JMP;
	    else {
	      tempi->ic_opcode=EC_NOP;
	      tempi->ic_class=NULL;
	      break;
	    }
	  }
	  goto here1;
	case EC_BR_AND_ZERO:
	  if (tempi1->ic_opcode==EC_IMM) {
	    i=Bsr(tempi1->ic_data);
	    if (0<=i==Bsf(tempi1->ic_data)) {
	      tempi1->ic_data=i;
	      tempi->ic_flags|=ICF_NOT_ADDR;
	      tempi->ic_opcode=EC_BR_NOT_BT;
	      goto here1;
	    }
	  }
	  if (tempi2->ic_opcode==EC_IMM) {
	    i=Bsr(tempi2->ic_data);
	    if (0<=i==Bsf(tempi2->ic_data)) {
	      tempi2->ic_data=i;
	      tempi->ic_flags|=ICF_SWAP|ICF_NOT_ADDR;
	      tempi->ic_opcode=EC_BR_NOT_BT;
	    }
	  }
	  goto here1;
	case EC_BR_AND_NOT_ZERO:
	  if (tempi1->ic_opcode==EC_IMM) {
	    i=Bsr(tempi1->ic_data);
	    if (0<=i==Bsf(tempi1->ic_data)) {
	      tempi1->ic_data=i;
	      tempi->ic_flags|=ICF_NOT_ADDR;
	      tempi->ic_opcode=EC_BR_BT;
	      goto here1;
	    }
	  }
	  if (tempi2->ic_opcode==EC_IMM) {
	    i=Bsr(tempi2->ic_data);
	    if (0<=i==Bsf(tempi2->ic_data)) {
	      tempi2->ic_data=i;
	      tempi->ic_flags|=ICF_SWAP|ICF_NOT_ADDR;
	      tempi->ic_opcode=EC_BR_BT;
	    }
	  }
	  goto here1;
	case EC_BR_MM_ZERO:
	case EC_BR_MM_NOT_ZERO:
	  if (tempi1->ic_opcode==EC_LEA &&
	      tempi1->p1.type&TY_MASK==TY_DISP &&
	      tempi1->p1.reg==CREG_RBP)
	    for (i=0;i<member_cnt;i++)
	      if (mv[i].offset==tempi1->p1.disp) {
		mv[i].lea_balance--;
		mv[i].score+=1+tempi->ic_indent_level;
		break;
	      }
	  tempi->p1.ptype=CmpType(tempc);
	  goto here1;
	case EC_BR_GREATER_EQUAL:
	case EC_BR_LESS_EQUAL:
	case EC_BR_GREATER:
	case EC_BR_LESS:
	  if (tempi->ic_flags&ICF_USE_F64)
	    CmpF2PushPop(tempi,tempi1,tempi2);
	case EC_BR_GREATER_EQUAL2:
	case EC_BR_LESS_EQUAL2:
	case EC_BR_GREATER2:
	case EC_BR_LESS2:
	case EC_BR_NOT_EQUAL2:
	case EC_BR_EQUAL_EQUAL2:
	case EC_BR_NOT_EQUAL:
	case EC_BR_EQUAL_EQUAL:
	case EC_GET_LABEL:
	case EC_BR_BT:
	case EC_BR_BTS:
	case EC_BR_BTR:
	case EC_BR_BTC:
	case EC_BR_NOT_BT:
	case EC_BR_NOT_BTS:
	case EC_BR_NOT_BTR:
	case EC_BR_NOT_BTC:
	case EC_BR_LBTS:
	case EC_BR_LBTR:
	case EC_BR_LBTC:
	case EC_BR_NOT_LBTS:
	case EC_BR_NOT_LBTR:
	case EC_BR_NOT_LBTC:
	case EC_JMP:
here1:
	  lb=tempi->ic_data;
	  if (!(tempi->ic_flags&ICF_PUSH_CMP))
	    while (lb->fwd)
	      lb=lb->fwd;
	  lb->flags|=CBMF_USED;
	  break;
	case EC_LEAVE:
	case EC_RET:
	  break;
	case EC_ADDR:
	case EC_NOP:
	case EC_END:
	case EC_QUESTION:
	case EC_IRQ:
	case EC_END_EXP:
	case EC_CALL_END:
	case EC_CALL_END2:
	case EC_CALL_START:
	case EC_PUSH_REGS:
	case EC_POP_REGS:
	case EC_LABEL:
	  break;
	case EC_NOP1:
	  ps->ptr+=tempi->ic_data;
	  break;

	default:
	  OptMissing(lx,tempi);
      }
    }
    tempi++;
  }
  if (fun_class) {
    fun_class->used_reg_mask=fun_class->used_reg_mask&~(CREG_VARS_MASK|CREG_NON_PTR_MASK)|used_reg_mask;
    if (!Bt(&lx->opts,OPTf_NO_REG_VAR) &&
      !(lx->flags & LF_NO_REG_OPT)) {
      QSort(mv,member_cnt,sizeof(OptMemberVar),&OptMVCompare);
      while (member_cnt && !mv[member_cnt-1].score)
	member_cnt--;
      j=0;
      for (i=0;i<member_cnt;i++) {
	if (!mv[i].lea_balance && mv[i].offset) {//addr operator cancels
	  mv[j].m=mv[i].m;
	  mv[j++].offset=mv[i].offset;
	} else {
	  if (mv[i].m->register==CREG_ALLOC)
	    PrintFWarn("%S\n $$LK-A,\"FL:%s,%d\"$$'%s' in '%s'\n","ST_CANT_REG_VAR",
	      lx->cur_lfn->name,lx->cur_lfn->line_num,mv[i].m->str,fun_class->str);
	}
      }
      if (j>0) {
	if (j>num_reg_vars) {
	  l=0;
	  for (i=0;i<j && l<num_non_ptr_vars;i++) {
	    tempm=mv[i].m;
	    tempc=tempm->member_class;
	    while (tempc->fwd_class)
	      tempc=tempc->fwd_class;
	    if (!tempc->ptr_cnt && !tempm->dim.next) {
	      while (l<num_non_ptr_vars &&
		Bts(&fun_class->used_reg_mask,to_non_ptr_vars_map[l]))
		l++;
	      if (l<num_non_ptr_vars) {
		tempm->register=to_non_ptr_vars_map[l++];
		register_offsets[tempm->register].offset=mv[i].offset;
		register_offsets[tempm->register].m=tempm;
		if (Bt(&lx->flags,Lf_OPT_TRACE_PRESENT))
		  "Fun:%s\nReg %Z Var \"%s\" %08X[RBP]\n",
		    fun_class->str,tempm->register,"ST_U64_REGS",
		      tempm->str,register_offsets[tempm->register].offset;
		mv[i].offset=0; //flag as reg var
	      }
	    }
	  }
	}
	l=0;
	for (i=0;i<j && l<num_reg_vars;i++) {
	  //if not just flagged as reg var
	  if (mv[i].offset && !mv[i].m->dim.next) {
	    while (l<num_reg_vars &&
	      Bts(&fun_class->used_reg_mask,to_reg_vars_map[l]))
	      l++;
	    if (l<num_reg_vars) {
	      tempm=mv[i].m;
	      tempm->register=to_reg_vars_map[l++];
	      register_offsets[tempm->register].offset=mv[i].offset;
	      register_offsets[tempm->register].m=tempm;
	      if (Bt(&lx->flags,Lf_OPT_TRACE_PRESENT))
		"Reg %Z Var \"%s\" %08X[RBP]\n",tempm->register,"ST_U64_REGS",
		  tempm->str,register_offsets[tempm->register].offset;
	    }
	  }
	}
      }
    }
    Free(mv);
  }
  OptEnd(lx,ps);
}

