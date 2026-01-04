//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2026, Dmitry Kuzmenko <dimass1129@gmail.com>
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <common/logging.h>
#include <daemon/pidfile.h>

#include <iso646.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

pidfile_t *pidfile_open(const char *pidfile_path)
{
    if (pidfile_path == nullptr)
        return nullptr;

    pidfile_t *pidfile = calloc(1, sizeof(pidfile_t));
    if (pidfile == nullptr) {
        error("Failed to allocate pidfile descriptor: %s", strerror(errno));
        return nullptr;
    }

    pidfile->fd   = -1;
    pidfile->path = nullptr;

    pidfile->path = strdup(pidfile_path);
    if (pidfile->path == nullptr) {
        error("Failed to strdup: %s", strerror(errno));
        pidfile_close(pidfile);
        return nullptr;
    }

    pidfile->fd = open(pidfile_path, O_RDWR | O_CREAT | O_CLOEXEC, S_IWUSR);
    if (pidfile->fd < 0) {
        error("Could not open PID file '%s': %s", pidfile_path, strerror(errno));
        pidfile_close(pidfile);
        return nullptr;
    }

    const struct flock flock_descr = {
        .l_type   = F_WRLCK,
        .l_whence = SEEK_SET,
        .l_start  = 0,
        .l_len    = 0,
        .l_pid    = 0
    };

retry:
    if (fcntl(pidfile->fd, F_SETLK, &flock_descr) < 0) switch (errno) {
    case EINTR:
        goto retry;
    case EAGAIN:
    case EACCES:
        error("PID file '%s' is locked; probably daemon is already running", pidfile_path);
        pidfile_close(pidfile);
        return nullptr;
    default:
        error("Failed to set lock on PID file '%s': %s", pidfile_path, strerror(errno));
        pidfile_close(pidfile);
        return nullptr;
    }

    return pidfile;
}

bool pidfile_write(pidfile_t *pidfile)
{
    if (pidfile == nullptr)
        return false;

    char buf[64];
    const size_t len = snprintf(buf, sizeof(buf), "%ld\n", (long)getpid());

    if (ftruncate(pidfile->fd, 0) < 0) {
        error("Cound not truncate PID file: %s", strerror(errno));
        return false;
    }

    // TODO: Handle partial writes.
retry:
    if (write(pidfile->fd, buf, len) != len) switch (errno) {
    case EINTR:
        goto retry;
    default:
        error("Cound not write PID file: %s", strerror(errno));
        return false;
    }

    return true;
}

void pidfile_close(pidfile_t *pidfile)
{
    if (pidfile == nullptr)
        return;

    if (not (pidfile->fd < 0)) {
        if (unlink(pidfile->path) < 0)
            error("Failed to unlink PID file '%s': %s", pidfile->path, strerror(errno));

        const struct flock flock_descr = {
            .l_type   = F_UNLCK,
            .l_whence = SEEK_SET,
            .l_start  = 0,
            .l_len    = 0,
            .l_pid    = 0
        };

    retry:
        if (fcntl(pidfile->fd, F_SETLK, &flock_descr) < 0) switch (errno) {
        case EINTR:
            goto retry;
        default:
            error("Failed to release lock on PID file '%s': %s", pidfile->path, strerror(errno));
        }

        close(pidfile->fd);
    }

    free((void *)pidfile->path);
    free(pidfile);
}
