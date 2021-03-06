/*
 * picotm - A system-level transaction manager
 * Copyright (c) 2017-2018  Thomas Zimmermann <contact@tzimmermann.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "picotm/picotm.h"
#include "picotm/picotm-error.h"
#include "picotm/picotm-module.h"
#include "picotm/picotm-lib-array.h"
#include "picotm/picotm-tm-ctypes.h"
#include <stdlib.h>
#include <string.h>
#include "ptr.h"
#include "safeblk.h"
#include "safe_stdio.h"
#include "taputils.h"
#include "test.h"
#include "testhlp.h"

#define STRSIZE 128

static unsigned long g_value;
static char          g_string[STRSIZE];

/**
 * Load a shared value.
 */
static void
tm_test_1(unsigned int tid)
{
    picotm_begin

        unsigned long value = load_ulong_tx(&g_value);
        if (!(value == 0)) {
            tap_error("condition failed: value == 0");
            struct picotm_error error = PICOTM_ERROR_INITIALIZER;
            picotm_error_set_error_code(&error, PICOTM_GENERAL_ERROR);
            picotm_error_mark_as_non_recoverable(&error);
            picotm_recover_from_error(&error);
        }

    picotm_commit

        abort_transaction_on_error(__func__);

    picotm_end
}

static void
tm_test_1_pre(unsigned long nthreads, enum loop_mode loop,
              enum boundary_type btype, unsigned long long bound)
{
    g_value = 0;
}

static void
tm_test_1_post(unsigned long nthreads, enum loop_mode loop,
               enum boundary_type btype, unsigned long long bound)
{
    if (!(g_value == 0)) {
        tap_error("post-condition failed: g_value == 0");
        abort_safe_block();
    }
}

/**
 * Store to a shared value.
 */
static void
tm_test_2(unsigned int tid)
{
    picotm_begin

        store_ulong_tx(&g_value, tid);

    picotm_commit

        abort_transaction_on_error(__func__);

    picotm_end
}

static void
tm_test_2_pre(unsigned long nthreads, enum loop_mode loop,
              enum boundary_type btype, unsigned long long bound)
{
    g_value = nthreads;
}

static void
tm_test_2_post(unsigned long nthreads, enum loop_mode loop,
               enum boundary_type btype, unsigned long long bound)
{
    if (!(g_value < nthreads)) {
        tap_error("post-condition failed: g_value < nthreads");
        abort_safe_block();
    }
}

/**
 * Load, add and store a shared value.
 */
static void
tm_test_3(unsigned int tid)
{
    picotm_begin

        unsigned long value = load_ulong_tx(&g_value);
        value += 1;
        store_ulong_tx(&g_value, value);

    picotm_commit

        abort_transaction_on_error(__func__);

    picotm_end
}

static void
tm_test_3_pre(unsigned long nthreads, enum loop_mode loop,
              enum boundary_type btype, unsigned long long bound)
{
    g_value = 0;
}

static void
tm_test_3_post(unsigned long nthreads, enum loop_mode loop,
               enum boundary_type btype, unsigned long long bound)
{
    switch (btype) {
        case CYCLE_BOUND:
            if (!(g_value == (nthreads * bound))) {
                tap_error("post-condition failed: g_value == (nthreads * bound)");
                abort_safe_block();
            }
            break;
        case TIME_BOUND:
            break;
    }
}

/**
 * Store, load and compare a shared value.
 */
static void
tm_test_4(unsigned int tid)
{
    picotm_begin

        store_ulong_tx(&g_value, tid);
        unsigned long value = load_ulong_tx(&g_value);

        if (!(value == tid)) {
            tap_error("condition failed: value == tid");
            struct picotm_error error = PICOTM_ERROR_INITIALIZER;
            picotm_error_set_error_code(&error, PICOTM_GENERAL_ERROR);
            picotm_error_mark_as_non_recoverable(&error);
            picotm_recover_from_error(&error);
        }

    picotm_commit

        abort_transaction_on_error(__func__);

    picotm_end
}

static void
tm_test_4_pre(unsigned long nthreads, enum loop_mode loop,
              enum boundary_type btype, unsigned long long bound)
{
    g_value = nthreads;
}

/**
 * Privatize, store, load and compare a shared value.
 */
static void
tm_test_5(unsigned int tid)
{
    picotm_begin

        privatize_ulong_tx(&g_value, PICOTM_TM_PRIVATIZE_LOADSTORE);

        g_value = tid;
        unsigned long value = g_value;

        if (!(value == tid)) {
            tap_error("condition failed: value == tid");
            struct picotm_error error = PICOTM_ERROR_INITIALIZER;
            picotm_error_set_error_code(&error, PICOTM_GENERAL_ERROR);
            picotm_error_mark_as_non_recoverable(&error);
            picotm_recover_from_error(&error);
        }

    picotm_commit

        abort_transaction_on_error(__func__);

    picotm_end
}

