//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2025, Dmitry Kuzmenko <dimass1129@gmail.com>
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <libutil.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <common/defs.h>

static const char usage[] = "Usage: drwlmd [-f] [-h]\n"
                            "  -f    run in foreground\n"
                            "  -h    show this help\n";

int main(int argc, char *argv[])
{
    int rv = EXIT_SUCCESS;
    int opt = -1;
    bool daemonize = true;

    while ((opt = getopt(argc, argv, "fh")) != -1)
    {
        switch (opt)
        {
        case 'f':
            daemonize = false;
            break;
        case 'h':
            // print usage
            return EXIT_SUCCESS;
        default:
            // print usage
            return EXIT_FAILURE;
        }
    }

    pid_t otherpid;
    pid_t childpid;
    struct pidfh *pfh = pidfile_open(pidfile_path, 0600, &otherpid);
    if (pfh == NULL) {
        if (errno == EEXIST) {
            err(EXIT_FAILURE, "Daemon already running, pid: %jd.", (intmax_t)otherpid);
        }
        /* If we cannot create pidfile from other reasons, only warn. */
        warn("Cannot open or create pidfile");
    }

    if (daemonize && daemon(0, 0) < 0) {
        warn("Cannot daemonize");
        rv = EXIT_FAILURE;
        goto exit;
    }

    pidfile_write(pfh);

    int clisock = socket(AF_UNIX, SOCKET_STREAM, 0);
    struct sockaddr_un addr = { .sun_family = AF_UNIX };
    strcpy(addr.sun_path, socket_path);

    unlink(socket_path);

    bind(clisock, (struct sockaddr *)&addr, sizeof(addr));
    listen(clisock, 1);

exit:
    pidfile_remove(pfh);

    return rv;
}
