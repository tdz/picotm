/* Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "fifo_tx.h"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <picotm/picotm-error.h>
#include <picotm/picotm-lib-array.h>
#include <picotm/picotm-lib-ptr.h>
#include <picotm/picotm-lib-tab.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "fcntlop.h"
#include "fcntloptab.h"
#include "ioop.h"
#include "iooptab.h"

static struct fifo_tx*
fifo_tx_of_file_tx(struct file_tx* file_tx)
{
    assert(file_tx);
    assert(file_tx_file_type(file_tx) == PICOTM_LIBC_FILE_TYPE_FIFO);

    return picotm_containerof(file_tx, struct fifo_tx, base);
}

static void
ref(struct file_tx* file_tx)
{
    fifo_tx_ref(fifo_tx_of_file_tx(file_tx));
}

static void
unref(struct file_tx* file_tx)
{
    fifo_tx_unref(fifo_tx_of_file_tx(file_tx));
}

static void
fifo_tx_try_rdlock_field(struct fifo_tx* self, enum fifo_field field,
                         struct picotm_error* error)
{
    assert(self);

    fifo_try_rdlock_field(self->fifo, field, self->rwstate + field, error);
}

static void
fifo_tx_try_wrlock_field(struct fifo_tx* self, enum fifo_field field,
                         struct picotm_error* error)
{
    assert(self);

    fifo_try_wrlock_field(self->fifo, field, self->rwstate + field, error);
}

static void
init_rwstates(struct picotm_rwstate* beg, const struct picotm_rwstate* end)
{
    while (beg < end) {
        picotm_rwstate_init(beg);
        ++beg;
    }
}

static void
uninit_rwstates(struct picotm_rwstate* beg, const struct picotm_rwstate* end)
{
    while (beg < end) {
        picotm_rwstate_uninit(beg);
        ++beg;
    }
}

static void
unlock_rwstates(struct picotm_rwstate* beg, const struct picotm_rwstate* end,
                struct fifo* fifo)
{
    enum fifo_field field = 0;

    while (beg < end) {
        fifo_unlock_field(fifo, field, beg);
        ++field;
        ++beg;
    }
}

static off_t
append_to_iobuffer(struct fifo_tx* self, size_t nbyte, const void* buf,
                   struct picotm_error* error)
{
    off_t bufoffset;

    assert(self);

    bufoffset = self->wrbuflen;

    if (nbyte && buf) {

        /* resize */
        void* tmp = picotm_tabresize(self->wrbuf,
                                     self->wrbuflen,
                                     self->wrbuflen+nbyte,
                                     sizeof(self->wrbuf[0]),
                                     error);
        if (picotm_error_is_set(error)) {
            return (off_t)-1;
        }
        self->wrbuf = tmp;

        /* append */
        memcpy(self->wrbuf+self->wrbuflen, buf, nbyte);
        self->wrbuflen += nbyte;
    }

    return bufoffset;
}

static int
fifo_tx_append_to_writeset(struct fifo_tx* self, size_t nbyte, off_t offset,
                          const void* buf, struct picotm_error* error)
{
    assert(self);

    off_t bufoffset = append_to_iobuffer(self, nbyte, buf, error);
    if (picotm_error_is_set(error)) {
        return -1;
    }

    unsigned long res = iooptab_append(&self->wrtab,
                                       &self->wrtablen,
                                       &self->wrtabsiz,
                                       nbyte, offset, bufoffset,
                                       error);
    if (picotm_error_is_set(error)) {
        return -1;
    }
    return res;
}

/*
 * accept()
 */

static int
accept_exec(struct file_tx* base, struct ofd_tx* ofd_tx, int sockfd,
            struct sockaddr* address, socklen_t* address_len, bool isnoundo,
            int* cookie, struct picotm_error* error)
{
    picotm_error_set_errno(error, ENOTSOCK);
    return -1;
}

/*
 * bind()
 */

