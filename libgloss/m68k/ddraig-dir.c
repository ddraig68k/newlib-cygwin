#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "ddraig.h"

/*
 * POSIX directory iteration for DdraigOS.
 *
 * The OS returns an opaque integer handle from DISK_OPENDIR.  We wrap that
 * in a malloc'd ddraig_DIR_t so callers get a stable DIR * pointer.
 *
 * Programs that include <dirent.h> need sys/dirent.h in the sysroot to
 * define struct dirent and DIR.  Add the following to
 *   $(CROSSDIR)/m68k-ddraig-elf/sys-include/sys/dirent.h
 * (or equivalent sysroot location):
 *
 *   #ifndef _SYS_DIRENT_H
 *   #define _SYS_DIRENT_H
 *   #include <sys/types.h>
 *   struct dirent {
 *       ino_t d_ino;
 *       char  d_name[256];
 *   };
 *   typedef struct { int _os_handle; struct dirent _cur; } DIR;
 *   #endif
 *
 * These declarations must match the types used here.
 */

typedef struct { int _os_handle; struct dirent _cur; } ddraig_DIR_t;

DIR *opendir(const char *path)
{
    syscall_data sys;
    int ret;

    if (path == NULL)
    {
        errno = ENOENT;
        return NULL;
    }

    sys.command = DISK_OPENDIR;
    sys.a0      = (void *)path;

    ret = ddraig_trap15(&sys);

    if (ret < 0)
    {
        errno = ENOENT;
        return NULL;
    }

    ddraig_DIR_t *d = malloc(sizeof(ddraig_DIR_t));
    if (d == NULL)
    {
        /* Close the OS handle we just opened. */
        sys.command = DISK_CLOSEDIR;
        sys.d0      = (u_int32_t)ret;
        (void)ddraig_trap15(&sys);
        errno = ENOMEM;
        return NULL;
    }

    d->_os_handle = ret;
    memset(&d->_cur, 0, sizeof(d->_cur));
    return (DIR *)d;
}

struct dirent *readdir(DIR *dirp)
{
    if (dirp == NULL)
    {
        errno = EBADF;
        return NULL;
    }

    ddraig_DIR_t *d = (ddraig_DIR_t *)dirp;
    syscall_data  sys;
    bios_dirent_t bde;
    int ret;

    sys.command = DISK_READDIR;
    sys.d0      = (u_int32_t)d->_os_handle;
    sys.a0      = &bde;

    ret = ddraig_trap15(&sys);

    if (ret != 0)
        return NULL;  /* end-of-dir (ret==1) or error (ret==-1) */

    d->_cur.d_ino = 0;
    strncpy(d->_cur.d_name, bde.fname, sizeof(d->_cur.d_name) - 1);
    d->_cur.d_name[sizeof(d->_cur.d_name) - 1] = '\0';
    return &d->_cur;
}

int closedir(DIR *dirp)
{
    if (dirp == NULL)
    {
        errno = EBADF;
        return -1;
    }

    ddraig_DIR_t *d = (ddraig_DIR_t *)dirp;
    syscall_data  sys;
    int ret;

    sys.command = DISK_CLOSEDIR;
    sys.d0      = (u_int32_t)d->_os_handle;

    ret = ddraig_trap15(&sys);

    free(d);
    return (ret < 0) ? -1 : 0;
}

void rewinddir(DIR *dirp)
{
    /* FatFS f_rewinddir is f_readdir(dp, 0) which resets the scan.
     * We replicate it by issuing DISK_READDIR with a NULL entry pointer,
     * which the OS interprets as a rewind (passes NULL to f_readdir). */
    if (dirp == NULL)
        return;

    ddraig_DIR_t *d = (ddraig_DIR_t *)dirp;
    syscall_data  sys;

    sys.command = DISK_READDIR;
    sys.d0      = (u_int32_t)d->_os_handle;
    sys.a0      = NULL;   /* NULL signals rewind to the OS */

    (void)ddraig_trap15(&sys);
}
