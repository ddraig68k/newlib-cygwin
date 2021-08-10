/*
 * io-open.c -- 
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#define IO open
#include "io.h"

#include "ddraig.h"


/* use BIOS call to open file
 * 
 * CALL:
 * sys.d0 = file flags
 * sys.a0 = filename 
 * 
 * RETURN:
 * return code = file number
 * sys.d0 = file number
 * sys.d1 = errno;
 */

int open (const char *fname, int flags, ...)
{
  	syscall_data sys;
	int ret;

	sys.command = DISK_FILEOPEN;
	sys.a0 = fname;
	sys.d0 = __hosted_to_bios_file_flags(flags);

	printf("newlib: about to call fopen trap\n\r");

	__asm__ volatile(
	"move.l	%1, %%a0\n"
	"trap	#15\n"
	"move.l %%d0, %0\n"
	: "=g" (ret)
	: "g" (&sys)
	: "%a0"
	);

  	errno = sys.d1;
  	return ret;
}
