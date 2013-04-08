/*******************************************************************************
 			系统调用处理函数汇编部分
		########### 	宏定义	####################
	@Author:Chemical Liang	@Version:1.0	@Date:2011-11-19
 *******************************************************************************/
.equ 	INT_SYSCALL, 			0x80
.equ	TEST_NUM,			0
.equ	WRITE_NUM,			1
.equ	PAUSE_NUM,			2
.equ	GETPID_NUM,			3
.equ	NICE_NUM,			4
.equ	GETSIGNAL_NUM,			5
.equ	GETBLOCK_NUM,			6
