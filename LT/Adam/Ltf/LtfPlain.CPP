U0 PrsLtfFlags(LexStruct *lx,U64 *flags,U32 *type)
{
  I64 i;
  BoolI8 turn_on;
  while (lx->token=='+' || lx->token=='-') {
    turn_on= lx->token=='+';
    if (Lex(lx)==TK_IDENT) {
      i=DefineMatch(lx->ident,"ST_LTF_FLAGS",LEF_EXACT);
      if (i<0)
	*type=LTFT_ERROR;
      else {
	BEqu(flags,i,turn_on);
	switch (i) {
	  case LTFLf_SELECTED:
	  case LTFLf_UNDERLINE:
	  case LTFLf_INVERT:
	  case LTFLf_BLINK:
	    BEqu(type,i,turn_on);
	    break;
	}
      }
      Lex(lx);	//skip flag
    }
  }
}

U64 ltf_dft_flags[LTFT_NUM_TYPES]={
//LTFT_TEXT
0,
	
//LTFT_NEW_LINE
0,

//LTFT_SOFT_NEW_LINE
0,

//LTFT_CURSOR
0,

//LTFT_TAB
0,

//LTFT_CLEAR
0,

//LTFT_PAGE_BREAK
0,

//LTFT_DATA
LTFLF_DATA_IS_PTR | LTFLF_HAS_TERMINATOR | LTFLF_AUX_STR | LTFLF_U64,

//LTFT_PAGE_LEN
0,

//LTFT_LEFT_MARGIN
0,

//LTFT_RIGHT_MARGIN
0,

//LTFT_HEADER
0,

//LTFT_FOOTER
0,

//LTFT_INDENT
0,

//LTFT_FOREGROUND
0,

//LTFT_BACKGROUND
0,

//LTFT_DFT_FOREGROUND
0,

//LTFT_DFT_BACKGROUND
0,

//LTFT_ALT_FOREGROUND
0,

//LTFT_LINK_FOREGROUND
0,

//LTFT_MACRO_FOREGROUND
0,

//LTFT_ANCHOR_FOREGROUND
0,

//LTFT_HIDDEN_FOREGROUND
0,

//LTFT_TREE_FOREGROUND
0,

//LTFT_USER_FOREGROUND
0,

//LTFT_BINARY_FOREGROUND
0,

//LTFT_KEYWORD_FOREGROUND
0,

//LTFT_KEYWORD2_FOREGROUND
0,

//LTFT_COMMENT_FOREGROUND
0,

//LTFT_QUOTE_FOREGROUND
0,

//LTFT_WORD_WRAP
0,

//LTFT_UNDERLINE
0,

//LTFT_INVERT
0,

//LTFT_BLINK
0,

//LTFT_SHIFTED_X
0,

//LTFT_SHIFTED_Y
0,

//LTFT_CURSOR_MOVEMENT
LTFLF_LEFT_EXP | LTFLF_RIGHT_EXP,

//LTFT_ANCHOR
LTFLF_AUX_STR,

//LTFT_LINK
LTFLF_LINK|LTFLF_AUX_STR,

//LTFT_BUTTON
LTFLF_ESC | LTFLF_LEFT_EXP | LTFLF_HAS_BORDER,

//LTFT_CHECK_BOX
LTFLF_CHECK_COLLAPSABLE|LTFLF_DATA_IS_PTR|LTFLF_U8,

//LTFT_MACRO
LTFLF_ESC | LTFLF_LEFT_MACRO,

//LTFT_MENU_VAL
LTFLF_ESC | LTFLF_LEFT_EXP,

//LTFT_HEX_ED
LTFLF_DATA_IS_PTR | LTFLF_ZERO_BASED,

//LTFT_HIDE_START
LTFLF_CHECKED_COLLAPSED|LTFLF_CHECK_COLLAPSABLE,

//LTFT_HIDE_END
0,

//LTFT_TREE
LTFLF_CHECKED_COLLAPSED|LTFLF_CHECK_COLLAPSABLE|LTFLF_TREE,

//LTFT_PICTURE
LTFLF_HAS_BIN,

//LTFT_INSERT_BINARY
LTFLF_HAS_BIN,

//LTFT_INSERT_BINARY_TYPE
LTFLF_HAS_BIN,

//LTFT_INSERT_BINARY_SIZE
LTFLF_HAS_BIN,

//LTFT_BPT
0,

//LTFT_SONG
LTFLF_AUX_STR,

//LTFT_HIGHLIGHT
0,

//LTFT_LOCK_TO_PAGE
0,

//LTFT_LOCK_TO_CENTER
0,

//LTFT_HTML_IMG
0,

//LTFT_HTML_VIDEO
0,

//LTFT_HTML_CODE
0,

//LTFT_ERROR
0};

