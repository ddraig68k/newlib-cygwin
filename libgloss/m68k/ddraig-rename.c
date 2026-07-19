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

	ret = ddraig_trap15(&sys);

  	errno = _bios_to_error_code(sys.d1);
  	return ret;
}
