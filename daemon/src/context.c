//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2026, Dmitry Kuzmenko <dimass1129@gmail.com>
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <daemon/context.h>
#include <daemon/pidfile.h>
#include <common/ipc.h>

#include <stdlib.h>

context_t *context_create()
{
    return calloc(1, sizeof(context_t));
}

void context_destroy(context_t *ctx)
{
    if (ctx == nullptr)
        return;

    ipc_close(ctx->ipc_listener);
    pidfile_close(ctx->pidfile);
    free(ctx);
}

