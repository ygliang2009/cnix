/*************************************************************************************
 				系统调用处理函数C部分
			##########   汇编部分请参考syshandle.s  ###############
		@Author:Chemical Liang	@Version:1.0	@Date:2011-11-19
 *************************************************************************************/
#include "cnix.h"
#include "proto.h"

/* 0号系统调用 */
int syscall_test()
{
	printf("this is syscall");
}
/* write系统调用 */
int syscall_write(char *buf, int len, int undefined, Pcb *proc_tmp)
{
	char *p = buf;
	prtstr(&tty[proc_tmp->tty_num],buf,len);
	/*int i = 0;
	while(i < len)
	{
		print_dest_char(&tty[proc_ready->tty_num],*p++);
		i++;
	}
	*/
	return 0;
}

/* pause系统调用 */
int sys_pause(void)
{
	proc_ready->state = TASK_INTERRUPTABLE;
	schedule();
	return DONE;
}

/* getpid系统调用 */
int sys_getpid(void)
{
	return proc_ready->pid;
}

/* getblock系统调用 */
int sys_getblock(void)
{
	return proc_ready->block;
}

/* getsignal系统调用*/
int sys_getsignal(void)
{
	return proc_ready->signal;
}

/* nice系统调用，调整当前进程的优先级 */
int sys_nice(int increment)
{
	int i = increment;
	if(i > 0 && proc_ready->priority + i > 100)
	{
		proc_ready->priority = 100;
	}else if(i < 0 && proc_ready->priority + i <= 0)
	{
		proc_ready->priority = 1;
	}else
	{
		proc_ready->priority = proc_ready->priority + i;
	}
	return proc_ready->priority;
}
