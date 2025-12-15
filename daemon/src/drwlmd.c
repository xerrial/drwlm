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
// #include <bsd/libutil.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>

#include <common/defs.h>

static const char usage[] = "Usage: drwlmd [-f] [-h]\n"
                            "  -f    run in foreground\n"
                            "  -h    show this help\n";

int create_pidfile(const char *pidfile_path)
{
    int fd = -1;
    int rv = -1;

    if ((fd = open(pidfile_path, O_RDWR | O_CREAT | O_CLOEXEC, S_IWUSR)) < 0)
    {
        fprintf(stderr, "Cound not open PID file '%s': %s\n", pidfile_path, strerror(errno));
        return -1;
    }

    const struct flock flock_descr = {
        .l_type   = F_WRLCK,
        .l_whence = SEEK_SET,
        .l_start  = 0,
        .l_len    = 0,
        .l_pid    = 0
    };

retry:
    if ((rv = fcntl(fd, F_SETLKW, &flock_descr)) < 0)
    {
        switch (errno)
        {
        case EINTR:
            goto retry;
        case EAGAIN: // fallthrough
        case EACCES:
            fprintf(stderr, "PID file '%s' is locked; probably daemon is already running\n", pidfile_path);
            return -1;
        default:
            fprintf(stderr, "Failed to set lock on PID file '%s': %s", pidfile_path, strerror(errno));
            return -1;
        }
    }

    return 0;
}

int write_pidfile(int pidfd)
{
    char buf[128];
    memset(buf, sizeof(buf), 0);
    // TODO: process errors for a good measure?
    // Failure is extremely unlikely, since INT_MAX string
    // is gonna fit into 128 bytes anyways, and usually
    // maximum PID is even less than INT_MAX.
    snprintf(buf, sizeof(buf), "%ld\n", (long)getpid());
    const size_t len = strlen(buf);

    if (ftruncate(pidfd, 0) < 0)
    {
        fprintf(stderr, "Cound not truncate PID file '%s': %s\n", strerror(errno));
        return -1;
    }

    // TODO: process EINTR and partial writes
    if (write(pidfd, buf, len) != len)
    {
        fprintf(stderr, "Cound not write PID file '%s': %s\n", strerror(errno));
        return -1;
    }

    return 0;
}

int close_pidfile(int pidfd)
{
    // Do unlink

    // Remove lock

    // Close file

    return 0;
}

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
            fprintf(stdout, usage);
            return EXIT_SUCCESS;
        default:
            fprintf(stderr, usage);
            return EXIT_FAILURE;
        }
    }

    if (daemonize && daemon(0, 0) < 0) {
        fprintf(stderr, "Cannot daemonize");
        rv = EXIT_FAILURE;
        goto exit;
    }

    openlog("drwlmd", LOG_PID, LOG_DAEMON);

    syslog(LOG_INFO, "Distributed Read-Write Lock Manager has started");

    // int clisock = socket(AF_UNIX, SOCKET_STREAM, 0);
    // struct sockaddr_un addr = { .sun_family = AF_UNIX };
    // strcpy(addr.sun_path, socket_path);

    // unlink(socket_path);

    // bind(clisock, (struct sockaddr *)&addr, sizeof(addr));
    // listen(clisock, 1);

    syslog(LOG_INFO, "Distributed Read-Write Lock Manager exit");

exit:

    return rv;
}
