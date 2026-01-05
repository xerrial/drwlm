//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2026, Dmitry Kuzmenko <dimass1129@gmail.com>
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <daemon/signal.h>
#include <logging.h>

#include <iso646.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

static int signal_intake = -1;

static void signal_action_handler(int signal)
{
    int old_errno = errno;

    errno = old_errno;
}

signal_context_t *signal_handling_init()
{
    signal_context_t *context = calloc(1, sizeof(signal_context_t));
    if (context == nullptr)
        return nullptr;

    context->intake  = -1;
    context->outtake = -1;

    int pipedes[2];
    if (pipe(pipedes) < 0) {
        error("Failed to pipe: %s", strerror(errno));
        goto failure;
    }

    signal_intake    = pipedes[1];
    context->intake  = pipedes[1];
    context->outtake = pipedes[0];

    // TODO: Actual signal handling setup.

    return context;

failure:
    signal_handling_deinit(context);
    return nullptr;
}

void *signal_handling_deinit(signal_context_t *context)
{
    if (context == nullptr)
        return;

    signal_intake = -1;

    if (context->intake != -1 and close(context->intake) < 0)
        error("Failed to close: %s", strerror(errno));

    if (context->outtake != -1 and close(context->outtake) < 0)
        error("Failed to close: %s", strerror(errno));

    free(context);
}

