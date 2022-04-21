#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include "io.h"

#include "ddraig.h"

int32_t _file_to_bios_flags(int f)
{
    int32_t result = 0;
 
    if (f == O_RDONLY)
        result |= FA_READ;
    if (f & O_WRONLY)
        result |= FA_WRITE | FA_CREATE_ALWAYS;
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

int32_t _bios_to_error_code(int err)
{
    int32_t result = 0;
 
    switch (err)
    {
        case 0:
            return 0;
        case FR_DISK_ERR:
            return EIO;
        case FR_INT_ERR:
            return EIO;
        case FR_NOT_READY:
            return EBUSY;
        case FR_NO_FILE:
            return ENOENT;
        case FR_NO_PATH:
            return ENOENT;
        case FR_INVALID_NAME:
            return EINVAL;
        case FR_DENIED:
            return EACCES;
        case FR_EXIST:
            return EEXIST;
        case FR_INVALID_OBJECT:
            return ENXIO;
        case FR_WRITE_PROTECTED:
            return EROFS;
        case FR_INVALID_DRIVE:
            return ENODEV;
        case FR_NOT_ENABLED:
            return ENXIO;
        case FR_NO_FILESYSTEM:
            return ENXIO;
        case FR_MKFS_ABORTED:
            return EINTR;
        case FR_TIMEOUT:
            return EAGAIN;
        case FR_LOCKED:
            return ENOSPC;
        case FR_NOT_ENOUGH_CORE:
            return EINTR;
        case FR_TOO_MANY_OPEN_FILES:
            return EMFILE;
        case FR_INVALID_PARAMETER:
            return EINVAL;
        default:
            return EIO;
    }

    return result;
}
