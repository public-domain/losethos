#help_index "Compiler/Lex"
#help_file "::/LT/Doc/Lex"
public extern LexStruct *LexNew(U8 *buf,U64 flags,U8 *filename=NULL);
public extern U0 LexDel(LexStruct *lx);
public extern U64 LexSize(LexStruct *lx);
public extern U0 SetSrcFileInfo(LexStruct *lx,SrcSymHashEntry *h);
public extern U0 LexPutToken(LexStruct *lx);
public extern U64 LexGetChar(LexStruct *lx);
public extern U0 LexSkipEol(LexStruct *lx);
public extern U0 LexPush(LexStruct *lx);
public extern U0 LexPop(LexStruct *lx);
//public extern U0 LexSubStkPtr(LexStruct *lx);
public extern I64 Lex(LexStruct *lx);
public extern U8 *LexExtStr(LexStruct *lx,U64 *size=NULL);
public extern U0 LexPutLine(LexStruct *lx,U8 *start);
public extern I64 LexLineNum(LexStruct *lx);
public extern U0 LexPutLink(LexStruct *lx);
public extern U0 PutLexErr(LexStruct *lx,U8 *name,U8 *msg=NULL);
public extern U0 PutLexWarn(LexStruct *lx,U8 *name,U8 *msg=NULL);
public extern U0 PrsExcept(LexStruct *lx,U8 *name=NULL,U8 *msg=NULL);
public extern U0 *CmpExpression(LexStruct *lx,U64 *type=NULL);
public extern U8 *CmpStatement(LexStruct *lx,U64 *type);
public extern U64 GetExpression(LexStruct *lx,U8 **ex); //FALSE=no err
public extern I64 Expression(LexStruct *lx);
public extern U0 ExpressionDel(U0 *code);
public extern I64 MemberMetaData(U8 *st,MemberListStruct *ml);
extern U0 MemberListDel(ClassStruct *c);
public extern MemberListMeta *MemberMetaFind(U8 *st,MemberListStruct *ml);
public extern MemberListStruct *MemberFind(U8 *st,ClassStruct *c);
public extern U0 LexIncludeLtf(LexStruct *lx,U8 *filename,Ltf *_l);

#help_index "Compiler/Directive"
#help_file "::/LT/Doc/Directives"
public extern U0 StreamPutS(U8 *st);
public extern U0 StreamPrintF(U8 *src,...);
public extern BoolI64 Externs2Imports(BoolI8 val);
public extern BoolI64 KeepPrivate(BoolI8 val);
public extern BoolI64 PTrace(BoolI8 val);
public extern BoolI64 OTrace(BoolI8 val);
public extern U64 OPassTraceMask(U64 i=0b1001111101);
public extern BoolI64 OptEqu(U64 num,BoolI8 val);
public extern BoolI64 OptOn(U64 num);
public extern BoolI64 OptOff(U64 num);
public extern BoolI64 GetOpt(U64 num);
public extern BoolI64 NoRegVar(BoolI8 val);
public extern BoolI64 CTrace(BoolI8 val);
public extern BoolI64 LTrace(BoolI8 val);
public extern BoolI64 Echo(BoolI8 val);
public extern BoolI64 IsReplaceSyms();
public extern U0 ImportHashTable(SysHashTable *tempht);
public extern I64 IncludeDepth();
public extern BoolI64 CmdLineExit();

#help_index "Compiler;Cmd Line (Typically)"
public extern I64 Cmp(U8 *filename,U8 *map_name=NULL,U8 *out_name=NULL,U8 mapfile_drv=0);

#help_index "Compiler"
public extern I64 ExePutS(U8 *buf,U8 *filename=NULL,BoolI8 just_load=FALSE);
public extern I64 ExePrintF(U8 *fmt,...);
public extern I64 ExeFile(U8 *name,BoolI8 just_load=FALSE);
public extern I64 ExePutS2(U8 *buf,U8 *filename=NULL,BoolI8 just_load=FALSE);
public extern I64 ExePrintF2(U8 *fmt,...);
public extern I64 ExeFile2(U8 *name,BoolI8 just_load=FALSE);
public extern I64 ExePutS3(U8 *buf,U8 *filename=NULL,BoolI8 just_load=FALSE);
public extern I64 ExePrintF3(U8 *fmt,...);
public extern I64 ExeFile3(U8 *name,BoolI8 just_load=FALSE);
extern SysHashTable *asm_hash;
extern U0 CmpInit(BoolI8 first);
extern U64 compiled_lines,compiled_funs,compiled_bytes;


//The following is for benchmarking
extern BoolI8 cmp_do_not_write_to_dsk;

#help_index "Debugging"
public extern U0 Ui(U8 *buf,U0 **_ip,I64 seg_size=64,I64 *branch=NULL);
public extern U0 *U(U0 *ip,U64 cnt=20,I64 seg_size=64);
public extern I64 Un(U0 *ip,U64 cnt=0x80,I64 seg_size=64);

#help_index ""
