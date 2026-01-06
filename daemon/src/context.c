//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2025-2026, The DRWLM contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <daemon/context.h>
#include <common/logging.h>

#include <stdlib.h>

daemon_context_t *daemon_context_create()
{
    daemon_context_t *context = calloc(1, sizeof(daemon_context_t));
    if (context == nullptr)
        error("Failed to allocate context: %s", strerror(errno));

    return context;
}

void daemon_context_destroy(daemon_context_t *context)
{
    if (context == nullptr)
        return;

    engine_destroy(context->engine);
    transport_deinit(context->transport);
    ipc_close(context->listener);
    pidfile_close(context->pidfile);
    free(context);
}

