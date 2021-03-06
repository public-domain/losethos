#help_index "Menus"
#help_file "::/LT/Doc/Menus"

MenuEntry *sys_cur_submenu_entry=NULL;

public MenuEntry *MenuSubEntryFind(MenuEntry *first,U8 *entry_name)
{
  while (first) {
    if (!StrCmp(first->name,entry_name))
      return first;
    first=first->next;
  }
  return NULL;
}

public MenuEntry *MenuEntryFind(Menu *m,U8 *full_name)
{
  U8 *st,*st2;
  MenuEntry *tempse;
  if (!m || m->mu_signature!=MENU_SIGNATURE_VAL || !full_name)
    return NULL;
  st=StrNew(full_name);
  st2=StrNew(full_name);
  tempse=(&m->sub)(U8 *)-offset(MenuEntry.sub);
  while (*st && tempse) {
    StrFirstRem(st,"/",st2);
    tempse=MenuSubEntryFind(tempse->sub,st2);
  }
  return tempse;
}

MenuEntry *MenuNewSub(LexStruct *lx,TaskStruct *task)
{
  MenuEntry *tempme=NULL,*tempse;
  if (lx->token==TK_IDENT) {
    tempme=CAlloc(sizeof(MenuEntry),task);
    if (StrLen(lx->ident)>31)
      lx->ident[31]=0;
    StrCpy(tempme->name,lx->ident);
    Lex(lx); //skip ident
    if (lx->token=='(') {
      tempme->msg_code=MSG_KEY_DOWN_UP;
      Lex(lx); //Skip (
      if (lx->token!=',' && lx->token!=')')
	tempme->msg_code=Expression(lx);
      if (lx->token==',')
	Lex(lx);
      if (lx->token!=',' && lx->token!=')')
	tempme->p1=Expression(lx);
      if (lx->token==',')
	Lex(lx);
      if (lx->token!=',' && lx->token!=')')
	tempme->p2=Expression(lx);
      if (lx->token!=')')
	PrsExcept(lx,"ST_MISSING_RIGHT_PAREN");
      if (Lex(lx)!=';')
	PrsExcept(lx,"ST_MISSING_SEMICOLON");
      Lex(lx); //Skip ;
    } else if (lx->token=='{') {
      Lex(lx); //Skip {
      tempme->dir=TRUE;
      tempse=&tempme->sub;
      while (tempse && lx->token!='}')
	tempse=tempse->next=MenuNewSub(lx,task);
      if (lx->token!='}')
	PrsExcept(lx,"ST_MISSING_RIGHT_BRACE");
      else
	Lex(lx); //Skip }
    } else
      PrsExcept(lx,"ST_EXPECTING_LEFT_BRACE");
  }
  return tempme;
}

public Menu *MenuNew(U8 *st,I64 flags=0,TaskStruct *task=NULL)
{
  Menu *m;
  MenuEntry *tempse;
  LexStruct *lx=LexNew(st,LF_DONT_FREE_BUF);
  if (!task) task=Fs;
  Lex(lx);
  m=CAlloc(sizeof(Menu),task);
  m->task=task;
  m->flags=flags;
  m->attr	 =LTBLUE<<4 +YELLOW;
  m->checked_attr=LTGREEN<<4+BROWN;
  tempse=&m->sub;
  while (tempse)
    tempse=tempse->next=MenuNewSub(lx,task);
  LexDel(lx);
  m->mu_signature=MENU_SIGNATURE_VAL;
  return m;
}

public Menu *MenuFile(U8 *filename,I64 flags=0,TaskStruct *task=NULL)
{
  Menu *m;
  U8 *st=MSPrintF("#include \"%s\"",filename);
  m=MenuNew(st,flags,task);
  Free(st);
  return m;
}

U0 MenuDelSub(MenuEntry *tempme)
{
  MenuEntry *tempse,*tempse1;
  if (tempme) {
    tempse=tempme->sub;
    while (tempse) {
      tempse1=tempse->next;
      MenuDelSub(tempse);
      tempse=tempse1;
    }
    Free(tempme);
  }
}

