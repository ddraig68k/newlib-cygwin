#include <unistd.h>
#include <errno.h>
#include "io.h"

#include "ddraig.h"

extern char inbyte (void);

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

    if (fd == STDIN_FILENO)
    {
        char *p = buf;
        size_t n;

        for (n = 0; n < count; n++)
            p[n] = inbyte ();

        return count;
    }

    if (fd == STDOUT_FILENO || fd == STDERR_FILENO)
    {
        errno = EBADF;
        return -1;
    }

    sys.command = DISK_FILEREAD;
    sys.a0 = buf;
    sys.d0 = fd;
    sys.d1 = count;

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
