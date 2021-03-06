U8 *CmpPmt(BoolI8 dbl_semicolon=FALSE)
{
  I64 i;
  U8 *st,*result;

  for (i=MinI64(3,Fs->answers_displayed-1);i>=0;i--) {
    if (Fs->answer_types[i]==IT_F64)
      "ansf%d=%15.7g=%15e\n",i,Fs->answers[i],Fs->answers[i];
    else
      "ans%d=0x%016X=%d\n",i,Fs->answers[i],Fs->answers[i];
  }
  if (st=CurDir) {
    "" st;
    Free(st);
  }
  '>';
  st=MGetS(GETSF_SHIFT_ESC_EXIT);
  if (dbl_semicolon) {
    i=StrLen(st);
    result=MAlloc(i+4);
    MemCpy(result,st,i+1);
    i--;
    while (i>=0 && Bt(white_space_bitmap,result[i]))
      i--;
    i++;
    if (i>0 && result[i-1]==';')
      result[i++]=';'; //The Lex goes one beyond
    result[i++]=13;//#define goes to <cr>
    result[i++]=10;
    result[i]=0;
  } else
    result=StrNew(st);
  Free(st);
  return result;
}

LexStruct *LexNew(U8 *buf,U64 flags,U8 *filename=NULL)
{
  LexStruct *lx=MAlloc(sizeof(LexStruct));
  LexFileStruct *tempf=&lx->lfns[0];
  MemSet(lx,0,offset(LexStruct.end_of_zeroed_members)+sizeof(LexFileStruct));
  lx->next=lx->last=lx;
  lx->flags=flags;
  lx->locked_flags=0;
  lx->opts=1<<OPTf_UNUSED_VAR_WARN;
  lx->start_line_num=lx->last_line_num=1;
  lx->pass_trace_mask=0b10001111101;
  lx->hash_mask=HTT_ALL-HTT_IMPORT_SYS_SYM-HTT_ASM_KEYWORD;
  lx->define_hash_table=lx->hash_table_list=lx->glbl_hash_table=lx->local_hash_table=Fs->hash_table;
  if (flags&LF_KEEP_AT_SIGN)
    lx->alpha_numeric_bitmap=alpha_numeric_no_at_bitmap;
  else
    lx->alpha_numeric_bitmap=alpha_numeric_bitmap;
  lx->cur_lfn=tempf;
  lx->next_stream_blk=lx->last_stream_blk=&lx->next_stream_blk;
  if (!filename)
    tempf->name=StrNew(sys_temp_filename);
  else
    tempf->name=filename;
  if (flags & LF_PROMPT)
    buf=CAlloc(4);
  tempf->buf=tempf->buf_ptr=tempf->line_start=lx->cur_buf_ptr=buf;
  tempf->line_num=1;
  return lx;
}

U0 LexDel(LexStruct *lx)
{
  LexFileStruct *tempf=lx->cur_lfn;
  Ltf *l;
  while (tempf!=&lx->lfns[0]) {
    if (l=tempf->l)
      LtfDel(l);
    Free(tempf->buf);
    Free(tempf->name);
    tempf--;
  }
  Free(tempf->name);
  if (!(lx->flags & LF_DONT_FREE_BUF))
    Free(tempf->buf);
  Free(lx->ident);
  Free(lx->dollar);
  Free(lx);
}

U64 LexSize(LexStruct *lx)
{
  U64 result=0;
  LexFileStruct *tempf=lx->cur_lfn;
  Ltf *l;
  while (tempf!=&lx->lfns[0]) {
    if (l=tempf->l)
      result+=LtfSize(l);
    result+=MSize2(tempf->buf);
    result+=MSize2(tempf->name);
    tempf--;
  }
  result+=MSize2(tempf->name);
  if (!(lx->flags & LF_DONT_FREE_BUF))
    result+=MSize2(tempf->buf);
  result+=MSize2(lx->ident);
  result+=MSize2(lx);
  return result;
}

U0 SetSrcFileInfo(LexStruct *lx,SrcSymHashEntry *h)
{
  LexFileStruct *tempf=lx->cur_lfn;
  h->src_link=MSPrintF("FL:%s,%d",tempf->name,tempf->line_num);
  if (Bt(&lx->opts,OPTf_KEEP_PRIVATE))
    h->type|=HTF_PRIVATE;
  if (lx->cur_index)
    h->index=StrNew(lx->cur_index);
  else
    h->index=NULL;
}

U0 LexPutToken(LexStruct *lx)
{
  '"';
  if (lx->token==TK_IDENT || lx->token==TK_STR)
    "" lx->ident;
  else if (lx->token==TK_INTEGER)
    "INT:%X",lx->cur_i;
  else if (lx->token==TK_U8)
    "U8:%X",lx->cur_i;
  else if (lx->token==TK_F64)
    "FLOAT:%e",lx->cur_f;
  else if (lx->token==CH_NEW_LINE)
    "<NEW_LINE>";
  else if (Bt(displayable_chars_bitmap,lx->token))
    '' lx->token;
  else {
    "T:%X",lx->token;
  }
  "\" ";
}

U32 lex_zeros=0;

U0 LexBackupLastChar(LexStruct *lx)
{
  LexFileStruct *tempf=lx->cur_lfn;
  tempf->buf_ptr=lx->cur_buf_ptr;
  if (lx->flags & LF_USE_LAST_U16) {
    tempf->last_U16=lx->last_U16;
    lx->flags&=~LF_USE_LAST_U16;
  } else
    tempf->last_U16=0;
}

BoolI64 LexDollar(LexStruct *lx,Ltf *l,LtfEntry *ll)
{
  U8 *st;
  if (lx->flags&LF_IN_QUOTES) {
    Free(lx->dollar);
    st=Ltf2PlainText(l,ll);
    lx->dollar=MSPrintF("$$%Q$$",st);
    lx->dollar_cnt=2;
    Free(st);
    return TRUE;
  } else
    return FALSE;
}

