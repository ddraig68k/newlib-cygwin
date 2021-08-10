/*
 * io-fstat.c -- 
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
#include <unistd.h>
#include <errno.h>
#define IO fstat
#include "io.h"

#include "ddraig.h"

/* use BIOS call to get file FIL struct
 * 
 * CALL:
 * sys.d0 = file id
 * 
 * RETURN:
 * return code = error code
 * sys.a0 = pointer to FIL struct;
 * sys.d1 = errno;
 */
int fstat (int fd, struct stat *buf)
{
  	syscall_data sys;
	int ret;

	sys.command = DISK_FILESTAT;
	sys.d0 = fd;

	__asm__ volatile(
	"move.l	%1, %%a0\n"
	"trap	#15\n"
	"move.l %%d0, %0\n"
	: "=g" (ret)
	: "g" (&sys)
	: "%a0"
	);

  	errno = sys.d1;
	if (ret < 0)
  		return ret;
		  
	FIL *filedata = sys.a0;

    // This is a file, return the file length.
    buf->st_mode |= S_IFREG;
    buf->st_size = f_size(filedata);
    buf->st_blksize = FF_MAX_SS;
    buf->st_blocks = (f_size(filedata) + (FF_MAX_SS - 1)) / FF_MAX_SS;
	buf->st_nlink = 1;

	return 0;
}