static int
bind_exec(struct file_tx* base, struct ofd_tx* ofd_tx, int sockfd,
          const struct sockaddr* address, socklen_t addresslen, bool isnoundo,
          int* cookie, struct picotm_error* error)
{
    picotm_error_set_errno(error, ENOTSOCK);
    return -1;
}

/*
 * connect()
 */

static int
connect_exec(struct file_tx* base, struct ofd_tx* ofd_tx, int sockfd,
             const struct sockaddr* address, socklen_t addresslen,
             bool isnoundo, int* cookie, struct picotm_error* error)
{
    picotm_error_set_errno(error, ENOTSOCK);
    return -1;
}

/*
 * fchmod()
 */

static int
fchmod_exec(struct file_tx* base, struct ofd_tx* ofd_tx, int fildes,
            mode_t mode, bool isnoundo, int* cookie,
            struct picotm_error* error)
{
    picotm_error_set_errno(error, EINVAL);
    return -1;
}

/*
 * fcntl()
 */

static int
fcntl_exec(struct file_tx* base, struct ofd_tx* ofd_tx, int fildes, int cmd,
           union fcntl_arg* arg, bool isnoundo, int* cookie,
           struct picotm_error* error)
{
    struct fifo_tx* self = fifo_tx_of_file_tx(base);

    switch (cmd) {
        case F_GETFD:
        case F_GETFL:
        case F_GETOWN: {

            /* Read-lock FIFO */
            fifo_tx_try_rdlock_field(self, FIFO_FIELD_STATE, error);
            if (picotm_error_is_set(error)) {
                return -1;
            }

            int res = TEMP_FAILURE_RETRY(fcntl(fildes, cmd));
            arg->arg0 = res;
            if (res < 0) {
                picotm_error_set_errno(error, errno);
                return res;
            }
            return res;
        }
        case F_GETLK: {

            /* Read-lock FIFO */
            fifo_tx_try_rdlock_field(self, FIFO_FIELD_STATE, error);
            if (picotm_error_is_set(error)) {
                return -1;
            }

            int res = TEMP_FAILURE_RETRY(fcntl(fildes, cmd, arg->arg1));
            if (res < 0) {
                picotm_error_set_errno(error, errno);
                return res;
            }
            return res;
        }
        case F_SETFL:
        case F_SETFD:
        case F_SETOWN: {

            if (!isnoundo) {
                picotm_error_set_revocable(error);
                return -1;
            }

            int res = TEMP_FAILURE_RETRY(fcntl(fildes, cmd, arg->arg0));
            if (res < 0) {
                picotm_error_set_errno(error, errno);
                return res;
            }
            return res;
        }
        case F_SETLK:
        case F_SETLKW: {

            if (!isnoundo) {
                picotm_error_set_revocable(error);
                return -1;
            }

            int res = TEMP_FAILURE_RETRY(fcntl(fildes, cmd, arg->arg1));
            if (res < 0) {
                picotm_error_set_errno(error, errno);
                return res;
            }
            return res;
        }
        default:
            picotm_error_set_errno(error, EINVAL);
            return -1;
    }
}

static void
fcntl_apply(struct file_tx* base, struct ofd_tx* ofd_tx, int fildes,
            int cookie, struct picotm_error* error)
{ }

static void
fcntl_undo(struct file_tx* base, struct ofd_tx* ofd_tx, int fildes,
           int cookie, struct picotm_error* error)
{ }

/*
 * fstat()
 */

static int
fstat_exec(struct file_tx* base, struct ofd_tx* ofd_tx, int fildes,
           struct stat* buf, bool isnoundo, int* cookie,
           struct picotm_error* error)
{
    struct fifo_tx* self = fifo_tx_of_file_tx(base);

    /* Acquire file-mode reader lock. */
    fifo_tx_try_rdlock_field(self, FIFO_FIELD_FILE_MODE, error);
    if (picotm_error_is_set(error)) {
        picotm_error_set_errno(error, errno);
        return -1;
    }

    int res = fstat(fildes, buf);
    if (res < 0) {
        picotm_error_set_errno(error, errno);
        return res;
    }
    return res;
}

