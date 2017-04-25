/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <picotm/compiler.h>

PICOTM_BEGIN_DECLS

/*
 * Error handling
 */

/**
 * The error-recovery strategy for system calls.
 *
 * Each module detects errors and initiates recovery. Sometimes reported
 * errors are not failures of the component, but expected corner cases.
 * For example 'read()' on non-blocking file descriptors signals `EAGAIN`
 * if there's no data available.
 *
 * The libc modules has to distiguish such cases from actual errors to
 * decide when to initiate recovery. `enum picotm_libc_error_recovery` is
 * a list of possible strategies.
 *
 * It is recommended to use `PICOTM_LIBC_ERROR_RECOVERY_AUTO`.
 */
enum picotm_libc_error_recovery {
    /** Use heuristics to decide which errors to recover from. This is
     * the default. */
    PICOTM_LIBC_ERROR_RECOVERY_AUTO,
    /** Recover from all errors. */
    PICOTM_LIBC_ERROR_RECOVERY_FULL
};

PICOTM_NOTHROW
/**
 * Sets the strategy for deciding when to recover from errors.
 */
void
picotm_libc_set_error_recovery(enum picotm_libc_error_recovery recovery);

PICOTM_NOTHROW
/**
 * Returns the strategy for deciding when to recover from errors.
 */
enum picotm_libc_error_recovery
picotm_libc_get_error_recovery(void);

/*
 * File I/O
 */

/**
 * File-type constants
 */
enum picotm_libc_file_type {
    PICOTM_LIBC_FILE_TYPE_OTHER = 0, /**< \brief Any file */
    PICOTM_LIBC_FILE_TYPE_REGULAR,   /**< \brief Regular file */
    PICOTM_LIBC_FILE_TYPE_FIFO,      /**< \brief FIFO */
    PICOTM_LIBC_FILE_TYPE_SOCKET     /**< \brief Socket */
};

/**
 * File-I/O CC mode
 */
enum picotm_libc_cc_mode {
    PICOTM_LIBC_CC_MODE_NOUNDO = 0,  /**< \brief Set CC mode to irrevocablilty */
    PICOTM_LIBC_CC_MODE_TS,          /**< \brief Set CC mode to optimistic timestamp checking */
    PICOTM_LIBC_CC_MODE_2PL,         /**< \brief Set CC mode to pessimistic two-phase locking */
    PICOTM_LIBC_CC_MODE_2PL_EXT      /**< \brief (Inofficial)Set CC mode to pessimistic two-phase locking with socket commit protocol */
};

/**
 * Validation mode
 */
enum picotm_libc_validation_mode {
    PICOTM_LIBC_VALIDATE_OP = 0,
    PICOTM_LIBC_VALIDATE_DOMAIN,
    PICOTM_LIBC_VALIDATE_FULL
};

PICOTM_NOTHROW
/**
 * Saves the value of 'errno' during a transaction. Module authors should
 * call this function before invoking a function that might modify errno's
 * value.
 */
void
picotm_libc_save_errno(void);

PICOTM_NOTHROW
/**
 * Sets the preferred mode of concurrency control for I/O on a specific
 * file type.
 */
void
picotm_libc_set_file_type_cc_mode(enum picotm_libc_file_type file_type,
                                  enum picotm_libc_cc_mode cc_mode);

PICOTM_NOTHROW
/**
 * Returns the currently preferred mode of concurrency control for I/O on
 * a specific file type.
 */
enum picotm_libc_cc_mode
picotm_libc_get_file_type_cc_mode(enum picotm_libc_file_type file_type);

PICOTM_NOTHROW
/**
 * Sets the mode of validation.
 */
void
picotm_libc_set_validation_mode(enum picotm_libc_validation_mode val_mode);

PICOTM_NOTHROW
/**
 * Returns the current mode of validation.
 */
enum picotm_libc_validation_mode
picotm_libc_get_validation_mode(void);

PICOTM_END_DECLS