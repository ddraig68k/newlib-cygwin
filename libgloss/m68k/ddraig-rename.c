/*
 * io-rename.c -- 
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

#include <stdio.h>
#include <string.h>
#include <errno.h>
#define IO rename
#include "io.h"

#include "ddraig.h"

/* use BIOS call to rename file
 * 
 * CALL:
 * sys.a0 = oldpath 
 * sys.a1 = newpath 
 * 
 * RETURN:
 * return code = errno
 * sys.d1 = errno;
 */

int _rename (const char *oldpath, const char *newpath)
{
  	syscall_data sys;
	int ret;

	sys.command = DISK_FILERENAME;
	sys.a0 = oldpath;
	sys.a1 = newpath;

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
