//This task lets you hear how busy the CPU is.
U0 LoadTask(U64 dummy=0)
{
  nounusedwarn dummy;
  I64 i,j=100;
//Preemption is ctrlled on a per task basis.
//Preemption is OFF by default on new tasks.
//It is turned on for user terminal tasks.
  Preempt(ON);
  while (TRUE) {
    for (i=0;i<10000;i++)
      InU8(0x21); //Roughly 1 microsecond on all PC's
    Snd(j);
    j+=20;
    if (j>1000) j=100;
  }
}


U0 SpinningPreempt()
{
  I64 i;
  "Spinning with preemption....\n";
  for (i=0;i<3000000;i++)
    InU8(0x21); //This is a sample hardware stat read
}

U0 SpinningNoPreempt()
{
  I64 i;
  BoolI8 old_preempt;
  "Spinning with no preemption...\n";

//Put on screen before it starts.  Text lags by one refresh
//and we might catch it in the middle of an update.
  WinSync;
  WinSync;
  WinSync;

  old_preempt=Preempt(OFF);
  for (i=0;i<3000000;i++)
    InU8(0x21);
  Preempt(old_preempt);
}

U0 SpinningNoInterrupts()
{
  I64 i;
//Interrupts are ctrlled on a per task basis.
  "Spinning with no interrupts...\n";
  WinSync;
  WinSync;
  WinSync;
  Cli;
  for (i=0;i<3000000;i++)
    InU8(0x21);
  Sti;
}


U0 CooperativeMultitaskingPreempt()
{
  F64 timeout=tNP+3.0;

  "Cooperative multitasking with preemption...\n";
  while (tNP<timeout) {
    InU8(0x21);
    Yield;
  }
}

U0 CooperativeMultitaskingNoPreempt()
{
  F64 timeout=tNP+3.0;
  BoolI8 old_preempt=Preempt(OFF);
  "Cooperative multitasking with no preemption...\n";
  while (tNP<timeout) {
    InU8(0x21);
    Yield;
  }
  Preempt(old_preempt);
}

U0 CooperativeMultitaskingNoInterrupts()
{
  F64 timeout=tNP+3.0;

  Cli;
  "Cooperative multitasking with no interrupts...\n";
  while (tNP<timeout) {
    InU8(0x21);
    Yield;
  }
  Sti;
}

U0 SpinningDemo()
{
  TaskStruct *task=Spawn(&LoadTask);
  SpinningPreempt;
  SpinningNoPreempt;
  SpinningNoInterrupts;
  CooperativeMultitaskingPreempt;
  CooperativeMultitaskingNoPreempt;
  CooperativeMultitaskingNoInterrupts;
  Kill(task);
  Snd(0);
}

SpinningDemo;
