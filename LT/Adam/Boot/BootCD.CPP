asm {
#define MODULE_SIZE		CD_BOOT_LOADER_SIZE
#define BOOT_HIGH_LOC	((BOOT_RAM_LIMIT-(BOOT_STK_SIZE+MODULE_SIZE))>>4)
USE16
BCD_START::
//DL is supposed to have the BIOS drive number
	CLD
	MOV	AX,BOOT_HIGH_LOC
	MOV	ES,AX

	CLI
	MOV	SS,AX
	MOV	SP,BOOT_STK_SIZE+MODULE_SIZE
	STI

	CALL	BCD_GET_IP
BCD_GET_IP:
	POP	BX
	SUB	BX,BCD_GET_IP-BCD_START
	SHR	BX,4
//Copy this bootloader code to just under 0x80000
	MOV	AX,CS
	ADD	AX,BX
	MOV	DS,AX
	MOV	CX,MODULE_SIZE
	XOR	SI,SI
	XOR	DI,DI
	REP_MOVSB

	MOV	AX,BOOT_HIGH_LOC
	MOV	DS,AX

//My assembler doesn't support 16-bit very well.
	DU8	0xEA;	//JMP BOOT_HIGH_LOC:BCD_MAIN
	DU16	BCD_MAIN-BCD_START,BOOT_HIGH_LOC;

BCD_BIOS_DRV_NUM:	DU8	0;
BCD_PAGE:		DU8	0;

BCD_DAP:		DU8	16,0,1,0; //One blk at a time
BCD_DAP_BUF:		DU16	0,0;
BCD_DAP_BLK:		DU64	0;
	
BCD_LOSETHOS_MSG:
	DU8	"\n\r\n\r\n\r\n\r\n\r"
"*********************************\n\r"
"***\n\r"
"*******************\n\r"
"************\n\r"
"************\n\r"
"******************\n\r"
"************\n\r"
"***********\n\r"
"*******************\n\r"
"*\n\r"
"***********************************\n\r"
"\n\n\n\n\nLoadingLoseThos",0;

BCD_NOT64_MSG:
	DU8	"LoseThos requires a 64-bit capable processor.\n\r",0;

//These get patched by $LK,"CDBootFileCreate","MN:CDBootFileCreate"$
BCD_BLK_LO::		DU16	0;
BCD_BLK_HI::		DU16	0;
BCD_BLK_CNT::		DU16	0;

BCD_PROGRESS_STEP::	DU32	0;
BCD_PROGRESS_VAL::	DU32	0;

BCD_PUTCHAR::
	MOV	AH,0xE
	MOV	BL,7 //Might be foreground color on some BIOS's
	MOV	BH,U8 [BCD_PAGE-BCD_START]
	INT	0x10
BCD_RET::
	RET
BCD_PUTS::
@@1:	LODSB
	OR	AL,AL
	JZ	BCD_RET
	CALL	BCD_PUTCHAR
	JMP	@@1

BCD_MAIN::
	MOV	U8 [BCD_BIOS_DRV_NUM-BCD_START],DL //Passed in by BIOS

	MOV	AH,0xF
	INT	0x10
	MOV	U8 [BCD_PAGE-BCD_START],BH //video page

	MOV	EAX,0x80000000
	CPUID
	CMP	EAX,0x80000001
	JB	@@05

	MOV	EAX,0x80000001
	CPUID
	BT	EDX,29
	JC	@@15
@@05:	MOV	SI,BCD_NOT64_MSG-BCD_START
	CALL	BCD_PUTS
@@10:	JMP	@@10

@@15:	MOV	SI,BCD_LOSETHOS_MSG-BCD_START
	CALL	BCD_PUTS

	MOV	AX,BOOT_RAM_BASE/16
	MOV	ES,AX
	XOR	ECX,ECX
	MOV	CX,U16 [BCD_BLK_CNT-BCD_START]

	MOV	EAX,(80-7-9)*65536	//80 columns
	XOR	EDX,EDX
	DIV	ECX
	MOV	U32 [BCD_PROGRESS_STEP-BCD_START],EAX
	MOV	U32 [BCD_PROGRESS_VAL-BCD_START],0

	MOV	AX,U16 [BCD_BLK_LO-BCD_START]
	MOV	DX,U16 [BCD_BLK_HI-BCD_START]

@@20:	PUSH	CX	//Blk cnt

//READ BLK
	PUSH	AX	//Blk lo
	PUSH	DX	//Blk hi
	PUSH	ES	//Buf seg
	MOV	U16 [BCD_DAP_BLK-BCD_START],AX
	MOV	U16 [BCD_DAP_BLK+2-BCD_START],DX
	MOV	AX,ES
	MOV	U16 [BCD_DAP_BUF+2-BCD_START],AX //ES:0000
	MOV	SI,BCD_DAP-BCD_START //DS:SI=DAP
	MOV	AH,0x42
	MOV	DL,U8 [BCD_BIOS_DRV_NUM-BCD_START]
	INT	0x13

	POP	AX	//ES
	ADD	AX,CD_BLK_SIZE/16
	MOV	ES,AX
	POP	DX
	POP	AX
	INC	AX
	JNZ	@@25
	INC	DX

@@25:	PUSH	AX
	MOV	BX,U16 [BCD_PROGRESS_VAL+2-BCD_START]
	MOV	EAX,U32 [BCD_PROGRESS_STEP-BCD_START]
	ADD	U32 [BCD_PROGRESS_VAL-BCD_START],EAX
	CMP	U16 [BCD_PROGRESS_VAL+2-BCD_START],BX
	JE	@@30
	MOV	AL,'.'
	CALL	BCD_PUTCHAR
@@30:	POP	AX

	POP	CX
	LOOP	@@20

//See $LK,"BootCdProbe","MN:BootCdProbe"$().
	MOV	EBX,U32 [BCD_BLK_LO-BCD_START]
	SHL	EBX,16
	MOV	BX,BOOT_CODE_CDROM  //$MA+A-X+PU,"See sys_boot_code","Grep(\"sys_boot_code\",\"/LT/\"TEXT_FILE_MASK);View;\n"$

//My assembler doesn't support 16-bit very well.
	DU8	0xEA;	//JMP BOOT_RAM_BASE:0000
	DU16	0,BOOT_RAM_BASE/16;
//Continues here $LK,"::/LT/OSMain/OSStart.CPZ","FL:::/LT/OSMain/OSStart.CPZ,1"$
BCD_END::
#assert BCD_END-BCD_START<MODULE_SIZE
}
