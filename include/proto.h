#ifndef _PROTO_H
#define _PROTO_H
/* idt的初始化 */
void init_idt_seg();
void init_idt_ptr();

/* 通用的中断调用函数*/
void common_handle();
void common_hwint();

/* 键盘控制块函数*/
void init_kcb(Kcb,char*);
void reset_kcb(Kcb,char*);

/* 键盘中断处理函数 */
void keyboard_handler();
/* 键盘控制输出函数 */
void disp_key();

/* 温盘中断处理函数 */
void ata_handler();

/* 中断控制函数 */
void init_tty();
void reset_tty();
void select_tty();
int is_current_tty(Kcb *);

/* 时钟中断处理函数 */
void clock_handler();

/* 更新GDT表 */
void update_gdt();

void delay();
void TestA();
void TestB();
void TestC();

/* 调试用的函数 */
void var_dump(int,int);

/* 时间延迟函数 */
void timedelay(int);

/* 系统进程 */
void task_hd();			/* 硬盘任务初始化 */
void keyboard_main();		/* 键盘任务初始化 */
void SysProcB();

/* 初始化任务表 */
void init_task();

/* C中断处理函数 */
void cclock_handler();

/* 软中断处理程序 */
void divide_exception_handler();
void debug_exception_handler();
void nmi_exception_handler();
void breakpoint_exception_handler();
void overflow_exception_handler();
void bounds_exception_handler();
void inval_op_exception_handler();
void coproc_not_exception_handler();
void double_fault_exception_handler();
void coproc_seg_exception_handler();
void inval_tss_exception_handler();
void seg_not_exception_handler();
void stack_fault_exception_handler();
void protection_exception_handler();
void page_fault_exception_handler();
void coproc_error_exception_handler();

/* 系统调用 */
int syscall_test();
void syscall_handler();

/* WRITE 系统调用 */
int syscall_write(char *,int,int,Pcb *);
void write(char *,int);

/* 清屏函数 */
void clear_screen();

/* 进程调度程序 */
void schedule();

/* 读写端口控制 */
int read_port(int);
void write_port(int,int);

/* IRQ中断控制 */
void enable_irq(int);
void disable_irq(int);

/* 显示一个整数 */
void printi(long);

/* 显示一个字符 */
void printc(char);

/* 开启屏蔽中断函数 */
void int_open();
void int_close();
/* 开启屏蔽中断函数新接口 */
void sti();
void cli();

/* 将光标移动到特定的位置 */
void cursor_mov(int);
/* 将屏幕移动到特定位置 */
void screen_mov(int);
/* 在指定屏幕上打印一个字符 */
void print_dest_char(Kcb*,char);
/* 在指定的屏幕上打印一个变长字符串 */
void prtvstr(Kcb *,char *);
/* 在指定的屏幕上打印一个定长字符串 */
void prtstr(Kcb *,char *,int);
/* 检验进程的信号标志位，处理相应的信号处理函数 */
void signal_handler();

/* 信号预处理函数 */
int do_signal(int);
int sys_sigaction(int,Sigaction *,Sigaction *);

/* 插入与移除进程运行队列中的进程 */
int remove_running_queue(Pcb *);
int insert_running_queue(Pcb *);
/* 插入与移除可中断睡眠进程队列中的进程 */
int remove_interrupt_queue(Pcb *);
int insert_interrupt_queue(Pcb *);
/* 插入与移除可不可中断睡眠进程队列中的进程 */
int remove_uninterrupt_queue(Pcb *);
int insert_uninterrupt_queue(Pcb *);

/* 进程进入睡眠状态 */
void sleep_on(Pcb *);
void interrupt_sleep_on(Pcb *);
/* 唤醒进程 */
void wake_up(Pcb *);

/* pause系统调用 */
int sys_pause(void);
int pause(void);

/* getpid系统调用:获取当前进程的PID系统调用 */
int sys_getpid(void);
int getpid(void);

/* nice系统调用:调整当前进程的优先级 */
int sys_nice(int);
int nice(int);

/* 获取当前的信号位图和阻塞信号位图 */
int getsignal(void);
int sys_getsignal(void);
int getblock(void);
int sys_getblock(void);

/* 信号默认STOP操作处理函数 */
void stop_handler();
/* 信号默认DUMP操作处理函数 */
void dump_handler();
/* 信号默认TERMINATE操作处理函数 */
void terminate_handler();
/* 信号默认CONTINUE操作处理函数 */
void continue_handler();

/* 内核write方法 */
int kernel_write(char*,int);

/* 内核打印函数 */
int printk(char *format, ...);
/* 系统出错的dump信息 */
void panic(const char *);

/* 硬盘初始化 */
int init_hd();
/* 读参数命令 */
void do_hdcmd(hd_cmd *);
/* 打印硬盘信息 */
void hd_identity(int);
#endif
