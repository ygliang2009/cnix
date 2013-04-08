/*********************************************************************************************
		Cnix操作系统定义的信号借鉴于Linux信号，默认操作也保持一致
  		@Author:Chemical Liang	@Version:1.0	@Date:2011-11-23 
 *********************************************************************************************/
#ifndef _SIGNAL_H
#define	_SIGNAL

#define 	SIGHUP		1		/* 挂起控制终端或进程 */
#define		SIGINT		2		/* 来自键盘的中断 */
#define		SIGQUIT		3		/* 从键盘退出 */
#define		SIGILL		4		/* 非法指令 */
#define		SIGTRAP		5		/* 跟踪的断点 */
//本操作系统废弃"SIGIOT"信号不使用
#define		SIGABRT		6		/* 异常结束 */
#define		SIGBUS		7		/* 总线错误 */
#define		SIGFPE		8		/* 浮点异常 */
#define		SIGKILL		9		/* 强迫进程退出 */
#define		SIGUSR1		10		/* 对进程可用 */
#define		SIGSEGV		11		/* 无效内存引用 */
#define		SIGUSR2		12		/* 对进程可用 */
#define		SIGPIPE		13		/* 对无读者的管道写 */
#define		SIGALRM		14		/* 实时定时器时钟 */
#define		SIGTERM		15		/* 进程终止 */
//本操作系统废弃对"SIGTERM"信号不使用
#define		SIGCHLD		16		/* 子进程停止，结束或在被跟踪时获得信号 */
#define		SIGCONT		17		/* 如果已停止则恢复进程执行 */
#define		SIGSTOP		18		/* 停止进程执行 */
#define		SIGTSTP		19		/* 从TTY发出停止进程 */
#define		SIGTTIN		20		/* 后台进程请求输入 */
#define		SIGTTOU		21		/* 后台进程请求输出 */
#define		SIGURG		22		/* 套接字上的紧急事件 */
#define		SIGXCPU		23		/* 超过CPU时限 */
#define		SIGXFSZ		24		/* 超过文件大小限制 */
#define		SIGVTALRM	25		/* 虚拟定时器时钟 */
#define		SIGPROF		26		/* 概况定时器时钟 */
#define		SIGSYS		27		/* 坏的系统调用 */
#define		SIGPWR		28		/* 电源供给失效 */


#define		TERMINATE	100
#define		DUMP		101
#define		STOP		102
#define		CONTINUE	103
#define		IGNORE		104

/* 把0,1,2强行转换成指向void *(int)型的函数指针 */
#define		SIG_DEF		(void (*)(int))1
#define		SIG_IGN		(void (*)(int))2

typedef  long 	set_t;

typedef struct SIGACTION
{
	int sig_num;
	int sig_flag;
	set_t sig_mask;
	void (*sig_handler)();
}Sigaction;

extern int default_sig_oper[];

#endif
