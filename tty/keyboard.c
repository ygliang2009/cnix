/*********************************************************************************
 		@Author:Chemical Liang	@Version:1.0	@Date:2011-11-10
		@Desc:键盘控制块操作相关函数
 *********************************************************************************/
#include "cnix.h"
#include "proto.h"

/************************************************************
 		键盘控制块初始化
 ************************************************************/
void init_kcb(Kcb tty_obj,char *name)
{
	int i;
	str_cpy(tty_obj.name,name);
	for(; i < NR_KEYBUF; i++)
	{
		tty_obj.key_buf[i] = 0;
	}
	tty_obj.read_count = 0;
	tty_obj.write_count = 0;
}

/*************************************************************
 		键盘控制块重置
 *************************************************************/
void reset_kcb(Kcb tty_obj,char *name)
{
	int i;
	str_cpy(tty_obj.name,name);
	for(; i<NR_KEYBUF; i++)
	{
		tty_obj.key_buf[i] = 0;
	}
	tty_obj.read_count = 0;
	tty_obj.write_count = 0;
}

/********************************************************************************
 *     键盘中断 
  键盘中存在一个叫做键盘编码器（Intel 8048系列兼容）的芯片，主板中存在一个键盘
  控制器，键盘的功能是由键盘编码器-8259A-键盘控制器之间的通信
 ********************************************************************************/

void ckeyboard_handler(int irq)
{
	/* 读0x60端口,将读出的用户输入写到缓冲区中，缓冲区超出界限则指针回到首部 */
	/* read_port 返回的是一个字节的数据 */
	char res = read_port(0x60);
	/* 指针的优先级高于++ */
	cur_tty->key_buf[cur_tty->write_count++] = res;
	cur_tty->left_count++;

	if(cur_tty->write_count == NR_KEYBUF){
		cur_tty->write_count = 0;
	}
}
