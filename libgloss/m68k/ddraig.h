#ifndef __DDRAIG_H__
#define __DDRAIG_H__

#include <sys/types.h>

typedef struct
{
    u_int32_t    command;
	u_int32_t	d0, d1, d2;
	void        *a0, *a1, *a2;
} syscall_data;

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


#define SYS_NOP				      0
#define SYS_TIMEDATE		    1

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
