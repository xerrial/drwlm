//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2025-2026, The DRWLM contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <common/logging.h>
#include <common/defs.h>
#include <cli/context.h>

static const char usage[] = "Usage: drwlmtool [-h]\n"
                            "  -h    show this help\n";

int main(int argc, char *argv[])
{
    int opt = -1;

    while ((opt = getopt(argc, argv, "h")) != -1) {
        switch (opt) {
        case 'h':
            fprintf(stdout, usage);
            return EXIT_SUCCESS;
        default:
            fprintf(stderr, usage);
            return EXIT_FAILURE;
        }
    }

    cli_context_t *context = cli_context_create();
    if (context == nullptr) {
        error("Failed to create context");
        goto failure;
    }

    context->connection = ipc_start_connection(socket_path);
    if (context->connection == nullptr) {
        error("Failed to connect to daemon");
        goto failure;
    }

    cli_context_destroy(context);
    return EXIT_SUCCESS;

failure:
    cli_context_destroy(context);
    return EXIT_FAILURE;
}
