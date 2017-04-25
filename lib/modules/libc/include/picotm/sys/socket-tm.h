/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <picotm/compiler.h>
#include <sys/socket.h>

PICOTM_NOTHROW
int
accept_tm(int socket, struct sockaddr* address, socklen_t* address_len);

PICOTM_NOTHROW
int
bind_tm(int socket, const struct sockaddr* address, socklen_t address_len);

PICOTM_NOTHROW
int
connect_tm(int socket, const struct sockaddr* address, socklen_t address_len);

PICOTM_NOTHROW
ssize_t
send_tm(int socket, const void* buffer, size_t length, int flags);

PICOTM_NOTHROW
ssize_t
recv_tm(int socket, void* buffer, size_t length, int flags);