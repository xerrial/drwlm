//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2026, Dmitry Kuzmenko <dimass1129@gmail.com>
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef DRWLM_COMMON_LOGGING_H
#define DRWLM_COMMON_LOGGING_H

#pragma once

#include <errno.h>
#include <string.h>
#include <syslog.h>

typedef enum {
    EMERGENCY = LOG_EMERG,
    ALERT     = LOG_ALERT,
    CRITICAL  = LOG_CRIT,
    ERROR     = LOG_ERR,
    WARNING   = LOG_WARNING,
    NOTICE    = LOG_NOTICE,
    INFO      = LOG_INFO,
    DEBUG     = LOG_DEBUG
} log_severity_t;

void log_startup(const char *proc);
void log_set_mask(int mask);
void log_detach(void);

void __log(log_severity_t severity, const char *fmt, ...);

#define emergency(fmt, ...) __log(EMERGENCY, fmt "\n" __VA_OPT__(,) __VA_ARGS__)
#define alert(fmt, ...)     __log(ALERT,     fmt "\n" __VA_OPT__(,) __VA_ARGS__)
#define critical(fmt, ...)  __log(CRITICAL,  fmt "\n" __VA_OPT__(,) __VA_ARGS__)
#define error(fmt, ...)     __log(ERROR,     fmt "\n" __VA_OPT__(,) __VA_ARGS__)
#define warning(fmt, ...)   __log(WARNING,   fmt "\n" __VA_OPT__(,) __VA_ARGS__)
#define notice(fmt, ...)    __log(NOTICE,    fmt "\n" __VA_OPT__(,) __VA_ARGS__)
#define info(fmt, ...)      __log(INFO,      fmt "\n" __VA_OPT__(,) __VA_ARGS__)
#define debug(fmt, ...)     __log(DEBUG,     fmt "\n" __VA_OPT__(,) __VA_ARGS__)
#define trace(fmt, ...)     __log(TRACE,     fmt "\n" __VA_OPT__(,) __VA_ARGS__)

#define emergency_errno(fmt, ...) emergency(fmt ": %s" __VA_OPT__(,) __VA_ARGS__, strerror(errno))
#define alert_errno(fmt, ...)     alert    (fmt ": %s" __VA_OPT__(,) __VA_ARGS__, strerror(errno))
#define critical_errno(fmt, ...)  critical (fmt ": %s" __VA_OPT__(,) __VA_ARGS__, strerror(errno))
#define error_errno(fmt, ...)     error    (fmt ": %s" __VA_OPT__(,) __VA_ARGS__, strerror(errno))
#define warning_errno(fmt, ...)   warning  (fmt ": %s" __VA_OPT__(,) __VA_ARGS__, strerror(errno))
#define notice_errno(fmt, ...)    notice   (fmt ": %s" __VA_OPT__(,) __VA_ARGS__, strerror(errno))
#define info_errno(fmt, ...)      info     (fmt ": %s" __VA_OPT__(,) __VA_ARGS__, strerror(errno))
#define debug_errno(fmt, ...)     debug    (fmt ": %s" __VA_OPT__(,) __VA_ARGS__, strerror(errno))
#define trace_errno(fmt, ...)     trace    (fmt ": %s" __VA_OPT__(,) __VA_ARGS__, strerror(errno))

#endif // !DRWLM_COMMON_LOGGING_H
