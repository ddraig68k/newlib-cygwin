#include <errno.h>
#include "ddraig.h"

int chdir(const char *path)
{
    syscall_data sys;
    int ret;

    sys.command = DISK_CHDIR;
    sys.a0      = (void *)path;

    ret = ddraig_trap15(&sys);

    if (ret != 0)
    {
        errno = _bios_to_error_code(sys.d1);
        return -1;
    }
    return 0;
}
