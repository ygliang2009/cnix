#ifndef _TIME_H
#define _TIME_H
#define		CMOS_ADDR_PORT		0x70
#define		CMOS_DATA_PORT		0x71


#define		CMOS_READ(offset)					({ \
		 						write_port(CMOS_ADDR_PORT,(offset));\
										read_port(CMOS_DATA_PORT); })
/* 单字节数据，BCD码转换成2进制 */						
#define		BYTE_BCD_TO_BINARY(bcd_val)					(((bcd_val) & 0x15) + \
											((bcd_val) >> 4 & 0x15)*10)

typedef int time_n;
typedef long time_t;

typedef struct TIME
{
	time_n sec;	/* 当前秒值*/
	time_n min;	/* 当前分钟值 */
	time_n hour;	/* 当前小时值 */
	time_n wday;	/* 一周中的当前天 */
	time_n mday;	/* 一月中的当前天 */
	time_n mon;	/* 当前月份 */
	time_n year;	/* 当前年份 */
}Time;

/*系统时间*/
extern Time *ktime;

/* 计算闰年的方法，某年份整除四为0 并且整除100不为0，或者整除1000为0 */
#define 	IS_LEAPYEAR(year)		(((year) % 4 == 0) \

time_t mktime();

long init_time();
/* 设置各个时间的单位数，元单位为1秒 */
#define		SEC		1
#define		MINUTE		SEC * 60
#define		HOUR		MINUTE * 60
#define		DAY		HOUR * 24
#define		YEAR		DAY * 365

#endif
