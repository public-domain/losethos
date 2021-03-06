#help_index "Utils;Cmd Line (Typically)"

#define GRF_REPLACE	0
#define GRF_SKIP	1
#define GRF_ALL		2
#define GRF_ED		3
#define GRF_ABORT_FILE	4
#define GRF_SKIP_FILE	5

I64 PopUpReplaceSkipAllCancelEd(U8 *msg)
{
  I64 i;
  Ltf *l=LtfNew;
  LtfPutS(l,msg);
  LtfPutS(l,"$$CM+LX,2,4$$$$BT,\"REPLACE\",GRF_REPLACE$$"
	"$$CM+LX,22,0$$$$BT,\"SKIP\",GRF_SKIP$$"
	"$$CM+LX,2,4$$$$BT,\"ALL\",GRF_ALL$$"
	"$$CM+LX,22,0$$$$BT,\"ABORT ALL\",LTFM_CANCEL$$"
	"$$CM+LX,2,4$$$$BT,\"EDIT\",GRF_ED$$"
	"$$CM+LX,22,0$$$$BT,\"ABORT FILE\",GRF_ABORT_FILE$$"
	"$$CM+LX,2,4$$$$BT,\"SKIP FILE\",GRF_SKIP_FILE$$\n");
  i=PopUpMenu(l);
  LtfDel(l);
  return i;
}



U64 GrepFile(U8 *pattern,U8 *name,U64 *_fuf_flags,U8 *replace_text)
{
  BoolI8 first_on_line,write_this_file=FALSE,cont=!Bt(_fuf_flags,FUf_CANCEL);
  U8 *src,*dst,*dst2,*name_buf=NULL;
  U64 ss_flags;
  Ltf *cur_l,*l=LtfRead(name,LTFF_PLAIN_TEXT_WITH_TABS|LTFF_NO_CURSOR);
  LtfEntry *cl;
  I64 i,j,plen,rlen,dlen;
  U64 cnt=0,old_flags;

  ss_flags=Bt(_fuf_flags,FUf_IGNORE) ? SRF_IGNORE_CASE :0;
  if (Bt(_fuf_flags,FUf_WHOLE_LABELS))
    ss_flags|=SRF_WHOLE_LABELS;

  plen=StrLen(pattern);
  if (replace_text)
    rlen=StrLen(replace_text);
  cl=l->root.next;
  while (cl!=l && cont) {
    if (cl->type_u8==LTFT_TEXT) {
      src=cl->tag;
      first_on_line=TRUE;
      while (src && cont) {
	if (src=SearchStr(src,pattern,ss_flags)) {
	  cnt++;
	  if (first_on_line || Bt(_fuf_flags,FUf_REPLACE)) {
	    first_on_line=FALSE;
	    PutFileLink(name,cl->y+1,TRUE);
	    name_buf=MSPrintF("%s,%d",name,cl->y+1);
	    if (cur_l=LtfCur) {
	      old_flags=cur_l->flags&LTFF_PLAIN_TEXT;
	      cur_l->flags|=LTFF_PLAIN_TEXT;
	    }
	    " %s\n",cl->tag;
	    if (cur_l)
	      cur_l->flags= cur_l->flags&~LTFF_PLAIN_TEXT |old_flags;
	  }
	  if (Bt(_fuf_flags,FUf_REPLACE)) {
	    if (Bt(_fuf_flags,FUf_ALL))
	      i=GRF_ALL;
	    else {
	      i=PopUpReplaceSkipAllCancelEd("");
	      if (i<0) {
		LBts(_fuf_flags,FUf_CANCEL);
		cont=FALSE;
		write_this_file=FALSE;
	      } else if (i==GRF_ALL)
		LBts(_fuf_flags,FUf_ALL);
	      else if (i==GRF_ABORT_FILE) {
		cont=FALSE;
		write_this_file=FALSE;
	      } else if (i==GRF_SKIP_FILE)
		cont=FALSE;
	    }
	    if (i==GRF_REPLACE || i==GRF_ALL) {
	      dlen=StrLen(cl->tag);
	      dst=MAlloc(dlen+1+rlen-plen);
	      dst2=dst;
	      j=src-cl->tag;
	      for (i=0;i<j;i++)
		*dst++=cl->tag[i];
	      for (i=0;i<rlen;i++)
		*dst++=replace_text[i];
	      src=dst;
	      for (i=j+plen;i<=dlen;i++)
		*dst++=cl->tag[i];
	      Free(cl->tag);
	      cl->tag=dst2;
	      if (cur_l=LtfCur) {
		old_flags=cur_l->flags&LTFF_PLAIN_TEXT;
		cur_l->flags|=LTFF_PLAIN_TEXT;
	      }
	      "%12s,%04d*%s\n",name,cl->y+1,dst2;
	      if (cur_l)
		cur_l->flags= cur_l->flags&~LTFF_PLAIN_TEXT |old_flags;
	      write_this_file=TRUE;
	    } else {
	      src++;
	      if (i==GRF_ED) {
		Free(name_buf);
		name_buf=StrNew(l->filename.name);
		l->flags&=~LTFF_NO_CURSOR;
		l->cur_entry=cl;
		l->cur_data_col=cl->min_col;
		LtfWrite(l);
		LtfDel(l);
		"Wrote:%s\n",name_buf;
		Ed(name_buf);
		l=LtfRead(name_buf, LTFF_PLAIN_TEXT_WITH_TABS);
		cl=l->cur_entry;
		if (cl->last!=l) cl=cl->last;
		src=NULL;
		write_this_file=FALSE;
	      }
	    }
	  } else
	    src++;
	  Free(name_buf);
	  name_buf=NULL;
	}
      }
    }
    cl=cl->next;
  }
  if (write_this_file) {
    LtfWrite(l);
    "Wrote:%s\n",l->filename.name;
  }
  LtfDel(l);
  return cnt;
}

