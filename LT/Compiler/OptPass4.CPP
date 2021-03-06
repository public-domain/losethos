BoolI64 OptIc4(IntermediateCode *tempi)
{
  I64 i,ty,pt,ty1,pt1;
  IntermediateCode *tempil1,*tempil2;

  if (tempi->ic_opcode<EC_IMM) return FALSE;

  tempil1=tempi;
  if (!(tempil2=OptLag1(tempil1)))
    return FALSE;

  if (tempil2->r.type&TY_MASK==TY_STK &&
      !(tempil2->ic_flags&ICF_PUSH_RESULT)) {
    if (tempil1->ic_opcode==EC_ADD_CONST &&
	tempil1->p1.type&TY_MASK==TY_STK) {
      if ((tempil2->ic_opcode==EC_REG ||
	  tempil2->ic_opcode==EC_MOV) &&
	  tempil2->p1.type&TY_MASK==TY_REG) {
	i=tempil1->ic_data;
	    if (MIN_I32<=i<=MAX_I32 &&
	    !Bt(&non_ptr_vars_mask,tempil2->p1.reg)) {
	  tempil1->ic_flags|=tempil2->ic_flags&ICF_NO_CVT_MASK;
	  tempil2->ic_opcode=EC_NOP;
	  tempil2->p1.type=TY_NULL+tempil2->p1.type&IT_MASK;
	  tempil2->r.type=TY_NULL+tempil2->r.type&IT_MASK;
	  tempil1->ic_opcode=EC_LEA;
	  tempil1->p1.type=TY_DISP+tempil1->p1.type&IT_MASK;
	  tempil1->p1.reg=tempil2->p1.reg;
	  tempil1->p1.disp=i;
	  return TRUE;
	}
      } else if (tempil2->ic_opcode==EC_SHL_CONST &&
	  tempil2->p1.type&TY_MASK==TY_REG) {
	i=tempil1->ic_data;
	    if (MIN_I32<=i<=MAX_I32 &&
	    tempil2->p1.reg!=CREG_RSP &&
	    1<=tempil2->ic_data<=3) {
	  tempil1->ic_flags|=tempil2->ic_flags&ICF_NO_CVT_MASK;
	  tempil2->ic_opcode=EC_NOP;
	  tempil2->p1.type=TY_NULL+tempil2->p1.type&IT_MASK;
	  tempil2->r.type=TY_NULL+tempil2->r.type&IT_MASK;
	  tempil1->ic_opcode=EC_LEA;
	  tempil1->p1.type=TY_SIB+tempil1->p1.type&IT_MASK;
	  tempil1->p1.reg=tempil2->p1.reg<<8+CREG_NONE;
	  if (tempil2->ic_data==1)
	    tempil1->p1.reg|=0x4000;
	  else if (tempil2->ic_data==2)
	    tempil1->p1.reg|=0x8000;
	  else
	    tempil1->p1.reg|=0xC000;
	  tempil1->p1.disp=i;
	  return TRUE;
	}
      }
    }
    if (tempil2->ic_opcode==EC_MOV ||
	tempil2->ic_opcode==EC_REG) {
      if (tempil1->p3.type&TY_MASK==TY_STK) {
	if (tempil2->ic_flags & ICF_R_TO_INT) {
	  if (tempil2->p1.type&TY_MASK==TY_IMM)
	    tempil2->p1.disp=tempil2->p1.disp(F64);
	  else
	    tempil1->ic_flags|=ICF_P3_TO_INT;
	} else if (tempil2->ic_flags&ICF_R_TO_F64) {
	  if (tempil2->p1.type&TY_MASK==TY_IMM)
	    tempil2->p1.disp(F64)=tempil2->p1.disp;
	  else
	    tempil1->ic_flags|=ICF_P3_TO_F64;
	}

	ty=tempil1->p3.type&IT_MASK;
	ty1=tempil2->p1.type&IT_MASK;
	if (ty1 && ty1<ty)
	  ty=ty1;
	ty1=tempil2->r.type&IT_MASK;
	if (ty1 && ty1<ty)
	  ty=ty1;
	tempil1->p3.type=tempil2->p1.type&TY_MASK+ty;

	pt=tempil1->p3.ptype;
	pt1=tempil2->p1.ptype;
	if (pt1 && pt1<pt && pt!=IT_F64)
	  pt=pt1;
	pt1=tempil2->r.ptype;
	if (pt1 && pt1<pt && pt!=IT_F64)
	  pt=pt1;
	tempil1->p3.ptype=pt;

	tempil1->p3.reg=tempil2->p1.reg;
	tempil1->p3.disp=tempil2->p1.disp;
	tempil1->ic_flags|=tempil2->ic_flags&ICF_NO_CVT_MASK;
	tempil2->ic_opcode=EC_NOP1;
	tempil2->ic_data=1;
	tempil2->p1.type=TY_NULL+tempil2->p1.type&IT_MASK;
	tempil2->r.type=TY_NULL+tempil2->r.type&IT_MASK;
	return TRUE;
      }
      if (tempil1->p2.type&TY_MASK==TY_STK) {
	if (tempil2->ic_flags & ICF_R_TO_INT) {
	  if (tempil2->p1.type&TY_MASK==TY_IMM)
	    tempil2->p1.disp=tempil2->p1.disp(F64);
	  else
	    tempil1->ic_flags|=ICF_P2_TO_INT;
	} else if (tempil2->ic_flags&ICF_R_TO_F64) {
	  if (tempil2->p1.type&TY_MASK==TY_IMM)
	    tempil2->p1.disp(F64)=tempil2->p1.disp;
	  else
	    tempil1->ic_flags|=ICF_P2_TO_F64;
	}

	ty=tempil1->p2.type&IT_MASK;
	ty1=tempil2->p1.type&IT_MASK;
	if (ty1 && ty1<ty)
	  ty=ty1;
	ty1=tempil2->r.type&IT_MASK;
	if (ty1 && ty1<ty)
	  ty=ty1;
	tempil1->p2.type=tempil2->p1.type&TY_MASK+ty;

	pt=tempil1->p2.ptype;
	pt1=tempil2->p1.ptype;
	if (pt1 && pt1<pt && pt!=IT_F64)
	  pt=pt1;
	pt1=tempil2->r.ptype;
	if (pt1 && pt1<pt && pt!=IT_F64)
	  pt=pt1;
	tempil1->p2.ptype=pt;

	tempil1->p2.reg=tempil2->p1.reg;
	tempil1->p2.disp=tempil2->p1.disp;
	tempil1->ic_flags|=tempil2->ic_flags&ICF_NO_CVT_MASK;
	tempil2->ic_opcode=EC_NOP1;
	tempil2->ic_data=1;
	tempil2->p1.type=TY_NULL+tempil2->p1.type&IT_MASK;
	tempil2->r.type=TY_NULL+tempil2->r.type&IT_MASK;
	return TRUE;
      }
      if (tempil1->p1.type&TY_MASK==TY_STK) {
	if (tempil2->ic_flags & ICF_R_TO_INT) {
	  if (tempil2->p1.type&TY_MASK==TY_IMM)
	    tempil2->p1.disp=tempil2->p1.disp(F64);
	  else
	    tempil1->ic_flags|=ICF_P1_TO_INT;
	} else if (tempil2->ic_flags&ICF_R_TO_F64) {
	  if (tempil2->p1.type&TY_MASK==TY_IMM) {
	    if (tempil2->p1.type&1) //signed/unsigned
	      tempil2->p1.disp(F64)=tempil2->p1.disp(U64);
	    else
	      tempil2->p1.disp(F64)=tempil2->p1.disp(I64);
	  } else
	    tempil1->ic_flags|=ICF_P1_TO_F64;
	}

	ty=tempil1->p1.type&IT_MASK;
	ty1=tempil2->p1.type&IT_MASK;
	if (ty1 && ty1<ty)
	  ty=ty1;
	ty1=tempil2->r.type&IT_MASK;
	if (ty1 && ty1<ty)
	  ty=ty1;
	tempil1->p1.type=tempil2->p1.type&TY_MASK+ty;

	pt=tempil1->p1.ptype;
	pt1=tempil2->p1.ptype;
	if (pt1 && pt1<pt && pt!=IT_F64)
	  pt=pt1;
	pt1=tempil2->r.ptype;
	if (pt1 && pt1<pt && pt!=IT_F64)
	  pt=pt1;
	tempil1->p1.ptype=pt;

	tempil1->p1.reg=tempil2->p1.reg;
	tempil1->p1.disp=tempil2->p1.disp;
	tempil1->ic_flags|=tempil2->ic_flags&ICF_NO_CVT_MASK;
	tempil2->ic_opcode=EC_NOP1;
	tempil2->ic_data=1;
	tempil2->p1.type=TY_NULL+tempil2->p1.type&IT_MASK;
	tempil2->r.type=TY_NULL+tempil2->r.type&IT_MASK;
	return TRUE;
      }
    }
    if (tempil1->ic_opcode==EC_DEREF) {
      if (tempil2->ic_opcode==EC_ADD_CONST &&
	  tempil2->p1.type&TY_MASK==TY_REG &&
	  tempil1->p1.type&TY_MASK==TY_STK) {
	i=tempil2->ic_data;
	    if (MIN_I32<=i<=MAX_I32 &&
	    !Bt(&non_ptr_vars_mask,tempil2->p1.reg)) {
	  tempil1->ic_flags|=tempil2->ic_flags;
	  tempil2->ic_opcode=EC_NOP1;
	  tempil2->ic_data=-1;
	  tempil2->p1.type=TY_NULL+tempil2->p1.type&IT_MASK;
	  tempil2->r.type=TY_NULL+tempil2->r.type&IT_MASK;
	  tempil1->ic_opcode=EC_MOV;
	  tempil1->p1.type=TY_DISP+tempil1->p1.ptype;
	  tempil1->p1.reg=tempil2->p1.reg;
	  tempil1->p1.disp=i;
	  return TRUE;
	}
      }
      if (tempil2->ic_opcode==EC_LEA &&
	  tempil1->p1.type&TY_MASK==TY_STK) {
	tempil1->ic_flags|=tempil2->ic_flags;
	tempil2->ic_opcode=EC_NOP;
	tempil1->ic_opcode=EC_MOV;
	tempil1->p1.type=tempil2->p1.type&TY_MASK+tempil1->p1.ptype;
	tempil1->p1.reg=tempil2->p1.reg;
	tempil1->p1.disp=tempil2->p1.disp;
	return TRUE;
      }
    }
  }
  if (tempil1->ic_opcode==EC_DEREF) {
    if (tempil1->p1.type&TY_MASK==TY_REG) {
      tempil1->p1.type=TY_DISP+tempil1->p1.ptype;
      tempil1->p1.disp=0;
      tempil1->ic_opcode=EC_MOV;
      return TRUE;
    }
  }
  return FALSE;
}

