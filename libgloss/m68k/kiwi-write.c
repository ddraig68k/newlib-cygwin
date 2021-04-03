/* write.c -- write bytes to an output device.
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

extern int  outbyte(char x);

/*
 * write -- write bytes to the serial port. Ignore fd, since
 *          stdout and stderr are the same. Since we have no filesystem,
 *          open will only return an error.
 */
ssize_t write (int fd, const void *buf, size_t count)
{
  int i;
  char *ptr = (char *)buf;

  if (fd < 3) // fd 0-2 = stdout, stdin, stderr
  {
    for (i = 0; i < count; i++)
    {
      if (ptr[i] == '\n')
      {
        outbyte ('\r');
      }
      outbyte(ptr[i]);
    }
  }
  else
  {
    // other fd have to be checked (name) for correct device
    fl_fwrite(buf, count, 1, fl_table[fd]);
  }

  return count;
}
