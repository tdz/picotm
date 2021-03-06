/*
 * picotm - A system-level transaction manager
 * Copyright (c) 2017-2018  Thomas Zimmermann <contact@tzimmermann.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#pragma once

#include "picotm/picotm-lib-ref.h"
#include "picotm/picotm-lib-rwstate.h"
#include "picotm/picotm-libc.h"
#include <sys/types.h>
#include "chrdev.h"
#include "file_tx.h"

/**
 * \cond impl || libc_impl || libc_impl_fd
 * \ingroup libc_impl
 * \ingroup libc_impl_fd
 * \file
 * \endcond
 */

struct picotm_error;

/**
 * Holds transaction-local state for a character device.
 */
struct chrdev_tx {

    struct picotm_ref16 ref;

    struct file_tx base;

    struct chrdev* chrdev;

    enum picotm_libc_write_mode wrmode;

    unsigned char* wrbuf;
    size_t         wrbuflen;
    size_t         wrbufsiz;

    struct ioop* wrtab;
    size_t       wrtablen;
    size_t       wrtabsiz;

    struct fcntlop* fcntltab;
    size_t          fcntltablen;

    /** State of the local reader/writer locks */
    struct picotm_rwstate rwstate[NUMBER_OF_CHRDEV_FIELDS];
};

/**
 * Initialize transaction-local character device.
 * \param   self    The instance of `struct chrdev` to initialize.
 */
void
chrdev_tx_init(struct chrdev_tx* self);

/**
 * Uninitialize transaction-local character device.
 * \param   self    The instance of `struct chrdev` to initialize.
 */
void
chrdev_tx_uninit(struct chrdev_tx* self);

/**
 * Acquire a reference on the open file description
 */
void
chrdev_tx_ref_or_set_up(struct chrdev_tx* self, struct chrdev* chrdev,
                        struct picotm_error* error);

/**
 * Acquire a reference on the open file description
 */
void
chrdev_tx_ref(struct chrdev_tx* self, struct picotm_error* error);

/**
 * Release reference
 */
void
chrdev_tx_unref(struct chrdev_tx* self);

/**
 * Returns true if transactions hold a reference
 */
bool
chrdev_tx_holds_ref(struct chrdev_tx* self);
