//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2025-2026, The DRWLM contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <daemon/signal.h>
#include <common/utils.h>
#include <common/logging.h>

#include <iso646.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

static signal_handler_t signal_handler = { .intake = -1, .outtake = -1 };

static void signal_action_handler(int signal)
{
    int old_errno = errno;

    // TODO: Actual handling.

    errno = old_errno;
}

signal_handler_t *signal_handling_init()
{
    if (signal_handler.intake != -1) {
        error("Cannot init already initialized signal handling");
        return nullptr;
    }

    int pipedes[2];
    if (pipe(pipedes) < 0) {
        error("Failed to pipe: %s", strerrno);
        goto failure;
    }

    signal_handler.intake  = pipedes[1];
    signal_handler.outtake = pipedes[0];

    // TODO: Actual signal handling setup.

    return &signal_handler;

failure:
    signal_handling_deinit(&signal_handler);
    return nullptr;
}

int signal_handler_descriptor(signal_handler_t *handler)
{
    return handler == nullptr ? -1 : handler->outtake;
}

void signal_handling_deinit(signal_handler_t *handler)
{
    if (handler == nullptr)
        return;

    if (handler->intake != -1 and close(handler->intake) < 0)
        error("Failed to close: %s", strerrno);

    if (handler->outtake != -1 and close(handler->outtake) < 0)
        error("Failed to close: %s", strerrno);

    free(handler);
}

