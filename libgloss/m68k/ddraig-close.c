#include <unistd.h>
#include <errno.h>
#include "io.h"

#include "ddraig.h"
#include "stdio.h"
#include "memory.h"

/* use BIOS call to clsoe file
 * 
 * CALL:
 * sys.d0 = file number
 * 
 * RETURN:
 * return code = error code
 */

int close (int fd)
{
	int ret;
  	volatile syscall_data sys;

	if (fd >= STDIN_FILENO && fd <= STDERR_FILENO)
		return 0;

	sys.command = DISK_FILECLOSE;
	sys.d0 = fd;

	ret = ddraig_trap15(&sys);

	return ret;
}
