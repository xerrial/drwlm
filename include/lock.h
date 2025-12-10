#ifndef DRWLM_LOCK_H
#define DRWLM_LOCK_H

#pragma once

#include <stdlib.h>
#include <assert.h>

typedef enum
{
    RWLOCK_MODE_NL = 0,
    RWLOCK_MODE_RD,
    RWLOCK_MODE_IW,
    RWLOCK_MODE_WR,
    RWLOCK_MODES_NUM
} rwlock_mode_t;

static inline bool mode_in_range(rwlock_mode_t mode)
{
    return mode >= RWLOCK_MODE_NL && mode < RWLOCK_MODES_NUM;
}

const char *rwlock_mode_to_str(rwlock_mode_t mode)
{
    assert(mode_in_range(mode));

    switch (mode)
    {
    case RWLOCK_MODE_NL: return "NL";
    case RWLOCK_MODE_RD: return "RD";
    case RWLOCK_MODE_IW: return "IW";
    case RWLOCK_MODE_WR: return "WR";
    }
}

bool modes_compatible(rwlock_mode_t current, rwlock_mode_t desired)
{
    assert(mode_in_range(current));
    assert(mode_in_range(desired));

    // TODO: можно небольшими изменениями в таблице реализовать
    // Read-preferring или Write-preferring политики.

    // Текущая таблица - Write-preferring.
    static const bool compatibility_table[RWLOCK_MODES_NUM][RWLOCK_MODES_NUM] =
    {
        //  Desired:
        //  NL     RD     IW     WR       Current:
        { true,  true,  true,  true }, // NL
        { true,  true,  true, false }, // RD
        { true, false,  true,  true }, // IW
        { true, false,  true, false }  // WR
    };

    return compatibility_table[current][desired];
}

typedef struct
{
    rwlock_mode_t mode;
    bool          transitioning;
    rwlock_mode_t desired_mode;
    xa_id_t       pending_xid;
    unsigned      granted_count;
} lock_t;


#endif // !DRWLM_LOCK_H
