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

static int log_mask = LOG_UPTO(LOG_DEBUG);
static bool log_is_detached = false;

void log_startup(const char *proc)
{
    openlog(proc, LOG_PID, LOG_DAEMON);
}

void log_set_mask(int mask)
{
    log_mask = mask;
    setlogmask(log_mask);
}

void log_detach(void)
{
    log_is_detached = true;
}

void __log(log_severity_t severity, const char *fmt, ...)
{
    va_list args;

    if (not log_is_detached and (log_mask & LOG_MASK(severity))) {
        va_start(args, fmt);
        vfprintf(stderr, fmt, args);
        va_end(args);
    }

    va_start(args, fmt);
    vsyslog(severity, fmt, args);
    va_end(args);

    return;
}
