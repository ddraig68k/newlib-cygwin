#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "io.h"

#include "ddraig.h"

/* use BIOS call to delete file
 * 
 * CALL:
 * sys.a0 = filename 
 * 
 * RETURN:
 * return code = file error code
 * sys.d1 = errno;
 */

int unlink (const char *path)
{
  	syscall_data sys;
	int ret;

	sys.command = DISK_FILEDELETE;
	sys.a0 = path;

	__asm__ volatile(
	"move.l	%1, %%a0\n"
	"trap	#15\n"
	"move.l %%d0, %0\n"
	: "=g" (ret)
	: "g" (&sys)
	: "%a0"
	);

  	errno = _bios_to_error_code(sys.d1);
  	return ret;
}
