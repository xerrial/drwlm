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
#include <common/logging/console.h>
#include <common/logging/syslog.h>
#include <common/ipc.h>
#include <daemon/pidfile.h>
#include <daemon/context.h>
#include <daemon/handlers.h>

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
    log_attach(&log_console_backend);
    log_attach(&log_syslog_backend);

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

    log_detach(&log_console_backend);

    info("Distributed Read-Write Lock Manager has started");

    if (not pidfile_write(context->pidfile)) {
        error("Failed to write pidfile");
        goto failure;
    }

    context->listener = ipc_create_listener(socket_path);
    if (context->listener == nullptr) {
        error("Failed to start server");
        goto failure;
    }

    // TODO: Commented out for debug purposes.
    // context->transport = transport_init(lockspace_name);
    // if (context->transport == nullptr) {
    //     error("Failed to init transport");
    //     goto failure;
    // }

    context->engine = engine_create();
    if (context->engine == nullptr) {
        error("Failed to init engine");
        goto failure;
    }

    if (not engine_register(context->engine,
                            ipc_socket_descriptor(context->listener),
                            incoming_ipc_connection_handler,
                            context))
    {
        error("Failed to register listener to engine");
        goto failure;
    }

    if (not engine_start(context->engine)) {
        error("Engine stopped due to failure");
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
