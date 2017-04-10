/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "systx/sys/stat.h"
#include "fs/comfstx.h"

SYSTX_EXPORT
int
chmod_tm(const char* path, mode_t mode)
{
    return com_fs_tx_chmod(path, mode);
}

SYSTX_EXPORT
int
fstat_tm(int fildes, struct stat* buf)
{
    return com_fs_tx_fstat(fildes, buf);
}

SYSTX_EXPORT
int
lstat_tm(const char* path, struct stat* buf)
{
    return com_fs_tx_lstat(path, buf);
}

SYSTX_EXPORT
int
mkdir_tm(const char* path, mode_t mode)
{
    return com_fs_tx_mkdir(path, mode);
}

SYSTX_EXPORT
int
mkfifo_tm(const char* path, mode_t mode)
{
    return com_fs_tx_mkfifo(path, mode);
}

#if defined(_BSD_SOURCE) || defined(_SVID_SOURCE) || _XOPEN_SOURCE >= 500
SYSTX_EXPORT
int
mknod_tm(const char* path, mode_t mode, dev_t dev)
{
    return com_fs_tx_mknod(path, mode, dev);
}
#endif

SYSTX_EXPORT
int
stat_tm(const char* path, struct stat* buf)
{
    return com_fs_tx_stat(path, buf);
}