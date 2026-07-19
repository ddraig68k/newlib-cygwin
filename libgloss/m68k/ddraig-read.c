#include <unistd.h>
#include <errno.h>
#include "io.h"

#include "ddraig.h"

/* use BIOS call to read file
 * 
 * CALL:
 * sys.d0 = file number
 * sys.d1 = count
 * sys.a0 = buffer 
 * 
 * RETURN:
 * return code = number of bytes read
 * sys.d0 = number of bytes read
 * sys.d1 = errno;
 */

ssize_t read (int fd, void *buf, size_t count)
{
  	syscall_data sys;
    int ret;

    if (fd == STDOUT_FILENO || fd == STDERR_FILENO)
    {
        errno = EBADF;
        return -1;
    }

    sys.command = DISK_FILEREAD;
    sys.a0 = buf;
    sys.d0 = fd;
    sys.d1 = count;

    ret = ddraig_trap15(&sys);

  	errno = _bios_to_error_code(sys.d1);
  	return ret;
}
