#include <errno.h>

#include "ddraig.h"

/* Heap start symbol from the linker script — used as fallback for ET_EXEC. */
extern char __end[] __attribute__((aligned(4)));
static char *heap_ptr;

/*
 * sbrk -- adjust heap break by nbytes, return previous break.
 *
 * For PIE processes the OS tracks the heap limit (stack_limit) and the
 * current break, so we delegate entirely to SYS_BRK. This lets the heap
 * grow across the full DRAM arena allocated by the loader.
 *
 * For ET_EXEC (non-PIE) or kernel calls, SYS_BRK returns -1 and we fall
 * back to local pointer arithmetic anchored at __end.
 */
char *sbrk(int nbytes)
{
    syscall_data sys;
    int ret;

    sys.command = SYS_BRK;
    sys.d0      = (u_int32_t)nbytes;

    ret = ddraig_trap14(&sys);

    if (ret != -1)
        return (char *)ret;

    /* Fallback: simple pointer bump for non-PIE programs. */
    if (!heap_ptr)
        heap_ptr = (char *)&__end;

    char *base = heap_ptr;
    heap_ptr  += nbytes;

    /* Rough sanity check: stay within the 8 MB DRAM window. */
    if (heap_ptr > (char *)(8 * 1024 * 1024))
    {
        heap_ptr = base;
        errno = ENOMEM;
        return (char *)-1;
    }

    return base;
}
