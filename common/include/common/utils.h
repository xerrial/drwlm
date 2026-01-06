//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2025-2026, The DRWLM contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef DRWLM_COMMON_UTILS_H
#define DRWLM_COMMON_UTILS_H

#pragma once

#include <stdlib.h>

#define packed __attribute__((packed))

#define loop while (true)

#define lengthof(array) (sizeof(array) / sizeof(array[0]))

#define allocate(type) calloc(1, sizeof(type))

#endif // !DRWLM_COMMON_UTILS_H
