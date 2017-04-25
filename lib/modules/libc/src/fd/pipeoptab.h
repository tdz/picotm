/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef PIPEOPTAB_H
#define PIPEOPTAB_H

unsigned long
pipeoptab_append(struct pipeop **tab, size_t *nelems, int pipefd[2]);

void
pipeoptab_clear(struct pipeop **tab, size_t *nelems);

void
pipeoptab_dump(struct pipeop *tab, size_t nelems);

#endif
