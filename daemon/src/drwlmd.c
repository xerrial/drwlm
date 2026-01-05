//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2025-2026, The DRWLM contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <iso646.h>
#include <libgen.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <common/defs.h>
#include <common/logging.h>
#include <common/ipc.h>
#include <daemon/pidfile.h>
#include <daemon/context.h>

static const char usage[] = "Usage: drwlmd [-f] [-h]\n"
                            "  -f    run in foreground\n"
                            "  -h    show this help\n";

int main(int argc, char *argv[])
{
    int opt = -1;
    bool daemonize = true;

    while ((opt = getopt(argc, argv, "fh")) != -1) {
        switch (opt) {
        case 'f':
            daemonize = false;
            break;
        case 'h':
            fprintf(stdout, usage);
            return EXIT_SUCCESS;
        default:
            fprintf(stderr, usage);
            return EXIT_FAILURE;
        }
    }

    log_startup(basename(argv[0]));

    daemon_context_t *context = daemon_context_create();
    if (context == nullptr) {
        error("Failed to create context");
        goto failure;
    }

    context->pidfile = pidfile_open(pidfile_path);
    if (context->pidfile == nullptr) {
        error("Failed to open pidfile");
        goto failure;
    }

    if (daemonize and daemon(0, 0) < 0) {
        error("Cannot daemonize");
        goto failure;
    }

    log_detach();

    info("Distributed Read-Write Lock Manager has started");

    if (not pidfile_write(context->pidfile)) {
        error("Failed to write pidfile");
        goto failure;
    }

    context->listener = ipc_start_listener(socket_path);
    if (context->listener == nullptr) {
        error("Failed to start server");
        goto failure;
    }

    context->corosync = corosync_init(lockspace_name);
    if (context->corosync == nullptr) {
        error("Failed to init corosync");
        goto failure;
    }

    context->engine = engine_create();
    if (context->engine == nullptr) {
        error("Failed to init engine");
        goto failure;
    }

    info("Distributed Read-Write Lock Manager exit");

    daemon_context_destroy(context);
    return EXIT_SUCCESS;

failure:
    error("Distributed Read-Write Lock Manager shutdown due to failure");

    daemon_context_destroy(context);
    return EXIT_FAILURE;
}