static void
tm_test_5_pre(unsigned long nthreads, enum loop_mode loop,
              enum boundary_type btype, unsigned long long bound)
{
    g_value = nthreads;
}

/**
 * Privatize and memcpy a shared string.
 */
static void
tm_test_6(unsigned int tid)
{
    static const char format[] = "tid %lu";

    char istr[STRSIZE];
    char ostr[STRSIZE];
    safe_snprintf(istr, sizeof(istr), format, tid);

    picotm_begin

        privatize_c_tx(istr, '\0', PICOTM_TM_PRIVATIZE_LOAD);

        privatize_tx(ostr, sizeof(ostr), PICOTM_TM_PRIVATIZE_STORE);
        privatize_tx(g_string, sizeof(g_string), PICOTM_TM_PRIVATIZE_LOADSTORE);

        memcpy(g_string, istr, sizeof(g_string));
        memcpy(ostr, g_string, sizeof(ostr));

        g_value = tid;
        unsigned long value = g_value;

        if (!(value == tid)) {
            tap_error("condition failed: value == tid");
            struct picotm_error error = PICOTM_ERROR_INITIALIZER;
            picotm_error_set_error_code(&error, PICOTM_GENERAL_ERROR);
            picotm_error_mark_as_non_recoverable(&error);
            picotm_recover_from_error(&error);
        }

    picotm_commit

        abort_transaction_on_error(__func__);

    picotm_end

    unsigned long value;
    int len = safe_sscanf(ostr, format, &value);

    if (!(len == 1)) {
        tap_error("condition failed: len == 1");
        abort_safe_block();
    }

    if (!(value == tid)) {
        tap_error("condition failed: value == tid");
        abort_safe_block();
    }
}

/**
 * Discard a buffer and load it. Should generate an Out-of-Bounds error.
 */
static void
tm_test_7(unsigned int tid)
{
    picotm_safe bool performed_error_recovery = false;

    const char buf[8] = "";

    picotm_begin

        privatize_tx(buf, sizeof(buf), 0);

        char tx_buf[8];
        load_tx(buf, tx_buf, sizeof(tx_buf));

    picotm_commit

        if ((picotm_error_status() != PICOTM_ERROR_CODE) ||
            (picotm_error_as_error_code() != PICOTM_OUT_OF_BOUNDS)) {
            abort_transaction_on_error(__func__);
        }

        performed_error_recovery = true;
        /* leave error recovery without restarting TX */

    picotm_end

    if (!performed_error_recovery) {
        tap_error("Transaction did not perform error recovery.\n");
        abort_safe_block();
    }
}

/**
 * Discard a buffer and store to it. Should generate an
 * Out-of-Bounds error.
 */
static void
tm_test_8(unsigned int tid)
{
    picotm_safe bool performed_error_recovery = false;

    char buf[8];

    picotm_begin

        privatize_tx(buf, sizeof(buf), 0);

        const char tx_buf[8] = "";
        store_tx(buf, tx_buf, sizeof(buf));

    picotm_commit

        if ((picotm_error_status() != PICOTM_ERROR_CODE) ||
            (picotm_error_as_error_code() != PICOTM_OUT_OF_BOUNDS)) {
            abort_transaction_on_error(__func__);
        }

        performed_error_recovery = true;
        /* leave error recovery without restarting TX */

    picotm_end

    if (!performed_error_recovery) {
        tap_error("Transaction did not perform error recovery.\n");
        abort_safe_block();
    }
}

/*
 * Byte-wise load/store
 */

static void
copy_in_out(const unsigned char ibuf[256], unsigned char obuf[256],
            unsigned long off)
{
    off %= 256;

    picotm_begin

        unsigned char tbuf[256];
        memset(tbuf, 0, sizeof(tbuf));

        const unsigned char* ibeg = picotm_arrayat(ibuf, off);
        const unsigned char* iend = picotm_arrayat(ibuf, 256);
              unsigned char* tpos = picotm_arrayat(tbuf, off);

        for (const unsigned char* ipos = ibeg; ipos < iend; ++ipos, ++tpos) {
            *tpos = load_uchar_tx(ipos);
        }

        int cmp = memcmp(tbuf + off, ibuf + off, sizeof(tbuf) - off);
        if (cmp) {
            tap_error("condition failed: memcmp(tbuf, ibuf)\n");
            struct picotm_error error = PICOTM_ERROR_INITIALIZER;
            picotm_error_set_error_code(&error, PICOTM_GENERAL_ERROR);
            picotm_error_mark_as_non_recoverable(&error);
            picotm_recover_from_error(&error);
        }

        unsigned char* tbeg = picotm_arrayat(tbuf, off);
        unsigned char* tend = picotm_arrayat(tbuf, 256);
        unsigned char* opos = picotm_arrayat(obuf, off);

        for (tpos = tbeg; tpos < tend; ++tpos, ++opos) {
            store_uchar_tx(opos, *tpos);
        }

    picotm_commit
        abort_transaction_on_error(__func__);
    picotm_end
}

