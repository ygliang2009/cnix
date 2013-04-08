/************************************************************************
		终端处理函数
 	@Author:Chemical Liang	@Version:1.0	@Date:2011-11-11
 ************************************************************************/
#include "cnix.h"
#include "type.h"
#include "stdlib.h"

static void print_current_char(char);

/*************************************************************************
 		将键盘缓冲区的字符经过处理显示在屏幕上
**************************************************************************/
void disp_key()
{
	int isBreak = 0;
	int cols = 0;
	int isFunc = 0;
	char output[2];

	if(cur_tty->left_count == 0)
		return;
	char res = cur_tty->key_buf[cur_tty->read_count++];
	if(cur_tty->read_count == NR_KEYBUF)
		cur_tty->read_count = 0;	
	cur_tty->left_count --;
	
	/* 这里需要把右边的整型常量强制转换成char类型，否则，左边的res强制扩展成整型，涉及到符号位随之移动 */
	
	/* 左[shift]的make和右[shift]的make码 */
	if(res == (char)0x2A || res == (char)0x36)
	{
		is_shift = 1;
		return;
	/* 左[shift]的break和右[shift]的break码 */
	}else if(res == (char)0xAA || res == (char)0xB6)
	{
		is_shift = 0;
		return;
	}else if(res == (char)0x38 || res == (char)0xB8)
	{
	/* [alt]的break和make码 */
		if(res == (char)0x38)
		{
			is_alt = 1;
		}else{
			is_alt = 0;
		}
		return;
	}else if(res == (char)0x1D || res == (char)0x9D)
	{
	/* [ctrl]的break码和make码，暂时不做处理，左右ctrl扫描码一致 */
		return;
	}else if(res == (char)0x0E)
	{
	/* [backspace]键实现退格键，只实现[break]的make码，不处理break码 */
		int cursor_pos = cur_tty->screen.cursor_pos;
		/* 不能退格到起始的$字符以外 */
		if(cursor_pos  > cur_tty->screen.cursor_start_pos){
			char *cur_pos = (char *)(cursor_pos * 2 + V_MEM_BASE);
			*(cur_pos - 2) = ' ';
			*(cur_pos - 1) = DEFAULT_CHAR_COLOR;
			cur_tty->screen.cursor_pos--;
			cursor_mov(cur_tty->screen.cursor_pos);
		}
		return;
	}else if(res == (char)0x1C)
	{
	/* 回车键按下 */
		if(cur_tty->screen.cursor_pos + SCREEN_WIDTH < cur_tty->screen.start_pos + cur_tty->screen.vmem_size)
		{
			/****************************************************************************
			   回车键应该指向下一行的起始位置，又由于在tty2,tty3...终端上start_pos并不是
			   以SCREEN_WIDTH对齐，所以需要取出start_pos相对于SCREEN_WIDTH的余数
			 ****************************************************************************/
			int cursor_pos = (cur_tty->screen.cursor_pos/SCREEN_WIDTH + 1) * SCREEN_WIDTH + cur_tty->screen.start_pos%SCREEN_WIDTH;
			cur_tty->screen.cursor_pos = cursor_pos;
			cursor_mov(cur_tty->screen.cursor_pos);
			/* 回车按下后屏幕需要跟着滚动 */
			if(cur_tty->screen.cursor_pos >= (cur_tty->screen.disp_pos + SCREEN_SIZE))
			{
				cur_tty->screen.disp_pos = cur_tty->screen.cursor_pos - SCREEN_SIZE + SCREEN_WIDTH;
				screen_mov(cur_tty->screen.disp_pos);
			}
		}
		return;
	}else if(res == (char)0x3B)
	{
	/* F1键按下 */
		if(is_alt == 1){
			select_tty(1);
		}
		return;
	}else if(res == (char)0x3C)
	{
	/* F2键按下 */
		if(is_alt == 1){
			select_tty(2);
		}
		return;
	}else if(res == (char)0x3D)
	{
	/* F3键按下 */
		if(is_alt == 1){
			select_tty(3);
		}
		return;
	}else if(res == (char)0x3E)
	{
	/* F4键按下 */
		if(is_alt == 1){
			select_tty(4);
		}
		return;
	}else if(res == (char)0x3F)
	{
	/* F5键按下 */
		if(is_alt == 1){
			select_tty(5);
		}
		return;
	}else if(res == (char)0x40)
	{
	/* F6键按下 */
		if(is_alt == 1){
			select_tty(6);
		}
		return;
	}else if(res == (char)0x41)
	{
	/* F7键按下 */
		if(is_alt == 1){
			select_tty(7);
		}
		return;
	}else if(res == (char)0x42)
	{
	/* F8键按下 */
		if(is_alt == 1){
			select_tty(8);
		}
		return;
	}else if(res == (char)0x43)
	{
	/* F9键按下 */
		if(is_alt == 1){
			select_tty(9);
		}
		return;
	}else if(res == (char)0x44)
	{
	/* F10键按下 */
		if(is_alt == 1){
			select_tty(10);
		}
		return;
	}
	else if(res == (char)0x48)
	{
	/* [up]键实现向上卷屏功能，只实现[up]的make码，不处理break码 */
		if(is_shift == 1){
			Screen *scr = &cur_tty->screen;
			int disp_pos = scr->disp_pos;
			if(disp_pos - SCREEN_WIDTH >= scr->start_pos)
			{
				scr->disp_pos = disp_pos - SCREEN_WIDTH;
				screen_mov(scr->disp_pos);	
			}else{
				return;
			}
		}
		return;
	}else if(res == (char)0x50)
	{
	/* [down]键实现向下卷屏功能，只实现[down]的make码，不处理break码 */
		if(is_shift == 1){
			Screen *scr = &cur_tty->screen;
			int disp_pos = scr->disp_pos;
			if(disp_pos + SCREEN_WIDTH <= scr->start_pos + scr->vmem_size)
			{
				scr->disp_pos = disp_pos + SCREEN_WIDTH;
				screen_mov(scr->disp_pos);
			}else{
				return;
			}
			
			return;
		}
		return;	
	}
	/* &的优先级小于 == 所以要加括号喔亲！*/
	isBreak = ((res&0x0080)!= 0)?1:0;
	cols = (is_shift == 1)?1:0;
	
	/* 如果是break中断则跳过不予处理 */	
	if(isBreak == 0)
	{
		int key_char = key_mapper[res * 2 + cols];
		if((key_char & DISKEYMASK) != 0)
		{
			/* 非打印字符，则直接返回 */
			return;
		}
		output[0] = key_mapper[res * 2 + cols];
		/* 使用新的打印函数 */
		print_current_char(output[0]);
	/********************************************************************
		 **旧的打印函数** 
		output[0] = key_mapper[res * 2 + cols];
		output[1] = '\0';
		prints(output);
		cursor_mov(disPos);
	********************************************************************/
	}else{
		return;
	}
}

