//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2025-2026, The DRWLM contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <iso646.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <common/logging.h>
#include <common/defs.h>
#include <common/lock.h>
#include <cli/context.h>

static const char usage[] = "Usage: drwlmtool [-h]\n"
                            "  -h    show this help\n";

int main(int argc, char *argv[])
{
    int opt = -1;
    rwlock_mode_t mode;

    while ((opt = getopt(argc, argv, "hm:")) != -1) {
        switch (opt) {
        case 'm':
            if (not strcmp("nl", optarg))
                mode = RWLOCK_MODE_NL;
            else if (not strcmp("rd", optarg))
                mode = RWLOCK_MODE_RD;
            else if (not strcmp("iw", optarg))
                mode = RWLOCK_MODE_IW;
            else if (not strcmp("wr", optarg))
                mode = RWLOCK_MODE_WR;
            else {
                fprintf(stderr, "Invalid mode '%s'\n", optarg);
                return EXIT_FAILURE;
            }
            continue;
        case 'h':
            fprintf(stdout, usage);
            return EXIT_SUCCESS;
        default:
            fprintf(stderr, usage);
            return EXIT_FAILURE;
        }
    }

    printf("Got mode '%s'\n", rwlock_mode_to_str(mode));

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

    ipc_message_t message = {
        .type   = REQUEST_LOCK,
        .length = sizeof(uint8_t)
    };
    memset(message.payload, 0, sizeof(message.payload));
    message.payload[0] = (uint8_t)mode;

    if (not ipc_send(context->connection, &message)) {
        error("Failed to send message to daemon");
        goto failure;
    }

    cli_context_destroy(context);
    return EXIT_SUCCESS;

failure:
    cli_context_destroy(context);
    return EXIT_FAILURE;
}