U64 LexGetChar(LexStruct *lx)
{
  U8 *src;
  LexFileStruct *tempf,*tempf1;
  BoolI8 cont;
  U64 result;
  Ltf *l;
  LtfEntry *cl;
  U8 *ptr;
  if (lx->flags & LF_USE_LAST_U16) {
    lx->flags&=~LF_USE_LAST_U16;
    if (lx->last_U16==CH_SHIFT_SPACE)
      return CH_SPACE;
    else
      return lx->last_U16;
  }
  while (TRUE) {
lex_getchar_start:
    if (!(src=lx->cur_buf_ptr++)) {
      lx->cur_buf_ptr--;
      goto lgc_here2;
    }
    nobound_switch (lx->last_U16=result=*src++) {
      case 0:
lgc_here2:
	tempf=lx->cur_lfn;
	if (tempf->flags & LFSF_IS_LTF) {
	  l=tempf->l;
	  cl=tempf->cur_entry;
	  cl=cl->next;
	  while (cl!=l) {
	    cont=FALSE;
	    tempf->cur_entry=cl;
	    nobound_switch (cl->type_u8) {
	      case LTFT_TEXT:
		if (cl->flags & ~(LTF_BL_IV_UL|LTFLF_WORD_WRAP|LTFLF_HIGHLIGHT|LTFLF_LOCK_TO_PAGE|LTFLF_LOCK_TO_CENTER|LTFLF_SKIP|LTFLF_HARD_SKIP|LTFLF_FILTER_SKIP) &&
		    LexDollar(lx,l,cl) && *(src=lx->dollar)) {
		  tempf->line_num=cl->y+1;
		  tempf->buf=tempf->buf_ptr=lx->cur_buf_ptr=src;
		} else if (*(src=cl->tag))
		  tempf->buf=tempf->buf_ptr=lx->cur_buf_ptr=src;
		else {
		  cont=TRUE;
		  cl=cl->next;
		}
		break;
	      case LTFT_NEW_LINE:
		tempf->buf=tempf->buf_ptr=lx->cur_buf_ptr=&lex_zeros;
		tempf->line_start=cl->next;
		tempf->line_num=cl->y+2;	//+1 because NEW_LINE is on previous line, +1 because l y starts at zero
		compiled_lines++;
		if (lx->opts & OPTF_ECHO)
		  NewLine;
		lx->last_U16=CH_NEW_LINE;
		return CH_NEW_LINE;
	      case LTFT_TAB:
		tempf->buf=tempf->buf_ptr=lx->cur_buf_ptr=&lex_zeros;
		tempf->line_num=cl->y+1;
		if (lx->opts & OPTF_ECHO)
		  '' CH_TAB;
		lx->last_U16=CH_TAB;
		return CH_TAB;
	      case LTFT_PAGE_BREAK:
		tempf->buf=tempf->buf_ptr=lx->cur_buf_ptr=&lex_zeros;
		tempf->line_num=cl->y+2;	//TODO: should be a page down
		if (lx->opts & OPTF_ECHO)
		  '' CH_FORM_FEED;
		lx->last_U16=CH_FORM_FEED;
		return CH_FORM_FEED;
	      case LTFT_INSERT_BINARY:
		tempf->buf=tempf->buf_ptr=lx->cur_buf_ptr=&lex_zeros;
		tempf->line_num=cl->y+1;
		Free(lx->ident);
		lx->ident=NULL;
		lx->ident_len=0;
		if (cl->bin_data) {
		  ptr=MAlloc(cl->bin_data->size);
		  if (cl->bin_data->data)
		    MemCpy(ptr,cl->bin_data->data,cl->bin_data->size);
		  lx->ident=ptr;
		  lx->ident_len=cl->bin_data->size;
		}
		lx->last_U16=TK_INSERT_BINARY;
		return TK_INSERT_BINARY;
	      case LTFT_INSERT_BINARY_TYPE:
		tempf->buf=tempf->buf_ptr=lx->cur_buf_ptr=&lex_zeros;
		if (cl->bin_data)
		  lx->cur_i=cl->bin_data->type;
		else
		  lx->cur_i=0;
		tempf->line_num=cl->y+1;
		lx->last_U16=TK_INSERT_BINARY_TYPE;
		return TK_INSERT_BINARY_TYPE;
	      case LTFT_INSERT_BINARY_SIZE:
		tempf->buf=tempf->buf_ptr=lx->cur_buf_ptr=&lex_zeros;
		if (cl->bin_data)
		  lx->cur_i=cl->bin_data->size;
		else
		  lx->cur_i=0;
		tempf->line_num=cl->y+1;
		lx->last_U16=TK_INSERT_BINARY_SIZE;
		return TK_INSERT_BINARY_SIZE;
	      case LTFT_SHIFTED_Y:
		if (LexDollar(lx,l,cl) && *(src=lx->dollar)) {
		  tempf->line_num=cl->y+1;
		  tempf->buf=tempf->buf_ptr=lx->cur_buf_ptr=src;
		} else {
		  tempf->buf=tempf->buf_ptr=lx->cur_buf_ptr=&lex_zeros;
		  tempf->line_num=cl->y+1;
		  if (cl->attr<0) {
		    lx->last_U16=TK_SUPERSCRIPT;
		    return TK_SUPERSCRIPT;
		  } else if (cl->attr>0) {
		    lx->last_U16=TK_SUBSCRIPT;
		    return TK_SUBSCRIPT;
		  } else {
		    lx->last_U16=TK_NORMALSCRIPT;
		    return TK_NORMALSCRIPT;
		  }
		}
		break;
	      case LTFT_CURSOR:
		cont=TRUE;
		cl=cl->next;
		break;
	      case 0xFF: //nobound_switch
	      default:
		if (LexDollar(lx,l,cl) && *(src=lx->dollar)) {
		  tempf->line_num=cl->y+1;
		  tempf->buf=tempf->buf_ptr=lx->cur_buf_ptr=src;
		} else {
		  cont=TRUE;
		  cl=cl->next;
		}
	    }
	    if (!cont) break;
	  }
	  if (cl==l) {
	    LtfDel(l);
	    tempf->l=tempf->buf=NULL;
	    Free(tempf->name);
	    tempf->name=NULL;
	    goto lgc_here;
	  }
	} else {
lgc_here:
	  tempf=lx->cur_lfn;
	  if (tempf!=&lx->lfns[0]) {
	    Free(tempf->name);
	    tempf->name=NULL;
	    Free(tempf->buf);
	    tempf->buf=NULL;
	    tempf1=tempf;
	    lx->cur_lfn=--tempf;
	    lx->cur_buf_ptr=tempf->buf_ptr;
	    if (lx->last_U16=tempf->last_U16) {
	      lx->flags&=~LF_USE_LAST_U16;
	      if (lx->last_U16==CH_SHIFT_SPACE)
		return CH_SPACE;
	      else
		return lx->last_U16;
	    } else
	      lx->flags&=~LF_USE_LAST_U16;
	  } else {
	    if (lx->flags & LF_PROMPT) {
	      Free(tempf->buf);
	      lx->cur_buf_ptr=tempf->line_start=
		  tempf->buf_ptr=tempf->buf=CmpPmt(TRUE);
	    } else {
	      lx->last_U16=TK_EOF;
	      lx->cur_buf_ptr=src-1;
	      return TK_EOF;
	    }
	  }
	}
	break;
      case CH_CURSOR:
      case CH_FORM_FEED:
	goto lex_getchar_start;
      case CH_NEW_LINE:
	tempf=lx->cur_lfn;
	if (tempf->flags & LFSF_IS_LTF) {
	  if (result==CH_SHIFT_SPACE)
	    return CH_SPACE;
	  else
	    return result;
	} else {
	  tempf->line_num++;
	  compiled_lines++;
	  tempf->line_start=src;
	}
      case 0xFF: //nobound_switch
      default:
	if (lx->opts & OPTF_ECHO)
	  '' result;
	if (result==CH_SHIFT_SPACE)
	  return CH_SPACE;
	else
	  return result;
    }
  }
}

U0 LexSkipEol(LexStruct *lx)
{
  U8 ch;
  do ch=$WW,0$LexGetChar(lx);
  while (Bt(non_eol_chars_bitmap,ch));
}

