U0 DateTimeFmtMapsRst()
{
  I64 i;
  for (i=0;i<NUM_DATE_FMTS;i++)
    date_fmt_map[i]=i;
  for (i=0;i<NUM_TIME_FMTS;i++)
    time_fmt_map[i]=i;
}

U0 SysGlblsInit()
{
  I64 i,j;
  U8 *m;
  RaxRbxRcxRdx ee;

  CpuId(0x1,&ee);
  sys_cache_line_width=ee.rbx.u8[1]*8;
  sys_pci_dev_root.next=sys_pci_dev_root.last=&sys_pci_dev_root;
  sys_rnd_seed = 93186752^GetTimeStamp;
  sys_focus_task=Fs;
  sys_macro_root.next=sys_macro_root.last=&sys_macro_root;
  sys_dft_iso_name="::/Temp/CDDVD.ISO";
  MemSet(mp_apic_ids,0,MP_MAX_PROCESSORS*sizeof(U8));
  MemSet(sys_usb_devs,0,MAX_USB*sizeof(LTUSB));

  InDbg(ON);

  reverse_bits_table=CAlloc(256);
  for (i=0;i<256;i++)
    for (j=0;j<8;j++)
      if (Bt(&i,7-j))
	LBts(reverse_bits_table+i,j);
  set_bits_table=CAlloc(256);
  for (i=0;i<256;i++)
    for (j=0;j<8;j++)
      if (Bt(&i,j))
	set_bits_table[i]++;

  ext=CAlloc(EXT_TABLE_SIZE*sizeof(U0 *));
  ext[EXT_DEBUGGER]=&SysDbgEnter;
  ext[EXT_EXIT_DEBUGGER]=&SysDbgExit;
  fp_mgets=&SysMGetS;

  fp_ctrl_alt_let_cbs=CAlloc(26*sizeof(U0 *));
  ctrl_alt_let_descs=CAlloc(26*sizeof(U8 *));;
  ctrl_alt_let_in_irq_flags=0;

  psect_cache=CAlloc(PSECT_CACHE_SIZE*sizeof(PSectCacheEntry));
  psect_cache_index=0;

  StrCpy(sys_temp_filename,"HOME/Temp.TXZ");
  MemSet(&sys_put_key_root,0,sizeof(KeyDevStruct));
  sys_put_key_root.next=sys_put_key_root.last=&sys_put_key_root;
  KeyDevAdd(&AKDInputFilterPutKey,&AKDInputFilterPutS,0x40000000,FALSE);
  KeyDevAdd(&AKDRawDisplayPutKey,&AKDRawDisplayPutS,0x60000000,FALSE);
  #exe {
    StreamPrintF("sys_acct_name=\"%s\";",osmain_cfg->acct_name);
    StreamPutS("#exe{OptOff(OPTf_PAREN_WARN);}");
    StreamPrintF("LTDCInit(%s);",osmain_cfg->dsk_cache_size_exp);
    StreamPutS("#exe{OptOn(OPTf_PAREN_WARN);}");
  };
  pow10_I64=CAlloc(sizeof(F64)*(308+308+1));
  for (i=-308;i<309;i++)
    pow10_I64[i+309]=Pow10(i);

//$AN,"Independent Heap Ctrl Example","Independent Heap Ctrl Example"$
  i=1; //Request 1 blk, we might get more.
  m=AllocUncachedMemBlks(&i,sys_code_bp);
  sys_uncached_heap=IndependentHeapCtrlInit(m,i*0x200000/0x200);
  ProgressBarsRst;
  date_fmt_map=MAlloc(NUM_DATE_FMTS);
  time_fmt_map=MAlloc(NUM_TIME_FMTS);
  DateTimeFmtMapsRst;
}

U0 SysGrInit()
{
//See $LK,"SYS_BIOS_GR_MODE","FF:D:/LT/OSMain/OSStart.CPZ,SYS_BIOS_GR_MODE:2"$
//See $LK,"::/LT/Adam/Gr/GrCfg.APZ","FI:::/LT/Adam/Gr/GrCfg.APZ"$
  gr_font=sys_font_8x8;
  if (bios_gr_mode==0x12 || bios_gr_mode==0x6A) {
    text_mode=FALSE;

/*
Mode must be multiple of 32 wide
$LK,"GrUpdateLine32","MN:GrUpdateLine32"$().
*/

    if (bios_gr_mode==0x6A) {
      gr_width =800;
      gr_height=600;
    } else {
      gr_width =640;
      gr_height=480;
    }
    text_cols=gr_width/FONT_WIDTH;
    text_rows=gr_height/FONT_HEIGHT;
    OutU8(VGA_SC_INDEX,VGA_MAP_MASK);
    OutU8(VGA_SC_DATA,0x0F);
    MemSet(VGA_GRAPHIC_MEM_BASE,0,gr_width*gr_height>>3);
    sys_dc_screen_image=CAlloc(gr_width*gr_height/8);
  } else {
    text_mode=TRUE;
    gr_width =640;
    gr_height=480;
    text_cols=80;
    text_rows=25;
    MemSet(VGA_TEXT_MEM_BASE,0,text_rows*text_cols<<1);
  }
}


