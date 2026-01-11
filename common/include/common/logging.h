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

typedef struct log_backend     log_backend_t;
typedef struct log_backend_ops log_backend_ops_t;

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

typedef void log_initialize_fn(const char *proc);
typedef void log_write_fn(log_severity_t severity, const char *fmt, ...);
typedef void log_finalize_fn(void);

struct log_backend_ops
{
    log_initialize_fn *initialize;
    log_write_fn      *write;
    log_finalize_fn   *finalize;
};

struct log_backend {
    const char *name;
    log_backend_ops_t handlers;
    log_backend_t *prev;
    log_backend_t *next;
};

void log_startup(const char *proc);
void log_set_mask(int mask);
void log_attach(log_backend_t *backend);
void log_detach(log_backend_t *backend);

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
