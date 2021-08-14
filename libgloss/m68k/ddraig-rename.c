#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "io.h"

#include "ddraig.h"

/* use BIOS call to rename file
 * 
 * CALL:
 * sys.a0 = oldpath 
 * sys.a1 = newpath 
 * 
 * RETURN:
 * return code = errno
 * sys.d1 = errno;
 */

int _rename (const char *oldpath, const char *newpath)
{
  	syscall_data sys;
	int ret;

	sys.command = DISK_FILERENAME;
	sys.a0 = oldpath;
	sys.a1 = newpath;

	__asm__ volatile(
	"move.l	%1, %%a0\n"
	"trap	#15\n"
	"move.l %%d0, %0\n"
	: "=g" (ret)
	: "g" (&sys)
	: "%a0"
	);

  	errno = sys.d1;
  	return ret;
}
