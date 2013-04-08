#ifndef _CONST_H
#define _CONST_H

/* 选择子类型值说明 */
#define SA_RPL_MASK     0xFFFC
#define SA_RPL0         0
#define SA_RPL1         1
#define SA_RPL2         2
#define SA_RPL3         3
#define SA_TI_MASK      0xFFFB
#define SA_TIG          0
#define SA_TIL          4

/* 段描述符属性 */
#define DA_DR		0x90
#define DA_DRW		0x92
#define DA_DRWA		0x93
#define DA_C		0x98
#define DA_CR		0x9A
#define DA_CCO		0x9C
#define DA_CCOR		0x9E
#define DA_DPL0		0x00
#define DA_DPL1		0x20
#define DA_DPL2		0x40
#define DA_DPL3		0x60
/* 系统段描述符类型值说明 */
#define DA_LDT		0x82
#define DA_TaskGate	0x85
#define	DA_386TSS	0x89
#define DA_386CGate	0x8C
#define DA_386IGate	0x8E
#define DA_386TGate	0x8F

/* 数据端选择子 */
#define SELECTOR_CODE	0x08
#define SELECTOR_DATA	0x10
#define SELECTOR_KERNEL_CODE	SELECTOR_CODE
#define SELECTOR_KERNEL_DATA	SELECTOR_DATA

#define SELECTOR_VIDEO	(0x18+SA_RPL3)
#define SELECTOR_TSS	0x20
#define SELECTOR_LDT	0x28

#define PRIVILEGE_KRNL	0
#define PRIVILEGE_TASK	1
#define PRIVILEGE_USER	3

/* IDT可承载256个描述符 */
#define IDT_SIZE 	256
/* GDT共可承载128个描述符 */
#define GDT_SIZE	128

/* 中断向量号 */
#define INT_VECTOR_DIVIDE	0x0
#define INT_VECTOR_DEBUG	0x1
#define INT_VECTOR_NMI		0x2
#define INT_VECTOR_BREAKPOINT	0x3
#define INT_VECTOR_OVERFLOW	0x4
#define INT_VECTOR_BOUNDS	0x5
#define INT_VECTOR_INVAL_OP	0x6
#define INT_VECTOR_COPROC_NOT	0x7
#define INT_VECTOR_DOUBLE_FAULT	0x8
#define INT_VECTOR_COPROC_SEG	0x9
#define INT_VECTOR_INVAL_TSS	0xA
#define INT_VECTOR_SEG_NOT	0xB
#define INT_VECTOR_STACK_FAULT	0xC
#define INT_VECTOR_PROTECTION	0xD
#define INT_VECTOR_PAGE_FAULT	0xE
#define INT_VECTOR_COPROC_ERR	0x10

/* IRQ0,IRQ8起始中断向量号 */
#define INT_VECTOR_IRQ0		0x20
#define INT_VECTOR_IRQ8		0x28
#define INT_SYSCALL		0x80

#define INDEX_FAKE		0
#define INDEX_CODE		1
#define INDEX_DATA		2
#define INDEX_VIDEO		3
#define INDEX_TSS		4
#define INDEX_LDT		5

/* 每个LdtSeg里含有两个Descriptor结构 */
#define LDT_SIZE		2

/* 系统调用最大支持的个数为128 */
#define NR_SYSCALL		128

/* 键盘缓冲区最大支持字符数 */
#define NR_KEYBUF		64

/* TTY的个数 */
#define NR_TTY			3

/* 显卡内存大小 */
#define	V_MEM_SIZE		0x8000
/* 显存首地址 */
#define V_MEM_BASE		0xB8000
/* 默认的字符显示颜色 */
#define DEFAULT_CHAR_COLOR	0x07
/* 屏幕宽度 */
#define SCREEN_WIDTH		80
/* 屏幕大小 */
#define SCREEN_SIZE		(80*25)

/* 设定NULL为空指针 */
#ifndef _NULL
#define		NULL		((void *)0)
#endif

#endif
