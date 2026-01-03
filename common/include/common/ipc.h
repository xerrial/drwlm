//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2026, Dmitry Kuzmenko <dimass1129@gmail.com>
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef DRWLM_COMMON_IPC_H
#define DRWLM_COMMON_IPC_H

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <common/error.h>

// TODO: Add static_assert on sizes.

constexpr size_t IPC_MAX_PAYLOAD_LENGTH = 254;

typedef struct __attribute__((packed))
{
    uint8_t type;
    uint8_t length;
    uint8_t payload[IPC_MAX_PAYLOAD_LENGTH];
} ipc_message_t;

typedef int socket_t;

error_t ipc_start_server(socket_t *handle, const char *socket_path);
error_t ipc_connect(socket_t *handle, const char *socket_path);

error_t ipc_send(const socket_t *handle, ipc_message_t *message);

error_t ipc_close(socket_t *handle);


#endif // !DRWLM_COMMON_IPC_H
