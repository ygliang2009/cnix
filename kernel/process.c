/*********************************************************************************
 		进程相关函数，包括进程表，进程运行队列，可中断进程队列
				的相关操作等
		@Author:Chemical Liang	@Version:1.0	@Date:2011-11-0X
		@Author:Chemical Liang	@Version:1.1	@Date:2011-11-24
 *********************************************************************************/
#include "error.h"
#include "const.h"
#include "tty.h"
#include "global.h"
#include "signal.h"
#include "proc.h"
#include "block.h"
#include "proto.h"

/* 初始化可运行进程队列 */
static void init_running_queue()
{
	int i = 0;
	while(i < PROC_MAX)
	{
		running_queue->queue[i] = NULL;	
		i++;	
	}	
	running_queue->length = 0;
}

/* 插入可运行进程队列节点 */
int insert_running_queue(Pcb *pcb)
{
	if(running_queue->length >= PROC_MAX)
	{
		return -NO_PROC_POS;
	}
	/* 数组下标优先级最大 */
	running_queue->queue[running_queue->length] = pcb;
	running_queue->length ++;
	return DONE;	
}

/* 从可运行进程队列中移除节点，涉及到节点迁移 */
int remove_running_queue(Pcb *pcb)
{
	int i = 0;
	int pos = 0;
	int j = 0;
	while(i < running_queue->length)
	{
		if(running_queue->queue[i] == pcb)
		{
			/* 如果相等，则标记上节点 */
			pos = i;
			j = pos;
			running_queue->queue[pos] = NULL;
			while(j < running_queue->length - 1)
			{
				running_queue[j] = running_queue[j + 1];
				j ++;
			}
			running_queue->length--;
			return DONE;
		}else{
			i++;
		}
	}
	if(i == running_queue->length)
	{
		return -NO_SUCH_NODE;			
	}
	return -WHATS_THIS;	
}


/* 初始化可中断睡眠进程队列 */
static void init_interrupt_queue()
{
	int i = 0;
	while(i < PROC_MAX)
	{
		interrupt_queue->queue[i] = NULL;	
		i++;	
	}	
	interrupt_queue->length = 0;
}


/* 插入可中断睡眠进程队列节点 */
int insert_interrupt_queue(Pcb *pcb)
{
	if(interrupt_queue->length >= PROC_MAX)
	{
		return -NO_PROC_POS;
	}
	/* 数组下标优先级最大 */
	interrupt_queue->queue[interrupt_queue->length] = pcb;
	interrupt_queue->length ++;
	return DONE;	
}


/* 从可中断睡眠进程队列中移除节点 */
int remove_interrupt_queue(Pcb *pcb)
{
	int i = 0;
	int pos = 0;
	int j = 0;
	while(i < interrupt_queue->length)
	{
		if(interrupt_queue->queue[i] == pcb)
		{
			/* 如果相等，则标记上节点 */
			pos = i;
			j = pos;
			interrupt_queue->queue[pos] = NULL;
			while(j < interrupt_queue->length - 1)
			{
				interrupt_queue[j] = interrupt_queue[j + 1];
				j ++;
			}
			interrupt_queue->length--;
			return DONE;
		}else{
			i++;
		}
	}
	if(i == interrupt_queue->length)
	{
		return -NO_SUCH_NODE;			
	}
	return -WHATS_THIS;	
}

/* 初始化不可中断等待进程队列 */
static void init_uninterrupt_queue()
{
	int i = 0;
	while(i < PROC_MAX)
	{
		uninterrupt_queue->queue[i] = NULL;	
		i++;	
	}	
	uninterrupt_queue->length = 0;
}


/* 插入不可中断等待进程队列节点 */
int insert_uninterrupt_queue(Pcb *pcb)
{
	if(uninterrupt_queue->length >= PROC_MAX)
	{
		return -NO_PROC_POS;
	}
	/* 数组下标优先级最大 */
	uninterrupt_queue->queue[uninterrupt_queue->length] = pcb;
	uninterrupt_queue->length ++;
	return DONE;	
}


/* 从不可中断等待进程队列中移除节点 */
int remove_uninterrupt_queue(Pcb *pcb)
{
	int i = 0;
	int pos = 0;
	int j = 0;
	while(i < uninterrupt_queue->length)
	{
		if(uninterrupt_queue->queue[i] == pcb)
		{
			/* 如果相等，则标记上节点 */
			pos = i;
			j = pos;
			uninterrupt_queue->queue[pos] = NULL;
			while(j < uninterrupt_queue->length - 1)
			{
				uninterrupt_queue[j] = uninterrupt_queue[j + 1];
				j ++;
			}
			uninterrupt_queue->length--;
			return DONE;
		}else{
			i++;
		}
	}
	if(i == uninterrupt_queue->length)
	{
		return -NO_SUCH_NODE;			
	}
	return -WHATS_THIS;	
}


/* 向进程表中插入进程 */
void insert_task(p_func eip_p,int sp_offset,int timecount,char name[],int tty_num)
{
	if(taskTable.count < TASK_MAX){
		taskTable.tableContent[taskTable.count].eip_pointer = eip_p;
		taskTable.tableContent[taskTable.count].sp_offset=  sp_offset;
		taskTable.tableContent[taskTable.count].count = timecount;
		/* 赋值字符数组要用str_cpy */
		str_cpy(taskTable.tableContent[taskTable.count].name,name);
		taskTable.count ++;
	}	
}

