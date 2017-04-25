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
#include "counter.h"
#include "pgtree.h"
#include "pgtreess.h"
#include "cmap.h"
#include "cmapss.h"
#include "rwlock.h"
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
com_fd_exec_accept(struct com_fd *data, int sockfd, struct sockaddr *address,
                                          socklen_t *address_len)
{
    /* Update/create fdtx */

    struct fdtx *fdtx = com_fd_get_fdtx(data, sockfd);
    assert(fdtx);

    enum error_code err = fdtx_ref_or_validate(fdtx, sockfd, 0);

    if (err) {
        return err;
    }

    /* Update/create ofdtx */

    struct ofdtx *ofdtx = com_fd_get_ofdtx(data, fdtx->ofd);
    assert(ofdtx);

    int optcc;
    err = ofdtx_ref(ofdtx, fdtx->ofd, sockfd, 0, &optcc);

    if (err) {
        return err;
    }

    com_fd_set_optcc(data, optcc);

    /* Accept connection */

    int connfd = TEMP_FAILURE_RETRY(accept(sockfd, address, address_len));

    if (connfd < 0) {
        return ERR_SYSTEM;
    }

    fdtx = data->fdtx+connfd;

    /* Reference fdtx */

    if ( (err = fdtx_ref(fdtx, connfd, 0)) ) {
        if (TEMP_FAILURE_RETRY(close(connfd)) < 0) {
            perror("close");
        }
        return err;
    }

    /* Inject event */
    if (com_fd_inject(data, ACTION_ACCEPT, connfd, -1) < 0) {
        return -1;
    }

    return connfd;
}

int
com_fd_apply_accept(struct com_fd *data, const struct com_fd_event *event, size_t n)
{
    assert(data);
    assert(event || !n);

    return 0;
}

int
com_fd_undo_accept(struct com_fd *data, int fildes, int cookie)
{
    assert(data);

    assert(fildes >= 0);
    assert(fildes < MAXNUMFD);

    struct fdtx *fdtx = com_fd_get_fdtx(data, fildes);
    assert(fdtx);

    /* Mark file descriptor to be closed */
    fdtx_signal_close(fdtx);

    return 0;
}
