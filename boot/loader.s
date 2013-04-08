/************************************************************************
		loader.s：加载内核入内存，进入保护模式，分段分页
			转入内核kernel.s 执行内核代码
	@Author:Chemical Liang	@Version:1.0	@Date:2011-10-XX
*************************************************************************/
/************************************************************************
	??这里如果应用.org 0x100 则会报错??
 ************************************************************************/
.file "loader.s"
.code16
.text
	jmp LoaderStart

.include "boot/include/boot.h"
.include "boot/include/loader.h"
.include "boot/include/fat12.h"

LoaderKeyWord:
	.ascii "Loader has been loaded in"
GdtSeg:
	Descriptor 		0x0,		0x0,		0x0
CodeSeg:
	/* 代码段属性为 [读/执行], 支持最大寻址为4GB */
	Descriptor		0x0,		0xfffff,	DA_32 | DA_CR | DA_LIMIT_4K
DataSeg:
	/* 数据段属性为 [读/写], 支持最大寻址为4GB */
	Descriptor		0x0,		0xfffff,	DA_32 | DA_DRW | DA_LIMIT_4K
VideoSeg:
	/* 显存段属性为 [读/写], 支持的特权级为DPL3级别，因此用户态下也可写显卡 */
	Descriptor		0xB8000,	0xfffff,	DA_DRW + DA_DPL3
.set GdtSegLen, 		(. - GdtSeg - 1)

	/* 选择子 */
.set SelectorCodeSeg,		(CodeSeg - GdtSeg)
.set SelectorDataSeg,		(DataSeg - GdtSeg)
.set SelectorVideoSeg,		(VideoSeg - GdtSeg)

	/* lgdt参数 Gdt指针 */
GdtPtr:
	/* GdtSegLen 指针 or 值？*/
	.2byte GdtSegLen 
	.4byte (BaseOfLoaderPhyAddr + GdtSeg)

LoaderStart:
	mov %cs,%ax
	mov %ax,%ds
	mov %ax,%es
	/* 堆栈基址 */
	mov %ax,%ss
	/* 低0x100为栈的生长方向,非常重要 */
	mov $0x100,%sp
	mov $0xB800,%ax
	mov %ax,%gs
	push $LoaderKeyWord
	push $((80*1+0)*2)
	push $25
	call DispStrReal
	nop

	/* 检测硬盘信息，为保证取到绝对地址，需要设置ds为0 */
	push %ds
	mov $0x00,%ax
	mov %ax,%ds
	/* 取中断向量0x41的值，即hd0的参数表地址 -> ds:si */
	mov (0x41*4),%si
	mov $BaseOfDisk,%ax
	mov %ax,%es
	mov $OffsetOfDisk1,%di
	mov $0x10,%cx
	rep
	movsb
	/* 取中断向量0x46的值，即hd1的参数表地址 -> ds:si */
	mov (0x41*4),%si
	mov $BaseOfDisk,%ax
	mov %ax,%es
	mov $OffsetOfDisk2,%di
	mov $0x10,%cx
	rep
	movsb
	/* 取完以后判断是否hd1存在，如果不存在，则清空hd1所在位置的结构值 */
	/* 判别方式: int 13号中断，功能号: ah = 0x15 dl = 驱动器号(0x8x表示硬盘，0x80表第一个，0x81表第二个) */
	/* 输出: ah = 类型码; 00 - 没有这个盘 01 - 是软驱 02 - 是软驱(或其他可移动设备) 03 - 是硬盘 */
	mov $0x15,%ax	
	/* 判断有无hd1 */
	mov $0x81,%dl
	int $0x13
	jc no_disk1
	cmp $3,%ah
	je is_disk1
no_disk1:
	/* 如果没有第一个硬盘，则做清零操作 */
	mov $BaseOfDisk,%ax
	mov %ax,%es
	mov $OffsetOfDisk2,%di
	mov $0x10,%cx
	mov $0x00,%ax
	rep
	stosb