U64 ltf_dft_type_flags[LTFT_NUM_TYPES]={
//LTFT_TEXT
0,

//LTFT_NEW_LINE
0,

//LTFT_SOFT_NEW_LINE
0,

//LTFT_CURSOR
0,

//LTFT_TAB
0,

//LTFT_CLEAR
0,

//LTFT_PAGE_BREAK
0,

//LTFT_DATA
0,

//LTFT_PAGE_LEN
0,

//LTFT_LEFT_MARGIN
0,

//LTFT_RIGHT_MARGIN
0,

//LTFT_HEADER
0,

//LTFT_FOOTER
0,

//LTFT_INDENT
0,

//LTFT_FOREGROUND
0,

//LTFT_BACKGROUND
0,

//LTFT_DFT_FOREGROUND
0,

//LTFT_DFT_BACKGROUND
0,

//LTFT_ALT_FOREGROUND
0,

//LTFT_LINK_FOREGROUND
0,

//LTFT_MACRO_FOREGROUND
0,

//LTFT_ANCHOR_FOREGROUND
0,

//LTFT_HIDDEN_FOREGROUND
0,

//LTFT_TREE_FOREGROUND
0,

//LTFT_USER_FOREGROUND
0,

//LTFT_BINARY_FOREGROUND
0,

//LTFT_KEYWORD_FOREGROUND
0,

//LTFT_KEYWORD2_FOREGROUND
0,

//LTFT_COMMENT_FOREGROUND
0,

//LTFT_QUOTE_FOREGROUND
0,

//LTFT_WORD_WRAP
0,

//LTFT_UNDERLINE
0,

//LTFT_INVERT
0,

//LTFT_BLINK
0,

//LTFT_SHIFTED_X
0,

//LTFT_SHIFTED_Y
0,

//LTFT_CURSOR_MOVEMENT
0,

//LTFT_ANCHOR
0,

//LTFT_LINK
LTFLT_UNDERLINE,

//LTFT_BUTTON
0,

//LTFT_CHECK_BOX
0,

//LTFT_MACRO
LTFLT_UNDERLINE,

//LTFT_MENU_VAL
LTFLT_UNDERLINE,

//LTFT_HEX_ED
0,

//LTFT_HIDE_START
LTFLT_UNDERLINE,

//LTFT_HIDE_END
0,

//LTFT_TREE
LTFLT_UNDERLINE,

//LTFT_PICTURE
0,

//LTFT_INSERT_BINARY
0,

//LTFT_INSERT_BINARY_TYPE
0,

//LTFT_INSERT_BINARY_SIZE
0,

//LTFT_BPT
0,

//LTFT_SONG
0,

//LTFT_HIGHLIGHT
0,

//LTFT_LOCK_TO_PAGE
0,

//LTFT_LOCK_TO_CENTER
0,

//LTFT_HTML_IMG
0,

//LTFT_HTML_VIDEO
0,

//LTFT_HTML_CODE
0,

//LTFT_ERROR
0
};