U0 ExpressionDel(U8 *code)
{
  Free(code);
}

U0 LexIncludeStr(LexStruct *lx,U8 *abs_fname,
  U8 *src,BoolI8 actual_file)
{
  LexFileStruct *tempf;
  LexBackupLastChar(lx);
  tempf=++lx->cur_lfn;
  tempf->buf=src;
  tempf->buf_ptr=src;
  tempf->line_num=1;
  tempf->line_start=src;
  tempf->last_U16=0;
  if (actual_file) {
    tempf->name=StrNew(abs_fname);
    tempf->flags=LFSF_IS_INCLUDED;
  } else {
    tempf->name=StrNew(sys_temp_filename);
    tempf->flags=0;
  }
  lx->cur_buf_ptr=src;
}

U64 ltf_stop_btypes[(LTFT_NUM_TYPES+63)/64]=
{
1<<LTFT_TEXT|
1<<LTFT_TAB|
1<<LTFT_INSERT_BINARY|
1<<LTFT_INSERT_BINARY_TYPE|
1<<LTFT_INSERT_BINARY_SIZE
};


Ltf *LexLtfRead(U8 *abs_fname,U64 flags)
{
  Ltf *l=LtfNew;
  U8 *src;
  U64 size=0;
  l->flags|=flags;
  StrCpy(l->filename.name,abs_fname);
  src=FileRead(abs_fname,&size);
  if (!src || !size) {
    Free(src);
    src=CAlloc(1);
    size=0;
  }
  LtfLoad(l,src,size);
  Free(src);
  return l;
}

U0 LexIncludeLtf(LexStruct *lx,U8 *abs_fname,Ltf *_l)
{ //If ltf==null load
  BoolI8 is_null;
  LtfEntry *cl;
  LexFileStruct *tempf;
  Ltf *l;
  LexBackupLastChar(lx);
  tempf=++lx->cur_lfn;
  tempf->line_start=&lex_zeros;
  if (_l)
    l=_l;
  else
    l=LexLtfRead(abs_fname,LTFF_DBL_DOLLARS);
  tempf->l=l;
  cl=l->root.next;
  while (cl!=l) {
    if (Bt(ltf_stop_btypes,cl->type_u8))
      break;
    cl=cl->next;
  }
  if (cl!=l) {
    tempf->line_start=cl;
    tempf->line_num=cl->y+1;
    if (cl->type_u8==LTFT_TEXT) {
      tempf->cur_entry=cl;
      tempf->buf=cl->tag;
      tempf->buf_ptr=cl->tag;
    } else {
      tempf->cur_entry=cl->last;  //TODO might be problem at begining of file
      tempf->buf=&lex_zeros;
      tempf->buf_ptr=&lex_zeros;
    }
    is_null=FALSE;
  } else
    is_null=TRUE;
  if (!abs_fname)
    tempf->name=StrNew(sys_temp_filename);
  else
    tempf->name=StrNew(abs_fname);
  if (is_null) {
//    LtfDel(lx); TODO
    tempf->flags=LFSF_IS_INCLUDED;
    tempf->buf=CAlloc(1);
    tempf->buf_ptr=&lex_zeros;
    tempf->line_num=1;
  } else
    tempf->flags=LFSF_IS_INCLUDED|LFSF_IS_LTF;
  tempf->last_U16=0;
  lx->cur_buf_ptr=tempf->buf_ptr;
}

U0 LexPush(LexStruct *lx)
{
  LexFileStruct *tempf=lx->cur_lfn,
    *tempf1=&lx->stk[lx->stk_ptr++];
  if (lx->stk_ptr>=LFN_STK_CNT)
    PrsExcept(lx,"ST_LEX_STK_OVERFLOW");
  LexBackupLastChar(lx);
  if (tempf->last_U16)
    lx->flags|=LF_USE_LAST_U16;
  MemCpy(tempf1,tempf,sizeof(LexFileStruct));
}

U0 LexPop(LexStruct *lx)
{
  LexFileStruct *tempf=lx->cur_lfn,
    *tempf1=&lx->stk[--lx->stk_ptr];
  lx->cur_buf_ptr=tempf1->buf_ptr;
  if (lx->last_U16=tempf1->last_U16)
    lx->flags|=LF_USE_LAST_U16;
  else
    lx->flags&=~LF_USE_LAST_U16;
  MemCpy(tempf,tempf1,sizeof(LexFileStruct));
}

I64 MemberMetaData(U8 *st,MemberListStruct *tempm)
{
  MemberListMeta *meta=tempm->meta;
  while (meta) {
    if (!StrCmp(meta->str,st))
      return meta->user_data;
    meta=meta->next;
  }
  return 0;
}

MemberListMeta *MemberMetaFind(U8 *st,MemberListStruct *tempm)
{
  MemberListMeta *meta=tempm->meta;
  while (meta) {
    if (!StrCmp(meta->str,st))
      return meta;
    meta=meta->next;
  }
  return NULL;
}

MemberListStruct *MemberFind(U8 *st,ClassStruct *tempc)
{
  I64 i;
  MemberListStruct *tempm;
  do {
    tempm=tempc->member_list;
    while (tempm) {
      if (!(i=StrCmp(tempm->str,st))) {
	tempm->use_cnt++;
	return tempm;
      }
      if (i<=0)
	tempm=tempm->left;
      else
	tempm=tempm->right;
    }
  } while (tempc=tempc->base_class);
  return NULL;
}

U0 MemberAdd(LexStruct *lx,MemberListStruct *tempm,ClassStruct *tempc)
{
  U8 *st=tempm->str;
  MemberListStruct **tempm1,*tempm2;
  if (MemberFind(st,tempc) && StrCmp(st,"pad") && StrCmp(st,"reserved") && StrCmp(st,"_anon_"))
    PrsExcept(lx,"ST_DUP_MEMBER");
  tempm1=&tempc->member_list;
  while (tempm2=*tempm1) {
    if (StrCmp(tempm2->str,st)<=0)
      tempm1=&tempm2->left;
    else
      tempm1=&tempm2->right;
  }
  *tempm1=tempm;
  tempm->left=NULL;
  tempm->right=NULL;
  tempm2=tempc->last_in_member_list;
  tempm2->next=tempc->last_in_member_list=tempm;
}

U0 MemberListDel(ClassStruct *tempc)
{
  MemberListStruct *tempm,*tempm1;
  MemberListMeta *temp_meta,*temp_meta1;
  tempm=tempc->member_list;
  while (tempm) {
    tempm1=tempm->next;
    Free(tempm->str);
    LinkedListDel(tempm->dim.next);
    if (tempm->flags & MLF_STR_DFT_AVAILABLE)
      Free(tempm->dft_val);
    if (tempm->flags & MLF_FUN)
      HashDel(tempm->fun_ptr-tempm->fun_ptr->ptr_cnt);
    temp_meta=tempm->meta;
    while (temp_meta) {
      temp_meta1=temp_meta->next;
      Free(temp_meta->str);
      if (temp_meta->flags&MLMF_IS_STR)
	Free(temp_meta->user_data);
      Free(temp_meta);
      temp_meta=temp_meta1;
    }
    Free(tempm);
    tempm=tempm1;
  }
  tempc->size=0;
  tempc->last_in_member_list=&tempc->member_list;
  tempc->member_list=NULL;
  tempc->member_cnt=0;
  if (tempc->type&HTT_FUN)
    tempc(FunStruct *)->arg_cnt=0;
}

