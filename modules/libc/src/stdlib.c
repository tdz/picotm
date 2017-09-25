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

#include "picotm/stdlib.h"
#include <errno.h>
#include <malloc.h>
#include <picotm/picotm.h>
#include <picotm/picotm-module.h>
#include <string.h>
#include "allocator/module.h"
#include "error/module.h"
#include "picotm/stdlib-tm.h"

PICOTM_EXPORT
void
_Exit_tx(int status)
{
    __picotm_commit();
    _Exit(status);
}

PICOTM_EXPORT
void
abort_tx()
{
    __picotm_commit();
    abort();
}

PICOTM_EXPORT
void*
calloc_tx(size_t nmemb, size_t size)
{
    error_module_save_errno();

    size_t alloc_size = nmemb * size;

    void* mem;
    allocator_module_posix_memalign(&mem, 2 * sizeof(void*), alloc_size);

    return memset(mem, 0, alloc_size);
}

PICOTM_EXPORT
void
exit_tx(int status)
{
    __picotm_commit();
    exit(status);
}

PICOTM_EXPORT
void
free_tx(void* ptr)
{
    size_t usiz = malloc_usable_size(ptr);
    if (usiz) {
        privatize_tx(ptr, usiz, PICOTM_TM_PRIVATIZE_LOADSTORE);
    }
    allocator_module_free(ptr, usiz);
}

PICOTM_EXPORT
void*
malloc_tx(size_t size)
{
    error_module_save_errno();

    void* mem;
    allocator_module_posix_memalign(&mem, 2 * sizeof(void*), size);

    return mem;
}

PICOTM_EXPORT
char*
mkdtemp_tx(char* template)
{
    privatize_c_tx(template, '\0', PICOTM_TM_PRIVATIZE_LOADSTORE);
    return mkdtemp_tm(template);
}

PICOTM_EXPORT
int
mkstemp_tx(char* template)
{
    privatize_c_tx(template, '\0', PICOTM_TM_PRIVATIZE_LOADSTORE);
    return mkstemp_tm(template);
}

PICOTM_EXPORT
int
posix_memalign_tx(void** memptr, size_t alignment, size_t size)
{
    privatize_tx(*memptr, sizeof(*memptr), PICOTM_TM_PRIVATIZE_STORE);
    return posix_memalign_tm(memptr, alignment, size);
}

PICOTM_EXPORT
void
qsort_tx(void* base, size_t nel, size_t width,
         int (*compar)(const void*, const void*))
{
    privatize_tx(base, nel * width, PICOTM_TM_PRIVATIZE_LOADSTORE);
    qsort_tm(base, nel, width, compar);
}

PICOTM_EXPORT
void*
realloc_tx(void* ptr, size_t size)
{
    size_t usiz = malloc_usable_size(ptr);
    if (usiz) {
        privatize_tx(ptr, usiz, PICOTM_TM_PRIVATIZE_LOADSTORE);
    }

    error_module_save_errno();

    void* mem = NULL;

    if (size) {
        allocator_module_posix_memalign(&mem, 2 * sizeof(void*), size);
    }

    if (ptr && mem) {
        /* Valgrind might report invalid reads and out-of-bounds access
         * within this function. This is a false positive. The result of
         * malloc_usable_size() is the maximum available buffer space,
         * not the amount of allocated or valid memory. Any memcpy() could
         * therefore operate on uninitialized data.
         */
        memcpy(mem, ptr, size < usiz ? size : usiz);
    }

    if (ptr && !size) {
        allocator_module_free(ptr, usiz);
    }

    return mem;
}

PICOTM_EXPORT
int
rand_r_tx(unsigned int* seed)
{
    privatize_tx(seed, sizeof(*seed), PICOTM_TM_PRIVATIZE_LOADSTORE);
    return rand_r_tm(seed);
}