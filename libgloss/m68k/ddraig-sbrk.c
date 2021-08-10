/* sbrk.c -- allocate memory dynamically.
 * 
 * Copyright (c) 1995,1996 Cygnus Support
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
#include <errno.h>

extern char __end[] __attribute__ ((aligned (4))); // Pointer to start of heap
static char *heap_ptr = 0;

#define RAMSIZE (1024*1024*8)

/*
 * sbrk -- changes heap size size. Get nbytes more
 *         RAM. We just increment a pointer in what's
 *         left of memory on the board.
 */
char *sbrk (int nbytes)
{
    char    *base;

    if (!heap_ptr)
        heap_ptr = (char *)&__end;

    if ((RAMSIZE - (int)heap_ptr - nbytes) >= 0)
    {
        base = heap_ptr;
        heap_ptr += nbytes;
        return (base);
    }
    else
    {
        errno = ENOMEM;
        return ((char *)-1);
    }
}
