asm {
USE64
// ************************************
_MEMCMP::
	ENTER	0
	PUSH	RSI
	PUSH	RDI
	MOV	RSI,U64 SF_ARG1[RBP]
	MOV	RDI,U64 SF_ARG2[RBP]
	MOV	RCX,U64 SF_ARG3[RBP]
	XOR	RAX,RAX
	REPE_CMPSB
	JZ	@@05
	MOV	AL,1
	JA	@@05
	MOV	RAX,-1
@@05:	POP	RDI
	POP	RSI
	LEAVE
	RET1	24
// ************************************
_CALL::
	ENTER	0
	MOV	RAX,U64 SF_ARG1[RBP]
	OR	RAX,RAX
	JZ	@@05
	CALL	RAX
@@05:	LEAVE
	RET1	8
// ************************************
_BIT_FIELD_OR_U32::
	ENTER	0
	MOV	RBX,U64 SF_ARG2[RBP]
	SHR	RBX,3
	ADD	RBX,U64 SF_ARG1[RBP]
	MOV	RAX,U64 SF_ARG3[RBP]
	MOV	RCX,U64 SF_ARG2[RBP]
	AND	CL,7
	SHL	RAX,CL
	OR	U64 [RBX],RAX
	LEAVE
	RET1	24
// ************************************
_BIT_FIELD_EXTRACT_U32::
	ENTER	0
	MOV	RBX,U64 SF_ARG2[RBP]
	MOV	RCX,RBX
	SHR	RBX,3
	ADD	RBX,U64 SF_ARG1[RBP]
	MOV	RDX,U64 [RBX]
	AND	CL,7
	SHR	RDX,CL
	MOV	RCX,U64 SF_ARG3[RBP]
	MOV	RAX,1
	SHL	RAX,CL
	DEC	RAX
	AND	RAX,RDX
	LEAVE
	RET1	24
// ************************************
_XCHG_U64::
	ENTER	0
	MOV	RDX,U64 SF_ARG1[RBP]
	MOV	RAX,U64 SF_ARG2[RBP]
	XCHG	U64 [RDX],RAX
	LEAVE
	RET1	16
_XCHG_U32::
	ENTER	0
	MOV	RDX,U64 SF_ARG1[RBP]
	XOR	RAX,RAX
	MOV	EAX,U32 SF_ARG2[RBP]
	XCHG	U32 [RDX],EAX
	LEAVE
	RET1	16
_XCHG_U16::
	ENTER	0
	MOV	RDX,U64 SF_ARG1[RBP]
	XOR	RAX,RAX
	MOV	AX,U16 SF_ARG2[RBP]
	XCHG	U16 [RDX],AX
	LEAVE
	RET1	16
_XCHG_U8::
	ENTER	0
	MOV	RDX,U64 SF_ARG1[RBP]
	XOR	RAX,RAX
	MOV	AL,U8 SF_ARG2[RBP]
	XCHG	U8 [RDX],AL
	LEAVE
	RET1	16

_LXCHG_U64::
	ENTER	0
	MOV	RDX,U64 SF_ARG1[RBP]
	MOV	RAX,U64 SF_ARG2[RBP]
	LOCK
	XCHG	U64 [RDX],RAX
	LEAVE
	RET1	16
_LXCHG_U32::
	ENTER	0
	MOV	RDX,U64 SF_ARG1[RBP]
	XOR	RAX,RAX
	MOV	EAX,U32 SF_ARG2[RBP]
	LOCK
	XCHG	U32 [RDX],EAX
	LEAVE
	RET1	16
_LXCHG_U16::
	ENTER	0
	MOV	RDX,U64 SF_ARG1[RBP]
	XOR	RAX,RAX
	MOV	AX,U16 SF_ARG2[RBP]
	LOCK
	XCHG	U16 [RDX],AX
	LEAVE
	RET1	16
_LXCHG_U8::
	ENTER	0
	MOV	RDX,U64 SF_ARG1[RBP]
	XOR	RAX,RAX
	MOV	AL,U8 SF_ARG2[RBP]
	LOCK
	XCHG	U8 [RDX],AL
	LEAVE
	RET1	16
// ************************************
_REP_IN_U32::
	ENTER	0
	PUSH	RDI
	MOV	RDI,U64 SF_ARG1[RBP]
	MOV	RDX,U64 SF_ARG3[RBP]
	MOV	RCX,U64 SF_ARG2[RBP]
	REP_INSD
	POP	RDI
	LEAVE
	RET1	24

_REP_IN_U16::
	ENTER	0
	PUSH	RDI
	MOV	RDI,U64 SF_ARG1[RBP]
	MOV	RDX,U64 SF_ARG3[RBP]
	MOV	RCX,U64 SF_ARG2[RBP]
	REP_INSW
	POP	RDI
	LEAVE
	RET1	24

_REP_IN_U8::
	ENTER	0
	PUSH	RDI
	MOV	RDI,U64 SF_ARG1[RBP]
	MOV	RDX,U64 SF_ARG3[RBP]
	MOV	RCX,U64 SF_ARG2[RBP]
	REP_INSB
	POP	RDI
	LEAVE
	RET1	24

_REP_OUT_U32::
	ENTER	0
	PUSH	RSI
	MOV	RSI,U64 SF_ARG1[RBP]
	MOV	RDX,U64 SF_ARG3[RBP]
	MOV	RCX,U64 SF_ARG2[RBP]
	REP_OUTSD
	POP	RSI
	LEAVE
	RET1	24

_REP_OUT_U16::
	ENTER	0
	PUSH	RSI
	MOV	RSI,U64 SF_ARG1[RBP]
	MOV	RDX,U64 SF_ARG3[RBP]
	MOV	RCX,U64 SF_ARG2[RBP]
	REP_OUTSW
	POP	RSI
	LEAVE
	RET1	24

_REP_OUT_U8::
	ENTER	0
	PUSH	RSI
	MOV	RSI,U64 SF_ARG1[RBP]
	MOV	RDX,U64 SF_ARG3[RBP]
	MOV	RCX,U64 SF_ARG2[RBP]
	REP_OUTSB
	POP	RSI
	LEAVE
	RET1	24
// ************************************
_CPUID::
	ENTER	0
	PUSH	RSI
	MOV	RAX,U64 SF_ARG1[RBP]
	CPUID
	MOV	RSI,U64 SF_ARG2[RBP]
	MOV	U64 [RSI],RAX
	MOV	U64 8[RSI],RBX
	MOV	U64 16[RSI],RCX
	MOV	U64 24[RSI],RDX
	POP	RSI
	LEAVE
	RET1	16
// ************************************
_CALLIND::
	ENTER	0
	PUSH	RSI
	PUSH	RDI
	MOV	RDX,U64 SF_ARG1[RBP]
	MOV	RCX,U64 SF_ARG2[RBP]	//argc
	LEA	RSI,U64 SF_ARG3[RBP]	//argv
	SHL	RCX,3
	SUB	RSP,RCX
	MOV	RDI,RSP
	REP_MOVSB
@@05:	OR	RDX,RDX
	JZ	@@15
	CALL	RDX
@@10:	POP	RDI
	POP	RSI
	LEAVE
	RET

@@15:	XOR	RAX,RAX
	MOV	RCX,U64 SF_ARG2[RBP]	//argc
	SHL	RCX,3
	ADD	RSP,RCX
	POP	RDI
	POP	RSI
	LEAVE
	RET
// ************************************
_CALLEXTNUM::
	ENTER	0
	PUSH	RSI
	PUSH	RDI
	MOV	RDX,U64 SF_ARG1[RBP]
	MOV	RCX,U64 SF_ARG2[RBP]	//argc
	LEA	RSI,U64 SF_ARG3[RBP]	//argv
	SHL	RCX,3
	SUB	RSP,RCX
	MOV	RDI,RSP
	REP_MOVSB
@@05:	MOV	RAX,U64 [SYS_EXTERN_TABLE]
	MOV	RAX,U64 [RAX+RDX*8]
	OR	RAX,RAX
	JZ	@@15
	CALL	RAX
@@10:	POP	RDI
	POP	RSI
	LEAVE
	RET

@@15:	XOR	RAX,RAX
	MOV	RCX,U64 SF_ARG2[RBP]	//argc
	SHL	RCX,3
	ADD	RSP,RCX
	POP	RDI
	POP	RSI
	LEAVE
	RET
// ************************************
_CALLEXTSTR::
	ENTER	0
	PUSH	RSI
	PUSH	RDI
	MOV	RCX,U64 SF_ARG2[RBP]	//argc
	LEA	RSI,U64 SF_ARG3[RBP]	//argv
	SHL	RCX,3
	SUB	RSP,RCX
	MOV	RDI,RSP
	REP_MOVSB
@@05:	MOV	RSI,U64 SF_ARG1[RBP]
	CALL	SYS_EXT_FIND1
	OR	RAX,RAX
	JZ	@@15
	CALL	RAX
@@10:	POP	RDI
	POP	RSI
	LEAVE
	RET

@@15:	MOV	RCX,U64 SF_ARG2[RBP]	//argc
	SHL	RCX,3
	ADD	RSP,RCX
	POP	RDI
	POP	RSI
	LEAVE
	RET
// ************************************
_SET_GS_BASE::
	ENTER	0
	MOV	RAX,SF_ARG1[RBP]
	PUSH	RAX
	PUSH	RCX
	PUSH	RDX
	MOV	RDX,RAX
	SHR	RDX,32
	MOV	ECX,IA32_GS_BASE
	WRMSR
	POP	RDX
	POP	RCX
	POP	RAX
	LEAVE
	RET1	8

SET_FS_BASE::
	PUSH	RAX
	PUSH	RCX
	PUSH	RDX
	MOV	RDX,RAX
	SHR	RDX,32
	MOV	ECX,IA32_FS_BASE
	WRMSR
	POP	RDX
	POP	RCX
	POP	RAX
	RET
_SET_MSR::
	ENTER	0
	MOV	RAX,SF_ARG2[RBP]
	MOV	RDX,RAX
	SHR	RDX,32
	MOV	RCX,SF_ARG1[RBP]
	WRMSR
	LEAVE
	RET1	16
_SYS_HLT::
	CLI
@@05:	HLT
	JMP	@@05
}
