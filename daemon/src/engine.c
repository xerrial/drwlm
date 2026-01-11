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
#include <assert.h>

typedef struct epoll_event epoll_event_t;

static void engine_store_handler(engine_t *engine,
                                 engine_event_handler_t *handler)
{
    assert(engine  != nullptr);
    assert(handler != nullptr);

    handler->prev = nullptr;
    handler->next = engine->handlers_list;
    engine->handlers_list = handler;
}


static void engine_discard_handler(engine_t *engine,
                                   engine_event_handler_t *handler)
{
    assert(engine  != nullptr);
    assert(handler != nullptr);

    engine_event_handler_t *next = handler->next;
    engine_event_handler_t *prev = handler->prev;

    if (next != nullptr)
        next->prev = prev;

    if (prev != nullptr)
        prev->next = next;

    if (engine->handlers_list == handler)
        engine->handlers_list = next;

    free(handler);
}

engine_t *engine_create()
{
    engine_t *engine = allocate(engine_t);
    if (engine == nullptr) {
        error("Failed to allocate engine descriptor: %s", strerrno);
        return nullptr;
    }

    engine->epoll = -1;

    engine->epoll = epoll_create1(0);
    if (engine->epoll < 0) {
        error("Failed to create epoll instace: %s", strerrno);
        goto failure;
    }

    debug("Engine created");
    return engine;

failure:
    engine_destroy(engine);
    return nullptr;
}

bool engine_follow(engine_t *engine, int descriptor,
                   engine_callback_fn *callback, void *context)
{
    if (engine == nullptr or callback == nullptr or descriptor < 0) {
        error("Invalid arguments");
        return false;
    }

    engine_event_handler_t *handler = allocate(engine_event_handler_t);
    if (handler == nullptr) {
        error("Failed to allocate event handler: %s", strerrno);
        return false;
    }

    handler->descriptor = descriptor;
    handler->callback   = callback;
    handler->context    = context;

    epoll_event_t event = { .events = EPOLLIN | EPOLLET, .data.ptr = handler };

    int rv = epoll_ctl(engine->epoll, EPOLL_CTL_ADD, descriptor, &event);
    if (rv < 0) {
        error("epoll_ctl failed: %s", strerrno);
        goto failure;
    }

    engine_store_handler(engine, handler);
    debug("Engine registered event handler: descriptor=%d", descriptor);
    return true;

failure:
    free(handler);
    return false;
}

static bool engine_unfollow(engine_t *engine, engine_event_handler_t *handler)
{
    if (engine == nullptr or handler == nullptr)
        return false;

    int rv = epoll_ctl(engine->epoll, EPOLL_CTL_DEL, handler->descriptor, nullptr);
    if (rv < 0) {
        error("epoll_ctl failed: %s", strerrno);
        goto failure;
    }

    debug("Engine unregistered event handler: descriptor=%d", handler->descriptor);

    engine_discard_handler(engine, handler);
    return true;

failure:
    engine_discard_handler(engine, handler);
    return false;
}

bool engine_start(engine_t *engine)
{
    if (engine == nullptr) {
        error("Invalid argument");
        return false;
    }

    debug("Engine starts handling incoming events");

    engine->stop = false;

    while (not engine->stop) {
        epoll_event_t event;
        int num = epoll_wait(engine->epoll, &event, 1, -1);
        if (num < 0) switch (errno) {
        case EINTR:
            continue;
        default:
            error("epoll_wait failed: %s", strerrno);
            return false;
        }

        engine_event_handler_t *handler = event.data.ptr;

        if (event.events & EPOLLIN) {
            handler->callback(NEWDATA, handler->context);
        }
        else if (event.events & (EPOLLHUP | EPOLLERR)) {
            handler->callback(HANGUP, handler->context);
            engine_unfollow(engine, handler);
        }
    }

    debug("Engine stops handling incoming events");

    return true;
}

bool engine_stop(engine_t *engine)
{
    if (engine == nullptr)
        return false;

    engine->stop = true;

    debug("Engine is set to stop");

    return true;
}

void engine_destroy(engine_t *engine)
{
    if (engine == nullptr)
        return;

    if (not (engine->epoll < 0))
        close(engine->epoll);

    while (engine->handlers_list != nullptr)
        engine_discard_handler(engine, engine->handlers_list);

    free(engine);

    debug("Engine destroyed");
}
