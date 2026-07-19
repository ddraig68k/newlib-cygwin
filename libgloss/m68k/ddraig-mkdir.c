#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include "ddraig.h"

int mkdir(const char *path, mode_t mode)
{
    (void)mode;  /* FAT has no Unix permission bits */

    if (path == NULL)
    {
        errno = ENOENT;
        return -1;
    }

    syscall_data sys;
    int ret;

    sys.command = DISK_MKDIR;
    sys.a0      = (void *)path;

    ret = ddraig_trap15(&sys);

    if (ret < 0)
        errno = EACCES;
    return ret;
}

/* rmdir on FatFS: f_unlink works on empty directories. */
int rmdir(const char *path)
{
    if (path == NULL)
    {
        errno = ENOENT;
        return -1;
    }

    syscall_data sys;
    int ret;

    sys.command = DISK_FILEDELETE;
    sys.a0      = (void *)path;

    ret = ddraig_trap15(&sys);

    if (ret < 0)
        errno = EACCES;
    return ret;
}
