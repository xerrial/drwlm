//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2026, Dmitry Kuzmenko <dimass1129@gmail.com>
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef DRWLM_COMMON_PROTOCOL_H
#define DRWLM_COMMON_PROTOCOL_H

#pragma once

#include <common/utils.h>

#include <stdint.h>

typedef uint64_t protocol_xaction_id_t;
typedef uint32_t protocol_node_id_t;
typedef uint8_t  protocol_rwlock_mode_t;

typedef enum : uint8_t {
    REQUEST  = 0x00,
    RESPONSE = 0x01
} protocol_message_type_t;

typedef enum : uint8_t {
    GRANTED = 0x00,
    DENIED  = 0x01
} protocol_reply_t;

#define PROTOCOL_HEADER \
    protocol_message_type_t type;

typedef packed_struct {
    PROTOCOL_HEADER
} protocol_message_t;

typedef packed_struct {
    PROTOCOL_HEADER
    protocol_node_id_t     sender;
    protocol_xaction_id_t  xaction;
    protocol_rwlock_mode_t mode;
} protocol_request_t;

typedef packed_struct {
    PROTOCOL_HEADER
    protocol_node_id_t    sender;
    protocol_node_id_t    addressee;
    protocol_xaction_id_t xaction;
    protocol_reply_t      reply;
} protocol_response_t;

#endif // !DRWLM_COMMON_PROTOCOL_H
