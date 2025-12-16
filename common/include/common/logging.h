//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2025, Dmitry Kuzmenko <dimass1129@gmail.com>
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef DRWLM_COMMON_LOGGING_H
#define DRWLM_COMMON_LOGGING_H

#pragma once

#include <errno.h>
#include <string.h>

typedef enum {
    EMERGENCY,
    ALERT,
    CRITICAL,
    ERROR,
    WARNING,
    NOTICE,
    INFO,
    DEBUG,
    TRACE
} log_severity_t;

void log(log_severity_t severity, const char *fmt, ...)
{ }

#define emergency(fmt, ...) log(EMERGENCY, fmt "\n", ##__VA_ARGS__)
#define alert(fmt, ...)     log(ALERT,     fmt "\n", ##__VA_ARGS__)
#define critical(fmt, ...)  log(CRITICAL,  fmt "\n", ##__VA_ARGS__)
#define error(fmt, ...)     log(ERROR,     fmt "\n", ##__VA_ARGS__)
#define warning(fmt, ...)   log(WARNING,   fmt "\n", ##__VA_ARGS__)
#define notice(fmt, ...)    log(NOTICE,    fmt "\n", ##__VA_ARGS__)
#define info(fmt, ...)      log(INFO,      fmt "\n", ##__VA_ARGS__)
#define debug(fmt, ...)     log(DEBUG,     fmt "\n", ##__VA_ARGS__)
#define trace(fmt, ...)     log(TRACE,     fmt "\n", ##__VA_ARGS__)

#define emergency_errno(fmt, ...) emergency(fmt ": %s", ##__VA_ARGS__, strerror(errno))
#define alert_errno(fmt, ...)     alert    (fmt ": %s", ##__VA_ARGS__, strerror(errno))
#define critical_errno(fmt, ...)  critical (fmt ": %s", ##__VA_ARGS__, strerror(errno))
#define error_errno(fmt, ...)     error    (fmt ": %s", ##__VA_ARGS__, strerror(errno))
#define warning_errno(fmt, ...)   warning  (fmt ": %s", ##__VA_ARGS__, strerror(errno))
#define notice_errno(fmt, ...)    notice   (fmt ": %s", ##__VA_ARGS__, strerror(errno))
#define info_errno(fmt, ...)      info     (fmt ": %s", ##__VA_ARGS__, strerror(errno))
#define debug_errno(fmt, ...)     debug    (fmt ": %s", ##__VA_ARGS__, strerror(errno))
#define trace_errno(fmt, ...)     trace    (fmt ": %s", ##__VA_ARGS__, strerror(errno))

#endif // !DRWLM_COMMON_LOGGING_H
