#define NUM_INTERNAL_TYPES	16

class InternalType
{
  U8 type,size,name[8];
};

InternalType internal_types_table[NUM_INTERNAL_TYPES]={
{IT_I0,0,"I0i"},
{IT_U0,0,"U0i"},
{IT_I0,0,"I0"},
{IT_U0,0,"U0"},
{IT_I8,1,"I8i"},
{IT_I8,1,"I8"},
{IT_U8,1,"U8i"},
{IT_U8,1,"U8"},
{IT_I16,2,"I16i"},
{IT_U16,2,"U16i"},
{IT_I32,4,"I32i"},
{IT_U32,4,"U32i"},
{IT_I64,8,"I64i"},
{IT_U64,8,"U64i"},
{IT_F64,8,"F64i"},
{IT_F64,8,"F64"},
};

U0 AsmHashLoad(BoolI8 first)
{
  U64 i,j;
  InternalType *tempit;
  LexStruct *lx;
  I64 size,size1;
  GenericHashEntry *temph;
  OpcodeHashEntry *tempo,*tempo1;
  InstructionEntry *tempi;
  ClassStruct *tempc;

  asm_hash=HashTableNew(2048);
  size1=offset(OpcodeHashEntry.ins)+sizeof(InstructionEntry)<<5;
  tempo=MAlloc(size1);

  lx=LexNew(TextFileRead("OpCodes.TXZ"),0,StrNew("OpCodes.TXZ"));
  lx->hash_table_list=NULL;
  Lex(lx);
  while (lx->token) {
    if (lx->token!=TK_IDENT)
      PrsExcept(lx,"ST_EXPECTING_IDENTIFIER");
    i=ListEntryMatch(lx->ident,"OPCODE\0KEYWORD\0ASM_KEYWORD\0R8\0R16\0R32\0R64\0SEG\0FSTK\0MM\0XMM\0");
    Lex(lx); //skip keyword
    if (!i) { //OPCODE
      MemSet(tempo,0,size1);
      tempo->type=HTT_OPCODE;
      tempo->instruction_entry_cnt=0;
      tempo->str=lx->ident;
      lx->ident=0;
      Lex(lx);	//Skip OPCODE
      while (lx->token && lx->token!=';') {
	tempi=&tempo->ins[tempo->instruction_entry_cnt];
	tempi->ins_entry_num=tempo->instruction_entry_cnt++;
	tempi->opcode_cnt=0;
	while (lx->token==TK_INTEGER||lx->token==TK_U8) {
	  tempi->opcode[tempi->opcode_cnt++]=lx->cur_i;
	  Lex(lx);
	}
	tempi->flags=0;
	if (lx->token=='+') {
	  tempi->flags|=IEF_PLUS_OPCODE;
	  Lex(lx);
	}
	if (lx->token=='/') {
	  Lex(lx);
	  if (lx->token==TK_INTEGER||lx->token==TK_U8) {
	    tempi->slash_val=lx->cur_i;
	    Lex(lx);
	  } else if (lx->token==TK_IDENT) {
	    tempi->slash_val=SV_REGISTER;
	    Lex(lx);
	  }
	} else
	  tempi->slash_val=SV_NONE;
	if (lx->token=='*') {
	  tempi->flags|=IEF_OP_SIZE16;
	  Lex(lx);
	}
	if (lx->token==TK_MOD_EQUAL) {
	  tempi->flags|=IEF_OP_SIZE32|IEF_48_REX;
	  Lex(lx);
	}
	if (lx->token=='%') {
	  tempi->flags|=IEF_OP_SIZE32;
	  Lex(lx);
	}
	if (lx->token=='!') {
	  tempi->flags|=IEF_DONT_SWITCH_MODES;
	  Lex(lx);
	}
	if (lx->token=='&') {
	  tempi->flags|=IEF_DFT;
	  Lex(lx);
	}
	if (lx->token==':') {
	  tempi->flags|=IEF_STI;
	  Lex(lx);
	}
	if (lx->token=='-') {
	  tempi->flags|=IEF_NO_REX;
	  Lex(lx);
	} else if (lx->token=='~') {
	  tempi->flags|=IEF_40_REX;
	  Lex(lx);
	} else if (lx->token=='=') {
	  tempi->flags|=IEF_48_REX;
	  Lex(lx);
	} else if (lx->token=='`') {
	  tempi->flags|=IEF_REX_ONLY_R8_R15;
	  Lex(lx);
	} else if (lx->token=='^') {
	  tempi->flags|=IEF_REX_XOR_LIKE;
	  Lex(lx);
	}
	if (lx->token=='$$') {
	  tempi->flags|=IEF_ENDING_ZERO;
	  Lex(lx);
	}
	if (lx->token!=TK_IDENT)
	  PrsExcept(lx,"ST_EXPECTING_IDENTIFIER");
	tempi->opcode_modifier=DefineMatch(lx->ident,
	    "ST_OPCODE_MODIFIERS");
	tempi->arg1=0;
	tempi->arg2=0;
	if (Lex(lx)==TK_IDENT) {
	  j=DefineMatch(lx->ident,"ST_ARG_TYPES");
	  tempi->arg1=j;
	  if (j>=ARGT_REL8 && j<=ARGT_REL32 ||
	      j>=ARGT_MOFFS8 && j<=ARGT_MOFFS64)
	    tempi|=IEF_NO_REX;

	  if (Lex(lx)==TK_IDENT) {
	    j=DefineMatch(lx->ident,"ST_ARG_TYPES");
	    Lex(lx);
	    tempi->arg2=j;
	    if (j>=ARGT_REL8 && j<=ARGT_REL32 ||
		j>=ARGT_MOFFS8 && j<=ARGT_MOFFS64)
	      tempi|=IEF_NO_REX;
	  }
	}
      }
      if (lx->token!=';')
	PrsExcept(lx,"ST_MISSING_SEMICOLON");
      Lex(lx); //Skip ';'
      size=offset(OpcodeHashEntry.ins)+
	  sizeof(InstructionEntry)*tempo->instruction_entry_cnt;
      tempo1=MAlloc(size);
      MemCpy(tempo1,tempo,size);
      tempo1->use_cnt=0;
      if (HashFind(tempo1->str,asm_hash,HTT_OPCODE))
	PrsExcept(lx,"ST_DUP_OPCODE_ENTRY");
      HashAdd(tempo1,asm_hash);
    } else if (1<=i<=10) { //KEYWORD,ASM_KEYWORD,R8,R16,R32,R64,SEG,FSTK,MM,XMM
      if (lx->token!=TK_IDENT)
	PrsExcept(lx,"ST_EXPECTING_IDENTIFIER");
      temph=CAlloc(sizeof(GenericHashEntry));
      temph->str=lx->ident;
      lx->ident=NULL;
      Lex(lx); //skip keyword name
      if (lx->token!=TK_INTEGER&&lx->token!=TK_U8)
	PrsExcept(lx,"ST_EXPECTING_INTEGER");
      temph->user_data0=lx->cur_i;
      if (Lex(lx)!=';')
	PrsExcept(lx,"ST_MISSING_SEMICOLON");
      Lex(lx);	//skip ';'
      switch (i) {
	case 1:
	  temph->type=HTT_KEYWORD;
	  HashAdd(temph,asm_hash);
	  if (first) {
	    temph=MAllocIdentical(temph);
	    HashAdd(temph,adam_task->hash_table);
	  }
	  break;
	case 2:
	  temph->type=HTT_ASM_KEYWORD;
	  HashAdd(temph,asm_hash);
	  break;
	case 3:
	  temph->type=HTT_R8;
	  HashAdd(temph,asm_hash);
	  break;
	case 4:
	  temph->type=HTT_R16;
	  HashAdd(temph,asm_hash);
	  break;
	case 5:
	  temph->type=HTT_R32;
	  HashAdd(temph,asm_hash);
	  break;
	case 6:
	  temph->type=HTT_R64;
	  HashAdd(temph,asm_hash);
	  break;
	case 7:
	  temph->type=HTT_SEG;
	  HashAdd(temph,asm_hash);
	  break;
	case 8:
	  temph->type=HTT_FSTK;
	  HashAdd(temph,asm_hash);
	  break;
	case 9:
	  temph->type=HTT_MM;
	  HashAdd(temph,asm_hash);
	  break;
	case 10:
	  temph->type=HTT_XMM;
	  HashAdd(temph,asm_hash);
	  break;
      }
    } else
      PrsExcept(lx,"Unknown Statement");
  }
  Free(tempo);
  LexDel(lx);
  for (i=0;i<NUM_INTERNAL_TYPES;i++) {
    tempit=&internal_types_table[i];
    tempc=PrsClassNew;
    tempc->type=HTT_INTERNAL_TYPE;
    tempc->sub_type=tempit->type;
    Bts(&tempc->flags,Cf_INTERNAL_TYPE);
    tempc->size=tempit->size;
    tempc->str=AStrNew(tempit->name);
    HashAdd(tempc,asm_hash);
    sys_internal_types[tempc->sub_type]=tempc;
    if (first) {
      tempc=MAllocIdentical(tempc);
      HashAdd(tempc,adam_task->hash_table);
    }
  }
}


