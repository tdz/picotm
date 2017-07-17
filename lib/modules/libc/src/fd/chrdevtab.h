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

#include <stdbool.h>
#include <stddef.h>

/**
 * \cond impl || libc_impl || libc_impl_fd
 * \ingroup libc_impl
 * \ingroup libc_impl_fd
 * \file
 * \endcond
 */

struct chrdev;
struct picotm_error;

/**
 * Returns a reference to an chrdev structure for the given file descriptor.
 *
 * \param       fildes      A file descriptor.
 * \param       want_new    True to request a new instance.
 * \param[out]  error       Returns an error.
 * \returns A referenced instance of `struct chrdev` that refers to the file
 *          descriptor's open file description.
 */
struct chrdev*
chrdevtab_ref_fildes(int fildes, bool want_new, struct picotm_error* error);

/**
 * Returns the index of an chrdev structure within the chrdev table.
 *
 * \param   chrdev An chrdev structure.
 * \returns The chrdev structure's index in the chrdev table.
 */
size_t
chrdevtab_index(struct chrdev* chrdev);