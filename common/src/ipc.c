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

typedef struct sockaddr_un sockaddr_un_t;
typedef struct sockaddr    sockaddr_t;

static bool make_addr(sockaddr_un_t *addr, const char *socket_path)
{
    memset(addr, 0, sizeof(sockaddr_un_t));
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

socket_t ipc_connect(const char *socket_path)
{
    if (socket_path == nullptr)
        return -1;

    socket_t handle = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (handle < 0) {
        error_errno("Failed to create socket");
        goto failure;
    }

    sockaddr_un_t addr;
    if (not make_addr(&addr, socket_path)) {
        error("Failed to make addr");
        goto failure;
    }

retry:
    if (connect(handle, (sockaddr_t *)&addr, sizeof(addr)) < 0) switch (errno) {
    case EINTR:
        goto retry;
    default:
        error_errno("Failed to connect socket");
        goto failure;
    }

    return handle;

failure:
    close(handle);
    return -1;
}

bool ipc_send(socket_t handle, ipc_message_t *message)
{
    if (message == nullptr)
        return false;

retry:
    if (write(handle, message, sizeof(ipc_message_t)) < 0) switch (errno) {
    case EINTR:
        goto retry;
    default:
        error_errno("Failed to write");
        return false;
    }

    return true;
}

void ipc_close(socket_t handle)
{
    close(handle);
}
