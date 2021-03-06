asm {
USE32

SYS_BIOS32_SERVICE_DIR:: DU32	0;
SYS_PCI_SERVICES::	 DU32	0;

SYS_FIND_BIOS32_SERVICE_DIR::
	MOV	ESI,0xE0000
	MOV	ECX,(0x100000-0xE0000)/4
@@05:	CMP	U32 [ESI],'_32_'
	JNE	@@20
	PUSH	ECX
	XOR	ECX,ECX
	MOV	CL,U8 9[ESI]
	SHL	ECX,4
@@10:	MOV	EDI,ESI
	XOR	EAX,EAX
	XOR	EDX,EDX
@@15:	MOV	DL,U8 [EDI]
	ADD	EAX,EDX
	INC	EDI
	DEC	ECX
	JNZ	@@15
	POP	ECX
	OR	AL,AL
	JNZ	@@20
	MOV	U32 [SYS_BIOS32_SERVICE_DIR],ESI
	MOV	ESI,U32 4[ESI]
	MOV	U32 [SYS_BIOS32_SERVICE_CALL],ESI
	RET

@@20:	ADD	ESI,4
	LOOP	@@05
	MOV	U32 [SYS_BIOS32_SERVICE_DIR],0
	RET

SYS_FIND_PCI_SERVICES::
	MOV	ESI,U32 [SYS_BIOS32_SERVICE_DIR]
	OR	ESI,ESI
	JNZ	@@05
	MOV	U32 [SYS_PCI_SERVICES],0
	RET
@@05:	MOV	EAX,'$$PCI'
	XOR	EBX,EBX
	DU8	0x9A;		 //CALL GDT_CS_SEL:BIOS32_SERVICE
SYS_BIOS32_SERVICE_CALL:: DU32	0;
	DU16	GDT_CS_SEL;
	OR	AL,AL
	JNZ	@@05
	LEA	ESI,U32 [EBX+EDX]
	MOV	U32 [SYS_PCI_SERVICES],ESI
	RET

@@05:	MOV	U32 [SYS_PCI_SERVICES],0
	RET




USE64
C32_EAX::	DU32	0;
C32_EBX::	DU32	0;
C32_ECX::	DU32	0;
C32_EDX::	DU32	0;
C32_ESI::	DU32	0;
C32_EDI::	DU32	0;

C32_RSP::	DU64	0;

_FAR_CALL32::
//This calls a 32-bit mode routine.
//(We must switch from 64-bit mode to do it.)
//
//NON REENTRANT
//
	ENTER	0
	MOV	RAX,U64 SF_ARG1[RBP]
	OR	RAX,RAX
	JNZ	@@05
	LEAVE
	RET1	8	//return FALSE
@@05:	MOV	U32 [C32_ADD],EAX
	PUSH_REGS
	PUSHFD
	XOR	RAX,RAX
	PUSH	U64 FS:TaskStruct.addr[RAX]
	PUSH	U64 GS:CPUStruct.addr[RAX]
	MOV	U64 [C32_RSP],RSP
	PUSH	U32 GDT_ZERO_DS_SEL	//STKSEG
	PUSH	U32 0x9C000	//STK
	PUSH	U32 0		//FLAGS--interrupts off
	PUSH	U32 GDT_CS_SEL
	LEA	RAX,[@@15]
	PUSH	RAX
	IRET
USE32
@@15:
	WBINVD
//disable paging
	MOV_EAX_CR0
	BTR	EAX,31
	MOV_CR0_EAX

	MOV	ECX,IA32_EFER
	XOR	EDX,EDX
	XOR	EAX,EAX
	WRMSR

	MOV	AX,GDT_ZERO_DS_SEL
	MOV	FS,AX
	MOV	GS,AX
	//SS already set

	MOV	EAX,U32 [C32_EAX]
	MOV	EBX,U32 [C32_EBX]
	MOV	ECX,U32 [C32_ECX]
	MOV	EDX,U32 [C32_EDX]
	MOV	ESI,U32 [C32_ESI]
	MOV	EDI,U32 [C32_EDI]

	DU8	0x9A;		 //CALL GDT_CS_SEL:[C32_ADD]
C32_ADD:: DU32	0;
	DU16	GDT_CS_SEL;

	MOV	U32 [C32_EAX],EAX
	MOV	U32 [C32_EBX],EBX
	MOV	U32 [C32_ECX],ECX
	MOV	U32 [C32_EDX],EDX
	MOV	U32 [C32_ESI],ESI
	MOV	U32 [C32_EDI],EDI

	CALL	INIT_EM64T
USE64
	MOV	AX,GDT_ZERO_DS_SEL
	MOV	DS,AX
	MOV	ES,AX
	MOV	SS,AX

	MOV	RSP,U64 [C32_RSP]

//GS ALREADY ON STK
	CALL	_SET_GS_BASE
	POP	RAX
	CALL	SET_FS_BASE

	POPFD
	POP_REGS
	XOR	RAX,RAX
	MOV	AL,TRUE
	LEAVE
	RET1	8
}