U8 *Ltf2PlainText(Ltf *l,LtfEntry *ll)
{ //TODO: break strs
  I64 i,j;
  U64 t1,f1,flags,type;
  U8 *buf,*buf2;

  buf=StrNew(DefineSub(ll->type_u8,"ST_LTF_CMDS"));
  if (ll->type_u8!=LTFT_ERROR) {
    f1=ltf_dft_flags[ll->type_u8];
    t1=ll->type_u8|ltf_dft_type_flags[ll->type_u8];

    flags=ll->flags & ~(LTF_BL_IV_UL|LTFLF_SELECTED|LTFLF_HIGHLIGHT|LTFLF_WORD_WRAP|LTFLF_LOCK_TO_PAGE|LTFLF_LOCK_TO_CENTER|LTFLF_SKIP|LTFLF_FILTER_SKIP);
    for (i=0;i<LTFLf_NUM_FLAGS;i++)
      if (Bt(&f1,i)!=Bt(&flags,i)) {
	if (Bt(&flags,i))
	  buf=StrAdd(buf,"+");
	else
	  buf=StrAdd(buf,"-");
	buf=StrAdd(buf,DefineSub(i,"ST_LTF_FLAGS"));
      }

    type=ll->type&~LTFLT_SELECTED;
    for (i=LTFLt_BLINK;i<=LTFLt_UNDERLINE;i++)
      if (Bt(&t1,i)!=Bt(&type,i)) {
	if (Bt(&type,i))
	  buf=StrAdd(buf,"+");
	else
	  buf=StrAdd(buf,"-");
	buf=StrAdd(buf,DefineSub(i,"ST_LTF_FLAGS"));
      }
    buf=StrAdd(buf,",");
    nobound_switch (ll->type_u8) {
      case LTFT_TEXT:
      case LTFT_LINK:
      case LTFT_ANCHOR:
      case LTFT_MENU_VAL:
      case LTFT_BUTTON:
      case LTFT_CHECK_BOX:
      case LTFT_TREE:
      case LTFT_PICTURE:
      case LTFT_INSERT_BINARY:
      case LTFT_INSERT_BINARY_TYPE:
      case LTFT_INSERT_BINARY_SIZE:
      case LTFT_SONG:
      case LTFT_HTML_IMG:
      case LTFT_HTML_VIDEO:
      case LTFT_HTML_CODE:
	if (i==LTFT_CHECK_BOX)
	  buf2=ll->tag+4;
	else if (ll->flags & LTFLF_TREE)
	  buf2=ll->tag+3;
	else
	  buf2=ll->tag;
	buf2=MSPrintF("%s\"%$$Q\",",buf,buf2);
	Free(buf); buf=buf2;
	break;
      case LTFT_DATA:
	buf2=MSPrintF("%s%d,",buf,ll->len);
	Free(buf); buf=buf2;
	break;
      case LTFT_HEX_ED:
	buf2=MSPrintF("%s%d,",buf,ll->len);
	Free(buf); buf=buf2;
	buf2=MSPrintF("%s%d,",buf,ll->hex_ed_width);
	Free(buf); buf=buf2;
	break;
      case LTFT_PAGE_LEN:
      case LTFT_LEFT_MARGIN:
      case LTFT_RIGHT_MARGIN:
      case LTFT_HEADER:
      case LTFT_FOOTER:
      case LTFT_INDENT:
      case LTFT_FOREGROUND:
      case LTFT_BACKGROUND:
      case LTFT_DFT_FOREGROUND:
      case LTFT_DFT_BACKGROUND:
      case LTFT_ALT_FOREGROUND:
      case LTFT_LINK_FOREGROUND:
      case LTFT_MACRO_FOREGROUND:
      case LTFT_ANCHOR_FOREGROUND:
      case LTFT_HIDDEN_FOREGROUND:
      case LTFT_TREE_FOREGROUND:
      case LTFT_USER_FOREGROUND:
      case LTFT_BINARY_FOREGROUND:
      case LTFT_KEYWORD_FOREGROUND:
      case LTFT_KEYWORD2_FOREGROUND:
      case LTFT_COMMENT_FOREGROUND:
      case LTFT_QUOTE_FOREGROUND:
      case LTFT_WORD_WRAP:
      case LTFT_HIGHLIGHT:
      case LTFT_LOCK_TO_PAGE:
      case LTFT_LOCK_TO_CENTER:
      case LTFT_UNDERLINE:
      case LTFT_INVERT:
      case LTFT_BLINK:
      case LTFT_SHIFTED_X:
      case LTFT_SHIFTED_Y:
	if (ll->attr!=LTF_DFT) {
	  buf2=MSPrintF("%s%d,",buf,ll->attr);
	  Free(buf); buf=buf2;
	}
      case LTFT_NUM_TYPES-1: //nobound_switch
	break;
    }
    flags=ll->flags& (LTFLF_SCROLLING_X|LTFLF_SHIFTED_X|LTFLF_SCROLLING_Y|
    LTFLF_AUX_STR|LTFLF_LEFT_EXP|LTFLF_LEFT_MACRO|
    LTFLF_HAS_BIN|LTFLF_RIGHT_EXP|LTFLF_RIGHT_MACRO|
    LTFLF_WIDTH|LTFLF_HEIGHT|LTFLF_DEFINE|LTFLF_HTML_LINK);
    while (flags) {
      j=Bsf(flags);
      Btr(&flags,j);
      nobound_switch (j) {
	case LTFLf_SCROLLING_X:
	  buf2=MSPrintF("%s%d",buf,ll->scroll_len);
	  Free(buf); buf=buf2;
	  break;
	case LTFLf_SHIFTED_X:
	  j=ll->type.u16[1]&0x1F;
	  if (j&0x10) j|=0xFFFFFFF0;
	  buf2=MSPrintF("%s%d",buf,j);
	  Free(buf); buf=buf2;
	  break;
	case LTFLf_SHIFTED_Y:
	  j=ll->type>>21 &0x1F;
	  if (j&0x10) j|=0xFFFFFFF0;
	  buf2=MSPrintF("%s%d",buf,j);
	  Free(buf); buf=buf2;
	  break;
	case LTFLf_AUX_STR:
	  buf2=MSPrintF("%s\"%$$Q\"",buf,ll->aux_str);
	  Free(buf); buf=buf2;
	  break;
	case LTFLf_LEFT_EXP:
	  buf2=MSPrintF("%s%d",buf,ll->left_exp);
	  Free(buf); buf=buf2;
	  break;
	case LTFLf_LEFT_MACRO:
	  buf2=MSPrintF("%s\"%$$Q\"",buf,ll->left_macro);
	  Free(buf); buf=buf2;
	  break;
	case LTFLf_RIGHT_EXP:
	  buf2=MSPrintF("%s%d",buf,ll->right_exp);
	  Free(buf); buf=buf2;
	  break;
	case LTFLf_RIGHT_MACRO:
	  buf2=MSPrintF("%s\"%$$Q\"",buf,ll->right_macro);
	  Free(buf); buf=buf2;
	  break;
	case LTFLf_HAS_BIN:
	  buf2=MSPrintF("%s%d",buf,ll->bin_num);
	  Free(buf); buf=buf2;
	  break;
	case LTFLf_WIDTH:
	  buf2=MSPrintF("%s%d",buf,ll->width);
	  Free(buf); buf=buf2;
	  break;
	case LTFLf_HEIGHT:
	  buf2=MSPrintF("%s%d",buf,ll->height);
	  Free(buf); buf=buf2;
	  break;
	case LTFLf_DEFINE:
	  buf2=MSPrintF("%s\"%$$Q\"",buf,ll->define_str);
	  Free(buf); buf=buf2;
	  break;
	case LTFLf_HTML_LINK:
	  buf2=MSPrintF("%s\"%$$Q\"",buf,ll->html_link);
	  Free(buf); buf=buf2;
	  break;
#assert LTFLf_HTML_LINK==LTFLf_NUM_FLAGS-1 //nobound_switch
      }
      buf=StrAdd(buf,",");
    }
    buf[StrLen(buf)-1]=0;  //Kill last comma
  }
  buf2=StrNew(buf,l->mem_task); //exact allocation
  Free(buf);
  return buf2;
}