#assert !offset(MemberListStruct.next)
#assert !offset(MemberListMeta.next)

MemberListStruct *MemberListCopy(ClassStruct *tempc)
{
  MemberListStruct *result=NULL,*tempm,*tempm1=&result;
  MemberListMeta *temp_meta,*temp_meta1;
  tempm=tempc->member_list;
  while (tempm) {
    tempm1=tempm1->next=MAllocIdentical(tempm);
    tempm1->str=MAllocIdentical(tempm->str);
    tempm1->dim.next=LinkedListCopy(tempm->dim.next);
    if (tempm->flags & MLF_STR_DFT_AVAILABLE)
      tempm1->dft_val=MAllocIdentical(tempm->dft_val);
    if (tempm->flags & MLF_FUN)
      tempm1->fun_ptr=HashEntryImportCopy(tempm->fun_ptr-tempm->fun_ptr->ptr_cnt)(FunStruct *)+tempm->fun_ptr->ptr_cnt;

    temp_meta=tempm->meta;
    temp_meta1=&tempm1->meta;
    while (temp_meta) {
      temp_meta1=temp_meta1->next=MAllocIdentical(temp_meta);
      temp_meta1->str=MAllocIdentical(temp_meta->str);
      if (temp_meta->flags&MLMF_IS_STR)
	temp_meta1->user_data=MAllocIdentical(temp_meta->user_data);
      temp_meta=temp_meta->next;
    }
    tempm=tempm->next;
  }
  return result;
}

U64 MemberListSize(ClassStruct *tempc)
{
  U64 result=0;
  MemberListStruct *tempm;
  MemberListMeta *temp_meta;
  tempm=tempc->member_list;
  while (tempm) {
    result+=MSize2(tempm->str);
    result+=LinkedListSize(tempm->dim.next);
    if (tempm->flags & MLF_STR_DFT_AVAILABLE)
      result+=MSize2(tempm->dft_val);
    if (tempm->flags & MLF_FUN)
      result+=HashEntrySize(tempm->fun_ptr-tempm->fun_ptr->ptr_cnt);
    temp_meta=tempm->meta;
    while (temp_meta) {
      result+=MSize2(temp_meta->str);
      if (temp_meta->flags&MLMF_IS_STR)
	result+=MSize2(temp_meta->user_data);
      result+=MSize2(temp_meta);
      temp_meta=temp_meta->next;
    }
    result+=MSize2(tempm);
    tempm=tempm->next;
  }
  return result;
}

I64 LexInStr(LexStruct *lx,
  U8 *buf,I64 size,BoolI8 *done)
{
  I64 i=0,j,ch;
  *done=TRUE;
  while (i<size-1) {
    ch=LexGetChar(lx);
    if (!ch || ch=='"') {
      buf[i++]=0;
      return i;
    } else if (ch=='\\') {
      switch (ch=LexGetChar(lx)) {
	case 'n':
	  buf[i++]=CH_NEW_LINE;
	  break;
	case 'r':
	  buf[i++]=CH_CARRIAGE_RETURN;
	  break;
	case '"':
	  buf[i++]='"';
	  break;
	case '\'':
	  buf[i++]='\'';
	  break;
	case '\`':
	  buf[i++]='\`';
	  break;
	case '\\':
	  buf[i++]='\\';
	  break;
	case 't':
	  buf[i++]=CH_TAB;
	  break;
	case 'x':
	case 'X':
	  j=0;
	  while (TRUE) {
	    ch=ToUpper(LexGetChar(lx));
	    if (Bt(hex_numeric_bitmap,ch)) {
	      if (ch<='9')
		j=j<<4+ch-'0';
	      else
		j=j<<4+ch-'A'+10;
	    } else
	      break;
	  }
	  buf[i++]=j;
	  lx->flags|=LF_USE_LAST_U16;
	  break;
	case 'b':
	case 'B':
	  j=0;
	  while (TRUE) {
	    ch=LexGetChar(lx);
	    if ('0'<=ch<='1')
	      j=j<<1+ch-'0';
	    else
	      break;
	  }
	  buf[i++]=j;
	  lx->flags|=LF_USE_LAST_U16;
	  break;
	case 'o':
	case 'O':
	  j=0;
	  goto lis_octal;
	case '0'...'7':
	  j=ch-'0';
lis_octal:
	  while (TRUE) {
	    ch=LexGetChar(lx);
	    if ('0'<=ch<='7')
	      j=j<<3+ch-'0';
	    else
	      break;
	  }
	  buf[i++]=j;
	  lx->flags|=LF_USE_LAST_U16;
	  break;
	default:
	  lx->flags|=LF_USE_LAST_U16;
	  buf[i++]='\\';
      }
    } else if (ch=='$$') {
      buf[i++]='$$';
      if (lx->dollar_cnt)
	lx->dollar_cnt--;
      else if (LexGetChar(lx)!='$$') {
	lx->dollar_cnt=1;
	lx->flags|=LF_USE_LAST_U16;
      }
    } else
      buf[i++]=ch;
  }
  *done=FALSE;
  return i;
}

