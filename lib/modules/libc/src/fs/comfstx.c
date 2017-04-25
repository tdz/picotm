/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <picotm/picotm.h>
#include <unistd.h>
#include "errcode.h"
#include "types.h"
#include "counter.h"
#include "fd/fcntlop.h"
#include "fd/fd.h"
#include "fd/fdtab.h"
#include "comfs.h"
#include "comfstx.h"

static int
com_fs_tx_lock(void *data)
{
    return com_fs_lock(data);
}

static int
com_fs_tx_unlock(void *data)
{
    com_fs_unlock(data);

    return 0;
}

static int
com_fs_tx_validate(void *data, int noundo)
{
    return com_fs_validate(data);
}

static int
com_fs_tx_apply_event(const struct event *event, size_t n, void *data)
{
    return com_fs_apply_event(data, event, n);
}

static int
com_fs_tx_undo_event(const struct event *event, size_t n, void *data)
{
    return com_fs_undo_event(data, event, n);
}

static int
com_fs_tx_finish(void *data)
{
    com_fs_finish(data);

    return 0;
}

static int
com_fs_tx_uninit(void *data)
{
    com_fs_uninit(data);

    return 0;
}

struct com_fs *
com_fs_tx_aquire_data()
{
    static __thread struct {
        bool          is_initialized;
        struct com_fs instance;
    } t_com_fs;

    if (t_com_fs.is_initialized) {
        return &t_com_fs.instance;
    }

    long res = picotm_register_module(com_fs_tx_lock,
                                     com_fs_tx_unlock,
                                     com_fs_tx_validate,
                                     com_fs_tx_apply_event,
                                     com_fs_tx_undo_event,
                                     NULL,
                                     NULL,
                                     com_fs_tx_finish,
                                     com_fs_tx_uninit,
                                     &t_com_fs.instance);
    if (res < 0) {
        return NULL;
    }
    unsigned long module = res;

    res = com_fs_init(&t_com_fs.instance, module);
    if (res < 0) {
        return NULL;
    }

    t_com_fs.is_initialized = true;

    return &t_com_fs.instance;
}

int
com_fs_tx_fchdir(int fildes)
{
    extern int com_fs_exec_fchdir(struct com_fs*, int);

    int res;

    struct com_fs *comfs = com_fs_tx_aquire_data();
    assert(comfs);

    do {
        res = com_fs_exec_fchdir(comfs, fildes);

        switch (res) {
            case ERR_CONFLICT:
            case ERR_PEERABORT:
                picotm_abort();
                break;
            case ERR_NOUNDO:
                picotm_irrevocable();
                break;
            default:
                break;
        }
    } while (res == ERR_NOUNDO);

    return res;
}

int
com_fs_tx_mkstemp(char *pathname)
{
    extern int com_fs_exec_mkstemp(struct com_fs*, char*);

    int res;

    struct com_fs *comfs = com_fs_tx_aquire_data();
    assert(comfs);

    do {
        res = com_fs_exec_mkstemp(comfs, pathname);

        switch (res) {
            case ERR_CONFLICT:
            case ERR_PEERABORT:
                picotm_abort();
                break;
            case ERR_NOUNDO:
                picotm_irrevocable();
                break;
            default:
                break;
        }
    } while (res == ERR_NOUNDO);

    return res;
}

int
com_fs_tx_chdir(const char *path)
{
    extern int com_fd_tx_open(const char*, int, mode_t);
    extern int com_fs_tx_fchdir(int);

    assert(path);

    int fildes = TEMP_FAILURE_RETRY(com_fd_tx_open(path, O_RDONLY, 0));

    if (fildes < 0) {
        return -1;
    }

    return com_fs_tx_fchdir(fildes);
}

int
com_fs_tx_chmod(const char *pathname, mode_t mode)
{
    struct com_fs *comfs = com_fs_tx_aquire_data();
    assert(comfs);

    return fchmodat(com_fs_get_cwd(comfs), pathname, mode, 0);
}

int
com_fs_tx_fchmod(int fildes, mode_t mode)
{
    /* reference file descriptor while working on it */

    struct fd *fd = fdtab+fildes;

    int err = fd_ref(fd, fildes, 0);

    if (err) {
        return err;
    }

    int res = fchmod(fildes, mode);

    fd_unref(fd, fildes);

    return res;
}

int
com_fs_tx_fstat(int fildes, struct stat *buf)
{
    /* reference file descriptor while working on it */

    struct fd *fd = fdtab+fildes;

    int err = fd_ref(fd, fildes, 0);

    if (err) {
        return err;
    }

    int res = fstat(fildes, buf);

    fd_unref(fd, fildes);

    return res;
}

char *
com_fs_tx_getcwd(char *buf, size_t size)
{
    struct com_fs *comfs = com_fs_tx_aquire_data();
    assert(comfs);

    if (!size) {
        errno = EINVAL;
        return NULL;
    }

    /* return transaction-local working directory */

    char* cwd = com_fs_get_cwd_path(comfs);
    if (!cwd) {
        return NULL;
    }

    size_t len = strlen(cwd) + sizeof(*cwd);

    if (!(size >= len)) {
        errno = ERANGE;
        goto err_size_ge_len;
    }

    memcpy(buf, cwd, len);
    free(cwd);

    return buf;

err_size_ge_len:
    free(cwd);
    return NULL;
}

int
com_fs_tx_link(const char *oldpath, const char *newpath)
{
    struct com_fs *comfs = com_fs_tx_aquire_data();
    assert(comfs);

    return linkat(com_fs_get_cwd(comfs), oldpath,
                  com_fs_get_cwd(comfs), newpath, AT_SYMLINK_FOLLOW);
}

int
com_fs_tx_lstat(const char *path, struct stat *buf)
{
    struct com_fs *comfs = com_fs_tx_aquire_data();
    assert(comfs);

    return fstatat(com_fs_get_cwd(comfs), path, buf, AT_SYMLINK_NOFOLLOW);
}

int
com_fs_tx_mkdir(const char *pathname, mode_t mode)
{
    struct com_fs *comfs = com_fs_tx_aquire_data();
    assert(comfs);

    return mkdirat(com_fs_get_cwd(comfs), pathname, mode);
}

int
com_fs_tx_mkfifo(const char *path, mode_t mode)
{
    struct com_fs *comfs = com_fs_tx_aquire_data();
    assert(comfs);

    return mkfifoat(com_fs_get_cwd(comfs), path, mode);
}

int
com_fs_tx_mknod(const char *path, mode_t mode, dev_t dev)
{
    struct com_fs *comfs = com_fs_tx_aquire_data();
    assert(comfs);

    return mknodat(com_fs_get_cwd(comfs), path, mode, dev);
}

int
com_fs_tx_stat(const char *path, struct stat *buf)
{
    struct com_fs *comfs = com_fs_tx_aquire_data();
    assert(comfs);

    return fstatat(com_fs_get_cwd(comfs), path, buf, 0);
}

int
com_fs_tx_unlink(const char *pathname)
{
    struct com_fs *comfs = com_fs_tx_aquire_data();
    assert(comfs);

    return unlinkat(com_fs_get_cwd(comfs), pathname, 0);
}
