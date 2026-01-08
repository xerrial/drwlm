//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2025-2026, The DRWLM contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef DRWLM_DAEMON_HANDLERS_H
#define DRWLM_DAEMON_HANDLERS_H

#pragma once

#include <daemon/engine.h>

void incoming_ipc_message_handler(engine_event_type_t type, void *context);
void incoming_ipc_connection_handler(engine_event_type_t type, void *context);

#endif // !DRWLM_DAEMON_HANDLERS_H
