/*
 * io-gettimeofday.c -- 
 *
 * Copyright (c) 2006 CodeSourcery Inc
 *
 * The authors hereby grant permission to use, copy, modify, distribute,
 * and license this software and its documentation for any purpose, provided
 * that existing copyright notices are retained in all copies and that this
 * notice is included verbatim in any distributions. No written agreement,
 * license, or royalty fee is required for any of the authorized uses.
 * Modifications to this software may be copyrighted by their authors
 * and need not follow the licensing terms described here, provided that
 * the new terms are clearly indicated on the first page of each file where
 * they apply.
 */

#include <sys/time.h>
#include <time.h>
#include <errno.h>
#define IO gettimeofday
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
