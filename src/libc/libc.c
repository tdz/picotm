/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "systx/systx-libc.h"

static enum systx_libc_cc_mode g_file_type_cc_mode[] = {
    SYSTX_LIBC_CC_MODE_NOUNDO,
    SYSTX_LIBC_CC_MODE_2PL,
    SYSTX_LIBC_CC_MODE_TS,
    SYSTX_LIBC_CC_MODE_TS
};

static enum systx_libc_validation_mode g_validation_mode;

SYSTX_EXPORT
void
systx_libc_set_file_type_cc_mode(enum systx_libc_file_type file_type,
                                 enum systx_libc_cc_mode cc_mode)
{
    return __atomic_store_n(g_file_type_cc_mode + file_type, cc_mode, __ATOMIC_RELEASE);
}

SYSTX_EXPORT
enum systx_libc_cc_mode
systx_libc_get_file_type_cc_mode(enum systx_libc_file_type file_type)
{
    return __atomic_load_n(g_file_type_cc_mode + file_type, __ATOMIC_ACQUIRE);
}

SYSTX_NOTHROW
void
systx_libc_set_validation_mode(enum systx_libc_validation_mode val_mode)
{
    __atomic_store_n(&g_validation_mode, val_mode, __ATOMIC_RELEASE);
}

SYSTX_NOTHROW
enum systx_libc_validation_mode
systx_libc_get_validation_mode()
{
    return __atomic_load_n(&g_validation_mode, __ATOMIC_ACQUIRE);
}