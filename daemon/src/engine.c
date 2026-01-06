//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2025-2026, The DRWLM contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <daemon/engine.h>
#include <common/utils.h>
#include <common/logging.h>
#include <common/utils.h>


#include <iso646.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/epoll.h>

typedef struct epoll_event epoll_event_t;

engine_t *engine_create()
{
    engine_t *engine = allocate(engine_t);
    if (engine == nullptr)
        return nullptr;

    engine->epoll = -1;

    engine->epoll = epoll_create1(0);
    if (engine->epoll < 0) {
        error("Failed to create epoll instace: %s", strerror(errno));
        goto failure;
    }

    return engine;

failure:
    engine_destroy(engine);
    return nullptr;
}

bool engine_register(engine_t *engine, int descriptor,
                     engine_callback_t callback, void *context)
{
    if (engine == nullptr or callback == nullptr or descriptor < 0)
        return false;

    engine_event_handler_t *handler = allocate(engine_event_handler_t);
    if (handler == nullptr)
        return false;

    handler->callback = callback;
    handler->context  = context;

    epoll_event_t event = { .events = EPOLLIN, .data.ptr = handler };

    int rv = epoll_ctl(engine->epoll, EPOLL_CTL_ADD, descriptor, &event);
    if (rv < 0) {
        goto failure;
    }

    handler->next = engine->handlers_list;
    engine->handlers_list = handler;

    return true;

failure:
    free(handler);
    return false;
}

bool engine_start(engine_t *engine)
{
    if (engine == nullptr)
        return false;

    engine->stop = false;

    epoll_event_t event[1];

    while (not engine->stop) {
        int num = epoll_wait(engine->epoll, event, lengthof(event), -1);
        if (num < 0) switch (errno) {
        case EINTR:
            continue;
        default:
            return false;
        }
    }

    return true;
}

bool engine_stop(engine_t *engine)
{
    if (engine == nullptr)
        return false;

    engine->stop = true;

    return true;
}

void engine_destroy(engine_t *engine)
{
    if (engine == nullptr)
        return;

    if (not (engine->epoll < 0)) {
        close(engine->epoll);

    while (true) {
        engine_event_handler_t *handler = engine->handlers_list;

        if (handler == nullptr)
            break;

        engine->handlers_list = handler->next;
        free(handler);
    }

    free(engine);
}
