/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "fcntloptab.h"
#include <assert.h>
#include <picotm/picotm-error.h>
#include <picotm/picotm-lib-tab.h>
#include <stdio.h>
#include <string.h>
#include "fcntlop.h"

unsigned long
fcntloptab_append(struct fcntlop** tab, size_t* nelems, int command,
                  const union fcntl_arg*value,
                  const union fcntl_arg* oldvalue,
                  struct picotm_error* error)
{
    assert(tab);
    assert(nelems);

    void *tmp = picotm_tabresize(*tab, *nelems, *nelems + 1,
                                 sizeof((*tab)[0]), error);
    if (picotm_error_is_set(error)) {
        return (unsigned long)-1;
    }
    *tab = tmp;

    fcntlop_init((*tab)+(*nelems), command, value, oldvalue);

    return (*nelems)++;
}

void
fcntloptab_clear(struct fcntlop **tab, size_t *nelems)
{
    assert(tab);
    assert(nelems);

    picotm_tabfree(*tab);
    *tab = NULL;
    *nelems = 0;
}

void
fcntloptab_dump(struct fcntlop *tab, size_t nelems)
{
    struct fcntlop *ent;

    assert(tab || !nelems);

    for (ent = tab; ent < tab+nelems; ++ent) {
        fprintf(stderr, "%ld: ", (long)(ent-tab));
        fcntlop_dump(ent);
        fprintf(stderr, "\n");
    }
}

