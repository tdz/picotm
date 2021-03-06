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

#include "sockettab.h"
#include "picotm/picotm-error.h"
#include "picotm/picotm-lib-array.h"
#include "picotm/picotm-lib-tab.h"
#include "picotm/picotm-module.h"
#include <errno.h>
#include "range.h"

void
fildes_sockettab_init(struct fildes_sockettab* self,
                      struct picotm_error* error)
{
    self->len = 0;

    int err = pthread_rwlock_init(&self->rwlock, NULL);
    if (err) {
        picotm_error_set_errno(error, err);
        return;
    }
}

static size_t
socket_uninit_walk_cb(void* socket, struct picotm_error* error)
{
    socket_uninit(socket);
    return 1;
}

void
fildes_sockettab_uninit(struct fildes_sockettab* self)
{
    struct picotm_error error = PICOTM_ERROR_INITIALIZER;

    picotm_tabwalk_1(self->tab, self->len, sizeof(self->tab[0]),
                     socket_uninit_walk_cb, &error);

    pthread_rwlock_destroy(&self->rwlock);
}

static void
rdlock_sockettab(struct fildes_sockettab* sockettab,
                 struct picotm_error* error)
{
    int err = pthread_rwlock_rdlock(&sockettab->rwlock);
    if (err) {
        picotm_error_set_errno(error, err);
        return;
    }
}

static void
wrlock_sockettab(struct fildes_sockettab* sockettab,
                 struct picotm_error* error)
{
    int err = pthread_rwlock_wrlock(&sockettab->rwlock);
    if (err) {
        picotm_error_set_errno(error, err);
        return;
    }
}

static void
unlock_sockettab(struct fildes_sockettab* sockettab)
{
    do {
        int err = pthread_rwlock_unlock(&sockettab->rwlock);
        if (err) {
            struct picotm_error error = PICOTM_ERROR_INITIALIZER;
            picotm_error_set_errno(&error, err);
            picotm_error_mark_as_non_recoverable(&error);
            picotm_recover_from_error(&error);
            continue;
        }
        break;
    } while (true);
}

/* requires a writer lock */
static struct socket*
append_empty_socket(struct fildes_sockettab* sockettab,
                    struct picotm_error* error)
{
    if (sockettab->len == picotm_arraylen(sockettab->tab)) {
        /* Return error if not enough ids available */
        picotm_error_set_conflicting(error, NULL);
        return NULL;
    }

    struct socket* socket = sockettab->tab + sockettab->len;

    socket_init(socket, error);
    if (picotm_error_is_set(error)) {
        return NULL;
    }

    ++sockettab->len;

    return socket;
}

/* requires reader lock */
static struct socket*
find_by_id(struct fildes_sockettab* sockettab, const struct file_id* id)
{
    struct socket *socket_beg = picotm_arraybeg(sockettab->tab);
    const struct socket* socket_end = picotm_arrayat(sockettab->tab,
                                                     sockettab->len);

    while (socket_beg < socket_end) {

        const int cmp = socket_cmp_and_ref(socket_beg, id);
        if (!cmp) {
            return socket_beg;
        }

        ++socket_beg;
    }

    return NULL;
}

/* requires writer lock */
static struct socket*
search_by_id(struct fildes_sockettab* sockettab, const struct file_id* id,
             int fildes, struct picotm_error* error)
{
    struct socket* socket_beg = picotm_arraybeg(sockettab->tab);
    const struct socket* socket_end = picotm_arrayat(sockettab->tab,
                                                     sockettab->len);

    while (socket_beg < socket_end) {

        const int cmp = socket_cmp_and_ref_or_set_up(socket_beg, id, fildes,
                                                     error);
        if (!cmp) {
            if (picotm_error_is_set(error)) {
                return NULL;
            }
            return socket_beg; /* set-up socket structure; return */
        }

        ++socket_beg;
    }

    return NULL;
}

struct socket*
fildes_sockettab_ref_fildes(struct fildes_sockettab* self, int fildes,
                            struct picotm_error* error)
{
    struct file_id id;
    file_id_init_from_fildes(&id, fildes, error);
    if (picotm_error_is_set(error)) {
        return NULL;
    }

    /* Try to find an existing socket structure with the given id.
     */

    rdlock_sockettab(self, error);
    if (picotm_error_is_set(error)) {
        return NULL;
    }

    struct socket* socket = find_by_id(self, &id);
    if (socket) {
        goto unlock; /* found socket for id; return */
    }

    unlock_sockettab(self);

    /* Not found entry; acquire writer lock to create a new entry in
     * the socket table.
     */

    wrlock_sockettab(self, error);
    if (picotm_error_is_set(error)) {
        return NULL;
    }

    /* Re-try find operation; maybe element was added meanwhile. */
    socket = find_by_id(self, &id);
    if (socket) {
        goto unlock; /* found socket for id; return */
    }

    /* No entry with the id exists; try to set up an existing, but
     * currently unused, socket structure.
     */

    struct file_id empty_id;
    file_id_clear(&empty_id);

    socket = search_by_id(self, &empty_id, fildes, error);
    if (picotm_error_is_set(error)) {
        goto err_search_by_id;
    } else if (socket) {
        goto unlock; /* found socket for id; return */
    }

    /* The socket table is full; create a new entry for the socket id at the
     * end of the table.
     */

    socket = append_empty_socket(self, error);
    if (picotm_error_is_set(error)) {
        goto err_append_empty_socket;
    }

    /* To perform the setup, we must have acquired a writer lock at
     * this point. No other transaction may interfere. */
    socket_ref_or_set_up(socket, fildes, error);
    if (picotm_error_is_set(error)) {
        goto err_socket_ref_or_set_up;
    }

unlock:
    unlock_sockettab(self);

    return socket;

err_socket_ref_or_set_up:
err_append_empty_socket:
err_search_by_id:
    unlock_sockettab(self);
    return NULL;
}

size_t
fildes_sockettab_index(struct fildes_sockettab* self, struct socket* socket)
{
    return socket - self->tab;
}
