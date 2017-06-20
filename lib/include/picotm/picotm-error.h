/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

/**
 * \ingroup group_lib
 * \ingroup group_modules
 * \file
 *
 * \brief Contains struct picotm_error and helper functions.
 */

#include <stdbool.h>
#include <stddef.h>
#include "compiler.h"

PICOTM_BEGIN_DECLS

/**
 * Signals detected errors to picotm.
 *
 * If a component detects an error, it should prefer setting the system-
 * specific error code that was returned by the failed call. This is more
 * specific than the generic one's below. In some portable code, such as
 * the tm module, using the generic codes might be preferable.
 */
enum picotm_error_code {
    /** The exact error is unknown. */
    PICOTM_GENERAL_ERROR = 0,
    /** Out-of-Memory error. */
    PICOTM_OUT_OF_MEMORY,
    /** Invalid floating-point environment. */
    PICOTM_INVALID_FENV
};

/**
 * Signals error status to picotm.
 */
enum picotm_error_status {
    /** Conflict among transactions detected. */
    PICOTM_CONFLICTING = 1,
    /** Transaction requires irrevocability to continue. */
    PICOTM_REVOCABLE,
    /** Error detected. Encoded as `enum picotm_error_code`. */
    PICOTM_ERROR_CODE,
    /** Error detected. Encoded as errno code. */
    PICOTM_ERRNO
};

/**
 * Describes an error.
 */
struct picotm_error {
    /** The status code of the error. */
    enum picotm_error_status status;

    /** True is the error is non-recoverable, or false otherwise. */
    bool is_non_recoverable;

    /** A string with additonal information about the error. */
    const char* description;

    union {
        /** The conflicting transaction for PICOTM_CONFLICTING if known, or NULL otherwise. */
        void*                  conflicting_tx;
        /** The picotm error code for PICOTM_ERROR_CODE if known, or PICOTM_GENERAL_ERROR otherwise. */
        enum picotm_error_code error_hint;
        /** The picotm errno for PICOTM_ERRNO if known, or 0 otherwise. */
        int                    errno_hint;
    } value;
};

#define PICOTM_ERROR_INITIALIZER    \
    { \
        .status = 0, \
        .is_non_recoverable = false, \
        .description = NULL \
    }

PICOTM_NOTHROW
/**
 * Clears an error structure.
 *
 * \param error The error to clear.
 */
void
picotm_error_clear(struct picotm_error* error);

PICOTM_NOTHROW
/**
 * Sets an error of type PICOTM_CONFLICTING.
 *
 * \param error             The error to set.
 * \param conflicting_tx    The conflicting transaction if known, or NULL
 *                          otherwise.
 */
void
picotm_error_set_conflicting(struct picotm_error* error, void* conflicting_tx);

PICOTM_NOTHROW
/**
 * Sets an error of type PICOTM_REVOCABLE.
 *
 * \param error The error to set.
 */
void
picotm_error_set_revocable(struct picotm_error* error);

PICOTM_NOTHROW
/**
 * Sets an error of type PICOTM_ERROR_CODE.
 *
 * \param error         The error to set.
 * \param error_hint    The picotm error code if known, or
 *                      PICOTM_GENERAL_ERROR otherwise.
 */
void
picotm_error_set_error_code(struct picotm_error* error,
                            enum picotm_error_code error_hint);

PICOTM_NOTHROW
/**
 * Sets an error of type PICOTM_ERRNO.
 *
 * \param error         The error to set.
 * \param errno_hint    The errno code if known, or 0 otherwise.
 */
void
picotm_error_set_errno(struct picotm_error* error, int errno_hint);

/**
 * Tests if an error has been set.
 *
 * \param error The error to set.
 * \returns     True if an error has been set, or false otherwise.
 */
static inline bool
picotm_error_is_set(const struct picotm_error* error)
{
    return !!error->status;
}

PICOTM_NOTHROW
/**
 * Tests if an error has been set to CONFLICTING status.
 *
 * \param error The error to set.
 * \returns     True if an error has been set to CONFLICTING, or false
 *              otherwise.
 */
bool
picotm_error_is_conflicting(const struct picotm_error* error);

PICOTM_NOTHROW
/**
 * Tests if an error has been set to REVOCABLE status.
 *
 * \param error The error to set.
 * \returns     True if an error has been set to REVOCABLE, or false
 *              otherwise.
 */
bool
picotm_error_is_revocable(const struct picotm_error* error);

PICOTM_NOTHROW
/**
 * Tests if an error has been set to an error status.
 *
 * \param error The error to set.
 * \returns     True if an error has been set to an error status, or false
 *              otherwise.
 */
bool
picotm_error_is_error(const struct picotm_error* error);

PICOTM_NOTHROW
/**
 * Marks an error as non-recoverable.
 *
 * By default, errors are expected to recoverable. For irrevocable
 * transactions or after applying events from the transaction log,
 * errors cannot easily be recovered, because the program state is
 * undefined in these situations. An errors that happens under such
 * conditions is marked as being non-recoverable.
 *
 * Normally, modules should not care baout this flag. The commit logic
 * within picotm will set this flag if appropriate.
 *
 * \param error The error to mark as non-recoverable.
 */
void
picotm_error_mark_as_non_recoverable(struct picotm_error* error);

PICOTM_NOTHROW
/**
 * Sets an error's description.
 *
 * \param error         The error.
 * \param description   A descriptive string.
 */
void
picotm_error_set_description(struct picotm_error* error,
                             const char* description);

PICOTM_NOTHROW
/**
 * Returns an error's description.
 *
 * \param error The error.
 * \returns     The error description if set, or NULL otherwise.
 */
const char*
picotm_error_get_description(const struct picotm_error* error);

PICOTM_END_DECLS
