/******************************************************************************************************
 		中断描述符&&中断调用程序相关
 	@Author:Chemical Liang	@Version:1.0	@Date:2011-11-01
	@Author:Chemical Liang	@Version:1.1	@Date:2011-11-17
 ******************************************************************************************************/

#include "const.h"
#include "global.h"
#include "signal.h"
#include "proc.h"
#include "tty.h"
#include "block.h"
#include "proto.h"

void init_idt_ptr()
{
	int idtLen = sizeof(Gate) * IDT_SIZE - 1;
	/* 把指向字符的指针扩充成指向short型的指针，然后对char[0]所在地址处的数据结构附short值 */
	short *idtLenPtr = (short *)&idtPtr[0];
	*idtLenPtr = idtLen;
	/* 把指向字符的指针扩充成指向int型的指针，然后对char[2]所在地址处的数据结构附int值 */
	int *idtBaseAddr = (int *)&idtPtr[2];
	*idtBaseAddr = (int)&idtSeg;
}

static void init_gate(int index,unsigned char desc_type,int_handler offset,unsigned char privilege)
{
	Gate* gateObj = &idtSeg[index];
	int offset_tmp = (int)offset;
	gateObj->offset1 = (offset_tmp & 0xFFFF);
	gateObj->offset2 = ((offset_tmp >> 16) & 0xFFFF);
	gateObj->selector = 8;
	gateObj->dcount = 0;
	gateObj->attr = desc_type | (privilege << 5);
}

void init_idt_seg()
{
	init8259A();
	/* 除法错，RES: div指令 */
	init_gate(INT_VECTOR_DIVIDE,DA_386IGate,divide_exception_handler,PRIVILEGE_KRNL);
	/* 调试异常，RES: 任何代码段和数据段的访问 */
	init_gate(INT_VECTOR_DEBUG,DA_386IGate,debug_exception_handler,PRIVILEGE_KRNL);
	/* 非屏蔽中断，RES: 非屏蔽性外部中断 */
	init_gate(INT_VECTOR_NMI,DA_386IGate,nmi_exception_handler,PRIVILEGE_KRNL);
	/* 调试断点，RES: 指令int 3 */
	init_gate(INT_VECTOR_BREAKPOINT,DA_386IGate,breakpoint_exception_handler,PRIVILEGE_USER);
	/* 溢出，RES: 指令INTO */
	init_gate(INT_VECTOR_OVERFLOW,DA_386IGate,overflow_exception_handler,PRIVILEGE_USER);
	/* 越界，RES: 指令BOUND */
	init_gate(INT_VECTOR_BOUNDS,DA_386IGate,bounds_exception_handler,PRIVILEGE_KRNL);
	/* 无效操作码，RES: 指令UD2或者无效指令 */
	init_gate(INT_VECTOR_INVAL_OP,DA_386IGate,inval_op_exception_handler,PRIVILEGE_KRNL);
	/* 设备不可用，RES: 浮点或WAIT/FWAIT指令 */
	init_gate(INT_VECTOR_COPROC_NOT,DA_386IGate,coproc_not_exception_handler,PRIVILEGE_KRNL);
	/* 双重错误， RES: 所有能产生异常或NMI或INTR的指令 */
	init_gate(INT_VECTOR_DOUBLE_FAULT,DA_386IGate,double_fault_exception_handler,PRIVILEGE_KRNL);
	/* 协处理器段越界 RES: 浮点指令 */
	init_gate(INT_VECTOR_COPROC_SEG,DA_386IGate,coproc_seg_exception_handler,PRIVILEGE_KRNL);
	/* 无效TSS RES: 任务切换或访问TSS时 */
	init_gate(INT_VECTOR_INVAL_TSS,DA_386IGate,inval_tss_exception_handler,PRIVILEGE_KRNL);
	/* 段不存在 RES: 加载段寄存器或访问系统段时 */
	init_gate(INT_VECTOR_SEG_NOT,DA_386IGate,seg_not_exception_handler,PRIVILEGE_KRNL);
	/* 堆栈段错误 RES: 堆栈错误或加载SS时 */
	init_gate(INT_VECTOR_STACK_FAULT,DA_386IGate,stack_fault_exception_handler,PRIVILEGE_KRNL);
	/* 常规保护错误 RES: 内存或其他保护检验 */
	init_gate(INT_VECTOR_PROTECTION,DA_386IGate,protection_exception_handler,PRIVILEGE_KRNL);
	/* 页错误 RES: 内存访问 */
	init_gate(INT_VECTOR_PAGE_FAULT,DA_386IGate,page_fault_exception_handler,PRIVILEGE_KRNL);
	
	init_gate(INT_VECTOR_COPROC_ERR,DA_386IGate,coproc_error_exception_handler,PRIVILEGE_KRNL);
	/* 时钟中断 */
	init_gate(INT_VECTOR_IRQ0 + 0,DA_386IGate,clock_handler,PRIVILEGE_KRNL);
	/* 键盘中断 */
	init_gate(INT_VECTOR_IRQ0 + 1,DA_386IGate,keyboard_handler,PRIVILEGE_KRNL);
	/* 级联从8259A 中断*/
	init_gate(INT_VECTOR_IRQ0 + 2,DA_386IGate,common_hwint,PRIVILEGE_KRNL);
	/* 串口2中断 */
	init_gate(INT_VECTOR_IRQ0 + 3,DA_386IGate,common_hwint,PRIVILEGE_KRNL);
	/* 串口1中断 */
	init_gate(INT_VECTOR_IRQ0 + 4,DA_386IGate,common_hwint,PRIVILEGE_KRNL);
	/* LPT2 */
	init_gate(INT_VECTOR_IRQ0 + 5,DA_386IGate,common_hwint,PRIVILEGE_KRNL);
	/* 软盘中断 */
	init_gate(INT_VECTOR_IRQ0 + 6,DA_386IGate,common_hwint,PRIVILEGE_KRNL);
	/* LPT1 */
	init_gate(INT_VECTOR_IRQ0 + 7,DA_386IGate,common_hwint,PRIVILEGE_KRNL);
	/* 时钟中断 */
	init_gate(INT_VECTOR_IRQ8 + 0,DA_386IGate,common_hwint,PRIVILEGE_KRNL);
	/* 重定向IRQ2 */
	init_gate(INT_VECTOR_IRQ8 + 1,DA_386IGate,common_hwint,PRIVILEGE_KRNL);
	/* 保留 */
	init_gate(INT_VECTOR_IRQ8 + 2,DA_386IGate,common_hwint,PRIVILEGE_KRNL);
	/* 保留 */
	init_gate(INT_VECTOR_IRQ8 + 3,DA_386IGate,common_hwint,PRIVILEGE_KRNL);
	/* 鼠标中断 */
	init_gate(INT_VECTOR_IRQ8 + 4,DA_386IGate,common_hwint,PRIVILEGE_KRNL);
	/* 异常 */
	init_gate(INT_VECTOR_IRQ8 + 5,DA_386IGate,common_hwint,PRIVILEGE_KRNL);
	/* 温彻斯特盘 */
	init_gate(INT_VECTOR_IRQ8 + 6,DA_386IGate,ata_handler,PRIVILEGE_KRNL);
	/* 保留 */
	init_gate(INT_VECTOR_IRQ8 + 7,DA_386IGate,common_hwint,PRIVILEGE_KRNL);
	
	/* 系统调用，调用中断号0x80 */
	init_gate(INT_SYSCALL,DA_386IGate,syscall_handler,PRIVILEGE_USER);
}

