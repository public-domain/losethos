/*
This is a JIT file, not static.

Asm labels can only be defined once
in a task.Hit F5 to spawn a new task
and run it.
*/

asm {
//Opcodes are slightly different to make writing my x86_64 assembler easier.
//See $LK,"::/LT/Compiler/OpCodes.TXZ","FI:::/LT/Compiler/OpCodes.TXZ"$.
USE64	//Defaults to 64, but this is a good habit.
	IMPORT	Beep;

_BEEPS::
//You can clobber RAX,RBX,RCX,RDX,R8,R9 in a pure asm subroutine.
	ENTER	0
	MOV	RCX,U64 SF_ARG1[RBP]  //$LK,"SF_ARG1","FF:::/LT/OSMain/Adam1a.HPZ,SF_ARG1"$

@@05:	PUSH	RCX
//U0 Beep(U64 freq=500,BoolI8 busy=FALSE)
	PUSH	FALSE	//Do not busy (spin) wait
	PUSH	500	//Hz
	CALL	Beep
	POP	RCX
	LOOP	@@05

	LEAVE
//Return vals are in RAX if you have one.
	RET1	8
}


//LTextern binds a asm sym to a function.
//My convention is to put an underscore
//on C callable routines.
LTextern _BEEPS U0 Beeps(U64 cnt);

I64 AsmAndC()
{
  I64 noreg i;	//Normally this would be stored in a reg
		//Check with $LK,"Uf","MN:Uf"$("AsmAndC").

  i=PmtI64("Num of beeps 1-5 (%d):",3,1,5);
  Beeps(i);

  asm {
//You can clobber RAX,RBX,RCX,RDX, but preserve the rest for code in a C+ routine.
USE64	//Defaults to 64, but this is a good habit.
	IMPORT	Snd;
	MOV	RCX,&i[RBP] //You can clobber RAX,RBX,RCX,RDX.
			    //You better preserve the rest.

@@05:	PUSH	RCX
//U0 Snd(F64 freq,U64 waveform=WF_SQUARE,F64 amp=1.0);
	MOV	RAX,1.0	//amplitude
	PUSH	RAX
	PUSH	WF_SQUARE

	MOV	RAX,RCX	//freq=loop*100.0+100.0
	IMUL2	RAX,100 //LoseThos uses nonstandard opcodes
			//to avoid multiple form of the same one.
			//See $LK,"::/LT/Compiler/OpCodes.TXZ","FI:::/LT/Compiler/OpCodes.TXZ"$.
			//$BK,1$Asm is kind-of a bonus -- there's no reason$BK,0$
			//$BK,1$to use it, but I need it for the kernel.$BK,0$
	ADD	RAX,100
	PUSH	RAX
	FILD	U64 [RSP]	//Convert from int to floating
	FSTP	U64 [RSP]

	CALL	Snd

	MOV	RCX,0x20000000	//Simple delay loop
@@10:	LOOP	@@10

	POP	RCX
	LOOP	@@05
  }
  Snd(0);
  return i;
}

"Beeps:%d\n",AsmAndC;
