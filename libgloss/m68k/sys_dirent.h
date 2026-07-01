/*
 * sys/dirent.h — DdraigOS platform implementation.
 *
 * This file must be installed to the toolchain sysroot as:
 *   $(CROSSDIR)/m68k-ddraig-elf/include/sys/dirent.h
 *
 * TODO: wire this into the libgloss Makefile so it is installed automatically
 * when the toolchain is rebuilt.  Until then, copy it manually after install.
 *
 * DIR._os_handle holds the handle number returned by DISK_OPENDIR.
 * DIR._cur is the storage for the entry returned by the most recent readdir().
 * The BSP implementation is ddraig-dir.c in the same directory as this file.
 */

#ifndef _SYS_DIRENT_H
#define _SYS_DIRENT_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct dirent {
    ino_t d_ino;
    char  d_name[256];
};

typedef struct {
    int            _os_handle;
    struct dirent  _cur;
} DIR;

#ifdef __cplusplus
}
#endif

#endif /* _SYS_DIRENT_H */
