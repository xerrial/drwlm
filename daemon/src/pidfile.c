//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2025, Dmitry Kuzmenko <dimass1129@gmail.com>
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <daemon/pidfile.h>

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

int create_pidfile(const char *pidfile_path)
{
    int pidfd = -1;

    if ((pidfd = open(pidfile_path, O_RDWR | O_CREAT | O_CLOEXEC, S_IWUSR)) < 0)
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
    if (fcntl(pidfd, F_SETLK, &flock_descr) < 0)
    {
        switch (errno)
        {
        case EINTR:
            goto retry;
        case EAGAIN:
        case EACCES:
            fprintf(stderr, "PID file '%s' is locked; probably daemon is already running\n", pidfile_path);
            close(pidfd);
            return -1;
        default:
            fprintf(stderr, "Failed to set lock on PID file '%s': %s", pidfile_path, strerror(errno));
            close(pidfd);
            return -1;
        }
    }

    return pidfd;
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

int close_pidfile(int pidfd, const char *pidfile_path)
{
    if (unlink(pidfile_path) < 0)
    {
        fprintf(stderr, "Failed to unlink '%s': %s\n", pidfile_path, strerror(errno));
    }

    const struct flock flock_descr = {
        .l_type   = F_UNLCK,
        .l_whence = SEEK_SET,
        .l_start  = 0,
        .l_len    = 0,
        .l_pid    = 0
    };

retry:
    if (fcntl(pidfd, F_SETLK, &flock_descr) < 0)
    {
        switch (errno)
        {
        case EINTR:
            goto retry;
        default:
            fprintf(stderr, "Failed to release lock on PID file '%s': %s\n", pidfile_path, strerror(errno));
        }
    }

    close(pidfd);

    return 0;
}
