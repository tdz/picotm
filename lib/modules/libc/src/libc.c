/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "picotm/picotm-libc.h"

static enum picotm_libc_cc_mode g_file_type_cc_mode[] = {
    PICOTM_LIBC_CC_MODE_NOUNDO,
    PICOTM_LIBC_CC_MODE_2PL,
    PICOTM_LIBC_CC_MODE_TS,
    PICOTM_LIBC_CC_MODE_TS
};

static enum picotm_libc_validation_mode g_validation_mode;

PICOTM_EXPORT
void
picotm_libc_set_file_type_cc_mode(enum picotm_libc_file_type file_type,
                                  enum picotm_libc_cc_mode cc_mode)
{
    return __atomic_store_n(g_file_type_cc_mode + file_type, cc_mode, __ATOMIC_RELEASE);
}

PICOTM_EXPORT
enum picotm_libc_cc_mode
picotm_libc_get_file_type_cc_mode(enum picotm_libc_file_type file_type)
{
    return __atomic_load_n(g_file_type_cc_mode + file_type, __ATOMIC_ACQUIRE);
}

PICOTM_NOTHROW
void
picotm_libc_set_validation_mode(enum picotm_libc_validation_mode val_mode)
{
    __atomic_store_n(&g_validation_mode, val_mode, __ATOMIC_RELEASE);
}

PICOTM_NOTHROW
enum picotm_libc_validation_mode
picotm_libc_get_validation_mode()
{
    return __atomic_load_n(&g_validation_mode, __ATOMIC_ACQUIRE);
}