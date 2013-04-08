/***************************************************************************
	@Author:Chemical Liang	@Version:1.0	@Date:2011-10-24
****************************************************************************/
.extern cstart
.extern disPos
.section .bss
.lcomm StackSpace,256
.set TopOfStack, .
.global TopOfStack
.code32
.section .text
.global _start
.global prints
.global init8259A
.global gdtPtr
.global absPrint
.global printVar
.set SelectorTSS,0x20

_start:
	jmp Kernel_Start
KernelMessage:
	.ascii "Hello chemical"
Kernel_Start:
	movl $TopOfStack,%esp
	movl $0,%edi
	movb $'S',%gs:(%edi)
	/* 将GDT注册给gdtPtr结构 */
	sgdtw gdtPtr
	/* 一些内核态的初始化信息 */
	call cstart
	/* 加载新的GDT结构 */
	lgdtw gdtPtr
	/* 加载新的idtPtr */
	lidtw idtPtr
	nop
	sti
	/* 加载TSS */
	movw $SelectorTSS,%ax
	ltr %ax 
	/* 内核主方法 */
	jmp kernel_main

/****************************************************************
	void prints(char *message)
	初期的内核调试打印函数 
 ***************************************************************/
prints:
	pushl %ebp
	movl %esp,%ebp
	pushl %eax
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl %esi
	pushl %edi
	movl 8(%ebp),%esi
	movl (disPos),%edi
printLoop:
	lodsb
	testb %al,%al
	jz printReturn
	cmpb $0x0a,%al
	jnz printDisp
	pushl %eax
	xor %eax,%eax
	movl %edi,%eax
	movb $160,%bl
	divb %bl
	andl $0xFF,%eax
	incl %eax
	movb $160,%bl
	mulb %bl
	movl %eax,%edi
	popl %eax
	jmp printLoop
printDisp:
	movb $0x0F,%ah
	movb %al,%gs:(%edi)
	addl $2,%edi
	jmp printLoop
printReturn:
	movl %edi,(disPos)
	popl %edi
	popl %esi
	popl %edx
	popl %ecx
	popl %ebx
	popl %eax
	movl %ebp,%esp
	popl %ebp
	ret

/***********************************************************************************
	8259A初始化
************************************************************************************/
init8259A:
	/* 主8259A,ICW1 */
	movb $0x11,%al
	outb %al,$0x20
	call iodelay
	/* 从8259A,ICW1 */
	outb %al,$0xa0
	call iodelay
	/* 主8259A,ICW2,IRQ0对应的中断向量0x20 */
	movb $0x20,%al
	outb %al,$0x21
	call iodelay
	/* 从8259A,ICW2,IRQ8对应的中断向量0x28 */
	movb $0x28,%al
	outb %al,$0xa1
	call iodelay
	/* 主8259A,ICW3 */
	movb $0x04,%al
	outb %al,$0x21
	call iodelay
	/* 从8259A,ICW3 */
	movb $0x02,%al
	outb %al,$0xa1
	call iodelay
	/* 主8259A,ICW4 */
	movb $0x01,%al
	outb %al,$0x21
	call iodelay
	/* 从8259A,ICW4 */
	outb %al,$0xa1
	call iodelay
	/* 主8259A,OCW1 */
	movb $0b11111111,%al
	outb %al,$0x21
	call iodelay
	/* 从8259A,OCW1 */
	movb $0b11111111,%al
	outb %al,$0xa1
	call iodelay
	ret
iodelay:
	nop
	nop
	ret

var_dump:
	nop
	pushl %ebp
	nop
	movl %esp,%ebp
	nop
	pushl %eax
	nop
	movl 8(%ebp),%eax
	movl 12(%ebp),%ebx
	popl %eax
	movl %ebp,%esp
	popl %ebp
	ret