U0 OptPass4(LexStruct *lx,OptRegStruct *register_offsets,U64 *_type)
{
  ClassStruct *tempc,*tempc1,*tempc2,*tempc3;
  IntermediateCode *tempi,*tempi1,*tempi2,*tempi3,*tempil1,*tempil2,*tempil3,
  *temp_lag[4];
  I64 code,i;
  PrsStk *ps=OptStart(lx,temp_lag);
  BoolI8 dead_code=FALSE;
  CbMiscStruct *lb;

  if (_type)
    *_type=IT_I64;

  tempi=lx->cbh.cb_in_ptr;
  if (tempi->ic_opcode==EC_START)
    tempi++;
  while (code=tempi->ic_opcode) {
    OptNext2(tempi,temp_lag);
    if (dead_code&&code!=EC_LABEL)
      tempi->ic_opcode=EC_NOP;
    else {
      tempc=tempi->ic_class;

      tempi3=&opt_nop;
      tempi2=&opt_nop;
      tempi1=&opt_nop;

      tempil1=temp_lag[1];
      tempil2=temp_lag[2];
      tempil3=temp_lag[3];
      nobound_switch (cmp_arg_cnt[code]) {
	case 9:
	  ps->ptr-=tempi->ic_data>>3;
	  break;
	case 3:
	  tempi3=PrsPop(ps);
	  tempc3=tempi3->ic_class;
	case 2:
	  tempi2=PrsPop(ps);
	  tempc2=tempi2->ic_class;
	case 1:
	  tempi1=PrsPop(ps);
	  tempc1=tempi1->ic_class;
	  break;
	case 0: //nobound_switch
	  break;
      }
      nobound_switch (code) {
	case EC_IMM:
	case EC_TYPE:
	  tempi->p1.type=TY_IMM+IT_I64;
	  tempi->p1.disp=tempi->ic_data;
	  tempi->ic_opcode=EC_MOV;
	  break;
	case EC_F64:
	  tempi->p1.type=TY_IMM+IT_I64;
	  tempi->p1.disp=tempi->ic_data;
	  tempi->p1.ptype=IT_F64;
	  tempi->ic_opcode=EC_MOV;
	  break;
	case EC_STR_CONST:
	case EC_FS:
	case EC_GS:
	case EC_MOV_FS:
	case EC_MOV_GS:
	case EC_RIP:
	case EC_RBP:
	case EC_REG:
	case EC_COM:
	case EC_LT_TYPECAST:
	case EC_NOT:
	case EC_UNARY_MINUS:
	case EC_PUSH_CMP:
	  break;
	case EC_MOV:
	  if (tempi->p1.type&TY_MASK==TY_DISP &&
	      tempi->p1.reg==CREG_RBP) {
	    i=CmpOffset2Reg(tempi->p1.disp,register_offsets);
	    if (i>=0) {
	      tempi->p1.type=TY_REG+tempi->p1.type&IT_MASK;
	      tempi->p1.reg=i;
	      tempi->p1.disp=0;
	    }
	  }
	  break;
	case EC_DEREF:
	  if (tempi1->ic_opcode==EC_LEA) {
	    if (tempi1->p1.type&TY_MASK==TY_DISP &&
		tempi1->p1.reg==CREG_RBP) {
	      i=CmpOffset2Reg(tempi1->p1.disp,register_offsets);
	      if (i>=0) {
		tempi->ic_flags|=tempi1->ic_flags;
		tempi1->ic_opcode=EC_NOP;
		tempi->ic_opcode=EC_REG;
		tempi->p1.type=TY_REG+tempi->p1.type&IT_MASK;
		tempi->p1.reg=i;
		tempi->p1.disp=0;
	      }
	    }
	  } else if ((tempi1->ic_opcode==EC_ABS_ADDR ||
	      tempi1->ic_opcode==EC_MOV &&
	      tempi1->p1.type==TY_IMM+IT_I64 &&
	      0<=tempi1->p1.disp<=MAX_I32) &&
	      !(tempi1->ic_flags&ICF_NO_RIP)) {
	    if (tempi1->ic_opcode==EC_ABS_ADDR)
	      tempi->p1.disp=tempi1->ic_data;
	    else
	      tempi->p1.disp=tempi1->p1.disp;
	    tempi->ic_flags|=tempi1->ic_flags;
	    tempi1->ic_opcode=EC_NOP;
	    tempi1->p1.type=TY_NULL+tempi1->p1.type&IT_MASK;
	    tempi1->r.type=TY_NULL+tempi1->r.type&IT_MASK;
	    tempi->ic_opcode=EC_MOV;
	    tempi->p1.type=TY_RIP_DISP32+tempi->p1.ptype;
	    tempi->p1.reg=CREG_RIP;
	  }
	  break;
	case EC_BR_MM_ZERO:
	case EC_BR_MM_NOT_ZERO:
	//(branch ++ to zero is unlikely)
	case EC_DEREF_PP:
	case EC_DEREF_MM:
	case EC__PP:
	case EC__MM:
	case EC_PP_:
	case EC_MM_:
	  if (tempi1->ic_opcode==EC_LEA) {
	    if (tempi1->p1.type&TY_MASK==TY_DISP &&
		tempi1->p1.reg==CREG_RBP) {
	      i=CmpOffset2Reg(tempi1->p1.disp,register_offsets);
	      if (i>=0) {
		tempi->ic_flags|=tempi1->ic_flags;
		tempi1->ic_opcode=EC_NOP1;
		tempi1->ic_data=1;
		tempi1->r.type=TY_NULL;
		tempi->p1.type=TY_REG+tempi->p1.type&IT_MASK;
		tempi->p1.reg=i;
		tempi->p1.disp=0;
		tempi->ic_flags|=ICF_NOT_ADDR;
	      } else
		goto lea_gone;
	    } else {
	      lea_gone:
	      tempi->ic_flags|=tempi1->ic_flags;
	      tempi1->ic_opcode=EC_NOP1;
	      tempi1->ic_data=1;
	      tempi1->r.type=TY_NULL;
	      tempi->p1.type=tempi1->p1.type;
	      tempi->p1.reg=tempi1->p1.reg;
	      tempi->p1.disp=tempi1->p1.disp;
	      tempi->ic_flags|=ICF_NOT_ADDR;
	    }
	  } else if ((tempi1->ic_opcode==EC_ABS_ADDR ||
	      tempi1->ic_opcode==EC_MOV &&
	      tempi1->p1.type==TY_IMM+IT_I64 &&
	      0<=tempi1->p1.disp<=MAX_I32) &&
	      !(tempi1->ic_flags&ICF_NO_RIP)) {
	    tempi->ic_flags|=tempi1->ic_flags;
	    if (tempi1->ic_opcode==EC_ABS_ADDR)
	      tempi->p1.disp=tempi1->ic_data;
	    else
	      tempi->p1.disp=tempi1->p1.disp;
	    tempi1->ic_opcode=EC_NOP;
	    tempi1->p1.type=TY_NULL+tempi1->p1.type&IT_MASK;
	    tempi1->r.type=TY_NULL+tempi1->r.type&IT_MASK;
	    tempi->p1.type=TY_RIP_DISP32+tempi->p1.ptype;
	    tempi->p1.reg=CREG_RIP;
	    tempi->ic_flags|=ICF_NOT_ADDR;
	  }
	  break;
	case EC_ADD:
	  if (tempi1->ic_opcode==EC_MOV && tempi1->p1.type==TY_REG+IT_I64 ||
	      tempi1->ic_opcode==EC_REG) {
	    if (tempi2->ic_opcode==EC_MOV && tempi2->p1.type==TY_REG+IT_I64 ||
		tempi2->ic_opcode==EC_REG) {
	      if (tempi2->p1.reg!=CREG_RSP) {
		tempi->p1.disp=0;
		tempi->p1.reg=tempi1->p1.reg+tempi2->p1.reg<<8;
		goto sib;
	      } else if (tempi1->p1.reg!=CREG_RSP) {
		tempi->p1.disp=0;
		tempi->p1.reg=tempi2->p1.reg+tempi1->p1.reg<<8;
sib:
		tempi->ic_flags|=tempi1->ic_flags&ICF_NO_CVT_MASK;
		tempi1->ic_opcode=EC_NOP1;
		tempi1->ic_data=1;
		tempi1->p1.type=TY_NULL+tempi1->p1.type&IT_MASK;
		tempi1->r.type=TY_NULL+tempi1->r.type&IT_MASK;

		tempi->ic_flags|=tempi2->ic_flags&ICF_NO_CVT_MASK;
		tempi2->ic_opcode=EC_NOP;

		tempi->ic_opcode=EC_LEA;
		tempi->p1.type=TY_SIB+IT_I64;
		tempi->p1.ptype=IT_I64;

		tempi->p2.type=TY_NULL+tempi->p2.type&IT_MASK;
	      }
	    } else if (tempi2->ic_opcode==EC_SHL_CONST && tempi2->p1.type==TY_REG+IT_I64 &&
		tempi2->ic_data<=3) {
	      if (tempi2->p1.reg!=CREG_RSP) {
		tempi->p1.disp=0;
		tempi->p1.reg=tempi1->p1.reg+tempi2->p1.reg<<8;
		if (tempi2->ic_data==1)
		  tempi->p1.reg|=0x4000;
		else if (tempi2->ic_data==2)
		  tempi->p1.reg|=0x8000;
		else
		  tempi->p1.reg|=0xC000;
		goto sib;
	      }
	    }
	  } else if (tempi1->ic_opcode==EC_LEA && tempi1->p1.type&TY_MASK==TY_DISP) {
	    if (tempi1->p1.reg==CREG_RBP && CmpOffset2Reg(tempi1->p1.disp,register_offsets)>=0)
	      break;
	    if (tempi2->ic_opcode==EC_MOV && tempi2->p1.type==TY_REG+IT_I64 ||
		tempi2->ic_opcode==EC_REG) {
	      if (tempi2->p1.reg!=CREG_RSP) {
		tempi->p1.disp=tempi1->p1.disp;
		tempi->p1.reg=tempi1->p1.reg+tempi2->p1.reg<<8;
		goto sib;
	      } else if (tempi1->p1.reg!=CREG_RSP) {
		tempi->p1.disp=tempi1->p1.disp;
		tempi->p1.reg=tempi2->p1.reg+tempi1->p1.reg<<8;
		goto sib;
	      }
	    } else if (tempi2->ic_opcode==EC_SHL_CONST && tempi2->p1.type==TY_REG+IT_I64 &&
		tempi2->ic_data<=3) {
	      if (tempi2->p1.reg!=CREG_RSP) {
		tempi->p1.disp=tempi1->p1.disp;
		tempi->p1.reg=tempi1->p1.reg+tempi2->p1.reg<<8;
		if (tempi2->ic_data==1)
		  tempi->p1.reg|=0x4000;
		else if (tempi2->ic_data==2)
		  tempi->p1.reg|=0x8000;
		else
		  tempi->p1.reg|=0xC000;
		goto sib;
	      }
	    }
	  }
	  break;
	case EC_SHL_CONST:
	case EC_LEA:
	case EC_SHR_CONST:
	case EC_POWER:
	case EC_SHL:
	case EC_SHR:
	case EC_MUL:
	case EC_DIV:
	case EC_MOD:
	case EC_AND:
	case EC_OR:
	case EC_XOR:
	case EC_SUB:
	case EC_EQUAL_EQUAL:
	case EC_NOT_EQUAL:
	case EC_LESS:
	case EC_GREATER:
	case EC_LESS_EQUAL:
	case EC_GREATER_EQUAL:
	case EC_AND_AND:
	case EC_OR_OR:
	case EC_XOR_XOR:
	case EC_COLON:
	  break;
	case EC_ASSIGN_PP:
	case EC_ASSIGN_MM:
	//this val was stashed during pass012 for pointer arithmetic
	  tempi->ic_class2=tempi->t.p3c;
	case EC_ASSIGN:
	case EC_SHL_EQUAL:
	case EC_SHR_EQUAL:
	case EC_MUL_EQUAL:
	case EC_DIV_EQUAL:
	case EC_MOD_EQUAL:
	case EC_AND_EQUAL:
	case EC_OR_EQUAL:
	case EC_XOR_EQUAL:
	case EC_ADD_EQUAL:
	case EC_SUB_EQUAL:
	  if (tempi1->ic_opcode==EC_LEA) {
	    if (TY_DISP<=tempi1->p1.type&TY_MASK<=TY_SIB) {
	      if (tempi1->p1.type&TY_MASK==TY_DISP && tempi1->p1.reg==CREG_RBP) {
		i=CmpOffset2Reg(tempi1->p1.disp,register_offsets);
		if (i>=0) {
		  tempi->ic_flags|=tempi1->ic_flags;
		  tempi1->ic_opcode=EC_NOP1;
		  tempi1->ic_data=1;
		  tempi1->p1.type=TY_NULL;
		  tempi1->r.type=TY_NULL;
		  tempi->p1.type=TY_REG+tempi->p1.type&IT_MASK;
		  tempi->p1.reg=i;
		  tempi->p1.disp=0;
		} else {
		  tempi->ic_flags|=tempi1->ic_flags;
		  tempi1->ic_opcode=EC_NOP1;
		  tempi1->ic_data=1;
		  tempi1->r.type=TY_NULL;
		  tempi1->p1.type=TY_NULL;
		  tempi->p1.type=TY_DISP+tempi->p1.type&IT_MASK;
		  tempi->p1.reg=CREG_RBP;
		  tempi->p1.disp=tempi1->p1.disp;
		}
	      } else {
		tempi->p1.type=tempi1->p1.type&TY_MASK+tempi->p1.type&IT_MASK;
		tempi->ic_flags|=tempi1->ic_flags;
		tempi1->ic_opcode=EC_NOP1;
		tempi1->ic_data=1;
		tempi1->r.type=TY_NULL;
		tempi1->p1.type=TY_NULL;
		tempi->p1.reg=tempi1->p1.reg;
		tempi->p1.disp=tempi1->p1.disp;
	      }
	      tempi->ic_flags|=ICF_NOT_ADDR;
	    }
	  } else if ((tempi1->ic_opcode==EC_ABS_ADDR ||
	      tempi1->ic_opcode==EC_MOV &&
	      tempi1->p1.type==TY_IMM+IT_I64 &&
	      0<=tempi1->p1.disp<=MAX_I32) &&
	      !(tempi1->ic_flags&ICF_NO_RIP)) {
	    tempi->ic_flags|=tempi1->ic_flags;
	    if (tempi1->ic_opcode==EC_ABS_ADDR)
	      tempi->p1.disp=tempi1->ic_data;
	    else
	      tempi->p1.disp=tempi1->p1.disp;
	    tempi1->ic_opcode=EC_NOP1;
	    tempi1->ic_data=1;
	    tempi1->p1.type=TY_NULL+tempi1->p1.type&IT_MASK;
	    tempi1->r.type=TY_NULL+tempi1->r.type&IT_MASK;
	    tempi1->r.type=TY_NULL;
	    tempi->p1.type=TY_RIP_DISP32+tempi->p1.type&IT_MASK;
	    tempi->p1.reg=CREG_RIP;
	    tempi->ic_flags|=ICF_NOT_ADDR;
	  }
	  break;
	case EC_ADD_CONST:
	case EC_SUB_CONST:
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
	  break;
	case EC_RETURN_VAL:
	case EC_RETURN_VAL2:
	  if (!tempi->ic_class) {
	    if (_type) {
	      tempil1=tempi;
	      while (tempil1=OptLag1(tempil1))
		if (tempil1->ic_class) {
		  if (tempil1->ic_flags & ICF_R_TO_F64)
		    *_type=IT_F64;
		  else if (tempil1->ic_flags & ICF_R_TO_INT)
		    *_type=IT_I64;
		  else
		    *_type=tempil1->ic_class->sub_type;
		  break;
		}
	    }
	    tempi->ic_class=sys_internal_types[IT_I64];
	  } else if (_type)
	    *_type=tempi->ic_class->sub_type;
	  break;
	case EC_GET_LABEL:
	case EC_ABS_ADDR:
	case EC_HEAP_GLBL:
	case EC_ADDR_IMPORT:
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
	case EC_NOBOUND_SWITCH:
	case EC_SWITCH:
	case EC_RET:
	case EC_END:
	case EC_ADDR:
	case EC_QUESTION:
	case EC_IRQ:
	case EC_NOP:
	case EC_CALL_START:
	case EC_LEAVE:
	case EC_PUSH_REGS:
	case EC_POP_REGS:
	case EC_ASM:
	case EC_BR_AND_NOT_ZERO:
	case EC_BR_AND_ZERO:
	  break;
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
	case EC_BT:
	case EC_BTS:
	case EC_BTR:
	case EC_BTC:
	case EC_LBTS:
	case EC_LBTR:
	case EC_LBTC:
	  if (tempi2->ic_opcode==EC_ADDR &&
	      !(tempi->ic_flags&ICF_NOT_ADDR)) {
	    if (tempi2->p1.type&TY_MASK==TY_STK &&
		tempi2->r.type&TY_MASK==TY_STK) {
	      if (!(tempil2=OptLag1(tempi2)) ||
		   tempil2->ic_opcode!=EC_ABS_ADDR &&
		   !(tempil2->ic_opcode==EC_MOV &&
		     tempil2->p1.type==TY_IMM+IT_I64 &&
		     0<=tempil2->p1.disp<=MAX_I32) ||
		     tempil2->ic_flags&ICF_NO_RIP)
		tempil2=NULL;
	      else {
		if (tempil2->ic_opcode==EC_ABS_ADDR)
		  tempi->p2.disp=tempil2->ic_data;
		else
		  tempi->p2.disp=tempil2->p1.disp;
	      }
	    } else {
	      if (tempi2->p1.type==TY_IMM+IT_I64 &&
		  0<=tempi2->p1.disp<=MAX_I32 &&
		  !(tempi2->ic_flags&ICF_NO_RIP)) {
		tempil2=tempi2;
		tempi2=NULL;
		tempi->p2.disp=tempil2->p1.disp;
	      } else
		tempil2=NULL;
	    }
	    if (tempil2) {
	      if (tempi2) {
		tempi->ic_flags|=tempi2->ic_flags;
		tempi2->ic_opcode=EC_NOP;
		tempi2->ic_flags=0;
		tempi2 ->p1.type=TY_NULL+tempi2 ->p1.type&IT_MASK;
		tempi2 ->r .type=TY_NULL+tempi2 ->r .type&IT_MASK;
	      }
	      tempi->ic_flags|=tempil2->ic_flags|ICF_NOT_ADDR;
	      tempil2->ic_opcode=EC_NOP;
	      tempil2->ic_flags=0;
	      tempil2->p1.type=TY_NULL+tempil2->p1.type&IT_MASK;
	      tempil2->r .type=TY_NULL+tempil2->r .type&IT_MASK;
	      tempi->p2.type=TY_RIP_DISP32+tempi->p2.type&IT_MASK;
	      tempi->p2.reg=CREG_RIP;
	    }
	  }
	  break;
	case EC_BEQU:
	case EC_LBEQU:
	  if (tempi3->ic_opcode==EC_ADDR &&
	      !(tempi->ic_flags&ICF_NOT_ADDR)) {
	    if (tempi3->p1.type&TY_MASK==TY_STK &&
		tempi3->r.type&TY_MASK==TY_STK) {
	      if (!(tempil3=OptLag1(tempi3)) ||
		   tempil3->ic_opcode!=EC_ABS_ADDR &&
		   !(tempil3->ic_opcode==EC_MOV &&
		     tempil3->p1.type==TY_IMM+IT_I64 &&
		     0<=tempil3->p1.disp<=MAX_I32) ||
		     tempil3->ic_flags&ICF_NO_RIP)
		tempil3=NULL;
	      else {
		if (tempil3->ic_opcode==EC_ABS_ADDR)
		  tempi->p3.disp=tempil3->ic_data;
		else
		  tempi->p3.disp=tempil3->p1.disp;
	      }
	    } else {
	      if (tempi3->p1.type==TY_IMM+IT_I64 &&
		  0<=tempi3->p1.disp<=MAX_I32 &&
		  !(tempi3->ic_flags&ICF_NO_RIP)) {
		tempil3=tempi3;
		tempi3=NULL;
		tempi->p3.disp=tempil3->p1.disp;
	      } else
		tempil3=NULL;
	    }
	    if (tempil3) {
	      if (tempi3) {
		tempi->ic_flags|=tempi3->ic_flags;
		tempi3->ic_opcode=EC_NOP;
		tempi3->ic_flags=0;
		tempi3 ->p1.type=TY_NULL+tempi3 ->p1.type&IT_MASK;
		tempi3 ->r .type=TY_NULL+tempi3 ->r .type&IT_MASK;
	      }
	      tempi->ic_flags|=tempil3->ic_flags|ICF_NOT_ADDR;
	      tempil3->ic_opcode=EC_NOP;
	      tempil3->ic_flags=0;
	      tempil3->p1.type=TY_NULL+tempil3->p1.type&IT_MASK;
	      tempil3->r .type=TY_NULL+tempil3->r .type&IT_MASK;
	      tempi->p3.type=TY_RIP_DISP32+tempi->p3.type&IT_MASK;
	      tempi->p3.reg=CREG_RIP;
	    }
	  }
	  break;
	case EC_BR_NOT_EQUAL2:
	case EC_BR_EQUAL_EQUAL2:
	case EC_BR_GREATER_EQUAL2:
	case EC_BR_LESS_EQUAL2:
	case EC_BR_GREATER2:
	case EC_BR_LESS2:

	case EC_BR_ZERO:
	case EC_BR_NOT_ZERO:
	case EC_BR_NOT_EQUAL:
	case EC_BR_EQUAL_EQUAL:
	case EC_BR_GREATER_EQUAL:
	case EC_BR_LESS_EQUAL:
	case EC_BR_GREATER:
	case EC_BR_LESS:
	  lb=tempi->ic_data;
	  if (tempi->ic_flags&ICF_PUSH_CMP) {
	    lb->flags|=CBMF_POP_CMP;
	    lb->fwd=NULL;
	  }
	  break;
	case EC_LABEL:
	  lb=tempi->ic_data;
	  if (lb->flags&CBMF_USED)
	    dead_code=FALSE;
	  break;
	case EC_JMP:
	  dead_code=TRUE;
	  break;
	case EC_NOP1:
	  ps->ptr+=tempi->ic_data;
	  break;
	case EC_CALL_END2:
	  break;
	case EC_CALL_END:
	case EC_END_EXP:
	  if (tempi->p1.type&TY_MASK==TY_STK) {
	    if (tempil1->r.type&TY_MASK==TY_STK &&
		!(tempil1->ic_flags&ICF_PUSH_RESULT)) {
	      tempi->p1.type=TY_REG+tempi->p1.type&IT_MASK;
	      tempi->p1.reg=CREG_RAX;
	      tempi->p1.disp=0;
	      tempil1->r.type=TY_REG+tempil1->r.type&IT_MASK;
	      tempil1->r.reg=CREG_RAX;
	      tempil1->r.disp=0;
	    }
	  }
	  break;
	default:
	  OptMissing(lx,tempi);
      }

      while (OptIc4(tempi));
      code=tempi->ic_opcode;
      if (cmp_result_cnt[code])
	PrsPush(ps,tempi);
    }
    tempi++;
  }
  OptEnd(lx,ps);
}
