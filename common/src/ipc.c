//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2025, Dmitry Kuzmenko <dimass1129@gmail.com>
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <common/ipc.h>
#include <common/logging.h>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

error_t ipc_connect(ipc_connection_t *connection, const char *socket_path)
{
    if (connection == nullptr || socket_path == nullptr)
        return ERROR_INVALID_ARGUMENT;
    if (connection->sock >= 0)
        return ERROR_INVALID_ARGUMENT;

    int sock = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (sock < 0) {
        error_errno("Failed to create socket");
        return ERROR_SYSTEM;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1); // TODO: Process errors.

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        error_errno("Failed to connect socket");
        return ERROR_SYSTEM;
    }

    connection->sock = sock;

    return OK;
}

error_t ipc_send(ipc_connection_t *connection, ipc_message_t *message)
{
    if (connection == nullptr || message == nullptr)
        return ERROR_INVALID_ARGUMENT;

    if (write(connection->sock, message, sizeof(ipc_message_t)) < 0) {
        error_errno("Failed to write");
        return ERROR_SYSTEM;
    }

    return OK;
}

error_t ipc_close(ipc_connection_t *connection)
{
    if (connection == nullptr)
        return ERROR_INVALID_ARGUMENT;

    close(connection->sock);
    connection->sock = -1;

    return OK;
}
