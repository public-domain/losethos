U0 UserStartUp()
{ //Run each time a user a spawned
  UseTermLtf("PersonalMenu.TXZ");
  LtfCurAttr(WHITE<<4+BLUE);
  Type("::/LT/Doc/Start.TXZ");
  LBts(&Fs->display_flags,DISPLAYf_SHOW);
  WinZBufUpdate;
  if (!IsSysDeterministic)
    Preempt(ON);
  Dir;
  CPURep;
}

U0 ServantStartUp()
{ //Run each time a servant task is spawned.
  UseTermLtf(NULL);
  LBts(&Fs->display_flags,DISPLAYf_SHOW);
  WinZBufUpdate;
  if (!IsSysDeterministic)
    Preempt(ON);
}

TaskStruct *user1,*user2;

U0 StartUpTasks(U64 dummy=0)
{
  nounusedwarn dummy;
  U8 buf[136];
  TaskStruct *task1,*task2;

  TaskLocalInit; //needed for disk access
  if (!IsSysDeterministic)
    Preempt(ON);
  Silent(ON);	//no output to screen

  try {
    task1=SpawnUser;
    user1=task1;
    task2=SpawnUser;
    user2=task2;
    WinToTop(task1);
    WinTileVert;
    if (!IsSysDeterministic)
      PopUp("#include \"::/LT/Adam/Start.CPZ\";");

    SPrintF(buf,"Boot Time:%h17Ts\n",BootTime);
    AdamLog(buf);
    XTalk(task1,"#include \"DoItOnce.CPZ\";\n");

    if (!IsSysDeterministic)
      WsInit("/LT/" TEXT_FILE_MASK ";~*Bible.TXZ");
    AdamLog("WordStat Time:%7.3fs\n",ToF64(BootTime)/LTDATE_FREQ);
    if (ts_has_win_drv)
      DrvEnable(TS_WIN_DRV_LET,OFF);
    if (!IsSysDeterministic)
      CacheLocalBlkDevDirs;
  } catch
    Fs->catch_except=TRUE;
}

DelTree("::/Temp/ScreenShots");
MkDir("::/Temp/ScreenShots");

//This must be spawned because start-up files
// may invoke the Adam() cmd.
Spawn(&StartUpTasks,NULL,"Spawn Users");
"\nOS Compile Time:%D %T\n",sys_compile_time,sys_compile_time;
