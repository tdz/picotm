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
#include <sys/select.h>
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
#include "fdtab.h"
#include "fdtx.h"
#include "comfd.h"

static enum error_code
ref_fdset(struct com_fd *data, int nfds, const fd_set *fdset)
{
    assert(nfds > 0);
    assert(!nfds || fdset);

    int fildes;

    for (fildes = 0; fildes < nfds; ++fildes) {
        if (FD_ISSET(fildes, fdset)) {

            /* Update/create fdtx */

            struct fdtx *fdtx = com_fd_get_fdtx(data, fildes);
            assert(fdtx);

            enum error_code err =
                fdtx_ref_or_validate(fdtx, fildes, 0);

            if (err) {
                return err;
            }
        }
    }

    return 0;
}

int
com_fd_exec_select(struct com_fd *data, int nfds, fd_set *readfds,
                                                  fd_set *writefds,
                                                  fd_set *errorfds,
                                                  struct timeval *timeout,
                                                  int isnoundo)
{
    assert(data);

    /* Ref all selected file descriptors */

    if (readfds) {
        enum error_code err = ref_fdset(data, nfds, readfds);
        if (err) {
            return err;
        }
    }
    if (writefds) {
        enum error_code err = ref_fdset(data, nfds, writefds);
        if (err) {
            return err;
        }
    }
    if (errorfds) {
        enum error_code err = ref_fdset(data, nfds, errorfds);
        if (err) {
            return err;
        }
    }

    int res;

    if (!timeout && !isnoundo) {

        /* Arbitrarily choosen default timeout of 5 sec */
        struct timeval def_timeout = {5, 0};

        res = select(nfds, readfds, writefds, errorfds, &def_timeout);

        if (!res) {
            return ERR_CONFLICT;
        }
    } else {
        res = select(nfds, readfds, writefds, errorfds, timeout);
    }

    return res;
}
