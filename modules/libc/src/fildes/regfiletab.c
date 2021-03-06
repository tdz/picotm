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

#include "regfiletab.h"
#include "picotm/picotm-error.h"
#include "picotm/picotm-lib-array.h"
#include "picotm/picotm-lib-tab.h"
#include "picotm/picotm-module.h"
#include <errno.h>
#include "range.h"

void
fildes_regfiletab_init(struct fildes_regfiletab* self,
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
regfile_uninit_walk_cb(void* regfile, struct picotm_error* error)
{
    regfile_uninit(regfile);
    return 1;
}

void
fildes_regfiletab_uninit(struct fildes_regfiletab* self)
{
    struct picotm_error error = PICOTM_ERROR_INITIALIZER;

    picotm_tabwalk_1(self->tab, self->len, sizeof(self->tab[0]),
                     regfile_uninit_walk_cb, &error);

    pthread_rwlock_destroy(&self->rwlock);
}

static void
rdlock_regfiletab(struct fildes_regfiletab* regfiletab,
                  struct picotm_error* error)
{
    int err = pthread_rwlock_rdlock(&regfiletab->rwlock);
    if (err) {
        picotm_error_set_errno(error, err);
        return;
    }
}

static void
wrlock_regfiletab(struct fildes_regfiletab* regfiletab,
                  struct picotm_error* error)
{
    int err = pthread_rwlock_wrlock(&regfiletab->rwlock);
    if (err) {
        picotm_error_set_errno(error, err);
        return;
    }
}

static void
unlock_regfiletab(struct fildes_regfiletab* regfiletab)
{
    do {
        int err = pthread_rwlock_unlock(&regfiletab->rwlock);
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
static struct regfile*
append_empty_regfile(struct fildes_regfiletab* regfiletab,
                     struct picotm_error* error)
{
    if (regfiletab->len == picotm_arraylen(regfiletab->tab)) {
        /* Return error if not enough ids available */
        picotm_error_set_conflicting(error, NULL);
        return NULL;
    }

    struct regfile* regfile = regfiletab->tab + regfiletab->len;

    regfile_init(regfile, error);
    if (picotm_error_is_set(error)) {
        return NULL;
    }

    ++regfiletab->len;

    return regfile;
}

/* requires reader lock */
static struct regfile*
find_by_id(struct fildes_regfiletab* regfiletab, const struct file_id* id)
{
    struct regfile *regfile_beg = picotm_arraybeg(regfiletab->tab);
    const struct regfile* regfile_end = picotm_arrayat(regfiletab->tab,
                                                       regfiletab->len);

    while (regfile_beg < regfile_end) {

        const int cmp = regfile_cmp_and_ref(regfile_beg, id);
        if (!cmp) {
            return regfile_beg;
        }

        ++regfile_beg;
    }

    return NULL;
}

/* requires writer lock */
static struct regfile*
search_by_id(struct fildes_regfiletab* regfiletab, const struct file_id* id,
             int fildes, struct picotm_error* error)
{
    struct regfile* regfile_beg = picotm_arraybeg(regfiletab->tab);
    const struct regfile* regfile_end = picotm_arrayat(regfiletab->tab,
                                                       regfiletab->len);

    while (regfile_beg < regfile_end) {

        const int cmp = regfile_cmp_and_ref_or_set_up(regfile_beg, id, fildes,
                                                      error);
        if (!cmp) {
            if (picotm_error_is_set(error)) {
                return NULL;
            }
            return regfile_beg; /* set-up regfile structure; return */
        }

        ++regfile_beg;
    }

    return NULL;
}

struct regfile*
fildes_regfiletab_ref_fildes(struct fildes_regfiletab* self, int fildes,
                             struct picotm_error* error)
{
    struct file_id id;
    file_id_init_from_fildes(&id, fildes, error);
    if (picotm_error_is_set(error)) {
        return NULL;
    }

    /* Try to find an existing regfile structure with the given id; iff
     * a new element was not explicitly requested.
     */

    rdlock_regfiletab(self, error);
    if (picotm_error_is_set(error)) {
        return NULL;
    }

    struct regfile* regfile = find_by_id(self, &id);
    if (regfile) {
        goto unlock; /* found regfile for id; return */
    }

    unlock_regfiletab(self);

    /* Not found entry; acquire writer lock to create a new entry in
     * the regfile table.
     */

    wrlock_regfiletab(self, error);
    if (picotm_error_is_set(error)) {
        return NULL;
    }

    /* Re-try find operation; maybe element was added meanwhile. */
    regfile = find_by_id(self, &id);
    if (regfile) {
        goto unlock; /* found regfile for id; return */
    }

    /* No entry with the id exists; try to set up an existing, but
     * currently unused, regfile structure.
     */

    struct file_id empty_id;
    file_id_clear(&empty_id);

    regfile = search_by_id(self, &empty_id, fildes, error);
    if (picotm_error_is_set(error)) {
        goto err_search_by_id;
    } else if (regfile) {
        goto unlock; /* found regfile for id; return */
    }

    /* The regfile table is full; create a new entry for the regfile id at the
     * end of the table.
     */

    regfile = append_empty_regfile(self, error);
    if (picotm_error_is_set(error)) {
        goto err_append_empty_regfile;
    }

    /* To perform the setup, we must have acquired a writer lock at
     * this point. No other transaction may interfere. */
    regfile_ref_or_set_up(regfile, fildes, error);
    if (picotm_error_is_set(error)) {
        goto err_regfile_ref_or_set_up;
    }

unlock:
    unlock_regfiletab(self);

    return regfile;

err_regfile_ref_or_set_up:
err_append_empty_regfile:
err_search_by_id:
    unlock_regfiletab(self);
    return NULL;
}

size_t
fildes_regfiletab_index(struct fildes_regfiletab* self,
                        struct regfile* regfile)
{
    return regfile - self->tab;
}
