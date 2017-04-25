/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef REGIONTAB_H
#define REGIONTAB_H

unsigned long
regiontab_append(struct region **tab, size_t *nelems,
                                      size_t *siz,
                                      size_t nbyte,
                                      off_t offset);

void
regiontab_clear(struct region **tab, size_t *nelems);

int
regiontab_sort(struct region *tab, size_t nelems);

void
regiontab_dump(struct region *tab, size_t nelems);

#endif
