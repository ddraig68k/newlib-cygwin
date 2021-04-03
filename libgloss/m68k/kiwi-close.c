/* close.c -- close a file descriptor.
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
#include "../glue.h"
#include "fat_filelib.h"

/*
 * close -- We don't need to do anything, but pretend we did.
 */
int close(int fd)
{
  if(fd<3)
  {
    return (0);
  }
  else
  {
    //printf("Close fd %u\n",fd);
    fl_fclose(fl_table[fd]);
    fl_table[fd]=NULL;
    return (0);
  }
}
