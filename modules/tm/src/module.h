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

#include <stddef.h>
#include <stdint.h>

/**
 * \cond impl || tm_impl
 * \ingroup tm_impl
 * \file
 * \endcond
 */

void
tm_module_load(uintptr_t addr, void* buf, size_t siz);

void
tm_module_store(uintptr_t addr, const void* buf, size_t siz);

void
tm_module_loadstore(uintptr_t laddr, uintptr_t saddr, size_t siz);

void
tm_module_privatize(uintptr_t addr, size_t siz, unsigned long flags);

void
tm_module_privatize_c(uintptr_t addr, int c, unsigned long flags);
