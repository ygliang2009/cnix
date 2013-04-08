/***********************************************************************************
 			硬盘驱动头文件 
 	@Author:Chemical Liang	@Version:1.0	@Date:2011-11-XX
 **********************************************************************************/
#ifndef _HD_H
#define	_HD_H
#define		HD_INFO			0x475
/* 硬盘的基本结构信息 */
#define		HD_ADDR			0x20080
#define		QUEUE_NUM		32
#define		SECTOR_SIZE		512
extern 	int hd_num;

/*********************************************************************************************************
				设备相关信息(设备号同Linux系统)
  	_________________________________________________________________________________________
 	|		  |		|				|			|
	|     主设备号	  |	类型	|   		说明		|      相关操作函数	|
	|		  |		|				|			|
	|—————————————————|—————————————|———————————————————————————————|———————————————————————|
	|	0	  |	无	| 无				|  NULL			|
	|—————————————————|—————————————|———————————————————————————————|———————————————————————|
	|	1	  |   块/字符	| ram，内存设备(虚拟盘等)	|  			|
	|—————————————————|—————————————|———————————————————————————————|———————————————————————|	
	|	2	  |   块设备	| fd，软驱设备			|			|
 	|—————————————————|—————————————|———————————————————————————————|———————————————————————|
	|	3	  |   块设备    | hd, 硬盘设备			|			|	
	|—————————————————|—————————————|———————————————————————————————|———————————————————————|
	|	4	  |    字符	| ttyx设备(虚拟或串行设备)	|			|	
	|—————————————————|—————————————|———————————————————————————————|———————————————————————|
	|	5	  |    字符	| tty设备			| 			|
	|—————————————————|—————————————|———————————————————————————————|———————————————————————|
	|	6	  |    字符	| lp打印机设备			|			|
	|—————————————————|—————————————|———————————————————————————————|———————————————————————|
	|—————————————————|—————————————|———————————————————————————————|———————————————————————|	

 ********************************************************************************************************/

#define		NR_BLOCK			7

#if	(MAJOR_NR == 1)
#define		DEVICE_NAME			"ramdisk"
#define		DEVICE_REQUEST			do_rd_request
#define		DEVICE_NR(device)		((device) & 7)
#elif	(MAJOR_NR == 2)
#define		DEVICE_NAME			"floppy"
#define		DEVICE_REQUEST			do_floppy
#define		DEVICE_NR(device)		((device) & 3)
#elif	(MAJOR_NR == 3)
#define		DEVICE_NAME			"harddisk"
#define		DEVICE_INTR			do_hd
#define		DEVICE_TIMEOUT			hd_timeout
#define		DEVICE_REQUEST			do_hd_request
#else
#define		ERROR_DEV			"unknown block device"
#endif

#ifdef		DEVICE_INTR	
void (*DEVICE_INTR)(void) = NULL;
#endif

#ifdef		DEVICE_TIMEOUT
int DEVICE_TIMEOUT = 0;
#define		SET_INTR(x)		(DEVICE_INTR = (x), DEVICE_TIMEOUT = 200)
#define		CLEAR_DEVICE_TIMEOUT	DEVICE_TIMEOUT = 0;
#else
#define		SET_INTR(x)		(DEVICE_INTR = (x))
#endif

static void (DEVICE_REQUEST)(void);

typedef struct REQUEST
{
	int dev_num;				/* 请求的从设备号 */
	int cmd;				/* 请求操作: READ|WRITE */
	unsigned long sector;			/* 请求其实扇区号 */
	unsigned long sector_nums;		/* 请求扇区数 */
	struct REQUEST *next;			/* 指向下一个请求的指针 */
}Request;

/* IDE消息请求队列 */
extern Request request[];

typedef struct BLOCK_DEV_STRUCT
{
	void (*request_fn)();
	Request *current_request;	
}Block_dev_struct;

extern Block_dev_struct block_dev[];