U64 GrepList(U8 *pattern,LTDirEntry *tempm1,U64 *_fuf_flags,U8 *replace_text)
{
  U64 cnt=0;
  LTDirEntry *tempm2;

  while (tempm1 && !Bt(_fuf_flags,FUf_CANCEL)) {
    tempm2=tempm1->next;
    if (tempm1->attr & LT_ATTR_DIR) {
      if (tempm1->sub) {
	"%S %s\n","ST_SCANNING_DIR",tempm1->full_name;
	cnt+=GrepList(pattern,tempm1->sub,_fuf_flags,replace_text);
      }
    } else
      cnt+=GrepFile(pattern,tempm1->full_name,_fuf_flags,replace_text);
    LTDirEntryDel(tempm1);
    tempm1=tempm2;
  }
  return cnt;
}

public U64 Grep(U8 *pattern,U8 *files_find_mask=TEXT_FILE_MASK,U8 *fu_flags=NULL,U8 *replace_text=NULL)
/*
This does not do regular expressions--my bad.
Anyway, it's good for searching and replacing.
Let's say it stands for global replace ;-)

"+r"=recurse
"+i"=ignore case
"+l"=whole labels only.
     This will check for a nonlabel character before
     and after.  If you have a var, "ll" and don't
     want to match words like "will" and "wallow", you
     set this flag and it will see that the characters
     before or after "ll" are label characters.
*/

{
  U64 cnt=0,fuf_flags=0;
  GetFFlags(&fuf_flags,Define("ST_FILE_UTIL_FLAGS"),"+r+i"); //Set dft flags
  GetFFlags(&fuf_flags,Define("ST_FILE_UTIL_FLAGS"),fu_flags);
  LBEqu(&fuf_flags,FUf_REPLACE,replace_text);
  cnt+=GrepList(pattern,FilesFind(files_find_mask,fuf_flags),&fuf_flags,replace_text);
  return cnt;
}

public U64 FileOcc(U8 *pattern,U8 *files_find_mask=TEXT_FILE_MASK,U8 *fu_flags="+r+i+l")
{/*
"+r"=recurse
"+i"=ignore case
"+l"=whole labels only.
*/
  U64 cnt=0;
  BoolI8 old_silent=Silent(TRUE);
  cnt=Grep(pattern,files_find_mask,fu_flags);
  Silent(old_silent);
  return cnt;
}

class GrepStruct
{
  U8	find_text[136]	    fmtstr "$$DA-P,135,\"FIND:%s\"$$\n";
  U8	replace_text[136]   fmtstr "$$DA-P,135,\"REPLACE:%s\"$$\n";
  BoolI8 replace	    fmtstr "$$CB,\"REPLACE\"$$\n";
  BoolI8 match_case	    fmtstr "$$CB,\"MATCH CASE\"$$\n";
  BoolI8 whole_labels	    fmtstr "$$CB,\"WHOLE LABELS\"$$\n";
  U8	filemask[136]	    fmtstr "$$DA-P,135,\"FILE MASK:%s\"$$\n";
  BoolI8 recurse	    fmtstr "$$CB,\"RECURSE\"$$\n";
};

U64 GrepWiz()
{
  U64 result=0;
  Ltf *l;
  U8 buf[32],*dir,*st;
  GrepStruct *g=CAlloc(sizeof(GrepStruct));
  g->recurse=TRUE;
  StrCpy(g->filemask,TEXT_FILE_MASK);
  if (l=LtfCur) {
    StrCpy(g->find_text,l->find_replace->find_text);
    StrCpy(g->replace_text,l->find_replace->replace_text);
    g->replace=l->find_replace->replace;
    g->match_case=l->find_replace->match_case;
    g->whole_labels=l->find_replace->whole_labels;
  }
  if (LtfFormDo(g,"GrepStruct",NULL,FALSE,"$$FG,PURPLE$$$$TX+CX,\"Grep\"$$\n$$FG$$")) {
    if (l) {
      StrCpy(l->find_replace->find_text,g->find_text);
      StrCpy(l->find_replace->replace_text,g->replace_text);
      l->find_replace->replace=g->replace;
      l->find_replace->match_case=g->match_case;
      l->find_replace->whole_labels=g->whole_labels;
    }
    dir=PopUpPickDir;
    if (*dir) {
      *buf=0;
      if (g->match_case)
	StrCat(buf,"-i");
      if (!g->recurse)
	StrCat(buf,"-r");
      if (g->whole_labels)
	StrCat(buf,"+l");
      if (g->replace)
	st=MSPrintF("\"$$$$WW+H,1$$$$\";Cd(\"%s\");Grep(\"%Q\",\"%Q\",\"%Q\",\"%Q\");UserTaskCont;",dir,g->find_text,g->filemask,buf,g->replace_text);
      else
	st=MSPrintF("\"$$$$WW+H,1$$$$\";Cd(\"%s\");Grep(\"%Q\",\"%Q\",\"%Q\");UserTaskCont;",dir,g->find_text,g->filemask,buf);
      result=PopUp(st);
    }
    Free(dir);
  }
  Free(g);
  return result;
}

#help_index ""
