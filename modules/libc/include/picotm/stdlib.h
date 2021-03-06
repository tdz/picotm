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

#pragma once

#include "picotm/config/picotm-libc-config.h"
#include "picotm/compiler.h"
#include "picotm/picotm-tm.h"
#include <stdlib.h>

PICOTM_BEGIN_DECLS

/**
 * \ingroup group_libc
 * \file
 *
 * \brief Transactional wrappers for interfaces of <stdlib.h>.
 */

#if defined(PICOTM_LIBC_HAVE_TYPE_DIV_T) && PICOTM_LIBC_HAVE_TYPE_DIV_T || \
    defined(__PICOTM_DOXYGEN)
/** \addtogroup group_libc
 * \{ */
PICOTM_TM_LOAD_TX(div_t, div_t);
PICOTM_TM_STORE_TX(div_t, div_t);
PICOTM_TM_PRIVATIZE_TX(div_t, div_t);
/** \} */
#endif

#if defined(PICOTM_LIBC_HAVE_TYPE_LDIV_T) && PICOTM_LIBC_HAVE_TYPE_LDIV_T || \
    defined(__PICOTM_DOXYGEN)
/** \addtogroup group_libc
 * \{ */
PICOTM_TM_LOAD_TX(ldiv_t, ldiv_t);
PICOTM_TM_STORE_TX(ldiv_t, ldiv_t);
PICOTM_TM_PRIVATIZE_TX(ldiv_t, ldiv_t);
/** \} */
#endif

#if defined(PICOTM_LIBC_HAVE_TYPE_LLDIV_T) && \
            PICOTM_LIBC_HAVE_TYPE_LLDIV_T || \
    defined(__PICOTM_DOXYGEN)
/** \addtogroup group_libc
 * \{ */
PICOTM_TM_LOAD_TX(lldiv_t, lldiv_t);
PICOTM_TM_STORE_TX(lldiv_t, lldiv_t);
PICOTM_TM_PRIVATIZE_TX(lldiv_t, lldiv_t);
/** \} */
#endif

#if defined(PICOTM_LIBC_HAVE_TYPE_SIZE_T) && \
            PICOTM_LIBC_HAVE_TYPE_SIZE_T || \
    defined(__PICOTM_DOXYGEN)
#if !defined(__PICOTM_LOAD_SIZE_T_TX) || !__PICOTM_LOAD_SIZE_T_TX
#undef __PICOTM_LOAD_SIZE_T_TX
#define __PICOTM_LOAD_SIZE_T_TX     (1)
/** \addtogroup group_libc
 * \{ */
PICOTM_TM_LOAD_TX(size_t, size_t); /* as defined in <picotm/stddef.h> */
/** \} */
#endif
#if !defined(__PICOTM_STORE_SIZE_T_TX) || !__PICOTM_STORE_SIZE_T_TX
#undef __PICOTM_STORE_SIZE_T_TX
#define __PICOTM_STORE_SIZE_T_TX    (1)
/** \addtogroup group_libc
 * \{ */
PICOTM_TM_STORE_TX(size_t, size_t); /* as defined in <picotm/stddef.h> */
/** \} */
#endif
#if !defined(__PICOTM_PRIVATIZE_SIZE_T_TX) || !__PICOTM_PRIVATIZE_SIZE_T_TX
#undef __PICOTM_PRIVATIZE_SIZE_T_TX
#define __PICOTM_PRIVATIZE_SIZE_T_TX    (1)
/** \addtogroup group_libc
 * \{ */
PICOTM_TM_PRIVATIZE_TX(size_t, size_t); /* as defined in <picotm/stddef.h> */
/** \} */
#endif
#endif

#if defined(PICOTM_LIBC_HAVE_TYPE_WCHAR_T) && \
            PICOTM_LIBC_HAVE_TYPE_WCHAR_T || \
    defined(__PICOTM_DOXYGEN)
#if !defined(__PICOTM_LOAD_WCHAR_T_TX) || !__PICOTM_LOAD_WCHAR_T_TX
#undef __PICOTM_LOAD_WCHAR_T_TX
#define __PICOTM_LOAD_WCHAR_T_TX    (1)
/** \addtogroup group_libc
 * \{ */