/*************************************************************************
 	?? void init_task()方法初始化任务段，这里开始的时候用的是静态初
 	 始化结构的方法，见global.c中的初始化方法，这种初始化方法始终不
 	正确. 待以后解决??
 *************************************************************************/

void init_task()
{
	/* 对A,B,C 三个系统进程赋予优先级初始值 */
	init_running_queue();
	init_interrupt_queue();
	init_uninterrupt_queue();
	insert_task(task_hd,STACK_SIZE_A,10,"task_hd",0);
	insert_task(SysProcB,STACK_SIZE_B,5,"SysProcB",0);
	insert_task(keyboard_main,STACK_SIZE_C,15,"keyboard_main",0);	
}

/* 初始化进程表中的进程 */
void init_proc()
{
	int stack_top = STACK_SIZE_TOTAL;
	short selector_ldt = SELECTOR_LDT;
	int i = 0;
	/* 构造LDT，然后跑到LDT执行用户态代码 */
	while(i < taskTable.count){
		str_cpy(proc_table[i].name,taskTable.tableContent[i].name);
		mem_cpy(&(proc_table[i].ldts[0]),&gdtSeg[INDEX_CODE],sizeof(Descriptor));	
		mem_cpy(&(proc_table[i].ldts[1]),&gdtSeg[INDEX_DATA],sizeof(Descriptor));
		
		/* 信号位图与阻塞位图初始化为0，SIGKILL和SIGSTOP默认开启 */
		proc_table[i].signal = 0x00000000|(1 << (SIGKILL - 1))|(1 << (SIGSTOP - 1));
		proc_table[i].block = 0x00000000|(1 << (SIGKILL - 1))|(1 << (SIGSTOP - 1));
		
		/* Sigaction结构初始化 */
		int sig_tmp = 0;
		for(;sig_tmp < 32;sig_tmp++)
		{
			/* 信号量NUMBER从1开始 */
			proc_table[i].sigaction[sig_tmp].sig_num = sig_tmp + 1;
			proc_table[i].sigaction[sig_tmp].sig_flag = 0;
			proc_table[i].sigaction[sig_tmp].sig_mask = 0;
			/* 默认为SIG_DEF处理方式 */
			proc_table[i].sigaction[sig_tmp].sig_handler = SIG_DEF;
		}
		
		proc_table[i].pid = i;	
		
		/* 代码段初始化为只读 */
		proc_table[i].ldts[0].attr1 = DA_C | PRIVILEGE_TASK << 5;
		/* 数据段申请为可读写 */
		proc_table[i].ldts[1].attr1 = DA_DRW | PRIVILEGE_TASK << 5;
		proc_table[i].ldt_sel = selector_ldt; 	 
		
		/* 优先级初始化,默认值为50 */
		if(taskTable.tableContent[i].count == 0)
		{
			proc_table[i].priority = 50;
		}else{
			proc_table[i].priority = taskTable.tableContent[i].count;
		}
		/* 滴答数首先初始化为0 */
		proc_table[i].counter = proc_table[i].priority;
		
		/* 插入进程表以后的进程被初始化为TASK_RUNNING状态 */
		proc_table[i].state = TASK_RUNNING;

		/* 进程的运行时间初始化为0 */
		proc_table[i].sys_time = 0;
		proc_table[i].usr_time = 0;
		
		/* 初始化进程堆栈中的各个寄存器 */
		proc_table[i].pcbStack.cs = (0 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | SA_RPL1;
       		proc_table[i].pcbStack.ds = (8 & SA_RPL_MASK & SA_TI_MASK)| SA_TIL | SA_RPL1;	
       		proc_table[i].pcbStack.es = (8 & SA_RPL_MASK & SA_TI_MASK)| SA_TIL | SA_RPL1;	
       		proc_table[i].pcbStack.fs = (8 & SA_RPL_MASK & SA_TI_MASK)| SA_TIL | SA_RPL1;	
       		proc_table[i].pcbStack.ss = (8 & SA_RPL_MASK & SA_TI_MASK)| SA_TIL | SA_RPL1;	
       		proc_table[i].pcbStack.gs = (SELECTOR_VIDEO & SA_RPL_MASK) | SA_RPL1;
		
		/* eip指向任务表中的函数指针 */	
		proc_table[i].pcbStack.eip = (int)(taskTable.tableContent[i].eip_pointer);
		proc_table[i].pcbStack.esp = (int)(task_stack + stack_top);
		proc_table[i].pcbStack.eflags = 0x1202;
		stack_top -= taskTable.tableContent[i].sp_offset;
		selector_ldt += 8;
		/* 运行进程队列中的进程入队 */
		running_queue->queue[i] = &proc_table[i];
		i++;
	}
	proc_ready = proc_table;
}

/* 进程A */
void TestA()
{
	int i = 0;
	prints("\n                                ProcessA\n");
	while(1)
	{
		prints("A.");
		/* 这里必须要有个延迟，否则编译器直接报内存溢出错误 */
		timedelay(10);
	}
}

