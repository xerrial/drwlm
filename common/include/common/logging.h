//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2025-2026, The DRWLM contributors
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

#endif // !DRWLM_COMMON_LOGGING_H