/**
 * Load/store complete and partial buffers, starting *anywhere* within
 * the buffer's memory region.
 */
static void
tm_test_9(unsigned int tid)
{
    static const unsigned char ibuf[] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
        0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
        0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
        0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
        0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
        0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
        0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
        0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
        0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
        0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
        0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
        0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
        0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7,
        0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
        0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
        0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
        0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
        0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
        0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
        0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
        0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
        0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
    };

    for (size_t off = 0; off < sizeof(ibuf); ++off) {

        unsigned char obuf[256];
        memset(obuf, 0, sizeof(obuf));
        copy_in_out(ibuf, obuf, off);

        int cmp = memcmp(obuf + off, ibuf + off, sizeof(obuf) - off);
        if (cmp) {
            tap_error("condition failed: memcmp(obuf, ibuf)\n");
            abort_safe_block();
        }
    }
}

/*
 * Byte-wise xchg-or-load/store
 */

static void
xcld_in_out(const unsigned char ibuf[256], unsigned char obuf[256],
            const unsigned char xbuf[256], unsigned long off)
{
    off %= 256;

    picotm_begin

        privatize_tx(xbuf, 256, PICOTM_TM_PRIVATIZE_LOAD);

        unsigned char tbuf[256];
        memset(tbuf, 0, sizeof(tbuf));

        const unsigned char* ibeg = picotm_arrayat(ibuf, off);
        const unsigned char* iend = picotm_arrayat(ibuf, 256);
        const unsigned char* xpos = picotm_arrayat(xbuf, off);
              unsigned char* tpos = picotm_arrayat(tbuf, off);

        for (const unsigned char* ipos = ibeg;
                                  ipos < iend;
                                ++ipos, ++xpos, ++tpos) {
            if (*xpos) {
                *tpos = *xpos;
            } else {
                *tpos = load_uchar_tx(ipos);
            }
        }

        unsigned char* tbeg = picotm_arrayat(tbuf, off);
        unsigned char* tend = picotm_arrayat(tbuf, 256);
        unsigned char* opos = picotm_arrayat(obuf, off);

        for (tpos = tbeg; tpos < tend; ++tpos, ++opos) {
            store_uchar_tx(opos, *tpos);
        }

    picotm_commit
        abort_transaction_on_error(__func__);
    picotm_end
}

/**
 * Xchg-or-load/store complete and partial buffers, starting *anywhere* within
 * the buffer's memory region.
 */
static void
tm_test_10(unsigned int tid)
{
    static const unsigned char ibuf[] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
        0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
        0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
        0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
        0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
        0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
        0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
        0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
        0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
        0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
        0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
        0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
        0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7,
        0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
        0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
        0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
        0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
        0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
        0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
        0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
        0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
        0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
    };

    /* Prim-number indices are not loaded */
    static const unsigned char xbuf[] = {
        0x00, 0x00, 0xff, 0xff, 0x00, 0xff, 0x00, 0xff,
        0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0x00,
        0x00, 0xff, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00,
        0x00, 0xff, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff,
        0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
        0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0xff,
        0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0x00,
        0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
        0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0xff, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
        0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
        0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0x00,
        0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
        0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00
    };

    for (size_t off = 0; off < sizeof(ibuf); ++off) {

        unsigned char obuf[256];
        memset(obuf, 0, sizeof(obuf));
        xcld_in_out(ibuf, obuf, xbuf, off);

        const unsigned char* obeg = picotm_arrayat(obuf, off);
        const unsigned char* oend = picotm_arrayat(obuf, 256);
        const unsigned char* xpos = picotm_arrayat(xbuf, off);
        const unsigned char* ipos = picotm_arrayat(ibuf, off);

        for (const unsigned char* opos = obeg;
                                  opos < oend;
                                ++opos, ++xpos, ++ipos) {
            bool eq;
            if (*xpos) {
                eq = (*opos == *xpos);
            } else {
                eq = (*opos == *ipos);
            }
            if (!eq) {
                tap_error("condition failed: obuf == (ibuf ? xbuf)\n");
                abort_safe_block();
            }
        }
    }
}

