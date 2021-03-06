SrcSymHashEntry *HashEntryImportCopy(SrcSymHashEntry *temph)
{ //Actually this imports, not copies!!
  SrcSymHashEntry *result;
  result=MAllocIdentical(temph);
  result->str=MAllocIdentical(temph->str);
  result->use_cnt=0;
  result->src_link=NULL;
  result->index=NULL;
  result->list=NULL;
  result->debug=NULL;
  result->indent=NULL;
  if (temph->type & (HTT_FUN | HTT_CLASS)) {
    result(ClassStruct *)->member_list=MemberListCopy(temph);
    if (temph->type & HTT_FUN) {
      if (Bt(temph(FunStruct *)->flags,Ff_INTERNAL)) {
	result->import_name=NULL;
	LBtr(result(FunStruct *)->flags,Cf_EXTERN);
      } else {
	result->import_name=MAllocIdentical(temph->import_name);
	result->type|=HTF_IMPORT;
	LBts(result(FunStruct *)->flags,Cf_EXTERN);
      }
    }
  } else if (temph->type & (HTT_EXPORT_SYS_SYM | HTT_GLBL_VAR)) {
    result->import_name=MAllocIdentical(temph->import_name);
    result->type|=HTF_IMPORT;
    if (temph->type&HTT_EXPORT_SYS_SYM) {
      result->type|=HTF_UNRESOLVED;
      result(ExportSysSymHashEntry *)->val=0;
    } else {
      result(GlblVarStruct *)->dim.next=LinkedListCopy(temph(GlblVarStruct *)->dim.next);
      result(GlblVarStruct *)->flags|=GVAF_IMPORT|GVAF_ALIAS;
      if (temph(GlblVarStruct *)->fun_ptr)
	result(GlblVarStruct *)->fun_ptr=HashEntryImportCopy(temph(GlblVarStruct *)->fun_ptr-temph(GlblVarStruct *)->fun_ptr->ptr_cnt)(FunStruct *)+temph(GlblVarStruct *)->fun_ptr->ptr_cnt;
    }
  } else if (temph->type&HTT_DEFINE_STR)
    result(DefineStrHashEntry *)->data=MAllocIdentical(temph(DefineStrHashEntry *)->data);
  else if (temph->type&HTT_TYPEDEF) {
    result(GlblVarStruct *)->dim.next=LinkedListCopy(temph(GlblVarStruct *)->dim.next);
    if (temph(GlblVarStruct *)->fun_ptr)
      result(GlblVarStruct *)->fun_ptr=HashEntryImportCopy(temph(GlblVarStruct *)->fun_ptr-temph(GlblVarStruct *)->fun_ptr->ptr_cnt)(FunStruct *)+temph(GlblVarStruct *)->fun_ptr->ptr_cnt;
  }
  return result;
}

U0 HashTableImport(SysHashTable *src,SysHashTable *dst)
{
  I64 i;
  SrcSymHashEntry *temph,*temph1,*temph2;
  if (!src) return;
  for (i=0;i<=src->mask;i++) {
    temph=src->body[i];
    while (temph) {
      if (temph->type & HTT_SRC_SYM) {
	//Only add first in list
	temph2=src->body[i];
	while (temph2!=temph)
	  if (!StrCmp(temph->str,temph2->str) &&
	      temph2->type & temph->type & HTT_SRC_SYM ||
	      temph->type & HTT_EXPORT_SYS_SYM)
	    break;
	  else
	    temph2=temph2->next;

	if (temph2==temph && (temph1=HashEntryImportCopy(temph))) {
	  if (IsReplaceSyms)
	    HashReplace(temph1,dst);
	  else
	    HashAdd(temph1,dst);
	}
      }
      temph=temph->next;
    }
  }
}

U64 HashEntrySize(SrcSymHashEntry *temph)
{
  DbgInfo *dbg;
  U64 result=MSize2(temph);
  if (!(temph->type&HTT_DICT_WORD))
    result+=MSize2(temph->str);
  if (temph->type & HTT_SRC_SYM) {
    result+=MSize2(temph->src_link);
    result+=MSize2(temph->index);
    result+=MSize2(temph->import_name);
    result+=LinkedListSize(temph->list);
    if (temph->type & (HTT_FUN | HTT_EXPORT_SYS_SYM)) {
      result+=MSize2(temph->debug);
      result+=MSize2(temph->indent);
    }
    if (temph->type & HTT_CLASS)
      result+=MemberListSize(temph);
    else if (temph->type & HTT_FUN) {
      result+=MemberListSize(temph);
      if (dbg=temph->debug)
	//This should be $LK,"MSize","MN:MSize"$() but it would crash on static .BIZ file funs.
	result+=dbg->body[dbg->max_line-dbg->min_line+1]-temph(FunStruct *)->executable_addr;
    } else if (temph->type&HTT_DEFINE_STR)
      result+=MSize2(temph(DefineStrHashEntry *)->data);
    else if (temph->type & (HTT_GLBL_VAR | HTT_TYPEDEF)) {
      result+=LinkedListSize(temph(GlblVarStruct *)->dim.next);
      if (!(temph(GlblVarStruct *)->flags&GVAF_ALIAS))
	result+=MSize2(temph(GlblVarStruct *)->data_addr);
      if (temph(GlblVarStruct *)->fun_ptr)
	result+=HashEntrySize(temph(GlblVarStruct *)->fun_ptr-temph(GlblVarStruct *)->fun_ptr->ptr_cnt);
    }
  } else if (temph->type & HTT_FILE)
    result+=MSize2(temph(GenericHashEntry *)->user_data0);
  else if (temph->type & HTT_MODULE &&
	   StrCmp(temph->str,OSMAIN_MODULE_NAME))
    result+=MSize2(temph(GenericHashEntry *)->user_data0);
  return result;
}

U64 HashTableSize(SysHashTable *tempht)
{
  U64 i,result=0;
  SrcSymHashEntry *temph;
  if (!tempht)
    return 0;
  for (i=0;i<=tempht->mask;i++) {
    temph=tempht->body[i];
    while (temph) {
      result+=HashEntrySize(temph);
      temph=temph->next;
    }
  }
  result+=MSize2(tempht->body);
  result+=MSize2(tempht);
  return result;
}
