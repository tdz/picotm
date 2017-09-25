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

#include "module.h"
#include <assert.h>
#include <picotm/picotm-module.h>
#include <stdlib.h>
#include "error_tx.h"

struct error_module {
    struct error_tx tx;
    bool            is_initialized;
};

/*
 * Module interface
 */

static void
errno_undo(struct error_module* module, struct picotm_error* error)
{
    error_tx_undo(&module->tx, error);
}

static void
errno_finish(struct error_module* module, struct picotm_error* error)
{
    error_tx_finish(&module->tx, error);
}

static void
errno_release(struct error_module* module)
{
    error_tx_uninit(&module->tx);
    module->is_initialized = false;
}

/*
 * Thread-local data
 */

static void
undo_cb(void* data, struct picotm_error* error)
{
    errno_undo(data, error);
}

static void
finish_cb(void* data, struct picotm_error* error)
{
    errno_finish(data, error);
}

static void
uninit_cb(void* data)
{
    errno_release(data);
}

static struct error_tx*
get_error_tx(bool initialize, struct picotm_error* error)
{
    static __thread struct error_module t_module;

    if (t_module.is_initialized) {
        return &t_module.tx;
    } else if (!initialize) {
        return NULL;
    }

    unsigned long module = picotm_register_module(NULL, NULL, NULL,
                                                  NULL, undo_cb,
                                                  NULL, NULL,
                                                  NULL, NULL,
                                                  finish_cb,
                                                  uninit_cb,
                                                  &t_module,
                                                  error);
    if (picotm_error_is_set(error)) {
        return NULL;
    }

    error_tx_init(&t_module.tx, module);

    t_module.is_initialized = true;

    return &t_module.tx;
}

static struct error_tx*
get_non_null_error_tx(void)
{
    do {
        struct picotm_error error = PICOTM_ERROR_INITIALIZER;

        struct error_tx* error_tx = get_error_tx(true, &error);

        if (!picotm_error_is_set(&error)) {
            /* assert here as there's no legal way that error_tx
             * could be NULL */
            assert(error_tx);
            return error_tx;
        }

        picotm_recover_from_error(&error);
    } while (true);
}

/*
 * Public interface
 */

void
error_module_save_errno()
{
    struct error_tx* error_tx = get_non_null_error_tx();

    /* We only have to save 'errno' once per transaction. */
    if (error_tx_errno_saved(error_tx)) {
        return;
    }

    error_tx_save_errno(error_tx);
}

void
error_module_set_error_recovery(enum picotm_libc_error_recovery recovery)
{
    return error_tx_set_error_recovery(get_non_null_error_tx(), recovery);
}

enum picotm_libc_error_recovery
error_module_get_error_recovery()
{
    return error_tx_get_error_recovery(get_non_null_error_tx());
}