/*
 * Byte-wise load/xchg-or-store
 */

static void
xcst_in_out(const unsigned char ibuf[256], unsigned char obuf[256],
            const unsigned char xbuf[256], unsigned long off)
{
    off %= 256;

    picotm_begin

        privatize_tx(xbuf, 256, PICOTM_TM_PRIVATIZE_LOAD);

        unsigned char tbuf[256];
        memset(tbuf, 0, sizeof(tbuf));

        const unsigned char* ibeg = picotm_arrayat(ibuf, off);
        const unsigned char* iend = picotm_arrayat(ibuf, 256);
              unsigned char* tpos = picotm_arrayat(tbuf, off);

        for (const unsigned char* ipos = ibeg; ipos < iend; ++ipos, ++tpos) {
            *tpos = load_uchar_tx(ipos);
        }

              unsigned char* tbeg = picotm_arrayat(tbuf, off);
              unsigned char* tend = picotm_arrayat(tbuf, 256);
              unsigned char* opos = picotm_arrayat(obuf, off);
        const unsigned char* xpos = picotm_arrayat(xbuf, off);

        for (tpos = tbeg; tpos < tend; ++tpos, ++xpos, ++opos) {
            if (*xpos) {
                store_uchar_tx(opos, *xpos);
            } else {
                store_uchar_tx(opos, *tpos);
            }
        }

    picotm_commit
        abort_transaction_on_error(__func__);
    picotm_end
}

/**
 * Load/xchg-or-store complete and partial buffers, starting *anywhere* within
 * the buffer's memory region.
 */
static void
tm_test_11(unsigned int tid)
{
    static const unsigned char ibuf[] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
        0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
        0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
        0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
        0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
        0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
        0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
        0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
        0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
        0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
        0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
        0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
        0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7,
        0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
        0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
        0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
        0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
        0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
        0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
        0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
        0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
        0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
    };

    /* Prim-number indices are not loaded */
    static const unsigned char xbuf[] = {
        0x00, 0x00, 0xff, 0xff, 0x00, 0xff, 0x00, 0xff,
        0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0x00,
        0x00, 0xff, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00,
        0x00, 0xff, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff,
        0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
        0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0xff,
        0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0x00,
        0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
        0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0xff, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
        0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
        0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0x00,
        0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
        0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00
    };

    for (size_t off = 0; off < sizeof(ibuf); ++off) {

        unsigned char obuf[256];
        memset(obuf, 0, sizeof(obuf));
        xcst_in_out(ibuf, obuf, xbuf, off);

        const unsigned char* obeg = picotm_arrayat(obuf, off);
        const unsigned char* oend = picotm_arrayat(obuf, 256);
        const unsigned char* xpos = picotm_arrayat(xbuf, off);
        const unsigned char* ipos = picotm_arrayat(ibuf, off);

        for (const unsigned char* opos = obeg;
                                  opos < oend;
                                ++opos, ++xpos, ++ipos) {
            bool eq;
            if (*xpos) {
                eq = (*opos == *xpos);
            } else {
                eq = (*opos == *ipos);
            }
            if (!eq) {
                tap_error("condition failed: obuf == (ibuf ? xbuf)\n");
                abort_safe_block();
            }
        }
    }
}

static const struct test_func tm_test[] = {
    {"tm_test_1", tm_test_1, tm_test_1_pre, tm_test_1_post},
    {"tm_test_2", tm_test_2, tm_test_2_pre, tm_test_2_post},
    {"tm_test_3", tm_test_3, tm_test_3_pre, tm_test_3_post},
    {"tm_test_4", tm_test_4, tm_test_4_pre, NULL},
    {"tm_test_5", tm_test_5, tm_test_5_pre, NULL},
    {"tm_test_6", tm_test_6, NULL, NULL},
    {"tm_test_7", tm_test_7, NULL, NULL},
    {"tm_test_8", tm_test_8, NULL, NULL},
    {"Byte-wise load/store", tm_test_9, NULL, NULL},
    {"Byte-wise conditional-load/store", tm_test_10, NULL, NULL},
    {"Byte-wise load/conditional-store", tm_test_11, NULL, NULL}
};

/*
 * Entry point
 */

#include "opts.h"
#include "pubapi.h"

int
main(int argc, char* argv[])
{
    return pubapi_main(argc, argv, PARSE_OPTS_STRING(),
                       tm_test, arraylen(tm_test));
}
