I64 HasLower(U8 *src)
{
  U8 ch;
  while (ch=*src++)
    if ('a'<=ch<='z')
     return TRUE;
  return FALSE;
}

SysHashEntry *PSectFind(U0 *add,I64 *offset)
{ //See $LK,"::/LT/OSMain/Hash1a.CPZ","FL:::/LT/OSMain/Hash1a.CPZ,1"$
  ExportSysSymHashEntry *result=NULL,*tempess;
  SysHashTable *h=Fs->hash_table;
  BoolI8 has_lower=FALSE;
  U64 best=0x8000,j;

  if (!ChkPtr(add)) {
    *offset=best;
    return NULL;
  }
  while (h) {
    tempess=h->next_added;
    while (tempess!=h) {
      if (tempess->type&HTT_FUN)
	j=tempess(FunStruct *)->executable_addr;
      else if (tempess->type&HTT_EXPORT_SYS_SYM)
	j=tempess->val;
      else
	j=0;
      j=add(I64)-j;
      if (tempess->type&HTT_EXPORT_SYS_SYM) {
	if (j<best || j==best && !has_lower) {
	  has_lower=HasLower(tempess->str);
	  best=j;
	  result=tempess;
	}
      } else if (tempess->type&HTT_FUN) {
	if (j<best || j==best &&
	    (result && result->type&HTT_EXPORT_SYS_SYM||!has_lower)) {
	  has_lower=HasLower(tempess->str);
	  best=j;
	  result=tempess;
	}
      }
      tempess=tempess->next_added;
    }
    h=h->next;
  }
  *offset=best;
  return result;
}

U0 PSectCacheAdd(SysHashEntry *temps,U0 *add)
{
  I64 i;
  DbgInfo *dbg;
  BoolI8 old_preempt;
  PSectCacheEntry *temppc;
  if (temps && temps->type&HTT_FUN &&
      (dbg=temps(FunStruct *)->debug)) {
    old_preempt=Preempt(OFF);
    lock {i=psect_cache_index++;}
    temppc=&psect_cache[i&(PSECT_CACHE_SIZE-1)];
    temppc->base=dbg->body[0];
    if (add<temppc->base)
      temppc->base=add;
    temppc->limit=dbg->body[dbg->max_line-dbg->min_line];
    if (add>=temppc->limit)
      temppc->limit=add(I64)+1;
    i=MinI64(StrLen(temps->str),PSECT_CACHE_STR_LEN-1);
    MemCpy(temppc->str,temps->str,i);
    temppc->str[i]=0;
    temppc->timestamp=GetTimeStamp;
    Preempt(old_preempt);
  }
}

U8 *PSectCacheFind(U8 *add,U64 *offset)
{
  I64 i,timeout;
  PSectCacheEntry *temppc=psect_cache;
  if (&sys_idle_start<=add<&sys_idle_end) {
    *offset=add-&sys_idle_start;
    return "SYS_IDLE_START";
  } else {
    timeout=GetTimeStamp-8*time_stamp_freq;
    for (i=0;i<PSECT_CACHE_SIZE;i++,temppc++)
      if (temppc->base<=add<temppc->limit &&
	  temppc->timestamp>timeout) {
	*offset=add-temppc->base;
	return temppc->str;
      }
    return NULL;
  }
}

U0 SPrintFPSect(U8 *buf,U64 add,U64 field_len)
{
  U64 offset;
  ExportSysSymHashEntry *tempess;
  U8 *str,*str2;
  BoolI8 is_fun=FALSE;
  if (add) {
    if (str=PSectCacheFind(add,&offset)) {
      if (!(&sys_idle_start<=add<&sys_idle_end))
	is_fun=TRUE;
    } else  {
      if (tempess=PSectFind(add,&offset)) {
	if (tempess->type&HTT_FUN)
	  is_fun=TRUE;
	PSectCacheAdd(tempess,add);
	str=tempess->str;
      }
    }
    if (str) {
      if (offset<=0xFFFF) {
	if (is_fun) {
	  str2=MSPrintF("&%s",str);
	  if (field_len>7)
	    SPrintF(buf,"%*ts+0x%04X",field_len-7,str2,offset);
	  else
	    SPrintF(buf,"%s+0x%04X",str2,offset);
	  Free(str2);
	} else {
	  if (field_len>7)
	    SPrintF(buf,"%*ts+0x%04X",field_len-7,str,offset);
	  else
	    SPrintF(buf,"%s+0x%04X",str,offset);
	}
      }
      return;
    }
  }
  if (field_len)
    SPrintF(buf,"%*tX",field_len,add);
  else
    SPrintF(buf,"%X",add);
}


