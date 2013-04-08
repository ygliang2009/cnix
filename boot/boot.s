/*********************************************************************
		引导程序：	一切从这里开始
	@Author:Chemical Liang	@Version:1.0	@Date:2011-10-14
**********************************************************************/
.file "boot.s"
.code16
.text
	jmp LABLE_START
/**********************************************************
	此nop不可缺少，否则mount出错
 **********************************************************/
	nop
.include "boot/include/fat12.h"

LABLE_START:
	mov %cs,%ax
	mov %ax,%ds
	mov %ax,%es
/*SP指针从这里开始*/
	mov $0x7c00,%sp

/*清屏*/
	mov $0x0600,%ax
	mov $0x0700,%bx
	mov $0,%cx
	mov $0x184f,%dx
	int $0x10
/*调用问候语*/
	mov $BootMessage,%ax
	mov $28,%cx
	mov $0x0402,%dx
	call DispStr

/***************************************************************************
  软盘的根目录从第19个扇区开始，共占用14个扇区 
****************************************************************************/
	xor %ah,%ah # \
	xor %di,%di #  |----软驱复位
	int $0x13   # /
/***************************************************************************
  这里需要注意一个BUG，RootStartNo必须被定义为.set或者.equ这样的形式，如果被
  定义为.byte 这样的形式，则下列指令中wSectorNo被赋值为RootStartNo的内存地址
  而非RootStartNo里面的数值
****************************************************************************/
	movw $RootStartNo,(wSectorNo)
	movw $RootLength,(wLength)	
CMP_ROOTLENGTH:	
	cmpw $0,(wLength)
	jz NO_LOADER
	decw (wLength)
	mov $BaseOfLoader,%ax
	mov %ax,%es
	mov $OffsetOfLoader,%bx
/***************************************************************************
  给定起始扇区ReadSector，调用13号中断，读取扇区中的数据，13号中断将结果放入
  ds:bx中，将数据移动到di中以便于字符比较
****************************************************************************/
	movw (wSectorNo),%ax
	movb $1,%cl
	call ReadSector
	mov $FileName,%si
	mov $OffsetOfLoader,%di
	cld
/***************************************************************************
  根目录中存有条目信息，对每个根目录的每个条目进行读取，每条条目占有32字节，
  文件信息存放在条目的最起始字节位置
****************************************************************************/
	mov $16,%dx
CMP_ONEITEM:
	test %dx,%dx
	jz CMP_ROOT
	dec %dx
	mov $0,%cx
	jmp CMP_ONELOOP	
/*如果所有的条目都已遍历完毕，则跳到下一个条目进行读取遍历*/
CMP_ROOT:
	incw (wSectorNo)
	jmp CMP_ROOTLENGTH
CMP_ONELOOP:
/***********************************************************
  遍历每一个条目，比较文件名是否为给定文件名，一个字符一个
  字符地进行比较，如果某字符不等，则跳到下一个条目上在做比较
  每一条目32字节，所以需要有and $0xe0,%di，add $0x20,%di操作
************************************************************/
	cmp $11,%cx
	je File_Found
	incw %cx
	lodsb 
	cmp %es:(%di),%al
	je CMP_NEXT
	andw $0xffe0,%di
	addw $0x0020,%di
	mov $FileName,%si
	jmp CMP_ONEITEM
CMP_NEXT:
	inc %di
	jmp CMP_ONELOOP	
STOP:
	jmp .
NO_LOADER:
	push %ax
	push %bx
	push %cx
	push %dx
	mov $NLStr,%ax
	mov %ax,%bp
	mov $0x1301,%ax
	mov $0x00c,%bx
	mov $0x0010,%dx
	mov $9,%cx
	int $0x10
	pop %dx
	pop %cx
	pop %bx
	pop %ax
	jmp STOP	
File_Found:
/******************************************************************************
  根目录区分布图
*******************************************************************************/
	and $0xFFE0,%di
	add $0x1A,%di
	mov %es:(%di),%cx
	push %cx
/******************************************************************************
        BPB区为磁盘的第一个扇区，占有系统的引导程序，FAT区共两个，分别是
 	FAT1和FAT2，每个FAT区为9个字节，见磁盘的 BPB_FATSz16字段，根目录区
 	存有文件系统所有的文件单元，相当于Linux系统的inode区，根目录区的结构
 	是由若干个entry组成，每个Entry 32字节
       	0     1	      9       17          32
 	__________________________________________________
      	|     |        |       |           | 
      	| BPB | FAT1   | FAT2  | 根目录区  |   BLOCK区
      	|_____|________|_______|___________|_______________
 ******************************************************************************/
	add $31,%cx
	mov $BaseOfLoader,%ax
	mov %ax,%es
	mov $OffsetOfLoader,%ax
	mov %ax,%bx
	mov %cx,%ax
/*BaseOfLoader:OffsetOfLoader 用完比较文件就可以覆盖了*/
Write_Loader:
	mov $1,%cl
/*读取文件内容*/
	call ReadSector
	pop %ax
/*读取标志是否还有文件块的FAT信息*/
	call GetFatEntry
	cmp $0x0FFF,%ax
	jz File_Loaded
	push %ax
	add $31,%ax
	add $512,%bx
	jmp Write_Loader
File_Loaded:
/**********************************************************************************
  为什么这里用jmp $BaseOfLoader,$OffsetOfLoader就不行呢??
  间接跳转
************************************************************************************/
	ljmp $0x9000,$0x100
.include "boot/include/common.h"
DispStr:
	mov $BootMessage,%ax
	mov $28,%cx
	mov %ax,%bp
	mov $0x1301,%ax
	mov $0x00c,%bx
	int $0x10
	ret
BootMessage:
	.ascii "Booting start,Loading loader"
NLStr:
	.ascii "No Loader"
LoadMessage:
	.ascii "Hello, Loader Bin!"
/**************************************************************************
  loader.bin 一定要大写
***************************************************************************/
FileName:
	.ascii "LOADER  BIN"
/*根目录开始位置*/
.set RootStartNo,19
.set RootLength,14
/*************************************************************************
  int 13 中断结果传送的位置
**************************************************************************/
.set BaseOfLoader,0x9000
.set OffsetOfLoader, 0x0100
FlagOdd:
	.byte 0
wSectorNo:
	.2byte 0
wLength:
	.2byte 0
.org 510
.word 0xaa55
