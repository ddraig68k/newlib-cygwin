#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include "io.h"

#include "ddraig.h"

/* use BIOS call to get file FIL struct
 * 
 * CALL:
 * sys.d0 = file id
 * 
 * RETURN:
 * return code = error code
 * sys.a0 = pointer to FIL struct;
 * sys.d1 = errno;
 */
int fstat (int fd, struct stat *buf)
{
  	syscall_data sys;
	int ret;

	sys.command = DISK_FILESTAT;
	sys.d0 = fd;

	__asm__ volatile(
	"move.l	%1, %%a0\n"
	"trap	#15\n"
	"move.l %%d0, %0\n"
	: "=g" (ret)
	: "g" (&sys)
	: "%a0"
	);

  	errno = sys.d1;
	if (ret < 0)
  		return ret;
		  
	FIL *filedata = sys.a0;

    // This is a file, return the file length.
    buf->st_mode |= S_IFREG;
    buf->st_size = f_size(filedata);
    buf->st_blksize = FF_MAX_SS;
    buf->st_blocks = (f_size(filedata) + (FF_MAX_SS - 1)) / FF_MAX_SS;
	buf->st_nlink = 1;

	return 0;
}