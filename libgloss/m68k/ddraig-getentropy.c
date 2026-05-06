/*
	Copyright (C) 2025 Mikael Hildenborg
	SPDX-License-Identifier: BSD-2-Clause
*/

#include <_ansi.h>
#include <_syslist.h>
#include <errno.h>

int getentropy(void *buf, size_t buflen)
{
    (void)buf;
    (void)buflen;

    errno = ENOSYS;
    return -1;
}
