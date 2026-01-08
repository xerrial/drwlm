//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2025-2026, The DRWLM contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <common/logging.h>
#include <common/utils.h>
#include <daemon/pidfile.h>

#include <iso646.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

typedef struct flock flock_t;

pidfile_t *pidfile_open(const char *path)
{
    if (path == nullptr)
        return nullptr;

    pidfile_t *pidfile = allocate(pidfile_t);
    if (pidfile == nullptr) {
        error("Failed to allocate pidfile descriptor: %s", strerrno);
        return nullptr;
    }

    pidfile->fd   = -1;
    pidfile->path = nullptr;

    pidfile->path = strdup(path);
    if (pidfile->path == nullptr) {
        error("Failed to strdup: %s", strerrno);
        goto failure;
    }

    pidfile->fd = open(path, O_RDWR | O_CREAT | O_CLOEXEC, S_IWUSR);
    if (pidfile->fd < 0) {
        error("Could not open PID file '%s': %s", path, strerrno);
        goto failure;
    }

    const flock_t flock_descr = {
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
        error("PID file '%s' is locked; probably daemon is already running", path);
        goto failure;
    default:
        error("Failed to set lock on PID file '%s': %s", path, strerrno);
        goto failure;
    }

    return pidfile;

failure:
    pidfile_close(pidfile);
    return nullptr;
}

bool pidfile_write(pidfile_t *pidfile)
{
    if (pidfile == nullptr)
        return false;

    char buf[64];
    const size_t len = snprintf(buf, sizeof(buf), "%ld\n", (long)getpid());

    if (ftruncate(pidfile->fd, 0) < 0) {
        error("Cound not truncate PID file: %s", strerrno);
        return false;
    }

    // TODO: Handle partial writes.
retry:
    if (write(pidfile->fd, buf, len) != len) switch (errno) {
    case EINTR:
        goto retry;
    default:
        error("Cound not write PID file: %s", strerrno);
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
            error("Failed to unlink PID file '%s': %s", pidfile->path, strerrno);

        const flock_t flock_descr = {
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
            error("Failed to release lock on PID file '%s': %s", pidfile->path, strerrno);
        }

        if (close(pidfile->fd) < 0)
            error("Failed to close PID file '%s': %s", pidfile->path, strerrno);
    }

    free((void *)pidfile->path);
    free(pidfile);
}
