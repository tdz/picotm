/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <picotm/compiler.h>

PICOTM_BEGIN_DECLS

/**
 * \ingroup group_libc
 * \file
 *
 * \brief Public interfaces of picotm's libc module.
 */

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
 * \param   recovery    The error-recovering strategy.
 */
void
picotm_libc_set_error_recovery(enum picotm_libc_error_recovery recovery);

PICOTM_NOTHROW
/**
 * Returns the strategy for deciding when to recover from errors.
 * \returns The current error-recovering strategy.
 */
enum picotm_libc_error_recovery
picotm_libc_get_error_recovery(void);

/*
 * File I/O
 */

/**
 * File-type constants.
 */
enum picotm_libc_file_type {
    PICOTM_LIBC_FILE_TYPE_OTHER = 0, /**< \brief Any file */
    PICOTM_LIBC_FILE_TYPE_REGULAR,   /**< \brief Regular file */
    PICOTM_LIBC_FILE_TYPE_FIFO,      /**< \brief FIFO */
    PICOTM_LIBC_FILE_TYPE_SOCKET     /**< \brief Socket */
};

/**
 * Concurrency-control mode for file-descriptor I/O.
 */
enum picotm_libc_cc_mode {
    /** Set CC mode to irrevocablilty */
    PICOTM_LIBC_CC_MODE_NOUNDO = 0,
    /** Set CC mode to pessimistic two-phase locking */
    PICOTM_LIBC_CC_MODE_2PL
};

/**
 * Validation mode for optimistic domains.
 */
enum picotm_libc_validation_mode {
    /** Validate the current operation. */
    PICOTM_LIBC_VALIDATE_OP = 0,
    /** Validate the current domain. */
    PICOTM_LIBC_VALIDATE_DOMAIN,
    /** Validate everything. */
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
 * \param   file_type   The file type to set the CC mode for.
 * \param   cc_mode     The CC mode.
 */
void
picotm_libc_set_file_type_cc_mode(enum picotm_libc_file_type file_type,
                                  enum picotm_libc_cc_mode cc_mode);

PICOTM_NOTHROW
/**
 * Returns the currently preferred mode of concurrency control for I/O on
 * a specific file type.
 * \param   file_type   The file type to get the CC mode from.
 * \returns             The current CC mode for the given file type.
 */
enum picotm_libc_cc_mode
picotm_libc_get_file_type_cc_mode(enum picotm_libc_file_type file_type);

PICOTM_NOTHROW
/**
 * Sets the mode of validation for optimistic domains.
 * \param   val_mode    The validation mode.
 */
void
picotm_libc_set_validation_mode(enum picotm_libc_validation_mode val_mode);

PICOTM_NOTHROW
/**
 * Returns the current mode of validation for optimistic domains.
 * \returns The current validation mode.
 */
enum picotm_libc_validation_mode
picotm_libc_get_validation_mode(void);

PICOTM_END_DECLS

/**
 * \defgroup group_libc The C Standard Library Module
 *
 * \brief Covers the C Standard Library module. This module offers many
 *        features of the C Standard Library and POSIX standard, such as
 *        string and memory functions, memory allocation, and file-descriptor
 *        I/O.
 *
 * The C Standard Library Module covers many features of the C Standard
 * Library and the POSIX standard.
 *
 * # Error Handling
 *
 * The C Standard Library module provides transaction-safe errno as errno_tx.
 * With a few exceptions, the transactional functions will handle errors for
 * you. Only rarely you should have a need to examine the errno status. If
 * accessed, the errno value is saved and restored during aborts.
 *
 * # Memory allocation
 *
 * You can allocate memory with malloc_tx(), calloc_tx() or
 * posix_memalign_tx(). The allocation is transaction-safe. On aborts,
 * allocated memory blocks will automatically be released. To explicitly
 * release a block of memory call free_tx(). If the transaction aborts
 * afterwards, the released memory block will be available again. For
 * reallocations call realloc_tx(). This call combines memory allocation
 * and releases in a single function.
 *
 * # String and Memory functions.
 *
 * The C Standard Library module supports a wide range of string and memory
 * helpers, such as strcpy(), strdup(), memmove(), or memcmp().
 *
 * # File-Descriptor I/O
 *
 * File descriptors can be used transactionally. The module protects file
 * descriptors, open file description, and file buffer; if possible. Call
 * open_tx() and close_tx() to open and close files within a transaction.
 *
 * File content is read with read_tx() or pread_tx(), and written with
 * write_tx() or pwrite_tx(). Depending on the operation, the file position
 * is updated.
 *
 * The module also supports FIFO and socket buffers; although some operations
 * require irrevocability.
 *
 * # File-System Support
 *
 * The file system is already a shared resource. Since the module only
 * supports  process-local transactions, at least POSIX-like semantics are
 * available. For example, link_tx() is performed immediately and
 * atomically, or temporary files created with mkstemp_tx() are removed
 * automatically on aborts.
 */
