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

	sys.command = DISK_FILECLOSE;
	sys.d0 = fd;

	__asm__ volatile(
	"move.l	%1, %%a0\n"
	"trap	#15\n"
	"move.l %%d0, %0\n"
	: "=g" (ret)
	: "g" (&sys)
	: "%a0"
	);

  	return sys.d0;
}
