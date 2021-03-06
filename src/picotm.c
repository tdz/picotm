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

#include "picotm.h"
#include "picotm/picotm-lib-global-state.h"
#include "picotm/picotm-lib-shared-state.h"
#include "picotm/picotm-lib-state.h"
#include "picotm/picotm-lib-thread-state.h"
#include <assert.h>
#include <errno.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "picotm_lock_manager.h"
#include "picotm_tx.h"

/*
 * Shared state
 */

struct global_state {
    /**
     * The global lock manager for all transactional locks. Register your
     * transaction's lock-owner instance with this object.
     */
    struct picotm_lock_manager lm;
};

static void
init_global_shared_state_fields(struct global_state* global,
                                struct picotm_error* error)
{
    picotm_lock_manager_init(&global->lm, error);
}

static void
uninit_global_shared_state_fields(struct global_state* global)
{
    picotm_lock_manager_uninit(&global->lm);
}

PICOTM_SHARED_STATE(global_state, struct global_state);
PICOTM_SHARED_STATE_STATIC_IMPL(global_state, struct global_state,
                                init_global_shared_state_fields,
                                uninit_global_shared_state_fields)

/*
 * Global data
 */

PICOTM_GLOBAL_STATE_STATIC_IMPL(global_state)

/*
 * Thread-local data
 */

struct thread_state {
    /**
     * The thread-local transaction state.
     */
    struct picotm_tx tx;
};

static void
init_thread_state_fields(struct thread_state* thread,
                         struct picotm_error* error)
{
    assert(thread);

    struct global_state* global = PICOTM_GLOBAL_STATE_REF(global_state,
                                                          error);
    if (picotm_error_is_set(error)) {
        return;
    }

    picotm_tx_init(&thread->tx, &global->lm, error);
    if (picotm_error_is_set(error)) {
        goto err_picotm_tx_init;
    }

    return;

err_picotm_tx_init:
    PICOTM_GLOBAL_STATE_UNREF(global_state);
}

static void
uninit_thread_state_fields(struct thread_state* thread)
{
    assert(thread);

    picotm_tx_release(&thread->tx);

    /* We're going to release our reference to the global state from
     * init_thread_state_fields(). Because the global-state object's
     * address never changes, there's no need to store it in the thread
     * state. */

    PICOTM_GLOBAL_STATE_UNREF(global_state);
}

PICOTM_STATE(thread_state, struct thread_state);
PICOTM_STATE_STATIC_IMPL(thread_state, struct thread_state,
                         init_thread_state_fields,
                         uninit_thread_state_fields)

/*
 * Thread state
 */

PICOTM_THREAD_STATE_STATIC_IMPL(thread_state)

static struct picotm_tx*
get_tx(bool initialize, struct picotm_error* error)
{
    struct thread_state* thread = PICOTM_THREAD_STATE_ACQUIRE(thread_state,
                                                              initialize,
                                                              error);
    if (picotm_error_is_set(error)) {
        return NULL;
    } else if (!thread) {
        return NULL; /* not yet initialized */
    }
    return &thread->tx;
}

static struct picotm_tx*
get_non_null_tx(void)
{
    while (true) {
        struct picotm_error error = PICOTM_ERROR_INITIALIZER;

        struct picotm_tx* tx = get_tx(true, &error);
        if (picotm_error_is_set(&error)) {
            picotm_recover_from_error(&error);
            continue;
        }
        assert(tx);
        return tx;
    };
}

/*
 * Thread-local error state
 */

static struct picotm_error*
get_non_null_error(void)
{
    /**
     * The thread-local error structure holds error codes even across
     * transaction restarts, such that during recovery the previous run's
     * error can still be retrieved.
     */
    static __thread struct picotm_error error = PICOTM_ERROR_INITIALIZER;

    return &error;
}

/*
 * Lock-owner look-up functions
 */

struct picotm_lock_owner*
picotm_lock_owner_get_thread_local_instance()
{
    struct picotm_tx* tx = get_non_null_tx();

    return &tx->lo;
}

struct picotm_lock_manager*
picotm_lock_owner_get_lock_manager(struct picotm_lock_owner* lo)
{
    assert(lo);

    const struct picotm_tx* tx = picotm_containerof(lo, struct picotm_tx, lo);

    return tx->lm;
}