I64 Lex(LexStruct *lx)
{
  U64 ch;
  I64 i,j,k;
  SysHashEntry *temph;
  BoolI8 str_done,neg_e;
  U8 *fbuf;
  U8 *buf2,*buf3,buf[LEX_MAX_IDENT];
  lx->last_indent_level=lx->indent_level;
  lx->last_line_num=lx->cur_lfn->line_num;
  while (TRUE) {
lex_cont:
    lx->start_line_num=lx->cur_lfn->line_num;
    nobound_switch (ch=LexGetChar(lx)) {
      case 0:
	return lx->token=TK_EOF;

      case TK_SUPERSCRIPT:
	ch='>';
	goto lex_ident;
      case TK_SUBSCRIPT:
	ch='<';
	goto lex_ident;
      case TK_NORMALSCRIPT:
	ch='=';
	goto lex_ident;
      case '@':
	if (lx->flags&LF_KEEP_AT_SIGN) {
	  lx->token=ch;
	  goto lex_end;
	}
      case 'A'...'Z':
      case 'a'...'z':
      case '_':
      case 128...255:
lex_ident:
	i=0;
	buf[i++]=ch;
	while (TRUE) {
	  if (i>=LEX_MAX_IDENT-1)
	    break;
	  else if (!(ch=LexGetChar(lx)))
	    break;
	  else if (Bt(lx->alpha_numeric_bitmap,ch))
	    buf[i++]=ch;
	  else if (ch==TK_SUPERSCRIPT)
	    buf[i++]='>';
	  else if (ch==TK_SUBSCRIPT)
	    buf[i++]='<';
	  else if (ch==TK_NORMALSCRIPT)
	    buf[i++]='=';
	  else {
	    lx->flags|=LF_USE_LAST_U16;
	    break;
	  }
	}
	buf[i++]=0;
	temph=NULL;
	if (lx->local_var_list) {
	  lx->local_var_entry=MemberFind(buf,
	      lx->local_var_list);
	} else
	  lx->local_var_entry=NULL;
	if (!lx->local_var_entry && lx->hash_table_list)
	  temph=HashFind(buf,
	      lx->hash_table_list,lx->hash_mask);
	if (temph)
	  j=temph->type;
	else
	  j=0;
	if (j & HTT_DEFINE_STR && !(lx->flags & LF_NO_DEFINES)) {
	  LexIncludeStr(lx,
	    temph->str,StrNew(temph(DefineStrHashEntry *)->data),FALSE);
	  lx->cur_lfn->flags|=LFSF_IS_DEFINE;
	} else {
	  lx->hash_entry=temph;
	  Free(lx->ident);
	  lx->ident=StrNew(buf);
	  lx->ident_len=i;
	  lx->token=TK_IDENT;
	  goto lex_end;
	}
	break;
      case '0'...'9':
	i=ch-'0';
	ch=ToUpper(LexGetChar(lx));
	if (ch=='X') {
	  while (TRUE) {
	    ch=ToUpper(LexGetChar(lx));
	    if (Bt(hex_numeric_bitmap,ch)) {
	      if (ch<='9')
		i=i<<4+ch-'0';
	      else
		i=i<<4+ch-'A'+10;
	    } else {
	      lx->cur_i=i;
	      lx->flags|=LF_USE_LAST_U16;
	      lx->token=TK_INTEGER;
	      goto lex_end;
	    }
	  }
	} else if (ch=='B') {
	  while (TRUE) {
	    ch=LexGetChar(lx);
	    if (ch=='0')
	      i=i<<1;
	    else if (ch=='1')
	      i=i<<1+1;
	    else {
	      lx->cur_i=i;
	      lx->flags|=LF_USE_LAST_U16;
	      lx->token=TK_INTEGER;
	      goto lex_end;
	    }
	  }
	} else if (ch=='O') {
	  while (TRUE) {
	    ch=LexGetChar(lx);
	    if ('0'<=ch<='7')
	      i=i<<3+ch-'0';
	    else {
	      lx->cur_i=i;
	      lx->flags|=LF_USE_LAST_U16;
	      lx->token=TK_INTEGER;
	      goto lex_end;
	    }
	  }
	}
	while (TRUE) {
	  if (Bt(dec_numeric_bitmap,ch))
	    i=i*10+ch-'0';
	  else {
	    if (ch=='.' || ch=='e' || ch=='E') break;
lex_is_int:
	    lx->cur_i=i;
	    lx->flags|=LF_USE_LAST_U16;
	    lx->token=TK_INTEGER;
	    goto lex_end;
	  }
	  ch=LexGetChar(lx);
	}
	if (ch=='.') {
	  ch=LexGetChar(lx);
	  if (ch=='.') {
	    lx->flags|=LF_LAST_WAS_DOT;
	    goto lex_is_int;
	  }
	}
lex_float_start:
	k=0;
	while (TRUE) {
	  if (Bt(dec_numeric_bitmap,ch)) {
	    i=i*10+ch-'0';
	    k++;
	  } else {
	    if (ch=='e' || ch=='E')
	      break;
	    lx->cur_f=i*Pow10I64(-k);
	    lx->flags|=LF_USE_LAST_U16;
	    lx->token=TK_F64;
	    goto lex_end;
	  }
	  ch=LexGetChar(lx);
	}
	ch=LexGetChar(lx);
	neg_e=FALSE;
	if (ch=='-') {
	  neg_e=TRUE;
	  ch=LexGetChar(lx);
	}
	j=0;
	while (TRUE) {
	  if (Bt(dec_numeric_bitmap,ch))
	    j=j*10+ch-'0';
	  else {
	    if (neg_e)
	      lx->cur_f=i*Pow10I64(-j-k);
	    else
	      lx->cur_f=i*Pow10I64(j-k);
	    lx->flags|=LF_USE_LAST_U16;
	    lx->token=TK_F64;
	    goto lex_end;
	  }
	  ch=LexGetChar(lx);
	}
	break;
      case '"':
	lx->flags|=LF_IN_QUOTES;
	buf2=NULL;
	i=0;
	do {
	  j=LexInStr(lx,buf,LEX_MAX_IDENT,&str_done);
	  buf3=MAlloc(i+j);
	  if (buf2) {
	    MemCpy(buf3,buf2,i);
	    Free(buf2);
	    buf2=buf3;
	    MemCpy(buf2+i,buf,j);
	  } else {
	    buf2=buf3;
	    MemCpy(buf2,buf,j);
	  }
	  i+=j;
	} while (!str_done);
	Free(lx->ident);
	lx->ident=MAlloc(i);
	MemCpy(lx->ident,buf2,i);
	Free(buf2);
	lx->ident_len=i;
	lx->flags&=~LF_IN_QUOTES;
	lx->token=TK_STR;
	goto lex_end;
      case '\'':
	if (!(ch=LexGetChar(lx)))
	  k='\'';
	else {
	  k=0;
	  if (ch!='\'')
	    for (j=0;j<8;j++) {
	      if (ch=='\\') {
		switch (ch=LexGetChar(lx)) {
		  case 'n':
		    k.u8[j]=CH_NEW_LINE;
		    break;
		  case 'r':
		    k.u8[j]=CH_CARRIAGE_RETURN;
		    break;
		  case '"':
		    k.u8[j]='"';
		    break;
		  case '\'':
		    k.u8[j]='\'';
		    break;
		  case '\`':
		    k.u8[j]='\`';
		    break;
		  case '\\':
		    k.u8[j]='\\';
		    break;
		  case 't':
		    k.u8[j]=CH_TAB;
		    break;
		  case 'x':
		  case 'X':
		    i=0;
		    while (TRUE) {
		      ch=ToUpper(LexGetChar(lx));
		      if (Bt(hex_numeric_bitmap,ch)) {
			if (ch<='9')
			  i=i<<4+ch-'0';
			else
			  i=i<<4+ch-'A'+10;
		      } else
			break;
		    }
		    k.u8[j]=i;
		    lx->flags|=LF_USE_LAST_U16;
		    break;
		  case 'b':
		  case 'B':
		    i=0;
		    while (TRUE) {
		      ch=LexGetChar(lx);
		      if ('0'<=ch<='1')
			i=i<<1+ch-'0';
		      else
			break;
		    }
		    k.u8[j]=i;
		    lx->flags|=LF_USE_LAST_U16;
		    break;
		  case 'o':
		  case 'O':
		    i=0;
		    goto lex_octal;
		  case '0'...'7':
		    i=ch-'0';
lex_octal:
		    while (TRUE) {
		      ch=LexGetChar(lx);
		      if ('0'<=ch<='7')
			i=i<<3+ch-'0';
		      else
			break;
		    }
		    k.u8[j]=i;
		    lx->flags|=LF_USE_LAST_U16;
		    break;
		  default:
		    lx->flags|=LF_USE_LAST_U16;
		    k.u8[j]='\\';
		    break;
		}
	      } else if (ch=='$$') {
		ch=LexGetChar(lx);
		k.u8[j]='$$';
		if (ch!='$$')
		  lx->flags|=LF_USE_LAST_U16;
	      } else
		k.u8[j]=ch;
	      if (!(ch=LexGetChar(lx)) || ch=='\'')
		break;
	    }
	}
	lx->cur_i=k;
	lx->token=TK_U8;
	goto lex_end;
      case '#':
	if (lx->flags&LF_KEEP_NUM_SIGN) {
	  lx->token=ch;
	  goto lex_end;
	}
	if (Lex(lx)!=TK_IDENT)	//skip '#'
	  goto lex_end;
	if (!(temph=lx->hash_entry))
	  goto lex_end;
	if (!(temph->type & HTT_KEYWORD))
	  goto lex_end;
	switch (i=temph(GenericHashEntry *)->user_data0) {
	  case KW_INCLUDE:
	    if (Lex(lx)!=TK_STR)
	      goto lex_end;
	    if (Fs==adam_task)
	      fbuf=DftExt(lx->ident,"APZ");
	    else
	      fbuf=DftExt(lx->ident,"CPZ");
	    buf2=FileNameAbs(fbuf);
	    Free(fbuf);
	    if (Bt(&sys_run_level,RLf_LTF))
	      LexIncludeLtf(lx,buf2,NULL);
	    else
	      LexIncludeStr(lx,buf2,TextFileRead(buf2),TRUE);
	    Free(buf2);
	    break;
	  case KW_DEFINE:
	    lx->flags|=LF_NO_DEFINES;
	    if (Lex(lx)==TK_IDENT) {
	      temph=CAlloc(sizeof(DefineStrHashEntry));
	      temph->str=lx->ident;
	      lx->ident=0;
	      temph->type=HTT_DEFINE_STR;
	      SetSrcFileInfo(lx,temph);

	      do ch=LexGetChar(lx); //skip space between define name and start
	      while (Bt(non_eol_white_space_bitmap,ch));

	      i=j=0;
	      buf2=NULL;
	      if (ch) {
		do {
		  if (ch=='\\') {
		    if (ch=LexGetChar(lx)) {
		      if (ch!=CH_CARRIAGE_RETURN && ch!=CH_NEW_LINE) {
			buf[j++]='\\';
			buf[j++]=ch;
		      } else if (ch==CH_CARRIAGE_RETURN && LexGetChar(lx)!=CH_NEW_LINE)
			lx->flags|=LF_USE_LAST_U16;
		    } else {
		      buf[j++]='\\';
		      break;
		    }
		  } else if (ch!=CH_NEW_LINE)
		    buf[j++]=ch;
		  else
		    break;
		  while (ch=LexGetChar(lx)) {
		    if (ch=='/') {
		      ch=LexGetChar(lx);
		      if (ch=='/') {
			do ch=LexGetChar(lx);
			while (Bt(non_eol_chars_bitmap,ch));
			break;
		      } else {
			buf[j++]='/';
			lx->flags|=LF_USE_LAST_U16;
		      }
		    } else if (ch=='\\')
		      break;
		    else if (Bt(non_eol_chars_bitmap,ch))
		      buf[j++]=ch;
		    else
		      break;
		    if (j>=LEX_MAX_IDENT-4) { //spare spot for '\\' next loop
		      buf[j++]=0;
		      buf3=MAlloc(i+j);
		      if (buf2) {
			MemCpy(buf3,buf2,i);
			Free(buf2);
			buf2=buf3;
			MemCpy(buf2+i,buf,j);
		      } else {
			buf2=buf3;
			MemCpy(buf2,buf,j);
		      }
		      i+=j-1;
		      j=0;
		    }
		  }
		} while (ch=='\\');
	      }
	      buf[j++]=0;
	      buf3=MAlloc(i+j);
	      if (buf2) {
		MemCpy(buf3,buf2,i);
		Free(buf2);
		buf2=buf3;
		MemCpy(buf2+i,buf,j);
	      } else {
		buf2=buf3;
		MemCpy(buf2,buf,j);
	      }
	      temph(DefineStrHashEntry *)->data=buf2;
	      temph(DefineStrHashEntry *)->cnt=-1;
	      if (IsReplaceSyms)
		HashReplace(temph,lx->define_hash_table);
	      else
		HashAdd(temph,lx->define_hash_table);
	    }
	    lx->flags&=~LF_NO_DEFINES;
	    break;
	  case KW_ELSE:
	    if (lx->flags & LF_IN_IF) {
	      lx->token=TK_ELSE;
	      goto lex_end;
	    }
lex_else:
	    j=1;
	    do {
	      if (ch=LexGetChar(lx)) {
		if (ch=='#') {
		  if (!Lex(lx))
		    goto lex_end;
		  i=PrsKeyWord(lx);
		  if (i==KW_IF || i==KW_IFDEF || i==KW_IFNDEF || i==KW_IFSTATIC || i==KW_IFNSTATIC)
		    j++;
		  else if (i==KW_ENDIF)
		    j--;
		}
	      } else {
		lx->token=TK_EOF;
		goto lex_end;
	      }
	    } while (j);
	    break;
	  case KW_IF:
	    if (lx->flags & LF_IN_IF) {
	      lx->token=TK_IF;
	      goto lex_end;
	    }
lex_if:
	    lx->flags|=LF_IN_IF;
	    if (!Lex(lx)) {
	      lx->flags&=~LF_IN_IF;
	      goto lex_end;
	    }
	    if (Expression(lx)) {
	      lx->flags&=~LF_IN_IF;
	      switch (lx->token) {
		case TK_IF:	goto lex_if;
		case TK_IFDEF:	goto lex_ifdef;
		case TK_IFNDEF:	goto lex_ifndef;
		case TK_IFSTATIC: goto lex_ifstatic;
		case TK_IFNSTATIC: goto lex_ifnstatic;
		case TK_ELSE:	goto lex_else;
		case TK_ENDIF:	goto lex_cont;
		default:		goto lex_end;
	      }
	    } else {
	      lx->flags&=~LF_IN_IF;
	      if (lx->token!=TK_ENDIF && lx->token!=TK_ELSE) {
		if (lx->token==TK_IF || lx->token==TK_IFDEF || lx->token==TK_IFNDEF|| lx->token==TK_IFSTATIC || lx->token==TK_IFNSTATIC)
		  j=2;
		else
		  j=1;
		do {
		  if (ch=LexGetChar(lx)) {
		    if (ch=='#') {
		      if (!Lex(lx))
			goto lex_end;
		      i=PrsKeyWord(lx);
		      if (i==KW_IF || i==KW_IFDEF || i==KW_IFNDEF || i==KW_IFSTATIC || i==KW_IFNSTATIC)
			j++;
		      else if (i==KW_ENDIF)
			j--;
		      else if (i==KW_ELSE && j==1)
			break;
		    }
		  } else {
		    lx->token=TK_EOF;
		    goto lex_end;
		  }
		} while (j);
	      }
	    }
	    break;
	  case KW_IFDEF:
	    if (lx->flags & LF_IN_IF) {
	      lx->token=TK_IFDEF;
	      goto lex_end;
	    }
lex_ifdef:
	    lx->flags|=LF_NO_DEFINES;
	    if (!Lex(lx)) {
	      lx->flags&=~LF_NO_DEFINES;
	      goto lex_end;
	    }
	    lx->flags&=~LF_NO_DEFINES;
	    if (lx->token!=TK_IDENT)
	      goto lex_end;
	    if (lx->hash_entry)
	      goto lex_cont;
	    j=1;
	    do {
	      if (ch=LexGetChar(lx)) {
		if (ch=='#') {
		  if (!Lex(lx))
		    goto lex_end;
		  i=PrsKeyWord(lx);
		  if (i==KW_IF || i==KW_IFDEF || i==KW_IFNDEF || i==KW_IFSTATIC || i==KW_IFNSTATIC)
		    j++;
		  else if (i==KW_ENDIF)
		    j--;
		  else if (i==KW_ELSE && j==1)
		    break;
		}
	      } else {
		lx->token=TK_EOF;
		goto lex_end;
	      }
	    } while (j);
	    break;
	  case KW_IFNDEF:
	    if (lx->flags & LF_IN_IF) {
	      lx->token=TK_IFNDEF;
	      goto lex_end;
	    }
lex_ifndef:
	    lx->flags|=LF_NO_DEFINES;
	    if (!Lex(lx)) {
	      lx->flags&=~LF_NO_DEFINES;
	      goto lex_end;
	    }
	    lx->flags&=~LF_NO_DEFINES;
	    if (lx->token!=TK_IDENT)
	      goto lex_end;
	    if (!lx->hash_entry)
	      goto lex_cont;
	    j=1;
	    do {
	      if (ch=LexGetChar(lx)) {
		if (ch=='#') {
		  if (!Lex(lx))
		    goto lex_end;
		 i=PrsKeyWord(lx);
		  if (i==KW_IF || i==KW_IFDEF || i==KW_IFNDEF || i==KW_IFSTATIC || i==KW_IFNSTATIC)
		    j++;
		  else if (i==KW_ENDIF)
		    j--;
		  else if (i==KW_ELSE && j==1)
		    break;
		}
	      } else {
		lx->token=TK_EOF;
		goto lex_end;
	      }
	    } while (j);
	    break;
	  case KW_IFSTATIC:
	    if (lx->flags & LF_IN_IF) {
	      lx->token=TK_IFSTATIC;
	      goto lex_end;
	    }
lex_ifstatic:
	    if (lx->flags & LF_STATIC_COMPILE)
	      goto lex_cont;
	    j=1;
	    do {
	      if (ch=LexGetChar(lx)) {
		if (ch=='#') {
		  if (!Lex(lx))
		    goto lex_end;
		  i=PrsKeyWord(lx);
		  if (i==KW_IF || i==KW_IFDEF || i==KW_IFNDEF || i==KW_IFSTATIC || i==KW_IFNSTATIC)
		    j++;
		  else if (i==KW_ENDIF)
		    j--;
		  else if (i==KW_ELSE && j==1)
		    break;
		}
	      } else {
		lx->token=TK_EOF;
		goto lex_end;
	      }
	    } while (j);
	    break;
	  case KW_IFNSTATIC:
	    if (lx->flags & LF_IN_IF) {
	      lx->token=TK_IFSTATIC;
	      goto lex_end;
	    }
lex_ifnstatic:
	    if (!(lx->flags & LF_STATIC_COMPILE))
	      goto lex_cont;
	    j=1;
	    do {
	      if (ch=LexGetChar(lx)) {
		if (ch=='#') {
		  if (!Lex(lx))
		    goto lex_end;
		  i=PrsKeyWord(lx);
		  if (i==KW_IF || i==KW_IFDEF || i==KW_IFNDEF || i==KW_IFSTATIC || i==KW_IFNSTATIC)
		    j++;
		  else if (i==KW_ENDIF)
		    j--;
		  else if (i==KW_ELSE && j==1)
		    break;
		}
	      } else {
		lx->token=TK_EOF;
		goto lex_end;
	      }
	    } while (j);
	    break;
	  case KW_ENDIF:
	    if (lx->flags & LF_IN_IF) {
	      lx->token=TK_ENDIF;
	      goto lex_end;
	    }
	    break;
	  case KW_ASSERT:
	    if (!Lex(lx))
	      goto lex_end;
	    if (!Expression(lx))
	      PutLexWarn(lx,"ST_ASSERT_FAILED");
	    goto lex_end;
	  case KW_EXE:
	    if (!Lex(lx))
	      goto lex_end;
	    (*fp_prs_stream_blk)(lx);
	    goto lex_end;
	  case KW_HELP_INDEX:
	    if (Lex(lx)!=TK_STR)
	      goto lex_end;
	    Free(lx->cur_index);
	    lx->cur_index=lx->ident;
	    lx->ident=NULL;
	    break;
	  case KW_HELP_FILE:
	    if (Lex(lx)!=TK_STR)
	      goto lex_end;
	    temph=CAlloc(sizeof(SrcSymHashEntry));
	    fbuf=DftExt(lx->ident,"TXZ");
	    temph->str=FileNameAbs(fbuf);
	    Free(fbuf);
	    temph->type=HTT_HELP_FILE|HTF_PUBLIC;
	    SetSrcFileInfo(lx,temph);
	    if (IsReplaceSyms)
	      HashReplace(temph,lx->glbl_hash_table);
	    else
	      HashAdd(temph,lx->glbl_hash_table);
	    break;
	}
	break;
      case CH_NEW_LINE:
	if (!(lx->flags&LF_KEEP_NEW_LINES))
	  break; //else fall through
      case TK_INSERT_BINARY:
      case TK_INSERT_BINARY_TYPE:
      case TK_INSERT_BINARY_SIZE:
	lx->token=ch;
	goto lex_end;
      case '.':
	if (lx->flags&LF_KEEP_DOT) {
	  lx->token=ch;
	  goto lex_end;
	}
	if (lx->flags&LF_LAST_WAS_DOT) {
	  lx->flags&=~LF_LAST_WAS_DOT;
	  goto lex_dot_dot;
	}
	ch=LexGetChar(lx);
	if ('0'<=ch<='9') {
	  i=0;
	  goto lex_float_start;
	} else if (ch=='.') {
lex_dot_dot:
	  lx->token=TK_DOT_DOT;
	  if (LexGetChar(lx)=='.')
	    lx->token=TK_DOT_DOT_DOT;
	  else
	    lx->flags|=LF_USE_LAST_U16;
	  goto lex_end;
	}
	lx->flags|=LF_USE_LAST_U16;
	lx->token='.';
	goto lex_end;
      case '!':
      case '$$'...'&':
      case '('...'-':
      case '/':
      case ':'...'?':
      case '[':
      case ']'...'^':
      case '{'...'~':
      case '`':
	if (!(i=dual_U16_tokens1[ch])) {
	  if (ch=='$$') {
	    ch=LexGetChar(lx);
	    if (ch=='$$') {
	      lx->token='$$';
	      goto lex_end;
	    } else if (ch) {
	      do ch=LexGetChar(lx);
	      while (ch && ch!='$$');
	      if (!ch) {
		lx->token=TK_EOF;
		goto lex_end;
	      } else
		goto lex_cont;
	    } else {
	      lx->flags|=LF_USE_LAST_U16;
	      lx->token='$$';
	      goto lex_end;
	    }
	  } else {
	    lx->token=ch;
	    goto lex_end;
	  }
	} else {
	  j=LexGetChar(lx);
	  if (i.u16[0]==j) {
	    i>>=16;
	    if (!i) { // "/*"
	      j=1;
	      do {
		if (!(ch=LexGetChar(lx)))
		  return lx->token=TK_EOF;
lex_check_comment:
		if (ch=='*') {
		  if (!(ch=LexGetChar(lx)))
		    return lx->token=TK_EOF;
		  if (ch=='/')
		    j--;
		  else
		    goto lex_check_comment;
		} else if (ch=='/') {
		  if (!(ch=LexGetChar(lx)))
		    return lx->token=TK_EOF;
		  if (ch=='*')
		    j++;
		  else
		    goto lex_check_comment;
		}
	      } while (j);
	      goto lex_cont;
	    } else {
	      lx->token=i;
	      goto lex_end;
	    }
	  }
	  if (i=dual_U16_tokens2[ch]) {
	    if (i.u16[0]==j) {
	      i>>=16;
	      if (!i) {  // "//"
		LexSkipEol(lx);
		if (lx->flags&LF_KEEP_NEW_LINES) {
		  lx->token=CH_NEW_LINE;
		  goto lex_end;
		} else
		  goto lex_cont;
	      } else {
		if (i==TK_SHL || i==TK_SHR) {
		  j=LexGetChar(lx);
		  if (j=='=') {
		    if (i==TK_SHL)
		      i=TK_SHL_EQUAL;
		    else
		      i=TK_SHR_EQUAL;
		  } else
		    lx->flags|=LF_USE_LAST_U16;
		}
		lx->token=i;
		goto lex_end;
	      }
	    }
	    if (i=dual_U16_tokens3[ch]) {
	      if (i.u16[0]==j) {
		lx->token=i.u16[1];
		goto lex_end;
	      }
	    }
	  }
	  lx->flags|=LF_USE_LAST_U16;
	  lx->token=ch;
	  goto lex_end;
      }
      case TK_NUM_TK:
      default:
	break;
    }
  }
lex_end:
  if (Bt(&lx->opts,OPTf_LEX_TRACE))
    LexPutToken(lx);
  return lx->token;
}

