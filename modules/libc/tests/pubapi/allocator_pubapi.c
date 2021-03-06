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

#include "picotm/stddef.h"
#include "picotm/stdlib.h"
#include "picotm/stdlib-tm.h"
#include "picotm/picotm.h"
#include "picotm/picotm-tm-ctypes.h"
#include "ptr.h"
#include "safe_stdlib.h"
#include "test.h"
#include "testhlp.h"

/**
 * Allocate and free within transaction.
 */
static void
allocator_test_1(unsigned int tid)
{
    picotm_begin

        char* mem[10];

        for (char** ptr = mem; ptr < mem + arraylen(mem); ++ptr) {
            *ptr = calloc_tx(15, 34);
        }

        for (char** ptr = mem; ptr < mem + arraylen(mem); ++ptr) {
            const char *beg = *ptr;
            const char *end = *ptr + (15 * 34);
            while (beg < end) {
                if (*beg) {
                    abort_tx();
                }
                ++beg;
            }
        }

        delay_transaction_tx(tid);

        for (char** ptr = mem; ptr < mem + arraylen(mem); ++ptr) {
            free_tm(*ptr);
        }

    picotm_commit

        abort_transaction_on_error(__func__);

    picotm_end
}

/**
 * Allocate outside of transaction; free within transaction.
 */
static void
allocator_test_2(unsigned int tid)
{
    char* buf[10];

    for (char** ptr = buf; ptr < buf + arraylen(buf); ++ptr) {
        *ptr = safe_calloc(15, 34);
    }

    picotm_begin

        char* mem[10];

        for (size_t i = 0; i < arraylen(mem); ++i) {
            mem[i] = load_ptr_tx(buf + i);
        }

        for (char** ptr = mem; ptr < mem + arraylen(mem); ++ptr) {
            const char *beg = *ptr;
            const char *end = *ptr + (15 * 34);
            while (beg < end) {
                if (*beg) {
                    abort_tx();
                }
                ++beg;
            }
        }

        delay_transaction_tx(tid);

        for (char** ptr = mem; ptr < mem + arraylen(mem); ++ptr) {
            free_tm(*ptr);
        }

    picotm_commit

        abort_transaction_on_error(__func__);

    picotm_end
}

/**
 * Allocate within transaction; free outside of transaction.
 */
static void
allocator_test_3(unsigned int tid)
{
    char* buf[10];

    picotm_begin

        char* mem[10];

        for (char** ptr = mem; ptr < mem + arraylen(mem); ++ptr) {
            *ptr = calloc_tx(15, 34);
        }

        for (char** ptr = mem; ptr < mem + arraylen(mem); ++ptr) {
            const char *beg = *ptr;
            const char *end = *ptr + (15 * 34);
            while (beg < end) {
                if (*beg) {
                    abort_tx();
                }
                ++beg;
            }
        }

        delay_transaction_tx(tid);

        for (size_t i = 0; i < arraylen(buf); ++i) {
            store_ptr_tx(buf + i, mem[i]);
        }

    picotm_commit

        abort_transaction_on_error(__func__);

    picotm_end

    for (char** ptr = buf; ptr < buf + arraylen(buf); ++ptr) {
        free(*ptr);
    }
}

/**
 * Allocate, realloc and free within transaction.
 */
static void
allocator_test_4(unsigned int tid)
{
    picotm_begin

        uint8_t* mem = malloc_tx(30 * sizeof(mem[0]));

        for (uint8_t i = 0; i < 30; ++i) {
            mem[i] = i;
        }

        mem = realloc_tm(mem, 15 * 34 * sizeof(mem[0]));

        for (uint8_t i = 0; i < 30; ++i) {
            if (mem[i] != i) {
                abort_tx();
            }
        }

        for (int i = 0; i < (15 * 34); ++i) {
            mem[i] = 0;
        }

        delay_transaction_tx(tid);

        free_tm(mem);

    picotm_commit

        abort_transaction_on_error(__func__);

    picotm_end
}

/**
 * Allocate outside of transaction; realloc and free within transaction.
 */
static void
allocator_test_5(unsigned int tid)
{
    uint8_t* buf = safe_malloc(30 * sizeof(buf[0]));

    for (uint8_t i = 0; i < 30; ++i) {
        buf[i] = i;
    }

    picotm_begin

        /* Load 'buf' into transaction instead of using it
         * directly. The return value of realloc_tx() is
         * stored under the same name. So restarting the
         * transaction after realloc_tx() would free the
         * memory in 'buf' and create a dangling pointer.
         *
         * _Always_ loading and storing non-transactional
         * variables explicitly is ideomatic and avoids all
         * naming problems. */
        uint8_t* mem = load_ptr_tx(&buf);

        mem = realloc_tm(mem, 15 * 34 * sizeof(mem[0]));

        for (uint8_t i = 0; i < 30; ++i) {
            if (mem[i] != i) {
                abort_tx();
            }
        }

        for (int i = 0; i < (15 * 34); ++i) {
            mem[i] = 0;
        }

        delay_transaction_tx(tid);

        free_tm(mem);

    picotm_commit

        abort_transaction_on_error(__func__);

    picotm_end
}

