/*
 * MIT License
 * Copyright (c) 2017   Thomas Zimmermann <contact@tzimmermann.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
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
 *
 * SPDX-License-Identifier: MIT
 */

#include "picotm/picotm-lib-tab.h"
#include "table.h"

PICOTM_EXPORT
void*
picotm_tabresize(void* base, size_t nelems, size_t newnelems, size_t siz,
                 struct picotm_error* error)
{
    return tabresize(base, nelems, newnelems, siz, error);
}

PICOTM_EXPORT
void
picotm_tabfree(void* base)
{
    tabfree(base);
}

PICOTM_EXPORT
size_t
picotm_tabwalk_1(void* base, size_t nelems, size_t siz,
                 picotm_tabwalk_1_function walk, struct picotm_error* error)
{
    return tabwalk_1(base, nelems, siz, walk, error);
}

PICOTM_EXPORT
size_t
picotm_tabwalk_2(void* base, size_t nelems, size_t siz,
                 picotm_tabwalk_2_function walk, void* data,
                 struct picotm_error* error)
{
    return tabwalk_2(base, nelems, siz, walk, data, error);
}

PICOTM_EXPORT
size_t
picotm_tabwalk_3(void* base, size_t nelems, size_t siz,
                 picotm_tabwalk_3_function walk, void* data1, void* data2,
                 struct picotm_error* error)
{
    return tabwalk_3(base, nelems, siz, walk, data1, data2, error);
}

PICOTM_EXPORT
size_t
picotm_tabrwalk_1(void* base, size_t nelems, size_t siz,
                  picotm_tabwalk_1_function walk, struct picotm_error* error)
{
    return tabrwalk_1(base, nelems, siz, walk, error);
}

PICOTM_EXPORT
size_t
picotm_tabrwalk_2(void* base, size_t nelems, size_t siz,
                  picotm_tabwalk_2_function walk, void* data,
                  struct picotm_error* error)
{
    return tabrwalk_2(base, nelems, siz, walk, data, error);
}

PICOTM_EXPORT
size_t
picotm_tabuniq(void* base, size_t nelems, size_t siz,
              int (*compare)(const void*, const void*))
{
    return tabuniq(base, nelems, siz, compare);
}
