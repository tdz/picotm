/* Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include <stdbool.h>
#include "log.h"
#include "module.h"
#include "picotm/picotm.h"

/**
 * \cond impl || lib_impl
 * \ingroup lib_impl
 * \file
 * \endcond
 */

#define MAX_NMODULES    (256)

struct picotm_error;
struct tx_shared;

enum tx_mode {
    TX_MODE_REVOCABLE,
    TX_MODE_IRREVOCABLE
};

struct tx {
    struct __picotm_tx public_state;
    struct log        log;
    struct tx_shared *shared;
    enum tx_mode      mode;

    unsigned long nmodules; /**< \brief Number allocated modules */
    struct module module[MAX_NMODULES]; /** \brief Registered modules */

    bool is_initialized;
};

void
tx_init(struct tx* self, struct tx_shared* tx_shared);

void
tx_release(struct tx* self);

bool
tx_is_irrevocable(const struct tx* self);

unsigned long
tx_register_module(struct tx* self,
                   void (*lock)(void*, struct picotm_error*),
                   void (*unlock)(void*, struct picotm_error*),
                   void (*validate)(void*, int, struct picotm_error*),
                   void (*apply)(void*, struct picotm_error*),
                   void (*undo)(void*, struct picotm_error*),
                   void (*apply_event)(const struct picotm_event*,
                                       void*, struct picotm_error*),
                   void (*undo_event)(const struct picotm_event*,
                                      void*, struct picotm_error*),
                   void (*updatecc)(void*, int, struct picotm_error*),
                   void (*clearcc)(void*, int, struct picotm_error*),
                   void (*finish)(void*, struct picotm_error*),
                   void (*uninit)(void*),
                   void* data,
                   struct picotm_error* error);

void
tx_append_event(struct tx* self, unsigned long module, unsigned long op,
                uintptr_t cookie, struct picotm_error* error);

void
tx_begin(struct tx* self, enum tx_mode mode, struct picotm_error* error);

void
tx_commit(struct tx* self, struct picotm_error* error);

void
tx_rollback(struct tx* self, struct picotm_error* error);

bool
tx_is_valid(struct tx* self);