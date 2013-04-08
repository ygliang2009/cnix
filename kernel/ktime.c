/***************************************************************************
 * 		本函数非系统库函数，仅在内核中使用
 *
 * 	@Author:Chemical Liang	@Version:1.0	@Date:2011-11-19
 **************************************************************************/
#include "cnix.h"
#include "type.h"
#include "stdlib.h"
#include "time.h"

/* 每个月的单元秒数 */
const static int month[12] = 
{
	/* 1月份 */ 31 * DAY,
	/* 2月份 */ (31 + 28) * DAY,
	/* 3月份 */ (31 + 28 + 31) * DAY,
	/* 4月份 */ (31 + 28 + 31 + 30) * DAY,
	/* 5月份 */ (31 + 28 + 31 + 30 + 31) * DAY,
	/* 6月份 */ (31 + 28 + 31 + 30 + 31 + 30) * DAY,
	/* 7月份 */ (31 + 28 + 31 + 30 + 31 + 30 + 31) * DAY,
	/* 8月份 */ (31 + 28 + 31 + 30 + 31 + 30 + 31 + 31) * DAY,
	/* 9月份 */ (31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30) * DAY,
	/* 10月份 */ (31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31) * DAY,
	/* 11月份 */ (31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30) * DAY,
	/* 12月份 */ (31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + 31) * DAY
};

/* 返回现在到1970年0点0分0秒的单元秒数 */
long mktime()
{
	int year = 0;
	long result = 0;
	
	year = ktime->year;
	/* CMOS读取的time只有两位值，所以如果小于70的话应该是2000年以后 */
	if(year < 70)
	{
		year += 100;
		ktime->year = year;

	}
	/* CMOS读取初始值为1970年，下列计算加入当前年份距离1970年相差的年数 + 其中经过的闰年数 */
	result += ((year - 70) * YEAR + (year + 1)/4 * DAY);
	/* 如果当前年份加上2以后能被4整除，并且当前月份在2月份之后，则需要在当前加入2月份的29天*/
	if(((ktime->year + 2) % 4 == 0) && (ktime->mon > 2))
	{
		result += DAY;
	}
	result += month[ktime->mon - 1];
	result += ktime->mday * DAY;
	result += ktime->hour * HOUR;
	result += ktime->min * MINUTE;
	result += ktime->sec * SEC;
        return result;	
}

long init_time()
{
	long result = 0;
	/***********************************************************************
	 	CMOS的读取速度比较慢，为减少时间误差，如果秒数变化则重新读取
	 ***********************************************************************/
	do{
		ktime->sec = CMOS_READ(0x00);	
		ktime->min = CMOS_READ(0x02);
		ktime->hour = CMOS_READ(0x04);
		ktime->wday = CMOS_READ(0x06);
		ktime->mday = CMOS_READ(0x07);
		ktime->mon = CMOS_READ(0x08);
		ktime->year = CMOS_READ(0x09);

	}while(ktime->sec != CMOS_READ(0x00));

	BYTE_BCD_TO_BINARY(ktime->sec);
	BYTE_BCD_TO_BINARY(ktime->min);
	BYTE_BCD_TO_BINARY(ktime->hour);
	BYTE_BCD_TO_BINARY(ktime->wday);
	BYTE_BCD_TO_BINARY(ktime->mday);
	BYTE_BCD_TO_BINARY(ktime->mon);
	BYTE_BCD_TO_BINARY(ktime->year);
	
	result = mktime();
	return result;
}