/***********************************************
	 硬盘控制器的各个寄存器 
************************************************/
/* 数据寄存器 */
#define		HD_DATA			0x1f0	
/* 错误寄存器 */
#define		HD_ERROR		0x1f1	//	\
/* 写前预补偿寄存器 */				//	| - 同一个寄存器，HD_ERROR为读时，HD_PRECOMP为写时
#define		HD_PRECOMP		0x1f1   //	/
/* 扇区数寄存器 */
#define		HD_NSECTOR		0x1f2
/* 柱面号寄存器 -- 柱面号低字节 */
#define		HD_LCYL			0x1f3
/* 柱面号寄存器 -- 柱面号中字节 */
#define		HD_MCYL			0x1f4
/* 柱面号寄存器 -- 柱面号高字节 */
#define		HD_HCYL			0x1f5
/* 驱动器/磁头寄存器 */
#define		HD_DEVICE		0x1f6
/* 主状态寄存器 */
#define		HD_STATUS		0x1f7	//	\
/* 命令寄存器 */				//	| - 同一个寄存器，HD_STATUS为读时，HD_COMMAND为写时	
#define		HD_COMMAND		0x1f8	//	/
/*硬盘控制寄存器 */
#define		HD_CMD			0x3f6

/**********************************************************
 *		8位主状态寄存器屏蔽码
 *********************************************************/
/* 位0: 命令执行错误 */
#define		ERR_STAT		0x01
/* 位1: 收到索引*/
#define		INDEX_STAT		0x02
/* 位2: ECC校验错 */
#define		ECC_STAT		0x04
/* 位3: 数据请求服务，当该位被置位时，表示驱动器已经准备 *
 * 好在主机和数据接口之间传送一个字或一个字节的数据 	*/
#define		DRQ_STAT		0x08
/* 位4: 驱动器寻到结束 */
#define		SEEK_STAT		0x10
/* 位5: 驱动器故障(写出错) */
#define		WRERR_STAT		0x20
/* 位6: 驱动器准备好 */
#define		READY_STAT		0x40
/* 位7: 控制器忙碌 */
#define		BUSY_STAT		0x80

/*************************************************************
 *		硬盘控制器命令(HD_COMMAND的取值)
 *************************************************************/
/* 驱动器重新校正 */
#define		WIN_RESTORE		0x10
/* 读扇区 */
#define		WIN_READ		0x20
/* 写扇区 */
#define		WIN_WRITE		0x30
/* 扇区校验 */
#define		WIN_VERIFY		0x40
/* 格式化磁道 */
#define		WIN_FORMAT		0x50
/* 控制器初始化 */
#define		WIN_INIT		0x60
/* 寻道 */
#define		WIN_SEEK		0x70
/* 控制器诊断 */
#define		WIN_DIAGNOSE		0x90
/* 建立驱动器参数 */
#define		WIN_SPECIFY		0x91

/***************************************************************
 *		HD_ERROR的各个位
 ***************************************************************/
#define		MAKE_ERR		0x01
#define		TRKO_ERR		0x02

typedef	struct DRIVE_INFO
{
	int head;	/* 磁头数 */
	int sect;	/* 扇区数 */
	int cyl;	/* 柱面数*/
	int wpcom;	/* 写前预补偿柱面号 */
	int lzone;	/* 磁头着陆区柱面号 */
	int ctl;	/* 每磁道扇区号 */
}DriveInfo;

extern DriveInfo hd_info[];

/* 磁盘发送命令 */
typedef struct HdCmd
{
	char features;
        char count;
	char ly_low;
	char ly_mid;
	char ly_high;
	char device;
	char command;	
}hd_cmd;

#define		ATA_IDENTITY		0xEC
#define		MAKE_DEVICE_REG(lba,drv,lba_highest)	(((lba)<<8)|((drv)<<4)|(lba_highest&0xF)|0xA0)
#endif	
