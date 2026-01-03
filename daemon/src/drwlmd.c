//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2026, Dmitry Kuzmenko <dimass1129@gmail.com>
//
// SPDX-License-Identifier: BSD-3-Clause
//
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

static const char usage[] = "Usage: drwlmd [-f] [-h]\n"
                            "  -f    run in foreground\n"
                            "  -h    show this help\n";

int main(int argc, char *argv[])
{
    int rv = EXIT_SUCCESS;
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

    int pidfd = create_pidfile(pidfile_path);
    if (pidfd < 0) {
        error("Failed to create pidfile");
        return EXIT_FAILURE;
    }

    if (daemonize && daemon(0, 0) < 0) {
        error("Cannot daemonize");
        rv = EXIT_FAILURE;
        goto exit;
    }

    // openlog("drwlmd", LOG_PID, LOG_DAEMON);

    info("Distributed Read-Write Lock Manager has started");

    if (write_pidfile(pidfd) < 0) {
        error("Failed to write pidfile");
        rv = EXIT_FAILURE;
        goto exit;
    }

    socket_t daemon_sock;
    if (ipc_start_server(&daemon_sock, socket_path) != OK) {
        error("Failed to start server");
        rv = EXIT_FAILURE;
        goto exit;
    }

    info("Distributed Read-Write Lock Manager exit");

exit:
    ipc_close(&daemon_sock);
    close_pidfile(pidfd, pidfile_path);

    return rv;
}
