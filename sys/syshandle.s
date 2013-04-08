/*******************************************************************************
 			系统调用处理函数汇编部分
		########### 	C部分请参见syscall.c	##############
	@Author:Chemical Liang	@Version:1.0	@Date:2011-11-19
	
	注意：这里把时钟中断处理程序放入到了系统调用所在文件中。由于时钟
	中断和系统调用在由内核态切换到用户态期间都需要检测信号的存在，并
			      执行信号处理程序
*******************************************************************************/

.section .text
.include "sys/include/syshandle.h"
.global syscall_control
.global syscall_handler
.global clock_handler
.global test
.global write
.extern signal_handler 
.global pause
.global getpid
.global nice
.global getsignal
.global getblock

SIGNAL_OFFSET = 76
BLOCK_OFFSET = 80

syscall_control:
	int $0x80
	ret

syscall_handler:
	call save
	sti
/* 系统调用最多支持三个参数 */
	pushl (proc_ready)
	pushl %edx
	pushl %ecx
	pushl %ebx 
	call *syscall_table(,%eax,4)
	addl $16,%esp
	movl %eax,44(%ebp)	
	cli
	ret

/* test系统调用 */
test:
	movl $TEST_NUM,%eax
	int $INT_SYSCALL
	ret

/* write系统调用 */
write:
	movl $WRITE_NUM,%eax
	movl 4(%esp),%ebx		/* char *buf */
	movl 8(%esp),%ecx		/* int len */
	movl $0,%edx
	int $INT_SYSCALL
	ret

/* pause系统调用 */
pause:
	movl $PAUSE_NUM,%eax
	int $INT_SYSCALL
	ret

/* getpid系统调用 */
getpid:
	movl $GETPID_NUM,%eax
	int $INT_SYSCALL
	ret

/* nice系统调用 */
nice:	
	movl $NICE_NUM,%eax
	movl 4(%esp),%ebx
	int $INT_SYSCALL
	ret

/* getsignal系统调用 */
getsignal:
	movl $GETSIGNAL_NUM,%eax
	int $INT_SYSCALL
	ret

/* getblock系统调用 */
getblock:
	movl $GETBLOCK_NUM,%eax
	int $INT_SYSCALL
	ret

/***********************************************************
		时钟中断处理程序
	注意：这里把时钟中断处理程序放入到了系统调用所在
	文件中。由于时钟中断和系统调用在由内核态切换到用
	户态期间都需要检测信号的存在，并执行信号处理程序
***********************************************************/
clock_handler:
	call save	
	/* 屏幕首字母轮询显示 */
	movl $0,%edi
	movb %gs:(%edi),%al
	inc %al
	movb %al,%gs:(%edi)
	movb $0x20,%al
	outb %al,$0x20
	sti
	/* 进程切换调度相关函数 */
	call cclock_handler
	pushl %esi
	/****************************************
		信号处理相关函数
	*****************************************/
	/* %eax存有signal位图 */
	/*****************************************************************
		proc_ready是一个指针，所以需要间接寻址方式
	******************************************************************/
	movl proc_ready,%esi
	movl SIGNAL_OFFSET(%esi), %eax
	/* %ebx存有block位图 */
	movl BLOCK_OFFSET(%esi), %ebx
	popl %esi	
	/* ret指令执行完毕以后就跳入到storeProc中(proc.s中定义) */
	andl %ebx,%eax
	cmpl $0,%eax
	/* 没有信号位则直接跳出 */
	je ret_tag
	/* 顺序扫描没有被阻塞的可用位 */
	movl $32,%ecx
check_loop:
	/* 从低到高遍历信号标识，将位数存入到do_signal函数中 */
	bsfl %eax,%ebx
	/* 信号需要在1-32之间 */
	cmpl $1,%ebx
	jb ret_tag
	cmpl $32,%ebx
	ja ret_tag
	/* 推入信号偏移 */
	pushl %ebx
	call do_signal
	addl $4,%esp 
	btrl %ebx,%eax
	loop check_loop
ret_tag:
	cli
	ret
