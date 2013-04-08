/**************************************************************************
 *			BOOT.S 的头文件
 *
 * 	Author:Chemical Liang	@Version:1.0	@Date:2011-10-10
 **************************************************************************/
BootMessage:
	.ascii "Hello, OS World!"
NoLoaderStr:
	.ascii "No Loader"
NoKernelStr:
	.ascii "No Kernel"
KernelFoundMsg:
	.ascii "Kernel has been founded"
KernelLoadMsg:
	.ascii "Kernel has been loaded"
/**************************************************************************
  loader.bin 一定要大写
***************************************************************************/
FileName:
	.ascii "LOADER  BIN"
KernelFileName:
	.ascii "KERNEL  BIN"
/*根目录开始位置*/
.set RootStartNo,19
.set RootLength,14
/*************************************************************************
  int 13 中断结果传送的位置
**************************************************************************/
.set BaseOfLoader,0x9000
.set OffsetOfLoader, 0x0100
.set BaseOfKernelBin, 0x8000
.set OffsetOfKernelBin, 0x0000
.set BaseOfLoaderPhyAddr, BaseOfLoader*0x10
.set BaseOfKernelPhyAddr, BaseOfKernelBin*0x10
.set KernelEntryPointPhyAddr, 0x30400
.set PageDirBase, 0x100000
.set PageTableBase, 0x101000
FlagOdd:
	.2byte 0
wSectorNo:
	.2byte 0
wLength:
	.2byte 0
