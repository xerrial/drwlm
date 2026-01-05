//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2025-2026, The DRWLM contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <cli/context.h>
#include <common/logging.h>

#include <stdlib.h>

cli_context_t *cli_context_create()
{
    cli_context_t *context = calloc(1, sizeof(cli_context_t));
    if (context == nullptr)
        error("Failed to allocate context: %s", strerror(errno));

    return context;
}

void cli_context_destroy(cli_context_t *context)
{
    if (context == nullptr)
        return;

    ipc_close(context->connection);
    free(context);
}