public U0 MenuDel(Menu *m)
{
  MenuEntry *tempme,*tempme1;
  if (!m || m->mu_signature!=MENU_SIGNATURE_VAL)
    return;
  m->mu_signature=0;
  tempme=m->sub;
  while (tempme) {
    tempme1=tempme->next;
    MenuDelSub(tempme);
    tempme=tempme1;
  }
  Free(m);
}

I64 MenuEntryWidth(MenuEntry *tempme)
{
  I64 result=StrLen(tempme->name);
  MenuEntry *tempse=tempme->sub;
  while (tempse) {
    result=MaxI64(result,StrLen(tempse->name));
    tempse=tempse->next;
  }
  return result+1;
}

public Menu *MenuPush(U8 *st)
{
  Menu *m=MenuNew(st);
  m->next=Fs->cur_menu;
  Fs->cur_menu=m;
  return m;
}

public Menu *MenuFilePush(U8 *filename)
{
  Menu *m=MenuFile(filename);
  m->next=Fs->cur_menu;
  Fs->cur_menu=m;
  return m;
}

public U0 MenuPop()
{
  Menu *m=Fs->cur_menu;
  if (!m) return;
  Fs->cur_menu=m->next;
  MenuDel(m);
}

U0 DrawMenu(GrDC *dc)
{
  Menu *m;
  MenuEntry *tempme,*tempse,*cur_submenu=NULL;
  U8 *st=NULL;
  TaskStruct *task=sys_focus_task;
  I64 i,w,x0,y0,x1=ipx,y1=ipy;
  if (!TaskValidate(task) || !(m=task->cur_menu) ||
      m->mu_signature!=MENU_SIGNATURE_VAL) {
    sys_cur_submenu_entry=NULL;
    return;
  }

  dc->color=m->attr>>4;
  GrRect(dc,0,0,GR_WIDTH,FONT_HEIGHT);
  x0=0;
  tempme=m->sub;
  while (tempme) {
    w=MenuEntryWidth(tempme)*FONT_WIDTH;
    if (x0<=x1<x0+w) {
      if (0<=y1<FONT_HEIGHT) {
	dc->color=m->attr&15;
	GrRect(dc,x0,0,w,FONT_HEIGHT);
	dc->color=m->attr>>4;
      } else
	dc->color=m->attr&15;
      GrPutS(dc,x0,0,tempme->name);
      y0=FONT_HEIGHT;
      tempse=tempme->sub;
      while (tempse) {
	if (tempse->checked)
	  i=m->checked_attr;
	else
	  i=m->attr;
	if (y0<=y1<y0+FONT_HEIGHT) {
	  if (tempse->msg_code==MSG_KEY_DOWN||tempse->msg_code==MSG_KEY_DOWN_UP) {
	    if (!tempse->p2)
	      tempse->p2=A2ScanCode(tempse->p1);
	    st=ScanCode2KeyName(tempse->p2);
	  }
	  sys_cur_submenu_entry=cur_submenu=tempse;
	  dc->color=i&15;
	  GrRect(dc,x0,y0,w,FONT_HEIGHT);
	  dc->color=i>>4;
	  GrPutS(dc,x0,y0,tempse->name);
	  if (st) {
	    dc->color=i>>4;
	    GrRect(dc,x0+w,y0-FONT_HEIGHT,(StrLen(st)+1)*FONT_WIDTH,FONT_HEIGHT*3);
	    dc->color=i&15;
	    GrPutS(dc,x0+w,y0,st);
	    Free(st);
	  }
	} else {
	  dc->color=i>>4;
	  GrRect(dc,x0,y0,w,FONT_HEIGHT);
	  dc->color=i&15;
	  GrPutS(dc,x0,y0,tempse->name);
	}
	y0+=FONT_HEIGHT;
	tempse=tempse->next;
      }
    } else {
      dc->color=m->attr&15;
      GrPutS(dc,x0,0,tempme->name);
    }
    x0+=w;
    tempme=tempme->next;
  }
  sys_cur_submenu_entry=cur_submenu;
}


#help_index ""

