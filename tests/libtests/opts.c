/*
 * picotm - A system-level transaction manager
 * Copyright (c) 2017   Thomas Zimmermann <contact@tzimmermann.org>
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

#include "opts.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ptr.h"
#include "taputils.h"

enum boundary_type  g_btype = CYCLE_BOUND;
enum loop_mode      g_loop = INNER_LOOP;
unsigned int        g_off = 0;
unsigned int        g_num = 0;
unsigned int        g_cycles = 10;
unsigned int        g_nthreads = 1;
unsigned int        g_normalize = 0;
size_t              g_txcycles = 1;

static enum parse_opts_result
opt_btype(const char* optarg)
{
    if (!strcmp("cycles", optarg)) {
        g_btype = CYCLE_BOUND;
    } else if (!strcmp("time", optarg)) {
        g_btype = TIME_BOUND;
    } else {
        return PARSE_OPTS_ERROR;
    }

    return PARSE_OPTS_OK;
}

static enum parse_opts_result
opt_cycles(const char *optarg)
{
    errno = 0;

    g_cycles = strtoul(optarg, NULL, 0);

    if (errno) {
        perror("strtoul()");
        return PARSE_OPTS_ERROR;
    }

    return PARSE_OPTS_OK;
}

static enum parse_opts_result
opt_normalize(const char* optarg)
{
    g_normalize = 1;

    return PARSE_OPTS_OK;
}

static enum parse_opts_result
opt_nthreads(const char* optarg)
{
    errno = 0;

    g_nthreads = strtoul(optarg, NULL, 0);

    if (errno) {
        perror("strtoul()");
        return PARSE_OPTS_ERROR;
    }

    return PARSE_OPTS_OK;
}

static enum parse_opts_result
opt_num(const char* optarg)
{
    errno = 0;

    g_num = strtoul(optarg, NULL, 0);

    if (errno) {
        perror("strtoul()");
        return PARSE_OPTS_ERROR;
    }

    return PARSE_OPTS_OK;
}

static enum parse_opts_result
opt_off(const char* optarg)
{
    errno = 0;

    g_off = strtoul(optarg, NULL, 0);

    if (errno) {
        perror("strtoul()");
        return PARSE_OPTS_ERROR;
    }

    return PARSE_OPTS_OK;
}

static enum parse_opts_result
opt_loop(const char* optarg)
{
    if (!strcmp("inner", optarg)) {
        g_loop = INNER_LOOP;
    } else if (!strcmp("time", optarg)) {
        g_loop = OUTER_LOOP;
    } else {
        return PARSE_OPTS_ERROR;
    }

    return PARSE_OPTS_OK;
}

static enum parse_opts_result
opt_tx_cycles(const char* optarg)
{
    errno = 0;

    g_txcycles = strtoul(optarg, NULL, 0);

    if (errno) {
        perror("strtoul()");
        return PARSE_OPTS_ERROR;
    }

    return PARSE_OPTS_OK;
}

static enum parse_opts_result
opt_verbose(const char* optarg)
{
    errno = 0;

    g_tap_verbosity = strtoul(optarg, NULL, 0);

    if (errno) {
        perror("strtoul()");
        return PARSE_OPTS_ERROR;
    }

    return PARSE_OPTS_OK;
}

enum parse_opts_result (*parse_opt[256])(const char*) = {
    ['I'] = opt_tx_cycles,
    ['L'] = opt_loop,
    ['N'] = opt_normalize,
    ['b'] = opt_btype,
    ['c'] = opt_cycles,
    ['n'] = opt_num,
    ['o'] = opt_off,
    ['t'] = opt_nthreads,
    ['v'] = opt_verbose
};

enum parse_opts_result
parse_opts(int argc, char* argv[], const char* optstring)
{
    if (argc < 2) {
        printf("enter `picotm-test -h` for a list of command-line options\n");
        return PARSE_OPTS_ERROR;
    }

    int c;

    while ((c = getopt(argc, argv, "I:L:NR:Vb:c:hm:n:o:t:v:")) != -1) {
        if ((c ==  '?') || (c == ':')) {
            return PARSE_OPTS_ERROR;
        }
        if (c >= (ssize_t)arraylen(parse_opt) || !parse_opt[c]) {
            return PARSE_OPTS_ERROR;
        }
        enum parse_opts_result res = parse_opt[c](optarg);
        if (res) {
            return res;
        }
    }

    return PARSE_OPTS_OK;
}
