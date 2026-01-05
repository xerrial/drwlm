//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2025-2026, The DRWLM contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef DRWLM_DAEMON_PIDFILE_H
#define DRWLM_DAEMON_PIDFILE_H

#pragma once

typedef struct {
    int fd;
    const char *path;
} pidfile_t;

pidfile_t *pidfile_open(const char *path);
bool pidfile_write(pidfile_t *pidfile);
void pidfile_close(pidfile_t *pidfile);

#endif // !DRWLM_DAEMON_PIDFILE_H
