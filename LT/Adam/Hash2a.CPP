#help_index "Hash"
public I64 HashTypeNum(SysHashEntry *temph)
{
  if (temph)
    return Bsf(temph->type&HTT_TYPE_MASK);
  else
    return -1;
}

public I64 HashVal(SysHashEntry *temph)
{
  nobound_switch (HashTypeNum(temph)) {
    case HTt_EXPORT_SYS_SYM:
      return temph(ExportSysSymHashEntry *)->val;
    case HTt_IMPORT_SYS_SYM:
      return temph(ImportSysSymHashEntry *)->module_base;
    case HTt_DEFINE_STR:
    case HTt_TYPEDEF:
    case HTt_CLASS:
    case HTt_INTERNAL_TYPE:
    case HTt_WORD:
    case HTt_DICT_WORD:
    case HTt_OPCODE:
    case HTt_HELP_FILE:
      return temph;
    case HTt_GLBL_VAR:
      if (temph(GlblVarStruct *)->flags&GVAF_EXTERN)
	return temph;
      else
	return temph(GlblVarStruct *)->data_addr;
    case HTt_FUN:
      if (Bt(temph(FunStruct *)->flags,Cf_EXTERN))
	return temph;
      else
	return temph(FunStruct *)->executable_addr;
    case HTt_KEYWORD:
    case HTt_ASM_KEYWORD:
    case HTt_R8:
    case HTt_R16:
    case HTt_R32:
    case HTt_R64:
    case HTt_SEG:
    case HTt_FSTK:
    case HTt_MM:
    case HTt_XMM:
    case HTt_MODULE:
    case HTt_FILE:
    case HTt_NAMED_FRAME_PTR:
      return temph(GenericHashEntry *)->user_data0;

    case -1:		//nobound_switch
    case HTt_NUM_TYPES: //nobound_switch
    default:
      return 0;
  }
}

#help_index "Info;Hash/Reports;"
class HashRepStruct
{
  GenericHashEntry *h;
  U8 *index;
};


I64 HashEntriesCompare(HashRepStruct *h1,HashRepStruct *h2)
{
  I64 i1,i2;
  if (i1=StrCmp(h1->h->str,h2->h->str))
    return i1;
  i1=HashTypeNum(h1->h);
  i2=HashTypeNum(h2->h);
  return i1-i2;
}

I64 HashEntriesCompare2(HashRepStruct *h1,HashRepStruct *h2)
{
  FunStruct *tempf1=h1->h,*tempf2=h2->h;
  I64 i1=HashVal(tempf1),i2=HashVal(tempf2);
  if (i1==i2) {
    i1=HashTypeNum(tempf1);
    i2=HashTypeNum(tempf2);
    if (i1==i2)
      return StrCmp(tempf1->str,tempf2->str);
  }
  return i1-i2;
}

I64 HelpIndexCnt(U8 *ptr,U8 *index)
{
  U8 cnt=0,ch;
  I64 index_len=StrLen(index);
  while (*ptr) {
    if (!StrNCmp(ptr,index,index_len))
      cnt++;
    while (ch=*ptr++)
      if (ch==';')
	break;
    if (!ch)
      ptr--;
  }
  return cnt;
}

U8 *HelpIndexStr(U8 **ptr_,U8 *index)
{
  U8 *ptr=*ptr_,*ptr2,ch,*result;
  I64 index_len=StrLen(index);
  while (*ptr) {
    ptr2=ptr;
    while (ch=*ptr++)
      if (ch==';')
	break;
    if (!ch)
      ptr--;
    *ptr_=ptr;
    if (!StrNCmp(ptr2,index,index_len)) {
      if (ch==';')
	ptr--;
      *ptr=0;
      result=StrNew(ptr2);
      *ptr=ch;
      return result;
    }
  }
  return NULL;
}

I64 HashEntriesCompare3(HashRepStruct *h1,HashRepStruct *h2)
{
  I64 i,i1=0,i2=0;
  i=StrCmp(h1->index,h2->index);
  if (i)
    return i;
  else {
    if (h1->h->type&HTT_HELP_FILE)
      i1=1;
    if (h2->h->type&HTT_HELP_FILE)
      i2=1;
    i=i2-i1;
    if (i)
      return i;
    else
      return StrCmp(h1->h->str,h2->h->str);
  }
}

