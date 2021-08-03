#ifndef __DDRAIG_H__
#define __DDRAIG_H__

#include <sys/types.h>

typedef struct
{
    u_int32_t    command;
	u_int32_t	d0, d1, d2;
	void        *a0, *a1, *a2;
} syscall_data;


#define DISK_NOP            0
#define DISK_FILEOPEN       1
#define DISK_FILECLOSE      2


#endif
