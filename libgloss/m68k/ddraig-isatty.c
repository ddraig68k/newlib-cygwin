#include <unistd.h>
#include <errno.h>
#include "io.h"

/*
 * isatty -- check if fd is a terminal
 * input parameters:
 *   0 : file descriptor
 * output parameters:
 *   0 : result
 *   1 : errno
 */

int isatty (int fd)
{
    if (fd < 3)
        return 1;
    else
        return 0;
}