U0 CmpLoadDefines()
{
  DefineLoad("ST_ERR",ST_ERR_ST);
  DefineLoad("ST_WARN",ST_WARN_ST);
  DefineLoad("ST_MISSING_SEMICOLON","missing ';' at");
  DefineLoad("ST_MISSING_EXPRESSION","missing Expression at ");
  DefineLoad("ST_MISSING_RIGHT_PAREN","missing ')' at ");
  DefineLoad("ST_MISSING_RIGHT_BRACKET","missing ']' at ");
  DefineLoad("ST_UNDEFINED_SYM","undefined sym at ");
  DefineLoad("ST_UNDEFINED_IDENTIFIER","undefined identifier at ");
  DefineLoad("ST_INVALID_CLASS","invalid class at ");
  DefineLoad("ST_INVALID_MEMBER","invalid member at ");
  DefineLoad("ST_EXPECTING_TYPE","expecting type at ");
  DefineLoad("ST_EXPECTING_IDENTIFIER","expecting identifier at ");
  DefineLoad("ST_EXPECTING_RIGHT_PAREN","expecting ')' at ");
  DefineLoad("ST_EXPECTING_LEFT_BRACE","expecting '{' at ");
  DefineLoad("ST_MISSING_RIGHT_BRACE","missing '}' at ");
  DefineLoad("ST_INVALID_LVAL","invalid lval at ");
  DefineLoad("ST_EXPECTING_COMMA","expecting ',' at ");
  DefineLoad("ST_EXPECTING_SYS_SYM","expecting System sym at ");
  DefineLoad("ST_MISSING_WHILE","missing 'while' at");
  DefineLoad("ST_ALREADY_ONE_IMMEDIATE","already one immediate at ");
  DefineLoad("ST_SYNTAX_ERROR","syntax error at ");
  DefineLoad("ST_EXPECTING_SCALE_FACTOR","expecting scale factor at ");
  DefineLoad("ST_QUESTION_NOT_ALLOWED","'?' not allowed at ");
  DefineLoad("ST_EXPECTING_LEFT_PAREN","expecting '(' at ");
  DefineLoad("ST_UNDEFINED_DUP_CNT","undefined DUP cnt at ");
  DefineLoad("ST_NO_LOCAL_GLBL_EXPORT","no local glbl exports at ");
  DefineLoad("ST_EXPECTING_COLON","expecting ':' at ");
  DefineLoad("ST_EXPECTING_DOT","expecting '.' at ");
  DefineLoad("ST_EXPECTING_LOCAL_VAR","expecting local var at ");
  DefineLoad("ST_REDEFINITION","redefinition at ");
  DefineLoad("ST_INVALID_INSTRUCTION","invalid instruction at ");
  DefineLoad("ST_BRANCH_OUT_OF_RANGE","branch out of range at ");
  DefineLoad("ST_ATTEMPT_DEFINE_IMPORT","attempt to define import at ");
  DefineLoad("ST_COULD_USE_U8_DISP","could use U8 displacement at ");
  DefineLoad("ST_LINE","line: ");
  DefineLoad("ST_NO_SIZE_SPECIFIED","no size specified at ");
  DefineListLoad("ST_IT_TYPES","I0\0U0\0I8\0U8\0I16\0U16\0I32\0U32\0I64\0U64\0F64\0uF64\0");
  DefineListLoad("ST_TY_TYPES","NULL\0IMM\0REG\0DISP\0SIB\0RIP\0STK\0");
  DefineListLoad("ST_U8_REGS","AL\0CL\0DL\0BL\0AH\0CH\0DH\0BH\0"
			"R8u8\0R9u8\0R10u8\0R11u8\0R12u8\0R13u8\0R14u8\0R15u8\0");
  DefineListLoad("ST_U8_REX_REGS","AL\0CL\0DL\0BL\0RSPu8\0RBPu8\0RSIu8\0RDIu8\0"
			"R8u8\0R9u8\0R10u8\0R11u8\0R12u8\0R13u8\0R14u8\0R15u8\0");
  DefineListLoad("ST_U16_REGS","AX\0CX\0DX\0BX\0SP\0BP\0SI\0DI\0"
			"R8u16\0R9u16\0R10u16\0R11u16\0R12u16\0R13u16\0R14u16\0R15u16\0");
  DefineListLoad("ST_U32_REGS","EAX\0ECX\0EDX\0EBX\0ESP\0EBP\0ESI\0EDI\0"
			"R8u32\0R9u32\0R10u32\0R11u32\0R12u32\0R13u32\0R14u32\0R15u32\0");
  DefineListLoad("ST_U64_REGS","RAX\0RCX\0RDX\0RBX\0RSP\0RBP\0RSI\0RDI\0"
			"R8\0R9\0R10\0R11\0R12\0R13\0R14\0R15\0");
  DefineListLoad("ST_MM_REGS","MM0\0MM1\0MM2\0MM3\0MM4\0MM5\0MM6\0MM7\0");
  DefineListLoad("ST_XMM_REGS","XMM0\0XMM1\0XMM2\0XMM3\0XMM4\0XMM5\0XMM6\0XMM7\0");
  DefineListLoad("ST_OPCODE_MODIFIERS","NO\0R\0CB\0CW\0CD\0CP\0IB\0IW\0ID\0RB\0RW\0RD\0I\0");
  DefineListLoad("ST_ARG_TYPES",
		"NO\0REL8\0REL16\0REL32\0UIMM8\0UIMM16\0UIMM32\0UIMM64\0"
		"R8\0R16\0R32\0R64\0"
		"IMM8\0IMM16\0IMM32\0IMM64\0RM8\0RM16\0RM32\0RM64\0"
		"M8\0M16\0M32\0M64\0"
		"M1632\0M16N32\0M16N16\0M32N32\0"
		"MOFFS8\0MOFFS16\0MOFFS32\0MOFFS64\0"
		"AL\0AX\0EAX\0RAX\0CL\0DX\0\0SREG\0"
		"SS\0DS\0ES\0FS\0GS\0CS\0ST0\0STI\0"
		"MM\0MM32\0MM64\0XMM\0XMM32\0XMM64\0XMM128\0XMM0\0");
  DefineListLoad("ST_SEG_REGS","ES\0CS\0SS\0DS\0FS\0GS\0");
  DefineListLoad("ST_FSTK_REGS","ST0\0ST1\0ST2\0ST3\0ST4\0ST5\0ST6\0ST7\0");
  DefineLoad("ST_EXPECTING_INTEGER","expecting integer at ");
  DefineLoad("ST_USE_POSTFIX_CAST","Use LoseThos postfix typecasting at ");
  DefineLoad("ST_PRS_INTERNAL_ERROR","Compiler Parse Error at ");
  DefineLoad("ST_OPT_INTERNAL_ERROR","Compiler Optimization Error at ");
  DefineLoad("ST_UNRESOLVED_REFERENCE","Unresolved Reference:");
  DefineLoad("ST_SCANNING_DIR","Scanning Directory:");
  DefineLoad("ST_UNUSED_VAR","Unused var ");
  DefineLoad("ST_UNNEEDED_NOUNUSEDWARN","Unneeded nounusedwarn ");
  DefineLoad("ST_UNUSED_EXTERN","Unused extern ");
  DefineLoad("ST_UNUSED_LABEL","Unused label ");
  DefineLoad("ST_UNUSED_LABEL_AT","Unused label at ");
  DefineLoad("ST_NOT_ALLOWED_IN_FUN","Not allowed in fun");
  DefineLoad("ST_MISSING_CATCH","missing 'catch' at");
  DefineLoad("ST_UNDEFINED_GOTO_LABEL","Undefined goto label ");
  DefineLoad("ST_UNDEFINED_GOTO_LABEL_AT","Undefined goto label at ");
  DefineLoad("ST_DUP_GOTO_LABEL","Duplicate goto label at ");
  DefineLoad("ST_BREAK_NOT_ALLOWED","'break' not allowed\n");
  DefineLoad("ST_ERRORS_WARNINGS","Errs:%d Warns:%d Code:%X Size:%X\n");
  DefineLoad("ST_IMPORT_NOT_NEEDED","import not needed at ");
  DefineLoad("ST_CASE_RANGE_ERROR","switch range error at ");
  DefineLoad("ST_LEX_STK_OVERFLOW","Lex Stk overflow");
  DefineLoad("ST_ASSERT_FAILED","Assert Failed ");
  DefineLoad("ST_ADDR_EXTERN","Can't take addr of extern fun");
  DefineLoad("ST_EXTERNS_NOT_ALLOWED","Extern Not Allowed at ");
  DefineLoad("ST_UNNECESSARY_PARENTHESIS","Unnecessary parenthesis");
  DefineLoad("ST_INVALID_TYPEDEF","Invalid typedef");
  DefineLoad("ST_STATIC_UNION","Static unions are not implemented");
  DefineLoad("ST_IMPORT_HASH_TABLE_NOT_ALLOWED","import_hash_table only allowed in static compiled #exe{} mode.");
  DefineLoad("ST_CANT_INIT_GLBL_ON_HEAP","Can't init glbl var on data heap in static module");
  DefineLoad("ST_INVALID_ARRAY_SIZE","Invalid array size at ");
  DefineLoad("ST_NO_FUN_ASM_ALIGN","ALIGN not allowed in fun asm blk ");
  DefineLoad("ST_ALIGN_MUST_BE_POWER_OF_TWO","ALIGN must be power of two at ");
  DefineLoad("ST_ALIGN_TOO_LARGE","In JIT mode, max ALIGN is 8 ");
  DefineLoad("ST_FUN_SIZE_NOT_DEFINED","Function size not defined at ");
  DefineLoad("ST_DUP_MEMBER","Duplicate member at ");
  DefineLoad("ST_FUN_PTR_FUN_PTR","Duplicate fun pointer at ");
  DefineLoad("ST_EXPECTING_STAR","Expecting '*' at ");
  DefineLoad("ST_EXPECTING_AMPERSAND","Expecting '&' at ");
  DefineLoad("ST_NOT_IMPLEMENTED","Feat not implemented");
  DefineLoad("ST_DUP_CASE","Duplicate case at ");
  DefineLoad("ST_DEREF_U0","Dereference U0 ");
  DefineLoad("ST_ASSIGN_U0","Assign U0 ");
  DefineLoad("ST_NO_GLBL_LABELS","No global labels at ");
  DefineLoad("ST_SHOULD_RETURN_VAL","Function should return val ");
  DefineLoad("ST_CANT_RETURN_VAL","Not in fun.Can't return a val ");
  DefineLoad("ST_SHOULD_NOT_RETURN_VAL","Function should NOT return val ");
  DefineLoad("ST_DUP_OPCODE_ENTRY","Duplicate OPCODE entry ");
  DefineLoad("ST_IN_FUN","in fun ");
  DefineLoad("ST_NOT_ARRAY_OR_PTR","not array or ptr ");
  DefineLoad("ST_16BIT_UNASM","16-bit unassembly is not well supported.\n");
  DefineLoad("ST_CANT_REG_VAR","Can't reg var ");
  DefineLoad("ST_ILLEGAL_FWD_REF","Illegal fwd ref at ");
  DefineLoad("ST_REG_IN_USE","Register in use ");
  DefineLoad("ST_ONE_BASE_CLASS","Only one base class allowed at this time at ");
  DefineLoad("ST_ADD_NOT_VAL","Must be address, not value ");
  DefineLoad("ST_HEADER_FOR_PRINTF","Missing header for PrintF(), PutS() and PutChar() at ");
  DefineLoad("ST_HEADER_FOR_SYSTRY","Missing header for SysTry() and SysUntry() at ");
$AN,"Anchor for inserting Compiler Defines","COMPILE_DEFINE"$


  DefineListLoad("ST_INTERMEDIATE_CODE",
// '*' means it is a dereference instruction
// '=' means it is an assignment instruction
// '^' means it is a compare instruction

// '9' arg cnt means var

// 'N' means not constant.  Sets LF_NOT_CONSTANT
//     which means an expression contains mem
//     accesses or things which might change.

// arg cnt,result cnt,deref/assign/cmp,nonconst,might not fill result
"00..END\0"
"00..START\0"
"00..NOP\0"
"10..END_EXP\0"
"00..NOP1\0"
"00..LABEL\0"
"00..CALL_START\0"
"01..CALL_END\0"
"00..CALL_END2\0"
"10..RETURN_VAL\0"
"00..RETURN_VAL2\0"
"01..IMM\0"
"01..F64\0"
"01.NSTR_CONST\0"
"01.NABS_ADDR\0"
"01.NADDR_IMPORT\0"
"01.NHEAP_GLBL\0"
"01..SIZEOF\0"
"01..TYPE\0"
"01.NGET_LABEL\0"
"01.NRBP\0"
"01.NREG\0"
"00.NFS\0"
"00.NGS\0"
"01.NLEA\0"
"01.NMOV\0"
"10..TO_I64\0"
"10..TO_F64\0"
"10..TO_BOOL\0"
"10..TOUPPER\0"
"11..LT_TYPECAST\0"
"11.NADDR\0"
"11..COM\0"
"11..NOT\0"
"11..UNARY_MINUS\0"
"11*NDEREF\0"
"11*NDEREF_PP\0"
"11*NDEREF_MM\0"
"11*N_PP\0"
"11*N_MM\0"
"11*NPP_\0"
"11*NMM_\0"
"21..SHL\0"
"21..SHR\0"
"11=.SHL_CONST\0"
"11=.SHR_CONST\0"
"21..POWER\0"
"21..MUL\0"
"21..DIV\0"
"21..MOD\0"
"21..AND\0"
"21..OR\0"
"21..XOR\0"
"21..ADD\0"
"21..SUB\0"
"11=.ADD_CONST\0"
"11=.SUB_CONST\0"
"21^.EQUAL_EQUAL\0"
"21^.NOT_EQUAL\0"
"21^.LESS\0"
"21^.GREATER\0"
"21^.LESS_EQUAL\0"
"21^.GREATER_EQUAL\0"
"01..PUSH_CMP\0"
"21..AND_AND\0"
"21..OR_OR\0"
"21..XOR_XOR\0"
"31..COLON\0"
"00..QUESTION\0"
"21=NASSIGN\0"
"21=NASSIGN_PP\0"
"21=NASSIGN_MM\0"
"21=NSHL_EQUAL\0"
"21=NSHR_EQUAL\0"
"21=NMUL_EQUAL\0"
"21=NDIV_EQUAL\0"
"21=NMOD_EQUAL\0"
"21=NAND_EQUAL\0"
"21=NOR_EQUAL\0"
"21=NXOR_EQUAL\0"
"21=NADD_EQUAL\0"
"21=NSUB_EQUAL\0"
"00..JMP\0"
"20..SWITCH\0"
"20..NOBOUND_SWITCH\0"
"90..ADD_RSP\0"
"90..ADD_RSP1\0"
"00..ENTER\0"
"00=.PUSH_REGS\0"
"00=.POP_REGS\0"
"00..LEAVE\0"
"00..RET\0"
"00.NCALL\0"
"00.NCALL_INDIRECT\0"
"00.NCALL_INDIRECT2\0"
"00.NCALL_IMPORT\0"
"00.NCALL_EXTERN\0"
"00.NIRQ\0"
"00.NASM\0"
"00.NCLI\0"
"00.NSTI\0"
"10.NPREEMPT\0"
"00.NCLD\0"
"00.NSTD\0"
"00.NHLT\0"
"10.NPUSH\0"
"00.NPOP\0"
"00.NPUSHFD\0"
"00.NPOPFD\0"
"00.NPAUSE\0"
"10.NCLFLUSH\0"
"10.NINVLPG\0"
"00.NWBINVD\0"
"00.NINVD\0"
"10.NINU8\0"
"10.NINU16\0"
"10.NINU32\0"
"20.NOUTU8\0"
"20.NOUTU16\0"
"20.NOUTU32\0"
"00.NGET_FLAGS\0"
"10.NSET_FLAGS\0"
"00.NGET_RAX\0"
"10.NSET_RAX\0"
"00.NGET_BASE_PTR\0"
"10.NSET_BASE_PTR\0"
"00.NGET_STK_PTR\0"
"10.NSET_STK_PTR\0"
"01.NRIP\0"
"00.NRDTSC\0"
"00.NHPET\0"
"20.NBT\0"
"20.NBTS\0"
"20.NBTR\0"
"20.NBTC\0"
"30.NBEQU\0"
"20.NLBTS\0"
"20.NLBTR\0"
"20.NLBTC\0"
"30.NLBEQU\0"
"10.NBSF\0"
"10.NBSR\0"
"20.NINSQUE\0"
"20.NINSQUE_REV\0"
"10.NREMQUE\0"
"10.NSTRLEN\0"
"30.NMEMCPY\0"
"30.NMEMSET\0"
"30.NMEMSET_U16\0"
"30.NMEMSET_U32\0"
"30.NMEMSET_U64\0"
"10..BR_ZERO\0"
"10..BR_NOT_ZERO\0"
"20..BR_NOT_EQUAL\0"
"20..BR_EQUAL_EQUAL\0"
"20..BR_GREATER_EQUAL\0"
"20..BR_LESS_EQUAL\0"
"20..BR_GREATER\0"
"20..BR_LESS\0"
"21..BR_2NOT_EQUAL\0"
"21..BR_2EQUAL_EQUAL\0"
"21..BR_2GREATER_EQUAL\0"
"21..BR_2LESS_EQUAL\0"
"21..BR_2GREATER\0"
"21..BR_2LESS\0"
"20..BR_AND_ZERO\0"
"20..BR_AND_NOT_ZERO\0"
"10.NBR_MM_ZERO\0"
"10.NBR_MM_NOT_ZERO\0"
"20..BR_AND_AND_ZERO\0"
"20..BR_AND_AND_NOT_ZERO\0"
"20..BR_OR_OR_ZERO\0"
"20..BR_OR_OR_NOT_ZERO\0"
"20.NBR_BT\0"
"20.NBR_BTS\0"
"20.NBR_BTR\0"
"20.NBR_BTC\0"
"20.NBR_LBTS\0"
"20.NBR_LBTR\0"
"20.NBR_LBTC\0"
"20.NBR_NOT_BT\0"
"20.NBR_NOT_BTS\0"
"20.NBR_NOT_BTR\0"
"20.NBR_NOT_BTC\0"
"20.NBR_NOT_LBTS\0"
"20.NBR_NOT_LBTR\0"
"20.NBR_NOT_LBTC\0"
"20.NSWAP_U8\0"
"20.NSWAP_U16\0"
"20.NSWAP_U32\0"
"20.NSWAP_U64\0"
"10..ABS_I64\0"
"10..SIGN_I64\0"
"20..MIN_I64\0"
"20..MIN_U64\0"
"20..MAX_I64\0"
"20..MAX_U64\0"
"30..LIMIT_I64\0"
"30..LIMIT_U64\0"
"20..MOD_U64\0"
"10..SQRI64\0"
"10..SQRU64\0"
"01.NMOV_FS\0"
"01.NMOV_GS\0"
);$AN,"Anchor for adding intermediate codes","IC_INIT_TABLE"$

}

