#include <stdbool.h>

typedef enum
{
    NL = 0,
    RD,
    IW,
    WR,
    RWLOCK_MODES_NUM
} rwlock_mode_t;

bool modes_compatible(rwlock_mode_t current, rwlock_mode_t desired)
{
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


typedef unsigned char node_id_t;
typedef unsigned int  xa_id_t;

typedef enum {
    GRANTED = 0,
    DENIED
} reply_t;

typedef struct
{
    node_id_t       from;
    xa_id_t         xid;
    rwlock_mode_t   mode;
} request_t;

typedef struct
{
    node_id_t       to;
    node_id_t       from;
    xa_id_t         xid;
} response_t;

void on_request(request_t *req)
{
    return;
}

void on_response(response_t *resp)
{
    return;
}
