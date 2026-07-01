#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "ddraig.h"

static int do_disk_fcntl(int fd, int cmd, int arg)
{
    syscall_data sys;
    int ret;

    sys.command = DISK_FCNTL;
    sys.d0      = (u_int32_t)fd;
    sys.d1      = (u_int32_t)cmd;
    sys.d2      = (u_int32_t)arg;

    __asm__ volatile(
        "move.l %1, %%a0\n"
        "trap   #15\n"
        "move.l %%d0, %0\n"
        : "=g" (ret)
        : "g"  (&sys)
        : "%a0"
    );

    if (ret < 0)
        errno = EBADF;
    return ret;
}

int dup(int oldfd)
{
    return do_disk_fcntl(oldfd, BIOS_F_DUPFD, 0);
}

int dup2(int oldfd, int newfd)
{
    syscall_data sys;
    int ret;

    if (oldfd == newfd)
        return newfd;

    sys.command = DISK_DUP2;
    sys.d0      = (u_int32_t)oldfd;
    sys.d1      = (u_int32_t)newfd;

    __asm__ volatile(
        "move.l %1, %%a0\n"
        "trap   #15\n"
        "move.l %%d0, %0\n"
        : "=g" (ret)
        : "g"  (&sys)
        : "%a0"
    );

    if (ret < 0)
        errno = EBADF;
    return ret;
}
