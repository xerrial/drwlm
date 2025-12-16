//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2025, Dmitry Kuzmenko <dimass1129@gmail.com>
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef DRWLM_COMMON_ERROR_H
#define DRWLM_COMMON_ERROR_H

#pragma once

typedef enum
{
    OK = 0,

    ERROR_GENERIC,
    ERROR_INVALID_ARGUMENT,
    ERROR_OUT_OF_MEMORY,
    ERROR_INTERNAL,
    ERROR_ALREADY_RUNNING,

    ERROR_SYSTEM,

    __ERROR_COUNT
} error_t;

#endif // !DRWLM_COMMON_ERROR_H
