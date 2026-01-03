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

void context_destroy(context_t *context)
{
    if (context == nullptr)
        return;

    corosync_deinit(context->corosync);
    ipc_close(context->ipc_listener);
    pidfile_close(context->pidfile);
    free(context);
}

