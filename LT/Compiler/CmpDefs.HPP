//Intermediate compiler codes
#define EC_END			0x00
#define EC_START		0x01
#define EC_NOP			0x02
#define EC_END_EXP		0x03
#define EC_NOP1			0x04
#define EC_LABEL		0x05
#define EC_CALL_START		0x06
#define EC_CALL_END		0x07
#define EC_CALL_END2		0x08
#define EC_RETURN_VAL		0x09
#define EC_RETURN_VAL2		0x0A
#define EC_IMM			0x0B
#define EC_F64			0x0C
#define EC_STR_CONST		0x0D
#define EC_ABS_ADDR	0x0E
#define EC_ADDR_IMPORT	0x0F
#define EC_HEAP_GLBL		0x10
#define EC_SIZEOF		0x11
#define EC_TYPE			0x12
#define EC_GET_LABEL		0x13
#define EC_RBP			0x14
#define EC_REG			0x15
#define EC_FS			0x16
#define EC_GS			0x17

#define EC_LEA			0x18
#define EC_MOV			0x19

#define EC_TO_I64		0x1A
#define EC_TO_F64		0x1B
#define EC_TO_BOOL		0x1C
#define EC_TOUPPER		0x1D
#define EC_LT_TYPECAST		0x1E

#define EC_ADDR		0x1F
#define EC_COM			0x20
#define EC_NOT			0x21
#define EC_UNARY_MINUS		0x22

#define EC_DEREF		0x23
#define EC_DEREF_PP		0x24
#define EC_DEREF_MM		0x25
#define EC__PP			0x26
#define EC__MM			0x27
#define EC_PP_			0x28
#define EC_MM_			0x29

#define EC_SHL			0x2A
#define EC_SHR			0x2B
#define EC_SHL_CONST		0x2C
#define EC_SHR_CONST		0x2D
#define EC_POWER		0x2E

#define EC_MUL			0x2F
#define EC_DIV			0x30
#define EC_MOD			0x31

#define EC_AND			0x32

#define EC_OR			0x33
#define EC_XOR			0x34

#define EC_ADD			0x35
#define EC_SUB			0x36
#define EC_ADD_CONST		0x37
#define EC_SUB_CONST		0x38

#define EC_EQUAL_EQUAL		0x39
#define EC_NOT_EQUAL		0x3A
#define EC_LESS			0x3B
#define EC_GREATER		0x3C
#define EC_LESS_EQUAL		0x3D
#define EC_GREATER_EQUAL	0x3E
#define EC_PUSH_CMP		0x3F

#define EC_AND_AND		0x40

#define EC_OR_OR		0x41
#define EC_XOR_XOR		0x42

#define EC_COLON		0x43
#define EC_QUESTION		0x44

#define EC_ASSIGN		0x45
#define EC_ASSIGN_PP		0x46
#define EC_ASSIGN_MM		0x47

#define EC_SHL_EQUAL		0x48
#define EC_SHR_EQUAL		0x49
#define EC_MUL_EQUAL		0x4A
#define EC_DIV_EQUAL		0x4B
#define EC_MOD_EQUAL		0x4C
#define EC_AND_EQUAL		0x4D
#define EC_OR_EQUAL		0x4E
#define EC_XOR_EQUAL		0x4F
#define EC_ADD_EQUAL		0x50
#define EC_SUB_EQUAL		0x51

#define EC_JMP			0x52
#define EC_SWITCH		0x53
#define EC_NOBOUND_SWITCH	0x54

#define EC_ADD_RSP		0x55
#define EC_ADD_RSP1		0x56
#define EC_ENTER		0x57
#define EC_PUSH_REGS		0x58
#define EC_POP_REGS		0x59
#define EC_LEAVE		0x5A
#define EC_RET			0x5B

#define EC_CALL			0x5C
#define EC_CALL_INDIRECT	0x5D
#define EC_CALL_INDIRECT2	0x5E
#define EC_CALL_IMPORT		0x5F
#define EC_CALL_EXTERN		0x60
#define EC_IRQ			0x61

#define EC_ASM			0x62
#define EC_CLI			0x63
#define EC_STI			0x64
#define EC_PREEMPT		0x65
#define EC_CLD			0x66
#define EC_STD			0x67
#define EC_HLT			0x68
#define EC_PUSH			0x69
#define EC_POP			0x6A
#define EC_PUSHFD		0x6B
#define EC_POPFD		0x6C
#define EC_PAUSE		0x6D

#define EC_CLFLUSH		0x6E
#define EC_INVLPG		0x6F
//6E,6F unused
#define EC_WBINVD		0x70
#define EC_INVD			0x71

