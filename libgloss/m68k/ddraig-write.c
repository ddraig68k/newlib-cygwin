#include <unistd.h>
#include <errno.h>
#include "io.h"

#include "ddraig.h"

/* use BIOS call to write file
 * 
 * CALL:
 * sys.d0 = file number
 * sys.d1 = count
 * sys.a0 = buffer 
 * 
 * RETURN:
 * return code = number of bytes written
 * sys.d0 = number of bytes written
 * sys.d1 = errno;
 */

ssize_t write (int fd, const void *buf, size_t count)
{
  	syscall_data sys;
    int ret;

    if (fd == STDIN_FILENO)
    {
        errno = EBADF;
        return -1;
    }

    sys.command = DISK_FILEWRITE;
    sys.a0 = (void *) buf;
    sys.d0 = fd;
    sys.d1 = count;

    ret = ddraig_trap15(&sys);

  	errno = _bios_to_error_code(sys.d1);
  	return ret;
}
