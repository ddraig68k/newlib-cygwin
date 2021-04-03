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
#include <stdint.h>
#include <time.h>
#include <errno.h>
//#define IO gettimeofday
#include "io.h"

#define RTC_BASE        0x3dfc00 /* RTC72421 Basis address */
#define SECOND1         0
#define SECOND10        2
#define MINUTE1         4
#define MINUTE10        6
#define HOUR1           8
#define HOUR10          10
#define DAY1            12
#define DAY10           14
#define MONTH1          16
#define MONTH10         18
#define YEAR1           20
#define YEAR10          22
#define DOW             24 /* Day of week */
#define CD              26 /* Control register D */
#define CE              28 /* Control register E */
#define CF              30 /* Control register F */

#define READ_RTC(x)     (*((uint8_t *) RTC_BASE + (x) ))
#define WRITE_RTC(x, y) (*((uint8_t *) RTC_BASE + (x) ) = (y) )

void _gettimeofday_powerOn(void) {
        WRITE_RTC(CF,4); /* 24h mode */
        WRITE_RTC(CD,0); /* No interrupts */
}

void _gettimeofday_waitBSY(void) {
        WRITE_RTC(CD,1); /* No interrupts, HOLD Bit=1 */
        while ((READ_RTC(CD)&2)) {
                WRITE_RTC(CD,0); /* No interrupts, HOLD Bit=0 */
                WRITE_RTC(CD,1); /* No interrupts, HOLD Bit=1 */
        }
}

/*
 * gettimeofday -- get the current time
 * input parameters:
 *   0 : timeval ptr
 * output parameters:
 *   0 : result
 *   1 : errno
 */

int gettimeofday (struct timeval *tv, void *tz)
{
	/* 
	 * 20140902 MVN
	 * gettimeofday implemented, by using Simon's Clock-code.
	 * tz - timezone parameter is ignored.
	 * usec - microseconds are ignored.
	 * TODO: error check and setting errno would be nice.
	 */

	uint8_t h1, h10, mi1, mi10, s1, s10, d1, d10, mo1, mo10, y1, y10; //, y100=20, day;
        struct tm dt;

	/* get current date and time from RTC */
	_gettimeofday_powerOn();
        _gettimeofday_waitBSY();
        h1    = READ_RTC(HOUR1) & 15;
        h10   = READ_RTC(HOUR10) & 15;
        mi1   = READ_RTC(MINUTE1) & 15;
        mi10  = READ_RTC(MINUTE10) & 15;
        s1    = READ_RTC(SECOND1) & 15;
        s10   = READ_RTC(SECOND10) & 15;
        d1    = READ_RTC(DAY1) & 15;
        d10   = READ_RTC(DAY10) & 15;
        mo1   = READ_RTC(MONTH1) & 15;
        mo10  = READ_RTC(MONTH10) & 15;
        y1    = READ_RTC(YEAR1) & 15;
        y10   = READ_RTC(YEAR10) & 15;
        //day   = READ_RTC(DOW) & 15;
        WRITE_RTC(CD,0); /* No interrupts, HOLD Bit=0 */

	/* convert date and time to UNIX time (seconds since 01.01.1970) */
        dt.tm_year  = 10*y10 + y1 + 100; // Year  (2000 + 10*y10 + y1 - 1900)
        dt.tm_mon   = 10*mo10 + mo1 - 1; // Month (0 = January)
        dt.tm_mday  = 10*d10 + d1;       // Day
        dt.tm_hour  = 10*h10 + h1;       // Hour
        dt.tm_min   = 10*mi10 + mi1;     // Minute
        dt.tm_sec   = 10*s10 + s1;       // Second
        dt.tm_isdst = -1;                // DST   (-1 = unknown)

	/* set seconds and useconds */
	tv->tv_sec  = mktime(&dt);
  	tv->tv_usec = 0;

	//errno = ENOSYS;
	return 0;
}


/*
 * settimeofday -- set the current time
 * input parameters:
 *   0 : timeval ptr
 * output parameters:
 *   0 : result
 *   1 : errno
 */

int settimeofday (const struct timeval *tv, const struct timezone *tz)
{
	/*
         * 20140905 MVN
         * tz - timezone parameter is ignored.
         * usec - microseconds are ignored.
         * TODO: error check and setting errno would be nice.
         */

	struct tm ts;
    	char   nt[16];

	int d;        // Day     (1-31)
        int m;        // Month   (1-12)
        int y;        // Year    (2000-)
        int weekday ; // WeekDay (0 = Sunday, 1 = Monday, ...)

	/* convert UNIX timestamp to string in format "YYMMDDhhmmss" */
    	ts = *localtime(&tv->tv_sec);
    	strftime(nt, sizeof(nt), "%y%m%d%H%M%S", &ts);

	/* 
	 * caclulate weekday by 
         * One Liner Solution for "C" by Michael Keith and Tom Craver
	 * http://en.wikipedia.org/wiki/Determination_of_the_day_of_the_week
	 */
        d = 10 * nt[4] + nt[5];
        m = 10 * nt[2] + nt[3];
        y = 10 * nt[0] + nt[1] + 2000;
        weekday = (d+=m<3?y--:y-2,23*m/9+d+4+y/4-y/100+y/400)%7 ;

	// set hw clock
        WRITE_RTC(CF, 4); // 24 hours format
        WRITE_RTC(CD, 4); // no interrupts

        WRITE_RTC(CD, 5); // HOLD bit = 1
        while( (READ_RTC(CD) & 2) != 0 ) {
                WRITE_RTC(CD, 4);
                WRITE_RTC(CD, 5);
        }

        // set date
        WRITE_RTC(YEAR10,   nt[0]);   // 1 and
	WRITE_RTC(YEAR1,    nt[1]);   // 4 -> (20)14
        WRITE_RTC(MONTH10,  nt[2]);   // 0 and 
	WRITE_RTC(MONTH1,   nt[3]);   // 7 -> 07 = July
        WRITE_RTC(DAY10,    nt[4]);   // 1 and 
	WRITE_RTC(DAY1,     nt[5]);   // 5 -> 15
        // set time
        WRITE_RTC(HOUR10,   nt[6]);   // 1 and
	WRITE_RTC(HOUR1,    nt[7]);   // 2 -> 12(:34:56)
        WRITE_RTC(MINUTE10, nt[8]);   // 3 and 
	WRITE_RTC(MINUTE1,  nt[9]);   // 4 -> (12:)34(:56)
        WRITE_RTC(SECOND10, nt[10]);  // 5 and 
	WRITE_RTC(SECOND1,  nt[11]);  // 6 -> (12:34:)56
        // set weekday
        WRITE_RTC(DOW, weekday);      // 2 = Tuesday, ...

	WRITE_RTC(CD, 4); // no interrupts

	return 0;
}
