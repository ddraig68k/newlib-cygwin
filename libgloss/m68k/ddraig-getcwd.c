#include <errno.h>
#include <stddef.h>
#include "ddraig.h"

char *getcwd(char *buf, size_t size)
{
    syscall_data sys;
    int ret;

    sys.command = DISK_GETCWD;
    sys.d0      = (u_int32_t)size;
    sys.a0      = buf;

    ret = ddraig_trap15(&sys);

    if (ret != 0)
    {
        errno = _bios_to_error_code(sys.d1);
        return NULL;
    }
    return buf;
}