LtfEntry *PrsDollarCmd(Ltf *l,U8 *st)
{//This uses $LK,"Lex","MN:Lex"$() to parse a string and make Ltf entries.
  I64 i,j,flags;
  U8 *ptr,*st2;
  LtfEntry *ll=NULL;
  LexStruct *lx=LexNew(st,LF_DONT_FREE_BUF);
  Lex(lx);
  if (lx->token==TK_IDENT) {
    i=DefineMatch(lx->ident,"ST_LTF_CMDS");
    if (i<0)
      i=LTFT_ERROR;
    Lex(lx); //skip cmd code
    ll=CAlloc(sizeof(LtfEntry),l->mem_task);
    ll->type=i;
    ll->flags=ltf_dft_flags[i];
    ll->type|=ltf_dft_type_flags[i];
    PrsLtfFlags(lx,&ll->flags,&ll->type);
    if (lx->token==',')
      Lex(lx);
    nobound_switch (i) {
      case LTFT_TEXT:
      case LTFT_LINK:
      case LTFT_ANCHOR:
      case LTFT_MENU_VAL:
      case LTFT_BUTTON:
      case LTFT_CHECK_BOX:
      case LTFT_TREE:
      case LTFT_PICTURE:
      case LTFT_INSERT_BINARY:
      case LTFT_INSERT_BINARY_TYPE:
      case LTFT_INSERT_BINARY_SIZE:
      case LTFT_SONG:
      case LTFT_HTML_IMG:
      case LTFT_HTML_VIDEO:
      case LTFT_HTML_CODE:
	if (lx->token==TK_STR) {
	  st2=LexExtStr(lx);
	  if (i==LTFT_CHECK_BOX) {
	    st=MSPrintF("[X] %s",st2);
	    Free(st2);
	    ll->min_col=1;
	  } else if (ll->flags & LTFLF_TREE) {
	    st=MSPrintF("+] %s",st2);
	    Free(st2);
	    ll->min_col=1;
	  } else
	    st=st2;
	  ll->tag=StrNew(st,l->mem_task);
	  Free(st);
	} else {
	  if (LTFT_PICTURE<=i<=LTFT_INSERT_BINARY_SIZE ||
	      i==LTFT_SONG)
	    ll->tag=StrNew("",l->mem_task);
	  else {
	    ll->tag=NULL;
	    ll->type=LTFT_ERROR;
	  }
	}
	break;
      case LTFT_HIDE_START:
	ll->tag=StrNew("<-",l->mem_task);
	ll->min_col=1;
	break;
      case LTFT_HIDE_END:
	ll->tag=StrNew(">",l->mem_task);
	break;
      case LTFT_DATA:
	ll->flags|=LTFLF_AUX_STR; //don't allow switched off
	ll->len=Expression(lx);
	break;
      case LTFT_HEX_ED:
	ll->len=Expression(lx);
	if (lx->token==',')
	  Lex(lx);
	ll->hex_ed_width=Expression(lx);
	break;
      case LTFT_PAGE_LEN:
      case LTFT_LEFT_MARGIN:
      case LTFT_RIGHT_MARGIN:
      case LTFT_HEADER:
      case LTFT_FOOTER:
      case LTFT_INDENT:
      case LTFT_FOREGROUND:
      case LTFT_BACKGROUND:
      case LTFT_DFT_FOREGROUND:
      case LTFT_DFT_BACKGROUND:
      case LTFT_ALT_FOREGROUND:
      case LTFT_LINK_FOREGROUND:
      case LTFT_MACRO_FOREGROUND:
      case LTFT_ANCHOR_FOREGROUND:
      case LTFT_HIDDEN_FOREGROUND:
      case LTFT_TREE_FOREGROUND:
      case LTFT_USER_FOREGROUND:
      case LTFT_BINARY_FOREGROUND:
      case LTFT_KEYWORD_FOREGROUND:
      case LTFT_KEYWORD2_FOREGROUND:
      case LTFT_COMMENT_FOREGROUND:
      case LTFT_QUOTE_FOREGROUND:
      case LTFT_WORD_WRAP:
      case LTFT_HIGHLIGHT:
      case LTFT_LOCK_TO_PAGE:
      case LTFT_LOCK_TO_CENTER:
      case LTFT_UNDERLINE:
      case LTFT_INVERT:
      case LTFT_BLINK:
      case LTFT_SHIFTED_X:
      case LTFT_SHIFTED_Y:
	if (!lx->token)
	  ll->attr=LTF_DFT;
	else
	  ll->attr=Expression(lx);
	break;
      case LTFT_ERROR:
	ll->flags=0;
	ll->type=LTFT_ERROR;
#assert LTFT_ERROR==LTFT_NUM_TYPES-1
//	case LTFT_NUM_TYPES-1: //nobound_switch
	break;
    }
 
    flags=ll->flags& (LTFLF_SCROLLING_X|LTFLF_SHIFTED_X|LTFLF_SCROLLING_Y|
      LTFLF_AUX_STR|LTFLF_LEFT_EXP|LTFLF_LEFT_MACRO|
      LTFLF_HAS_BIN|LTFLF_RIGHT_EXP|LTFLF_RIGHT_MACRO|
      LTFLF_WIDTH|LTFLF_HEIGHT|LTFLF_DEFINE|LTFLF_HTML_LINK);
    while (flags) {
      j=Bsf(flags);
      Btr(&flags,j);
      while (lx->token==',')
	Lex(lx);
      nobound_switch (j) {  //TODO: Might check for expression errors
	case LTFLf_SCROLLING_X:
	  ll->scroll_len=Expression(lx);
	  break;
	case LTFLf_SHIFTED_X:
	  ll->type|=(Expression(lx) & 0x1F)<<16;
	  break;
	case LTFLf_SHIFTED_Y:
	  ll->type|=(Expression(lx) & 0x1F)<<21;
	  break;
	case LTFLf_AUX_STR:
	  if (lx->token==TK_STR) {
	    st2=LexExtStr(lx);
	    ll->aux_str=StrNew(st2,l->mem_task);
	    Free(st2);
	    if (i==LTFT_MACRO && !ll->tag)
	      ll->tag=StrNew(ll->aux_str,l->mem_task);
	    //$AN,"Anchor","DataTagWidth"$
	    if (i==LTFT_DATA) {
	      if (ptr=StrStr(ll->aux_str,":"))
		ll->min_col=ptr-ll->aux_str+1;
	      ll->tag=MAlloc(ll->len+ll->min_col+2,l->mem_task); //+2 because "_\0"
	    }
	  } else
	    ll->type=LTFT_ERROR;
	  break;
	case LTFLf_LEFT_EXP:
	  ll->left_exp=Expression(lx);
	  break;
	case LTFLf_LEFT_MACRO:
	  if (lx->token==TK_STR) {
	    st2=LexExtStr(lx);
	    ll->left_macro=StrNew(st2,l->mem_task);
	    Free(st2);
	    if (i==LTFT_MACRO && !ll->tag)
	      ll->tag=StrNew(ll->left_macro,l->mem_task);
	  } else
	    ll->type=LTFT_ERROR;
	  break;
	case LTFLf_RIGHT_EXP:
	  ll->right_exp=Expression(lx);
	  break;
	case LTFLf_RIGHT_MACRO:
	  if (lx->token==TK_STR) {
	    st2=LexExtStr(lx);
	    ll->right_macro=StrNew(st2,l->mem_task);
	    Free(st2);
	    if (i==LTFT_MACRO && !ll->tag)
	      ll->tag=StrNew(ll->right_macro,l->mem_task);
	  } else
	    ll->type=LTFT_ERROR;
	  break;
	case LTFLf_HAS_BIN:
	  if (!lx->token)
	    ll->bin_num=0;
	  else
	    ll->bin_num=Expression(lx);
	  break;
	case LTFLf_WIDTH:
	  ll->width=Expression(lx);
	  break;
	case LTFLf_HEIGHT:
	  ll->height=Expression(lx);
	  break;
	case LTFLf_DEFINE:
	  if (lx->token==TK_STR) {
	    st2=LexExtStr(lx);
	    ll->define_str=StrNew(st2,l->mem_task);
	    Free(st2);
	  } else
	    ll->type=LTFT_ERROR;
	  break;
	case LTFLf_HTML_LINK:
	  if (lx->token==TK_STR) {
	    st2=LexExtStr(lx);
	    ll->html_link=StrNew(st2,l->mem_task);
	    Free(st2);
	  } else
	    ll->type=LTFT_ERROR;
	  break;
#assert LTFLf_HTML_LINK==LTFLf_NUM_FLAGS-1 //nobound_switch
      }
    }
  } else if (!ll) {
    ll=CAlloc(sizeof(LtfEntry),l->mem_task);
    ll->type=LTFT_ERROR;
  }
  LexDel(lx);
  return ll;
}