void common_hwint()
{
	prints("\n\n------------Hwint Exception------------\n\n");
}

void common_handle()
{
	prints("\n\n------------Handle Exception-----------\n\n");
}

/*********************************************************************************
 		时钟中断处理程序
 *********************************************************************************/
void cclock_handler()
{
	if(countInt != 0)
	{
		return;
	}
	--proc_ready->counter;
	schedule();	
}

void divide_exception()
{
	prints("\n\n------------divide exception------------\n\n");
}

void debug_exception()
{
	prints("\n\n------------debug exception------------\n\n");
}

void nmi_exception()
{
	prints("\n\n------------nmi exception------------\n\n");
}

void breakpoint_exception()
{
	prints("\n\n------------breakpoint exception------------\n\n");
}

void overflow_exception()
{
	prints("\n\n------------overflow exception------------\n\n");
}

void bounds_exception()
{
	prints("\n\n------------bounds exception------------\n\n");
}

void inval_op_exception()
{
	prints("\n\n------------inval op exception------------\n\n");
}

void coproc_not_exception()
{
	prints("\n\n------------coproc not exception------------\n\n");
}

void double_fault_exception()
{
	prints("\n\n------------double fault exception------------\n\n");
}

void coproc_seg_exception()
{
	prints("\n\n------------coproc seg exception------------\n\n");
}

void inval_tss_exception()
{
	prints("\n\n------------inval tss exception------------\n\n");
}

void seg_not_exception()
{
	prints("\n\n------------seg not exception------------\n\n");
}

void stack_fault_exception()
{
	prints("\n\n------------stack fault exception------------\n\n");
}

void protection_exception()
{
	prints("\n\n------------protection exception------------\n\n");
}

void page_fault_exception()
{
	prints("\n\n------------page fault exception------------\n\n");
}

void coproc_error_exception()
{
	prints("\n\n------------coproc error exception------------\n\n");
}