static void
fstat_apply(struct file_tx* base, struct ofd_tx* ofd_tx, int fildes,
            int cookie, struct picotm_error* error)
{ }

static void
fstat_undo(struct file_tx* base, struct ofd_tx* ofd_tx, int fildes,
           int cookie, struct picotm_error* error)
{ }

/*
 * fsync()
 */

static int
fsync_exec(struct file_tx* base, struct ofd_tx* ofd_tx, int fildes,
           bool isnoundo, int* cookie, struct picotm_error* error)
{
    picotm_error_set_errno(error, EINVAL);
    return -1;
}

/*
 * listen()
 */

static int
listen_exec(struct file_tx* base, struct ofd_tx* ofd_tx, int sockfd,
            int backlog, bool isnoundo, int* cookie,
            struct picotm_error* error)
{
    picotm_error_set_errno(error, ENOTSOCK);
    return -1;
}

/*
 * lseek()
 */

static off_t
lseek_exec(struct file_tx* base, struct ofd_tx* ofd_tx, int fildes,
           off_t offset, int whence, bool isnoundo, int* cookie,
           struct picotm_error* error)
{
    picotm_error_set_errno(error, ESPIPE);
    return (off_t)-1;
}

/*
 * pread()
 */

static ssize_t
pread_exec(struct file_tx* base, struct ofd_tx* ofd_tx, int fildes, void* buf,
           size_t nbyte, off_t off, bool isnoundo, int* cookie,
           struct picotm_error* error)
{
    picotm_error_set_errno(error, ESPIPE);
    return -1;
}

/*
 * pwrite()
 */

static ssize_t
pwrite_exec(struct file_tx* base, struct ofd_tx* ofd_tx, int fildes,
            const void* buf, size_t nbyte, off_t off, bool isnoundo,
            int* cookie, struct picotm_error* error)
{
    picotm_error_set_errno(error, ESPIPE);
    return -1;
}

/*
 * read()
 */

static bool
errno_signals_blocking_io(int errno_code)
{
    return (errno_code == EAGAIN) || (errno_code == EWOULDBLOCK);
}

static ssize_t
do_read(int fildes, void* buf, size_t nbyte, struct picotm_error* error)
{
    uint8_t* pos = buf;
    const uint8_t* beg = pos;
    const uint8_t* end = beg + nbyte;

    while (pos < end) {

        ssize_t res = TEMP_FAILURE_RETRY(read(fildes, pos, end - pos));
        if (res < 0) {
            if (pos != beg) {
                break; /* return read data */
            } else if (errno_signals_blocking_io(errno)) {
                return -1; /* error for non-blocking I/O */
            } else {
                picotm_error_set_errno(error, errno);
                return res;
            }
        } else if (!res) {
            break; /* EOF reached */
        }

        pos += res;
    }

    return pos - beg;
}

static ssize_t
read_exec(struct file_tx* base, struct ofd_tx* ofd_tx, int fildes, void* buf,
          size_t nbyte, bool isnoundo, int* cookie,
          struct picotm_error* error)
{
    if (!isnoundo) {
        picotm_error_set_revocable(error);
        return -1;
    }

    struct fifo_tx* self = fifo_tx_of_file_tx(base);

    fifo_tx_try_wrlock_field(self, FIFO_FIELD_READ_END, error);
    if (picotm_error_is_set(error)) {
        return (off_t)-1;
    }

    ssize_t res = do_read(fildes, buf, nbyte, error);
    if (picotm_error_is_set(error)) {
        return res;
    }
    return res;
}

static void
read_apply(struct file_tx* base, struct ofd_tx* ofd_tx, int fildes,
           int cookie, struct picotm_error* error)
{ }

static void
read_undo(struct file_tx* base, struct ofd_tx* ofd_tx, int fildes,
          int cookie, struct picotm_error* error)
{ }

/*
 * recv()
 */

static ssize_t
recv_exec(struct file_tx* base, struct ofd_tx* ofd_tx, int sockfd,
          void* buffer, size_t length, int flags, bool isnoundo, int* cookie,
          struct picotm_error* error)
{
    picotm_error_set_errno(error, ENOTSOCK);
    return -1;
}

