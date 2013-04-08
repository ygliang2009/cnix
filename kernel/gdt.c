#include "const.h"
#include "global.h"
#include "tty.h"
#include "signal.h"
#include "proc.h"
#include "block.h"
#include "proto.h"

#define vir2phys(seg_base, vir) (int)(((int)seg_base) + (int)(vir))

int seg2phys(short seg);

extern Pcb proc_table[];

void init_descriptor(Descriptor * p_desc,int base,int limit,short attribute)
{
	p_desc->limit_low = limit & 0x0FFFF;
	p_desc->base_low = base & 0x0FFFF; 
	p_desc->base_mid = (base >> 16) & 0x0FF; 
	p_desc->attr1 = attribute & 0xFF;       
	p_desc->limit_high_attr2 = ((limit >> 16) & 0x0F) |(attribute >> 8) & 0xF0;
	p_desc->base_high = (base >> 24) & 0x0FF;
}   

void update_gdt()
{
	/* TSS设置这块还需要多关注一下 */
	
	/* 填充 GDT 中 TSS 这个描述符 */
	tss.ss0 = SELECTOR_DATA;
	init_descriptor(&gdtSeg[INDEX_TSS],
		vir2phys(seg2phys(SELECTOR_DATA), &tss),
		sizeof(tss) - 1,
		DA_386TSS);
	tss.iobase      = sizeof(tss);  /* 没有I/O许可位图 */

	// 填充 GDT 中进程的 LDT 的描述符
	int i = 0;
	int index_ldt = INDEX_LDT;
	while(i < NR_TASK){
		init_descriptor(&gdtSeg[index_ldt],vir2phys(seg2phys(SELECTOR_DATA), proc_table[i].ldts),LDT_SIZE * sizeof(Descriptor) - 1,DA_LDT);
		index_ldt++;
		i++;
	}

}


int seg2phys(short seg)
{
	Descriptor* p_dest = &gdtSeg[seg >> 3];
	return (p_dest->base_high << 24) | (p_dest->base_mid << 16) | (p_dest->base_low);
}  