/***********************************************************************************
 				初始化TTY终端
 ***********************************************************************************/
void init_tty()
{
 	int i = 0;
	int start_pos = 0;
	int vmem_size = 0;
	char name[6] = {'t','t','y'};
	/* 每一个终端的内存大小是整个大小比之TTY的总个数 */
	int v_memsize_inword = V_MEM_SIZE/2;	
	vmem_size = v_memsize_inword/NR_TTY;
	cur_tty = tty;
	for(; i < NR_TTY; i++)
	{
		char *tty_num = &name[4];
		char *tmp_num;
		itoa(i,tty_num,10);
		/*****************************************************************
			*******Kcb tty_obj = tty[i]; C语言的赋值是值拷贝*******
		******************************************************************/
		init_kcb(tty[i],name);

		/* 初始化每个终端的显示设备 */
		Screen *scr = &tty[i].screen;
		scr->start_pos = start_pos;
		scr->disp_pos = scr->start_pos;
		scr->vmem_size = vmem_size;
		start_pos += vmem_size;
		if(i == 0){
			scr->cursor_pos = disPos/2;
			prtvstr(&tty[i],"$root");
			/* 初始化光标起始位置 */
			scr->cursor_start_pos = scr->cursor_pos;
		}else{
			scr->cursor_pos = scr->disp_pos;
			prtvstr(&tty[i],"$root");
			/* 初始化光标起始位置 */
			scr->cursor_start_pos = scr->cursor_pos;
		}
	}
}


