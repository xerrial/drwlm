//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2025-2026, The DRWLM contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <daemon/handlers.h>
#include <daemon/context.h>
#include <common/ipc.h>
#include <common/logging.h>

#include <iso646.h>

void incoming_ipc_message_handler(engine_event_type_t type, void *context)
{
    debug("Incoming IPC message event occured");

    ipc_connection_t *connection = context;

    if (type == NEWDATA) {
        ipc_message_t message;

        int len = ipc_receive(connection, &message);
        if (len < 0) {
            error("Failed to receive message; closing connection");
            ipc_close(connection);
            return;
        } else if (len == 0) {
            return;
        }

        // TODO: Actual message handling.
        debug("Received message: type=%u, length=%u", message.type, message.length);

        return;
    }
    else if (type == HANGUP)
    {
        ipc_close(connection);
        return;
    }
}

void incoming_ipc_connection_handler(engine_event_type_t type, void *context)
{
    debug("Incoming IPC connection event occured");

    ipc_listener_t *listener = ((daemon_context_t *)context)->listener;
    engine_t       *engine   = ((daemon_context_t *)context)->engine;

    if (type == NEWDATA) {
        ipc_connection_t *connection = ipc_accept(listener);
        if (connection == nullptr) {
            error("Failed to handle new ipc connection");
            return;
        }

        bool rv = engine_follow(engine, ipc_socket_descriptor(connection),
                                incoming_ipc_message_handler, connection);
        if (not rv) {
            error("Failed to register handler");
            ipc_close(connection);
            return;
        }

        debug("Successfully accepted new IPC connection");

        return;
    }
    else if (type == HANGUP) {
        critical("Unexpected listener hangup");
        engine_stop(engine);
        return;
    }
}
