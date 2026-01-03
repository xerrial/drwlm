//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2026, Dmitry Kuzmenko <dimass1129@gmail.com>
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef DRWLM_DAEMON_CONTEXT_H
#define DRWLM_DAEMON_CONTEXT_H

#pragma once

#include <daemon/pidfile.h>
#include <common/ipc.h>
#include <corosync/cpg.h>
#include <corosync/corotypes.h>

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
    socket_t ipc_listener;

    /**
     * @brief Corosync CPG handle.
     */
    cpg_handle_t cpg;
} context_t;


context_t *context_create();
void context_destroy(context_t *ctx);

#endif // !DRWLM_DAEMON_CONTEXT_H
