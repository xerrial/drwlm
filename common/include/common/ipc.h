//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2025-2026, The DRWLM contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef DRWLM_COMMON_IPC_H
#define DRWLM_COMMON_IPC_H

#pragma once

#include <common/utils.h>

#include <stdint.h>
#include <stddef.h>

#define IPC_MAX_PAYLOAD_LENGTH 254

typedef packed struct {
    uint8_t type;
    uint8_t length;
    uint8_t payload[IPC_MAX_PAYLOAD_LENGTH];
} ipc_message_t;

typedef struct {
    const char *path;
    int socket;
} ipc_socket_t;

typedef ipc_socket_t ipc_listener_t;
typedef ipc_socket_t ipc_connection_t;

ipc_listener_t *ipc_start_listener(const char *path);
ipc_connection_t *ipc_start_connection(const char *path);
ipc_connection_t *ipc_accept(ipc_listener_t *listener);
bool ipc_send(ipc_connection_t *handle, ipc_message_t *message);
void ipc_close(ipc_socket_t *handle);

#endif // !DRWLM_COMMON_IPC_H
