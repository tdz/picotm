/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <stdlib.h>
#include "compiler.h"

SYSTX_NOTHROW
/**
 * Executes free() within a transaction.
 */
void free_tm(void* ptr);

SYSTX_EXPORT
/**
 * Executes mkdtemp() within a transaction.
 */
char*
mkdtemp_tm(char* template);

SYSTX_EXPORT
/**
 * Executes mkstemp() within a transaction.
 */
int
mkstemp_tm(char* template);

SYSTX_NOTHROW
/**
 * Executes posix_memalign() within a transaction.
 */
int posix_memalign_tm(void** memptr, size_t alignment, size_t size);

SYSTX_NOTHROW
/**
 * Executes realloc() within a transaction.
 */
void* realloc_tm(void* ptr, size_t size);

SYSTX_NOTHROW
/**
 * Executes rand_r() within a transaction.
 */
int
rand_r_tm(unsigned int* seed);
