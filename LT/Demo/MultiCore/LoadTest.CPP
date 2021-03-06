I64 load_test_done;

U0 Job1(U64 dummy=0)
{ //Compile Compiler
  nounusedwarn dummy;
  TaskLocalInit; //Enable file access
  Preempt(ON);	 //Off by default
  Silent(ON);
  while (!load_test_done) {
    Cd("::/LT/Compiler");
    Cmp("Compiler");
  }
  lock {load_test_done--;}
}

U0 Job2(U64 dummy=0)
{ //Log a file
  nounusedwarn dummy;
  TaskLocalInit; //Enable file access
  Preempt(ON);	 //Off by default
  while (!load_test_done) {
    AdamLog("%F","::/LT/Demo/MultiCore/LoadTest.CPZ");
    Yield;
  }
  lock {load_test_done--;}
}

U0 Job3(U64 dummy=0)
{ //Log a file
  nounusedwarn dummy;
  TaskLocalInit; //Enable file access
  Preempt(ON);	 //Off by default
  while (!load_test_done) {
    Cd("::/LT/Compiler");
    AdamLog("%F","Compiler.SPZ");
    Yield;
  }
  lock {load_test_done--;}
}

U0 Main()
{
  load_test_done=0;
  "Running MultiCore Test...\n";
  MPSpawn(&Job1);
  MPSpawn(&Job2);
  MPSpawn(&Job1);
  MPSpawn(&Job3);
  Spawn(&Job1);
  Spawn(&Job2);
  Spawn(&Job1);
  Spawn(&Job3);
  PressAKey;
  load_test_done=8;
//We don't want to yank code from under MP cores.
  while (load_test_done)
    Yield;
}

Main;
