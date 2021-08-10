/*
 * io-stat.c -- 
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

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <memory.h>

#define IO stat
#include "io.h"

#include "ddraig.h"

/* use BIOS call to get file stat
 * 
 * CALL:
 * sys.a0 = filename 
 * sys.a1 = file stat struct
 * 
 * RETURN:
 * return code = errno
 * sys.d1 = errno;
 */

int stat (const char *__restrict filename, struct stat *__restrict buf)
{
  	syscall_data sys;
	int ret;

	FILINFO fileinfo;

	memset(&fileinfo, 0, sizeof(FILINFO));

	sys.command = DISK_FILESTAT;
	sys.a0 = (const void *)filename;
	sys.a1 = &fileinfo;

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

    // This is a file, return the file length.
    buf->st_mode |= S_IFREG;
    buf->st_size = fileinfo.fsize;
    buf->st_blksize = FF_MAX_SS;
    buf->st_blocks = (fileinfo.fsize + (FF_MAX_SS - 1)) / FF_MAX_SS;
	buf->st_nlink = 1;

	return 0;

}
