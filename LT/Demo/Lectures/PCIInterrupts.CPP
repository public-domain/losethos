//See $LK,"PCI","HI:PCI"$.

#if mp_cnt>1
#define DEST_CPU	1
#else
#define DEST_CPU	0
#endif

#assert I_USER==0x40

interrupt U0 MyVect40()
{
  lock {progress1++;}
  IntEOI;
}

interrupt U0 MyVect41()
{
  lock {progress2++;}
  IntEOI;
}

interrupt U0 MyVect42()
{
  lock {progress3++;}
  IntEOI;
}

interrupt U0 MyVect43()
{
  lock {progress4++;}
  IntEOI;
}

U64 vect[4]={&MyVect40,&MyVect41,&MyVect42,&MyVect43};

U0 Main()
{
  I64 i;
  U8 *da=IOAPIC_REG;
  U32 *_d=IOAPIC_DATA;

  ProgressBarsRst;
  for (i=I_USER;i<I_USER+4;i++)
    SetIDTEntry(i,vect[i&3],DEST_CPU,IDTET_IRQ);

//There are like 4*6 vects.  Bus zero
//has the first four vects.
  for (i=IOREDTAB;i<IOREDTAB+4*2;i+=2) {
    *da=i+1;
    *_d=mp_apic_ids[DEST_CPU]<<24;
    *da=i;
    *_d=0x4000+0x40+(i-IOREDTAB)/2;
  }

  Sleep(1000);
  "The keyboard and timer seem to be active.\n"
  "IntA:%d IntB:%d IntC:%d IntD:%d\n\n"
  "IntB is my keybaord and registers a single KEY-UP.\n"
  "Rerun and press a few keys.\n",
  progress1,progress2,progress3,progress4;

  //Disable vects
  for (i=IOREDTAB;i<IOREDTAB+4*2;i+=2) {
    *da=i+1;
    *_d=mp_apic_ids[DEST_CPU]<<24;
    *da=i;
    *_d=0x14000+0x40+(i-IOREDTAB)/2;
  }
  ProgressBarsRst;
}

Main;
