/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include "counter.h"
#include "picotm/picotm-libc.h"

#define FDTX_FL_LOCALSTATE 1L /** \brief Signals local state changes */

struct fcntlop;

union fcntl_arg;

/**
 * Holds transaction-local reads and writes for a file descriptor
 */
struct fd_tx {
    int fildes;
    int ofd;
    enum picotm_libc_cc_mode cc_mode;

	unsigned long flags;

    struct fcntlop* fcntltab;
    size_t          fcntltablen;

    count_type fdver; /* Last fd version, modified by fdtx */
};

/**
 * Init transaction-local file-descriptor state
 */
int
fd_tx_init(struct fd_tx* self);

/**
 * Uninit state
 */
void
fd_tx_uninit(struct fd_tx* self);

/**
 * Validate transaction-local state
 */
int
fd_tx_validate(struct fd_tx* self);

int
fd_tx_update_cc(struct fd_tx* self);

int
fd_tx_clear_cc(struct fd_tx* self);

/**
 * Aquire a reference on file-descriptor state or validate
 */
enum error_code
fd_tx_ref_or_validate(struct fd_tx* self, int fildes, unsigned long flags);

/**
 * Aquire a reference on file-descriptor state
 */
enum error_code
fd_tx_ref(struct fd_tx* self, int fildes, unsigned long flags);

/**
 * Release reference
 */
void
fd_tx_unref(struct fd_tx* self);

/**
 * Returns non-zero if transaction holds a reference on file descriptor
 */
int
fd_tx_holds_ref(const struct fd_tx* self);

int
fd_tx_pre_commit(struct fd_tx* self);

int
fd_tx_post_commit(struct fd_tx* self);

/**
 * Set file descriptor to CLOSING
 */
void
fd_tx_signal_close(struct fd_tx* self);

/**
 * Dump file-descriptor state
 */
void
fd_tx_dump(const struct fd_tx* self);

/*
 * close()
 */

int
fd_tx_close_exec(struct fd_tx* self, int fildes, int* cookie, int noundo);

int
fd_tx_close_apply(struct fd_tx* self, int fildes, int cookie);

int
fd_tx_close_undo(struct fd_tx* self, int fildes, int cookie);

/*
 * fcntl()
 */

int
fd_tx_fcntl_exec(struct fd_tx* self, int cmd, union fcntl_arg *arg,
                 int* cookie, int noundo);

int
fd_tx_fcntl_apply(struct fd_tx* self, int cookie);

int
fd_tx_fcntl_undo(struct fd_tx* self, int cookie);