//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2025-2026, The DRWLM contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <daemon/context.h>
#include <common/logging.h>
#include <common/utils.h>

daemon_context_t context = {
    .pidfile   = nullptr,
    .listener  = nullptr,
    .transport = nullptr,
    .engine    = nullptr
};

void daemon_context_cleanup(daemon_context_t *context)
{
    if (context == nullptr)
        return;

    engine_destroy(context->engine);
    transport_deinit(context->transport);
    ipc_close(context->listener);
    pidfile_close(context->pidfile);
}
