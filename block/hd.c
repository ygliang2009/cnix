/********************************************************************************
 * 
 *			硬盘驱动程序
 *	
 *	@Author:Chemical Liang	@Version:1.0	@Date:2011-11-XX
 ********************************************************************************/
/* 硬盘的主设备号为3 */
#include "const.h"
#include "global.h"
#include "signal.h"
#include "proc.h"
#include "tty.h"
#include "time.h"
#define		MAJOR_NR	3
#include "block.h"
#include "proto.h"
/* 从指定的端口中读取一个字节 */
static inline char inb(int port_nr)
{
	char m;
	__asm__ __volatile__(
		"inb %%dx,%%al\n\t"
		"nop\n\t"
		"nop\n\t"
		:"=a"(m):"d"(port_nr)
		);
	return m;
}

/* 向指定的端口中写一个字节 */
static inline void outb(int port_nr, char cont)
{
	__asm__ __volatile__(
		"outb %%al,%%dx\n\t"
		"nop\n\t"
		"nop\n\t"
		::"d"(port_nr),"a"(cont)
		);
}

/* 从指定的端口中读取num*2个字节 */
static inline void read_buf(int port_nr,char *buf,int num)
{
	__asm__ __volatile__(
		"cld\n\t"
		"rep\n\t"
		"insw\n\t"
		::"d"(port_nr),"D"(buf),"c"(num)
		);
}

/* 向指定的端口中写入num*2个字节*/
static inline void write_buf(int port_nr,char *buf,int num)
{
	__asm__ __volatile(
		"cld\n\t"
		"rep\n\t"
		"outw\n\t"
		::"d"(port_nr),"S"(buf),"c"(num)
		);
}

/* 检测控制器的空闲状态 */
static int controller_ready()
{
	int retries = 1000000;
	while(retries--)
	{
		/* 通过读主状态寄存器，判断HD_STATUS是BUSY_STAT还是READY_STAT */
		/* 从而判断寄存器的忙闲状态 				      */
		if((inb(HD_STATUS) & 0x80) != BUSY_STAT)
			break;	
	}
	return retries;
}

/* 等待控制器就绪 */
static int drive_busy()
{
	unsigned int i = 1000000;
	unsigned char c;
	c = inb(HD_STATUS);
	while(i--){
		if(c & SEEK_STAT & READY_STAT & BUSY_STAT == SEEK_STAT|READY_STAT)	
			break;
	}
	if(i == 0)
		return 0;
	else 
		return 1;
}

int init_hd()
{
	static int i = 1;
	int drive;
	/* 从0x20080地址处获取在loader.s向该地址填充的硬盘基本信息 */
	int hd_tmp = HD_ADDR;
	int hd_d = 0;
	for(drive = 0; drive < 2; drive++)
	{
		hd_info[drive].cyl = *(unsigned short *)(hd_tmp);
		hd_info[drive].head = *(unsigned char *)(2 + hd_tmp);
		hd_info[drive].wpcom = *(unsigned short *)(5 + hd_tmp);
		hd_info[drive].ctl = *(unsigned char *)(8 + hd_tmp);
		hd_info[drive].lzone = *(unsigned short *)(12 + hd_tmp);
		hd_info[drive].sect = *(unsigned char *)(14 + hd_tmp);
		hd_tmp += 16;
	}
	if(hd_info[0].cyl == 0)
	{
		hd_d = 1;
	}else{
		hd_d = 2;
	}
	/* 在0x475地址处获取硬盘数量信息，首要条件是虚拟地址和线性地址相等 */
	char *driver_num = (char *)(HD_INFO);
	/* 赋值给全局变量hd_num */
	hd_num = (int)(*driver_num);
	printk("driver_num is %d",hd_d);
	printk("driver_cyl is %d",hd_info[0].cyl);
	printk("  driver_head is %d",hd_info[0].head);
	printk("  driver_wpcom is %d",hd_info[0].wpcom);
	printk("  driver_ctrl is %d",hd_info[0].ctl);
	return 0;	
}

void do_hdcmd(hd_cmd *cmd)
{
	/* 依次向磁盘中写端口 */
	if(drive_busy())
	{
		panic("hd error");
	}
	outb(HD_CMD,(char)0);
	outb(HD_PRECOMP,cmd->features);
	outb(HD_NSECTOR,cmd->count);
	outb(HD_LCYL,cmd->ly_low);
	outb(HD_MCYL,cmd->ly_mid);
	outb(HD_HCYL,cmd->ly_high);
	outb(HD_DEVICE,cmd->device);
	outb(HD_COMMAND,cmd->command);
}

/***************************************************************
 			获取磁盘信息
 ***************************************************************/
void hd_identity(int drive)
{
	char buf[1000];
	char r;
	int i = 0;
	hd_cmd *cmd;
	cmd->device = MAKE_DEVICE_REG(0,drive,0);
       	cmd->command = ATA_IDENTITY;
	do_hdcmd(cmd);
	for(;i<10000&&!(r=inb(HD_STATUS)&DRQ_STAT);i++);
	if(!r)
	{
		return;
	}
	read_buf(HD_DATA,buf,SECTOR_SIZE);
	printf("hsss:%s",buf);	
}

void cata_handler()
{
	return;
}