U0 TimersInit()
{
  U64 i,*qq;
  U32 *_d;

  OutU8(0x43,0x34);
  OutU8(0x40,SYS_TIMER0_PERIOD);
  OutU8(0x40,SYS_TIMER0_PERIOD>>8);

// High Precision Event Timer

  if (PCIReadU16(0,31,0,0)==0x8086) { //Intel?
    //D31 F0, cfg 0xF0=RCBA of PCI-LPC Bridge
    _d=PCIReadU32(0,31,0,0xF0)(U8 *)&~0x3FFF+0x3404; //HPET cfg
  //7	enable
  //1:0 HPET is at 0xFED00000,0xFED01000, 0xFED02000 or 0xFED03000.
    *_d=*_d&3|0x80;
  }

  qq=HPET_GCAP_ID;
  i=*qq; //i.u32[1]= period in femtoS (10e-15)
  if (100000<i.u32[1]<1000000000) {
    HPET_freq	 =1000000000000000/i.u32[1];
    HPET_kHz_freq=1000000000000/i.u32[1];
    qq=HPET_GEN_CONF;
    *qq|=1; //Enable counting
  } else {
    HPET_freq=0;
    HPET_kHz_freq=0;
  }
}

U0 Reboot()
{
  U16 *d=0x472;
  Cli;
  if (mp_cnt>1)
    MPHalt;
  MemMarkPresent(d,TRUE);
  *d=0;
  OutU8(0x70,0x8F);
  OutU8(0x71,0x00);
  OutU8(0x70,0x00);
  OutU8(0x92,InU8(0x92)|1);
  SysHlt;
}

BoolI64 AdamStartUpScripts()
{
  BoolI8 result=TRUE;
  LexStruct *lx;
  Cd("/LT/OSMain");
  lx=LexNew(TextFileRead("Adam1.APZ"),1<<Lf_RAW_ERRORS,
    StrNew("::/LT/OSMain/Adam1.APZ"));
  InsQue(lx,Fs->last_lex);

  //Continues Here $LK,"/LT/OSMain/Adam1.APZ","FL:/LT/OSMain/Adam1.APZ,1"$
  try {
    Lex(lx);
    ExeCmdLine(lx);
    RemQue(lx);
    LexDel(lx);
  } catch {
    CatchAll;
    result=FALSE;
  }
  return result;
}

U0 OSMain()
{ //Continued from $LK,"OSMain","FF:::/LT/OSMain/OSStart.CPZ,I32 &OSMain"$
  adam_task=Fs;
  HeapsInit;
  LBts(&sys_run_level,RLf_FULL_HEAP);
  Fs->hash_table=HashTableNew(ADAM_HASH_SIZE,Fs);

#exe {StreamPrintF("SysDeterministic(%d);",osmain_cfg->opts[CFG_DETERMINISTIC]);};
  SysGlblsInit;

  Mem32BitDevInit;
  MemMarkPagesNotPresent;
  LoadOSMain;
  SysGrInit;
  StrCpy(Fs->task_name,"Adam Task CPU#0");
  StrCpy(Fs->task_title,Fs->task_name);
  Fs->text_attr=WHITE<<4+PURPLE;
  Fs->border_attr=WHITE<<4+LTPURPLE;
  Fs->win_right=text_cols-2;
  Fs->win_top++;
  TaskDerivedValsUpdate;

  LoadSysStrs;
  MPInit;
  IDTInit;

//Before this point use $LK,"Snd","MN:Snd"$() and $LK,"BusyWait","MN:BusyWait"$()
//to debug.  After this point, use $LK,"DbgNum","MN:DbgNum"$()
  LBts(&sys_run_level,RLf_RAW);

  if (BIOSTotalMem<250*0x100000)
    Dbg("Requires 256 Meg of RAM Memory");

  TimersInit;
  IntsInit;
  IntsInit; //Maybe we had slave IRQ pending, so do it again

  "Enable IRQ's\n";
  SetFlags(SYS_NORMAL_RFLAGS);
  BusyWait(2000);
  IDTInitComplete;
  LBts(&sys_run_level,RLf_INTERRUPTS);
  TimeStampFreqCal;
  if (!IsSysDeterministic)
    Preempt(ON);

#exe {
  if (!osmain_cfg->opts[CFG_NO_MP])
    StreamPutS("\"MultiCore Start\\n\\n\";"
	       "MPStart;");
};
  LBts(&sys_run_level,RLf_MP);

//  USBInitAll;

  KbdMouseVarsInit;
  IPVarsInit;
  KbdInit;

  if (IsSysDeterministic)
    MouseDriverInstall;
  else if (mp_cnt<2)
    Spawn(&MouseDriverInstall);
  else
    MPSpawn(&MouseDriverInstall);

  "BlkDevsInitAll\n";
  BlkDevsInitAll;
  "BlkDevsInitAll Done\n";
#exe {
  if (osmain_cfg->cdrom_ports_pmt)
    StreamPutS("if (boot_drv_pmt) PutDefine(\"ST_CDROM_PORTS_PMT\");");
};
  DrvDftSelect;
  HomeDirRst;
  LBts(&sys_run_level,RLf_DSK);

  "Loading Compiler\n";
  Cd("/LT/Compiler");
  Load("Compiler",LDF_SILENT);

  time_stamp_freq_initial=TimeStampFreqCal;
  InDbg(OFF);
  LBts(&sys_run_level,RLf_COMPILER);

  "Adam Start-Up Scripts...\n";
  if (!AdamStartUpScripts)
    Dbg("Adam Start-Up Error");

  LBts(&sys_run_level,RLf_ADAM_SERVER);
  ServantTaskCont; //Never to return
}

asm {	ALIGN	16,OC_NOP
SYS_OSMAIN_END::}
