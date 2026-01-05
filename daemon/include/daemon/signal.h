//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2026, Dmitry Kuzmenko <dimass1129@gmail.com>
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef DRWLM_DAEMON_SIGNAL_H
#define DRWLM_DAEMON_SIGNAL_H

#pragma once

typedef struct {
    /**
     * @brief Write-end of selfpipe to write signals into.
     */
    int intake;

    /**
     * @brief Read-end of selfpipe to receive signals.
     */
    int outtake;
} signal_context_t;

signal_context_t *signal_handling_init();
void *signal_handling_deinit(signal_context_t *context);

#endif // !DRWLM_DAEMON_SIGNAL_H
