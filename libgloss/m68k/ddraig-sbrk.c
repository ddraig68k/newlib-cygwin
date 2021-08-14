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
