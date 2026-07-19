#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "ddraig.h"

/*
 * _system: run a shell command string on DdraigOS.
 *
 * Parses the command string into an argv array and calls ddraig_exec().
 * No shell features (pipes, redirects, variables) — just a bare command
 * with space-separated arguments.  This is sufficient for pdpmake invoking
 * the toolchain (vasm, vlink, vbcc, etc.).
 *
 * Returns the child exit code, or -1 if the string is empty or unparseable.
 */

#define MAX_ARGS 32

int system(const char *command)
{
    if (command == NULL)
        return 1;   /* non-zero: shell is available */

    if (*command == '\0')
        return 0;

    /* Work on a mutable copy. */
    char *buf = strdup(command);
    if (buf == NULL)
    {
        errno = ENOMEM;
        return -1;
    }

    char *argv[MAX_ARGS + 1];
    int   argc = 0;
    char *p    = buf;

    while (*p != '\0' && argc < MAX_ARGS)
    {
        /* Skip leading whitespace. */
        while (*p == ' ' || *p == '\t')
            p++;

        if (*p == '\0')
            break;

        /* Handle single-quoted arguments. */
        if (*p == '\'')
        {
            p++;
            argv[argc++] = p;
            while (*p != '\0' && *p != '\'')
                p++;
            if (*p == '\'')
                *p++ = '\0';
        }
        /* Handle double-quoted arguments (no escape processing). */
        else if (*p == '"')
        {
            p++;
            argv[argc++] = p;
            while (*p != '\0' && *p != '"')
                p++;
            if (*p == '"')
                *p++ = '\0';
        }
        else
        {
            argv[argc++] = p;
            while (*p != '\0' && *p != ' ' && *p != '\t')
                p++;
            if (*p != '\0')
                *p++ = '\0';
        }
    }

    argv[argc] = NULL;

    int ret = -1;
    if (argc > 0)
    {
        volatile syscall_data sys;

        sys.command = SYS_EXEC;
        sys.d0      = (u_int32_t)argc;
        sys.a0      = (void *)argv[0];
        sys.a1      = (void *)argv;

        ret = ddraig_trap14(&sys);
    }

    free(buf);
    /* Encode as POSIX wait-status so WIFEXITED/WEXITSTATUS work correctly. */
    return (ret >= 0) ? (ret << 8) : ret;
}
