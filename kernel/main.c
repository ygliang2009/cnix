#include "const.h"
#include "global.h"
#include "tty.h"
#include "signal.h"
#include "proc.h"
#include "block.h"
#include "proto.h"
#include "time.h"

static void print_starttime()
{
	prints("#### Cnix OS Has Been Started  ####\n");
	prints("Time:");
	printi(ktime->year);
	prints("-");
	printi(ktime->mon);
	prints("-");
	printi(ktime->mday);
	prints(" ");
	printi(ktime->hour);
	prints(":");
	printi(ktime->min);
	prints(":");
	printi(ktime->sec);
}

void kernel_main()
{
	prints("\n------ kernel_main start -------\n\n");
	/* 初始化任务表 */
	init_task();
	/* 初始化进程表 */
	init_proc();
	/* 初始化系统调用表 */
	//init_syscall();
	/* 初始化CMOS时间 */
	start_time = init_time();
	/* 打印开机时间*/
	print_starttime();
	/* 中断嵌套控制变量 */
	countInt = -1;
	/* 开启时钟中断 */
	enable_irq(0);
	/* 开启键盘中断 */
	enable_irq(1);
	/* 初始化键盘控制块 */
	prints("\n\n");
	/* 初始化终端设备 */
	init_tty();
	
	/* 转去加载ldt，执行用户态进程 */
	restart();

	while(1)
	{
	}	
}

