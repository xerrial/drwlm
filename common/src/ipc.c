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

ipc_listener_t *ipc_create_listener(const char *path)
{
    if (path == nullptr) {
        error("Invalid arguments");
        return nullptr;
    }

    ipc_listener_t *listener = allocate(ipc_listener_t);
    if (listener == nullptr) {
        error("Failed to allocate ipc socket descriptor: %s", strerrno);
        return nullptr;
    }

    listener->socket = -1;
    listener->path   = nullptr;

    listener->path = strdup(path);
    if (listener->path == nullptr) {
        error("Failed to strdup: %s", strerrno);
        goto failure;
    }

    if (unlink(path) < 0) switch (errno) {
    case ENOENT:
        break;
    default:
        error("Failed to unlink stale socket '%s': %s", path, strerrno);
        goto failure;
    }

    listener->socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (listener->socket < 0) {
        error("Failed to create socket: %s", strerrno);
        goto failure;
    }

    sockaddr_un_t addr;
    if (not make_addr(&addr, path)) {
        error("Failed to make addr");
        goto failure;
    }

    if (bind(listener->socket, (sockaddr_t *)&addr, sizeof(addr)) < 0) {
        error("Failed to bind: %s", strerrno);
        goto failure;
    }

    if (listen(listener->socket, 1) < 0) {
        error("Failed to listen: %s", strerrno);
        goto failure;
    }

    debug("Created IPC listener: descriptor=%d", listener->socket);

    return listener;

failure:
    ipc_close(listener);
    return nullptr;
}

ipc_connection_t *ipc_create_connection(const char *path)
{
    if (path == nullptr) {
        error("Invalid arguments");
        return nullptr;
    }

    ipc_connection_t *connection = allocate(ipc_connection_t);
    if (connection == nullptr) {
        error("Failed to allocate ipc socket descriptor: %s", strerrno);
        return nullptr;
    }

    connection->socket = -1;
    connection->path   = nullptr;

    connection->socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (connection->socket < 0) {
        error("Failed to connect to '%s': %s", path, strerrno);
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
        error("Failed to connect socket: %s", strerrno);
        goto failure;
    }

    debug("Created IPC connection: descriptor=%d", connection->socket);

    return connection;

failure:
    ipc_close(connection);
    return nullptr;
}

ipc_connection_t *ipc_accept(ipc_listener_t *listener)
{
    if (listener == nullptr) {
        error("Invalid arguments");
        return nullptr;
    }

    ipc_connection_t *connection = allocate(ipc_connection_t);
    if (connection == nullptr) {
        error("Failed to allocate ipc socket descriptor: %s", strerrno);
        return nullptr;
    }

    connection->socket = -1;

    connection->socket = accept(listener->socket, nullptr, nullptr);
    if (connection->socket < 0) {
        error("Failed to accept connection: %s", strerrno);
        goto failure;
    }

    debug("IPC accepted new connection");

    return connection;

failure:
    ipc_close(connection);
    return nullptr;
}

bool ipc_send(ipc_connection_t *connection, ipc_message_t *message)
{
    if (connection == nullptr or message == nullptr) {
        error("Invalid arguments");
        return false;
    }

retry:
    int len = write(connection->socket, message, sizeof(ipc_message_t));
    if (len < 0) switch (errno) {
    case EINTR:
        goto retry;
    default:
        error("Failed to write: %s", strerrno);
        return false;
    }

    debug("Sent message over IPC");

    return true;
}

int ipc_receive(ipc_connection_t *connection, ipc_message_t *message)
{
    if (connection == nullptr or message == nullptr) {
        error("Invalid arguments");
        return -1;
    }

retry:
    int len = read(connection->socket, message, sizeof(ipc_message_t));
    if (len < 0) switch (errno)
    {
    case EINTR:
        goto retry;
    default:
        error("Failed to read: %s", strerrno);
        return -1;
    }

    if (len == 0) {
        debug("Tried to receive message over IPC, but there was nothing to receive");
        return 0;
    }

    if (len != sizeof(ipc_message_t)) {
        error("Unexpected partial read");
        return -1;
    }

    debug("Received message over IPC");

    return len;
}

int ipc_socket_descriptor(ipc_socket_t *handle)
{
    return handle == nullptr ? -1 : handle->socket;
}

void ipc_close(ipc_socket_t *handle)
{
    if (handle == nullptr)
        return;

    debug("Closing IPC socket: descriptor=%d", handle->socket);

    if (not (handle->socket < 0)) {
        if (close(handle->socket) < 0)
            error("Failed to close socket '%s': %s", handle->path, strerrno);

        if (handle->path != nullptr and unlink(handle->path) < 0)
            error("Failed to unlink '%s': %s", handle->path, strerrno);
    }

    free((void *)handle->path);
    free(handle);
}