public U0 LtfInsert(Ltf *l,LtfEntry *ll)
{
  U8 *dst;
  BoolI8 unlock=LtfLock(l);
  LtfEntry *cl=l->cur_entry,*nl;

  ll->x=cl->x;
  ll->y=cl->y;
  ll->page_line_num=cl->page_line_num;
  ll->settings=NULL;
  if (l->cur_data_col>0 &&
      cl->type_u8==LTFT_TEXT &&
      !(cl->flags&(LTFLF_TAG_CB|LTFLF_DEFINE|LTFLF_HTML_LINK)) &&
      l->cur_data_col<cl->max_col) {
    nl=CAlloc(sizeof(LtfEntryBase)+sizeof(U0 *),l->mem_task);
    nl->type=LTFT_TEXT|cl->type&0xFFFFFF00;
    nl->x=cl->x;
    nl->y=cl->y;
    nl->page_line_num=cl->page_line_num;
    nl->flags=cl->flags;
    dst=cl->tag+l->cur_data_col;
    nl->tag=StrNew(dst,l->mem_task);
    *dst=0;
    cl->max_col=l->cur_data_col;
    InsQue(nl,cl);
    l->cur_data_col=0;
    cl=nl;
    cl->max_col=StrLen(cl->tag);
  }
  if (cl->type_u8==LTFT_TEXT && l->cur_data_col>=cl->max_col) {
    InsQue(ll,cl);
    l->cur_entry=ll->next;
  } else {
    InsQue(ll,cl->last);
    l->cur_entry=cl;
  }
  l->cur_data_col=l->cur_entry->min_col;
  LtfRemSoftNewLines(l,l->cur_entry);
  if (unlock)
    LtfUnlock(l);
}