is_disk1:	
	pop %ds

	/* 软驱复位 */	
	xor %ah,%ah	
	xor %di,%di
	int $0x13 
	/* 在根目录中寻找 KERNEL BIN */	
	movw $RootStartNo,(wSectorNo)		/*在对内存进行寻址的时候必须要加上movw中的w*/
	movw $RootLength,(wLength)
CmpRootLength:
	cmpw $0,(wLength)
	jz NoKernel
	decw (wLength)
	movw $BaseOfKernelBin,%ax
	movw %ax,%es
	movw $OffsetOfKernelBin,%bx
	movw (wSectorNo),%ax
	movb $1,%cl
	call ReadSector
	movw $KernelFileName,%si
	movw $OffsetOfKernelBin,%di
	cld
	movw $16,%dx
CmpOneItem:
	test %dx,%dx
	jz CmpRoot
	dec %dx
	movw $0,%cx
	jmp CmpOneLoop
CmpRoot:
	incw (wSectorNo)
	jmp CmpRootLength
CmpOneLoop:
	cmp $11,%cx
	je KernelFileFound
	incw %cx
	lodsb
	cmp %es:(%di),%al
	je CmpNext
	andw $0xffe0,%di
	addw $0x0020,%di
	movw $KernelFileName,%si
	jmp CmpOneItem
CmpNext:
	incw %di
	jmp CmpOneLoop
Stop:
	jmp .
NoKernel:
	push $NoKernelStr
	push $(((80*3)+0)*2)
	push $9
	call DispStrReal
	jmp Stop
KernelFileFound:
	/* %di和%ax在方法中都会发生改变，这里要注意最好可以有一个通用方法 */
	push %di
	push %ax
	push $KernelFoundMsg
	push $((80*3+0)*2)
	push $23
	call DispStrReal
	/* 堆栈还原 */
	movw %sp,%ax
	add $6,%ax
	movw %ax,%sp
	pop %ax
	pop %di
	/*?? 这里不是0xFFE0,而是0xFFF0 ??*/
	and $0xFFF0,%di
	add $0x1A,%di
	movw %es:(%di),%cx
	push %cx
	add $31,%cx
	movw $BaseOfKernelBin,%ax
	movw %ax,%es
	movw $OffsetOfKernelBin,%ax
	movw %ax,%bx
	movw %cx,%ax
/* BaseOfKernel:OffsetOfKernel 用完比较文件就可以覆盖了 */
WriteLoader:
	push %ax
	push %bx
	mov $0x0E,%ah
	mov $'*',%al
	mov $0x0F,%bl
	int $0x10
	pop %bx
	pop %ax
	movb $1,%cl	
	call ReadSector
	pop %ax
	call GetFatEntry
	cmp $0x0FFF,%ax
	jz FileLoaded
	push %ax
	add $31,%ax
	add $512,%bx
	jmp WriteLoader
FileLoaded:
	push %ax
	push $KernelLoadMsg
	push $((80*5+0)*2)
	push $22
	call DispStrReal
	/* 堆栈还原 */
	movw %sp,%ax
	add $6,%ax
	movw %ax,%sp
	pop %ax
	xor %eax,%eax
	lgdtw GdtPtr
	cli
	inb $0x92,%al
	orb $0b00000010,%al
	outb %al,$0x92
	movl %cr0,%eax
	orl $1,%eax
	movl %eax,%cr0
	/* 注意这里的加和标识 */
	ljmpl $(SelectorCodeSeg),$BaseOfLoaderPhyAddr+Code32Start

/*************************************************************
	void DispStrReal(char *message,int pos,int length)
*************************************************************/
DispStrReal:
	push %bp
	mov %sp,%bp
	/* 实模式下2个字节一个单元 */
	mov 8(%bp),%ax
	mov %ax,%si
	mov $0xC,%ah
	mov 6(%bp),%ax
	mov %ax,%di
	mov 4(%bp),%cx
DispStrRealLoop1:
	lodsb
	mov %al,%gs:(%di)
	add $2,%di
	loop DispStrRealLoop1
	mov %bp,%sp
	pop %bp
	ret

