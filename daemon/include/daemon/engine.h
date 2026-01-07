//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2025-2026, The DRWLM contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef DRWLM_DAEMON_ENGINE_H
#define DRWLM_DAEMON_ENGINE_H

#pragma once

typedef enum {
    NEWDATA,
    HANGUP
} engine_event_type_t;

typedef void (*engine_callback_t)(engine_event_type_t type, void *context);

typedef struct engine_event_handler {
    /**
     * @brief Monitored descriptor.
     */
    int descriptor;

    /**
     * @brief Function to be called on event.
     */
    engine_callback_t callback;

    /**
     * @brief Context to pass into the callback.
     */
    void *context;

    /**
     * @brief Next event descriptor in a list.
     */
    struct engine_event_handler *next;

    /**
     * @brief Previous event descriptor in a list.
     */
    struct engine_event_handler *prev;
} engine_event_handler_t;

typedef struct {
    /**
     * @brief Engine stop flag.
     */
    bool stop;

    /**
     * @brief Epoll descriptor.
     */
    int epoll;

    /**
     * @brief Event descriptors list.
     */
    engine_event_handler_t *handlers_list;
} engine_t;

engine_t *engine_create();
bool engine_register(engine_t *engine, int descriptor,
                     engine_callback_t callback, void *context);
bool engine_start(engine_t *engine);
bool engine_stop(engine_t *engine);
void engine_destroy(engine_t *engine);

#endif // !DRWLM_DAEMON_ENGINE_H