I64 SrcLineNum(U0 *add,U64 cnt=1)
{
  SrcSymHashEntry *temph;
  I64 i,ii,j,offset,best=-1,d,best_d;
  DbgInfo *dbg;
  U8 **body;
  if (temph=PSectFind(add,&offset)) {
    if (temph->type&(HTT_FUN|HTT_EXPORT_SYS_SYM)) {
      if (dbg=temph->debug) {
	j=dbg->max_line-dbg->min_line+1;
	body=dbg->body;
	best_d=MAX_I64;
	for (i=0;i<j;i++) {
	  if (0<body[i]<=add<body[i]+cnt) {
	    ii=i+1;
	    while (!body[ii])
	      ii++;
	    if (add<body[ii]) {
	      d=add(I64)-body[i];
	      if (d<best_d) {
		best_d=d;
		best=i+dbg->min_line;
	      }
	    }
	  }
	}
      }
    }
  }
  return best;
}

U8 *SrcFileName(U0 *add,U64 cnt=1,TaskStruct *mem_task=NULL)
{
  SrcSymHashEntry *temph;
  I64 i,j,ii,offset,best=NULL,d,best_d;
  DbgInfo *dbg;
  U8 **body;
  U8 *src;
  if (temph=PSectFind(add,&offset)) {
    if (temph->type&(HTT_FUN|HTT_EXPORT_SYS_SYM)) {
      if (dbg=temph->debug) {
	j=dbg->max_line-dbg->min_line+1;
	body=dbg->body;
	best_d=MAX_I64;
	for (i=0;i<j;i++) {
	  if (0<body[i]<=add<body[i]+cnt) {
	    ii=i+1;
	    while (!body[ii])
	      ii++;
	    if (add<body[ii]) {
	      d=add(I64)-body[i];
	      if (d<best_d) {
		best_d=d;
		best=temph->src_link;
	      }
	    }
	  }
	}
	}
    }
  }
  if (best) {
    src=StrNew(best,mem_task);
    StrFirstRem(src,":");
    StrLastRem(src,",");
    return src;
  } else
    return NULL;
}

U8 *SrcEdLink(U0 *add,U64 cnt=1,TaskStruct *mem_task=NULL)
{
  U8 *filename,buf[512];
  I64 linenum=SrcLineNum(add,cnt);
  if (linenum>=0) {
    if (filename=SrcFileName(add,cnt)) {
      SPrintF(buf,"FL:%s,%d",filename,linenum);
      return StrNew(buf,mem_task);
    }
  }
  return NULL;
}


BoolI64 PutSrcLink(U0 *add,U64 cnt=1,U8 *buf=NULL)
{
  U8 *src;
  if (src=SrcEdLink(add,cnt)) {
    if (buf)
      SPrintF(buf,"$$LK,\"%P\",\"%s\"$$",add,src);
    else
      "$$LK,\"%P\",\"%s\"$$",add,src;
    Free(src);
    return TRUE;
  } else if (buf)
    *buf=0;
  return FALSE;
}

Ltf *EdAddSrc(U0 *add,U64 cnt=1,BoolI8 bail=FALSE)
{
  U8 *src;
  Ltf *l=NULL;
  if (src=SrcEdLink(add,cnt)) {
    l=EdJoin(src,bail);
    Free(src);
  }
  return l;
}
