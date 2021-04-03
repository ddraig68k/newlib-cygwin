/* read.c -- read bytes from a input device.
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
#include <stdio.h>
#include "fat_filelib.h"

extern char inbyte();

/*
 * read  -- read bytes from the serial port. Ignore fd, since
 *          we only have stdin.
 */
ssize_t read (int fd, void *buf, size_t count)
{
  int i = 0;
  char a;
  char *ptr = (char *)buf;

  if (fd < 3) 	// fd 0-2 = stdin, stdout, stderr
  {
    for (i = 0; i < count; i++)
    {
      a = inbyte();
      if (a == 0)
        return i;

      ptr[i] = a;
      if ((ptr[i] == '\n') || (ptr[i] == '\r'))
      {
        i++;
        break;
      }
    }
  
    return i;
  }
  else
  {
    //FAT library
    return fl_fread(buf, 1, count, fl_table[fd]);
  }
}
