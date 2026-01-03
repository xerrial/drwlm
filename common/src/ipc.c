//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2026, Dmitry Kuzmenko <dimass1129@gmail.com>
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <common/ipc.h>
#include <common/logging.h>

#include <iso646.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

static bool make_addr(struct sockaddr_un *addr, const char *socket_path)
{
    memset(addr, 0, sizeof(struct sockaddr_un));
    addr->sun_family = AF_UNIX;
    // TODO: Handle errors.
    strncpy(addr->sun_path, socket_path, sizeof(addr->sun_path) - 1);

    return true;
}

socket_t ipc_start_server(const char *socket_path)
{
    if (socket_path == nullptr)
        return -1;

    socket_t listener = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (listener < 0) {
        error_errno("Failed to create socket");
        goto failure;
    }

    struct sockaddr_un addr;
    if (not make_addr(&addr, socket_path)) {
        error("Failed to make addr");
        goto failure;
    }

    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        error_errno("Failed to bind");
        goto failure;
    }

    if (listen(listener, 1) < 0) {
        error_errno("Failed to listen");
        goto failure;
    }

    return listener;

failure:
    close(listener);
    return -1;
}

error_t ipc_connect(socket_t *handle, const char *socket_path)
{
    if (handle == nullptr or socket_path == nullptr)
        return ERROR_INVALID_ARGUMENT;

    error_t err = OK;

    socket_t sock = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (sock < 0) {
        error_errno("Failed to create socket");
        err = ERROR_SYSTEM;
        goto cleanup;
    }

    struct sockaddr_un addr;
    if (make_addr(&addr, socket_path) != OK) {
        error("Failed to make addr");
        err = ERROR_INTERNAL;
        goto cleanup;
    }

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        error_errno("Failed to connect socket");
        err = ERROR_SYSTEM;
        goto cleanup;
    }

    *handle = sock;
    return OK;

cleanup:
    close(sock);
    return err;
}

error_t ipc_send(const socket_t *handle, ipc_message_t *message)
{
    if (handle == nullptr || message == nullptr)
        return ERROR_INVALID_ARGUMENT;

    if (write(*handle, message, sizeof(ipc_message_t)) < 0) {
        error_errno("Failed to write");
        return ERROR_SYSTEM;
    }

    return OK;
}

void ipc_close(socket_t handle)
{
    close(handle);
}