#define EC_INU8			0x72
#define EC_INU16		0x73
#define EC_INU32		0x74
#define EC_OUTU8		0x75
#define EC_OUTU16		0x76
#define EC_OUTU32		0x77

#define EC_GET_FLAGS		0x78
#define EC_SET_FLAGS		0x79
#define EC_GET_RAX		0x7A
#define EC_SET_RAX		0x7B
#define EC_GET_BASE_PTR		0x7C
#define EC_SET_BASE_PTR		0x7D
#define EC_GET_STK_PTR		0x7E
#define EC_SET_STK_PTR		0x7F
#define EC_RIP			0x80

#define EC_RDTSC		0x81
#define EC_HPET			0x82

#define EC_BT			0x83
#define EC_BTS			0x84
#define EC_BTR			0x85
#define EC_BTC			0x86
#define EC_BEQU			0x87
#define EC_LBTS			0x88
#define EC_LBTR			0x89
#define EC_LBTC			0x8A
#define EC_LBEQU		0x8B
#define EC_BSF			0x8C
#define EC_BSR			0x8D

#define EC_INSQUE		0x8E
#define EC_INSQUE_REV		0x8F
#define EC_REMQUE		0x90

#define EC_STRLEN		0x91
#define EC_MEMCPY		0x92

#define EC_MEMSET		0x93
#define EC_MEMSET_U16		0x94
#define EC_MEMSET_U32		0x95
#define EC_MEMSET_U64		0x96

#define EC_BR_ZERO		0x97
#define EC_BR_NOT_ZERO		0x98

#define EC_BR_NOT_EQUAL		0x99
#define EC_BR_EQUAL_EQUAL	0x9A
#define EC_BR_GREATER_EQUAL	0x9B
#define EC_BR_LESS_EQUAL	0x9C
#define EC_BR_GREATER		0x9D
#define EC_BR_LESS		0x9E
#define EC_BR_NOT_EQUAL2	0x9F
#define EC_BR_EQUAL_EQUAL2	0xA0
#define EC_BR_GREATER_EQUAL2	0xA1
#define EC_BR_LESS_EQUAL2	0xA2
#define EC_BR_GREATER2		0xA3
#define EC_BR_LESS2		0xA4
#define EC_BR_AND_ZERO		0xA5
#define EC_BR_AND_NOT_ZERO	0xA6
#define EC_BR_MM_ZERO		0xA7
#define EC_BR_MM_NOT_ZERO	0xA8
#define EC_BR_AND_AND_ZERO	0xA9
#define EC_BR_AND_AND_NOT_ZERO	0xAA
#define EC_BR_OR_OR_ZERO	0xAB
#define EC_BR_OR_OR_NOT_ZERO	0xAC

#define EC_BR_BT		0xAD
#define EC_BR_BTS		0xAE
#define EC_BR_BTR		0xAF
#define EC_BR_BTC		0xB0
#define EC_BR_LBTS		0xB1
#define EC_BR_LBTR		0xB2
#define EC_BR_LBTC		0xB3
#define EC_BR_NOT_BT		0xB4
#define EC_BR_NOT_BTS		0xB5
#define EC_BR_NOT_BTR		0xB6
#define EC_BR_NOT_BTC		0xB7
#define EC_BR_NOT_LBTS		0xB8
#define EC_BR_NOT_LBTR		0xB9
#define EC_BR_NOT_LBTC		0xBA

#define EC_SWAP_U8		0xBB
#define EC_SWAP_U16		0xBC
#define EC_SWAP_U32		0xBD
#define EC_SWAP_U64		0xBE

#define EC_ABS_I64		0xBF
#define EC_SIGN_I64		0xC0
#define EC_MIN_I64		0xC1
#define EC_MIN_U64		0xC2
#define EC_MAX_I64		0xC3
#define EC_MAX_U64		0xC4
#define EC_LIMIT_I64		0xC5
#define EC_LIMIT_U64		0xC6
#define EC_MOD_U64		0xC7
#define EC_SQR_I64		0xC8
#define EC_SQR_U64		0xC9

#define EC_MOV_FS		0xCA
#define EC_MOV_GS		0xCB

#define EC_NUM_ENTRIES		0xCC
//Don't forget $LK,"IC_INIT_TABLE","FA:::/LT/Compiler/CmpInit.CPZ,IC_INIT_TABLE"$

