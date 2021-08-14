#include <sys/time.h>
#include <errno.h>
#include "io.h"

/*
 * time -- get the current time
 * input parameters:
 *   0 : timeval ptr
 * output parameters:
 *   0 : result
 *   1 : errno
 */

time_t time (time_t *t)
{
  struct timeval tv;

  if (gettimeofday (&tv, 0))
    return -1;

  if (t)
    *t = tv.tv_sec;
  
  return tv.tv_sec;
}
