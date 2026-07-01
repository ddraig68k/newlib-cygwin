#include <fcntl.h>
#include <errno.h>
#include <stdarg.h>
#include "ddraig.h"

/*
 * fcntl — only F_DUPFD and F_SETFD are needed by dash (via redir.c/savefd).
 * All other commands return 0 (no-op) so programs don't hard-fail.
 */
int fcntl(int fd, int cmd, ...)
{
    syscall_data sys;
    int ret;
    int arg = 0;

    if (cmd == F_DUPFD || cmd == F_DUPFD_CLOEXEC)
    {
        va_list ap;
        va_start(ap, cmd);
        arg = va_arg(ap, int);
        va_end(ap);

        sys.command = DISK_FCNTL;
        sys.d0      = (u_int32_t)fd;
        sys.d1      = (u_int32_t)BIOS_F_DUPFD;
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
            errno = EMFILE;
        return ret;
    }

    if (cmd == F_SETFD)
    {
        /* FD_CLOEXEC has no effect on DdraigOS (no fork); succeed silently. */
        return 0;
    }

    if (cmd == F_GETFD)
        return 0;

    /* Any other command: unsupported but don't fail hard. */
    return 0;
}
