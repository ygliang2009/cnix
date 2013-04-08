/**************************************************************************
 *			loader.s的头文件	
 *
 * 	@Author:Chemical Liang	@Version:1.0	@Date:2011-10-13
 **************************************************************************/
.macro Descriptor Base,	Limit,	Attr
	.2byte \Limit & 0xFFFF
	.2byte \Base & 0xFFFF
	.byte (\Base >> 16) & 0xFF
	.2byte ((\Limit >> 8) & 0xF00) | (\Attr & 0xF0FF)
	.byte (\Base >> 24) & 0xFF
.endm

.macro Gate Selector,Offset,PCount,Attr
	.2byte (\Offset & 0xFFFF)
	.2byte \Selector
	.2byte (\PCount & 0x1F) | ((\Attr << 8) & 0xFF00)
	.2byte ((\Offset >> 16) & 0xFFFF)
.endm

.macro InitDesc Desc,Code_Start
	xor %eax,%eax
	mov %cs,%ax
	shl $4,%eax
	addl $(\Code_Start),%eax
	mov %ax,((\Desc) + 2)
	shr $16,%eax
	movb %al,((\Desc) + 4)
	movb %ah,((\Desc) + 7)
.endm

.set DA_32,		0x4000		/*32-bit segment*/
.set DA_LIMIT_4K,	0x8000

.set PG_P,		1
.set PG_RWR,		0
.set PG_RWW,		2
.set PG_USS,		0
.set PG_USU,		4

.set DA_DPL0,		0x00		/*DPL=1*/
.set DA_DPL1,		0x20
.set DA_DPL2,		0x40
.set DA_DPL3,		0x60

.set DA_DR,		0x90		/*Read Only*/
.set DA_DRW,		0x92		/*Read/Write*/
.set DA_DRWA,		0x93		/*Read/Write Access*/
.set DA_C,		0x98		/*Execute-Only*/
.set DA_CR,		0x9A		/*Execute-Read*/
.set DA_CCO,		0x9C		/*Execute-Only,Confirming*/
.set DA_CCOR,		0x9E		/*Execute/Read-Only,confirming*/

.set DA_LDT,		0x82		/*LDT*/
.set DA_TaskGate,	0x85		/*Task Gate*/
.set DA_386TSS,		0x89		/*32-bit TSS*/	
.set DA_386CGate,	0x8C		/*32-bit Call Gate*/
.set DA_386IGate,	0x8E		/*32-bit Interupt Gate*/
.set DA_386TGate,	0x8F		/*32-bit Trap Gate*/

/*Selector Attribute*/
.set SA_RPL0,		0
.set SA_RPL1,		1
.set SA_RPL2,		2
.set SA_RPL3,		3
.set SA_TIG,		0
.set SA_TIL,		4

/* 硬盘基本信息存入地址 */
.equ BaseOfDisk,	0x2000
.equ OffsetOfDisk1,	0x0080
.equ OffsetOfDisk2,	0x0090
