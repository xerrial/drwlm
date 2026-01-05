//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2025-2026, The DRWLM contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef DRWLM_DAEMON_CONTEXT_H
#define DRWLM_DAEMON_CONTEXT_H

#pragma once

#include <daemon/pidfile.h>
#include <common/ipc.h>
#include <daemon/corosync.h>

/**
 * @brief DRWLM daemon context.
 */
typedef struct {
    /**
     * @brief Pidfile handle.
     */
    pidfile_t *pidfile;

    /**
     * @brief IPC listener socket.
     */
    ipc_socket_t *listener;

    /**
     * @brief Corosync context.
     */
    corosync_context_t *corosync;
} daemon_context_t;


daemon_context_t *daemon_context_create();
void daemon_context_destroy(daemon_context_t *context);

#endif // !DRWLM_DAEMON_CONTEXT_H
