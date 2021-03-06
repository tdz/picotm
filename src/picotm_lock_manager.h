/*
 * picotm - A system-level transaction manager
 * Copyright (c) 2017   Thomas Zimmermann <contact@tzimmermann.org>
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

#include "picotm_os_rwlock.h"
#include <stdbool.h>
#include <stddef.h>

/**
 * \cond impl || lib_impl
 * \ingroup lib_impl
 * \file
 * \endcond
 */

struct picotm_error;
struct picotm_lock_owner;

/**
 * \brief Contains call-back functions for lock implementations.
 */
struct picotm_lock_slist_funcs {

    unsigned long (*get_first_index)(void*);

    unsigned long (*cmpxchg_first_index)(void*, unsigned long, unsigned long);
};

/**
 * \brief Coordinates among lock owners contending for locks.
 */
struct picotm_lock_manager {

    struct picotm_os_rwlock    lo_rwlock;
    struct picotm_lock_owner** lo;
    size_t                     nlos;

    /**
     * Locks the transaction system for either one exclusive lock owner,
     * or multiple non-exclusive lock owners.
     *
     * \todo This lock implements irrevocability. Replace this lock with a
     * more fine-grained system that allows recovable transactions while an
     * irrevocable transaction is present.
     */
    struct picotm_os_rwlock   exclusive_lo_lock;
    struct picotm_lock_owner* exclusive_lo;
};

/**
 * \brief Initializes a lock-manager instance.
 * \param self The lock manager.
 * \param [out] error Returns an error to the caller.
 */
void
picotm_lock_manager_init(struct picotm_lock_manager* self,
                         struct picotm_error* error);

/**
 * \brief Cleans-up a lock-manager instance.
 * \param self The lock manager.
 */
void
picotm_lock_manager_uninit(struct picotm_lock_manager* self);

/**
 * \brief Registers a lock owner at a lock manager.
 * \param self The lock manager.
 * \param lo The lock owner.
 * \param [out] error Returns an error to the caller.
 *
 * Each lock-owner instance has to be registered at the lock manager
 * exactly once. This function assigns an index to the lock owner, so
 * it can be queued up in a waiter list.
 */
void
picotm_lock_manager_register_owner(struct picotm_lock_manager* self,
                                   struct picotm_lock_owner* lo,
                                   struct picotm_error* error);

/**
 * \brief Unregisters a lock owner at a lock manager.
 * \param self The lock manager.
 * \param lo The lock owner.
 *
 * Lock owner have to be unregistered at their lock manager before
 * cleaning them up. After this function returns, the lock owner's
 * index will be available for registration.
 */
void
picotm_lock_manager_unregister_owner(struct picotm_lock_manager* self,
                                     struct picotm_lock_owner* lo);

/**
 * \brief Sets irrevocability for a lock owner.
 * \param self The lock manager.
 * \param exclusive_lo The lock owner that is to become irrevocable.
 * \param[out] error Returns an error to the caller.
 */
void
picotm_lock_manager_make_irrevocable(struct picotm_lock_manager* self,
                                     struct picotm_lock_owner* exclusive_lo,
                                     struct picotm_error* error);

/**
 * \brief Waits for an irrevocable lock owner to complete.
 * \param self The lock manager.
 * \param[out] error Returns an error to the caller.
 */
void
picotm_lock_manager_wait_irrevocable(struct picotm_lock_manager* self,
                                     struct picotm_error* error);

/**
 * \brief Unblocks irrevocability for other lock owners.
 * \param self The lock manager.
 */
void
picotm_lock_manager_release_irrevocability(struct picotm_lock_manager* self);

/**
 * \brief Instructs a lock manager to setup a lock owner for waiting.
 * \param self The lock manager.
 * \param lo The lock owner.
 * \param is_writer The lock owner is waiting to acquire a writer lock.
 * \param slist_funcs The lock list's call-back functions.
 * \param slist The anonymous lock-list data structure.
 * \param [out] error Returns an error to the caller.
 */
bool
picotm_lock_manager_wait(struct picotm_lock_manager* self,
                         struct picotm_lock_owner* lo, bool is_writer,
                         const struct picotm_lock_slist_funcs* slist_funcs,
                         void* slist, struct picotm_error* error);

/**
 * \brief Instructs a lock manager to wake up one or more waiting lock owners.
 * \param self The lock manager.
 * \param concurrent_readers_supported True if the lock can support multiple
 *                                     concurrent readers.
 * \param slist_funcs The lock list's call-back functions.
 * \param slist The anonymous lock-list data structure.
 * \param [out] error Returns an error to the caller.
 */
void
picotm_lock_manager_wake_up(struct picotm_lock_manager* self,
                            bool concurrent_readers_supported,
                            const struct picotm_lock_slist_funcs* slist_funcs,
                            void* slist, struct picotm_error* error);

/*
 * Look-up functions.
 */

/**
 * \brief Returns the lock manager of a lock owner.
 * \param lown A lock-owner instance.
 * \returns The lock owner's lock manager.
 */
struct picotm_lock_manager*
picotm_lock_owner_get_lock_manager(struct picotm_lock_owner* lown);
