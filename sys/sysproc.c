#include "cnix.h"
#include "stdlib.h"


/* 系统进程B */
void SysProcB()
{
	int i = 0;
	while(1)
	{
//		prints("C.");
		/* 这里必须要有个延迟，否则编译器直接报内存溢出错误 */
//		timedelay(5);
	}
}



/* 键盘处理进程 */
void keyboard_main()
{
	int i = 0;
	while(1)
	{
		disp_key();
	}
}

/* 硬盘驱动程序 */
void task_hd()
{
	/* 任务初始化 */
	init_hd();
	/* 系统级别任务，while是必须有的 */
	while(1)
	{
		/* 任务将在此不断地循环 */
		
	}
}


/* 进程调度程序 */
void schedule()
{
	/* 当前进程运行系统时间加一个时钟滴答 */
	proc_ready->sys_time++;
	int max = 0;
	int i;
	for(i = 0;i < taskTable.count; i++)
	{
		/* 如果在进程信号位图中有没有被阻塞的信号，并且进程的状态为可中断等待状态，则置其为运行状态 */
		if((proc_table[i].signal & proc_table[i].block) && proc_table[i].state == TASK_INTERRUPTABLE)
		{
			proc_table[i].state = TASK_RUNNING;
		}

		if(proc_table[i].state == TASK_RUNNING && proc_table[i].counter > max)
		{
			max = proc_table[i].counter;
			proc_ready = &(proc_table[i]);
		}
	}
	if(!max)
	{
		i = 0;
		for(;i < taskTable.count; i++)
		{
			proc_table[i].counter = proc_table[i].priority;
		}
		proc_ready = &(proc_table[0]);
	}	
}

/* state 为 INTERRUPTABLE 或者 UNINTERRUPTABLE*/
static inline void __sleep_on(Pcb *pcb, int state)
{
	/* 不支持INTERRUPTABLE和UNINTERRUPTABLE以外的状态 */
	if(state != TASK_INTERRUPTABLE && state != TASK_UNINTERRUPTABLE)
		return;
	pcb->state = state;
	remove_running_queue(pcb);
	if(state == TASK_INTERRUPTABLE)
	{
		insert_interrupt_queue(pcb);		
	}else{
		insert_uninterrupt_queue(pcb);
	}
	/******************************************************************
	 	见《源码分析》sched.c中的sleep_on方法中部分源码思想不明。
		调度schedule时由于proc的state已经设置成INTERRUPTABLE状态，
		所以，调度程序今后一段时间将无法调度该进程(见schedule函数)，
		即程序在此阻塞，只有在给该进程一个信号时，该进程才会再次变
		为运行状态，进程才会继续执行
	 ******************************************************************/
	schedule();	
}

/* 不可中断等待状态休眠 */
void sleep_on(Pcb *pcb)
{
	__sleep_on(pcb,TASK_UNINTERRUPTABLE);
}

/* 可中断等待状态休眠 */
void interrupt_sleep_on(Pcb *pcb)
{
	__sleep_on(pcb,TASK_INTERRUPTABLE);	
}

/* 唤醒睡眠等待中的进程 */
void wake_up(Pcb *pcb)
{
	if(pcb == NULL)
		return;
	if(pcb->state == TASK_STOP)
	{
		printf("wake up: TASK_STOP");
		return;
	}
	else if(pcb->state == TASK_ZOMBIE){
		printf("wake up: TASK_ZOMBIE");
		return;
	}
	pcb->state = TASK_RUNNING;
}
