//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2025-2026, The DRWLM contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <syslog.h>
#include <stdarg.h>

#include <common/logging/syslog.h>

static void log_syslog_initialize(const char *proc)
{
    info("Initializing syslog backend...");
    openlog(proc, LOG_CONS | LOG_NDELAY, LOG_DAEMON);
}

static void log_syslog_write(log_severity_t severity,
                             const char    *fmt,
                             va_list        args)
{
    vsyslog(severity, fmt, args);
}

static void log_syslog_finalize(void)
{
    info("Finalizing syslog backend...");
    closelog();
}

log_backend_t log_syslog_backend = {
    .name     = "syslog",
    .handlers = {
        .initialize = log_syslog_initialize,
        .write      = log_syslog_write,
        .finalize   = log_syslog_finalize
    }
};
