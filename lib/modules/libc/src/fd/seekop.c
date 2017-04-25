/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <assert.h>
#include <stdio.h>
#include <sys/types.h>
#include "seekop.h"

int
seekop_init(struct seekop *seekop, off_t from, off_t offset, int whence)
{
    assert(seekop);

    seekop->from = from;
    seekop->offset = offset;
    seekop->whence = whence;

    return 0;
}

void
seekop_dump(struct seekop *seekop)
{
    fprintf(stderr, "seekop %p %ld %ld %d", (void*)seekop, (long)seekop->from,
                                                           (long)seekop->offset,
                                                                 seekop->whence);
}
