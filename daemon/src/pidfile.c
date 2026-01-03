//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2026, Dmitry Kuzmenko <dimass1129@gmail.com>
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <common/logging.h>
#include <daemon/pidfile.h>

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

int create_pidfile(const char *pidfile_path)
{
    int pidfd = open(pidfile_path, O_RDWR | O_CREAT | O_CLOEXEC, S_IWUSR);
    if (pidfd < 0) {
        error_errno("Cound not open PID file '%s'", pidfile_path);
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
    if (fcntl(pidfd, F_SETLK, &flock_descr) < 0) {
        switch (errno) {
        case EINTR:
            goto retry;
        case EAGAIN:
        case EACCES:
            error("PID file '%s' is locked; probably daemon is already running", pidfile_path);
            close(pidfd);
            return -1;
        default:
            error_errno("Failed to set lock on PID file '%s'", pidfile_path);
            close(pidfd);
            return -1;
        }
    }

    return pidfd;
}

int write_pidfile(int pidfd)
{
    char buf[64];
    const size_t len = snprintf(buf, sizeof(buf), "%ld\n", (long)getpid());

    if (ftruncate(pidfd, 0) < 0) {
        error_errno("Cound not truncate PID file");
        return -1;
    }

    // TODO: process EINTR and partial writes
    if (write(pidfd, buf, len) != len) {
        error_errno("Cound not write PID file");
        return -1;
    }

    return 0;
}

int close_pidfile(int pidfd, const char *pidfile_path)
{
    if (unlink(pidfile_path) < 0) {
        error_errno("Failed to unlink PID file '%s'", pidfile_path);
    }

    const struct flock flock_descr = {
        .l_type   = F_UNLCK,
        .l_whence = SEEK_SET,
        .l_start  = 0,
        .l_len    = 0,
        .l_pid    = 0
    };

retry:
    if (fcntl(pidfd, F_SETLK, &flock_descr) < 0) {
        switch (errno) {
        case EINTR:
            goto retry;
        default:
            error_errno("Failed to release lock on PID file '%s'", pidfile_path);
        }
    }

    close(pidfd);

    return 0;
}