LTextern C32_EAX U32 c32_eax;
LTextern C32_EBX U32 c32_ebx;
LTextern C32_ECX U32 c32_ecx;
LTextern C32_EDX U32 c32_edx;
LTextern C32_ESI U32 c32_esi;
LTextern C32_EDI U32 c32_edi;
LTextern SYS_PCI_SERVICES U32 sys_pci_services;

//Not reentrant--used to call BIOS32
//Takes like, 250uS.
LTextern _FAR_CALL32 BoolI64 FarCall32(U0 (*fp_add)());

U8 PCIReadU8(I64 bus,I64 dev,I64 fun,I64 rg)
{
  U64 result;
  PushFD;
  Cli;
  while (LBts(&sys_semas[SYS_SEMA_FAR_CALL32],0))
    Yield;
  c32_eax=0xB108;
  c32_ebx=bus<<8+dev<<3+fun;
  c32_edi=rg;
  if (FarCall32(sys_pci_services))
    result=c32_ecx.u8[0];
  else
    result=0xFF;
  LBtr(&sys_semas[SYS_SEMA_FAR_CALL32],0);
  PopFD;
  return result;
}

U16 PCIReadU16(I64 bus,I64 dev,I64 fun,I64 rg)
{
  U64 result;
  PushFD;
  Cli;
  while (LBts(&sys_semas[SYS_SEMA_FAR_CALL32],0))
    Yield;
  c32_eax=0xB109;
  c32_ebx=bus<<8+dev<<3+fun;
  c32_edi=rg;
  if (FarCall32(sys_pci_services))
    result=c32_ecx.u16[0];
  else
    result=0xFFFF;
  LBtr(&sys_semas[SYS_SEMA_FAR_CALL32],0);
  PopFD;
  return result;
}

U32 PCIReadU32(I64 bus,I64 dev,I64 fun,I64 rg)
{
  U64 result;
  PushFD;
  Cli;
  while (LBts(&sys_semas[SYS_SEMA_FAR_CALL32],0))
    Yield;
  c32_eax=0xB10A;
  c32_ebx=bus<<8+dev<<3+fun;
  c32_edi=rg;
  if (FarCall32(sys_pci_services))
      result=c32_ecx;
    else
      result=0xFFFFFFFF;
  LBtr(&sys_semas[SYS_SEMA_FAR_CALL32],0);
  PopFD;
  return result;
}

U0 PCIWriteU8(I64 bus,I64 dev,I64 fun,I64 rg,I64 val)
{
  PushFD;
  Cli;
  while (LBts(&sys_semas[SYS_SEMA_FAR_CALL32],0))
    Yield;
  c32_eax=0xB10B;
  c32_ebx=bus<<8+dev<<3+fun;
  c32_edi=rg;
  c32_ecx=val;
  FarCall32(sys_pci_services);
  LBtr(&sys_semas[SYS_SEMA_FAR_CALL32],0);
  PopFD;
}

U0 PCIWriteU16(I64 bus,I64 dev,I64 fun,I64 rg,I64 val)
{
  PushFD;
  Cli;
  while (LBts(&sys_semas[SYS_SEMA_FAR_CALL32],0))
    Yield;
  c32_eax=0xB10C;
  c32_ebx=bus<<8+dev<<3+fun;
  c32_edi=rg;
  c32_ecx=val;
  FarCall32(sys_pci_services);
  LBtr(&sys_semas[SYS_SEMA_FAR_CALL32],0);
  PopFD;
}

U0 PCIWriteU32(I64 bus,I64 dev,I64 fun,I64 rg,I64 val)
{
  PushFD;
  Cli;
  while (LBts(&sys_semas[SYS_SEMA_FAR_CALL32],0))
    Yield;
  c32_eax=0xB10D;
  c32_ebx=bus<<8+dev<<3+fun;
  c32_edi=rg;
  c32_ecx=val;
  FarCall32(sys_pci_services);
  LBtr(&sys_semas[SYS_SEMA_FAR_CALL32],0);
  PopFD;
}

