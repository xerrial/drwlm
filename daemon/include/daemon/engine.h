//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2025-2026, The DRWLM contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef DRWLM_DAEMON_ENGINE_H
#define DRWLM_DAEMON_ENGINE_H

#pragma once

typedef struct {
    /**
     * @brief Engine stop flag.
     */
    bool stop;

    /**
     * @brief Epoll descriptor.
     */
    int epoll;
} engine_t;

typedef void(*engine_callback_t)(void *data);

engine_t *engine_create();
bool engine_register(engine_t *engine, int descriptor,
                     engine_callback_t callback, void *data);
bool engine_start(engine_t *engine);
bool engine_stop(engine_t *engine);
void engine_destroy(engine_t *engine);

#endif // !DRWLM_DAEMON_ENGINE_H
