/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include "errcode.h"
#include "types.h"
#include "counter.h"
#include "rwlock.h"
#include "pgtree.h"
#include "pgtreess.h"
#include "cmap.h"
#include "cmapss.h"
#include "rwlockmap.h"
#include "rwstatemap.h"
#include "fcntlop.h"
#include "ofdid.h"
#include "ofd.h"
#include "ofdtab.h"
#include "ioop.h"
#include "iooptab.h"
#include "ofdtx.h"

/*
 * Exec
 */

static ssize_t
ofdtx_recv_exec_noundo(struct ofdtx *ofdtx, int sockfd,
                                            void *buffer,
                                            size_t length,
                                            int flags,
                                            int *cookie)
{
    return TEMP_FAILURE_RETRY(recv(sockfd, buffer, length, flags));
}

ssize_t
ofdtx_recv_exec(struct ofdtx *ofdtx, int sockfd,
                                     void *buffer,
                                     size_t length,
                                     int flags,
                                     int *cookie, int noundo)
{
    static ssize_t (* const recv_exec[][4])(struct ofdtx*,
                                            int,
                                            void*,
                                            size_t,
                                            int,
                                            int*) = {
        {ofdtx_recv_exec_noundo, NULL, NULL, NULL},
        {ofdtx_recv_exec_noundo, NULL, NULL, NULL},
        {ofdtx_recv_exec_noundo, NULL, NULL, NULL},
        {ofdtx_recv_exec_noundo, NULL, NULL, NULL}};

    assert(ofdtx->type < sizeof(recv_exec)/sizeof(recv_exec[0]));
    assert(recv_exec[ofdtx->type]);

    if (noundo) {
        /* TX irrevokable */
        ofdtx->cc_mode = PICOTM_LIBC_CC_MODE_NOUNDO;
    } else {
        /* TX revokable */
        if ((ofdtx->cc_mode == PICOTM_LIBC_CC_MODE_NOUNDO)
            || !recv_exec[ofdtx->type][ofdtx->cc_mode]) {
            return ERR_NOUNDO;
        }
    }

    return recv_exec[ofdtx->type][ofdtx->cc_mode](ofdtx, sockfd, buffer, length, flags, cookie);
}

/*
 * Apply
 */

static ssize_t
ofdtx_recv_apply_noundo(void)
{
    return 0;
}

int
ofdtx_recv_apply(struct ofdtx *ofdtx, int sockfd, const struct com_fd_event *event, size_t n)
{
    static ssize_t (* const recv_apply[][4])(void) = {
        {ofdtx_recv_apply_noundo, NULL, NULL, NULL},
        {ofdtx_recv_apply_noundo, NULL, NULL, NULL},
        {ofdtx_recv_apply_noundo, NULL, NULL, NULL},
        {ofdtx_recv_apply_noundo, NULL, NULL, NULL}};

    assert(ofdtx->type < sizeof(recv_apply)/sizeof(recv_apply[0]));
    assert(recv_apply[ofdtx->type]);

    return recv_apply[ofdtx->type][ofdtx->cc_mode]();
}

/*
 * Undo
 */

int
ofdtx_recv_undo(struct ofdtx *ofdtx, int sockfd, int cookie)
{
    static int (* const recv_undo[][4])(struct ofdtx *, int, int) = {
        {NULL, NULL, NULL, NULL},
        {NULL, NULL, NULL, NULL},
        {NULL, NULL, NULL, NULL},
        {NULL, NULL, NULL, NULL}};

    assert(ofdtx->type < sizeof(recv_undo)/sizeof(recv_undo[0]));
    assert(recv_undo[ofdtx->type]);

    return recv_undo[ofdtx->type][ofdtx->cc_mode](ofdtx, sockfd, cookie);
}
