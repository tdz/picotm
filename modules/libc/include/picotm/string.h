/* Permission is hereby granted, free of charge, to any person obtaining a
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
 */

#pragma once

#include <picotm/compiler.h>
#include <string.h>

PICOTM_BEGIN_DECLS

/**
 * \ingroup group_libc
 * \file
 *
 * \brief Transactional wrappers for interfaces of <string.h>.
 */

/*
 * Memory functions
 */

PICOTM_NOTHROW
/**
 * A transaction-safe implementation of [memccpy()][posix::memccpy].
 *
 * [posix::memccpy]:
 *  http://pubs.opengroup.org/onlinepubs/9699919799/functions/memccpy.html
 */
void*
memccpy_tx(void* restrict dest, const void* restrict src, int c, size_t n);

PICOTM_NOTHROW
/**
 * A transaction-safe implementation of [memchr()][posix::memchr].
 *
 * [posix::memchr]:
 *  http://pubs.opengroup.org/onlinepubs/9699919799/functions/memchr.html
 */
void*
memchr_tx(const void* s, int c, size_t n);

PICOTM_NOTHROW
/**
 * A transaction-safe implementation of [memcmp()][posix::memcmp].
 *
 * [posix::memcmp]:
 *  http://pubs.opengroup.org/onlinepubs/9699919799/functions/memcmp.html
 */
int
memcmp_tx(const void* s1, const void* s2, size_t n);

PICOTM_NOTHROW
/**
 * A transaction-safe implementation of [memcpy()][posix::memcpy].
 *
 * [posix::memcpy]:
 *  http://pubs.opengroup.org/onlinepubs/9699919799/functions/memcpy.html
 */
void*
memcpy_tx(void* restrict dest, const void* restrict src, size_t n);

PICOTM_NOTHROW
/**
 * A transaction-safe implementation of [memmove()][posix::memmove].
 *
 * [posix::memmove]:
 *  http://pubs.opengroup.org/onlinepubs/9699919799/functions/memmove.html
 */
void*
memmove_tx(void* dest, const void* src, size_t n);

PICOTM_NOTHROW
/**
 * A transaction-safe implementation of [memset()][posix::memset].
 *
 * [posix::memset]:
 *  http://pubs.opengroup.org/onlinepubs/9699919799/functions/memset.html
 */
void*
memset_tx(void* s, int c, size_t n);

PICOTM_NOTHROW
/**
 * A transaction-safe implementation of [memrchr()][posix::memrchr].
 *
 * [posix::memrchr]:
 *  http://pubs.opengroup.org/onlinepubs/9699919799/functions/memrchr.html
 */
void*
memrchr_tx(const void* s, int c, size_t n);

PICOTM_NOTHROW
/**
 * A transaction-safe implementation of [rawmemchr()][gnu::rawmemchr].
 *
 * [gnu::rawmemchr]:
 *  https://www.gnu.org/software/libc/manual/html_node/Search-Functions.html#index-rawmemchr
 */
void*
rawmemchr_tx(const void* s, int c);

/*
 * String functions
 */

PICOTM_NOTHROW
/**
 * A transaction-safe implementation of [stpcpy()][posix::stpcpy].
 *
 * [posix::stpcpy]:
 *  http://pubs.opengroup.org/onlinepubs/9699919799/functions/stpcpy.html
 */
char*
stpcpy_tx(char* restrict dest, const char* restrict src);

PICOTM_NOTHROW
/**
 * A transaction-safe implementation of [stpncpy()][posix::stpncpy].
 *
 * [posix::stpncpy]:
 *  http://pubs.opengroup.org/onlinepubs/9699919799/functions/stpncpy.html
 */
char*
stpncpy_tx(char* restrict dest, const char* restrict src, size_t n);

PICOTM_NOTHROW
/**
 * A transaction-safe implementation of [strcat()][posix::strcat].
 *
 * [posix::strcat]:
 *  http://pubs.opengroup.org/onlinepubs/9699919799/functions/strcat.html
 */
char*
strcat_tx(char* restrict dest, const char* restrict src);

PICOTM_NOTHROW
/**
 * A transaction-safe implementation of [strchr()][posix::strchr_tx].
 *
 * [posix::strchr]:
 *  http://pubs.opengroup.org/onlinepubs/9699919799/functions/strchr.html
 */
char*
strchr_tx(const char* s, int c);

PICOTM_NOTHROW
/**
 * A transaction-safe implementation of [strcmp()][posix::strcmp].
 *
 * [posix::strcmp]:
 *  http://pubs.opengroup.org/onlinepubs/9699919799/functions/strcmp.html
 */
int
strcmp_tx(const char* s1, const char* s2);

PICOTM_NOTHROW
/**
 * A transaction-safe implementation of [strcoll_l()][posix::strcoll_l].
 *
 * [posix::strcoll_l]:
 *  http://pubs.opengroup.org/onlinepubs/9699919799/functions/strcoll_l.html
 */
int
strcoll_l_tx(const char* s1, const char* s2, locale_t locale);

PICOTM_NOTHROW
/**
 * A transaction-safe implementation of [strcpy()][posix::strcpy].
 *
 * [posix::strcpy]:
 *  http://pubs.opengroup.org/onlinepubs/9699919799/functions/strcpy.html
 */
