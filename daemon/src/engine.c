//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2025-2026, The DRWLM contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <daemon/engine.h>
#include <common/utils.h>
#include <common/logging.h>

#include <iso646.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/epoll.h>

engine_t *engine_create()
{
    engine_t *engine = calloc(1, sizeof(engine_t));
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
                     engine_callback_t callback, void *data)
{
    if (engine == nullptr or callback == nullptr or descriptor < 0)
        return false;

    struct epoll_event event;

    int rv = epoll_ctl(engine->epoll, EPOLL_CTL_ADD, descriptor, &event);
    if (rv < 0) {
        return false;
    }

    return false;
}

bool engine_start(engine_t *engine)
{
    if (engine == nullptr)
        return false;

    engine->stop = false;

    struct epoll_event event[1];

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
    }

    free(engine);
}