U8 *LexExtStr(LexStruct *lx,U64 *size=NULL)
{ //size includes terminator
  U64 len=lx->ident_len,len1,len2;
  U8 *st=lx->ident,*st1,*st2;
  lx->ident=NULL;
  while (lx->token==TK_STR) {
    st1=st;
    len1=len;
    if (Lex(lx)==TK_STR) {
      len2=lx->ident_len;
      st2=lx->ident;
      lx->ident=NULL;
      len=len1+len2-1;
      st=MAlloc(len);
      if (len1>1)
	MemCpy(st,st1,len1-1);
      MemCpy(st+len1-1,st2,len2);
      Free(st1);
      Free(st2);
    }
  }
  if (size)
    *size=len;
  return st;
}

U8 *LexPutLine(LexStruct *lx,U8 *start)
{
  U8 *ptr,ch;
  if (!start) return NULL;
  if (lx->cur_lfn->flags&LFSF_IS_LTF)
    return LtfPutLine(start);
  else {
    ptr=start;
    while (ch=*ptr++) {
      if (ch==CH_NEW_LINE) {
	if (*ptr==CH_CARRIAGE_RETURN)
	  ptr++;
	break;
      } else if (ch==CH_CARRIAGE_RETURN) {
	if (*ptr==CH_NEW_LINE)
	  ptr++;
	break;
      }
      '' ch;
    }
    if (!ch) ptr--;
    NewLine;
    return ptr;
  }
  return NULL;
}

