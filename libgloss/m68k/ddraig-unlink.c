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

	ret = ddraig_trap15(&sys);

  	errno = _bios_to_error_code(sys.d1);
  	return ret;
}
