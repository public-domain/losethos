/*
LoseThos runs exclusively in ring 0.
Ring 0 is part of the $LK,"Constitution","FI:::/LT/Doc/Constitution.TXZ"$.
This demo is for you to play around
with ring 3.  LoseThos is for
recreational programming, after all.

This redirects the general protection
fault, switches to ring 3, and generates
a fault to switch back.
*/

U8 *old_stk,*new_rip;

asm {
	IMPORT	SYS_PROGRESS1,old_stk,new_rip;
TO_RING0::	//Set to handle general protection 0xD fault temporarily.
	INC	U64 [SYS_PROGRESS1]
	PUSH	U32 GDT_ZERO_DS_SEL	//STKSEG
	MOV	RAX,U64 [&old_stk]
	PUSH	RAX
	PUSH	U32 0		//FLAGS--interrupts off
	PUSH	U32 GDT_CS64_SEL
	MOV	RAX,U64 [&new_rip]
	PUSH	RAX
	IRET
}



U0 Ring3Demo()
{
  U8 *old_pf;
  "Progress1 Before:%X\n",progress1;
  Cli;
  old_pf=SetIDTEntry(0x0D,TO_RING0,0,IDTET_TRAP,0);

  TssBusy(Gs->tss->tr_ring3,OFF);
  SetRAX(Gs->tss->tr_ring3+3);
asm {
	LTR	AX
}


asm {
	IMPORT	SYS_PROGRESS1,old_stk,new_rip;

	MOV	U64 [&old_stk],RSP

	LEA	RAX,[R3_CALLBACK]
	MOV	U64 [&new_rip],RAX
	
	MOV	AX,GDT_ZERO_DS_RING3_SEL+3
	MOV	DS,AX
	MOV	ES,AX

	PUSH	U32 GDT_ZERO_DS_RING3_SEL+3	//STKSEG
	PUSH	U64 [&old_stk]
	PUSH	U32 0		//FLAGS--interrupts off
	PUSH	U32 GDT_CS64_RING3_SEL+3
	LEA	RAX,[R3_START]
	PUSH	RAX
	IRET

R3_START:
	INC	U64 [SYS_PROGRESS1]
	CLI	//This causes general protection fault #13

R3_CALLBACK:
	MOV	AX,GDT_ZERO_DS_SEL
	MOV	DS,AX
	MOV	ES,AX
  }

  TssBusy(Gs->tss->tr,OFF);
  SetRAX(Gs->tss->tr);
asm {
	LTR	AX
}

  SetIDTEntry(0x0D,old_pf,0,IDTET_IRQ,0);
  Sti;
  "Progress1 After :%X\n",progress1;
}


Ring3Demo;
