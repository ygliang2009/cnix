#include "const.h"
#include "global.h"
#include "signal.h"
#include "proc.h"
#include "tty.h"
#include "block.h"
#include "proto.h"
#include "time.h"

Tss tss;

int disPos = 0;

//int countInt = -1;

Pcb proc_table[PROC_MAX];

Descriptor gdtSeg[GDT_SIZE];

char gdtPtr[6];

Gate idtSeg[IDT_SIZE];

char idtPtr[6];

/*Task task_table[TASK_MAX] = {
		{SysProcA,STACK_SIZE_A,"sysProcA"},
		{SysProcB,STACK_SIZE_B,"sysProcB"},
		{SysProcC,STACK_SIZE_C,"sysProcC"}
	};
*/
TaskTable taskTable;

int task_count = 0;

char task_stack[STACK_SIZE_TOTAL];

Pcb *proc_ready = 0;

/* 系统调用表 */
SysBlock sysBlock;
void* syscall_table[128] = {syscall_test,syscall_write,sys_pause,sys_getpid,sys_nice,sys_getsignal,sys_getblock};

/* 键盘控制块 */
Kcb kcb;
Kcb tty[NR_TTY];
Kcb *cur_tty;

/* shift键是否按下*/
int is_shift = 0;
/* alt键是否按下 */
int is_alt = 0;

/* 系统时间 */
Time * ktime;

/* 系统开机初始时间 */
long start_time = 0;

/* 运行进程队列 */
RunningQueue *running_queue;
/* 可中断等待进程队列 */
InterruptQueue *interrupt_queue;
/* 不可中断等待进程队列 */
UnInterruptQueue *uninterrupt_queue;

/* 硬盘个数 */
int hd_num = 0;
Request request[QUEUE_NUM];

/* 块设备表，每个设备占有一项，共7项 */
Block_dev_struct block_dev[NR_BLOCK];

/* 硬盘存储信息 */
DriveInfo hd_info[] = {{0,0,0,0,0,0},{0,0,0,0,0,0}};