I64 LexLineNum(LexStruct *lx)
{
  return lx->cur_lfn->line_num;
}

U0 LexPutLink(LexStruct *lx)
{
  if (IsRaw)
    "%s,%d ",lx->cur_lfn->name,lx->cur_lfn->line_num;
  else {
    PutFileLink(lx->cur_lfn->name,lx->cur_lfn->line_num);
    AdamErr("%s,%d\n",lx->cur_lfn->name,lx->cur_lfn->line_num);
    '' CH_SPACE;
  }
}

U0 PutLexErr(LexStruct *lx,U8 *name,U8 *msg=NULL)
{
  if (Bt(&lx->flags,Lf_RAW_ERRORS)) {
    Preempt(OFF);
    Raw(ON);
  }
  PutErr;
  lx->error_cnt++;
  if (!IsRaw)
    AdamErr("Task:%08X ",Fs);
  PutDefine(name);
  if (!IsRaw)
    AdamErr("%S",name);
  if (msg) {
    "" msg;
    if (!IsRaw)
      AdamErr("%s\n",msg);
  }
  LexPutToken(lx);
  LexPutLink(lx);
  LexPutLine(lx,lx->cur_lfn->line_start);
}

U0 PutLexWarn(LexStruct *lx,U8 *name,U8 *msg=NULL)
{
  PrintFWarn("%S%s",name,msg);
  lx->warning_cnt++;
  LexPutToken(lx);
  LexPutLink(lx);
  LexPutLine(lx,lx->cur_lfn->line_start);
}

U0 PutLexFunWarn(LexStruct *lx,FunStruct *fun_class,U8 *name,U8 *msg=NULL)
{
  PrintFWarn("%S%s",name,msg);
  if (fun_class) {
    "%S'%s'.\n","ST_IN_FUN",fun_class->str;
    if (IsRaw)
      "%s",fun_class->src_link;
    else {
      "$$LK-A,\"%s\"$$",fun_class->src_link;
      AdamErr("%s\n",fun_class->src_link);
    }
    NewLine;
  } else {
    LexPutToken(lx);
    LexPutLink(lx);
    LexPutLine(lx,lx->cur_lfn->line_start);
  }
  lx->warning_cnt++;
}

U0 PutAsmWarn(LexStruct *lx,U8 *name)
{
  PutLexWarn(lx,name);
  LexSkipEol(lx);
  Lex(lx);
}

U0 PrsExcept(LexStruct *lx,U8 *name=NULL,U8 *msg=NULL)
{
  PutLexErr(lx,name,msg);
  FlushMsgs;
  throw(EXCEPT_COMPILER,0);
}

