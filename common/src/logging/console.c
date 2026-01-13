//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2025-2026, The DRWLM contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <stdio.h>
#include <stdarg.h>

#include <common/logging/console.h>

static void log_console_write(log_severity_t severity,
                              const char    *fmt,
                              va_list        args)
{
    vfprintf(stderr, fmt, args);
}

log_backend_t log_console_backend = {
    .name     = "syslog",
    .handlers = {
        .initialize = nullptr,
        .write      = log_console_write,
        .finalize   = nullptr
    }
};