/*
 * Public interface
 */

PICOTM_EXPORT
_Bool
__picotm_begin(enum __picotm_mode mode, __picotm_jmp_buf* env)
{
    static const unsigned char tx_mode[] = {
        TX_MODE_REVOCABLE,
        TX_MODE_REVOCABLE,
        TX_MODE_IRREVOCABLE,
        TX_MODE_REVOCABLE,
        TX_MODE_REVOCABLE
    };

    switch (mode) {
    case PICOTM_MODE_RECOVERY: {
        struct picotm_error error = PICOTM_ERROR_INITIALIZER;
        struct picotm_tx* tx = get_tx(true, &error);
        if (picotm_error_is_set(&error)) {
            return false; /* Enter recovery mode. */
        }
        if (!picotm_error_is_non_recoverable()) {
            picotm_tx_rollback(tx, &error);
        }
        /* We're recovering from an error. Returning 'false'
         * will invoke the transaction's recovery code. */
        return false;
    }
    case PICOTM_MODE_IRREVOCABLE:
        /* fall through */
    case PICOTM_MODE_RETRY: {
            /* We (re-)start a transaction. Clear the old error state. */
            struct picotm_error* error = get_non_null_error();
            memset(error, 0, sizeof(*error));

            struct picotm_tx* tx = get_tx(false, error);
            if (picotm_error_is_set(error)) {
                return false; /* Enter recovery mode. */
            }
            assert(tx);
            picotm_tx_rollback(tx, error);
            if (picotm_error_is_set(error)) {
                return false; /* Enter recovery mode. */
            }
        }
        /* fall through */
    case PICOTM_MODE_RESTART:
        /* fall through */
    case PICOTM_MODE_START: {

            /* We (re-)start a transaction. Clear the old error state. */
            struct picotm_error* error = get_non_null_error();
            memset(error, 0, sizeof(*error));

            struct picotm_tx* tx = get_tx(true, error);
            if (picotm_error_is_set(error)) {
                return false; /* Enter recovery mode. */
            }

            picotm_tx_begin(tx, tx_mode[mode], mode != PICOTM_MODE_START, env, error);
            if (picotm_error_is_set(error)) {
                return false; /* Enter recovery mode. */
            }
        }
    }
    return true;
}

static void
restart_tx(struct picotm_tx* tx, enum __picotm_mode mode)
{
    assert(tx);

    /* Restarting the transaction here transfers control
     * to __picotm_begin(). */
    __picotm_longjmp(*(tx->env), (int)mode);
}

static void
restart_tx_from_error(struct picotm_tx* tx, const struct picotm_error* error)
{
    assert(error);
    assert(picotm_error_is_set(error));

    switch (error->status) {
    case PICOTM_CONFLICTING:
        restart_tx(tx, PICOTM_MODE_RETRY);
        break;
    case PICOTM_REVOCABLE:
        restart_tx(tx, PICOTM_MODE_IRREVOCABLE);
        break;
    case PICOTM_ERROR_CODE:     /* fall through */
    case PICOTM_ERRNO:          /* fall through */
    case PICOTM_KERN_RETURN_T:  /* fall through */
    case PICOTM_SIGINFO_T:
        restart_tx(tx, PICOTM_MODE_RECOVERY);
        break;
    }
}

PICOTM_EXPORT
void
__picotm_commit()
{
    struct picotm_tx* tx = get_non_null_tx();

    struct picotm_error* error = get_non_null_error();
    picotm_tx_commit(tx, error);
    if (picotm_error_is_set(error)) {
        restart_tx_from_error(tx, error);
    }
}

PICOTM_EXPORT
void
picotm_restart()
{
    restart_tx(get_non_null_tx(), PICOTM_MODE_RESTART);
}

PICOTM_EXPORT
void
picotm_irrevocable()
{
    restart_tx(get_non_null_tx(), PICOTM_MODE_IRREVOCABLE);
}

PICOTM_EXPORT
bool
picotm_is_irrevocable()
{
    return picotm_tx_is_irrevocable(get_non_null_tx());
}

