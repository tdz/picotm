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

/**
 * \ingroup group_modules
 * \file
 */

#include <assert.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include "compiler.h"

PICOTM_BEGIN_DECLS

/**
 * A 16-bit thread-local reference counter.
 */
struct picotm_ref16 {
    uint_fast16_t count;
};

/* Initializes the counter with the given value.
 * \warning This is an internal interface. Don't use it in application code.
 */
static inline void
__picotm_ref16_init(struct picotm_ref16* self, unsigned long count)
{
    self->count = count;
}

/* Increments a reference counter.
 * \warning This is an internal interface. Don't use it in application code.
 */
static inline bool
__picotm_ref16_up(struct picotm_ref16* self)
{
    uint16_t old_ref = self->count++;
    assert(old_ref != (uint16_t)-1);

    return (old_ref == 0);
}

/* Decrements a reference counter.
 * \warning This is an internal interface. Don't use it in application code.
 */
static inline bool
__picotm_ref16_down(struct picotm_ref16* self)
{
    uint16_t old_ref = self->count--;
    assert(old_ref != 0);

    return (old_ref == 1);
}

/* Reads the reference counter's value.
 * \warning This is an internal interface. Don't use it in application code.
 */
static inline uint16_t
__picotm_ref16_count(const struct picotm_ref16* self)
{
    return self->count;
}

/**
 * A 16-bit shared reference counter.
 */
struct picotm_shared_ref16 {
    atomic_uint_fast16_t count;
};

/* Initializes the counter with the given value.
 * \warning This is an internal interface. Don't use it in application code.
 */
static inline void
__picotm_shared_ref16_init(struct picotm_shared_ref16* self, uint16_t count)
{
    atomic_init(&self->count, count);
}

/* Increments a reference counter.
 * \warning This is an internal interface. Don't use it in application code.
 */
static inline bool
__picotm_shared_ref16_up(struct picotm_shared_ref16* self)
{
    uint16_t old_ref = atomic_fetch_add_explicit(&self->count, 1,
                                                 memory_order_acq_rel);
    assert(old_ref != (uint16_t)-1);

    return (old_ref == 0);
}

/* Decrements a reference counter.
 * \warning This is an internal interface. Don't use it in application code.
 */
static inline bool
__picotm_shared_ref16_down(struct picotm_shared_ref16* self)
{
    uint16_t old_ref = atomic_fetch_sub_explicit(&self->count, 1,
                                                 memory_order_acq_rel);
    assert(old_ref != 0);

    return (old_ref == 1);
}

/* Reads the reference counter's value.
 * \warning This is an internal interface. Don't use it in application code.
 */
static inline uint16_t
__picotm_shared_ref16_count(const struct picotm_shared_ref16* self)
{
#if __clang
    return atomic_load_explicit(&self->count, memory_order_acquire);
#else
    return atomic_load_explicit((atomic_uint_fast16_t*)&self->count,
                                memory_order_acquire);
#endif
}

/**
 * Initializes a reference counter with the given value.
 *
 * \param   self    A reference counter
 * \param   count   The initial reference count
 */
#define picotm_ref_init(self, count) _Generic(self,             \
    struct picotm_ref16*:           __picotm_ref16_init,        \
    struct picotm_shared_ref16*:    __picotm_shared_ref16_init) \
    (self, count)

/**
 * Increments a reference counter.
 *
 * \param   self    A reference counter
 * \returns True is this is the first reference, false otherwise.
 */
#define picotm_ref_up(self) _Generic(self,                      \
    struct picotm_ref16*:           __picotm_ref16_up,          \
    struct picotm_shared_ref16*:    __picotm_shared_ref16_up)   \
    (self)

/**
 * Decrements a reference counter.
 *
 * \param   self    A reference counter
 * \returns True is this is the final reference, false otherwise.
 */
#define picotm_ref_down(self) _Generic(self,                    \
    struct picotm_ref16*:           __picotm_ref16_down,        \
    struct picotm_shared_ref16*:    __picotm_shared_ref16_down) \
    (self)

/**
 * Reads a reference counter's value.
 *
 * \param   self     reference counter
 * \returns The current value of the reference counter.
 */
#define picotm_ref_count(self) _Generic(self,                           \
    const struct picotm_ref16*:         __picotm_ref16_count,           \
          struct picotm_ref16*:         __picotm_ref16_count,           \
    const struct picotm_shared_ref16*:  __picotm_shared_ref16_count,    \
          struct picotm_shared_ref16*:  __picotm_shared_ref16_count)    \
    (self)

PICOTM_END_DECLS