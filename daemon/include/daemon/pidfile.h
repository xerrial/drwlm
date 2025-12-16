//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2025, Dmitry Kuzmenko <dimass1129@gmail.com>
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef DRWLM_DAEMON_PIDFILE_H
#define DRWLM_DAEMON_PIDFILE_H

#pragma once

int create_pidfile(const char *pidfile_path);
int write_pidfile(int pidfd);
int close_pidfile(int pidfd, const char *pidfile_path);

#endif // !DRWLM_DAEMON_PIDFILE_H