.include "boot/include/common.h"

	/* 从这里进入保护模式代码段 */
.code32
Code32Start:
	mov $(SelectorVideoSeg),%ax
	mov %ax,%gs
	mov $(SelectorDataSeg),%ax
	mov %ax,%ds
	mov %ax,%es
	mov %ax,%fs
	mov %ax,%ss
	mov $TopOfStack,%esp
	call setupPaging
	pushl $ProtectModeWelcome
	pushl $((80*6+0)*2)
	pushl $18
	call dispStrProtect
	call initElfFile		
	ljmpl $(SelectorCodeSeg),$(KernelEntryPointPhyAddr)

/*************************************************************
	void dispStrProtect(char *message,int pos,int length)
*************************************************************/
dispStrProtect:
	pushl %ebp
	movl %esp,%ebp
	pushl %eax
	pushl %ecx
	pushl %esi
	pushl %edi
	/* 实模式下2个字节一个单元 */
	movl 16(%ebp),%eax
	movl %eax,%esi
	movb $0xC,%ah
	movl 12(%ebp),%eax
	movl %eax,%edi
	movl 8(%ebp),%ecx
DispStrProtectLoop1:
	lodsb
	movb %al,%gs:(%edi)
	addl $2,%edi
	loop DispStrProtectLoop1
	popl %edi
	popl %esi
	popl %ecx
	popl %eax
	movl %ebp,%esp
	popl %ebp
	ret

	/* 支持64MB分页机制 */
setupPaging:
	movw $(SelectorDataSeg),%ax
	movw %ax,%es
	movl $PageDirBase,%eax
	movl %eax,%edi
	movl $1024,%ecx
	movl $(PageTableBase|PG_P|PG_USU|PG_RWW),%eax
loopPageDir:
 	stosl	
	addl $4096,%eax
	loop loopPageDir

	movl $(PageTableBase),%eax
	movl %eax,%edi
	movl $(16*1024),%ecx
	movl $(PG_P|PG_USU|PG_RWW),%eax
loopPageTable:
	stosl
	addl $4096,%eax
	loop loopPageTable

	movl $(PageDirBase),%eax
	movl %eax,%cr3
	movl %cr0,%eax
	or $0x80000000,%eax
	movl %eax,%cr0
	ret

/*******************************************************************
	void memcpy(void *dest, void *src, int length)
********************************************************************/
memcpy:
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

/*************************************************************************
	void initElfFile()
 *************************************************************************/
initElfFile:
	nop
	nop
	nop
	pushl %ebp
	movl %esp,%ebp
	pushl %eax
	pushl %ecx
	pushl %esi
	pushl %edi
	xor %esi,%esi
	xor %ecx,%ecx
	movw (BaseOfKernelPhyAddr+0x2C),%cx
	movl (BaseOfKernelPhyAddr+0x1C),%esi
	add $BaseOfKernelPhyAddr,%esi
initBegin:
	movl (%esi),%eax
	cmpl $0,%eax
	jz noAction
	pushl 16(%esi)
	movl 4(%esi),%eax
	addl $BaseOfKernelPhyAddr,%eax
	pushl %eax
	pushl 8(%esi)
	call memcpy
	addl $12,%esp
noAction:
	addl $0x20,%esi	
	decl %ecx
	jnz initBegin
	popl %edi
	popl %esi
	popl %ecx
	popl %eax
	movl %ebp,%esp
	popl %ebp
	ret
DataStart:
_ProtectModeWelcome:
	.ascii "Enter protect Mode"
_CursorPos:
	.4byte (2*80+0)*2
_LFMes:
	.asciz "\n\0"
.set ProtectModeWelcome, (_ProtectModeWelcome + BaseOfLoaderPhyAddr)
.set LFMes, (_LFMes + BaseOfLoaderPhyAddr)
.set CursorPos, (_CursorPos + BaseOfLoaderPhyAddr)
.space 256,0
.set TopOfStack, .
.ident "@author:Chmical Liang"
