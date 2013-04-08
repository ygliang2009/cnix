#ifndef _PROC_H
#define _PROC_H

/* 进程状态定义 */
#define		TASK_RUNNING				1		/* 运行态 */
#define		TASK_INTERRUPTABLE			2		/* 可中断等待状态 */
#define		TASK_UNINTERRUPTABLE			3		/* 不可中断等待状态 */
#define		TASK_STOP				4		/* 终止状态 */
#define		TASK_ZOMBIE				5		/* 僵死状态 */

typedef void (*p_func)();

typedef struct pcb_stack
{
	int gs;
	int fs;
	int es;
	int ds;
	int edi;
	int esi;
	int ebp;
	int kernel_esp;
	int ebx;
	int edx;
	int ecx;
	int eax;
	int retaddr;
	int eip;
	int cs;
	int eflags;
	int esp;
	int ss;
}PcbStack;

typedef struct pcb
{
	PcbStack pcbStack;
	/* ldt_sel偏移位置为72 */
	short ldt_sel;
	/* 保持signal和block的偏移位置以方便syshandler.s读取，signal偏移位置本应是74，然而由于内存对齐，导致偏移为76 */
	int signal;					/* 信号位图，可填充32个信号*/
	/* 保持block的偏移位置，block的偏移位置为80 */
	int block;					/* 信号阻塞位图，当block & signal时即可得到当前阻塞的信号 */
	Descriptor ldts[LDT_SIZE];	
	int priority;					/* 进程优先级 */
	int counter;					/* 进程滴答数 */
	int state;					/* 进程状态码 RUN | STOP | INTERUPTABLE | UNINTERUPTABLE */
	int pid;					/* 进程id */
	char name[16];					/* 进程名称 */
	int tty_num;					/* 终端号 */
	Sigaction sigaction[32];				/* 对应各个信号处理结构 */
      	long sys_time;					/* 进程的系统执行时间 */
	long usr_time;					/* 进程的用户执行时间 */	
}Pcb;

typedef struct task
{
	p_func eip_pointer;
	int sp_offset;
	int count;
	char name[16];
	int tty_num;
}Task;

/* PCB偏移量 */
#define STACK_BASE		0
#define GS_OFFSET		0
#define	FS_OFFSET		4
#define DS_OFFSET		8
#define	ES_OFFSET		12
#define EDI_OFFSET		16
#define ESI_OFFSET		20
#define EBP_OFFSET		24
#define KERNELESP_OFFSET	28
#define EBX_OFFSET		32
#define EDX_OFFSET		36
#define ECX_OFFSET		40
#define EAX_OFFSET		44
#define RETADDR_OFFSET		48
#define EIP_OFFSET		52
#define ECS_OFFSET		56
#define EFLAGS_OFFSET		60
#define ESP_OFFSET		64
#define ESS_OFFSET		68
#define STACK_TOP		(ESS_OFFSET + 4)
#define LDT_SEL			STACK_TOP 

/* 进程表可容纳的最大进程数 */
#define PROC_MAX		20
#define TASK_MAX		10

/* 定义进程和任务数量 */
#define NR_TASK			3
#define NR_PROC			0

/* 定义每个任务的堆栈大小 */
#define STACK_SIZE_A 		2000
#define STACK_SIZE_B		2000
#define STACK_SIZE_C		2000

#define STACK_SIZE_TOTAL	(STACK_SIZE_A + STACK_SIZE_B + STACK_SIZE_C)

extern Pcb proc_table[];
extern Task task_table[];
extern char task_stack[];
extern Pcb *proc_ready;
extern int task_count;

typedef struct TASKTABLE{
	Task tableContent[TASK_MAX];
	int count;
}TaskTable;

extern TaskTable taskTable;

typedef struct RUNNING_QUEUE
{
	Pcb *queue[PROC_MAX];
	int length;
}RunningQueue;

/* 运行进程队列 */
extern RunningQueue* running_queue;

typedef struct INTERRUPT_TABLE
{
	Pcb *queue[PROC_MAX];
	int length;
}InterruptQueue;

/* 可中断等待进程队列 */
extern InterruptQueue *interrupt_queue;


typedef struct UNINTERRUPT_TABLE
{
	Pcb *queue[PROC_MAX];
	int length;
}UnInterruptQueue;

/* 不可中断等待进程队列 */
extern UnInterruptQueue *uninterrupt_queue;
#endif