//Possible choices for first arg, "h" include
// "adam_task->hash_table"
// "Fs->hash_table"
public U0 HashRep(SysHashTable *h=NULL,
  U64 mask=HTT_ALL,U8 *outname=NULL,U8 *flags=NULL,U8 mapfile_drv=0,U8 *index=NULL,
  Ltf *l_=NULL)
{
  SysHashTable *tempht;
  SrcSymHashEntry *temph;
  GenericHashEntry *ptr;
  HashRepStruct *list;
  U64 cnt,i,j,k,size,f=0;
  U8 buf[512],*buf4,*last_index=StrNew(""),*cur_index;
  I64 dbg_bin_num,indent_bin_num;
  Ltf *l,*l1;
  BoolI8 recurse,publics,map;
  LtfBinEntry *tempb;
  DbgInfo *dbg;
  IndentInfo *indent;
  BoolI8 old_preempt=Preempt(OFF);

  GetFFlags(&f,Define("ST_FILE_UTIL_FLAGS"),"+r+p");
  GetFFlags(&f,Define("ST_FILE_UTIL_FLAGS"),flags);
  recurse=Bt(&f,FUf_RECURSE);
  publics=Bt(&f,FUf_PUBLIC);
  map	 =Bt(&f,FUf_MAP);

  if (!h) h=Fs->hash_table;

  if (l_)
    l=l_;
  else if (outname) {
    l=LtfNew;
    l->flags|=LTFF_NO_CURSOR;
    StrCpy(l->filename.name,outname);
  } else
    l=NULL;

  cnt=0;
  tempht=h;
  while (tempht) {
    temph=tempht->next_added;
    while (temph!=tempht) {
      if (!(temph->type & (HTF_IMPORT | HTF_PRIVATE)) &&
	  temph->type & mask &&
	  (temph->type & HTF_PUBLIC || !publics))
	if (!index)
	  cnt++;
	else
	  if (temph->type&HTT_SRC_SYM &&
	    (cur_index=temph->index))
	  cnt+=HelpIndexCnt(cur_index,index);
      temph=temph->next_added;
    }
    if (recurse)
      tempht=tempht->next;
    else
      break;
  }
  if (!cnt) goto end_hash;

  list=CAlloc(cnt*sizeof(HashRepStruct));
  j=0;
  tempht=h;
  while (tempht) {
    temph=tempht->next_added;
    while (temph!=tempht) {
      if (!(temph->type & (HTF_IMPORT | HTF_PRIVATE)) &&
	  temph->type & mask &&
	  (temph->type & HTF_PUBLIC || !publics))
	if (!index)
	  list[j++].h=temph;
	else
	  if (temph->type&HTT_SRC_SYM &&
	    (cur_index=temph->index))
	  if (k=HelpIndexCnt(cur_index,index)) {
	    while (k--) {
	      list[j].index=HelpIndexStr(&cur_index,index);
	      list[j++].h=temph;
	    }
	  }
      temph=temph->next_added;
    }
    if (recurse)
      tempht=tempht->next;
    else
      break;
  }
  Preempt(old_preempt);

  if (map)
    QSort(list,cnt,sizeof(HashRepStruct),&HashEntriesCompare2);
  else if (index)
    QSort(list,cnt,sizeof(HashRepStruct),&HashEntriesCompare3);
  else
    QSort(list,cnt,sizeof(HashRepStruct),&HashEntriesCompare);

  for (i=0;i<cnt;i++) {
    ptr=list[i].h;
    dbg=NULL;
    if (index)
      if (cur_index=list[i].index) {
	if (StrCmp(cur_index,last_index)) {
	  Free(last_index);
	  last_index=StrNew(cur_index);
	  if (l)
	    LtfPrintF(l,"$$FG,GREEN$$%s$$FG$$\n",cur_index);
	  else
	    "$$FG,GREEN$$%s$$FG$$\n",cur_index;
	}
      }
    if (index && ptr->type & HTT_HELP_FILE) {
      if (l) {
	l1=LtfRead(ptr->str);
	LtfInsertLtf(l,l1);
	LtfDel(l1);
      } else
	Type(ptr->str);
    } else {
      if (ptr->type&HTT_SRC_SYM && ptr(SrcSymHashEntry *)->src_link) {
	buf4=StrNew(ptr(SrcSymHashEntry *)->src_link);
	if (mapfile_drv && StrLen(buf4)>3)
	  buf4[3]=mapfile_drv;
	dbg=indent=NULL;
	if (outname && l && ptr->type & HTT_FUN) {
	  if (dbg=ptr(FunStruct *)->debug) {
	    size=offset(DbgInfo.body)+
		sizeof(U0 *)*
		(dbg->max_line-dbg->min_line+2);
	    if (size>MSize(dbg)) {
	      "Corrupt Map Entry\n";
	      dbg=NULL;
	    } else {
	      if (dbg->min_line<=dbg->max_line) {
		tempb=CAlloc(sizeof(LtfBinEntry));
		tempb->type=LTFBT_DEBUG_LINES;
		tempb->size=size;
		tempb->data=MAlloc(size);
		MemCpy(tempb->data,dbg,size);
		dbg_bin_num=tempb->num=l->cur_bin_num++;
		tempb->use_cnt=1;
		InsQue(tempb,l->bin_root.last);
	      } else
		dbg=NULL;
	    }
	  }
	  if (indent=ptr(FunStruct *)->indent) {
	    size=offset(IndentInfo.body)+
		sizeof(U8)*
		(indent->max_line-indent->min_line+2);
	    if (size>MSize(indent)) {
	      "Corrupt Indent Entry\n";
	      indent=NULL;
	    } else {
	      if (indent->min_line<=indent->max_line) {
		tempb=CAlloc(sizeof(LtfBinEntry));
		tempb->type=LTFBT_INDENTS;
		tempb->size=size;
		tempb->data=MAlloc(size);
		MemCpy(tempb->data,indent,size);
		indent_bin_num=tempb->num=l->cur_bin_num++;
		tempb->use_cnt=1;
		InsQue(tempb,l->bin_root.last);
	      } else
		indent=NULL;
	    }
	  }
	}
	if (dbg || indent) {
	  if (dbg)  {
	    SPrintF(buf,"$$LK+BI,\"%-18s\",\"%s\",%d$$",
		ptr->str,buf4,dbg_bin_num);
	    LtfPutS(l,buf);
	  }
	  if (indent) {
	    if (dbg)
	      SPrintF(buf,"$$LK+BI,\"\",\"%s\",%d$$",
		  ptr->str,indent_bin_num);
	    else
	      SPrintF(buf,"$$LK+BI,\"%-18s\",\"%s\",%d$$",
		  ptr->str,buf4,indent_bin_num);
	    LtfPutS(l,buf);
	  }
	} else {
	  SPrintF(buf,"$$LK,\"%-18s\",\"%s\"$$",
	      ptr->str,buf4);
	  if (l)
	    LtfPutS(l,buf);
	  else
	    "" buf;
	}
	Free(buf4);
      } else {
	SPrintF(buf,"%-18ts",ptr->str);
	if (l)
	  LtfPutS(l,buf);
	else
	  "" buf;
      }

      if (!index) {
	if (ptr->type & HTT_DEFINE_STR)
	  SPrintF(buf,"%-8tQ %04tX",ptr(DefineStrHashEntry *)->data,ptr(DefineStrHashEntry *)->cnt);
	else if (ptr->type & HTT_GLBL_VAR) {
	  if (outname)
	    SPrintF(buf,"%08X",ptr(GlblVarStruct *)->data_addr_ip);
	  else
	    SPrintF(buf,"%08X",ptr(GlblVarStruct *)->data_addr);
	} else
	  SPrintF(buf,"%08X",HashVal(ptr));
	SPrintF(buf+StrLen(buf)," %04X ",ptr->use_cnt);
      } else
	*buf=0;

      k=ptr->type;
      if (publics)
	k&=~HTF_PUBLIC;

      for (j=0;j<32;j++)
	if (Bt(&k,j))
	  SPrintF(buf+StrLen(buf),"%Z ",j,"ST_HTT_TYPES");
      StrCat(buf,"\n");
      if (l)
	LtfPutS(l,buf);
      else
	"" buf;
    }
    Free(list[i].index);
  }
  Free(list);

end_hash:
  Preempt(old_preempt);
  if (l) {
    LtfBinsValidate(l);
    if (!l_) {
      LtfWrite(l);
      LtfDel(l);
    } else {
      if (l->root.next==l)
	LtfPutS(l,"No Match");
      else
	LtfRecalc(l);
    }
  }
  Free(last_index);
}

