.section .text
.global delay
.global restart

/* 中断处理程序 */
.global	divide_exception_handler
.global debug_exception_handler
.global nmi_exception_handler
.global breakpoint_exception_handler
.global overflow_exception_handler
.global bounds_exception_handler
.global inval_op_exception_handler
.global coproc_not_exception_handler
.global double_fault_exception_handler
.global coproc_seg_exception_handler
.global inval_tss_exception_handler 
.global seg_not_exception_handler
.global stack_fault_exception_handler
.global protection_exception_handler
.global page_fault_exception_handler
.global coproc_error_exception_handler
.global keyboard_handler
.global syscall_handler
.global	save

.extern	divide_exception
.extern debug_exception
.extern nmi_exception
.extern breakpoint_exception
.extern overflow_exception
.extern bounds_exception
.extern inval_op_exception
.extern coproc_not_exception
.extern double_fault_exception
.extern coproc_seg_exception
.extern inval_tss_exception
.extern seg_not_exception
.extern stack_fault_exception
.extern protection_exception
.extern page_fault_exception
.extern coproc_error_exception
.extern ckeyboard_handler

.extern countInt
.extern proc_ready
.extern cclock_handler
.extern syscall_table

.global syscall_control
.global test
.global ata_handler

	
.equ	INT_SYSCALL,0x80

delay:
	nop
	nop
	nop
	ret

/********************************************************************
   加载进程表中的LDT描述符，使进程转到用户态下执行，由于转移以后的  
   进程处在Ring3中，而时钟中断会将进程转到Ring0，此时CPU自动加载TSS
   保存的esp0信息。通常在中断处理程序里需要对原任务的个寄存器进行保存，
   为方便此情况, 需要在restart里保存TSS的esp0段为进程控制块的最低栈地址。
   从而当中断发生时，CPU加载该esp0地址，然后中断处理程序中做push时就会
   push到PCB的STACK结构中
 ********************************************************************/
restart:
	/* 栈基址指向proc_ready进程控制结构 */
	movl (proc_ready),%esp
	/* 72为LdtSelector在PCB中的偏移量，见proc.h */
	movw 72(%esp),%ax
	lldtw %ax
	lea 72(%esp),%eax
	movl %eax,(tss + 4)
	popl %gs
	popl %fs
	popl %es
	popl %ds
	popal	
	addl $4,%esp	
	iret

storeProc:
	movl (proc_ready),%esp
	movw 72(%esp),%ax
	lldtw %ax
	lea 72(%esp),%eax
	movl %eax,(tss + 4)	
leaveProc:
	decl (countInt)
	popl %gs
	popl %fs
	popl %es
	popl %ds
	popal
	addl $4,%esp
	iret

divide_exception_handler:
	call divide_exception
	iret

debug_exception_handler:
	call debug_exception
	iret

nmi_exception_handler:
	call nmi_exception
	iret

breakpoint_exception_handler:
	call breakpoint_exception
	iret

overflow_exception_handler:
	call overflow_exception
	iret

bounds_exception_handler:
	call bounds_exception
	iret

inval_op_exception_handler:
	call inval_op_exception
	iret

coproc_not_exception_handler:
	call coproc_not_exception
	iret

double_fault_exception_handler:
	call double_fault_exception
	iret

coproc_seg_exception_handler:
	call coproc_seg_exception
	iret

inval_tss_exception_handler:
	call inval_tss_exception
	iret

seg_not_exception_handler:
	call seg_not_exception
	iret

stack_fault_exception_handler:
	call stack_fault_exception
	iret

protection_exception_handler:
	call protection_exception
	iret

page_fault_exception_handler:
	call page_fault_exception
	iret

coproc_error_exception_handler:
	call coproc_error_exception
	iret

/**************************************************************************
	注意，save这个函数是很关键的，因为在今后的进程和系统调用当中都要
	用到这个save。在系统调用中，需要用到eax,ebx,ecx,edx四个寄存器，所以，
	这个save一定要保证在转出该函数时，此四个寄存器的值不被修改。
***************************************************************************/
save:
	/* 这里涉及到进程切换，从tss中加载内核态ss0地址 */
	pushal
	pushl %ds
	pushl %es
	pushl %fs
	pushl %gs
	/* edx需要作为系统调用的参数传递到系统调用处理函数中，所以在这里切不能改变edx的值 */
	pushl %edx
	/* 在初始化时已经为SS寄存器赋值 */
	movw %ss,%dx
	movw %dx,%ds
	movw %dx,%es
	popl %edx
	movl %esp,%ebp
	/* TopOfStack引用kernel.s,切入到内核堆栈 */
	movl $TopOfStack,%esp
	/* 中断嵌套递归调用避免 */
	incl (countInt)
	cmpl $0,(countInt)
	jnz save2
	pushl $storeProc
	/* 哪个函数调用了save就jmp到哪个函数 */
	movl 48(%ebp),%esi
	jmpl *%esi
save2:
	pushl $leaveProc
	movl 48(%ebp),%esi
	jmpl *%esi

keyboard_handler:
	call save	
	movb $0x20,%al
	outb %al,$0x20
	sti
	push $1
	call ckeyboard_handler
	addl $4,%esp	
	cli
	ret

ata_handler:
	call save
	inb $0xA1,%al
	orb $(1 << 6),%al
	outb %al,$0xA1
	movb $0x20,%al
	outb %al,$0x20
	nop
	outb %al,$0xA0
	sti
	push $14
	call cata_handler
	addl $4,%esp
	cli
	inb $0xA1,%al
	andb $~(1 << 6),%al
	outb %al,$0xA1
	ret		