#define KW_INCLUDE	0
#define KW_DEFINE	1
#define KW_UNION	2
#define KW_CATCH	3
#define KW_CLASS	4
#define KW_TRY		5
#define KW_IF		6
#define KW_ELSE		7
#define KW_FOR		8
#define KW_WHILE	9
#define KW_EXTERN	10
#define KW_LTEXTERN	11
#define KW_RETURN	12
#define KW_SIZEOF	13
#define KW_INTERNAL	14
#define KW_DO		15
#define KW_ASM		16
#define KW_GOTO		17
#define KW_EXE		18
#define KW_BREAK	19
#define KW_SWITCH	20
#define KW_NOBOUND_SWITCH 21
#define KW_CASE		22
#define KW_DFT		23
#define KW_PUBLIC	24
#define KW_OFFSET	25
#define KW_IMPORT	26
#define KW_LTIMPORT	27
#define KW_IFDEF	28
#define KW_IFNDEF	29
#define KW_IFSTATIC	30
#define KW_IFNSTATIC	31
#define KW_ENDIF	32
#define KW_ASSERT	33
#define KW_REG		34
#define KW_NOREG	35
#define KW_NOUNUSEDWARN	36
#define KW_HELP_INDEX	37
#define KW_HELP_FILE	38
#define KW_TYPEDEF	39
#define KW_STATIC	40
#define KW_INTERRUPT	41
#define KW_LOCK		42
#define KW_DEFINED	43
#define KW_ARGPOP	44
#define KW_NOARGPOP	45

#define AKW_ALIGN	64
#define AKW_I0		65
#define AKW_I8		66
#define AKW_I16		67
#define AKW_I32		68
#define AKW_I64		69
#define AKW_U0		70
#define AKW_U8		71
#define AKW_U16		72
#define AKW_U32		73
#define AKW_U64		74
#define AKW_F64		75
#define AKW_DU8		76
#define AKW_DU16	77
#define AKW_DU32	78
#define AKW_DU64	79
#define AKW_DUP		80
#define AKW_USE16	81
#define AKW_USE32	82
#define AKW_USE64	83
#define AKW_IMPORT	84
#define AKW_LIST	85
#define AKW_NOLIST	86
#define KW_NUM_KEYWORDS	87

#define CMP_TEMPLATE_INC		0x00
#define CMP_TEMPLATE_DEC		0x01
#define CMP_TEMPLATE_MOD		0x02
#define CMP_TEMPLATE_LESS		0x03
#define CMP_TEMPLATE_GREATER		0x04
#define CMP_TEMPLATE_LESS_EQUAL		0x05
#define CMP_TEMPLATE_GREATER_EQUAL	0x06
#define CMP_TEMPLATE_STRLEN		0x07
#define CMP_TEMPLATE_RDTSC		0x08
#define CMP_TEMPLATE_SIGN_I64		0x09
#define CMP_TEMPLATE_FSTP		0x0A
#define CMP_TEMPLATE_FLD		0x0B
#define CMP_TEMPLATE_HPET		0x0C

#define ASSOCF_LEFT	1
#define ASSOCF_RIGHT	2
#define ASSOC_MASK	3

#define PREC_NULL		0x00
#define PREC_TERM		0x04
#define PREC_UNARY_POST		0x08
#define PREC_UNARY_PRE		0x0C
#define PREC_EXP		0x10
#define PREC_MUL		0x14
#define PREC_AND		0x18
#define PREC_XOR		0x1C
#define PREC_OR			0x20
#define PREC_ADD		0x24
#define PREC_CMP		0x28
#define PREC_CMP2		0x2C
#define PREC_AND_AND		0x30
#define PREC_XOR_XOR		0x34
#define PREC_OR_OR		0x38
#define PREC_QUEST		0x3C
#define PREC_ASSIGN		0x40

#define PREC_MAX		0x44

#define CPS_IS_PUBLIC		1
#define CPS_IS_INTERRUPT	2
#define CPS_IS_ASM		4
#define CPS_IS_STATIC		8
#define CPS_IS_ARGPOP		16
#define CPS_IS_NOARGPOP		32

#define CPS_FUN_FLAGS		(CPS_IS_INTERRUPT|CPS_IS_PUBLIC|CPS_IS_ARGPOP|CPS_IS_NOARGPOP)

#define PRS_NULL		0
#define PRS_LTEXTERN		1
#define PRS_LTINTERN		2
#define PRS_LTIMPORT		3
#define PRS_EXTERN		4
#define PRS_IMPORT		5
#define PRS_TYPEDEF		6
#define PRS_NUM_GLBL		7

#define PRS_TYPECAST		7

#define PRSF_LEX_PUSH		8
#define PRS_LOCAL_VAR		(0+PRSF_LEX_PUSH)
#define PRS_FUN_ARG	(1+PRSF_LEX_PUSH)
#define PRS_CLASS		(2+PRSF_LEX_PUSH)
#define PRS_UNION		(3+PRSF_LEX_PUSH)
#define PRS_STATIC_LOCAL_VAR	(4+PRSF_LEX_PUSH)

class OptRegStruct
{
  I64 offset;
  MemberListStruct *m;
};
