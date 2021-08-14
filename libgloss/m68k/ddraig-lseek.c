#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include "io.h"

#include "ddraig.h"

/* use BIOS call to seek to file position
 * 
 * CALL:
 * sys.d0 = file number
 * sys.d1 = count
 * sys.d2 = whence
 * 
 * RETURN:
 * return code = file position
 * sys.d0 = file position
 * sys.d1 = errno;
 */

off_t lseek (int fd, off_t offset, int whence)
{
  	syscall_data sys;
    int ret;

    sys.command = DISK_FILESEEK;
    sys.d0 = fd;
    sys.d1 = offset;
    sys.d2 = whence;

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
