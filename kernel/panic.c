/**************************************************************************************
 *				panic.c
 *    打印内核的警告信息，这里效仿linux，将panic函数单独置于一个文件中
 *
 *    @Author:Chemical Liang	@Version:1.0	@Date:2011-11-XX
 **************************************************************************************/
#include "cnix.h"


/* 系统出错的dump信息 */
void panic(const char *s)
{
	printk("kernel panic:\n",s);
	for(;;);	
}
