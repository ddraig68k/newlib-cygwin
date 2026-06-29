#include <errno.h>
#include "ddraig.h"

int chdir(const char *path)
{
    syscall_data sys;
    int ret;

    sys.command = DISK_CHDIR;
    sys.a0      = (void *)path;

    __asm__ volatile(
    "move.l %1, %%a0\n"
    "trap   #15\n"
    "move.l %%d0, %0\n"
    : "=g" (ret)
    : "g" (&sys)
    : "%a0"
    );

    if (ret != 0)
    {
        errno = _bios_to_error_code(sys.d1);
        return -1;
    }
    return 0;
}
