//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2025-2026, The DRWLM contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef DRWLM_CLI_CONTEXT_H
#define DRWLM_CLI_CONTEXT_H

#pragma once

#include <common/ipc.h>

/**
 * @brief DRWLM cli tool context.
 */
typedef struct {
    /**
     * @brief IPC connection with daemon.
     */
    ipc_socket_t *connection;
} cli_context_t;

cli_context_t *cli_context_create();
void cli_context_destroy(cli_context_t *context);

#endif // !DRWLM_CLI_CONTEXT_H
