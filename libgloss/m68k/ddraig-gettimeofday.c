#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include "io.h"
#include <memory.h>

#include "ddraig.h"

/* use BIOS call to get date and time
 * 
 * CALL:
 * sys.a0 = rtc_date_t pointer 
 * 
 * RETURN:
 * return code = error code
 * sys.d1 = errno;
 */


int gettimeofday (struct timeval *tv, void *tzvp)
{
    rtc_date_t rtc;
    syscall_data sys;
	  int ret;

	  sys.command = SYS_TIMEDATE;
	  sys.a0 = &rtc;

    __asm__ volatile(
    "move.l	%1, %%a0\n"
    "trap	#14\n"
    "move.l %%d0, %0\n"
    : "=g" (ret)
    : "g" (&sys)
    : "%a0"
    );

  	errno = sys.d1;

    if (ret != 0)
        return ret;

    struct tm dt;
    memset (&dt, 0, sizeof (struct timeval));

    dt.tm_sec = rtc.tm_sec;
    dt.tm_min = rtc.tm_min;
    dt.tm_hour = rtc.tm_hour;
    dt.tm_mday = rtc.tm_day;
    dt.tm_mon = rtc.tm_mon;
    dt.tm_year = rtc.tm_year - 1900;
    dt.tm_isdst = 0;

    time_t timedate = mktime(&dt);
    tv->tv_sec = timedate;

  	return ret;
}
