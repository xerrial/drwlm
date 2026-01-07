//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2025-2026, The DRWLM contributors
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

static bool make_addr(sockaddr_un_t *addr, const char *path)
{
    memset(addr, 0, sizeof(sockaddr_un_t));
    addr->sun_family = AF_UNIX;
    // TODO: Handle errors.
    strncpy(addr->sun_path, path, sizeof(addr->sun_path) - 1);

    return true;
}

ipc_listener_t *ipc_start_listener(const char *path)
{
    if (path == nullptr)
        return nullptr;

    ipc_listener_t *listener = allocate(ipc_listener_t);
    if (listener == nullptr) {
        error("Failed to allocate ipc socket descriptor: %s", strerror(errno));
        return nullptr;
    }

    listener->socket = -1;
    listener->path   = nullptr;

    listener->path = strdup(path);
    if (listener->path == nullptr) {
        error("Failed to strdup: %s", strerror(errno));
        goto failure;
    }

    if (unlink(path) < 0) switch (errno) {
    case ENOENT:
        break;
    default:
        error("Failed to unlink stale socket '%s': %s", path, strerror(errno));
        goto failure;
    }

    listener->socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (listener->socket < 0) {
        error("Failed to create socket: %s", strerror(errno));
        goto failure;
    }

    sockaddr_un_t addr;
    if (not make_addr(&addr, path)) {
        error("Failed to make addr");
        goto failure;
    }

    if (bind(listener->socket, (sockaddr_t *)&addr, sizeof(addr)) < 0) {
        error("Failed to bind: %s", strerror(errno));
        goto failure;
    }

    if (listen(listener->socket, 1) < 0) {
        error("Failed to listen: %s", strerror(errno));
        goto failure;
    }

    return listener;

failure:
    ipc_close(listener);
    return nullptr;
}

ipc_connection_t *ipc_start_connection(const char *path)
{
    if (path == nullptr)
        return nullptr;

    ipc_connection_t *connection = allocate(ipc_connection_t);
    if (connection == nullptr) {
        error("Failed to allocate ipc socket descriptor: %s", strerror(errno));
        return nullptr;
    }

    connection->socket = -1;
    connection->path   = nullptr;

    connection->socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (connection->socket < 0) {
        error("Failed to connect to '%s': %s", path, strerror(errno));
        goto failure;
    }

    sockaddr_un_t addr;
    if (not make_addr(&addr, path)) {
        error("Failed to make addr");
        goto failure;
    }

retry:
    int rv = connect(connection->socket, (sockaddr_t *)&addr, sizeof(addr));
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

ipc_connection_t *ipc_accept(ipc_listener_t *listener)
{
    if (listener == nullptr)
        return nullptr;

    ipc_connection_t *connection = allocate(ipc_connection_t);
    if (connection == nullptr) {
        error("Failed to allocate ipc socket descriptor: %s", strerror(errno));
        return nullptr;
    }

    connection->socket = -1;

    connection->socket = accept(listener->socket, nullptr, nullptr);
    if (connection->socket < 0) {
        error("Failed to accept connection: %s", strerror(errno));
        goto failure;
    }

    return connection;

failure:
    ipc_close(connection);
    return nullptr;
}

bool ipc_send(ipc_connection_t *connection, ipc_message_t *message)
{
    if (connection == nullptr or message == nullptr)
        return false;

retry:
    int len = write(connection->socket, message, sizeof(ipc_message_t));
    if (len < 0) switch (errno) {
    case EINTR:
        goto retry;
    default:
        error("Failed to write: %s", strerror(errno));
        return false;
    }

    return true;
}

bool ipc_receive(ipc_connection_t *connection, ipc_message_t *message)
{
    if (connection == nullptr or message == nullptr)
        return false;

retry:
    int len = read(connection->socket, message, sizeof(ipc_message_t));
    if (len < 0) switch (errno)
    {
    case EINTR:
        goto retry;
    default:
        error("Failed to read: %s", strerror(errno));
        return false;
    }

    return true;
}

int ipc_socket_descriptor(ipc_socket_t *handle)
{
    return handle == nullptr ? -1 : handle->socket;
}

void ipc_close(ipc_socket_t *handle)
{
    if (handle == nullptr)
        return;

    if (not (handle->socket < 0)) {
        if (close(handle->socket) < 0)
            error("Failed to close socket '%s': %s", handle->path, strerror(errno));

        if (handle->path != nullptr and unlink(handle->path) < 0)
            error("Failed to unlink '%s': %s", handle->path, strerror(errno));
    }

    free((void *)handle->path);
    free(handle);
}
