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

//#include <sys/types.h>
//#include <sys/stat.h>
#include <fcntl.h>
//#include <stdarg.h>
//#include <string.h>
#include <errno.h>
//#define IO open
//#include "io.h"
#include <stdio.h>
#include "fat_filelib.h"

/*
 * open -- Open a file.
 * input parameters:
 *   0 : fname ptr
 *   1 : fname length
 *   2 : flags
 *   3 : mode
 * output parameters:
 *   0 : result
 *   1 : errno
 */

int open (const char *fname, int flags, ...)
{
	FL_FILE *fl_fd;
	char *mode="1234";
	int fd;
	//printf("Kiwi fopen: %s (%u,%c)\n",fname,flags,mode);
	if (flags == O_RDONLY) {
	    //printf("READ!\n");
	    mode="r";
	}
	if (flags & O_WRONLY) {
	    //printf("WRITE!\n");
	    mode="w";
	}
	if (flags & O_RDWR) {
	    //printf("READ/WRITE!\n");
	    mode="rw";
	}
	if (flags & O_APPEND) {
	    //printf("APPEND!\n");
	    mode="w+";
	}
	for(fd=3;fd<20;fd++) {
	  if(fl_table[fd] == NULL) {
	    fl_fd = fl_fopen(fname,mode);
	    fl_table[fd]=fl_fd;
	    //printf("fd: %u\n",fd);
	    return fd;
	  }
	}
//  gdb_parambuf_t parameters;
//  parameters[0] = (uint32_t) fname;
//  parameters[1] = strlen (fname) + 1;
//  parameters[2] = __hosted_to_gdb_open_flags (flags);
//  if (flags & O_CREAT)
//    {
//      va_list ap;
//      va_start (ap, flags);
//      parameters[3] = __hosted_to_gdb_mode_t (va_arg (ap, mode_t));
//      va_end (ap);
//    }
//  else
//    parameters[3] = 0;
//  __hosted (HOSTED_OPEN, parameters);
//  errno = __hosted_from_gdb_errno (parameters[1]);
//  return parameters[0];
  errno = ENOSYS;
  return -1;
}