public U0 HFunsRep(U8 *flags="",U8 *index=NULL)
{
  HashRep(Fs->hash_table,HTT_FUN,NULL,flags,0,index);
}

public U0 HMapRep(U8 *flags="-p+m",U8 *index=NULL)
{
  HashRep(Fs->hash_table,HTT_FUN,NULL,flags,0,index);
}

public U0 HVarsRep(U8 *flags="",U8 *index=NULL)
{
  HashRep(Fs->hash_table,HTT_GLBL_VAR,NULL,flags,0,index);
}

public U0 HClassesRep(U8 *flags="-p",U8 *index=NULL)
{
  HashRep(Fs->hash_table,HTT_CLASS,NULL,flags,0,index);
}

public U0 HDefinesRep(U8 *flags="-p",U8 *index=NULL)
{
  HashRep(Fs->hash_table,HTT_DEFINE_STR,NULL,flags,0,index);
}

public U0 HSymsRep(U8 *flags="-p",U8 *index=NULL)
{
  HashRep(Fs->hash_table,HTT_EXPORT_SYS_SYM,NULL,flags,0,index);
}

public U0 HAllRep(U8 *flags="-p",U8 *index=NULL)
{
  HashRep(Fs->hash_table,HTT_ALL,NULL,flags,0,index);
}

