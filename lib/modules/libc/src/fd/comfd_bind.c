/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/socket.h>
#include "errcode.h"
#include "types.h"
#include "seekop.h"
#include "rwlock.h"
#include "counter.h"
#include "pgtree.h"
#include "pgtreess.h"
#include "cmap.h"
#include "cmapss.h"
#include "rwlockmap.h"
#include "rwstatemap.h"
#include "fcntlop.h"
#include "ofdid.h"
#include "ofd.h"
#include "ofdtx.h"
#include "fd.h"
#include "fdtx.h"
#include "comfd.h"

int
com_fd_exec_bind(struct com_fd *data, int socket, const struct sockaddr *address,
                                              socklen_t addresslen, int isnoundo)
{
    /* Update/create fdtx */

    struct fdtx *fdtx = com_fd_get_fdtx(data, socket);
    assert(fdtx);

    enum error_code err = fdtx_ref_or_validate(fdtx, socket, 0);

    if (err) {
        return err;
    }

    /* Update/create ofdtx */

    struct ofdtx *ofdtx = com_fd_get_ofdtx(data, fdtx->ofd);
    assert(ofdtx);

    int optcc;
    err = ofdtx_ref(ofdtx, fdtx->ofd, socket, 0, &optcc);

    if (err) {
        return err;
    }

    com_fd_set_optcc(data, optcc);

    /* Bind */

    int cookie = -1;

    int res = ofdtx_bind_exec(ofdtx,
                              socket, address, addresslen,
                              &cookie, isnoundo);

    if (res < 0) {
        return res;
    }

    /* Inject event */
    if ((cookie >= 0)
        && (com_fd_inject(data, ACTION_BIND, socket, cookie) < 0)) {
        return -1;
    }

    return res;
}

int
com_fd_apply_bind(struct com_fd *data, const struct com_fd_event *event, size_t n)
{
    assert(data);
    assert(event || !n);

    int err = 0;

    while (n && !err) {

        assert(event->fildes >= 0);
        assert(event->fildes < sizeof(data->fdtx)/sizeof(data->fdtx[0]));

        const struct fdtx *fdtx = com_fd_get_fdtx(data, event->fildes);
        assert(fdtx);

        struct ofdtx *ofdtx = com_fd_get_ofdtx(data, fdtx->ofd);
        assert(ofdtx);

        int m = 1;

        while ((m < n) && (event[m].fildes == event->fildes)) {
            ++m;
        }

        err = ofdtx_bind_apply(ofdtx, event->fildes, event, m) < 0 ? -1 : 0;
        n -= m;
        event += m;
    }

    return err;
}

int
com_fd_undo_bind(struct com_fd *data, int fildes, int cookie)
{
    assert(data);
    assert(cookie < data->eventtablen);

    const struct com_fd_event *ev = data->eventtab+cookie;

    assert(ev->fildes >= 0);
    assert(ev->fildes < sizeof(data->fdtx)/sizeof(data->fdtx[0]));

    const struct fdtx *fdtx = com_fd_get_fdtx(data, ev->fildes);
    assert(fdtx);

    struct ofdtx *ofdtx = com_fd_get_ofdtx(data, fdtx->ofd);
    assert(ofdtx);

    return ofdtx_bind_undo(ofdtx, ev->fildes, ev->cookie) < 0 ? -1 : 0;
}
