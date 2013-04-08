/*********************************************************************
	@Author:Chemical Liang	@Version:1.0	@Date:2011-10-26
**********************************************************************/
.section .text
.code32
.global str_cpy
.global mem_cpy
.global memSet
.global read_port
.global write_port
.global int_open
.global int_close
.global sti
.global cli

/*******************************************************************
	void mem_cpy(void *dest, void *src, int length)
********************************************************************/
mem_cpy:
	pushl %ebp
	movl %esp,%ebp
	pushl %edi
	pushl %esi
	pushl %ecx
	xor %ecx,%ecx
	movl 8(%ebp),%edi
	movl 12(%ebp),%esi
	movl 16(%ebp),%ecx
memCopyLoop:
	cmpl $0,%ecx
	jz memCopyEnd
	movb %ds:(%esi),%al
	movb %al,%es:(%edi)
	inc %esi
	inc %edi
	decl %ecx
	/* 这里使用loop会报错 */
#	loop memCopyLoop
	jmp memCopyLoop
memCopyEnd:
	/* 这里需要把地址返回给eax */
	movl 8(%ebp),%eax
	popl %ecx
	popl %esi
	popl %edi
	movl %ebp,%esp
	popl %ebp	
	ret

/*********************************************************************
	void str_cpy(char *dest,char *src)
**********************************************************************/
str_cpy:
	pushl %ebp
	movl %esp,%ebp
	pushl %edi
	pushl %esi
	movl 8(%ebp),%edi
	movl 12(%ebp),%esi
char_cpy:
	movb %ds:(%esi),%al
	movb %al,%es:(%edi)
	cmpb $0,%al
	jz cpy_end
	inc %esi
	inc %edi
	jmp char_cpy
cpy_end:	
	popl %esi
	popl %edi
	movl %ebp,%esp
	popl %ebp
	ret

/********************************************************************
	void memSet(char *str,int num,int length)
*********************************************************************/
memSet:
	pushl %ebp
	movl %esp,%ebp
	pushl %eax
	pushl %ecx
	pushl %edi
	xor %eax,%eax
	xor %ecx,%ecx
	xor %edi,%edi
	movl 16(%ebp),%edi
	movl 12(%ebp),%eax
	movl 8(%ebp),%ecx
store_data:
	cmpl $0,%ecx
	jz return_memset
	movb %al,%es:(%edi)
	inc %edi
	dec %ecx
	jmp store_data
return_memset:
	popl %edi
	popl %ecx
	popl %eax
	movl %ebp,%esp
	popl %ebp
	ret

/*************************************************************************
	从指定端口中读一个字节数据: int read_port(int portNum)
**************************************************************************/
read_port:
	pushl %ebp
	movl %esp,%ebp
	/* 端口号直接寻址方式必须存储在%edx中 */
	pushl %edx
	xor %eax,%eax
	movw 8(%ebp),%dx
	inb %dx,%al
	nop
	popl %edx
	movl %ebp,%esp
	popl %ebp
	ret

/**************************************************************************
	向指定端口写一个字节数据: void write_port(int portNum,int value);
***************************************************************************/
write_port:
	pushl %ebp
	movl %esp,%ebp
	/* 端口号直接寻址方式必须存储在%edx中 */
	pushl %edx
	movb 12(%ebp),%al
	movw 8(%ebp),%dx
	outb %al,%dx
	popl %edx
	movl %ebp,%esp
	popl %ebp
	ret

/*****************************************************************************
		开启||屏蔽中断
******************************************************************************/
int_open:
	nop
	sti
	ret

int_close:
	nop
	cli
	ret

/*****************************************************************************
		开启||屏蔽中断(新接口)
******************************************************************************/
sti:
	nop
	sti
	ret

cli:
	nop
	cli
	ret
