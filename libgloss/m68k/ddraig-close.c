/*
 * io-close.c -- 
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

#include <unistd.h>
#include <errno.h>
#define IO close
#include "io.h"

#include "ddraig.h"

/* use BIOS call to clsoe file
 * 
 * CALL:
 * sys.d0 = file number
 * 
 * RETURN:
 * return code = error code
 */

int close (int fd)
{
  	syscall_data sys;
	int ret;

	sys.command = DISK_FILEOPEN;
	sys.d0 = __hosted_to_bios_file_flags(fd);

  	__asm__ volatile(
	"move.l	%1, %%a0\n"
	"trap	#15\n"
	"move.l %%d0, %0\n"
	: "=g" (ret)
	: "g" (&sys)
	: "%a0"
	);

  	return ret;
}
