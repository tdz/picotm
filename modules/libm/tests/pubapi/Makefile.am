#
# picotm - A system-level transaction manager
# Copyright (c) 2017-2018   Thomas Zimmermann <contact@tzimmermann.org>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#
# SPDX-License-Identifier: LGPL-3.0-or-later
#

PUBAPI_TESTS = complex-pubapi-t1.test \
               complex-pubapi-t1-valgrind.test \
               complex-pubapi-t4.test \
               math-pubapi-t1.test \
               math-pubapi-t1-valgrind.test \
               math-pubapi-t4.test

TESTS =
if ENABLE_MODULE_LIBM
TESTS += $(PUBAPI_TESTS)
endif

EXTRA_DIST = $(PUBAPI_TESTS)

# Prepare environment for test scripts.
AM_TESTS_ENVIRONMENT = \
    PATH="$(top_srcdir)/tests/env:${PATH}"; \
    . $(top_builddir)/tests/env/tests-env.sh;

TEST_LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) \
                  $(top_srcdir)/build-aux/tap-driver.sh

if ENABLE_MODULE_LIBM
check_PROGRAMS = complex-pubapi \
                 math-pubapi
endif

complex_pubapi_SOURCES = complex_pubapi.c \
                         testmacro.c \
                         testmacro.h

math_pubapi_SOURCES = math_pubapi.c \
                      testmacro.c \
                      testmacro.h

AM_LDFLAGS = -static

LDADD = $(top_builddir)/tests/libtests/libpicotm_tests.la \
        $(top_builddir)/tests/libsafeblk/libpicotm_safeblk.la \
        $(top_builddir)/tests/libtap/libpicotm_tap.la \
        $(top_builddir)/modules/libm/src/libpicotm-m.la \
        $(top_builddir)/modules/libc/src/libpicotm-c.la \
        $(top_builddir)/modules/tm/src/libpicotm-tm.la \
        $(top_builddir)/src/libpicotm.la

AM_CPPFLAGS = -iquote $(top_builddir)/tests/libtests \
              -iquote $(top_srcdir)/tests/libtests \
              -iquote $(top_builddir)/tests/libsafeblk \
              -iquote $(top_srcdir)/tests/libsafeblk \
              -iquote $(top_builddir)/tests/libtap \
              -iquote $(top_srcdir)/tests/libtap \
              -iquote $(top_builddir)/modules/libm/include \
              -iquote $(top_srcdir)/modules/libm/include \
              -iquote $(top_builddir)/modules/libc/include \
              -iquote $(top_srcdir)/modules/libc/include \
              -iquote $(top_builddir)/modules/tm/include \
              -iquote $(top_srcdir)/modules/tm/include \
              -iquote $(top_builddir)/include \
              -iquote $(top_srcdir)/include \
              -include config.h
