/*
When I was a kid with a Commodore 64,
the 6502 chip had no multiply instruction
and this is how we had to do it, except,
I used more regs in this example.

I'll leave it as an exercise to do
division by hand.
*/
asm {
//Opcodes are slightly different to make writing my x86_64 assembler easier.
//See $LK,"::/LT/Compiler/OpCodes.TXZ","FI:::/LT/Compiler/OpCodes.TXZ"$.

USE64	//Defaults to 64, but this is a good habit.

//You can clobber RAX,RBX,RCX,RDX,R8,R9

MUL_BY_HAND_U8_U8_TO_U16: //This is only for fun.
//8bit * 8bit-->16bit
//AL*BL-->AX
	MOV	CL,8
	SHL	AX,8
@@05:	SHL1	AX
	JNC	@@10
	ADD	AL,BL
@@10:	DEC	CL
	JNZ	@@05
	RET

_MUL_BY_HAND_U8_U8_TO_U16::	//C callable
	ENTER	0
	MOV	AL,U8 SF_ARG1[RBP] //$LK,"SF_ARG1","FF:::/LT/OSMain/Adam1a.HPZ,SF_ARG1"$
	MOV	BL,U8 SF_ARG2[RBP]
	CALL	MUL_BY_HAND_U8_U8_TO_U16
	MOVZX	RAX,AX
	LEAVE
	RET1	16

_MUL_U64_U64_TO_U128::
//64bit * 64bit-->128bit
	ENTER	0
	MOV	RBX,U64 SF_ARG3[RBP]
	MOV	RAX,U64 SF_ARG1[RBP] //$LK,"SF_ARG1","FF:::/LT/OSMain/Adam1a.HPZ,SF_ARG1"$
	MUL	U64 SF_ARG2[RBP]	//Result RDX:RAX 128bit
	MOV	U64 [RBX],RAX
	MOV	U64 8[RBX],RDX
	LEAVE
	RET1	24
};

//My convention is to put an underscore
//on C callable routines.
LTextern _MUL_BY_HAND_U8_U8_TO_U16 U16 MulU8(U8 n1,U8 n2);

class U128
{
  U64 lo,hi;
};

LTextern _MUL_U64_U64_TO_U128 U0 MulU64(U64 n1,U64 n2,U128 *_prod);

U0 MulByHand()
{
  U128 p;
  "2*7=0x%X\n",MulU8(2,7);
  "100*10=0x%X\n",MulU8(100,10);

  MulU64(0x0123456789ABCDEF,0x1000001,&p);
  "0x0123466789ABCDEF*0x1000001=0x%016X%016X\n",p.hi,p.lo;
}

MulByHand;

