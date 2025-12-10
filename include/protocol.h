#ifndef DRWLM_PROTOCOL_H
#define DRWLM_PROTOCOL_H

#pragma once

#include <stdlib.h>

#include <types.h>
#include <lock.h>

typedef enum
{
    REQUEST = 0,
    RESPONSE
} message_type_t;

typedef struct
{
    message_type_t type;
} message_header_t;

typedef enum
{
    GRANTED = 0,
    DENIED
} reply_t;

static inline bool reply_in_range(reply_t reply)
{
    return reply == GRANTED || reply == DENIED;
}

const char *reply_to_str(reply_t reply)
{
    switch (reply)
    {
    case GRANTED: return "GRANTED";
    case DENIED:  return "DENIED";
    }

    return NULL;
}

typedef struct
{
    message_header_t header;
    node_id_t        from;
    xa_id_t          xid;
    rwlock_mode_t    mode;
} request_t;

typedef struct
{
    message_header_t header;
    node_id_t        to;
    node_id_t        from;
    xa_id_t          xid;
    reply_t          reply;
} response_t;

#endif // !DRWLM_PROTOCOL_H