/***********************************************************************************
 				重置TTY终端
 ***********************************************************************************/
void reset_tty()
{
 	int i = 0;
	int start_pos = 0;
	int vmem_size = 0;
	char name[6] = {'t','t','y'};
	int v_memsize_inword = V_MEM_SIZE/2;	
	vmem_size = v_memsize_inword/NR_TTY;

	for(; i < NR_TTY; i++)
	{
		char *tty_num = &name[4];
		char *tmp_num;
		itoa(i,tty_num,10);
		reset_kcb(tty[i],name);

		Screen *scr = &tty[i].screen;
		scr->start_pos = start_pos;
		scr->disp_pos = scr->start_pos;
		scr->vmem_size = vmem_size;
		scr->cursor_pos = scr->start_pos;
		start_pos += vmem_size;
	}
	cur_tty = &tty[0];	
}

/*********************************************************************************
 		选择特定的终端设备作为当前控制台
 *********************************************************************************/
void select_tty(int tty_num)
{
	if(tty_num > NR_TTY)
		return;
	Kcb *dest_tty = &tty[tty_num-1];
	if(is_current_tty(dest_tty))
	{
		return;
	}
	cur_tty = dest_tty;
	screen_mov((cur_tty->screen).disp_pos);
	cursor_mov((cur_tty->screen).cursor_pos);
}

/**********************************************************************************
 		判断给定的TTY终端是否当前的TTY终端
 **********************************************************************************/
int is_current_tty(Kcb *tty)
{
	int res = (tty==cur_tty)?1:0;
	return res;
}

/*************************************************************************************
 		在当前屏幕(CUR_TTY)所在光标的位置处显示一个字符
**************************************************************************************/
static void print_current_char(char ch)
{
	Screen *scr = &cur_tty->screen;
	char *disp_pos = (char *)(scr->cursor_pos * 2 +  V_MEM_BASE);
	*disp_pos++ = ch;
	/* DEFAULT_CHAR_COLOR: 0x07 白底黑字 */
	*disp_pos++ = DEFAULT_CHAR_COLOR;
	scr->cursor_pos++;
	cursor_mov(scr->cursor_pos);		
}


/*************************************************************************************
 		在指定屏幕(CUR_TTY)所在光标的位置处显示一个字符
**************************************************************************************/
void print_dest_char(Kcb* kcb,char ch)
{
	Screen *scr = &kcb->screen;
	char *disp_pos = (char *)(scr->cursor_pos * 2 +  V_MEM_BASE);
	/* 处理回车 */
	if(ch == '\n')
	{
		*disp_pos = (*disp_pos/160 + 1) * 160;
	}
	else{
		*disp_pos++ = ch;
		/* DEFAULT_CHAR_COLOR: 0x07 白底黑字 */
		*disp_pos++ = DEFAULT_CHAR_COLOR;
		scr->cursor_pos++;
		cursor_mov(scr->cursor_pos);
	}		
}

/*************************************************************************************
 		在指定屏幕(CUR_TTY)所在光标的位置处显示一个字符串(变长)
**************************************************************************************/
void prtvstr(Kcb *kcb, char* str)
{
	char *p = str;
	while(*p){
		print_dest_char(kcb,*p);
		p++;
	}
}

/*************************************************************************************
 		在指定屏幕(CUR_TTY)所在光标的位置处显示一个字符串(定长)
**************************************************************************************/
void prtstr(Kcb *kcb, char* str,int len)
{
	char *p = str;
	int i = len;

	while(i > 0){
		print_dest_char(kcb,*p);
		p++;
		i--;
	}
}

int kernel_write(char *buf, int len)
{
	char *p = buf;
	prtstr(&tty[proc_ready->tty_num],buf,len);
	return 0;
}
