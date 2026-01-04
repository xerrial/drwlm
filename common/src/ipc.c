//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2026, Dmitry Kuzmenko <dimass1129@gmail.com>
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <common/ipc.h>
#include <common/logging.h>

#include <iso646.h>
#include <stdlib.h>
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

ipc_socket_t *ipc_start_server(const char *socket_path)
{
    if (socket_path == nullptr)
        return nullptr;

    ipc_socket_t *listener = calloc(1, sizeof(ipc_socket_t));
    if (listener == nullptr) {
        error("Failed to allocate ipc socket descriptor: %s", strerror(errno));
        return nullptr;
    }

    listener->sock = -1;
    listener->addr = nullptr;

    listener->addr = strdup(socket_path);
    if (listener->addr == nullptr) {
        error("Failed to strdup: %s", strerror(errno));
        goto failure;
    }

    if (unlink(socket_path) < 0) switch (errno) {
    case ENOENT:
        break;
    default:
        error("Failed to unlink stale socket '%s': %s", socket_path, strerror(errno));
        goto failure;
    }

    listener->sock = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (listener->sock < 0) {
        error("Failed to create socket: %s", strerror(errno));
        goto failure;
    }

    sockaddr_un_t addr;
    if (not make_addr(&addr, socket_path)) {
        error("Failed to make addr");
        goto failure;
    }

    if (bind(listener->sock, (sockaddr_t *)&addr, sizeof(addr)) < 0) {
        error("Failed to bind: %s", strerror(errno));
        goto failure;
    }

    if (listen(listener->sock, 1) < 0) {
        error("Failed to listen: %s", strerror(errno));
        goto failure;
    }

    return listener;

failure:
    ipc_close(listener);
    return nullptr;
}

ipc_socket_t *ipc_connect(const char *socket_path)
{
    if (socket_path == nullptr)
        return nullptr;

    ipc_socket_t *connection = calloc(1, sizeof(ipc_socket_t));
    if (connection == nullptr) {
        error("Failed to allocate ipc socket descriptor: %s", strerror(errno));
        return nullptr;
    }

    connection->sock = -1;
    connection->addr = nullptr;

    connection->sock = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (connection->sock < 0) {
        error("Failed to connect to '%s': %s", socket_path, strerror(errno));
        goto failure;
    }

    sockaddr_un_t addr;
    if (not make_addr(&addr, socket_path)) {
        error("Failed to make addr");
        goto failure;
    }

retry:
    int rv = connect(connection->sock, (sockaddr_t *)&addr, sizeof(addr));
    if (rv < 0) switch (errno) {
    case EINTR:
        goto retry;
    default:
        error("Failed to connect socket: %s", strerror(errno));
        goto failure;
    }

    return connection;

failure:
    ipc_close(connection);
    return nullptr;
}

bool ipc_send(ipc_socket_t *handle, ipc_message_t *message)
{
    if (message == nullptr)
        return false;

retry:
    int len = write(handle->sock, message, sizeof(ipc_message_t));
    if (len < 0) switch (errno) {
    case EINTR:
        goto retry;
    default:
        error("Failed to write: %s", strerror(errno));
        return false;
    }

    return true;
}

void ipc_close(ipc_socket_t *handle)
{
    if (not (handle->sock < 0)) {
        if (close(handle->sock) < 0)
            error("Failed to close socket '%s': %s", handle->addr, strerror(errno));

        if (handle->addr != nullptr and unlink(handle->addr) < 0)
            error("Failed to unlink '%s': %s", handle->addr, strerror(errno));
    }

    free((void *)handle->addr);
    free(handle);
}
