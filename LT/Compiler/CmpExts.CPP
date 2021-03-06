import BoolI64 PutSrcLink(U0 *add,U64 cnt=1,U8 *buf=NULL);
import Ltf *LtfRead(U8 *name,U64 flags=0);
import U0 LtfLoad(Ltf *l,U8 *src2,U64 size);
import U0 LtfWrite(Ltf *l,BoolI8 prompt=FALSE);
import Ltf *LtfNew(TaskStruct *task=NULL);
import U0 LtfRecalc(Ltf *l,BoolI8 has_cursor=FALSE,I64 recalc=LTF_RECALC_ALL);
import U0 LtfDel(Ltf *l);
import U0 LtfRst(Ltf *l,BoolI8 is_old);
import LtfEntry *LtfPutLine(LtfEntry *cl);
import U64 LtfSize(Ltf *l);
import U8 *Ltf2PlainText(Ltf *l,LtfEntry *ll);
import U0 HashRep(SysHashTable *h=NULL,
  U64 mask=HTT_ALL,U8 *outname=NULL,U8 *flags=NULL,U8 mapfile_drv=0,U8 *index=NULL,
  Ltf *l_=NULL);

extern I64 Expression(LexStruct *lx);
extern BoolI64 GetExpression(LexStruct *lx,U8 **exp);
extern U64 PrsUnaryTerm(LexStruct *lx,PrsStk *ps,MemberListStruct **local_var,ArrayDimStruct **tempad,
    I64 *max_prec,I64 *unary_pre_prec,I64 *paren_prec);
extern U64 PrsUnaryModifier(LexStruct *lx,PrsStk *ps,MemberListStruct **local_var,ArrayDimStruct **tempad,I64 *unary_post_prec);
extern U0 PrsDereference(LexStruct *lx);
extern U0 PrsExcept(LexStruct *lx,U8 *name=NULL,U8 *msg=NULL);
extern ExeStruct *CmpJoin(LexStruct *lx,FunStruct *locals,BoolI8 first_lex,BoolI8 asm_blk=FALSE,U8 *map_name=NULL,U8 mapfile_drv=0);
extern U0 CmpFixUpJITAsm(LexStruct *lx,ExeStruct *ex,I64 ip2);
extern U0 CmpFixUpStaticAsm(LexStruct *lx,ExeStruct *ex,I64 ip2);
extern ClassStruct *PrsClass(LexStruct *lx,BoolI8 is_union,U64 cps_flags,BoolI8 is_extern);
extern ClassStruct *PrsType(LexStruct *lx,ClassStruct **tempc1,
  I64 *_mode,MemberListStruct *tempm,U8 **_ident,FunStruct **_fun_ptr,
  ExportSysSymHashEntry **_tempess,ArrayDimStruct *tempad,U64 cps_flags);
extern U0 PrsVarInit2(LexStruct *lx,U8 **_dst,ClassStruct *tempc,ArrayDimStruct *tempad,U8 *data_addr_ip,U8 **base,BoolI8 data_heap,I64 pass);
extern FunStruct *PrsFunJoin(LexStruct *lx,
	ClassStruct *temp_return,U8 *name,U64 cps_flags);
extern BoolI64 PrsStatement(LexStruct *lx,FunStruct *locals,I64 try_cnt=0,
  CbMiscStruct *lb_break=NULL,BoolI8 saved_prs_semicolon=TRUE,BoolI8 if_else=FALSE);
extern IntermediateCode *OptLag1(IntermediateCode *tempi);
extern IntermediateCode *OptPass012(LexStruct *lx,FunStruct *fun_class,BoolI8 combined);
extern U0 OptPass3(LexStruct *lx,FunStruct *fun_class,OptRegStruct *register_offsets);
extern U0 OptPass4(LexStruct *lx,OptRegStruct *register_offsets,U64 *_type);
extern U0 OptPass5(LexStruct *lx);
extern U0 OptPass6(LexStruct *lx,FunStruct *fun_class);
extern U64 OptPass789A(LexStruct *lx,FunStruct *fun_class,OptRegStruct *register_offsets,U8 *buf,DbgInfo **_dbg,IndentInfo **_indent,I64 *min_line,I64 *max_line);
extern I64 PrsKeyWord(LexStruct *lx);
extern U64 HashEntrySize(SrcSymHashEntry *temph);
extern U0 PutLexWarn(LexStruct *lx,U8 *name,U8 *msg=NULL);
extern SrcSymHashEntry *HashEntryImportCopy(SrcSymHashEntry *temph);
extern U0 HashTableImport(SysHashTable *src,SysHashTable *dst);

LTextern CMP_TEMPLATES U32 cmp_templates[1];
LTextern CMP_TEMPLATES_DONT_POP U32 cmp_templates_dont_pop[1];
LTextern CMP_TEMPLATES_DONT_PUSH U32 cmp_templates_dont_push[1];
LTextern CMP_TEMPLATES_DONT_PUSH_POP U32 cmp_templates_dont_push_pop[1];
LTextern CMP_TEMPLATES_DONT_PUSH2 U32 cmp_templates_dont_push2[1];
