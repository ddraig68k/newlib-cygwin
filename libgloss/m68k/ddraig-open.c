#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#include "ddraig.h"
#include "stdio.h"


/* use BIOS call to open file
 * 
 * CALL:
 * sys.d0 = file flags
 * sys.a0 = filename 
 * 
 * RETURN:
 * return code = file number
 * sys.d0 = file number
 * sys.d1 = errno;
 */

int open (const char *fname, int flags, ...)
{
  	int mode = 0;
  	syscall_data sys;
	int ret;

	sys.command = DISK_FILEOPEN;
	sys.a0 = fname;
	sys.d0 = _bios_to_file_flags(flags);
  
  	va_list ap;
  
  	va_start(ap, flags);
  	mode = va_arg(ap, int);
  	va_end(ap);

	printf("newlib:open: About to call fopen trap with file %s and flags %02x and mode %02x\n\r", fname, flags, mode);

	__asm__ volatile(
	"move.l	%1, %%a0\n"
	"trap	#15\n"
	"move.l %%d0, %0\n"
	: "=g" (ret)
	: "g" (&sys)
	: "%a0"
	);

	printf("newlib:open: returned file number %d\n\r", ret);

  	errno = _bios_to_error_code(sys.d1);
  	return ret;
}
