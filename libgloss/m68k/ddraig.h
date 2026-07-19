#ifndef __DDRAIG_H__
#define __DDRAIG_H__

#include <sys/types.h>

typedef struct
{
    u_int32_t    command;
	u_int32_t	d0, d1, d2;
	void        *a0, *a1, *a2;
} syscall_data;

/*
 * TRAP #14/#15 enter C code in the OS, so they have the same volatile
 * register effects as a normal C call.  Keep that compiler contract here so
 * individual syscall wrappers cannot accidentally retain values in registers
 * that the OS is allowed to overwrite.  The OS also updates syscall_data and
 * caller-provided buffers, hence the memory clobber.
 */
static __inline__ int ddraig_trap14(volatile syscall_data *sys)
{
    int ret;

    __asm__ volatile(
        "move.l %1, %%a0\n"
        "trap   #14\n"
        "move.l %%d0, %0\n"
        : "=g" (ret)
        : "g"  (sys)
        : "%d0", "%d1", "%a0", "%a1", "cc", "memory"
    );

    return ret;
}

static __inline__ int ddraig_trap15(volatile syscall_data *sys)
{
    int ret;

    __asm__ volatile(
        "move.l %1, %%a0\n"
        "trap   #15\n"
        "move.l %%d0, %0\n"
        : "=g" (ret)
        : "g"  (sys)
        : "%d0", "%d1", "%a0", "%a1", "cc", "memory"
    );

    return ret;
}

// BIOS call commands
#define DISK_NOP            0
#define DISK_FILEOPEN       1
#define DISK_FILECLOSE      2
#define DISK_FILEREAD       3
#define DISK_FILEWRITE      4
#define DISK_FILESEEK		    5
#define DISK_FILESTRUCT		  6
#define DISK_FILESTAT		    7
#define DISK_FILERENAME		  8
#define DISK_FILEDELETE		  9
#define DISK_GETCWD         10
#define DISK_CHDIR          11
#define DISK_OPENDIR        12
#define DISK_READDIR        13
#define DISK_CLOSEDIR       14
#define DISK_MKDIR          15
#define DISK_DUP2           16
#define DISK_FCNTL          17

#define SYS_NOP             0
#define SYS_TIMEDATE        1
#define SYS_BRK             60
#define SYS_EXEC            62

/* fcntl command codes recognised by DISK_FCNTL */
#define BIOS_F_DUPFD        0
#define BIOS_F_SETFD        2
#define BIOS_FD_CLOEXEC     1

/* Number of fds tracked in the process fd_redir table (fds 0..PROCESS_FD_MAX-1). */
#define PROCESS_FD_MAX      16

/* Directory entry filled by DISK_READDIR. */
typedef struct
{
    u_int32_t fsize;
    u_int8_t  fattrib;
    char      fname[256];
} bios_dirent_t;

#define BIOS_AM_DIR  0x10   /* fattrib bit: entry is a directory */

typedef struct
{
  int	tm_sec;
  int	tm_min;
  int	tm_hour;
  int	tm_day;
  int	tm_mon;
  int	tm_year;
  int	tm_dow;
} rtc_date_t;


#include "ddraig-ff.h"

// Utility functions
int32_t _file_to_bios_flags(int f);
int32_t _bios_to_error_code(int err);

#endif
