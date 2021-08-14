#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include "io.h"

/*
 * system: execute command on (remote) host
 * input parameters:
 *   0 : command ptr
 *   1 : command length
 * output parameters:
 *   0 : result
 *   1 : errno
 */

int _system (const char *command)
{
  if (!command)
    return 0;

  errno = ENOSYS;
  return -1;
}
