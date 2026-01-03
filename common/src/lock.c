//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2026, Dmitry Kuzmenko <dimass1129@gmail.com>
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <common/lock.h>

const char *rwlock_mode_to_str(rwlock_mode_t mode)
{
    switch (mode) {
    case RWLOCK_MODE_NL: return "NL";
    case RWLOCK_MODE_RD: return "RD";
    case RWLOCK_MODE_IW: return "IW";
    case RWLOCK_MODE_WR: return "WR";
    default:
        return nullptr;
    }
}

bool modes_compatible(rwlock_mode_t current, rwlock_mode_t desired)
{
    // TODO: можно небольшими изменениями в таблице реализовать
    // Read-preferring или Write-preferring политики.
    // Текущая таблица - Write-preferring.
    static const bool
    compatibility_table[RWLOCK_MODES_NUM][RWLOCK_MODES_NUM] = {
        //  Desired:
        //  NL     RD     IW     WR       Current:
        { true,  true,  true,  true }, // NL
        { true,  true,  true, false }, // RD
        { true, false,  true,  true }, // IW
        { true, false,  true, false }  // WR
    };

    return compatibility_table[current][desired];
}
