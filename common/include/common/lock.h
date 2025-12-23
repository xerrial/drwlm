//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2025, Dmitry Kuzmenko <dimass1129@gmail.com>
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef DRWLM_COMMON_LOCK_H
#define DRWLM_COMMON_LOCK_H

#pragma once

typedef enum
{
    RWLOCK_MODE_NL = 0,
    RWLOCK_MODE_RD,
    RWLOCK_MODE_IW,
    RWLOCK_MODE_WR,
    RWLOCK_MODES_NUM
} rwlock_mode_t;

const char *rwlock_mode_to_str(rwlock_mode_t mode);
bool modes_compatible(rwlock_mode_t current, rwlock_mode_t desired);

#endif // !DRWLM_COMMON_LOCK_H
