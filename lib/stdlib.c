#include "const.h"
#include "type.h"
#include "stdlib.h"
#include "string.h"

/************************************************************************************
  			这里实现C标准库中的函数
 		@Author:Chemical Liang 	@Version:1.0 	@Date:2011-11-17
 ************************************************************************************/


/**********************************************************************
	itoa函数，在C扩展库中定义 
**********************************************************************/
void itoa(long num,char *res,int radix)
{
	int i = 0,j = 0;
	char index[] = "0123456789ABCDEF";
	long tmp_num;
	char tmp_array[32];

	if(radix != 10 && radix != 8 && radix != 2 && radix != 16)
		return;
	if(radix == 16)
	{
		/* 如果是16进制数就采用"0x+num"的形式输出 */
		res[i++] = '0';
		res[i++] = 'x';
	}else if(radix == 2){
		/* 如果是2进制数就采用"0b+num"的形式输出 */
		res[i++] = '0';
		res[i++] = 'b';
	}else if(radix == 8){
		/* 如果是8进制数就采用"0+num"的形式输出 */
		res[i++] = '0';
	}
	tmp_num = num;
	if(num < 0 && radix == 10){
		res[i++] = '-';
		tmp_num = -num;
	}
	do{
		tmp_array[j++] = index[tmp_num%radix];
		tmp_num = tmp_num/radix;
	}while(tmp_num);

	/* 倒排除法取余结果 */
	j--;
	while(j>=0)
	{
		res[i++] = tmp_array[j--];
	}
	res[i] = '\0';
}

/*******************************************************************************
		atoi(char **str)  C标准库中定义，输入参数为指向指针
		的指针，返回参数为转换的整数
********************************************************************************/
int atoi(char **str_p)
{
	int i = 10;
	int res = 0;
	while(is_digit(**str_p))
	{
		int offset = (**str_p) - '0';
		res = offset + res*i;
		(*str_p)++;
	}	
	return res;	
}

/*************************************************************************************
		printf函数的实现
	注意，可变参数个数实现方案在GCC的默认编译方式下无法实现，因为默认方式下
	GCC做出优化(默认优化为O3)破坏了该实现方案，所以需要在编译时降低优化级别，
	gcc -c stdlib.c -O2 或O2以下
    ##也许确实是编译器的问题，换成VC和DEV下的GCC均是可行的，可是在Ubuntu下#####
    ##	   自带的GCC中，该函数确实需要减低优化以后才可以编译通过	  #####	
 *************************************************************************************/
int printf(char *format, ...)
{
	int j = 0;
	char buf[128];
	char *arg_list = (char *)((char *)(&format) + 4);
	j = vsprintf(buf,format,arg_list);
	write(buf,j);
	return j;	
}

/***************************************************************************
 			内核态下的打印函数
 ***************************************************************************/
int printk(char *format, ...)
{
	int j = 0;
	char buf[128];
	char *arg_list = (char *)((char *)(&format) + 4);
	j = vsprintf(buf,format,arg_list);
	kernel_write(buf,j);
	return j;
}

/*********************************************************************************************************
 			将指定的参数串传递给指定的缓冲区数组中
	char *buf: 存入格式化的字符数组，这里设定其缓冲区长度为128个字节
	char *format  存有格式化字符，如：name:%s
	char *arg_list  指向函数参数的指针集合，其实是堆栈地址的集合
		附：printf入栈时的栈结构，arg_list此时为各个地址参数的集合
				
					|			|			
 入栈方向	∧	低地址	|	|   printf函数地址入口  |	<- esp
		||		|	|_______________________|    	format 地址 <- esp + 4
		||		|	|   format 参数地址	|
		||		|	|_______________________|	arg_list 地址 <- &format + 4
		||		|	|   (int) arg_list[0]	|	
		||		|	|_______________________|	
		||		|	|   (int) arg_list[1]	|
		||		|	|_______________________|
		||		|	|   (int) arg_list[2]	|
		||	高地址	|	|_______________________|
		||		V	|   ..................	|	#C语言中函数入栈顺序是由右向左#
					|			|

 ************************************************************************************************************/
int vsprintf(char *buf, char *format, char *arg_list)
{
	char tmp[128];
	int arg_int;
	char *arg_p = arg_list;
	char *buf_p = buf;
	char *format_p = format;
	char *arg_str = NULL;
	while(*format_p)
	{
		if(*format_p != '%'){
			*buf_p = *format_p;
			buf_p ++;
			format_p ++;
			continue;
		}
		/* 遇到了'%'*/
		format_p ++;
		
		if(is_digit(*format_p))
		{
			/* 宽度限定数 */	
		}else if(is_dot(*format_p))
		{
			format_p ++;
			if(is_digit(*format_p))
			{
				/* 精度限定数 */
			}	
		}
		switch(*format_p)
		{	
			/* 以10进制整型输出 */
			case 'd':
			    /* 取出参数值 */
			    arg_int = (*(int *)arg_p);
			    itoa(arg_int,tmp,10);
			    strcpy(buf_p,tmp);
			    arg_p += 4; 
			    buf_p += strlen(tmp);
			    break;
			    /* 以字符串形式输出 */
			case 's':
			    arg_str = (*(char **)arg_p);
			    strcpy(buf_p,arg_str);
			    arg_p += 4;
			    buf_p += strlen(arg_str);
			    break;
			/* 以16进制输出 */
			case 'x':
			    /* 取出参数值 */
			    arg_int = (*(int *)arg_p);
			    itoa(arg_int,tmp,16);
			    strcpy(buf_p,tmp);
			    /* arg_list 里面存储的是参数地址，每个地址为指针类型为4个字节 */
			    arg_p += 4; 
			    buf_p += strlen(tmp);
			    break;
		}
		format_p++;
	}
	/* 返回vsprintf格式化字符串后的buf长度，以便write系统调用使用 */
	return (buf_p - buf);	
}
