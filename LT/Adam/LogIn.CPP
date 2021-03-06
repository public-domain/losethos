#help_index "Task/Login"

//This is temporary until defined in $LK,"HOME/Adam3fSys.APZ","FI:HOME/Adam3fSys.APZ"$.
U0 UserStartUp()
{ //Run each time a user a spawned
  UseTermLtf("PersonalMenu.TXZ");
  LtfCurAttr(WHITE<<4+BLUE);
  Type("::/LT/Doc/Start.TXZ");
  LBts(&Fs->display_flags,DISPLAYf_SHOW);
  WinZBufUpdate;
  if (!IsSysDeterministic)
    Preempt(ON);
}

//This is temporary until defined in $LK,"HOME/Adam3fSys.APZ","FI:HOME/Adam3fSys.APZ"$.
U0 ServantStartUp()
{ //Run each time a servant task is spawned.
  UseTermLtf(NULL);
  LBts(&Fs->display_flags,DISPLAYf_SHOW);
  WinZBufUpdate;
  if (!IsSysDeterministic)
    Preempt(ON);
}

U0 AcctCollectList()
{
  LTDirEntry *tempm1;
  SysAcctStruct *tempa;
  sys_accts.next=sys_accts.last=&sys_accts;
  tempm1=FilesFind("/LT/Accts/*",1<<FUf_JUST_DIRS);
  while (tempm1) {
    if (*tempm1->name!='.') {
      tempa=CAlloc(sizeof(SysAcctStruct));
      StrCpy(tempa->name,tempm1->name);
      InsQue(tempa,sys_accts.last);
    }
    tempm1=tempm1->next;
  }
  LTDirListDel(tempm1);
}

public SysAcctStruct *AcctFind(U8 *name)
{
  SysAcctStruct *tempa;
  tempa=sys_accts.next;
  while (tempa!=&sys_accts) {
    if (!StrCmp(tempa->name,name))
      return tempa;
    tempa=tempa->next;
  }
  return NULL;
}

SysAcctStruct *PopUpGetSysAcct()
{
  Ltf *l=LtfNew;
  SysAcctStruct *tempa;

  LtfPutS(l,"Select Account.\nCursor down and press <SPACE BAR>.\n");
  tempa=sys_accts.next;
  while (tempa!=&sys_accts) {
    LtfPrintF(l,"$$CM+LX,2,3$$$$BT,\"%s\",%d$$",tempa->name,tempa);
    tempa=tempa->next;
  }
  LtfPutS(l,"\n");
  do tempa=PopUpMenu(l);
  while (tempa<0);
  LtfDel(l);
  return tempa;
}

U0 SysLogin()
{
  SysAcctStruct *tempa=AcctFind(sys_acct_name);
  if (!tempa)
    tempa=PopUpGetSysAcct;
  sys_acct=tempa;
  HomeDirRst;
}

#define ACCT_REGISTRY_FILENAME "HOME/Registry.CPZ"
Ltf *sys_registry_ltf=NULL;
I64 sys_msg_flags[1]={0};
F64 registry_version;

BoolI64 AcctCacheRegistry()
{
  BoolI8 old_silent;
  if (!sys_registry_ltf) {
    old_silent=Silent(ON);
    sys_registry_ltf=LtfRead(ACCT_REGISTRY_FILENAME);
    Silent(old_silent);
    return FALSE;
  } else
    return TRUE;
}

public BoolI64 AcctRegSetDftEntry(U8 *path,U8 *val,BoolI8 is_adam_entry=FALSE)
{
  BoolI8 unlock_l,result;
  AcctCacheRegistry;
  unlock_l=LtfLock(sys_registry_ltf);
  if (!LtfTreeFind(sys_registry_ltf,path)) {
    LtfTreeMake(sys_registry_ltf,path);
    LtfPutS(sys_registry_ltf,val);
    if (is_adam_entry) {
      if (Fs==adam_task)
	ExePutS(val);
      else
	Adam("%s",val);
    }
    if (IsDrvWritable(dft_drv))
      LtfWrite(sys_registry_ltf);
    result=FALSE;
  } else
    result=TRUE;
  if (unlock_l)
    LtfUnlock(sys_registry_ltf);
  return result;
}

public I64 AcctRegExecuteBranch(U8 *path)
{
  AcctCacheRegistry;
  return LtfTreeBranchExe(sys_registry_ltf,path);
}

public BoolI64 AcctRegWriteBranch(U8 *path,U8 *fmt,...)
{
  BoolI8 result,unlock_l;
  LtfEntry *tree_branch,*start_indent,*end_indent;
  U8 *buf=SPrintFJoin(NULL,fmt,argc,argv);
  AcctCacheRegistry;
  unlock_l=LtfLock(sys_registry_ltf);
  if (result=LtfTreeFind(sys_registry_ltf,path,&tree_branch,&start_indent,&end_indent))
    LtfCut(sys_registry_ltf,tree_branch,end_indent);
  LtfTreeMake(sys_registry_ltf,path);
  LtfPutS(sys_registry_ltf,buf);
  if (IsDrvWritable(dft_drv))
    LtfWrite(sys_registry_ltf);
  if (unlock_l)
    LtfUnlock(sys_registry_ltf);
  Free(buf);
  return result;
}
 
U0 AcctOneTimePopUp(I64 flag_num,U8 *msg)
{ //You're not supposed to make system pop-up flags, only me.
  Ltf *l=LtfNew;
  LtfEntry *ll;
  if (sys_acct && !Bt(sys_msg_flags,flag_num)) {
    if (msg) LtfPutS(l,msg);
    ll=LtfPutS(l,"\n$$CB+LE,\"Do not show this msg again.\",1$$");
    LtfPutS(l,"$$CM+CX,0,4$$$$BT,\"OKAY\",1$$\n");
    if (PopUpMenu(l)==1 && ll->flags&LTFLF_CHECKED_COLLAPSED) {
      LBts(sys_msg_flags,flag_num);
      AcctRegWriteBranch("Adam/LT/SysMsgFlags",
	"sys_msg_flags[0]=0x%X;\n",
	sys_msg_flags[0]);
    }
    LtfDel(l);
  }
}

U0 RegistryInit()
{
  U8 buf[136];
  BoolI8 existed=FileFind(ACCT_REGISTRY_FILENAME),version_present;
  AcctRegSetDftEntry("Adam/LT/SysMsgFlags","sys_msg_flags[0]=0;\n",TRUE);
  SPrintF(buf,"registry_version=%4.3f;\n",os_version);
  version_present=AcctRegSetDftEntry("Adam/LT/SysRegVer",buf,TRUE);
  AcctRegExecuteBranch("Adam");

  //Convert old registries
  if (existed && !version_present)
    registry_version=5.10;
  if (registry_version!=os_version) {
    if (registry_version<5.101)
      AcctRegWriteBranch("LT","");
    AcctRegWriteBranch("Adam/LT/SysRegVer",buf);
    AcctRegExecuteBranch("Adam");
  }
}

#help_index ""
