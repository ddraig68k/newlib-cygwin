/* lseek.c -- move read/write pointer.
 *
 * Copyright (c) 1995 Cygnus Support
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
#include <errno.h>
#include "../glue.h"
#include "fat_filelib.h"

/*
 * lseek -- reposition a file descriptor
 * input parameters:
 *   0 : file descriptor
 *   1 : high word of offset
 *   2 : low word of offset
 *   3 : seek flag
 * output parameters:
 *   0 : high word of result
 *   1 : low word of result
 *   2 : errno
 */

off_t lseek (int fd, off_t offset, int whence)
{
  if (fd < 3)
  {
    errno = ESPIPE;
    return ((off_t)-1);
  }
  else
  {
    errno = ESPIPE;
    return fl_fseek(fl_table[fd],offset,whence);
  }
}

