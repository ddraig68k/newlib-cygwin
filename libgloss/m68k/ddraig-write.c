/*
 * io-write.c -- 
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
#define IO write
#include "io.h"

#include "ddraig.h"

/* use BIOS call to write file
 * 
 * CALL:
 * sys.d0 = file number
 * sys.d1 = count
 * sys.a0 = buffer 
 * 
 * RETURN:
 * return code = number of bytes written
 * sys.d0 = number of bytes written
 * sys.d1 = errno;
 */

ssize_t write (int fd, const void *buf, size_t count)
{
  	syscall_data sys;
    int ret;

    sys.command = DISK_FILEWRITE;
    sys.a0 = buf;
    sys.d0 = fd;
    sys.d1 = count;

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