/*
 * send()
 */

static ssize_t
send_exec(struct file_tx* base, struct ofd_tx* ofd_tx, int sockfd,
          const void* buffer, size_t length, int flags, bool isnoundo,
          int* cookie, struct picotm_error* error)
{
    picotm_error_set_errno(error, ENOTSOCK);
    return -1;
}

/*
 * shutdown()
 */

static int
shutdown_exec(struct file_tx* base, struct ofd_tx* ofd_tx, int sockfd,
              int how, bool isnoundo, int* cookie, struct picotm_error* error)
{
    picotm_error_set_errno(error, ENOTSOCK);
    return -1;
}

/*
 * write()
 */

static ssize_t
do_write(int fildes, const void* buf, size_t nbyte,
         struct picotm_error* error)
{
    const uint8_t* pos = buf;
    const uint8_t* beg = pos;
    const uint8_t* end = beg + nbyte;

    while (pos < end) {

        ssize_t res = TEMP_FAILURE_RETRY(write(fildes, pos, end - pos));
        if (res < 0) {
            if (pos != beg) {
                break; /* return written data */
            } else if (errno_signals_blocking_io(errno)) {
                return -1; /* error for non-blocking I/O */
            } else {
                picotm_error_set_errno(error, errno);
                return res;
            }
        }

        pos += res;
    }

    return pos - beg;
}

static ssize_t
write_exec(struct file_tx* base, struct ofd_tx* ofd_tx, int fildes,
           const void* buf, size_t nbyte, bool isnoundo, int* cookie,
           struct picotm_error* error)
{
    struct fifo_tx* self = fifo_tx_of_file_tx(base);

    if (isnoundo) {
        self->wrmode = PICOTM_LIBC_WRITE_THROUGH;
    } else if (self->wrmode == PICOTM_LIBC_WRITE_THROUGH) {
        picotm_error_set_revocable(error);
        return -1;
    }

    /* Write-lock FIFO write end */
    fifo_tx_try_wrlock_field(self, FIFO_FIELD_WRITE_END, error);
    if (picotm_error_is_set(error)) {
        return -1;
    }

    ssize_t res;

    if (self->wrmode == PICOTM_LIBC_WRITE_THROUGH) {
        res = do_write(fildes, buf, nbyte, error);
        if (picotm_error_is_set(error)) {
            return res;
        }
    } else {

        /* Register write data */
        *cookie = fifo_tx_append_to_writeset(self, nbyte, 0, buf, error);
        if (picotm_error_is_set(error)) {
            return -1;
        }
        res = nbyte;
    }

    return res;
}

static void
write_apply(struct file_tx* base, struct ofd_tx* ofd_tx, int fildes,
            int cookie, struct picotm_error* error)
{
    struct fifo_tx* self = fifo_tx_of_file_tx(base);

    if (self->wrmode == PICOTM_LIBC_WRITE_BACK) {
        do_write(fildes,
                 self->wrbuf + self->wrtab[cookie].bufoff,
                 self->wrtab[cookie].nbyte, error);
        if (picotm_error_is_set(error)) {
            return;
        }
    }
}

static void
write_undo(struct file_tx* base, struct ofd_tx* ofd_tx, int fildes,
           int cookie, struct picotm_error* error)
{ }

/*
 * Public interfaces
 */

static void
lock(struct file_tx* base, struct picotm_error* error)
{ }

static void
unlock(struct file_tx* base, struct picotm_error* error)
{ }

/* Validation
 */

static void
validate(struct file_tx* base, struct picotm_error* error)
{ }

/* Update CC
 */

static void
update_cc(struct file_tx* base, struct picotm_error* error)
{
    struct fifo_tx* self = fifo_tx_of_file_tx(base);

    /* release reader/writer locks on FIFO state */
    unlock_rwstates(picotm_arraybeg(self->rwstate),
                    picotm_arrayend(self->rwstate),
                    self->fifo);
}

