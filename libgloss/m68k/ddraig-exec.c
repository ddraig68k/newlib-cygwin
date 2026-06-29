#include "ddraig.h"

/*
 * ddraig_exec -- launch an ELF child process from a running PIE program.
 *
 * The OS saves the calling process's memory context (heap pointers and OS
 * stack pointer) before starting the child and restores it on return, so the
 * parent's malloc heap and eventual clean exit are unaffected.
 *
 * Returns the child's exit code, or -1 if the path could not be found or the
 * file is not a valid ELF.
 */
int ddraig_exec(const char *path, int argc, char **argv)
{
    syscall_data sys;
    int ret;

    sys.command = SYS_EXEC;
    sys.d0      = (u_int32_t)argc;
    sys.a0      = (void *)path;
    sys.a1      = (void *)argv;

    __asm__ volatile(
        "move.l %1, %%a0\n"
        "trap   #14\n"
        "move.l %%d0, %0\n"
        : "=g" (ret)
        : "g"  (&sys)
        : "%a0"
    );

    return ret;
}
