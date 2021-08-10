
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include "io.h"

#include "ddraig.h"

int32_t __hosted_to_bios_file_flags(int f)
{
    int32_t result = 0;
 
    if (f & O_RDONLY)
        result |= FA_READ;
    if (f & O_WRONLY)
        result |= FA_WRITE;
    if (f & O_RDWR)
        result |= FA_READ | FA_WRITE;
    if (f & O_APPEND)
        result |= FA_OPEN_APPEND;
    if (f & O_CREAT)
        result |= FA_CREATE_ALWAYS;
    if (f & O_TRUNC)
        result |= FA_CREATE_NEW;

    return result;
}