PICOTM_TM_LOAD_TX(wchar_t, wchar_t); /* as defined in <picotm/stddef.h> */
/** \} */
#endif
#if !defined(__PICOTM_STORE_WCHAR_T_TX) || !__PICOTM_STORE_WCHAR_T_TX
#undef __PICOTM_STORE_WCHAR_T_TX
#define __PICOTM_STORE_WCHAR_T_TX   (1)
/** \addtogroup group_libc
 * \{ */
PICOTM_TM_STORE_TX(wchar_t, wchar_t); /* as defined in <picotm/stddef.h> */
/** \} */
#endif
#if !defined(__PICOTM_PRIVATIZE_WCHAR_T_TX) || \
    !__PICOTM_PRIVATIZE_WCHAR_T_TX
#undef __PICOTM_PRIVATIZE_WCHAR_T_TX
#define __PICOTM_PRIVATIZE_WCHAR_T_TX   (1)
/** \addtogroup group_libc
 * \{ */
PICOTM_TM_PRIVATIZE_TX(wchar_t, wchar_t); /* as defined in <picotm/stddef.h> */
/** \} */
#endif
#endif

#if defined(PICOTM_LIBC_HAVE__EXIT) && PICOTM_LIBC_HAVE__EXIT || \
    defined(__PICOTM_DOXYGEN)
PICOTM_NOTHROW
PICOTM_NORETURN
/**
 * \ingroup group_libc
 * A transaction-safe implementation of [_Exit()][posix::_Exit].
 *
 * \bug Calling _Exit() simply terminates the process. It probably should not
 *      be available in a transaction.
 *
 * [posix::_Exit]:
 *  http://pubs.opengroup.org/onlinepubs/9699919799/functions/_Exit.html
 */
void
_Exit_tx(int status);
#endif

#if defined(PICOTM_LIBC_HAVE_ABORT) && PICOTM_LIBC_HAVE_ABORT || \
    defined(__PICOTM_DOXYGEN)
PICOTM_NOTHROW
PICOTM_NORETURN
/**
 * \ingroup group_libc
 * A transaction-safe implementation of [abort()][posix::abort].
 *
 * \bug Calling abort() simply terminates the process. It probably should not
 *      be available in a transaction.
 *
 * [posix::abort]:
 *  http://pubs.opengroup.org/onlinepubs/9699919799/functions/abort.html
 */
void
abort_tx(void);
#endif

#if defined(PICOTM_LIBC_HAVE_CALLOC) && PICOTM_LIBC_HAVE_CALLOC || \
    defined(__PICOTM_DOXYGEN)
PICOTM_NOTHROW
/**
 * \ingroup group_libc
 * A transaction-safe implementation of [calloc()][posix::calloc].
 *
 * [posix::calloc]:
 *  http://pubs.opengroup.org/onlinepubs/9699919799/functions/calloc.html
 */
void*
calloc_tx(size_t nmemb, size_t size);
#endif

#if defined(PICOTM_LIBC_HAVE_EXIT) && PICOTM_LIBC_HAVE_EXIT || \
    defined(__PICOTM_DOXYGEN)
PICOTM_NOTHROW
PICOTM_NORETURN
/**
 * \ingroup group_libc
 * A transaction-safe implementation of [exit()][posix::exit].
 *
 * \bug Calling exit() simply terminates the process. It probably should not
 *      be available in a transaction.
 *
 * [posix::exit]:
 *  http://pubs.opengroup.org/onlinepubs/9699919799/functions/exit.html
 */
void
exit_tx(int status);
#endif

#if defined(PICOTM_LIBC_HAVE_FREE) && PICOTM_LIBC_HAVE_FREE || \
    defined(__PICOTM_DOXYGEN)
PICOTM_NOTHROW
/**
 * \ingroup group_libc
 * A transaction-safe implementation of [free()][posix::free].
 *
 * [posix::free]:
 *  http://pubs.opengroup.org/onlinepubs/9699919799/functions/free.html
 */