PICOTM_EXPORT
unsigned long
picotm_number_of_restarts()
{
    struct picotm_error error = PICOTM_ERROR_INITIALIZER;

    struct picotm_tx* tx = get_tx(false, &error);
    if (picotm_error_is_set(&error)) {
        return 0;
    } else if (!tx) {
        return 0; /* thread executed no transaction; not an error */
    }
    return tx->nretries;
}

PICOTM_EXPORT
void
picotm_release()
{
    PICOTM_THREAD_STATE_RELEASE(thread_state);
}

PICOTM_EXPORT
enum picotm_error_status
picotm_error_status()
{
    return get_non_null_error()->status;
}

PICOTM_EXPORT
_Bool
picotm_error_is_non_recoverable()
{
    return get_non_null_error()->is_non_recoverable;
}

PICOTM_EXPORT
enum picotm_error_code
picotm_error_as_error_code()
{
    return get_non_null_error()->value.error_hint;
}

PICOTM_EXPORT
int
picotm_error_as_errno()
{
    return get_non_null_error()->value.errno_hint;
}

#if defined(PICOTM_HAVE_TYPE_KERN_RETURN_T) && PICOTM_HAVE_TYPE_KERN_RETURN_T
PICOTM_EXPORT
kern_return_t
picotm_error_as_kern_return_t()
{
    return get_non_null_error()->value.kern_return_t_value;
}
#endif

#if defined(PICOTM_HAVE_TYPE_SIGINFO_T) && PICOTM_HAVE_TYPE_SIGINFO_T
PICOTM_EXPORT
const siginfo_t*
picotm_error_as_siginfo_t()
{
    return &get_non_null_error()->value.siginfo_t_info;
}
#endif

/*
 * Module interface
 */

PICOTM_EXPORT
unsigned long
picotm_register_module(const struct picotm_module_ops* ops, void* data,
                       struct picotm_error* error)
{
    return picotm_tx_register_module(get_non_null_tx(), ops, data, error);
}

PICOTM_EXPORT
void
picotm_append_event(unsigned long module, uint16_t head, uintptr_t tail,
                    struct picotm_error* error)
{
    picotm_tx_append_event(get_non_null_tx(), module, head, tail, error);
}

PICOTM_EXPORT
void
picotm_resolve_conflict(struct picotm_rwlock* conflicting_lock)
{
    picotm_error_set_conflicting(get_non_null_error(), conflicting_lock);

    restart_tx(get_non_null_tx(), PICOTM_MODE_RETRY);
}

PICOTM_EXPORT
void
picotm_recover_from_error_code(enum picotm_error_code error_hint)
{
    picotm_error_set_error_code(get_non_null_error(), error_hint);

    /* Nothing we can do on errors; let's try to recover. */
    restart_tx(get_non_null_tx(), PICOTM_MODE_RECOVERY);
}

PICOTM_EXPORT
void
picotm_recover_from_errno(int errno_hint)
{
    picotm_error_set_errno(get_non_null_error(), errno_hint);

    /* Nothing we can do on errors; let's try to recover. */
    restart_tx(get_non_null_tx(), PICOTM_MODE_RECOVERY);
}

#if defined(PICOTM_HAVE_TYPE_KERN_RETURN_T) && PICOTM_HAVE_TYPE_KERN_RETURN_T
PICOTM_EXPORT
void
picotm_recover_from_kern_return_t(kern_return_t value)
{
    picotm_error_set_kern_return_t(get_non_null_error(), value);

    /* Nothing we can do on errors; let's try to recover. */
    restart_tx(get_non_null_tx(), PICOTM_MODE_RECOVERY);
}
#endif

#if defined(PICOTM_HAVE_TYPE_SIGINFO_T) && PICOTM_HAVE_TYPE_SIGINFO_T
PICOTM_EXPORT
void
picotm_recover_from_siginfo_t(const siginfo_t* info)
{
    picotm_error_set_siginfo_t(get_non_null_error(), info);

    /* Nothing we can do on errors; let's try to recover. */
    restart_tx(get_non_null_tx(), PICOTM_MODE_RECOVERY);
}
#endif

PICOTM_EXPORT
void
picotm_recover_from_error(const struct picotm_error* error)
{
    assert(error);

    memcpy(get_non_null_error(), error, sizeof(*error));

    restart_tx_from_error(get_non_null_tx(), error);
}