char*
strcpy_tx(char* restrict dest, const char* restrict src);

PICOTM_NOTHROW
/**
 * A transaction-safe implementation of [strcspn()][posix::strcspn].
 *
 * [posix::strcspn]:
 *  http://pubs.opengroup.org/onlinepubs/9699919799/functions/strcspn.html
 */
size_t
strcspn_tx(const char* s, const char* reject);

PICOTM_NOTHROW
/**
 * A transaction-safe implementation of [strdup()][posix::strdup].
 *
 * [posix::strdup]:
 *  http://pubs.opengroup.org/onlinepubs/9699919799/functions/strdup.html
 */
char*
strdup_tx(const char* s);

PICOTM_NOTHROW
/**
 * A transaction-safe implementation of [strerror_r()][gnu::strerror_r].
 *
 * \warning This is an internal interface. Call strerror_r_tx() instead.
 *
 * [gnu::strerror_r]:
 *  https://www.gnu.org/software/libc/manual/html_node/Error-Messages.html#index-strerror_005fr
 */
char*
__strerror_r_gnu_tx(int errnum, char* buf, size_t buflen);

PICOTM_NOTHROW
/**
 * A transaction-safe implementation of [strerror_r()][posix::strerror_r].
 *
 * \warning This is an internal interface. Call strerror_r_tx() instead.
 *
 * [posix::strerror_r]:
 *  http://pubs.opengroup.org/onlinepubs/9699919799/functions/strerror_r.html
 */
int
__strerror_r_posix_tx(int errnum, char* buf, size_t buflen);

#if (_POSIX_C_SOURCE >= 200112L) && !_GNU_SOURCE
#define strerror_r_tx   __strerror_r_posix_tx
#else
#define strerror_r_tx   __strerror_r_gnu_tx
#endif

PICOTM_NOTHROW
/**
 * A transaction-safe implementation of [strlen()][posix::strlen].
 *
 * [posix::strlen]:
 *  http://pubs.opengroup.org/onlinepubs/9699919799/functions/strlen.html
 */
size_t
strlen_tx(const char* s);

PICOTM_NOTHROW
/**
 * A transaction-safe implementation of [strncat()][posix::strncat].
 *
 * [posix::strncat]:
 *  http://pubs.opengroup.org/onlinepubs/9699919799/functions/strncat.html
 */
char*
strncat_tx(char* restrict dest, const char* restrict src, size_t n);

PICOTM_NOTHROW
/**
 * A transaction-safe implementation of [strncmp()][posix::strncmp].
 *
 * [posix::strncmp]:
 *  http://pubs.opengroup.org/onlinepubs/9699919799/functions/strncmp.html
 */
int
strncmp_tx(const char* s1, const char* s2, size_t n);

PICOTM_NOTHROW
/**
 * A transaction-safe implementation of [strncpy()][posix::strncpy].
 *
 * [posix::strncpy]:
 *  http://pubs.opengroup.org/onlinepubs/9699919799/functions/strncpy.html
 */
char*
strncpy_tx(char* restrict dest, const char* restrict src, size_t n);

PICOTM_NOTHROW
/**
 * A transaction-safe implementation of [strndup()][posix::strndup].
 *
 * [posix::strndup]:
 *  http://pubs.opengroup.org/onlinepubs/9699919799/functions/strndup.html
 */
char*
strndup_tx(const char* s, size_t n);

PICOTM_NOTHROW
/**
 * A transaction-safe implementation of [strnlen()][posix::strnlen].
 *
 * [posix::strnlen]:
 *  http://pubs.opengroup.org/onlinepubs/9699919799/functions/strnlen.html
 */
size_t
strnlen_tx(const char* s, size_t maxlen);

PICOTM_NOTHROW
/**
 * A transaction-safe implementation of [strpbrk()][posix::strpbrk].
 *
 * [posix::strpbrk]:
 *  http://pubs.opengroup.org/onlinepubs/9699919799/functions/strpbrk.html
 */
char*
strpbrk_tx(const char* s, const char* accept);

PICOTM_NOTHROW
/**
 * A transaction-safe implementation of [strrchr()][posix::strrchr].
 *
 * [posix::strrchr]:
 *  http://pubs.opengroup.org/onlinepubs/9699919799/functions/strrchr.html
 */
char*
strrchr_tx(const char* s, int c);

PICOTM_NOTHROW
/**
 * A transaction-safe implementation of [strspn()][posix::strspn].
 *
 * [posix::strspn]:
 *  http://pubs.opengroup.org/onlinepubs/9699919799/functions/strspn.html
 */
size_t
strspn_tx(const char* s, const char* accept);

PICOTM_NOTHROW
/**
 * A transaction-safe implementation of [strstr()][posix::strstr].
 *
 * [posix::strstr]:
 *  http://pubs.opengroup.org/onlinepubs/9699919799/functions/strstr.html
 */
char*
strstr_tx(const char* haystack, const char* needle);

PICOTM_NOTHROW
/**
 * A transaction-safe implementation of [strtok_r()][posix::strtok_r].
 *
 * [posix::strtok_r]:
 *  http://pubs.opengroup.org/onlinepubs/9699919799/functions/strtok_r.html
 */
char*
strtok_r_tx(char* restrict str, const char* restrict delim,
            char** restrict saveptr);

PICOTM_END_DECLS