void
free_tx(void* ptr);
#endif

#if defined(PICOTM_LIBC_HAVE_MALLOC) && PICOTM_LIBC_HAVE_MALLOC || \
    defined(__PICOTM_DOXYGEN)
PICOTM_NOTHROW
/**
 * \ingroup group_libc
 * A transaction-safe implementation of [malloc()][posix::malloc].
 *
 * [posix::malloc]:
 *  http://pubs.opengroup.org/onlinepubs/9699919799/functions/malloc.html
 */
void*
malloc_tx(size_t size);
#endif

#if defined(PICOTM_LIBC_HAVE_MKDTEMP) && PICOTM_LIBC_HAVE_MKDTEMP && \
    !defined(__MACH__) || \
    defined(__PICOTM_DOXYGEN)
PICOTM_NOTHROW
/**
 * \ingroup group_libc
 * A transaction-safe implementation of [mkdtemp()][posix::mkdtemp].
 *
 * [posix::mkdtemp]:
 *  http://pubs.opengroup.org/onlinepubs/9699919799/functions/mkdtemp.html
 */
char*
mkdtemp_tx(char* template);
#endif

#if defined(PICOTM_LIBC_HAVE_MKSTEMP) && PICOTM_LIBC_HAVE_MKSTEMP || \
    defined(__PICOTM_DOXYGEN)
PICOTM_NOTHROW
/**
 * \ingroup group_libc
 * A transaction-safe implementation of [mkstemp()][posix::mkstemp].
 *
 * [posix::mkstemp]:
 *  http://pubs.opengroup.org/onlinepubs/9699919799/functions/mkstemp.html
 */
int
mkstemp_tx(char* template);
#endif

#if defined(PICOTM_LIBC_HAVE_POSIX_MEMALIGN) && \
        PICOTM_LIBC_HAVE_POSIX_MEMALIGN || \
    defined(__PICOTM_DOXYGEN)
PICOTM_NOTHROW
/**
 * \ingroup group_libc
 * A transaction-safe implementation of [posix_memalign()][posix::posix_memalign].
 *
 * [posix::posix_memalign]:
 *  http://pubs.opengroup.org/onlinepubs/9699919799/functions/posix_memalign.html
 */
int
posix_memalign_tx(void** memptr, size_t alignment, size_t size);
#endif

#if defined(PICOTM_LIBC_HAVE_QSORT) && PICOTM_LIBC_HAVE_QSORT || \
    defined(__PICOTM_DOXYGEN)
PICOTM_NOTHROW
/**
 * \ingroup group_libc
 * A transaction-safe implementation of [qsort()][posix::qsort].
 *
 * [posix::qsort]:
 *  http://pubs.opengroup.org/onlinepubs/9699919799/functions/qsort.html
 */
void
qsort_tx(void* base, size_t nel, size_t width,
         int (*compar)(const void*, const void*));
#endif

#if defined(PICOTM_LIBC_HAVE_RAND_R) && PICOTM_LIBC_HAVE_RAND_R || \
    defined(__PICOTM_DOXYGEN)
PICOTM_NOTHROW
/**
 * \ingroup group_libc
 * A transaction-safe implementation of [rand_r()][posix::rand_r].
 *
 * [posix::rand_r]:
 *  http://pubs.opengroup.org/onlinepubs/9699919799/functions/rand_r.html
 */
int
rand_r_tx(unsigned int* seed);
#endif

#if defined(PICOTM_LIBC_HAVE_REALLOC) && PICOTM_LIBC_HAVE_REALLOC || \
    defined(__PICOTM_DOXYGEN)
PICOTM_NOTHROW
/**
 * \ingroup group_libc
 * A transaction-safe implementation of [realloc()][posix::realloc].
 *
 * [posix::realloc]:
 *  http://pubs.opengroup.org/onlinepubs/9699919799/functions/realloc.html
 */
void*
realloc_tx(void* ptr, size_t size);
#endif

PICOTM_END_DECLS
