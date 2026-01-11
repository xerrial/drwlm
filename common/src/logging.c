//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2025-2026, The DRWLM contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <iso646.h>

#include <common/logging.h>
#include <common/logging/console.h>

static const char    *log_proc     = nullptr;
static int            log_mask     = LOG_UPTO(LOG_DEBUG);
static log_backend_t *log_backends = nullptr;

void log_startup(const char *proc)
{
    log_proc = proc;
}

void log_set_mask(int mask)
{
    log_mask = mask;
}

void log_attach(log_backend_t *backend)
{
    backend->next = log_backends;

    if (log_backends != nullptr) {
        log_backends->prev = backend;
        backend->next = log_backends;
    }

    log_backends = backend;

    if (backend->handlers.initialize != nullptr)
        backend->handlers.initialize(log_proc);
}

void log_detach(log_backend_t *backend)
{
    log_backend_t *next = backend->next;
    log_backend_t *prev = backend->prev;

    if (next != nullptr)
        next->prev = prev;

    if (prev != nullptr)
        prev->next = next;
    else
        log_backends = next;

    backend->prev = nullptr;
    backend->next = nullptr;

    if (backend->handlers.finalize != nullptr)
        backend->handlers.finalize();
}

void __log(log_severity_t severity, const char *fmt, ...)
{
    va_list args;
    log_backend_t *backend;

    if (severity & log_mask == 0)
        return;

    for (backend = log_backends; backend != nullptr; backend = backend->next) {
        va_start(args, fmt);
        backend->handlers.write(severity, fmt, args);
        va_end(args);
    }
}
