//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2026, Dmitry Kuzmenko <dimass1129@gmail.com>
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <iso646.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <syslog.h>

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

    context_t *ctx = context_create();
    if (ctx == nullptr) {
        error("Failed to create context");
        goto failure;
    }

    ctx->pidfile = pidfile_open(pidfile_path);
    if (ctx->pidfile < 0) {
        error("Failed to open pidfile");
        goto failure;
    }

    if (daemonize and daemon(0, 0) < 0) {
        error("Cannot daemonize");
        goto failure;
    }

    // openlog("drwlmd", LOG_PID, LOG_DAEMON);

    info("Distributed Read-Write Lock Manager has started");

    if (not pidfile_write(ctx->pidfile)) {
        error("Failed to write pidfile");
        goto failure;
    }

    ctx->ipc_listener = ipc_start_server(socket_path);
    if (ctx->ipc_listener < 0) {
        error("Failed to start server");
        goto failure;
    }

    info("Distributed Read-Write Lock Manager exit");

    context_destroy(ctx);
    return EXIT_SUCCESS;

failure:
    // ipc_close(&daemon_sock);
    context_destroy(ctx);
    return EXIT_FAILURE;
}