#define HDR_MAX	16
public I64 HashDepthRep(SysHashTable *tempht=NULL)
{
  I64 i,j,longest=0,cnt=0,a[HDR_MAX];
  SysHashEntry *temph;
  if (!tempht) tempht=Fs->hash_table;
  MemSet(a,0,sizeof(a));
  for (i=0;i<=tempht->mask;i++) {
    temph=tempht->body[i];
    if (temph) {
      j=1;
      while (temph->next) {
	temph=temph->next;
	j++;
      }
      if (j<HDR_MAX)
	a[j]++;
      cnt+=j;
      if (j>longest)
	longest=j;
    }
  }
  "Histogram\n";
  for (i=0;i<HDR_MAX;i++)
    if (a[i])
      "%02d:%d\n",i,a[i];
  "Size:%dCount:%dLongest:%d\n",
    tempht->mask+1,cnt,longest;
  return longest;
}

#help_index "Help System;Training"
#help_file "::/LT/Doc/HelpSystem"

public U0 LtfHelpIndex(Ltf *l,U8 *index)
{
  HashRep(Fs->hash_table,HTT_ALL,NULL,NULL,0,index,l);
}

public U0 PopUpHelpIndex(U8 *index,TaskStruct *parent=NULL)
{
  U8 *buf;
  buf=MSPrintF("LtfHelpIndex(LtfCur,\"%s\");View;",index);
  PopUp(buf,parent);
  Free(buf);
}

#help_index "Hash"
public U0 MapFileLoad(U8 *filename)
{
  U8 *st,*ptr,*name=DftExt(filename,"MPZ");
  U8 *absname=FileNameAbs(name);
  Ltf *l=LtfRead(name);
  LtfEntry *cl;
  SrcSymHashEntry *temph;
  U64 i,j,base=0;
  DbgInfo *dbg;
  IndentInfo *indent;

  FileExtRem(absname);
  if (absname[1]==':' && StrLen(absname)>2 &&
      (temph=HashSingleTableFind(absname+2,Fs->hash_table,HTT_MODULE)))
      base=temph(GenericHashEntry *)->user_data0+sizeof(BinFileHeaderStruct);

  if (!l) return;
  cl=l->root.next;
  while (cl!=l) {
    if (cl->type_u8==LTFT_LINK) {
      if (*cl->tag)
	st=MStrUtil(cl->tag,SUF_REM_TRAILING);
      else
	st=MStrUtil(cl->aux_str,SUF_REM_TRAILING);
      if (temph=HashSingleTableFind(st,Fs->hash_table,HTT_SRC_SYM)) {
	if (*cl->tag) {
	  Free(temph->src_link);
	  temph->src_link=cl->aux_str;

	  ptr=temph->src_link;
	  if (ptr[0] && ptr[1] && ptr[2]==':' && ptr[3]==':')
	    ptr[3]=dft_drv;
 
	  cl->aux_str=NULL;
	}
	if (temph->type&(HTT_FUN|HTT_EXPORT_SYS_SYM)) {
	  dbg	=temph->debug;
	  indent=temph->indent;
	  if (!dbg && cl->bin_data && cl->bin_data->type==LTFBT_DEBUG_LINES) {
	    if (dbg=cl->bin_data->data) {
	      if (cl->bin_data->size>MSize(dbg))
		"Corrupt Map Entry\n";
	      else {
		cl->bin_data->data=NULL;
		temph->debug=dbg;
		for (i=dbg->min_line;i<=dbg->max_line+1;i++) {
		  j=i-dbg->min_line;
		  if (dbg->body[j])
		    dbg->body[j]=dbg->body[j]+base;
		}
	      }
	    }
	  }
	  if (!indent && cl->bin_data && cl->bin_data->type==LTFBT_INDENTS) {
	    if (indent=cl->bin_data->data) {
	      if (cl->bin_data->size>MSize(indent))
		"Corrupt Indent Entry\n";
	      else {
		cl->bin_data->data=NULL;
		temph->indent=indent;
	      }
	    }
	  }
	}
      }
      Free(st);
    }
    cl=cl->next;
  }
  LtfDel(l);
  Free(name);
  Free(absname);
}
#help_index ""
