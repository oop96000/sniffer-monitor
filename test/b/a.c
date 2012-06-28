#ifndef __KERNEL__
#define __KERNEL__
#endif
#ifndef MODULE
#define MODULE
#endif
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/timex.h>
#include <linux/rtc.h>

/*添加到合适位置*/
/* 初始化程序 */
int init_module()
{
struct timex  txc;
struct rtc_time tm;
do_gettimeofday(&(txc.time));
rtc_time_to_tm(txc.time.tv_sec,&tm);
printk("UTC time :%d-%d-%d %d:%d:%d \n",tm.tm_year+1900,tm.tm_mon, tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec);
    return 0;
}

/* 清除程序 */
void cleanup_module()
{

}