#help_index "Ltf/Output"
public LtfEntry *LtfPutS(Ltf *l,U8 *st)
{ //Does not handle partial Ltf entries
  //Returns last newly created dollar-sign LtfEntry.
  U8 ch,*ptr=st,*ptr2,*st2,*ptr3,*ptr4,*src,
     *char_bitmap;
  BoolI8 unlock;
  I64 j;
  LtfEntry *ll=NULL,*result=NULL,*cl;
  if (!st || !l || l->ltf_signature!=LTF_SIGNATURE_VAL)
    return NULL;
  unlock=LtfLock(l);
  if (l->flags & LTFF_PLAIN_TEXT_WITH_TABS)
    char_bitmap=zero_cr_nl_cursor_bitmap;
  else if (l->flags & LTFF_PLAIN_TEXT)
    char_bitmap=zero_tab_ff_cr_nl_cursor_bitmap;
  else
    char_bitmap=zero_tab_ff_cr_nl_cursor_dollar_bitmap;
  l->recalc_start=l; //TODO:This might be optimized
  cl=l->cur_entry;
  while (*ptr) {
    ptr2=ptr;
    do ch=*ptr++;
    while (!Bt(char_bitmap,ch) || ch==CH_CURSOR && l->flags&LTFF_NO_CURSOR);
    ptr--;
    if (!ch) {
      if (j=ptr-ptr2) {
	ll=CAlloc(sizeof(LtfEntryBase)+sizeof(U0 *),l->mem_task);
	ll->type=LTFT_TEXT | l->settings_root.dft_text_attr << 8;
	if (l->flags & LTFF_NO_CURSOR) {
	  src=MAlloc(j+1);
	  MemCpy(src,ptr2,j+1);
	  StrUtil(src,SUF_REM_CTRL_CHARS);
	  j=StrLen(src);
	} else
	  src=ptr2;
	ll->tag=MAlloc(j+1,l->mem_task);
	MemCpy(ll->tag,src,j+1);
	ll->max_col=j;
	LtfInsert(l,ll);
	if (l->flags & LTFF_NO_CURSOR)
	  Free(src);
      }
    } else {
      if (j=ptr-ptr2) {
	*ptr=0;
	ll=CAlloc(sizeof(LtfEntryBase)+sizeof(U0 *),l->mem_task);
	ll->type=LTFT_TEXT | l->settings_root.dft_text_attr << 8;
	if (l->flags & LTFF_NO_CURSOR) {
	  src=MAlloc(j+1);
	  MemCpy(src,ptr2,j+1);
	  ptr3=src;
	  ptr4=src;
	  while (*ptr3)
	    if (*ptr3!=CH_CURSOR)
	      *ptr4++=*ptr3++;
	    else
	      ptr3++;
	  *ptr4=0;
	  j=ptr4-src;
	} else
	  src=ptr2;
	ll->tag=MAlloc(j+1,l->mem_task);
	MemCpy(ll->tag,src,j+1);
	ll->max_col=j;
	LtfInsert(l,ll);
	if (l->flags & LTFF_NO_CURSOR)
	  Free(src);
	*ptr=ch;
      }
      switch (ch) {
	case CH_CURSOR:
	  ll=CAlloc(sizeof(LtfEntryBase),l->mem_task);
	  ll->type=LTFT_CURSOR | l->settings_root.dft_text_attr << 8;
	  LtfInsert(l,ll);
	  ptr++;
	  break;
	case CH_FORM_FEED:
	  ll=CAlloc(sizeof(LtfEntryBase),l->mem_task);
	  ll->type=LTFT_PAGE_BREAK | l->settings_root.dft_text_attr << 8;
	  LtfInsert(l,ll);
	  ptr++;
	  break;
	case CH_TAB:
	  ll=CAlloc(sizeof(LtfEntryBase),l->mem_task);
	  ll->type=LTFT_TAB | l->settings_root.dft_text_attr << 8;
	  LtfInsert(l,ll);
	  ptr++;
	  break;
	case '$$':
	  ptr++; //skip first dollar
	  ptr2=ptr;
	  while (*ptr && *ptr!='$$')
	    ptr++;
	  if (*ptr) {
	    *ptr=0; //zero second dollar
	    if (ptr-1==ptr2 && *ptr2==CH_CURSOR) {
	      ll=CAlloc(sizeof(LtfEntryBase),l->mem_task);
	      ll->type=LTFT_CURSOR | l->settings_root.dft_text_attr << 8;
	      LtfInsert(l,ll);
	      ptr2++;
	    }
	    if (ptr==ptr2) {
	      ll=CAlloc(sizeof(LtfEntryBase)+sizeof(U0 *),l->mem_task);
	      ll->type=LTFT_TEXT | l->settings_root.dft_text_attr << 8;
	      ll->max_col=1;
	      if (l->flags & LTFF_DBL_DOLLARS)
		ll->tag=StrNew("$$$$",l->mem_task);
	      else
		ll->tag=StrNew("$$",l->mem_task);
	      LtfInsert(l,ll);
	    } else {
	      st2=MAlloc(ptr-ptr2+1);
	      ptr3=ptr2;
	      ptr4=st2;
	      while (ch=*ptr3++) {
		if (ch==CH_CURSOR) {
		  ll=CAlloc(sizeof(LtfEntryBase),l->mem_task);
		  ll->type=LTFT_CURSOR | l->settings_root.dft_text_attr << 8;
		  LtfInsert(l,ll);
		} else if (ch==CH_FORM_FEED) {
		  ll=CAlloc(sizeof(LtfEntryBase),l->mem_task);
		  ll->type=LTFT_PAGE_BREAK | l->settings_root.dft_text_attr << 8;
		  LtfInsert(l,ll);
		} else
		  *ptr4++=ch;
	      }
	      *ptr4=0;
	      if (ll=PrsDollarCmd(l,st2)) {
		result=ll;
		LtfInsert(l,ll);
	      }
	      Free(st2);
	    }
	    *ptr++='$$';
	  }
	  break;
	default:
	  ll=CAlloc(sizeof(LtfEntryBase),l->mem_task);
	  ll->type=LTFT_NEW_LINE | l->settings_root.dft_text_attr << 8;
	  LtfInsert(l,ll);
	  if (ch==CH_CARRIAGE_RETURN)
	    while (*ptr==CH_CARRIAGE_RETURN)
	      ptr++;
	  if (*ptr==CH_NEW_LINE)
	    ptr++;
	  while (*ptr==CH_CARRIAGE_RETURN)
	    ptr++;
      }
    }
  }
  if (unlock)
    LtfUnlock(l);
  return result;
}


public LtfEntry *LtfPrintF(Ltf *l,U8 *src,...)
{
  LtfEntry *result;
  U8 *buf=SPrintFJoin(NULL,src,argc,argv);
  result=LtfPutS(l,buf);
  Free(buf);
  return result;
}

#help_index "Ltf"

