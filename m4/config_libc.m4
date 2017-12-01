#
# SYNOPSIS
#
#   CONFIG_LIBC
#
# LICENSE
#
#   Copyright (c) 2017 Thomas Zimmermann <tdz@users.sourceforge.net>
#
#   Copying and distribution of this file, with or without modification,
#   are permitted in any medium without royalty provided the copyright
#   notice and this notice are preserved.  This file is offered as-is,
#   without any warranty.

AC_DEFUN([_CHECK_LIBC_ERRNO_H], [

    AC_CHECK_HEADERS([errno.h])

    if test "x$ac_cv_header_errno_h" != "xno"; then

        #
        # Public interfaces
        #

        _CHECK_MODULE_INTF([libc], [errno], [[#include <errno.h>]])
    fi
])

AC_DEFUN([_CHECK_LIBC_FCNTL_H], [

    AC_CHECK_HEADERS([fcntl.h])

    if test "x$ac_cv_header_fcntl_h" != "xno"; then

        #
        # Public interfaces
        #

        _CHECK_MODULE_INTF([libc], [creat], [[#include <fcntl.h>]])
        _CHECK_MODULE_INTF([libc], [fcntl], [[#include <fcntl.h>]])
        _CHECK_MODULE_INTF([libc], [open],  [[#include <fcntl.h>]])
    fi
])

AC_DEFUN([_CHECK_LIBC_SCHED_H], [

    AC_CHECK_HEADERS([sched.h])

    if test "x$ac_cv_header_sched_h" != "xno"; then

        #
        # Public interfaces
        #

        _CHECK_MODULE_INTF([libc], [sched_yield], [[#include <sched.h>]])
    fi
])

AC_DEFUN([_CHECK_LIBC_STDIO_H], [

    AC_CHECK_HEADERS([stdio.h])

    if test "x$ac_cv_header_stdio_h" != "xno"; then

        #
        # Public interfaces
        #

        _CHECK_MODULE_INTF([libc], [snprintf],  [[#include <stdio.h>]])
        _CHECK_MODULE_INTF([libc], [sscanf],    [[#include <stdio.h>]])
        _CHECK_MODULE_INTF([libc], [vsnprintf], [[#include <stdio.h>]])
        _CHECK_MODULE_INTF([libc], [vsscanf],   [[#include <stdio.h>]])
    fi
])

AC_DEFUN([_CHECK_LIBC_STDLIB_H], [

    AC_CHECK_HEADERS([stdlib.h])

    if test "x$ac_cv_header_stdlib_h" != "xno"; then

        #
        # Public interfaces
        #

        _CHECK_MODULE_INTF([libc], [_Exit],          [[#include <stdlib.h>]])
        _CHECK_MODULE_INTF([libc], [abort],          [[#include <stdlib.h>]])
        _CHECK_MODULE_INTF([libc], [calloc],         [[#include <stdlib.h>]])
        _CHECK_MODULE_INTF([libc], [exit],           [[#include <stdlib.h>]])
        _CHECK_MODULE_INTF([libc], [free],           [[#include <stdlib.h>]])
        _CHECK_MODULE_INTF([libc], [malloc],         [[#include <stdlib.h>]])
        _CHECK_MODULE_INTF([libc], [mkdtemp],        [[#include <stdlib.h>]])
        _CHECK_MODULE_INTF([libc], [mkstemp],        [[#include <stdlib.h>]])
        _CHECK_MODULE_INTF([libc], [posix_memalign], [[#include <stdlib.h>]])
        _CHECK_MODULE_INTF([libc], [qsort],          [[#include <stdlib.h>]])
        _CHECK_MODULE_INTF([libc], [realloc],        [[#include <stdlib.h>]])
        _CHECK_MODULE_INTF([libc], [rand_r],         [[#include <stdlib.h>]])
    fi
])

AC_DEFUN([_CHECK_LIBC_STRING_H], [

    AC_CHECK_HEADERS([string.h])

    if test "x$ac_cv_header_string_h" != "xno"; then

        #
        # Public interfaces
        #

        _CHECK_MODULE_INTF([libc], [memccpy],    [[#include <string.h>]])
        _CHECK_MODULE_INTF([libc], [memchr],     [[#include <string.h>]])
        _CHECK_MODULE_INTF([libc], [memcmp],     [[#include <string.h>]])
        _CHECK_MODULE_INTF([libc], [memcpy],     [[#include <string.h>]])
        _CHECK_MODULE_INTF([libc], [memmove],    [[#include <string.h>]])
        _CHECK_MODULE_INTF([libc], [memset],     [[#include <string.h>]])
        _CHECK_MODULE_INTF([libc], [memrchr],    [[#include <string.h>]])
        _CHECK_MODULE_INTF([libc], [rawmemchr],  [[#include <string.h>]])
        _CHECK_MODULE_INTF([libc], [stpcpy],     [[#include <string.h>]])
        _CHECK_MODULE_INTF([libc], [stpncpy],    [[#include <string.h>]])
        _CHECK_MODULE_INTF([libc], [strcat],     [[#include <string.h>]])
        _CHECK_MODULE_INTF([libc], [strchr],     [[#include <string.h>]])
        _CHECK_MODULE_INTF([libc], [strcmp],     [[#include <string.h>]])
        _CHECK_MODULE_INTF([libc], [strcoll_l],  [[#include <string.h>]])
        _CHECK_MODULE_INTF([libc], [strcpy],     [[#include <string.h>]])
        _CHECK_MODULE_INTF([libc], [strcspn],    [[#include <string.h>]])
        _CHECK_MODULE_INTF([libc], [strdup],     [[#include <string.h>]])
        _CHECK_MODULE_INTF([libc], [strerror_r], [[#include <string.h>]])
        _CHECK_MODULE_INTF([libc], [strlen],     [[#include <string.h>]])
        _CHECK_MODULE_INTF([libc], [strncat],    [[#include <string.h>]])
        _CHECK_MODULE_INTF([libc], [strncmp],    [[#include <string.h>]])
        _CHECK_MODULE_INTF([libc], [strncpy],    [[#include <string.h>]])
        _CHECK_MODULE_INTF([libc], [strndup],    [[#include <string.h>]])
        _CHECK_MODULE_INTF([libc], [strnlen],    [[#include <string.h>]])
        _CHECK_MODULE_INTF([libc], [strpbrk],    [[#include <string.h>]])
        _CHECK_MODULE_INTF([libc], [strrchr],    [[#include <string.h>]])
        _CHECK_MODULE_INTF([libc], [strspn],     [[#include <string.h>]])
        _CHECK_MODULE_INTF([libc], [strstr],     [[#include <string.h>]])
        _CHECK_MODULE_INTF([libc], [strtok_r],   [[#include <string.h>]])
    fi
])

AC_DEFUN([CONFIG_LIBC], [
    AC_CHECK_LIB([c], [longjmp])
    if test "x$ac_cv_lib_c_memcpy" != "xno"; then

        #
        # libc compile-time constants
        #

        AC_DEFINE([MAXNUMFD], [1024], [Maximum number of file descriptors])
        AC_DEFINE([RECBITS], [5], [Bits per file record])

        #
        # System interfaces and functionality
        #

        _CHECK_LIBC_ERRNO_H
        _CHECK_LIBC_FCNTL_H
        _CHECK_LIBC_SCHED_H
        _CHECK_LIBC_STDIO_H
        _CHECK_LIBC_STDLIB_H
        _CHECK_LIBC_STRING_H
    fi
])