/* Clear CC
 */

static void
clear_cc(struct file_tx* base, struct picotm_error* error)
{
    struct fifo_tx* self = fifo_tx_of_file_tx(base);

    /* release reader/writer locks on FIFO state */
    unlock_rwstates(picotm_arraybeg(self->rwstate),
                    picotm_arrayend(self->rwstate),
                    self->fifo);
}

/*
 * Public interfaces
 */

static const struct file_tx_ops fifo_tx_ops = {
    PICOTM_LIBC_FILE_TYPE_FIFO,
    /* ref counting */
    ref,
    unref,
    /* module interfaces */
    lock,
    unlock,
    validate,
    update_cc,
    clear_cc,
    /* file ops */
    accept_exec,
    NULL,
    NULL,
    bind_exec,
    NULL,
    NULL,
    connect_exec,
    NULL,
    NULL,
    fchmod_exec,
    NULL,
    NULL,
    fcntl_exec,
    fcntl_apply,
    fcntl_undo,
    fstat_exec,
    fstat_apply,
    fstat_undo,
    fsync_exec,
    NULL,
    NULL,
    listen_exec,
    NULL,
    NULL,
    lseek_exec,
    NULL,
    NULL,
    pread_exec,
    NULL,
    NULL,
    pwrite_exec,
    NULL,
    NULL,
    read_exec,
    read_apply,
    read_undo,
    recv_exec,
    NULL,
    NULL,
    send_exec,
    NULL,
    NULL,
    shutdown_exec,
    NULL,
    NULL,
    write_exec,
    write_apply,
    write_undo
};

void
fifo_tx_init(struct fifo_tx* self)
{
    assert(self);

    picotm_ref_init(&self->ref, 0);

    file_tx_init(&self->base, &fifo_tx_ops);

    self->fifo = NULL;

    self->wrmode = PICOTM_LIBC_WRITE_BACK;

    self->wrbuf = NULL;
    self->wrbuflen = 0;
    self->wrbufsiz = 0;

    self->wrtab = NULL;
    self->wrtablen = 0;
    self->wrtabsiz = 0;

    self->rdtab = NULL;
    self->rdtablen = 0;
    self->rdtabsiz = 0;

    self->fcntltab = NULL;
    self->fcntltablen = 0;

    init_rwstates(picotm_arraybeg(self->rwstate),
                  picotm_arrayend(self->rwstate));
}

void
fifo_tx_uninit(struct fifo_tx* self)
{
    assert(self);

    fcntloptab_clear(&self->fcntltab, &self->fcntltablen);
    iooptab_clear(&self->wrtab, &self->wrtablen);
    iooptab_clear(&self->rdtab, &self->rdtablen);
    free(self->wrbuf);

    uninit_rwstates(picotm_arraybeg(self->rwstate),
                    picotm_arrayend(self->rwstate));
}

/*
 * Referencing
 */

void
fifo_tx_ref_or_set_up(struct fifo_tx* self, struct fifo* fifo,
                      struct picotm_error* error)
{
    assert(self);
    assert(fifo);

    bool first_ref = picotm_ref_up(&self->ref);
    if (!first_ref) {
        return;
    }

    /* get reference on FIFO */
    fifo_ref(fifo);

    /* setup fields */

    self->fifo = fifo;

    self->wrmode = PICOTM_LIBC_WRITE_BACK;

    self->fcntltablen = 0;
    self->rdtablen = 0;
    self->wrtablen = 0;
    self->wrbuflen = 0;
}

void
fifo_tx_ref(struct fifo_tx* self)
{
    picotm_ref_up(&self->ref);
}

void
fifo_tx_unref(struct fifo_tx* self)
{
    assert(self);

    bool final_ref = picotm_ref_down(&self->ref);
    if (!final_ref) {
        return;
    }

    fifo_unref(self->fifo);
    self->fifo = NULL;
}

bool
fifo_tx_holds_ref(struct fifo_tx* self)
{
    assert(self);

    return picotm_ref_count(&self->ref) > 0;
}