I64 PCIClassFind(I64 class_code,I64 n)
{
//class_code is low three bytes
//n is index starting at zero
//Returns: -1 not found
//	  else bus,dev,fun.
  I64 result;
  PushFD;
  Cli;
  while (LBts(&sys_semas[SYS_SEMA_FAR_CALL32],0))
    Yield;
  c32_eax=0xB103;
  c32_esi=n;
  c32_ecx=class_code;
  if (FarCall32(sys_pci_services) && !c32_eax.u8[1])
    result=c32_ebx.u8[1]<<16+(c32_ebx&0xF8)<<5+c32_ebx&7;
  else
    result=-1;
  LBtr(&sys_semas[SYS_SEMA_FAR_CALL32],0);
  PopFD;
  return result;
}

BoolI64 BootCdProbe(LTBlkDev *bdev)
{
  BoolI8 result=FALSE;
  OSMainStruct *img=CAlloc(CD_BLK_SIZE);
  I64 i;
  "Port:%04X,%04XUnit:%02X",bdev->base0,bdev->base1,bdev->unit;
  if (ATAProbe(bdev->base0,bdev->base1,bdev->unit)==LTBDT_ATAPI) {
    //After preboot + BootCD.BIZ comes OSMain.BIC
    //See $LK,"CDBOOT","FA:::/LT/Adam/Dsk/DskCDDVD.APZ,CDBOOT"$, See $LK,"CD Boot Blk","FF:::/LT/Adam/Boot/BootCD.APZ,BOOT_CODE_CDROM"$
    "ATAPI";
    if (ATAPIStartStop(bdev,TRUE,BootTime+5*LTDATE_FREQ)) {
      "Started";
      for (i=0;i<2;i++) { //Retry
	if (ATAPIReadBlks2(bdev,img,sys_boot_code.u16[1],1,FALSE,BootTime+7*LTDATE_FREQ)) {
	  if (img->compile_time==sys_compile_time) {
	    "Found";
	    return TRUE;
	  } else
	    "Read";
	} else
	  "NoRead";
      }
    }
  }
  "Nope\n";
  Free(img);
  return result;
}

BoolI64 BootCdProbeAll(LTBlkDev *bdev)
{
  U32 d1,d2;
  bdev->base1=0;
  if (PCIReadU16(0,31,1,0)==0x8086) {
    d1=PCIReadU32(0,31,1,0x10);
    d2=PCIReadU32(0,31,1,0x14);
    if (d1&1 && d2&1) {
      if (bdev->base0=d1&-8) {
	bdev->unit=0;
	if (BootCdProbe(bdev))
	  return TRUE;
	bdev->unit=1;
	if (BootCdProbe(bdev))
	  return TRUE;
      }
    }
    d1=PCIReadU32(0,31,1,0x18);
    d2=PCIReadU32(0,31,1,0x1C);
    if (d1&1 && d2&1) {
      if (bdev->base0=d1&-8) {
	bdev->unit=0;
	if (BootCdProbe(bdev))
	  return TRUE;
	bdev->unit=1;
	if (BootCdProbe(bdev))
	  return TRUE;
      }
    }
  }
  if (PCIReadU16(0,31,2,0)==0x8086) {
    d1=PCIReadU32(0,31,2,0x10);
    d2=PCIReadU32(0,31,2,0x14);
    if (d1&1 && d2&1) {
      if (bdev->base0=d1&-8) {
	bdev->unit=0;
	if (BootCdProbe(bdev))
	  return TRUE;
	bdev->unit=1;
	if (BootCdProbe(bdev))
	  return TRUE;
      }
    }
    d1=PCIReadU32(0,31,2,0x18);
    d2=PCIReadU32(0,31,2,0x1C);
    if (d1&1 && d2&1) {
      if (bdev->base0=d1&-8) {
	bdev->unit=0;
	if (BootCdProbe(bdev))
	  return TRUE;
	bdev->unit=1;
	if (BootCdProbe(bdev))
	  return TRUE;
      }
    }
  }

  d1=0x1F0;
  d2=0x3F6;

  if (bdev->base0=d1&-8) {
    bdev->unit=0;
    if (BootCdProbe(bdev))
      return TRUE;
    bdev->unit=1;
    if (BootCdProbe(bdev))
      return TRUE;
  }

  d1=0x170;
  d2=0x376;

  if (bdev->base0=d1&-8) {
    bdev->unit=0;
    if (BootCdProbe(bdev))
      return TRUE;
    bdev->unit=1;
    if (BootCdProbe(bdev))
      return TRUE;
  }
}
