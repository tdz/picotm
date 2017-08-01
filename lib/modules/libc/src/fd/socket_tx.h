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

#pragma once

#include <picotm/picotm-lib-ref.h>
#include <picotm/picotm-lib-rwstate.h>
#include <sys/queue.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "file_tx.h"
#include "picotm/picotm-libc.h"
#include "socket.h"

/**
 * \cond impl || libc_impl || libc_impl_fd
 * \ingroup libc_impl
 * \ingroup libc_impl_fd
 * \file
 * \endcond
 */

struct picotm_error;

/**
 * Holds transaction-local reads and writes for an open file description
 */
struct socket_tx {

    struct picotm_ref16 ref;

    SLIST_ENTRY(socket_tx) active_list;

    struct file_tx base;

    struct socket* socket;

    unsigned char* wrbuf;
    size_t         wrbuflen;
    size_t         wrbufsiz;

    struct ioop* wrtab;
    size_t       wrtablen;
    size_t       wrtabsiz;

    struct ioop* rdtab;
    size_t       rdtablen;
    size_t       rdtabsiz;

    struct fcntlop* fcntltab;
    size_t          fcntltablen;

    /** CC mode of domain */
    enum picotm_libc_cc_mode cc_mode;

    /** State of the local reader/writer locks. */
    struct picotm_rwstate rwstate[NUMBER_OF_SOCKET_FIELDS];
};

/**
 * Init transaction-local open-file-description state
 */
void
socket_tx_init(struct socket_tx* self);

/**
 * Uninit state
 */
void
socket_tx_uninit(struct socket_tx* self);

/**
 * Validate the local state
 */
void
socket_tx_validate(struct socket_tx* self, struct picotm_error* error);

/**
 * Updates the data structures for concurrency control after a successful apply
 */
void
socket_tx_update_cc(struct socket_tx* self, struct picotm_error* error);

/**
 * Clears the data structures for concurrency control after a successful undo
 */
void
socket_tx_clear_cc(struct socket_tx* self, struct picotm_error* error);

/**
 * Acquire a reference on the open file description
 */
void
socket_tx_ref_or_set_up(struct socket_tx* self, struct socket* socket,
                        int fildes, struct picotm_error* error);

/**
 * Acquire a reference on the open file description
 */
void
socket_tx_ref(struct socket_tx* self);

/**
 * Release reference
 */
void
socket_tx_unref(struct socket_tx* self);

/**
 * Returns true if transactions hold a reference
 */
bool
socket_tx_holds_ref(struct socket_tx* self);

int
socket_tx_append_to_writeset(struct socket_tx* self, size_t nbyte,
                             off_t offset, const void* buf,
                             struct picotm_error* error);

int
socket_tx_append_to_readset(struct socket_tx* self, size_t nbyte,
                            off_t offset, const void* buf,
                            struct picotm_error* error);

/**
 * Prepares the open file description for commit
 */
void
socket_tx_lock(struct socket_tx* self);

/**
 * Finishes commit for open file description
 */
void
socket_tx_unlock(struct socket_tx* self);