U0 CmpFillTables()
{
  U8 *st;
  I64 i,k;
  U32 *u;
  U32 *d4;

  assign_bitmap=CAlloc((EC_NUM_ENTRIES+7)>>3);
  deref_bitmap=CAlloc((EC_NUM_ENTRIES+7)>>3);
  cmp_bitmap=CAlloc((EC_NUM_ENTRIES+7)>>3);
  not_constant_bitmap=CAlloc((EC_NUM_ENTRIES+7)>>3);
  fpop_bitmap=CAlloc((EC_NUM_ENTRIES+7)>>3);
  cmp_arg_cnt=CAlloc(EC_NUM_ENTRIES);
  cmp_result_cnt=CAlloc(EC_NUM_ENTRIES);
  st=Define("ST_INTERMEDIATE_CODE");
  for (i=0;i<EC_NUM_ENTRIES;i++) {
    cmp_arg_cnt[i]=*st-'0';
    cmp_result_cnt[i]=st[1]-'0';
    if (st[2]=='*')
      Bts(deref_bitmap,i);
    else if (st[2]=='=')
      Bts(assign_bitmap,i);
    else if (st[2]=='^')
      Bts(cmp_bitmap,i);
    if (st[3]=='N')
      Bts(not_constant_bitmap,i);
    st+=StrLen(st)+1;
    //See $LK,"ST_INTERMEDIATE_CODE","FF:::/LT/Compiler/CodeGen.CPZ,ST_INTERMEDIATE_CODE"$.
  }

  Bts(fpop_bitmap,EC_ADD);
  Bts(fpop_bitmap,EC_SUB);
  Bts(fpop_bitmap,EC_MUL);
  Bts(fpop_bitmap,EC_DIV);
  Bts(fpop_bitmap,EC__PP);
  Bts(fpop_bitmap,EC__MM);
  Bts(fpop_bitmap,EC_PP_);
  Bts(fpop_bitmap,EC_MM_);

  to_reg_vars_map=CAlloc(NUM_REGS*sizeof(U8));
  num_reg_vars=0;
  reg_vars_mask=CREG_VARS_MASK;
  for (i=0;i<NUM_REGS;i++)
    if (Bt(&reg_vars_mask,i))
      to_reg_vars_map[num_reg_vars++]=i;

  to_non_ptr_vars_map=CAlloc(NUM_REGS*sizeof(U8));
  num_non_ptr_vars=0;
  non_ptr_vars_mask=CREG_NON_PTR_MASK;
  for (i=0;i<NUM_REGS;i++)
    if (Bt(&non_ptr_vars_mask,i))
      to_non_ptr_vars_map[num_non_ptr_vars++]=i;
 
  to_stk_temps_map_odd=CAlloc(NUM_REGS*sizeof(U8));
  to_stk_temps_map_even=CAlloc(NUM_REGS*sizeof(U8));
  num_stk_temps=0;
  stk_temps_mask=CSTK_TEMPS_MASK;
  for (i=0;i<NUM_REGS;i++)
    if (Bt(&stk_temps_mask,i))
      to_stk_temps_map_even[num_stk_temps++]=i;
  k=num_stk_temps;
  for (i=0;i<NUM_REGS;i++)
    if (Bt(&stk_temps_mask,i))
      to_stk_temps_map_odd[--k]=i;

  u=CAlloc(sizeof(U32)*TK_NUM_TK);
  dual_U16_tokens1=u;
  u['!']=TK_NOT_EQUAL<<16+'=';
  u['&']=TK_AND_AND<<16+'&';
  u['*']=TK_MUL_EQUAL<<16+'=';
  u['+']=TK_PLUS_PLUS<<16+'+';
  u['-']=TK_DEREFERENCE<<16+'>';
  u['/']='*';
  u[':']=TK_DBL_COLON<<16+':';
  u['<']=TK_LESS_EQUAL<<16+'=';
  u['=']=TK_EQUAL_EQUAL<<16+'=';
  u['>']=TK_GREATER_EQUAL<<16+'=';
  u['^']=TK_XOR_EQUAL<<16+'=';
  u['|']=TK_OR_OR<<16+'|';
  u['%']=TK_MOD_EQUAL<<16+'=';

  u=CAlloc(sizeof(U32)*TK_NUM_TK);
  dual_U16_tokens2=u;
  u['&']=TK_AND_EQUAL<<16+'=';
  u['+']=TK_ADD_EQUAL<<16+'=';
  u['-']=TK_MINUS_MINUS<<16+'-';
  u['/']='/';
  u['<']=TK_SHL<<16+'<';
  u['>']=TK_SHR<<16+'>';
  u['^']=TK_XOR_XOR<<16+'^';
  u['|']=TK_OR_EQUAL<<16+'=';

  u=CAlloc(sizeof(U32)*TK_NUM_TK);
  dual_U16_tokens3=u;
  u['-']=TK_SUB_EQUAL<<16+'=';
  u['/']=TK_DIV_EQUAL<<16+'=';

//$AN,"","OP_precedence"$Here the operator precedences are
//defined.
  d4=CAlloc(sizeof(U32)*TK_NUM_TK);
  unary_ops_table=d4;
  d4['~']=PREC_UNARY_PRE<<16+EC_COM;
  d4['!']=PREC_UNARY_PRE<<16+EC_NOT;
  d4['-']=PREC_UNARY_PRE<<16+EC_UNARY_MINUS;
  d4['&']=PREC_UNARY_PRE<<16+EC_ADDR;
  d4['*']=PREC_UNARY_PRE<<16+EC_DEREF;


  d4=CAlloc(sizeof(U32)*TK_NUM_TK);
  binary_ops_table=d4;

  d4['`']	=(PREC_EXP+ASSOCF_RIGHT)<<16+EC_POWER;
  d4[TK_SHL]	=(PREC_EXP+ASSOCF_LEFT)<<16+EC_SHL;
  d4[TK_SHR]	=(PREC_EXP+ASSOCF_LEFT)<<16+EC_SHR;

  d4['*']	=PREC_MUL<<16+EC_MUL;
  d4['/']	=(PREC_MUL+ASSOCF_LEFT)<<16+EC_DIV;
  d4['%']	=(PREC_MUL+ASSOCF_LEFT)<<16+EC_MOD;

  d4['&']	=PREC_AND<<16+EC_AND;

  d4['^']	=PREC_XOR<<16+EC_XOR;

  d4['|']	=PREC_OR<<16+EC_OR;

  d4['+']	=PREC_ADD<<16+EC_ADD;
  d4['-']	=(PREC_ADD+ASSOCF_LEFT)<<16+EC_SUB;

  d4['<']	=PREC_CMP<<16+EC_LESS;
  d4['>']	=PREC_CMP<<16+EC_GREATER;
  d4[TK_LESS_EQUAL]	=PREC_CMP<<16+EC_LESS_EQUAL;
  d4[TK_GREATER_EQUAL]	=PREC_CMP<<16+EC_GREATER_EQUAL;

  d4[TK_EQUAL_EQUAL]	=PREC_CMP2<<16+EC_EQUAL_EQUAL;
  d4[TK_NOT_EQUAL]	=PREC_CMP2<<16+EC_NOT_EQUAL;

  d4[TK_AND_AND]	=PREC_AND_AND<<16+EC_AND_AND;

  d4[TK_XOR_XOR]=PREC_XOR_XOR<<16+EC_XOR_XOR;

  d4[TK_OR_OR]	=PREC_OR_OR<<16+EC_OR_OR;

  d4[':']	=(PREC_QUEST+ASSOCF_LEFT)<<16+EC_COLON;
  d4['?']	=(PREC_QUEST+ASSOCF_LEFT)<<16+EC_QUESTION;

  d4['=']	=(PREC_ASSIGN+ASSOCF_RIGHT)<<16+EC_ASSIGN;
  d4[TK_SHL_EQUAL]=(PREC_ASSIGN+ASSOCF_RIGHT)<<16+EC_SHL_EQUAL;
  d4[TK_SHR_EQUAL]=(PREC_ASSIGN+ASSOCF_RIGHT)<<16+EC_SHR_EQUAL;
  d4[TK_MUL_EQUAL]=(PREC_ASSIGN+ASSOCF_RIGHT)<<16+EC_MUL_EQUAL;
  d4[TK_DIV_EQUAL]=(PREC_ASSIGN+ASSOCF_RIGHT)<<16+EC_DIV_EQUAL;
  d4[TK_MOD_EQUAL]=(PREC_ASSIGN+ASSOCF_RIGHT)<<16+EC_MOD_EQUAL;
  d4[TK_AND_EQUAL]=(PREC_ASSIGN+ASSOCF_RIGHT)<<16+EC_AND_EQUAL;
  d4[TK_OR_EQUAL] =(PREC_ASSIGN+ASSOCF_RIGHT)<<16+EC_OR_EQUAL;
  d4[TK_XOR_EQUAL]=(PREC_ASSIGN+ASSOCF_RIGHT)<<16+EC_XOR_EQUAL;
  d4[TK_ADD_EQUAL]=(PREC_ASSIGN+ASSOCF_RIGHT)<<16+EC_ADD_EQUAL;
  d4[TK_SUB_EQUAL]=(PREC_ASSIGN+ASSOCF_RIGHT)<<16+EC_SUB_EQUAL;
}