/**
 * Allocate outside of transaction; realloc within transaction; free
 * outside of transaction.
 */
static void
allocator_test_6(unsigned int tid)
{
    uint8_t* buf = safe_malloc(30 * sizeof(buf[0]));

    for (uint8_t i = 0; i < 30; ++i) {
        buf[i] = i;
    }

    picotm_begin

        uint8_t* mem = load_ptr_tx(&buf);

        mem = realloc_tm(mem, 15 * 34 * sizeof(mem[0]));

        for (uint8_t i = 0; i < 30; ++i) {
            if (mem[i] != i) {
                abort_tx();
            }
        }

        for (int i = 0; i < (15 * 34); ++i) {
            mem[i] = 0;
        }

        delay_transaction_tx(tid);

        /* _Always_ store results in non-transactional variables
         * explicitly, even if the variable is thread-local or
         * otherwise not shared.
         *
         * This is ideomatic and will work in all cases.
         */
        store_ptr_tx(&buf, mem);

    picotm_commit

        abort_transaction_on_error(__func__);

    picotm_end

    free(buf);
}

static unsigned long long g_test_7_cycles;

static void
allocator_test_7_pre(unsigned long nthreads, enum loop_mode loop,
                     enum boundary_type btype, unsigned long long bound)
{
    switch (btype) {
        case CYCLE_BOUND:
            g_test_7_cycles = bound;
            break;
        case TIME_BOUND:
            g_test_7_cycles = 10;
            break;
    }
}

/**
 * Multiple allocations and frees within transaction.
 */
static void
allocator_test_7(unsigned int tid)
{
    const size_t tid_min32 = tid < 32 ? 32 : tid;

    picotm_begin

        unsigned long long cycles = load_ullong_tx(&g_test_7_cycles);
        size_t siz = load_size_t_tx(&tid_min32);

        for (unsigned long long i = 0; i < cycles; ++i) {
            uint8_t* mem = malloc_tx(siz);
            mem[tid] = 1;       /* touch memory to prevent optimization */
            free_tm(mem);
        }

    picotm_commit

        abort_transaction_on_error(__func__);

    picotm_end
}

static unsigned long long g_test_8_cycles;

static void
allocator_test_8_pre(unsigned long nthreads, enum loop_mode loop,
                     enum boundary_type btype, unsigned long long bound)
{
    switch (btype) {
        case CYCLE_BOUND:
            g_test_8_cycles = bound;
            break;
        case TIME_BOUND:
            g_test_8_cycles = 10;
            break;
    }
}

/**
 * Multiple allocations and frees outside of transaction. Gives
 * non-transactional base line for comparing with allocator_test_9().
 */
static void
allocator_test_8(unsigned int tid)
{
    for (unsigned long long i = 0; i < g_test_8_cycles; ++i) {
        uint8_t* mem = safe_malloc(32);
        mem[tid] = 1;   /* touch memory to prevent optimization */
        free(mem);
    }
}

static unsigned long long g_test_9_cycles;

static void
allocator_test_9_pre(unsigned long nthreads, enum loop_mode loop,
                     enum boundary_type btype, unsigned long long bound)
{
    switch (btype) {
        case CYCLE_BOUND:
            g_test_9_cycles = bound;
            break;
        case TIME_BOUND:
            g_test_9_cycles = 10;
            break;
    }
}

/**
 * Multiple allocations ands frees within transaction.
 */
static void
allocator_test_9(unsigned int tid)
{
    picotm_begin

        unsigned long long cycles = load_ullong_tx(&g_test_9_cycles);

        for (unsigned long long i = 0; i < cycles; ++i) {
            uint8_t* mem = malloc_tx(32);
            mem[tid] = 1;   /* touch memory to prevent optimization */
            free_tm(mem);
        }

    picotm_commit

        abort_transaction_on_error(__func__);

    picotm_end
}

static const struct test_func allocator_test[] = {
    {"allocator_test_1", allocator_test_1, NULL, NULL},
    {"allocator_test_2", allocator_test_2, NULL, NULL},
    {"allocator_test_3", allocator_test_3, NULL, NULL},
    {"allocator_test_4", allocator_test_4, NULL, NULL},
    {"allocator_test_5", allocator_test_5, NULL, NULL},
    {"allocator_test_6", allocator_test_6, NULL, NULL},
    {"allocator_test_7", allocator_test_7, allocator_test_7_pre, NULL},
    {"allocator_test_8", allocator_test_8, allocator_test_8_pre, NULL},
    {"allocator_test_9", allocator_test_9, allocator_test_9_pre, NULL}
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
                       allocator_test, arraylen(allocator_test));
}
