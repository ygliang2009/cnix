#include "const.h"
#include "global.h"
#include "signal.h"
#include "proc.h"
#include "tty.h"
#include "block.h"
#include "proto.h"

void cstart()
{
	prints("\n\n\n\n\n\n\n\n--------cstart kernel start--------");
       	/* 初始化内核GDT,注意应用形式 */
	mem_cpy(&gdtSeg,(void *)(*(int *)(&gdtPtr[2])),(*(short *)(&gdtPtr[0]))+1);
	short *gdt_len_ptr = (short *)(&gdtPtr[0]);
	*gdt_len_ptr = GDT_SIZE * sizeof(Descriptor) - 1;
	int *gdt_addr_ptr = (int *)(&gdtPtr[2]);
	*gdt_addr_ptr = (int)&gdtSeg;
	init_idt_ptr();
     	init_idt_seg();
	update_gdt();
	prints("\n--------cstart kernel end--------\n"); 
}

