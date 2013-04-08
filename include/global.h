#ifndef _GLOBAL_H
#define _GLOBAL_H

/* 描述符定义 */
typedef struct descriptor
{
	short limit_low;
	short base_low;
	char base_mid;
	char attr1;
	char limit_high_attr2;
	char base_high;	
}Descriptor;

/* 描述符表 */
extern Descriptor gdtSeg[];

/* 指向描述符的指针 */
extern char gdtPtr[];

/* 定义门描述符 */
typedef struct gate
{
	short offset1;
	short selector;
	char dcount;
	char attr;
	short offset2;
}Gate;

/* idt描述符段 */
extern Gate idtSeg[];

/* idt结构指针 */
extern char idtPtr[];

/* 指向中断处理函数的指针*/
typedef void (*int_handler)();

/* TSS结构描述符 */
typedef struct s_tss
{
	int backlink;
	int esp0;
	int ss0;
	int esp1;
	int ss1;
	int esp2;
	int ss2;
	int cr3;
	int eip;
	int flags;
	int eax;
	int ecx;
	int edx;
	int ebx;
	int esp;
	int ebp;
	int esi;
	int edi;
	int es;
	int cs;
	int ss;
	int ds;
	int fs;
	int gs;
	int ldt;
	short trap;
	short iobase;
}Tss;

extern Tss tss;

int countInt;

extern int disPos;

typedef void* SysCall;

extern SysCall syscall_table[];

typedef struct SYSBLOCK
{
	SysCall syscall_table[NR_SYSCALL];
	int count;
}SysBlock;

extern SysBlock sysBlock;

extern long start_time;
#endif
