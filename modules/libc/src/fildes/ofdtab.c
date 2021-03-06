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

#include "ofdtab.h"
#include "picotm/picotm-error.h"
#include "picotm/picotm-lib-array.h"
#include "picotm/picotm-lib-tab.h"
#include "picotm/picotm-module.h"
#include <errno.h>
#include <string.h>
#include "range.h"

void
fildes_ofdtab_init(struct fildes_ofdtab* self, struct picotm_error* error)
{
    self->len = 0;

    int err = pthread_rwlock_init(&self->rwlock, NULL);
    if (err) {
        picotm_error_set_errno(error, err);
        return;
    }
}

static size_t
ofd_uninit_walk_cb(void* ofd, struct picotm_error* error)
{
    ofd_uninit(ofd);
    return 1;
}

void
fildes_ofdtab_uninit(struct fildes_ofdtab* self)
{
    struct picotm_error error = PICOTM_ERROR_INITIALIZER;

    picotm_tabwalk_1(self->tab, self->len, sizeof(self->tab[0]),
                     ofd_uninit_walk_cb, &error);

    pthread_rwlock_destroy(&self->rwlock);
}

static void
rdlock_ofdtab(struct fildes_ofdtab* ofdtab, struct picotm_error* error)
{
    int err = pthread_rwlock_rdlock(&ofdtab->rwlock);
    if (err) {
        picotm_error_set_errno(error, err);
        return;
    }
}

static void
wrlock_ofdtab(struct fildes_ofdtab* ofdtab, struct picotm_error* error)
{
    int err = pthread_rwlock_wrlock(&ofdtab->rwlock);
    if (err) {
        picotm_error_set_errno(error, err);
        return;
    }
}

static void
unlock_ofdtab(struct fildes_ofdtab* ofdtab)
{
    do {
        int err = pthread_rwlock_unlock(&ofdtab->rwlock);
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
static struct ofd*
append_empty_ofd(struct fildes_ofdtab* ofdtab, struct picotm_error* error)
{
    if (ofdtab->len == picotm_arraylen(ofdtab->tab)) {
        /* Return error if not enough ids available */
        picotm_error_set_conflicting(error, NULL);
        return NULL;
    }

    struct ofd* ofd = ofdtab->tab + ofdtab->len;

    ofd_init(ofd, error);
    if (picotm_error_is_set(error)) {
        return NULL;
    }

    ++ofdtab->len;

    return ofd;
}

/* requires reader lock */
static struct ofd*
find_by_id(struct fildes_ofdtab* ofdtab, const struct ofd_id* id,
           bool error_ne_fildes, struct picotm_error* error)
{
    struct picotm_error saved_error = PICOTM_ERROR_INITIALIZER;

    struct ofd *ofd_beg = picotm_arraybeg(ofdtab->tab);
    const struct ofd* ofd_end = picotm_arrayat(ofdtab->tab, ofdtab->len);

    while (ofd_beg < ofd_end) {

        struct picotm_error cmp_error = PICOTM_ERROR_INITIALIZER;

        const int cmp = ofd_cmp_and_ref(ofd_beg, id, error_ne_fildes,
                                        &cmp_error);
        if (picotm_error_is_set(&cmp_error)) {
            /* An error might be reported if the id's file descriptors don't
             * match. We save the error, but continue the loops. If we later
             * find a full match, the function succeeds. Otherwise, it reports
             * the last error. */
            memcpy(&saved_error, &cmp_error, sizeof(saved_error));
        } else if (!cmp) {
            return ofd_beg;
        }

        ++ofd_beg;
    }

    if (picotm_error_is_set(&saved_error)) {
        memcpy(error, &saved_error, sizeof(*error));
    }

    return NULL;
}

/* requires writer lock */
static struct ofd*
search_by_id(struct fildes_ofdtab* ofdtab, const struct ofd_id* id,
             int fildes, struct picotm_error* error)
{
    struct ofd* ofd_beg = picotm_arraybeg(ofdtab->tab);
    const struct ofd* ofd_end = picotm_arrayat(ofdtab->tab, ofdtab->len);

    while (ofd_beg < ofd_end) {

        const int cmp = ofd_cmp_and_ref_or_set_up(ofd_beg, id, fildes, false,
                                                  error);
        if (picotm_error_is_set(error)) {
            return NULL;
        } else if (!cmp) {
            return ofd_beg; /* set-up ofd structure; return */
        }

        ++ofd_beg;
    }

    return NULL;
}

static bool
error_ne_fildes(bool newly_created)
{
    return !newly_created;
}

struct ofd*
fildes_ofdtab_ref_fildes(struct fildes_ofdtab* self, int fildes,
                         bool newly_created, struct picotm_error* error)
{
    struct ofd_id id;
    ofd_id_init_from_fildes(&id, fildes, error);
    if (picotm_error_is_set(error)) {
        return NULL;
    }

    struct ofd* ofd;

    /* Try to find an existing ofd structure with the given id; iff
     * a new element was not explicitly requested.
     */

    if (!newly_created) {
        rdlock_ofdtab(self, error);
        if (picotm_error_is_set(error)) {
            return NULL;
        }

        ofd = find_by_id(self, &id, error_ne_fildes(newly_created), error);
        if (picotm_error_is_set(error)) {
            goto err_find_by_id_1;
        } else if (ofd) {
            goto unlock; /* found ofd for id; return */
        }

        unlock_ofdtab(self);
    }

    /* Not found or new entry is requested; acquire writer lock to
     * create a new entry in the ofd table. */
    wrlock_ofdtab(self, error);
    if (picotm_error_is_set(error)) {
        return NULL;
    }

    if (!newly_created) {
        /* Re-try find operation; maybe element was added meanwhile. */
        ofd = find_by_id(self, &id, error_ne_fildes(newly_created), error);
        if (picotm_error_is_set(error)) {
            goto err_find_by_id_2;
        } else if (ofd) {
            goto unlock; /* found ofd for id; return */
        }
    }

    /* No entry with the id exists; try to set up an existing, but
     * currently unused, ofd structure.
     */

    struct ofd_id empty_id;
    ofd_id_init(&empty_id);

    ofd = search_by_id(self, &empty_id, fildes, error);
    if (picotm_error_is_set(error)) {
        goto err_search_by_id;
    } else if (ofd) {
        goto unlock; /* found ofd for id; return */
    }

    /* The ofd table is full; create a new entry for the ofd id at the
     * end of the table.
     */

    ofd = append_empty_ofd(self, error);
    if (picotm_error_is_set(error)) {
        goto err_append_empty_ofd;
    }

    /* To perform the setup, we must have acquired a writer lock at
     * this point. No other transaction may interfere. */
    ofd_ref_or_set_up(ofd, fildes, error);
    if (picotm_error_is_set(error)) {
        goto err_ofd_ref_or_set_up;
    }

unlock:
    unlock_ofdtab(self);

    return ofd;

err_ofd_ref_or_set_up:
err_append_empty_ofd:
err_search_by_id:
err_find_by_id_2:
err_find_by_id_1:
    unlock_ofdtab(self);
    return NULL;
}

size_t
fildes_ofdtab_index(struct fildes_ofdtab* self, struct ofd* ofd)
{
    return ofd - self->tab;
}
