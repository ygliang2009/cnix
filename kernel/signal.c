#include "cnix.h"

int default_sig_oper[] = 
{
	SIGHUP, 	TERMINATE,
	SIGINT,		TERMINATE,
	SIGQUIT,	DUMP,
	SIGILL,		DUMP,
	SIGTRAP,	DUMP,
	SIGABRT,	DUMP,
	SIGBUS,		DUMP,
	SIGFPE,		DUMP,
	SIGKILL,	TERMINATE,
	SIGUSR1,	TERMINATE,
	SIGSEGV,	DUMP,
	SIGUSR2,	TERMINATE,
	SIGPIPE,	TERMINATE,
	SIGALRM,	TERMINATE,
	SIGTERM,	TERMINATE,
	SIGCHLD,	IGNORE,
	SIGCONT,	CONTINUE,
	SIGSTOP,	STOP,
	SIGTSTP,	STOP,
	SIGTTIN,	STOP,
	SIGTTOU,	STOP,
	SIGURG,		IGNORE,
	SIGXCPU,	DUMP,
	SIGXFSZ,	DUMP,
	SIGVTALRM,	TERMINATE,
	SIGPROF,	TERMINATE,
	SIGSYS,		DUMP,
	SIGPWR,		DUMP
};

/* C函数的调用是不破坏堆栈结构的 */
int do_signal(int sig_nr)
{
	Sigaction *act = &proc_ready->sigaction[sig_nr];
	if(act->sig_handler == SIG_DEF)
	{
		/* 如果没有定义信号处理函数，则调用默认处理函数 */
		int oper_code = default_sig_oper[(act->sig_num - 1) * 2];
		switch(oper_code)
		{
			/* TERMINATE */
			case 100:
				terminate_handler();
				break;
			/* DUMP */
			case 101:
				dump_handler();
				break;
			/* STOP */
			case 102:
				stop_handler();
				break;
			/* CONTINUE */
			case 103:
				continue_handler();
				break;
			/* IGNORE */
			case 104:
				break;
			default:
				break;
		}//end switch	
	}else{
	/* 如果不用默认的处理方式，则调用sigaction->sig_handler方法 */
		/* 忽略该信号 */
		if(act->sig_handler == SIG_IGN)
		{
			return DONE;			
		}
		if(act->sig_handler)
		{
			(*(act->sig_handler))();
		}	
	}
	return DONE;
}

int sys_sigaction(int sig_num, Sigaction *sig_obj, Sigaction *old_obj)
{
	/* 信号量需要在1和32之间 */
	if(sig_num < 1 || sig_num > 32)
	{
		return -NO_SUCH_SIG; 
	}else if(sig_num == SIGKILL || sig_num == SIGSTOP)
	{
		return -SIG_PRO_ERR;
	}

	old_obj = &proc_ready->sigaction[sig_num - 1];
	proc_ready->sigaction[sig_num - 1] = *sig_obj;
	return DONE;
}

void stop_handler()
{
	printk("stop handler");
}

void dump_handler()
{
	printk("dump handler");
}

void terminate_handler()
{
	printk("terminate handler");
}

void continue_handler()
{
	printk("continue handler");
}
