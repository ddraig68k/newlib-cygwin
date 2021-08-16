#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <memory.h>
#include "io.h"

#include "ddraig.h"
#include "stdio.h"

/* use BIOS call to get file stat
 * 
 * CALL:
 * sys.a0 = filename 
 * sys.a1 = file stat struct
 * 
 * RETURN:
 * return code = errno
 * sys.d1 = errno;
 */

int stat (const char *__restrict filename, struct stat *__restrict buf)
{
  	syscall_data sys;
	int ret;

	FILINFO fileinfo;

	memset(&fileinfo, 0, sizeof(FILINFO));

	//printf("Calling stat on file %s\n\r", filename);

	sys.command = DISK_FILESTAT;
	sys.a0 = (void *)filename;
	sys.a1 = &fileinfo;

	__asm__ volatile(
	"move.l	%1, %%a0\n"
	"trap	#15\n"
	"move.l %%d0, %0\n"
	: "=g" (ret)
	: "g" (&sys)
	: "%a0"
	);

	//printf("stat returned %d\n\r", ret);

  	errno = _bios_to_error_code(sys.d1);
	if (ret < 0)
  		return ret;

    // This is a file, return the file length.
    buf->st_mode |= S_IFREG;
    buf->st_size = fileinfo.fsize;
    buf->st_blksize = FF_MAX_SS;
    buf->st_blocks = (fileinfo.fsize + (FF_MAX_SS - 1)) / FF_MAX_SS;
	buf->st_nlink = 1;

	return 0;

}
